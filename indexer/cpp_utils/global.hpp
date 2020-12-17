#ifndef GLOBALS
#define GLOBALS
#include <shared_mutex>
#include <mutex>

int MAX_INSERT_LIMIT = 10;
int CURRENT_WORD_ID = 0;
int NUM_THREADS = 8;

char SEPARATOR = ',';

std::string DB_PATH = "/home/kunal/Desktop/padhai/ml/wikipedia-search/db/wikipediaDB.db";

std::vector<std::string> docFiles;
std::vector<std::string> wordFiles;

#endif