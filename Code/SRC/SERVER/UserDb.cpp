//
//  UserDb.cpp
//  DatabasePerUser
//
//  Created by Barun Patra on 27/02/15.
//  Copyright (c) 2015 Sample. All rights reserved.
//

#include "UserDb.h"
//-----------------------------------CONSTRUCTOR----------------------------------------------//
UserUtilDb::UserUtilDb(string Uname)
{
	userTableOwned = Uname+"TABLEOWNED";
	userTableShared = Uname+"TABLESHARED";
	Database=NULL;
	open();
}
//---------------------------------LINKS TO DATABASE-------------------------------------------//
bool UserUtilDb::open()
{
    if(sqlite3_open(DATABASE_NAME,&Database)==SQLITE_OK)
        return true;
    return false;
}
//--------------------------------UTILITY FUNCTIONS --------------------------------------------//
char* UserUtilDb::stringToChar(const string& src)
{
    char* buff;
    buff = new char[src.size()+1];
    int i;
    for(i=0;i<src.size();i++)
        buff[i] = src[i];
    buff[i] = '\0';
    return buff;
}

void UserUtilDb::convertToDatabaseString(string &tmp)
{
     tmp = "'"+tmp+"'";
}
//------------------------------- HAS CREATED OWNED and SHARED--------------------------------------------//
static int callCreated(void* data,int argc,char** argv,char** azColName)
{
    if(atoi(argv[0])!=0)
        *((bool*)data) = true;
    return 0;
}
int UserUtilDb::createdOwned()
{
    string tmpTableName = userTableOwned;
    convertToDatabaseString(tmpTableName);
    string expr = "SELECT COUNT(*) FROM sqlite_master where type='table' and name="+tmpTableName+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    bool data = false;
    int result = sqlite3_exec(Database, Cexpr, callCreated, (void*) &data, &zErrorMsg);
    if(result!=SQLITE_OK)
        return 0;
    else
    {
        if(data==false)
            return 2;
        else
            return 1;
    }
}
int UserUtilDb::createdShared()
{
    string tmpTableName = userTableShared;
    convertToDatabaseString(tmpTableName);
    string expr = "SELECT COUNT(*) FROM sqlite_master where type='table' and name="+tmpTableName+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    bool data = false;
    int result = sqlite3_exec(Database, Cexpr, callCreated, (void*) &data, &zErrorMsg);
    if(result!=SQLITE_OK)
        return 0;
    else
    {
        if(data==false)
            return 2;
        else
            return 1;
    }

}
//-------------------------------- CREATE OWNED and SHARED-----------------------------------------------//
static int callback(void* NotUsed,int argc,char** argv,char** azColName)
{
    return 0;
}
int UserUtilDb::createOwned()
{
    int tmp = createdOwned();
    if(tmp ==0)
        return 0;
    else if(tmp == 1)
        return 1;
    string expr = "CREATE TABLE "+userTableOwned+"(FILES varchar(50),VERSIONS varchar(30),SHAREDUSR varchar(30),TYPE varchar(10),PATH varchar(30));";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callback,0, &zErrorMsg);
    if(result==SQLITE_OK)
        return 1;
    else
        return 0;
}

int UserUtilDb::createShared()
{
    int tmp = createdShared();
    if(tmp ==0)
        return 0;
    else if(tmp == 1)
        return 1;
    string expr = "CREATE TABLE "+userTableShared+"(FILES varchar(50),OWNERS varchar(30),PERMS varchar(10),TYPE varchar(10),PATH varchar(20));";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callback,0, &zErrorMsg);
    if(result==SQLITE_OK)
        return 1;
    else
        return 0;
}
//-------------------------------------------REMOVE USER---------------------------------------------//
int UserUtilDb::removeUser()
{
    string expr1 = "DROP TABLE "+userTableOwned;
    string expr2 = "DROP TABLE "+userTableShared;
    char* Cexpr1 = stringToChar(expr1);
    char* Cexpr2 = stringToChar(expr2);
    char* zErrorMsg1 = 0;
    char* zErrorMsg2 = 0;
    int result1 = sqlite3_exec(Database, Cexpr1, callback,0, &zErrorMsg1);
    int resutl2 = sqlite3_exec(Database, Cexpr2, callback, 0, &zErrorMsg2);
    if(result1==SQLITE_OK && resutl2==SQLITE_OK)
        return 1;
    else
        return 0;
}
//----------------------------------------ADDING FILES TO TABLES-------------------------------//
int UserUtilDb::addToTableOwned(string File, string Version,string type,string Path,string Shared)
{
    string expr;
    convertToDatabaseString(File);
    convertToDatabaseString(Version);
    convertToDatabaseString(Path);
    convertToDatabaseString(type);
    if(Shared=="")
    {
        
        expr="INSERT INTO "+userTableOwned+" (FILES,VERSIONS,TYPE,PATH) VALUES("+File+","+Version+","+type+","+Path+");";
    }
    else
    {convertToDatabaseString(Shared);
        expr="INSERT INTO "+userTableOwned+" (FILES,VERSIONS,TYPE,PATH,SHAREDUSR) VALUES("+File+","+Version+","+type+","+Path+","+Shared+");";
    }
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg=0;
    int result= sqlite3_exec(Database, Cexpr, callback, 0,&zErrorMsg);
    if(result==SQLITE_OK)
        return 1;
    else
        return 0;
}

