import Stemmer


def convertText():
    stemmer = Stemmer.Stemmer("english")
    reader = open("searchtext.txt", "r")
    writer = open("stemmedtext.txt", "w")
    for line in reader.readlines():
        writer.write(stemmer.stemWord(line[:-1]) + "\n")
    reader.close()
    writer.close()


if __name__ == "__main__":
    convertText()
