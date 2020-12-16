#ifndef STRUCTURES
#define STRUCTURES
#include <string>
#include "global.hpp"

struct docs
{
    int id;
    std::string title;
    int len;
    docs(int ID, std::string Title, int Len)
    {
        id = ID;
        title = Title;
        len = Len;
    }
};

struct words
{
    std::string word;
    int wordID;
};

struct occurrence
{
    int wordID;
    int docID;
    int frequency;
    int links;
    bool references;
    bool title;
    bool category;
};

#endif