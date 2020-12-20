import os
from settings import DOC_FILE, WORD_FILE, FILE_DESTINATIONS, MAX_FILE_SIZE


class InvertedIndexer():
    def __init__(self):
        self.doc_count = 0
        if os.path.isdir(FILE_DESTINATIONS) == False:
            os.mkdir(FILE_DESTINATIONS)
        self.cur_docfile_cnt = 0
        self.cur_docfile_size = 0
        self.cur_docfile_name = DOC_FILE + \
            str(self.cur_docfile_cnt) + ".txt"
        self.cur_wordfile_cnt = 0
        self.cur_wordfile_size = 0
        self.cur_wordfile_name = WORD_FILE + \
            str(self.cur_wordfile_cnt) + ".txt"
        self.doc_writer = open(os.path.join(
            FILE_DESTINATIONS, self.cur_docfile_name), "w")
        self.word_writer = open(os.path.join(
            FILE_DESTINATIONS, self.cur_wordfile_name), "w")

    def getworddoc(self, doc_id, frequency=0, title=False, category=False, references=False, links=0):
        ret = {
            'docID': doc_id,
            'frequency': frequency,
            'title': title,
            'category': category,
            'references': references,
            'links': links
        }
        return ret

    def getnextdocfile(self):
        self.doc_writer.close()
        self.cur_docfile_cnt += 1
        self.cur_docfile_name = DOC_FILE + \
            str(self.cur_docfile_cnt) + ".txt"
        self.doc_writer = open(os.path.join(
            FILE_DESTINATIONS, self.cur_docfile_name), "w")
        self.cur_docfile_size = 0

    def writetodocfiles(self, curdocs):
        towrite = f"{curdocs}\n"
        self.doc_writer.write(towrite)
        self.cur_docfile_size += len(towrite)
        # if self.cur_docfile_size >= MAX_FILE_SIZE // 10:
        #     print(self.cur_docfile_size)
        if self.cur_docfile_size > MAX_FILE_SIZE:
            self.getnextdocfile()

    def insertdocument(self, doc_title, num_tokens):
        # curdoc = {'doc_id': self.doc_count,
        #           'title': doc_title, 'tokens': num_tokens}
        if num_tokens > 1000000000:
            print("too many tokens")
        if num_tokens < 0:
            print("random error")
        curdoc = str(self.doc_count) + "," + doc_title + "," + str(num_tokens)
        # write curdon into file
        self.writetodocfiles(curdoc)
        self.doc_count += 1
        return self.doc_count - 1

    def getnextwordfile(self):
        self.word_writer.close()
        self.cur_wordfile_cnt += 1
        self.cur_wordfile_name = WORD_FILE + \
            str(self.cur_wordfile_cnt) + ".txt"
        self.word_writer = open(os.path.join(
            FILE_DESTINATIONS, self.cur_wordfile_name), "w")
        self.cur_wordfile_size = 0

    def writetowordfiles(self, tokens):
        towrite = ""
        for token in tokens:
            towrite += f"{token},{tokens[token]['docID']},{tokens[token]['frequency']},{tokens[token]['title']},{tokens[token]['category']},{tokens[token]['references']},{tokens[token]['links']}\n"
        self.word_writer.write(towrite)
        self.cur_wordfile_size += len(towrite)
        if self.cur_wordfile_size > MAX_FILE_SIZE:
            self.getnextwordfile()
        # print(self.cur_wordfile_size)

    def index(self, text_dict, cat_dict, ref_dict, link_dict, doc_id):
        tokens = {}
        for token in text_dict:
            tokens[token] = self.getworddoc(doc_id, frequency=text_dict[token])
        for token in cat_dict:
            if token not in tokens:
                tokens[token] = self.getworddoc(doc_id, category=True)
            else:
                tokens[token]['category'] = True
        for token in ref_dict:
            if token not in tokens:
                tokens[token] = self.getworddoc(doc_id, references=True)
            else:
                tokens[token]['references'] = True
        # for token in link_dict:
        #     if token not in tokens:
        #         tokens[token] = self.getworddoc(doc_id, links=link_dict[token])
        #     else:
        #         tokens[token]['links'] = link_dict[token]
        # write tokens into a file
        self.writetowordfiles(tokens)


if __name__ == '__main__':
    indexer = InvertedIndexer()
    indexer.index({'a': 3, 'b': 2}, {'a': True}, {}, {'c': 3})
