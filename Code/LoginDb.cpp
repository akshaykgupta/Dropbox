//
//  LoginDb.cpp
//  Database_final
//
//  Created by Barun Patra on 26/02/15.
//  Copyright (c) 2015 Sample. All rights reserved.
//

#include "LoginDb.h"
using namespace std;
//---------------------------Checks whether Table already exists or not --------------------------//
static int callCreated(void *data,int argc,char **argv,char **azColName)
{
	if(atoi(argv[0]) != 0)
    	*((bool*)data)=true;
    return 0;
}
int UserLoginDb::created()
{
    string tmpTablename=tableName;
    convertToDatabaseString(tmpTablename);
    string expr = "SELECT COUNT(*) FROM sqlite_master where type='table' and name="+tmpTablename+";";
    char* Cexpr = stringToChar(expr);
    bool data = false;
    char *zErrMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callCreated, (void*) &data, &zErrMsg);
    if(result==SQLITE_OK)
    {
        if(data == true)
            return 1;
        if(data == false)
            return 2;
    }
    else
	{
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
    return 0;
}
//------------------------Checks Whether User exists in Database---------------------------//
static int callexists(void *data,int argc,char **argv,char **azColName)
{
    if(atoi(argv[0]) != 0)
        *((bool*)data)=true;
    
    return 0;
}
int UserLoginDb::exists(string Username)
{
    convertToDatabaseString(Username);
    string expr = "SELECT COUNT(NAME) FROM "+tableName+" WHERE USERNAME="+Username+";";
    char* Cexpr = stringToChar(expr);
    bool data = false;
    char *zErrMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callexists, (void*) &data, &zErrMsg);
    if(result!=SQLITE_OK)
        return 0;
    if(data==true)
        return 1;
    return 2;
        
}

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
//----------------------------Initializes tableName. Makes TABLE tableName if it doesn't exist---------//
static int callback(void* data,int argc,char **argv,char **azColName)
{
    return 0;
}
int UserLoginDb::create()
{
    int tmp = created();
	if(tmp ==0)
        return 0;
    else if(tmp == 1)
        return 1;
    string expr = "CREATE TABLE "+tableName+"(USERNAME varchar(50),NAME varchar(30),PASS varchar(30),DIR varchar(30));";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callback, 0, &zErrorMsg);
    if(result ==SQLITE_OK)
        return 1;
    else
	{
		printf("SQL error: %s\n", zErrorMsg);
		sqlite3_free(zErrorMsg);
		return 0;
	}
}

//-----------------------Converts String to 'String' ----------------------------------------//
void UserLoginDb::convertToDatabaseString(string &tmp)
{
    tmp = "'"+tmp+"'";
}
//-----------------Adds New user to Database (Username,Name,Password, Directory)-------------------------------------//
int UserLoginDb::addUser(string Username,string name,string pass,string dir)
{
    if(exists(Username)==1)
        return 1;
    string expr="INSERT INTO "+tableName+" (USERNAME,NAME,PASS,DIR) VALUES ('"+Username+"','"+name+"','"+pass+"','"+dir+"');";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callback, 0, &zErrorMsg);
    if(result==SQLITE_OK)
		return 2;
	else
	{
		printf("SQL error: %s\n", zErrorMsg);
		sqlite3_free(zErrorMsg);
		return 0;
	}
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
    char* zErrorMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callback, 0, &zErrorMsg);
    if(result==SQLITE_OK)
        return 2;
    else
        return 1;
        
}
//-----------------------PassWord Check (Usename,Password) --------------------------------//
static int callpassCheck(void *data,int argc,char **argv,char **azColName)
{
    ((pair<string,string>*)data)->first = argv[0] ? argv[0] : "";
	((pair<string,string>*)data)->second = argv[1] ? argv[1] : "";
    return 0;
}
int UserLoginDb::passCheck(string Uname, string Pass, string& name)
{
    convertToDatabaseString(Uname);
    string expr = "SELECT PASS, NAME FROM "+tableName+" WHERE USERNAME="+Uname+";";
    char *Cexpr = stringToChar(expr);
    pair<string,string> data;
    char *zErrMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callpassCheck, (void*)&data, &zErrMsg);
    if(result!=SQLITE_OK)
	{
        printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
		return 0;
	}
    if (data.first == Pass)
	{	name = data.second;
        return 2;
	}
    return 1;
    
}
//-------------------------------Closes Connection to Database -----------------------------------//
void UserLoginDb::close()
{
    sqlite3_close(Database);
}
//-------------------------------Opens Connections to Database-----------------------------------//
bool UserLoginDb::open()
{
    if(sqlite3_open(DATABASE_NAME, &Database)==SQLITE_OK)
        return true;
    return false;
}
//------------------------------Constructor-------------------------------------------------------//
UserLoginDb::UserLoginDb(string tablenm)
{
    Database = NULL;
    tableName =tablenm;
    //open();
}
//-----------------------------SEARCHING USERS BY USERNAME----------------------------------------//
static int callSearch(void* data,int argc,char** argv,char** azColName)
{
   if(((vector<string>*)data)->size()<=20)
   {
       string tmp = argv[0] ? argv[0] : "";
	   if(tmp != "")
		   ((vector<string>*)data)->push_back(tmp);
   }
    return 0;
}
int UserLoginDb::Usersquery(string username, vector<string>& result)
{
    username+="%";
    convertToDatabaseString(username);
    string expr = "SELECT USERNAME FROM "+tableName+" WHERE USERNAME LIKE "+username+" ORDER BY USERNAME;";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int val = sqlite3_exec(Database, Cexpr, callSearch,(void*) &result, &zErrorMsg);
    if(val == SQLITE_OK)
        return 1;
    else
        return 0;
}
//---------------------------GET USER LOCAL DIR-----------------------------------------------------//
static int callGetDir(void* data,int argc,char** argv,char** azColName)
{
	*((string*)data) = argv[0]?argv[0]:"";
	return 0;
}
int UserLoginDb::getUserLocalDir(string Uname,string& dir)
{
		convertToDatabaseString(Uname);
		string expr="SELECT DIR FROM "+tableName+" WHERE USERNAME="+Uname+";";
	    char* Cexpr = stringToChar(expr);
	    char* zErrorMsg = 0;
		string data = "";
	    int result = sqlite3_exec(Database, Cexpr, callGetDir,(void*) &data, &zErrorMsg);
	    if(result != SQLITE_OK)
	        return 0;
	    else
		{	
			dir = data;
			return 1;
		}
}
