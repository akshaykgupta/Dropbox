#include "client.h"



void changeToChar(char buffer[], int bytesread)
{
	for (int i = 0; i < 4; i++)
	{
		buffer[1019 + i] = (char)(bytesread % 10) + '0';
		bytesread /= 10;
	}
}


void Client::fileTransfer(int fd){

	if (!connected){
		printf("%s\n", "Not connected to server");
		exit(1);
	}	

	int BUF_SIZE = 10000;
	char buffer[BUF_SIZE];
	int bytesread, ret,rv,byteswrite;
	pollfd pf;
	pf.fd = sockfd;
	pf.events = POLLOUT;
	do
	{
		rv = poll(&pf, 1, -1);
		if (rv == -1)
		perror("poll"); // error occurred in poll()
		if (pf.revents & POLLOUT)
		{	
			memset(buffer, 0, BUF_SIZE);
			bytesread = read(fd, buffer, BUF_SIZE-1);
			if(bytesread == 0)
				break; 
			if(bytesread < BUF_SIZE-1)
				buffer[BUF_SIZE-1] = '1';
			else
				buffer[BUF_SIZE-1] = '2';
			//changeToChar(buffer, bytesread);
			//ret = write(sockfd, buffer, BUF_SIZE);
			ret = SSL_write(ssl, buffer, BUF_SIZE);
			if (ret < 0)
			 { 
				printf("Error sending data!\n\t-%s", buffer);  
				exit(1);
			}  
			memset(buffer,0,BUF_SIZE);
			//byteswrite  = read(sockfd,buffer,BUF_SIZE);
			byteswrite  = SSL_read(ssl,buffer,BUF_SIZE);
		}  
	}
	while (bytesread != 0);

}

void Client::fileTransfer(ifstream& f){

	if (!connected){
		printf("%s\n", "Not connected to server");
		exit(1);
	}	

	int BUF_SIZE = 1024;
	char buffer[BUF_SIZE];
	int bytesread, ret,rv,byteswrite;
	pollfd pf;
	pf.fd = sockfd;
	pf.events = POLLOUT;
	do
	{
		rv = poll(&pf, 1, -1);
		if (rv == -1)
		perror("poll"); // error occurred in poll()
		if (pf.revents & POLLOUT)
		{	
			memset(buffer, 0, BUF_SIZE);
			int cn = 0;
			char c;
			while(cn < 1022 && f.get(c)){
				buffer[cn] = c;
				cn++;
			}
			if(cn < 1022)
				buffer[++cn] = '\0';
			else
				buffer[cn] = '\0';
			if(f.eof())
				buffer[1023] = '1';
			else
				buffer[1023] = '2';
			//ret = write(sockfd, buffer, BUF_SIZE);
			ret = SSL_write(ssl, buffer, BUF_SIZE);
			if (ret < 0)
			 { 
				printf("Error sending data!\n\t-%s", buffer);  
				exit(1);
			}
			memset(buffer,0,BUF_SIZE);
			//byteswrite  = read(sockfd,buffer,BUF_SIZE);
			byteswrite = SSL_read(ssl, buffer, BUF_SIZE);
			if(strcmp(buffer,"continue") == 0)
				cout<<"hi"<<"\n";
			else
				break;
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


string Client::readCommandFromServer()
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
				if (b == DELIMITER) break;
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
	rv = poll(&pf, 1, -1);
	if (rv == -1)
		perror("poll"); // error occurred in poll()

	if (pf.revents && POLLIN)
		readFile();

}

SSL_CTX* Client::InitCTX()
{   SSL_METHOD *method;
    SSL_CTX *ctx;

    OpenSSL_add_all_algorithms();		/* Load cryptos, et.al. */
    SSL_load_error_strings();			/* Bring in and register error messages */
    method = SSLv2_client_method();		/* Create new client-method instance */
    ctx = SSL_CTX_new(method);			/* Create new context */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
	cout<<"hi\n";
    return ctx;
}


void Client::ShowCerts()
{   X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl);	/* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);							/* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);							/* free the malloc'ed string */
        X509_free(cert);					/* free the malloc'ed certificate copy */
    }
    else
        printf("No certificates.\n");
}

void Client::connectToServer(char* serverAddr, int PORT)
{
	cout<<serverAddr<<" "<<PORT<<endl;
	SSL_library_init();
	if (!exec)
	{
		printf("%s\n", "Error creating socket");
		exit(1);
	}
	SSL_CTX *ctx;
	int ret;    
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
		cout<<"hi\n";
		ctx = InitCTX();
		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, sockfd);
		if(SSL_connect(ssl) == -1)
		{
  			printf("%s\n","Error estabilishing SSL connection");
  			exit(1);
		}
  		connected  = true;
  		printf("%s\n","Connected to server");
		ShowCerts();
}

/* Unit testing */
  	
void Client::writeToServer(char* a){

	if (!connected) {
		printf("%s\n","not connected to server");
		exit(1);
	}
	// ifstream f;
	// f.open("experiment.txt");
	int fd = open(a, O_RDONLY);
	if (fd < 0)
	{
		printf("%s\n","File IO error");
		exit(1);
	}
	
	long int pos = lseek(fd, 0L, SEEK_END);
	char sizefile[64];
	sprintf(sizefile, "%ld", pos);
	cout<<sizefile<<"\n";
	lseek(fd, 0, SEEK_SET);
	writeCommand(a);
	writeCommand(sizefile);
	//fileTransfer(f);
	fileTransfer(fd);
	// int n = write(sockfd, &b, sizeof(b));
	//f.close();
	close(fd);

}

string Client::readCommand()
{
	char name;
	int r, rv;
	string s = "";
	pollfd pf;
	pf.fd = newsockfd;
	pf.events = POLLIN;
	
	while (true)
	{
		rv = poll(&pf, 1, -1);
		if (rv == -1)
			perror("poll"); // error occurred in poll()
		if (pf.revents && POLLIN) {
			if((r = SSL_read(ssl, &name, sizeof(name))) != 0)
			{
				if (name == DELIMITER) break;
				s+= name;
			}
			else
				break;
		}
	}
	cout<<s<<"\n";
	return s;
}

void Client::writeCommand(char* a)
{
	if (!connected) {
		printf("%s\n","not connected to server");
		exit(1);
	}

	int n = strlen(a);
	//int written = write(sockfd, a, n);
	int written = SSL_write(ssl, a, n);
	if (written < 0)
	{
		printf("%s\n","Problem sending command");
		exit(1);
	}
	char b = DELIMITER;
	//n = write(sockfd, &b, sizeof(b));
	n = SSL_write(ssl, &b, sizeof(b));
	printf("%s\n", "command sent");
}


bool Client::verifyUser(string comparisonString)
{

	return readCommand() == comparisonString;

}
char* Client::stringToChar(string s)
{
	char* useless_conversions = new char[s.length()+1];
	int i;
	for(i = 0;i<s.length();i++)
	{
		useless_conversions[i] = s[i];
	}

	useless_conversions[i] = '\0';
	return useless_conversions;
}
bool Client::checkIfAllowed(string& username, string& fileName)
{
	
	writeCommand(stringToChar(username));
	writeCommand(stringToChar(fileName));
	return verifyUser("Allowed");

}


