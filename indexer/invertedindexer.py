import pymongo
from settings import URI, DB_NAME, WORD_COLLECTION, DOC_COLLECTION


class InvertedIndexer():
    def __init__(self):
        self.doc_count = 0
        self.uri = URI
        self.dbname = DB_NAME
        self.client = pymongo.MongoClient()
        self.db = self.client[DB_NAME]
        self.wordCollection = self.db[WORD_COLLECTION]
        self.documentCollection = self.db[DOC_COLLECTION]
        self.last_doc = ''
        self.operations = []

    def getworddoc(self, frequency=0, title=False, category=False, references=False, links=0):
        ret = {
            'docID': self.last_doc,
            'frequency': frequency,
            'title': title,
            'category': category,
            'references': references,
            'links': links
        }
        return ret

    def insertdocument(self, doc_title, num_tokens):
        curdoc = {'title': doc_title, 'tokens': num_tokens}
        self.last_doc = self.documentCollection.insert_one(curdoc)
        self.last_doc = self.last_doc.inserted_id
        self.doc_count += 1

    def executebulkops(self):
        self.wordCollection.bulk_write(self.operations, ordered=False)
        self.operations = []

    def finishwrites(self):
        if len(self.operations) > 0:
            self.executebulkops()

    def index(self, text_dict, cat_dict, ref_dict, link_dict):
        tokens = {}
        for token in text_dict:
            tokens[token] = self.getworddoc(frequency=text_dict[token])
        for token in cat_dict:
            if token not in tokens:
                tokens[token] = self.getworddoc(category=True)
            else:
                tokens[token]['category'] = True
        for token in ref_dict:
            if token not in tokens:
                tokens[token] = self.getworddoc(references=True)
            else:
                tokens[token]['references'] = True
        for token in link_dict:
            if token not in tokens:
                tokens[token] = self.getworddoc(links=link_dict[token])
            else:
                tokens[token]['links'] = link_dict[token]
        for token in tokens:
            self.operations.append(
                pymongo.UpdateOne({'word': token}, {
                                  '$push': {'docs': tokens[token]}}, upsert=True)
            )
            if len(self.operations) % 1000 == 0:
                self.executebulkops()


if __name__ == '__main__':
    indexer = InvertedIndexer()
    indexer.index({'a': 3, 'b': 2}, {'a': True}, {}, {'c': 3})
