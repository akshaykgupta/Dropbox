#include "server.h"


void Server::sendFilename(char* fName, int sd)
{
	int n, byteswritten=0 , written;

	char buffer[1024];
	strcpy(buffer , fName);
	n=strlen(buffer);
	while (byteswritten < n)
	{
		written=write(sd , buffer+byteswritten,(n-byteswritten));
		byteswritten+=written;
	}

	printf("File name : %s sent to server \n",buffer);
}

void Server::fileTransfer(int fd, int sockfd)
{
	int BUF_SIZE = 1024;
	char buffer[BUF_SIZE];
	int bytesread;
	do
	{
		bytesread = read(fd, buffer, BUF_SIZE);
		int ret = write(sockfd, buffer, BUF_SIZE);    
		if (ret < 0) {    
			printf("Error sending data!\n\t-%s", buffer);  
			exit(1);  
		}  
	}
	while (bytesread != 0);

}


void Server::readData(int newsockfd)
{

	int BUF_SIZE = 1024;

	char buffer[BUF_SIZE];
	ofstream f;
	int rv;
	char name;
	int bytesread;
	stringstream fileName;

	while ((bytesread = read(newsockfd, &name, sizeof(name) )) != 0)
	{
		if (name == '\0') break;
		fileName << name;

	}


	f.open(fileName.str(),ios::out);

	while ((rv = read(newsockfd, buffer, BUF_SIZE) ) != 0)   
		f.write(buffer, BUF_SIZE);
	

	f.close();
	
}


void Server::ReceiveData(int newsockfd){

	bool interrupt = false;
	int r, rv;

	pollfd pf;
	pf.fd = newsockfd;
	pf.events = POLLIN;


	do{

	rv = poll(&pf, 1, -1);
	if (rv == -1)
		perror("poll"); // error occurred in poll()

	if (pf.revents && POLLIN)
		readData(newsockfd);



		printf("%s\n", "Press 1 to continue receiving files");
		scanf("%d",&r);
		if (r == 1) interrupt = true;

	} while (!interrupt);



}


void Server::SendData(int sockfd, char* a)
{

	int fd = open(a, O_RDONLY);
	if (fd < 0)
	{
		printf("%s\n","File IO error");
		exit(1);
	}

	sendFilename(a, sockfd);
	char b = '\0';
	int n = write(sockfd, &b, sizeof(b));
	fileTransfer(fd, sockfd);

	close(fd);


}



void Server::ChildProcess(int sockfd, int newsockfd)
{
	close(sockfd);    
	char* a;
	int opt = 0;
 	while (true){
 		printf("%s\n","press 1 to send file");
 		printf("%s\n","press 2 to receive files");

 		while (opt != 0) scanf("%d",&opt);

 		if (opt == 1)
 		{
 			printf("%s\n", "Enter file name to send");
 			scanf("%s",a);
 			SendData(newsockfd,a);
 		}	
 		
 		if (opt == 2)
 		{
 			printf("%s\n","Receiving Data from client..");
 			ReceiveData(newsockfd);
 		}


 	}


 	 
}



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


