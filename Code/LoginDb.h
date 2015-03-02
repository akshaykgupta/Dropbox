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
#include <vector>
#include <stdlib.h>
#define DATABASE_NAME "Parent.db"
using namespace std;
/*
 TODO: make column PATHSERVER
 */
class UserLoginDb
{
private:
    sqlite3 *Database;
    int created(); //0 DB ERROR 1:CREATED 2:NOT CREATED
    char* stringToChar(const string &);
    string tableName;
    void convertToDatabaseString(string&);
    int exists(string); //0:DB Error 1:Exists 2:Does not Exist
	
	// static int callCreated(void *data,int argc,char **argv,char **azColName);
// 	static int callexists(void *data,int argc,char **argv,char **azColName);
// 	static int callback(void* data,int argc,char **argv,char **azColName);
// 	static int callpassCheck(void *data,int argc,char **argv,char **azColName);
// 	static int callSearch(void* data,int argc,char** argv,char** azColName);
	
public:
    bool open();
    int create(); //0 DB ERROR NOT CREATED 1:CREATED
    int addUser(string,string,string,string); // 0 for database not connected, 1 for Username already taken, 2 for success
    int delUser(string); // 0:DB Error 1:Failure 2:Success
    int passCheck(string , string, string&); //0:DB Error 1:Fail 2:Pass
    void close();
    int Usersquery(string,vector<string>&); //Gives names of 20 people starting with Username
    UserLoginDb(string);
    int getUserLocalDir(string,string&); // Gives local dir of given username 
};
#endif /* defined(__Database_final__LoginDb__) */
