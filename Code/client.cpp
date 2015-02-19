#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>  
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <iostream>
#include <netdb.h>

using namespace std;

class Client{
private:
	sockaddr_in addr, cl_addr; 
	int sockfd;
	bool exec, connected;


public:

	Client(): sockfd(socket(AF_INET, SOCK_STREAM, 0)){
		if (sockfd < 0)
			exec = false;
	}

	void connectToServer(char* serverAddr, User& u, int PORT){

		if (!exec)
		{
			printf("%s\n", "Error creating socket");
			exit(1);
		}

		int sockfd, ret;    
  		memset(&addr, 0, sizeof(addr));    
  		addr.sin_family = AF_INET;    
  		addr.sin_addr.s_addr = inet_addr(serverAddr);  
  		addr.sin_port = PORT; 


  		ret = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));   // connects client socket to the user  
  		if (ret < 0)
  		{
  			printf("%s\n","Error estabilishing connection");
  			exit(1);
  		}

  		bool connected  = true;
  		printf("%s\n","Connected to server");

	}

	void writeToServer();
	void readFromServer();
}





