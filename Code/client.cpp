#include "client.h"



void Client::sendFilename(char* fName){

	if (!connected) {
		printf("%s\n","not connected to server");
		exit(1);
	}


	int n,  written;

	char buffer[1024];
	strcpy(buffer , fName);
	n=strlen(buffer);

	written=write(sockfd , buffer,n);
	if (written < 0){
		printf("%s\n", "Error sending file name");
		exit(1);
	}

	printf("File name : %s sent to server \n",buffer);
}




void Client::fileTransfer(int fd){

	if (!connected){
		printf("%s\n", "Not connected to server");
		exit(1);
	}	

	int BUF_SIZE = 1024;
	char buffer[BUF_SIZE];
	int bytesread, ret;
	do
	{
		bytesread = read(fd, buffer, BUF_SIZE);
		ret = write(sockfd, buffer, BUF_SIZE);
		if (ret < 0) {    
			printf("Error sending data!\n\t-%s", buffer);  
			exit(1);  
		}  
	}
	while (bytesread != 0);

}


void Client::readData(){


	if (!connected) {
		printf("%s\n","not connected to server");
		exit(1);
	}


	int BUF_SIZE = 1024;
	int rv;
	int bytesread;

	char buffer[BUF_SIZE];
	fstream file;
	char name;
	stringstream fileName;

	while ((bytesread = read(sockfd, &name, sizeof(name) )) != 0)
	{
		if (name == '\0') break;
		fileName << name;

	}


	file.open(fileName.str(),ios::out);

	while ((rv = read(sockfd, buffer, BUF_SIZE) ) != 0)   
		file<<buffer;

	file.close();
}





void Client::readFromServer()
{

	if (!connected) {
		printf("%s\n","not connected to server");
		exit(1);
	}

	bool interrupt = false;
	int rv, r;

	pollfd pf;
	pf.fd = sockfd;
	pf.events = POLLIN;


	do{

		rv = poll(&pf, 1, -1);
		if (rv == -1)
			perror("poll"); // error occurred in poll()

		if (pf.revents && POLLIN)
			readData();



		printf("%s\n", "Press 1 to continue receiving files");
		scanf("%d", &r);
		if (r == 1) interrupt = true;

	} 
	while (!interrupt);



}

void Client::connectToServer(char* serverAddr, int PORT)
{

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

/* Unit testing */
  	
void Client::writeToServer(char* a){

	if (!connected) {
		printf("%s\n","not connected to server");
		exit(1);
	}

	int fd = open(a, O_RDONLY);
	if (fd < 0)
	{
		printf("%s\n","File IO error");
		exit(1);
	}

	sendFilename(a);
	char b = '\0';
	int n = write(sockfd, &b, sizeof(b));
	fileTransfer(fd);

	close(fd);

}









