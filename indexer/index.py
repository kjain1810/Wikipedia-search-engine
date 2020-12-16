from indexer import Indexer
import os

if __name__ == "__main__":
    indexer = Indexer()
    curpath = os.getcwd()

    for file in os.listdir(curpath):
        if file[0:4] == "Wiki":
            print("Found file " + file)
            indexer.parse_files(os.path.join(curpath, file))
            print("Parsed " + file)
