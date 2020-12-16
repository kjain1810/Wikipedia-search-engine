#ifndef GLOBALS
#define GLOBALS
#include <shared_mutex>
#include <mutex>

std::shared_mutex dictMutex;
std::mutex dbWriterMutex;

int MAX_INSERT_LIMIT = 1000;
int CURRENT_WORD_ID = 0;
int NUM_THREADS = 8;

char SEPARATOR = ',';

#endif