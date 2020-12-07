import xml.sax.handler
from nltk.corpus import stopwords
import pymongo
from settings import DB_NAME, WORD_COLLECTION, DOC_COLLECTION


class Indexer(xml.sax.handler.ContentHandler):
    def __init__(self, dbname):

        self.stop_words = stopwords.words("english")

        self.dbname = DB_NAME
        self.client = pymongo.MongoClient("localhost", 27017)
        self.db = self.client[DB_NAME]
        self.wordCollection = self.db[WORD_COLLECTION]
        self.documentCollection = self.db[DOC_COLLECTION]

        self.parser = xml.sax.make_parser()

    def parse_files(self, filepath):
        print(f"Parsing {filepath}...")
        self.parser.parse(filepath)
        print(f"{filepath} parsed!")
