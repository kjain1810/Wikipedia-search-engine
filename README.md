# Wikipedia Search
A fast, multi threaded indexer and search engine for Wikipedia.

## Usage

### Indexing

To index a Wikidump, place it in the folder ```indexer/``` and from the ```indexer``` folder, run the command:

```bash
bash indexing.sh
```

### Searching

To search an uploaded database of Wikidump, write the search words in ```searcher/searchtext.txt```, one word a line, and from the ```searcher``` folder, run the command:

```bash
bash searching.sh
```

## Scraper

Used to scrap pages from random categories to get a smaller subset of Wikipedia for testing

## Indexer

Indexes Wikidump files using XML Sax Handler. Data is pre processed with techniques like Stemming, removin stop words, etc. Then, it is stored in inverted index in a SQLite database.

## Searcher

Searches the SQLite database and ranks pages using Okapi BM25 method, producing the top results.

## TODO

1. Add support for searching in category, title, etc
2. Add front end