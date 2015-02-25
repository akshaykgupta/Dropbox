#include "client.h"






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


void Client::readFile(){


	if (!connected) {
		printf("%s\n","not connected to server");
		exit(1);
	}


	int BUF_SIZE = 1024;
	int rv;


	char buffer[BUF_SIZE];
	fstream file;
	string fileName = readCommandFromServer();

	file.open(fileName,ios::out);

	while ((rv = read(sockfd, buffer, BUF_SIZE) ) != 0)   
		file<<buffer;

	file.close();
}


string readCommandFromServer()
{
	if (!connected)
	{
		printf("%s\n","not connected to server");
		exit(1);
	}

	pollfd pf;
	pf.fd = sockfd;
	pf.events = POLLIN;



	int bytesread;
	char b;
	int rv;
	stringstream command;


	while (true){

		rv = poll(&pf, 1,-1);
		if (rv == -1) perror("poll");
		if (pf.revents && POLLIN){

		while ((bytesread = read(sockfd, &b,sizeof(b))) != 0){
				if (b == '\0') break;
				command << b;
			}
			return command.str();
		
		}

	
	}

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
			readFile();



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

	
	writeCommand(a);
	char b = '\0';
	int n = write(sockfd, &b, sizeof(b));
	fileTransfer(fd);

	close(fd);

}



void Client::writeCommand(char* a)
{
	if (!connected) {
		printf("%s\n","not connected to server");
		exit(1);
	}

	int n = strlen(a);
	int written = write(sockfd, a, n);
	if (written < 0)
	{
		printf("%s\n","Problem sending command");
		exit(1);
	}
	char b = '\0';
	int n = write(sockfd, &b, sizeof(b));
	printf("%s\n", "command sent");
}


bool Client::verifyUser(string& comparisonString)
{

	return readCommandFromServer() == comparisonString;

}

bool Client::checkIfAllowed(string& username, string& fileName)
{
	writeCommand(username);
	writeCommand(fileName);
	return verifyUser("Allowed");

}