//--------------------------------------ADD TO TABLE SHARED----------------------------------//
int UserUtilDb::addToTableShared(string Fname, string Owner,string Per,string type, string Path)
{
    convertToDatabaseString(Fname);
    convertToDatabaseString(Owner);
    convertToDatabaseString(Per);
    convertToDatabaseString(Path);
    string expr = "SELECT COUNT(FILES) FROM "+userTableShared+" WHERE FILES="+Fname+" AND OWNERS="+Owner+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg =0;
    bool data = false;
    int result = sqlite3_exec(Database, Cexpr, callCreated, (void*) &data, &zErrorMsg);
    if(result!=SQLITE_OK)
        return 0;
    if(data==false)
    {
     //add
        expr ="INSERT INTO "+userTableShared+" (FILES,OWNERS,PERMS,PATH) VALUES("+Fname+","+Owner+","+Per+","+Path+");";
        Cexpr = stringToChar(expr);
        int result = sqlite3_exec(Database, Cexpr, callback, (void*) &data, &zErrorMsg);
        if(result!=SQLITE_OK)
            return 0;
        else
            return 1;
    }
    else
    {
       //update
        expr="UPDATE "+userTableShared+" SET PERMS="+Per+", PATH="+Path+" WHERE FILES="+Fname+" AND OWNERS="+Owner+";";
        Cexpr = stringToChar(expr);
        int result = sqlite3_exec(Database, Cexpr, callback, (void*) &data, &zErrorMsg);
        if(result!=SQLITE_OK)
            return 0;
        else
            return 1;
    }
    return 1;
}
//------------------------------------GETTING FILES FROM TABLES-------------------------------------//
static int callGetOwner(void* data,int argc,char **argv,char **azColsName)
{
    string tmp = "";
    if(argv[0])
    tmp = argv[0];
    *((string*)data) = tmp;
    return 0;
}
int UserUtilDb::getFileFromOwned(string& retFile,string File, string Version)
{
    convertToDatabaseString(File);
    string expr;
    if(Version!="")
	{
        convertToDatabaseString(Version);
		expr = "SELECT PATH FROM "+userTableOwned+" WHERE FILES="+File+" AND VERSIONS="+Version+";";
	}
    else
    {
        expr = "SELECT PATH FROM "+userTableOwned+" WHERE FILES="+File+" ORDER BY VERSIONS DESC;";
    }
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    retFile = "";
    int result = sqlite3_exec(Database, Cexpr, callGetOwner,(void*)&retFile, &zErrorMsg);
    if(result == SQLITE_OK)
        return 1;
    else
        return 0;
}
static int callGetShare(void* data,int argc,char **argv,char **azColsName)
{
    ((pair<string,string>*)data)->first = argv[0]?argv[0]:"";
    ((pair<string,string>*)data)->second = argv[1]?argv[1]:"";
    return 0;
}
int UserUtilDb::getFileFromShared(string filename, string& path, string& Permission)
{
    convertToDatabaseString(filename);
    string expr = "SELECT PATH, PERMS FROM "+userTableShared+" WHERE FILES="+filename+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg =0;
    pair<string,string> tmp;
    int result = sqlite3_exec(Database, Cexpr, callGetShare,(void*)&tmp, &zErrorMsg);
    if(result!=SQLITE_OK)
        return 0;
    path = tmp.first;
    Permission = tmp.second;
    return 1;
}
//----------------------------------------GET ALL FILES OWNED--------------------------------------//
static int callgetAllFilesOwned(void* data,int argc,char** argv,char** azColName)
{
    string tmp = argv[0]?argv[0]:"";
    ((vector<string>*)data)->push_back(tmp);
    return 0;
}
int UserUtilDb::getAllFilesOwned(vector<string>& results)
{
    string expr = "SELECT FILES FROM "+userTableOwned+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg=0;
    int result=  sqlite3_exec(Database, Cexpr, callgetAllFilesOwned,(void*)&results, &zErrorMsg);
    if(result==SQLITE_OK)
        return 1;
    else
        return 0;
}
//-------------------------------------GET FILES AND VERSIONS--------------------------------------//
static int callgetAllFilesVersions(void* data, int argc,char** argv,char** azColName)
{
    pair<string,string> tmp;
    tmp.first = argv[0]?argv[0]:"";
    tmp.second = argv[1]?argv[1]:"" ;
    ((vector<pair<string,string> >*)data)->push_back(tmp);
    return 0;
}
int UserUtilDb::getAllPathandVersions(vector<pair<string,string> > &results)
{
    string expr = "SELECT PATH, VERSIONS FROM "+userTableOwned+" ORDER BY PATH;";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callgetAllFilesVersions, (void*)&results, &zErrorMsg);
    if(result == SQLITE_OK)
        return 1;
    else
        return 0;
}
//-----------------------------------GET FILES AND SHARED USERS-------------------------------------//
int UserUtilDb::getAllPathandShared(vector<pair<string,string> > & results)
{
    string expr = "SELECT DISTINCT PATH, SHAREDUSR FROM "+userTableOwned+" WHERE SHAREDUSR!='NOBODY' ORDER BY SHAREDUSR;";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callgetAllFilesVersions, (void*)&results, &zErrorMsg);
    if(result == SQLITE_OK)
        return 1;
    else
        return 0;
}
//--------------------------------GET ALL FILES AND OWNERS------------------------------------------//
int UserUtilDb::getAllPathsSharedandOwners(vector<pair<string,string> >& results)
{
    string expr = "SELECT DISTINCT PATH, OWNERS FROM "+userTableShared+" ORDER BY OWNERS;";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callgetAllFilesVersions, (void*)&results, &zErrorMsg);
    if(result == SQLITE_OK)
        return 1;
    else
        return 0;
}
//--------------------------------REMOVE FROM OWNED------------------------------------------------//
int UserUtilDb::removeFromOwned(string fname, string Ver)
{
    string expr;
    char* Cexpr;
    string tmpVer=Ver;
    if(Ver!="")
    {
        convertToDatabaseString(fname);
        convertToDatabaseString(Ver);
        expr = "SELECT COUNT(*) FROM "+userTableOwned+" WHERE PATH="+fname+" AND VERSIONS="+Ver+";";
        Cexpr = stringToChar(expr);
    }
    else
    {
        convertToDatabaseString(fname);
        expr = "SELECT COUNT(*) FROM "+userTableOwned+" WHERE PATH="+fname+";";
        Cexpr = stringToChar(expr);
    }
    char* zErrorMsg= 0;
    bool data = false;
    int result = sqlite3_exec(Database, Cexpr, callCreated,(void*)&data, &zErrorMsg);
    if(result!=SQLITE_OK)
        return 0;
    else
        if(data==false)
            return 0;
    if(tmpVer=="")
    {
        expr = "DELETE FROM "+userTableOwned+" WHERE PATH="+fname+";";
        Cexpr = stringToChar(expr);
    }
    else
    {
        expr = "DELETE FROM "+userTableOwned+" WHERE PATH="+fname+" AND VERSIONS="+Ver+";";
        Cexpr = stringToChar(expr);
    }
        zErrorMsg = 0;
        result = sqlite3_exec(Database, Cexpr, callback, 0, &zErrorMsg);
    if(result ==SQLITE_OK)
        return 1;
    else
        return 0;
}
int UserUtilDb::removeFromShared(string Fname, string Owner)
{   convertToDatabaseString(Fname);
    convertToDatabaseString(Owner);
    string expr = "DELETE FROM "+userTableShared+" WHERE FILES="+Fname+" AND OWNERS="+Owner+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callback,0, &zErrorMsg);
    if(result==SQLITE_OK)
        return 1;
    else
        return 0;
}
//-------------------------------DATABASE CLOSED------------------------------------------------//
void UserUtilDb::close()
{
    sqlite3_close(Database);
}
//-------------------------------GET PATH GIVEN FILENAME----------------------------------------//
static int callStr(void* data,int argc,char** argv,char** azColName)
{
    *((string*)data) = argv[0];
    return 0;
}
int UserUtilDb::getPathGivenFile(string Fname,string& path)
{
    convertToDatabaseString(Fname);
    string expr = "SELECT PATH FROM "+userTableOwned+" WHERE FILES="+Fname+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    string data ="";
    int result = sqlite3_exec(Database, Cexpr,callStr,(void*)&data, &zErrorMsg);
    if(result!=SQLITE_OK)
        return 0;
    else
        path = data;
    return 1;
}
//-------------------------------GET FILES AND PATH FOR FOLDER----------------------------------//

