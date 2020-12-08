import xml.sax.handler
from nltk.corpus import stopwords
import pymongo
from settings import DB_NAME, WORD_COLLECTION, DOC_COLLECTION


class Indexer(xml.sax.handler.ContentHandler):
    def __init__(self):

        self.stop_words = stopwords.words("english")

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

        self.tok_reg = r'[^A-Za-z0-9]+'
        self.infobox_reg = r"{{infobox((.|\n)*)}}\n"
        self.categories_reg = r"\[\[category:(.*)\]\]"
        self.references_reg = r"\{\{cite(.*?)\}\}<"
        self.external_reg = r"==.*external links.*=="
        self.links_reg = r"(https?://\S+)"
        self.ignore_reg = r"[0-9]+[a-z]+[0-9a-z]*"

    def parse_files(self, filepath):
        print(f"Parsing {filepath}...")
        self.parser.parse(filepath)
        print(f"{filepath} parsed!")

    def startElement(self, name, attrs):
        self.current_tag = name

    def endElement(self, name):
        if name == 'page':
            print(f"Finishing page {self.current_doc_name}")
            # print("Data: ")
            # print(self.current_doc_data[0:20])
            self.index()
            self.current_doc_data = ""
            self.current_doc_name = ""

    def characters(self, content):
        if self.current_tag == 'title':
            self.current_doc_name += content
        else:
            self.current_doc_data += content

    def index(self):
        pass


if __name__ == "__main__":
    indexer = Indexer()
    indexer.parse_files("Wikipedia-20201206200853.xml")
