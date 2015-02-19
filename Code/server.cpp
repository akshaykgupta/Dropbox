#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>  
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <iostream>

using namespace std;

#define BUF_SIZE 2000


class Server{
private:
	sockaddr_in addr, cl_addr;

	void SendData(int, char* );
	void ReceiveData(int , char* );	

public:

	void ChildProcess(int, int); // Handle a client when a connection is made
	void CreateServer(char*, int); // main 	


};


/* * * * * * * * * * * * * * * * * * * * * * * * * * */

void Server::ReceiveData(int newsockfd, char* buffer){
	int ret;
	int len = sizeof(cl_addr);
	ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, (socklen_t*) &len);  
  	if(ret < 0) {  
   			printf("Error receiving data!\n");    
   			exit(1);  
 	}
}


void Server::SendData(int newsockfd, char* buffer){
	int ret;
	int len = sizeof(cl_addr);
	ret = sendto(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);     
 	if (ret < 0) {    
   		printf("Error sending data!\n");    
   		exit(1);    
 	}

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * */

void Server::ChildProcess(int sockfd, int newsockfd){
	close(sockfd);     // stop listening for new connections. handle only the current connection
	char buffer[BUF_SIZE];
 	while (true){ 
		memset(buffer, 0, BUF_SIZE);  
		ReceiveData(newsockfd, buffer);
		SendData(newsockfd, buffer);  
 	}  
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * */

void Server::CreateServer(char* a, int port)
{
		int sockfd, ret, len, newsockfd;
		char clientAddr[100];
		pid_t childpid;

		//char* serverAddr = a;
		sockfd = socket(AF_INET, SOCK_STREAM,0); // create a socket
		if (sockfd < 0)
		{
			printf("%s\n", "ERROR");
			exit(1);
		}

		memset(&addr, 0, sizeof(addr));    // adding to server
		addr.sin_family = AF_INET;    
		addr.sin_addr.s_addr = INADDR_ANY;  
		addr.sin_port = port;    

		ret = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));  // connect socket to server
		listen(sockfd, 5);	// backlog queue for socket connection

		while (true)
		{
			 len = sizeof(cl_addr); 
			 newsockfd = accept(sockfd, (struct sockaddr *) &cl_addr, (socklen_t*) &len); // create connection with a client

			 if (newsockfd < 0){
			 	printf("Error accepting connection!\n");  
     			exit(1);  
			 }

			 inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, 100);

			 if ((childpid = fork() ) == 0){
			 	ChildProcess(sockfd, newsockfd);
			 }

			 close(newsockfd);
		}   
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * */
