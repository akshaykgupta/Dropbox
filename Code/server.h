
#ifndef SERVER

#define SERVER


#include <stdio.h> 
#include <sstream>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>  
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>  
#include <sys/poll.h>
#include <fstream>
#include <vector>
#include <dirent.h> 
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <boost/filesystem.hpp>
#include <resolv.h>
#include <netdb.h>
#include <set>
#include "LoginDb.h"
#include "UserDb.h"

using namespace std;
using namespace boost::filesystem;

#define UPLOADFILE "uploadfile"
#define UPLOADFOLDER "uploadfolder"
#define DOWNLOADFILE "downloadfile"
#define DOWNLOADFOLDER "downloadfolder"
#define SHARE "share"
#define UNSHARE "unshare"
#define REGISTER "register"
#define LOGIN "login"
#define DELETEFILE "deletefile"
#define DELETEUSER "deleteuser"
#define CLOSE "close"
#define SEARCH "search"
#define SHAREDOWNLOAD "sharedownload"
#define CHANGEPERMISSION "changepermission"
#define OPENSHARE "openshare"
#define DOWNLOADSHARED "downloadshared"
#define SYNC "sync"
#define DELIMITER '\0'
#define MAINTABLE "UserTable"

class Server{
private:

	//set<User* > Users;
	sockaddr_in addr, cl_addr;
	UserLoginDb logindb;
	string rootDir = "/Users/Akshay/Desktop/Users/";
	
	/* Do unit testing */
	void sendFilename(char*, int, SSL*);
	void fileTransfer(int, int, SSL*);
	bool readData(int, SSL*);
	string readCommand(int, SSL*);
	void writeCommand(int, SSL*, char*);
	void writeCommand(int, SSL*, string&);
	void getFile(int, SSL*, ofstream&, long int);
	void uploadFile(int, SSL*);
	void uploadFolder(int, SSL*);
	void SendData(int, SSL*, char* );
	void downloadFile(int, SSL*);
	void downloadFolder(int, SSL*);
	void downloadSharedFile(int, SSL*);
	void shareData(int, SSL*);
	void unshareData(int, SSL*);
	void registerUser(int, SSL*);
	void loginUser(int, SSL*);
	void deleteUser(int, SSL*);
	void deleteFile(int, SSL*);
	void searchUsers(int, SSL*);
	void changePermissions(int, SSL*);
	void sendSharedFiles(int, SSL*);
	void syncFiles(int, SSL*);

	SSL_CTX* InitServerCTX();
	void LoadCertificates(SSL_CTX*, char*, char*);	
	char* stringToChar(string &);

public:

	void ChildProcess(int, int, SSL*); // Handle a client when a connection is made
	void CreateServer(int); // main 	
	Server() : logindb(MAINTABLE)
	{}
};


#endif








