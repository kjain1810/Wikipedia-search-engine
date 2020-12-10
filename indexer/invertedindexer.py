import pymongo
from settings import DB_NAME, WORD_COLLECTION, DOC_COLLECTION


class InvertedIndexer():
    def __init__(self):
        self.doc_count = 0

        self.dbname = DB_NAME
        self.client = pymongo.MongoClient("localhost", 27017)
        self.db = self.client[DB_NAME]
        self.wordCollection = self.db[WORD_COLLECTION]
        self.documentCollection = self.db[DOC_COLLECTION]

    def index(self, text_dict, cat_dict, ref_dict, link_dict):
        print("Indexing...")
        pass
