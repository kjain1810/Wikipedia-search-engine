#ifndef DBOPS
#define DBOPS

#include <map>
#include <vector>
#include <iostream>
#include <sqlite3.h>
#include <string.h>
#include "structures.hpp"

std::map<std::string, int> word2id;
sqlite3 *db;
char *sqliteError;

std::shared_mutex dictMutex;
std::mutex lmao;

void initializeSQLite()
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

void insertDocuments(std::vector<docs> &documents, int threadID)
{
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sqliteError);
    char inserts[] = "INSERT INTO documents VALUES (?1, ?2, ?3);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, inserts, strlen(inserts), &stmt, NULL);
    for (auto doc : documents)
    {
        sqlite3_bind_int(stmt, 1, doc.id);
        sqlite3_bind_text(stmt, 2, doc.title.c_str(), doc.title.length(), SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, doc.len);
        if (sqlite3_step(stmt) != SQLITE_DONE)
            std::cout << "Database error on doc " << doc.id << "!\n";
        sqlite3_reset(stmt);
    }
    sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, &sqliteError);
}

void insertOccurences(std::vector<occurrence> &occurences)
{
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sqliteError);
    char inserts[] = "INSERT INTO freq(docID, wordID, freq) VALUES (?1, ?2, ?3)";
    sqlite3_stmt *stmt;
    char insertWords[] = "INSERT INTO words VALUES (?1, ?2)";
    sqlite3_stmt *stmt2;
    sqlite3_prepare_v2(db, inserts, strlen(inserts), &stmt, NULL);
    sqlite3_prepare_v2(db, insertWords, strlen(insertWords), &stmt2, NULL);
    for (auto occ : occurences)
    {
        // insert only frequencies for now
        sqlite3_bind_int(stmt, 1, occ.docID);
        sqlite3_bind_int(stmt, 3, occ.frequency);
        dictMutex.lock_shared();
        if (word2id.find(occ.word) == word2id.end())
        {
            dictMutex.unlock_shared();
            dictMutex.lock();
            word2id[occ.word] = CURRENT_WORD_ID++;
            dictMutex.unlock();
            dictMutex.lock_shared();
            sqlite3_bind_int(stmt2, 1, word2id[occ.word]);
            sqlite3_bind_text(stmt2, 2, occ.word.c_str(), occ.word.length(), SQLITE_STATIC);
            if (sqlite3_step(stmt2) != SQLITE_DONE)
                std::cout << "Database error on word " << occ.word << "!\n";
            sqlite3_reset(stmt2);
        }
        sqlite3_bind_int(stmt, 2, word2id[occ.word]);
        dictMutex.unlock_shared();
        if (sqlite3_step(stmt) != SQLITE_DONE)
            std::cout << "Database error on doc " << occ.docID << " and word " << occ.word /*  << " statement " << stmt */ << "!\n";
        sqlite3_reset(stmt);
    }
    sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, &sqliteError);
}

#endif