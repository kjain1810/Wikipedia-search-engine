import xml.sax.handler
from nltk.corpus import stopwords
import pymongo
from settings import DB_NAME


class Indexer(xml.sax.handler.ContentHandler):
    def __init__(self, dbname):
        global stop_words
        stop_words = stopwords.words("english")

        self.dbname = DB_NAME
        self.client = pymongo.MongoClient("localhost", 27017)
        self.db = self.client[DB_NAME]

    def parse_files(self, filepath):
        print(f"Parsing {filepath}")
