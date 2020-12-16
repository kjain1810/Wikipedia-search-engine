#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include "./cpp_utils/fileprocess.hpp"
#include "./cpp_utils/structures.hpp"
#include "./cpp_utils/global.hpp"
#include "./cpp_utils/dbops.hpp"

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

    // Testing processDocFile
    // for (auto filePath : docFiles)
    // {
    //     processDocFile(filePath);
    // }
    for (auto filePath : wordFiles)
    {
        processWordFile(filePath);
    }
    return 0;
}