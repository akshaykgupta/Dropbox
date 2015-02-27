
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
#include <dirent.h> 
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <resolv.h>
#include <netdb.h>
#include <set>
//#include "LoginDb.h"

using namespace std;

#define UPLOAD "upload"
#define DOWNLOAD "download"
#define SHARE "share"
#define REGISTER "register"
#define LOGIN "login"
#define DELETE "delete"
#define DELIMITER '\0'
#define MAINTABLE "UserTable"

class Server{
private:

	//set<User* > Users;
	sockaddr_in addr, cl_addr;
	//UserLoginDb userdb;
	
	/* Do unit testing */
	void sendFilename(char*, int, SSL*);
	void fileTransfer(int, int, SSL*);
	void readData(int, SSL*);
	string readCommand(int, SSL*);
	void writeCommand(int, SSL*, char*);
	string fileName(int, SSL*);
	void uploadData(int, SSL*);
	void SendData(int, SSL*, char* );
	void ReceiveData(int, SSL*);	
	void downloadData(int, SSL*);
	void registerUser(int, SSL*);
	void loginUser(int, SSL*);
	void deleteUser(int, SSL*);
	// void buildSets();//
// 	void populate(User&, const char*);//
	SSL_CTX* InitServerCTX();
	void LoadCertificates(SSL_CTX*, char*, char*);	

public:

	void ChildProcess(int, int, SSL*); // Handle a client when a connection is made
	void CreateServer(int); // main 	
	Server()
	{
		/*
		userdb.create(MAINTABLE);
		*/
	}
	
	~Server()
	{
		userdb.close();
	}

};


#endif








