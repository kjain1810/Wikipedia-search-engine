#ifndef THREADFUNCS
#define THREADFUNCS

#include <map>
#include <vector>
#include "global.hpp"
#include "fileprocess.hpp"

std::map<std::string, int> word2id;

void doc_func(int offset, std::vector<std::string> &vec)
{
    for (int a = offset; a < vec.size(); a += NUM_THREADS)
        processDocFile(vec[a]);
}

void word_func(int offset, std::vector<std::string> &vec)
{
    for (int a = offset; a < vec.size(); a += NUM_THREADS)
        processWordFile(vec[a]);
}

#endif