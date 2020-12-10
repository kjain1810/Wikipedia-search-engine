import xml.sax.handler
from nltk.corpus import stopwords
import pymongo
import re
import Stemmer
from settings import DB_NAME, WORD_COLLECTION, DOC_COLLECTION, \
    TITLE_WEIGHT, REMOVE_STOPWORDS, STEMMING
from invertedindexer import InvertedIndexer


class Indexer(xml.sax.handler.ContentHandler):
    def __init__(self):

        self.stop_words = stopwords.words("english")
        self.stemmer = Stemmer.Stemmer('english')
        self.invertedindexer = InvertedIndexer()

        self.dbname = DB_NAME
        self.client = pymongo.MongoClient("localhost", 27017)
        self.db = self.client[DB_NAME]
        self.wordCollection = self.db[WORD_COLLECTION]
        self.documentCollection = self.db[DOC_COLLECTION]

        self.parser = xml.sax.make_parser()
        self.parser.setFeature(xml.sax.handler.feature_namespaces, 0)
        self.parser.setContentHandler(self)

        self.current_tag = ""
        self.current_doc_name = ""
        self.current_doc_data = ""

        self.tok_reg = re.compile(r'[^A-Za-z0-9]+')
        self.ignore_reg = re.compile(r"[0-9]+[a-z]+[0-9a-z]*")
        self.categories_reg = re.compile(r"\[\[Category:(.*)\]\]")
        self.references_reg = re.compile(r"\{\{cite(.*?)\}\}<")
        self.external_reg = re.compile(r"==.*external links.*==")
        self.links_reg = re.compile(r"(https?://\S+)")

    def parse_files(self, filepath):
        print(f"Parsing {filepath}...")
        self.parser.parse(filepath)
        print(f"{filepath} parsed!")

    def startElement(self, name, attrs):
        self.current_tag = name

    def endElement(self, name):
        if name == 'page':
            print(f"Finishing page {self.current_doc_name}")
            self.index()
            self.current_doc_data = ""
            self.current_doc_name = ""

    def characters(self, content):
        if self.current_tag == 'title':
            self.current_doc_name += content.lower()
        else:
            self.current_doc_data += content.lower()

    def add_special(self, tokens, dict):
        tokenized = self.tok_reg.split(tokens)
        content = []
        for token in tokenized:
            if token == '':
                continue
            if token in self.stop_words:
                continue
            content.append(self.stemmer.stemWord(token))
        content = set(content)
        for token in content:
            dict[token] = True
        return dict

    def index(self):
        # all text
        tokenized_text = self.tok_reg.split(
            self.current_doc_data + (" " + self.current_doc_name.strip()) * TITLE_WEIGHT)
        page_content = []

        for token in tokenized_text:
            if token == '':
                continue
            if REMOVE_STOPWORDS and token in self.stop_words:
                continue
            toinsert = token
            if STEMMING:
                toinsert = self.stemmer.stemWord(toinsert)
            page_content.append(toinsert)

        word_dict = {}
        num_tokens = 0

        for token in page_content:
            if token[0].isnumeric() and len(token) > 4:
                continue
            if self.ignore_reg.match(token):
                continue
            if token not in word_dict:
                word_dict[token] = 0
                num_tokens += 1
            word_dict[token] += 1

        # category text
        category_dict = {}
        cat_list = self.categories_reg.findall(self.current_doc_data)
        for cat in cat_list:
            category_dict = self.add_special(cat, category_dict)

        # reference text
        reference_dict = {}
        ref_list = self.references_reg.findall(self.current_doc_data)
        for ref in ref_list:
            reference_dict = self.add_special(ref, reference_dict)

        # link text
        links_dict = {}
        link_list = self.external_reg.split(self.current_doc_data)
        if len(link_list) > 1:
            sections = link_list[1].split("\n")
            for section in sections:
                if section and section[0] == '*':
                    links = self.links_reg.findall(section)
                    for link in links:
                        if link not in links_dict:
                            links_dict[link] = 0
                        links_dict[link] += 1

        self.invertedindexer.index(
            word_dict, category_dict, reference_dict, links_dict)


if __name__ == "__main__":
    indexer = Indexer()
    indexer.parse_files("Wikipedia-20201206200853.xml")
