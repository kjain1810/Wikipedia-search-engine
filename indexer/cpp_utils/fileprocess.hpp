#ifndef FILEPROCESS
#define FILEPROCESS
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <jsoncpp/json/json.h>
#include "structures.hpp"
#include "global.hpp"
#include "dbops.hpp"

std::string getstring(std::string line, char separator, int &idx, int len)
{
    std::string ret = "";
    while (idx < len && line[idx] != separator)
    {
        ret += line[idx];
        idx++;
    }
    idx++;
    return ret;
}

int getint(std::string line, char separator, int &idx, int len)
{
    int ret = 0;
    while (idx < len && line[idx] != separator)
    {
        ret *= 10;
        ret += (int)(line[idx] - '0');
        idx++;
    }
    idx++;
    return ret;
}

void processDocFile(std::string filePath)
{
    std::ifstream docFile(filePath);
    std::string line;
    int cnt = 1;
    std::vector<docs> documents;
    while (std::getline(docFile, line))
    {
        int docID = 0, tokens = 0;
        std::string title = "";
        int len = line.length();
        int idx = 0;
        docID = getint(line, ',', idx, len);
        title = getstring(line, ',', idx, len);
        tokens = getint(line, ',', idx, len);
        documents.push_back(docs(docID, title, tokens));
        cnt++;
        if (documents.size() >= MAX_INSERT_LIMIT)
        {
            insertDocuments(documents);
            documents.clear();
        }
    }
    insertDocuments(documents);
}

void processWordFile(std::string filePath)
{
}

#endif