#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <pthread.h>
#include <time.h>
#include "./cpp_utils/structures.hpp"
#include "./cpp_utils/global.hpp"
#include "./cpp_utils/dbops.hpp"
#include "./cpp_utils/threadfuncs.hpp"

int numDocs = 0;
int docFileCnt = 0;
int numWords = 0;
int wordFileCnt = 0;

int main(int argc, char *argv[])
{
    clock_t tStart = clock();
    // get number of document files
    if (argc == 1)
    {
        std::cout << "Usage: ./run <directory1> ...\n";
        return 0;
    }
    for (const auto &entry : std::filesystem::directory_iterator(argv[1]))
    {
        std::string filePath = entry.path();
        int len = filePath.length();
        int st = len - 1;
        while (filePath[st] != '/')
            st--;
        if (filePath.substr(st + 1, 8) == "docfile_" && filePath.substr(len - 4, 4) == ".txt")
        {
            docFileCnt += 1;
            docFiles.push_back(filePath);
        }
    }
    std::cout << "Found " << docFileCnt << " doc files\n";
    for (const auto &entry : std::filesystem::directory_iterator(argv[1]))
    {
        std::string filePath = entry.path();
        int len = filePath.length();
        int st = len - 1;
        while (filePath[st] != '/')
            st--;
        if (filePath.substr(st + 1, 9) == "wordfile_" && filePath.substr(len - 4, 4) == ".txt")
        {
            wordFileCnt += 1;
            wordFiles.push_back(filePath);
        }
    }
    std::cout << "Found " << wordFileCnt << " word files\n";
    initializeSQLite();
    std::vector<pthread_t> ths;
    for (int a = 0; a < NUM_THREADS; a++)
    {
        pthread_t thread_here;
        int *arg = new int;
        *arg = a;
        pthread_create(&thread_here, NULL, doc_func, (void *)arg);
        ths.push_back(thread_here);
    }
    for (int a = 0; a < NUM_THREADS; a++)
        pthread_join(ths[a], NULL);
    std::cout << "Documents inserted!\n";
    ths.clear();
    for (int a = 0; a < NUM_THREADS; a++)
    {
        pthread_t thread_here;
        int *arg = new int;
        *arg = a;
        pthread_create(&thread_here, NULL, word_func, (void *)arg);
        ths.push_back(thread_here);
    }
    for (int a = 0; a < NUM_THREADS; a++)
        pthread_join(ths[a], NULL);
    std::cout << "Indexing over!";
    std::cout << std::fixed << std::setprecision(10) << "Indexed all files in " << (double)(clock() - tStart) / CLOCKS_PER_SEC << " seconds\n";
    return 0;
}