int callForFilesandFolders(void* data,int argc,char** argv,char** azColName)
{
    tpl y;
    y.first = argv[0];
    y.second = argv[1];
    y.third = argv[2];
    ((vector<tpl>*)data)->push_back(y);
    return 0;
}
int UserUtilDb::getFilesAndPathForFolderHelper(string PATH, string version, vector<tpl> &results)
{
    PATH+="_%";
    convertToDatabaseString(PATH);
    convertToDatabaseString(version);
    string expr = "SELECT FILES, PATH, TYPE FROM "+userTableOwned+" WHERE VERSIONS="+version+" AND PATH LIKE "+PATH+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int returnval = sqlite3_exec(Database, Cexpr, callForFilesandFolders,(void*) &results,&zErrorMsg);
    if(returnval==SQLITE_OK)
        return 1;
    else
        return 0;
}
int UserUtilDb::getFilesAndPathForFolderPath(string PATH, string version, vector<pair<string, string> > &results)
{
    PATH+="_%";
    convertToDatabaseString(PATH);
    convertToDatabaseString(version);
    string expr = "SELECT FILES, PATH, TYPE FROM "+userTableOwned+" WHERE VERSIONS="+version+" AND PATH LIKE "+PATH+";";
    char* Cexpr = stringToChar(expr);
    vector<tpl> chk;
    char* zErrorMsg = 0;
    int returnval = sqlite3_exec(Database, Cexpr, callForFilesandFolders,(void*) &chk,&zErrorMsg);
    if(returnval!=SQLITE_OK)
        return 0;
    else
    {
        int tmp =1;
        pair<string,string> x;
        for(int i=0;i<chk.size();i++)
        {
            if(chk[i].third=="FILE")
            {   x.first = chk[i].first;
                x.second = chk[i].second;
                results.push_back(x);
            }
            else
            {
                tmp = this->getFilesAndPathForFolderPath(chk[i].first,chk[i].second,results);
            }
        }
        return tmp;
    }
}
int UserUtilDb::getFilesAndPathForFolder(string Fname, string ver, vector<pair<string, string> > &results)
{
    int tmp =0;
    string path;
    tmp = getPathGivenFile(Fname, path);
    tmp = getFilesAndPathForFolderPath(path, ver, results);
    return tmp;
}

