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

bool getbool(std::string line, char separator, int &idx, int len)
{
    bool ret;
    if (line[idx] == 'F')
        ret = false;
    else
        ret = true;
    while (idx < len && line[idx] != ',')
        idx++;
    idx++;
    return ret;
}

void processDocFile(std::string filePath, int threadID)
{
    std::ifstream docFile(filePath);
    std::string line;
    std::vector<docs> documents;
    while (std::getline(docFile, line))
    {
        int docID = 0, tokens = 0;
        std::string title = "";
        int len = line.length();
        int st = 0, en = len - 1;
        while (line[st] <= '9' && line[st] >= '0')
        {
            docID *= 10;
            docID += (int)(line[st] - '0');
            st++;
        }
        int mul = 1;
        while (line[en] <= '9' && line[en] >= '0')
        {
            tokens += (mul * (int)(line[en] - '0'));
            en--;
        }
        for (int a = st + 1; a < en; a++)
            title += line[a];
        documents.push_back(docs(docID, title, tokens));
        if (documents.size() >= MAX_INSERT_LIMIT)
        {
            insertDocuments(documents, threadID);
            documents.clear();
        }
    }
    insertDocuments(documents, threadID);
}

void processWordFile(std::string filePath)
{
    std::ifstream wordFile(filePath);
    std::string line;
    std::vector<occurrence> occurrences;
    while (std::getline(wordFile, line))
    {
        std::string word;
        int docID, frequency, links;
        bool title, category, references;
        int idx = 0;
        int len = line.length();
        word = getstring(line, SEPARATOR, idx, len);
        docID = getint(line, SEPARATOR, idx, len);
        frequency = getint(line, SEPARATOR, idx, len);
        title = getbool(line, SEPARATOR, idx, len);
        category = getbool(line, SEPARATOR, idx, len);
        references = getbool(line, SEPARATOR, idx, len);
        links = getint(line, SEPARATOR, idx, len);
        occurrences.push_back(occurrence(word, docID, frequency, title, category, references, links));
        if (occurrences.size() >= MAX_INSERT_LIMIT)
        {
            insertOccurences(occurrences);
            occurrences.clear();
        }
    }
    insertOccurences(occurrences);
}

#endif