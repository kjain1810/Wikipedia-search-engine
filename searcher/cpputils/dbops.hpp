#ifndef DBOPS
#define DBOPS

#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <utility>
#include <map>
#include "global.hpp"

sqlite3 *db;
char *sqliteError;

void connectToDB()
{
    int flag = sqlite3_open(DB_PATH.c_str(), &db);
    if (flag)
    {
        std::cout << "Can't find database\n";
        exit(0);
    }
    sqlite3_exec(db, "PRAGMA synchronous=OFF", NULL, NULL, &sqliteError);
    sqlite3_exec(db, "PRAGMA count_changes=OFF", NULL, NULL, &sqliteError);
    sqlite3_exec(db, "PRAGMA journal_mode=MEMORY", NULL, NULL, &sqliteError);
    sqlite3_exec(db, "PRAGMA temp_store=MEMORY", NULL, NULL, &sqliteError);
}

void setTotalDocuments()
{
    sqlite3_stmt *stmt;
    std::string statement = "SELECT COUNT(*) FROM documents";
    sqlite3_prepare_v2(db, statement.c_str(), statement.length(), &stmt, NULL);
    sqlite3_step(stmt);
    std::string docs = std::string((char *)sqlite3_column_text(stmt, 0));
    TOTAL_DOCUMENTS = 0;
    for (int a = 0; a < docs.length(); a++)
    {
        TOTAL_DOCUMENTS *= 10;
        TOTAL_DOCUMENTS += (int)(docs[a] - '0');
    }
}

void setAverageDocLength()
{
    sqlite3_stmt *stmt;
    std::string statement = "SELECT AVG(len) FROM documents";
    sqlite3_prepare_v2(db, statement.c_str(), statement.length(), &stmt, NULL);
    sqlite3_step(stmt);
    std::string avg = std::string((char *)sqlite3_column_text(stmt, 0));
    AVG_DOC_LEN = 0.0;
    int idxdot = -1;
    for (int a = 0; a < avg.length(); a++)
    {
        if (avg[a] == '.')
        {
            idxdot = a;
            break;
        }
        AVG_DOC_LEN *= 10;
        AVG_DOC_LEN += (double)((int)(avg[a] - '0'));
    }
    if (idxdot >= 0)
    {
        double div = 10;
        for (int a = idxdot + 1; a < avg.length(); a++)
        {
            AVG_DOC_LEN += (double)((int)(avg[a] - '0')) / div;
            div *= 10;
        }
    }
}

int getWordID(std::string word)
{
    sqlite3_stmt *stmt;
    std::vector<std::vector<std::string>> result;
    std::string statement = "SELECT * FROM words WHERE word=\"" + word + "\"";
    sqlite3_prepare_v2(db, statement.c_str(), statement.length(), &stmt, NULL);
    sqlite3_step(stmt);
    result.push_back(std::vector<std::string>());
    result.push_back(std::vector<std::string>());
    while (sqlite3_column_text(stmt, 0))
    {
        result[0].push_back(std::string((char *)sqlite3_column_text(stmt, 0)));
        result[1].push_back(std::string((char *)sqlite3_column_text(stmt, 1)));
        sqlite3_step(stmt);
    }
    if (result[0].size() == 0)
        return -1;
    int ret = 0;
    for (int a = 0; a < result[0][0].length(); a++)
    {
        ret *= 10;
        ret += (int)(result[0][0][a] - '0');
    }
    return ret;
}

void getDocumentFrequency(int wordID, std::vector<std::pair<int, std::pair<int, int>>> &res)
{
    sqlite3_stmt *stmt;
    std::vector<std::vector<std::string>> result;
    std::string statement = "SELECT docID, freq FROM freq WHERE wordID=" + std::to_string(wordID);
    sqlite3_prepare_v2(db, statement.c_str(), statement.length(), &stmt, NULL);
    sqlite3_step(stmt);
    std::vector<std::string> docIDs, freqs;
    while (sqlite3_column_text(stmt, 0))
    {
        docIDs.push_back(std::string((char *)sqlite3_column_text(stmt, 0)));
        freqs.push_back(std::string((char *)sqlite3_column_text(stmt, 1)));
        sqlite3_step(stmt);
    }
    for (int a = 0; a < docIDs.size(); a++)
    {
        int docID = 0, freq = 0;
        for (int b = 0; b < docIDs[a].length(); b++)
            docID = docID * 10 + (int)(docIDs[a][b] - '0');
        for (int b = 0; b < freqs[a].length(); b++)
            freq = freq * 10 + (int)(freqs[a][b] - '0');
        res.push_back({docID, {freq, 0}});
    }
}

void getDocumentLengths(std::vector<std::pair<int, std::pair<int, int>>> &docIDs)
{
    sqlite3_stmt *stmt;
    std::string statement = "SELECT id, len FROM documents WHERE id IN (";
    for (int a = 0; a < docIDs.size(); a++)
        statement += std::to_string(docIDs[a].first) + ",";
    statement[statement.length() - 1] = ')';
    sqlite3_prepare_v2(db, statement.c_str(), statement.length(), &stmt, NULL);
    sqlite3_step(stmt);
    std::map<std::string, std::string> mep;
    while (sqlite3_column_text(stmt, 0))
    {
        mep[std::string((char *)sqlite3_column_text(stmt, 0))] = std::string((char *)sqlite3_column_text(stmt, 1));
        sqlite3_step(stmt);
    }
    for (int a = 0; a < docIDs.size(); a++)
    {
        std::string here = mep[std::to_string(docIDs[a].first)];
        for (int a = 0; a < here.length(); a++)
            docIDs[a].second.second = docIDs[a].second.second * 10 + (int)(here[a] - '0');
    }
}

void getDocumentNames(std::vector<int> IDs, std::vector<std::string> &res)
{
    sqlite3_stmt *stmt;
    std::string statement = "SELECT title FROM documents WHERE id IN (";
    for (int a = 0; a < IDs.size(); a++)
        statement += std::to_string(IDs[a]) + ",";
    statement[statement.length() - 1] = ')';
    sqlite3_prepare_v2(db, statement.c_str(), statement.length(), &stmt, NULL);
    sqlite3_step(stmt);
    while (sqlite3_column_text(stmt, 0))
    {
        res.push_back(std::string((char *)sqlite3_column_text(stmt, 0)));
        sqlite3_step(stmt);
    }
}

#endif