static int callPGPS(void* data,int argc,char** argv,char** azColName)
{
    *((string*)data) = argv[0];
    return 0;
}

int UserUtilDb::getPermissionGivenPathAndSharee(string &perm, string path, string Sharee)
{
    string tableName = Sharee+"TABLESHARED";
    convertToDatabaseString(path);
    perm="";
    string expr = "SELECT PERMS FROM "+tableName+" WHERE PATH="+path+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
    int returnval = sqlite3_exec(Database, Cexpr, callPGPS,(void*) &perm,&zErrorMsg);
    if (returnval!=SQLITE_OK)
        return 0;
    else
        return 1;
}



int UserUtilDb::getAllPathsShareePermissions(vector<tpl> & results)
{
    vector<pair<string,string> > tmp;
    getAllPathandShared(tmp);
    for(int i=0;i<tmp.size();i++)
    {
        if(tmp[i].first!="")
        {
            tpl tmp2;
            tmp2.first = tmp[i].first;
            tmp2.second = tmp[i].second;
            getPermissionGivenPathAndSharee(tmp2.third, tmp2.first, tmp2.second);
            results.push_back(tmp2);
        }
    }
    return 0;
}

//---------------------------------REMOVE SHARED FORM OWNED------------------------------------------------//
int UserUtilDb::removeSharedFromOwned(string shareID,string path)
{
	convertToDatabaseString(shareID);
	string expr;
	if(path == "")
	{
		expr = "DELETE FROM "+userTableOwned+" WHERE SHAREDUSR="+shareID+";";
	}
	else
	{
		convertToDatabaseString(path);
		expr = "DELETE FROM "+userTableOwned+" WHERE SHAREDUSR="+shareID+" AND PATH="+path+";";
	}
	char* Cexpr = stringToChar(expr);
	char* zErrorMsg =0;
	int result = sqlite3_exec(Database,Cexpr,callback,0,&zErrorMsg);
	if (result==SQLITE_OK)
		return 1;
	else
		return 0;
}

