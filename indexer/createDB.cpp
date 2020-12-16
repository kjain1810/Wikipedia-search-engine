#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>
#include "./cpp_utils/structures.hpp"
#include "./cpp_utils/global.hpp"
#include "./cpp_utils/dbops.hpp"
#include "./cpp_utils/threadfuncs.hpp"

int numDocs = 0;
int docFileCnt = 0;
int numWords = 0;
int wordFileCnt = 0;

std::vector<std::string> docFiles;
std::vector<std::string> wordFiles;

int main(int argc, char *argv[])
{
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

    std::vector<std::thread> ths;
    for (int a = 0; a < NUM_THREADS; a++)
    {
        std::thread here(&doc_func, a, docFiles);
        ths.push_back(here);
    }
    for (int a = 0; a < NUM_THREADS; a++)
        ths[a].join();

    ths.clear();
    for (int a = 0; a < NUM_THREADS; a++)
    {
        std::thread here(&word_func, a, wordFiles);
        ths.push_back(here);
    }
    for (int a = 0; a < NUM_THREADS; a++)
        ths[a].join();
    return 0;
}