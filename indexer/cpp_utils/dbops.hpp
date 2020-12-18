#ifndef DBOPS
#define DBOPS

#include <map>
#include <vector>
#include <iostream>
#include <sqlite3.h>
#include <string.h>
#include <shared_mutex>
#include "structures.hpp"
#include "global.hpp"

std::map<std::string, int> word2id;
sqlite3 *db;
char *sqliteError;

std::shared_mutex dictMutex;

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
    char insertFreq[] = "INSERT INTO freq(docID, wordID, freq) VALUES (?1, ?2, ?3)";
    sqlite3_stmt *stmtFreq;
    char insertRefs[] = "INSERT INTO refs(docID, wordID, refs) VALUES (?1, ?2, ?3)";
    sqlite3_stmt *stmtRefs;
    char insertTitle[] = "INSERT INTO title(docID, wordID, title) VALUES (?1, ?2, ?3)";
    sqlite3_stmt *stmtTitle;
    char insertCategory[] = "INSERT INTO category(docID, wordID, cat) VALUES (?1, ?2, ?3)";
    sqlite3_stmt *stmtCategory;
    char insertWords[] = "INSERT INTO words VALUES (?1, ?2)";
    sqlite3_stmt *stmtWords;
    sqlite3_prepare_v2(db, insertFreq, strlen(insertFreq), &stmtFreq, NULL);
    sqlite3_prepare_v2(db, insertRefs, strlen(insertRefs), &stmtRefs, NULL);
    sqlite3_prepare_v2(db, insertTitle, strlen(insertTitle), &stmtTitle, NULL);
    sqlite3_prepare_v2(db, insertCategory, strlen(insertCategory), &stmtCategory, NULL);
    sqlite3_prepare_v2(db, insertWords, strlen(insertWords), &stmtWords, NULL);
    for (auto occ : occurences)
    {
        // fetch or insert word
        int here;
        dictMutex.lock_shared();
        if (word2id.find(occ.word) == word2id.end())
        {
            dictMutex.unlock_shared();
            dictMutex.lock();
            here = CURRENT_WORD_ID;
            word2id[occ.word] = CURRENT_WORD_ID++;
            dictMutex.unlock();
            sqlite3_bind_int(stmtWords, 1, word2id[occ.word]);
            sqlite3_bind_text(stmtWords, 2, occ.word.c_str(), occ.word.length(), SQLITE_STATIC);
            if (sqlite3_step(stmtWords) != SQLITE_DONE)
                std::cout << "Database error on word " << occ.word << "!\n";
            sqlite3_reset(stmtWords);
        }
        else
        {
            here = word2id[occ.word];
            dictMutex.unlock_shared();
        }
        if (occ.frequency > 0)
        {
            sqlite3_bind_int(stmtFreq, 1, occ.docID);
            sqlite3_bind_int(stmtFreq, 2, here);
            sqlite3_bind_int(stmtFreq, 3, occ.frequency);
        }
        if (occ.references)
        {
            sqlite3_bind_int(stmtRefs, 1, occ.docID);
            sqlite3_bind_int(stmtRefs, 2, here);
            sqlite3_bind_int(stmtRefs, 3, 1);
        }
        if (occ.title)
        {
            sqlite3_bind_int(stmtTitle, 1, occ.docID);
            sqlite3_bind_int(stmtTitle, 2, here);
            sqlite3_bind_int(stmtTitle, 3, 1);
        }
        if (occ.category)
        {
            sqlite3_bind_int(stmtCategory, 1, occ.docID);
            sqlite3_bind_int(stmtCategory, 2, here);
            sqlite3_bind_int(stmtCategory, 3, 1);
        }

        if (occ.frequency > 0 && sqlite3_step(stmtFreq) != SQLITE_DONE)
            std::cout << "Database error on doc " << occ.docID << " and word " << occ.word << " with freq!\n";
        else if (occ.frequency > 0)
            sqlite3_reset(stmtFreq);
        if (occ.references && sqlite3_step(stmtRefs) != SQLITE_DONE)
            std::cout << "Database error on doc " << occ.docID << " and word " << occ.word << " with refs!\n";
        else if (occ.references)
            sqlite3_reset(stmtRefs);
        if (occ.title && sqlite3_step(stmtTitle) != SQLITE_DONE)
            std::cout << "Database error on doc " << occ.docID << " and word " << occ.word << " with title!\n";
        else if (occ.title)
            sqlite3_reset(stmtTitle);
        if (occ.category && sqlite3_step(stmtCategory) != SQLITE_DONE)
            std::cout << "Database error on doc " << occ.docID << " and word " << occ.word << " with category!\n";
        else if (occ.category)
            sqlite3_reset(stmtCategory);
    }
    sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, &sqliteError);
}

#endif