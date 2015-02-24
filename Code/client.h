
#ifndef CLIENT

#define CLIENT

#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>  
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <fcntl.h>  


using namespace std;


class Client{

private:

	struct sockaddr_in addr, cl_addr; 
	int sockfd;
	bool exec, connected;
	void sendFilename(char*);
	void fileTransfer(int);
	void readData();


public:

	
    Client() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		exec = false;
}

	void connectToServer(char* serverAddr,int);
	
	void readFromServer();
	void writeToServer(char* );  // file Name
	void writeCommand(char *); // write a char* to the server
 

};

#endif