//-------------------------------PATH TO LATEST VERSION----------------------------------------------------//
int UserUtilDb::pathToLatestVersion(string path,string &version)
{
	convertToDatabaseString(path);
	string expr = "SELECT VERSIONS FROM "+userTableOwned+" WHERE PATH="+path+" ORDER BY VERSIONS DESC LIMIT 1;";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg = 0;
	string data = "";
    int result = sqlite3_exec(Database, Cexpr, callPGPS,(void*) &data, &zErrorMsg);
    if(result != SQLITE_OK)
        return 0;
    else
	{	
		version = data;
		return 1;
	}
	
}

//------------------------------------------------------------------------------------------------//
static int callbackGetAllPaths(void* data,int argc,char** argv,char** azColName)
{
	if(argv[0] && argv[1])
	{
		pair<string,string> tmp;
		tmp.first = argv[0];
		tmp.second = argv[1];
		((vector<pair<string,string> >*)data)->push_back(tmp);	
	}
	return 0;
}
int UserUtilDb::getAllPathsAndLatestVersion(vector<pair<string,string> > & paths)
{
	string expr = "SELECT DISTINCT PATH, VERSIONS FROM "+userTableOwned+" ORDER BY VERSIONS DESC;";
	char* Cexpr = stringToChar(expr);
	char* zErrorMsg = 0;
	int result = sqlite3_exec(Database, Cexpr, callbackGetAllPaths,(void*) &paths, &zErrorMsg);
	if(result != SQLITE_OK)
		return 0;
	else
		return 1;
}

int UserUtilDb::getSharedUsersList(vector<string>& results)
{
    string expr = "SELECT DISTINCT SHAREDUSR FROM "+userTableOwned+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg=0;
    int result=  sqlite3_exec(Database, Cexpr, callgetAllFilesOwned,(void*)&results, &zErrorMsg);
    if(result==SQLITE_OK)
        return 1;
    else
        return 0;
}

int UserUtilDb::getOwnersList(vector<string> & results)
{
    string expr = "SELECT DISTINCT OWNERS FROM "+userTableOwned+";";
    char* Cexpr = stringToChar(expr);
    char* zErrorMsg=0;
    int result=  sqlite3_exec(Database, Cexpr, callgetAllFilesOwned,(void*)&results, &zErrorMsg);
    if(result==SQLITE_OK)
        return 1;
    else
        return 0;
}

int UserUtilDb::removeOwnerFromShared(string owner)
{
    convertToDatabaseString(owner);
    string expr = "DELETE FROM "+userTableShared+" WHERE OWNERS="+owner+";";
    char* Cexpr = stringToChar(Cexpr);
    char* zErrorMsg = 0;
    int result = sqlite3_exec(Database, Cexpr, callback,0, &zErrorMsg);
    if(result==SQLITE_OK)
        return 1;
    else
        return 0;
}


int UserUtilDb::removeFromOwnedFolder(string fname, string Ver)
{
    string expr;
    char* Cexpr;
    string tmpVer=Ver;
	string temp = fname;
	temp += "%";
    if(Ver!="")
    {
        convertToDatabaseString(fname);
        convertToDatabaseString(Ver);
        expr = "SELECT COUNT(*) FROM "+userTableOwned+" WHERE PATH="+fname+" AND VERSIONS="+Ver+";";
        Cexpr = stringToChar(expr);
    }
    else
    {
        convertToDatabaseString(fname);
        expr = "SELECT COUNT(*) FROM "+userTableOwned+" WHERE PATH="+fname+";";
        Cexpr = stringToChar(expr);
    }
    char* zErrorMsg= 0;
    bool data = false;
    int result = sqlite3_exec(Database, Cexpr, callCreated,(void*)&data, &zErrorMsg);
    if(result!=SQLITE_OK)
        return 0;
    else
        if(data==false)
            return 0;
    if(tmpVer=="")
    {
        convertToDatabaseString(temp);
        expr = "DELETE FROM "+userTableOwned+" WHERE PATH LIKE"+temp+";";
        Cexpr = stringToChar(expr);
    }
    else
    {
        convertToDatabaseString(temp);
        expr = "DELETE FROM "+userTableOwned+" WHERE PATH LIKE"+temp+" AND VERSIONS="+Ver+";";
        Cexpr = stringToChar(expr);
    }
        zErrorMsg = 0;
        result = sqlite3_exec(Database, Cexpr, callback, 0, &zErrorMsg);
    if(result ==SQLITE_OK)
        return 1;
    else
        return 0;
}

