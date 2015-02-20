
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


using namespace std;




class Server{
private:
	sockaddr_in addr, cl_addr;
	/* Do unit testing */
	void sendFilename(char* fName, int sd);
	void fileTransfer(int fd, int sockfd);
	void readData(int);


	void SendData(int, char* );
	void ReceiveData(int);	

public:

	void ChildProcess(int, int); // Handle a client when a connection is made
	void CreateServer(char*, int); // main 	


};


#endif








