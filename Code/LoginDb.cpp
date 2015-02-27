//
//  LoginDb.cpp
//  Database_final
//
//  Created by Barun Patra on 26/02/15.
//  Copyright (c) 2015 Sample. All rights reserved.
//

#include "LoginDb.h"
using namespace std;
//-----------------------Converts String to Char ----------------------------------------//
char* UserLoginDb::stringToChar(const string & src)
{
    char* buff;
    buff = new char[src.size()+1];
    int i;
    for(i=0;i<src.size();i++)
        buff[i] = src[i];
    buff[i] = '\0';
    return buff;
}
//-----------------------Converts String to 'String' ----------------------------------------//
void UserLoginDb::convertToDatabaseString(string &tmp)
{
    tmp = "'"+tmp+"'";
}
//-----------------Adds New user to Database (Username,Name,Password)-------------------------------------//
int UserLoginDb::addUser(string Username,string name,string pass)
{
    if(exists(Username)==1)
        return 1;
    sqlite3_stmt* statement;
    string expr="INSERT INTO "+tableName+" (USERNAME,NAME,PASS) VALUES ('"+Username+"','"+name+"','"+pass+"');";
        
    char* Cexpr = stringToChar(expr);
    if(sqlite3_prepare_v2(Database, Cexpr, -1, &statement, 0)==SQLITE_OK)
        {
            int result = 0;
            result = sqlite3_step(statement);
            if(result==101)
                return 2;
            else
                return 0;
        }
        else
            return 0;
   
}
//-----------------------Deletes User(Username)----------------------------------------//
int UserLoginDb::delUser(string Uname)
{
    int tmp = exists(Uname);
    if(tmp==0)
        return 0;
    if(tmp==2)
        return 1;
    convertToDatabaseString(Uname);
    string expr = "DELETE FROM "+tableName+" WHERE USERNAME="+Uname+";";
    char* Cexpr = stringToChar(expr);
    sqlite3_stmt *statement;
    if(sqlite3_prepare_v2(Database, Cexpr, -1, &statement, 0)==SQLITE_OK)
    {
        int result = 0;
        result = sqlite3_step(statement);
        if(result == 101)
            return 2;
        else
            return 0;
    }
    else
        return 0;
}
//------------------------Checks Whether User exists in Database---------------------------//
int UserLoginDb::exists(string Username)
{
    convertToDatabaseString(Username);
    string expr = "SELECT COUNT(NAME) FROM "+tableName+" WHERE USERNAME="+Username+";";
    char* Cexpr = stringToChar(expr);
    sqlite3_stmt *statement;
    if(sqlite3_prepare_v2(Database, Cexpr, -1, &statement, 0)==SQLITE_OK)
    {
        int result = 0;
        result = sqlite3_step(statement);
        string val ="";
        char* tmp = (char*)sqlite3_column_text(statement,0);
        if(tmp)
        {
            val = tmp;
        }
        int check = atoi(val.c_str());
        if(check!=0)
            return 1;
        else
            return 2;
    }
    else
        return 0;
}
//-----------------------PassWord Check (Usename,Password) --------------------------------//
int UserLoginDb::passCheck(string Uname, string Pass)
{
    convertToDatabaseString(Uname);
    string expr = "SELECT PASS FROM "+tableName+" WHERE USERNAME="+Uname+";";
    char *Cexpr = stringToChar(expr);
    sqlite3_stmt* statement;
    if(sqlite3_prepare_v2(Database, Cexpr, -1, &statement, 0)==SQLITE_OK)
    {
        int result = 0;
        result = sqlite3_step(statement);
        if(result == 101)
            return 1;
        string val = "";
        char* tmp = (char*)sqlite3_column_text(statement,0);
        if(tmp)
            val=tmp;
        if(val == Pass)
            return 2;
        else
            return 1;
    }
    else
        return 0;
    
}
//-------------------------------Closes Connection to Database -----------------------------------//
void UserLoginDb::close()
{
    sqlite3_close(Database);
}

bool UserLoginDb::open()
{
    if(sqlite3_open(DATABASE_NAME, &Database)==SQLITE_OK)
        return true;
    return false;
}
//------------------------------Constructor-------------------------------------------------------//
UserLoginDb::UserLoginDb()
{
    Database = NULL;
    tableName ="";
    open();
}
//---------------------------Checks whether Table already exists or not --------------------------//
int UserLoginDb::created()
{
    string tmpTablename=tableName;
    convertToDatabaseString(tmpTablename);
    string expr = "SELECT COUNT(*) FROM sqlite_master where type='table' and name="+tmpTablename+";";
    char* Cexpr = stringToChar(expr);
    sqlite3_stmt* statement;
    if(sqlite3_prepare_v2(Database, Cexpr, -1, &statement, 0)==SQLITE_OK)
    {
        int result =0;
        result = sqlite3_step(statement);
        if(result == 101)
            return 0;
        string val = "";
        char* tmp = (char*)sqlite3_column_text(statement,0);
        if(tmp)
            val=tmp;
        int value = atoi(val.c_str());
        if(value==0)
            return 2;
        else
            return 1;
    }
    else
        return 0;
    
}
//----------------------------Initializes tableName. Makes TABLE tableName if it doesn't exist---------//
int UserLoginDb::create(string tablenm)
{
    tableName = tablenm;
    int tmp = created();
    if(tmp ==0)
        return 0;
    else if(tmp == 1)
        return 1;
    string expr = "CREATE TABLE "+tableName+"(USERNAME varchar(50),NAME varchar(30),PASS varchar(30));";
    char* Cexpr = stringToChar(expr);
    sqlite3_stmt* statement;
    if(sqlite3_prepare_v2(Database, Cexpr, -1, &statement, 0)==SQLITE_OK)
    {
        int result = 0;
        result = sqlite3_step(statement);
        if(result == 101)
            return 1;
        else
            return 0;
    }
    else
        return 0;
}
