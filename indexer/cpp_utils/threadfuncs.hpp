#ifndef THREADFUNCS
#define THREADFUNCS

#include <vector>
#include <pthread.h>
#include <iostream>
#include "global.hpp"
#include "fileprocess.hpp"

void *doc_func(void *args)
{
    int offset = *((int *)args);
    for (int a = offset; a < docFiles.size(); a += NUM_THREADS)
    {
        processDocFile(docFiles[a], offset);
        std::cout << "Finished document file " << a + 1 << "\n";
    }
    pthread_exit(NULL);
}

void *word_func(void *args)
{
    int offset = *((int *)args);
    for (int a = offset; a < wordFiles.size(); a += NUM_THREADS)
    {
        processWordFile(wordFiles[a]);
        std::cout << "Finished word file " << a + 1 << "\n";
    }
    pthread_exit(NULL);
}

#endif