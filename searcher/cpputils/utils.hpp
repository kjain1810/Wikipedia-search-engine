#ifndef UTILS
#define UTILS

#include <string>
#include <iostream>

bool checkQuery(std::string query)
{
    for (int a = 0; a < query.length(); a++)
        if (query[a] == ' ')
            return false;
    if (query[0] >= '0' && query[0] <= '9' && query.length() != 4)
        return false;
    return true;
}

bool toExit()
{
    std::cout << "Do you want to exit? y/[n]";
    // char des;
    // std::cin >> des;
    std::string des;
    std::getline(std::cin, des);
    if (des[0] == 'y')
        return true;
    return false;
}

#endif