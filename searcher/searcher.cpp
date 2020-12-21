#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "cpputils/global.hpp"
#include "cpputils/utils.hpp"
#include "cpputils/searchcaller.hpp"

int main()
{
    connectToDB();
    setTotalDocuments();
    setAverageDocLength();
    std::ifstream fd("stemmedtext.txt");
    std::vector<std::string> queryWords;
    std::string word;
    while (fd >> word)
        queryWords.push_back(word);
    std::vector<std::string> topResults;
    perform_search(queryWords, topResults);
    if (topResults.size() == 0)
    {
        std::cout << "No results for the words :O\n";
        return 0;
    }
    std::cout << "Top results are: \n";
    for (int a = 0; a < NUM_RESULTS && a < topResults.size(); a++)
        std::cout << topResults[a] << "\n";
    return 0;
}