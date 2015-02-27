//
//  LoginDb.h
//  Database_final
//
//  Created by Barun Patra on 26/02/15.
//  Copyright (c) 2015 Sample. All rights reserved.
//

#ifndef __Database_final__LoginDb__
#define __Database_final__LoginDb__

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <iostream>
#include <stdlib.h>
#define DATABASE_NAME "Parent.db"
using namespace std;
class UserLoginDb
{
private:
    sqlite3 *Database;
    int created(); //0 DB ERROR 1:CREATED 2:NOT CREATED
    char* stringToChar(const string &);
    string tableName;
    void convertToDatabaseString(string&);
    int exists(string); //0:DB Error 1:Exists 2:Does not Exist
    bool open();
public:
    int create(string); //0 DB ERROR NOT CREATED 1:CREATED
    int addUser(string,string,string); // 0 for database not connected, 1 for Username already taken, 2 for success
    int delUser(string); // 0:DB Error 1:Failure 2:Success
    int passCheck(string,string); //0:DB Error 1:Fail 2:Pass
    void close();
    UserLoginDb();
    
};
#endif /* defined(__Database_final__LoginDb__) */
