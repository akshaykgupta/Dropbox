//
//  UserDb.h
//  DatabasePerUser
//
//  Created by Barun Patra on 27/02/15.
//  Copyright (c) 2015 Sample. All rights reserved.
//

#ifndef __DatabasePerUser__UserDb__
#define __DatabasePerUser__UserDb__


#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <iostream>
#include <utility>
#include <stdlib.h>
#define DATABASE_NAME "Parent.db"
using namespace std;
struct tpl
{
    string first;
    string second;
    string third;
};
class UserUtilDb
{
private:
    string userTableOwned; //Initialize in create as BarunTableOwned
    string userTableShared; //Initialize in create as ...
    sqlite3* Database;
    char* stringToChar(const string &);
    void convertToDatabaseString(string&);
    int createdOwned(); //Has Created Owned? 0:DB Error 1:Created 2:Not Created
    int createdShared();//Has Created Shared? 0:DB Error 1:Created 2:Not Created
    bool open();
public:
    UserUtilDb(string); //GetString
    int createOwned(); //0:NOT CREATED 1:CREATED
    int createShared(); //0:NOT CREATED 1:CREATED
    int removeUser(); //Drop both Tables;
    int addToTableOwned(string Fnm,string Ver,string type,string Pth,string Shw="NOBODY"); //files,versions,type, path,sharedWith
    int addToTableShared(string,string,string,string,string); //files,Owner,Permission,type,path
    int getFileFromOwned(string& Dest,string Fname,string Ver=""); //filename and version. By default Most recent. Return Path
    int getFileFromShared(string,string&,string&); //files. filename. populate path, populate Permission
    int getAllFilesOwned(vector<string> &); //populates vector with all files.
    int getAllPathandVersions(vector<pair<string,string> > &); //populates vector with (Path,Version) pair
    int getAllPathandShared(vector<pair<string,string> > &); //populates vector with(File,Shared_with) pair
    int getAllPathsSharedandOwners(vector<pair<string,string> > &); //populates vector with(File,Owner) pair
    int removeFromOwned(string fname,string ver=""); //Filename and Versions. Delete all rows if nothing specified
    int removeFromShared(string,string); //Filename and Owner
    void close();
    int getFilesAndPathForFolderPath(string path,string version,vector<pair<string,string> > &results);
    int getFilesAndPathForFolderHelper(string Fname, string version, vector<tpl> &results);
    int getPathGivenFile(string,string&);
    int getFilesAndPathForFolder(string name,string version,vector<pair<string,string> > &results);
	int getPermissionGivenPathAndSharee(string& perm,string Path,string Sharee);
	int getAllPathsShareePermissions(vector<tpl> &);
	int removeSharedFromOwned(string shareID,string path);
	int pathToLatestVersion(string path,string &version);
	int getAllPathsAndLatestVersion(vector<pair<string,string> > & paths);
};

#endif /* defined(__DatabasePerUser__UserDb__) */
