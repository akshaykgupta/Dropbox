#include "server.h"


void Server::sendFilename(char* fileName, int sd, SSL *ssl)
{
	int n, byteswritten=0 , written;

	char buffer[1024];
	strcpy(buffer , fileName);
	n=strlen(buffer);
	SSL_write(ssl , buffer, 1024);
	printf("File name : %s sent to server \n",buffer);
}

void Server::fileTransfer(int fd, int sockfd, SSL *ssl)
{
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


// string Server::readCommand(int newsockfd)
// {
// 	char name;
// 	int r;
// 	string s = "";
// 	while ((r = read(newsockfd, &name, sizeof(name))) != 0)
// 	{
// 		if (name == DELIMITER) break;
// 		s+= name;
// 	}
// 	cout<<s<<"\n";
// 	return s;
// }

string Server::readCommand(int newsockfd, SSL *ssl)
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

void Server::writeCommand(char* a, int newsockfd, SSL* ssl)
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

// void Server::uploadData(int newsockfd)
// {
// 	ofstream f;
//
// 	int BUF_SIZE = 1024;
// 	int rv;
//
// 	char buffer[BUF_SIZE];
// 	memset(buffer, 0, BUF_SIZE);
// 	string s = fileName(newsockfd);
// 	f.open(s, ios::out);
// 	while ((rv = read(newsockfd, buffer, BUF_SIZE)) != 0)
// 	{
// 		if(buffer[0] == '\0')
// 			break;
// 		cout<<strlen(buffer)<<"\n";
// 		f.write(buffer, strlen(buffer));
// 		memset(buffer, 0, BUF_SIZE);
// 	}
//
// 	f.close();
//
// 	// Handle version control, and change user info
//
//
// }

void Server::uploadData(int newsockfd, SSL *ssl)
{
	ofstream f;

	int BUF_SIZE = 10000;
	int r, rv, ret;

	char buffer[BUF_SIZE];
	string s = fileName(newsockfd, ssl);
	f.open(s, ios::out);
	string sizefile = readCommand(newsockfd, ssl);
	long int size = stol(sizefile);
	// pollfd pf;
	// pf.fd = newsockfd;
	// pf.events = POLLIN;
	long int byteswritten = 0;
	while (byteswritten < size)
	{
		memset(buffer, 0, BUF_SIZE);
		// rv = poll(&pf, 1, -1);
		// if (pf.revents & POLLIN) {
			// if((r = read(newsockfd, buffer, BUF_SIZE)) != 0)
		if((r = SSL_read(ssl, buffer, BUF_SIZE)) != 0)
			{
				if (buffer[BUF_SIZE-1] == '1')
				{
					cout<<"read!!!!!!!!!\n\n\n\n\n\n\n";
					f.write(buffer, size - byteswritten);
					byteswritten += size - byteswritten;
					memset(buffer, 0, BUF_SIZE);
					buffer[0] = '\0';
					strcat(buffer, "discontinue");
					//ret = write(newsockfd, buffer, BUF_SIZE);
					ret = SSL_write(ssl, buffer, BUF_SIZE);
					break;
				}
				byteswritten += BUF_SIZE-1;
				f.write(buffer, BUF_SIZE-1);
				memset(buffer, 0, BUF_SIZE);
				buffer[0] = '\0';
				strcat(buffer, "continue");
				//ret = write(newsockfd, buffer, BUF_SIZE);
				ret = SSL_write(ssl, buffer, BUF_SIZE);
			// }
			
			// else
			// {
			// 	cout<<"not here\n";
			// 	break;
			// }
		}
	}

	f.close();
	// char stuff[120];
	// stuff[0] = '\0';
	// strcat(stuff, "mv experiment.txt ");
	// strcat(stuff, s.c_str());
	// cout<<stuff<<"\n";
	// system(stuff);
	// Handle version control, and change user info


}

void downloadData(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	string fileName = readCommand(newsockfd, ssl);
	const char *a = fileName.c_str();
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
	writeCommand(sizefile, newsockfd, ssl);
	fileTransfer(fd, newsockfd, ssl);
}

void Server

string Server::fileName(int newsockfd, SSL *ssl)
{
	return readCommand(newsockfd, ssl);
}

string Server::registerUser(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	string password = readCommand(newsockfd, ssl);
	string name = readCommand(newsockfd, ssl);
	/*
	int worked = userdb.addUser(username, name, password);
	if(worked == 0)
		cout<<"DB error :(\n";
	else if(worked == 1)
		writeCommand("Unsuccessful");
	else if(worked == 2)
		writeCommand("Successful");
	*/
}

void Server::loginUser(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	string password = readCommand(newsockfd, ssl);
	/*
	int worked = userdb.passCheck(username, password);
	if(worked == 0)
		cout<<"DB error:(\n";
	else if(worked == 1)
		writeCommand("Invalid");
	else if(worked == 2)
		writeCommand("Valid");
	*/
}

void Server::deleteUser(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	/*
	int worked = delUser(username);
	*/
}

void Server::readData(int newsockfd, SSL *ssl)
{

	string command = readCommand(newsockfd, ssl);
	if (command == DOWNLOAD)
	{
		cout<<"AOK";
		//downloadData(newsockfd);
	}

	if (command == SHARE)
	{
		//shareData(newsockfd);
		cout<<"AOK";
	}


	if (command == UPLOAD)
	{
		uploadData(newsockfd, ssl);
		cout<<"AOK\n";
	}
	
	if (command == REGISTER)
	{
		registerUser(newsockfd, ssl);
	}
	
	if (command == LOGIN)
	{
		loginUser(newsockfd, ssl);
	}
	
	if (command == DELETE)
	{
		deleteUser(newsockfd, ssl);
	}
	
/*	while ((bytesread = read(newsockfd, &name, sizeof(name) )) != 0)
	{
		if (name == DELIMITER) break;
		fileName << name;

	}


	f.open(fileName.str(),ios::out);

	while ((rv = read(newsockfd, buffer, BUF_SIZE) ) != 0)   
		f.write(buffer, BUF_SIZE);
	

	f.close();*/
	
}


void Server::ReceiveData(int newsockfd, SSL *ssl){

	bool interrupt = false;
	int r, rv;

	pollfd pf;
	pf.fd = newsockfd;
	pf.events = POLLIN;


	do{

	// rv = poll(&pf, 1, -1);
	// if (rv == -1)
	// 	perror("poll"); // error occurred in poll()
	//
	// if (pf.revents && POLLIN)
		readData(newsockfd, ssl);



		printf("%s\n", "Press 1 to continue receiving files");
		scanf("%d",&r);
		if (r == 1) interrupt = true;

	} while (!interrupt);



}


void Server::SendData(int sockfd, SSL *ssl, char* a)
{

	int fd = open(a, O_RDONLY);
	if (fd < 0)
	{
		printf("%s\n","File IO error");
		exit(1);
	}

	sendFilename(a, sockfd, ssl);
	char b = DELIMITER;
	int n = write(sockfd, &b, sizeof(b));
	fileTransfer(fd, sockfd, ssl);

	close(fd);


}



void Server::ChildProcess(int sockfd, int newsockfd, SSL *ssl)
{
	close(sockfd);    
	char* a;
	int opt = 0;
 	while (true){
 		// printf("%s\n","press 1 to send file");
//  		printf("%s\n","press 2 to receive files");
//
//  		while (opt != 0) scanf("%d",&opt);
//
//  		if (opt == 1)
//  		{
//  			printf("%s\n", "Enter file name to send");
//  			scanf("%s",a);
//  			SendData(newsockfd,a);
//  		}
  		//
 		// if (opt == 2)
 		// {
 			printf("%s\n","Receiving Data from client..");
 			ReceiveData(newsockfd, ssl);
 		// }


 	}


 	 
}

SSL_CTX* Server::InitServerCTX()
{
	SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();		/* load & register all cryptos, etc. */
    SSL_load_error_strings();			/* load all error messages */
    method = SSLv2_server_method();		/* create new server-method instance */
    ctx = SSL_CTX_new(method);			/* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}

void Server::LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
	/* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}

void Server::CreateServer(int port)
{
		int sockfd, ret, len, newsockfd;
		char clientAddr[100];
		pid_t childpid;
		SSL_library_init();
		//char* serverAddr = a;
		sockfd = socket(AF_INET, SOCK_STREAM,0); // create a socket
		if (sockfd < 0)
		{
			printf("%s\n", "ERROR");
			exit(1);
		}
		
		SSL_CTX *ctx;
	    ctx = InitServerCTX();								/* initialize SSL */
	    LoadCertificates(ctx, "newreq.pem", "newreq.pem");	/* load certs */
		
		memset(&addr, 0, sizeof(addr));    // adding to server
		addr.sin_family = AF_INET;    
		addr.sin_addr.s_addr = INADDR_ANY;  
		addr.sin_port = port;    

		ret = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));  // connect socket to server
		listen(sockfd, 5);	// backlog queue for socket connection

		while (true)
		{
			 len = sizeof(cl_addr);
			 cout<<"hello\n"; 
			 newsockfd = accept(sockfd, (struct sockaddr *) &cl_addr, (socklen_t*) &len); // create connection with a client

			 if (newsockfd < 0){
			 	printf("Error accepting connection!\n");  
     			exit(1);  
			 }
			 SSL *ssl;
			 inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, 100);
	         ssl = SSL_new(ctx);         					/* get new SSL state with context */
	         SSL_set_fd(ssl, newsockfd);						/* set connection socket to SSL state */
			 if ((childpid = fork() ) == 0){
			    if ( SSL_accept(ssl) == -1 )					/* do SSL-protocol accept */
			         printf("Error accepting SSL connection!\n");
			 	ChildProcess(sockfd, newsockfd, ssl);
			 }

			 close(newsockfd);
		}   
}

// void Server::populate(User& u, const char* path)
// {
// 	dir = opendir(path);
//
// }
//
// void Server::buildSets() {
//     dirent *userEntry;
//     DIR *dir;
//     if ((dir = opendir(".") ) != NULL)     // system call to open directory
// 	{
// 		while ((userEntry = readdir (dir)) != NULL)   //system call to read file name from directory
// 		{
// 			//printf("\n%s",entry->d_name);
// 			if(userEntry->d_type == DT_DIR)
// 			{
// 				string dirName(userEntry->d_name);
// 				UserInfo i;
// 				i.username = dirName;
// 				//TODO set password
// 				User u;
// 				u.info = i;
// 				dirent *files;
// 				DIR *userDir;
// 				char path[256];
// 				strcat(path, "./");
// 				strcat(path, userEntry->d_name);
// 				populate(u, path);
// 			}
// 		}
// 	}
// 	else
// 	{
// 		cout << "Could not open directory.\n";
// 	}
// }


