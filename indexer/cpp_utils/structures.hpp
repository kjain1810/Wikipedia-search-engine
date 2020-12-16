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
    std::string word;
    int docID;
    int frequency;
    int links;
    bool references;
    bool title;
    bool category;

    occurrence(std::string w, int d, int f, bool r, bool t, bool c, int l)
    {
        word = w;
        docID = d;
        frequency = f;
        references = r;
        title = t;
        category = c;
        links = l;
    }
};

#endif