#ifndef SEARCHCALLER
#define SEARCHCALLER

#include <string>
#include <vector>
#include <pthread.h>
#include <utility>
#include <math.h>
#include "dbops.hpp"
#include "global.hpp"

struct argStruct
{
    int offset;
    std::vector<std::string> query;
};

pthread_mutex_t scoreUpdateMutex;
std::map<int, double> BM25s;

void searchWord(std::string word)
{
    int wordID = getWordID(word);
    if (wordID == -1)
        return;
    std::vector<std::pair<int, std::pair<int, int>>> docFreq;
    getDocumentFrequency(wordID, docFreq);
    int nWord = docFreq.size();
    double IDF = log(1 + (((double)TOTAL_DOCUMENTS - (double)nWord + 0.5)) / ((double)nWord + 0.5));
    getDocumentLengths(docFreq);
    std::vector<std::pair<int, double>> scores;
    for (int a = 0; a < docFreq.size(); a++)
    {
        double numerator = (double)(docFreq[a].second.first * (K1 + 1));
        double term = 1 - B + ((double)(B * docFreq[a].second.second)) / ((double)AVG_DOC_LEN);
        double denominator = (double)docFreq[a].second.first + (double)K1 * term;
        double TF = numerator / denominator;
        double score = IDF * TF;
        scores.push_back({docFreq[a].first, score});
    }
    pthread_mutex_lock(&scoreUpdateMutex);
    for (int a = 0; a < scores.size(); a++)
    {
        if (BM25s.find(scores[a].first) == BM25s.end())
            BM25s[scores[a].first] = 0;
        BM25s[scores[a].first] += scores[a].second;
    }
    pthread_mutex_unlock(&scoreUpdateMutex);
}

void *threadFunc(void *args)
{
    argStruct arg = *((argStruct *)args);
    for (int a = arg.offset; a < arg.query.size(); a += NUM_THREADS)
        searchWord(arg.query[a]);
    pthread_exit(NULL);
}

void perform_search(std::vector<std::string> query, std::vector<std::string> &topResults)
{
    BM25s.clear();
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&scoreUpdateMutex, NULL);
    for (int a = 0; a < NUM_THREADS; a++)
    {
        argStruct *arg = new argStruct();
        arg->offset = a;
        for (auto word : query)
            arg->query.push_back(word);
        pthread_create(&threads[a], NULL, threadFunc, (void *)arg);
    }
    for (int a = 0; a < NUM_THREADS; a++)
        pthread_join(threads[a], NULL);
    std::vector<int> bestIDs;
    for (int a = 0; a < NUM_RESULTS; a++)
    {
        double maxi = 0;
        int id;
        for (auto score : BM25s)
        {
            if (score.second < maxi)
                continue;
            int flg = 0;
            for (int b = 0; b < bestIDs.size(); b++)
                if (score.first == bestIDs[b])
                    flg = 1;
            if (flg)
                continue;
            maxi = score.second;
            id = score.first;
        }
        bestIDs.push_back(id);
    }
    getDocumentNames(bestIDs, topResults);
}

#endif