#ifndef DBOPS
#define DBOPS
#include <vector>
#include <iostream>
#include "structures.hpp"

void insertDocuments(std::vector<docs> &documents)
{
    // insert documents to db
    std::cout << documents.size() << "\n";
}

#endif