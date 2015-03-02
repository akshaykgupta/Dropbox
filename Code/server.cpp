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

void Server::writeCommand(int newsockfd, SSL* ssl, char* a)
{
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

void Server::writeCommand(int newsockfd, SSL* ssl, string& s)
{
	char *a = stringToChar(s);
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
	delete[] a;
}

void Server::getFile(int newsockfd, SSL *ssl, ofstream& f, long int size )
{
	int BUF_SIZE = 10000;
	int r, rv, ret;

	char buffer[BUF_SIZE];
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
					cout<<"read!!!!\n";
					f.write(buffer, size - byteswritten);
					byteswritten += size - byteswritten;
					memset(buffer, 0, BUF_SIZE);
					buffer[0] = '\0';
					strcat(buffer, "discontinue");
					//ret = write(newsockfd, buffer, BUF_SIZE);
					ret = SSL_write(ssl, buffer, BUF_SIZE);
					break;
				}
				//cout << buffer <<" ";
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
	// Handle version control, and change user info
}

void Server::uploadFile(int newsockfd, SSL *ssl)
{
	string user = readCommand(newsockfd, ssl);
	string version = readCommand(newsockfd, ssl);
	string Pathname = readCommand(newsockfd, ssl);
	string sizefile = readCommand(newsockfd, ssl);
	path p(Pathname);
	string Path = user;
	string name = p.filename().string();
	Path += "/";
	Path += name;
	long int size = stol(sizefile);
	UserUtilDb db(user);
	string sharePath;
	string permission;
	db.getFileFromShared(name, sharePath, permission);
	if(sharePath == "")
	{
		int worked = db.addToTableOwned(name, version, "FILE", Path);
		cout<<worked<<"\n";
		if(worked)
		{
			writeCommand(newsockfd, ssl, "continue");
			ofstream f;
			string s = rootDir;
			s += Path;
			s += "_";
			s += version;
			f.open(s, ios::out);
			getFile(newsockfd, ssl, f, size);
		}
		if(!worked)
		{
			writeCommand(newsockfd, ssl, "discontinue");
		}
	}
	else
	{
		if(permission == "r")
		{
			writeCommand(newsockfd, ssl, "disallowed");
		}
		else if(permission == "w")
		{
			ofstream f;
			string s = rootDir;
			s += "/";
			s += sharePath;
			s += "_";
			s += version;
			f.open(s, ios::out);
			getFile(newsockfd, ssl, f, size);
		}
	}	
}

void Server::uploadFolder(int newsockfd, SSL *ssl)
{
	string user = readCommand(newsockfd, ssl);
	string version = readCommand(newsockfd, ssl);
	while(true)
	{
		string type = readCommand(newsockfd, ssl);
		if(type == "Done")
			break;
		else if(type == "File")
		{
			string Pathname = readCommand(newsockfd, ssl);
			string sizefile = readCommand(newsockfd, ssl);
			path p(Pathname);
			string Path = user;
			Path += "/";
			Path += p.string();
			string name = p.filename().string();
			long int size = stol(sizefile);
			UserUtilDb db(user);
			string sharePath;
			string permission;
			db.getFileFromShared(name, sharePath, permission);
			if(sharePath == "")
			{
				int worked = db.addToTableOwned(name, version, "FILE", Path);
				cout<<worked<<"\n";
				if(worked)
				{
					writeCommand(newsockfd, ssl, "continue");
					ofstream f;
					string s = rootDir;
					s += Path;
					s += "_";
					s += version;
					f.open(s, ios::out);
					getFile(newsockfd, ssl, f, size);
				}
				if(!worked)
				{
					writeCommand(newsockfd, ssl, "discontinue");
				}
			}
			else
			{
				if(permission == "r")
				{
					writeCommand(newsockfd, ssl, "disallowed");
				}
				else if(permission == "w")
				{
					ofstream f;
					string s = rootDir;
					s += sharePath;
					s += "_";
					s += version;
					f.open(s, ios::out);
					getFile(newsockfd, ssl, f, size);
				}
			}
		}
		else if(type == "Folder")
		{
			string Pathname = readCommand(newsockfd, ssl);
			string Path = user;
			path p(Pathname);
			Path += "/";
			Path += p.string();
			Path += "/";
			cout << Path << "\n";
			string name = p.filename().string();
			UserUtilDb db(user);
			string sharePath;
			string permission;
			int worked = db.addToTableOwned(name, version, "FOLDER", Path);
			cout<<worked<<"\n";
			if(worked)
			{
				string s = rootDir;
				s += Path;
				if(!exists(s))
					create_directory(s);
			}
			if(!worked)
			{
				writeCommand(newsockfd, ssl, "discontinue");
			}
		}
	}
}

void Server::downloadFile(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	string Path = readCommand(newsockfd, ssl);
	string version = readCommand(newsockfd, ssl);
	string FullPath = rootDir;
	FullPath += Path;
	FullPath += "_";
	FullPath += version;
	const char *a = FullPath.c_str();
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
	writeCommand(newsockfd, ssl, sizefile);
	fileTransfer(fd, newsockfd, ssl);
}

void Server::downloadFolder(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	string folderName = readCommand(newsockfd, ssl);
	string version = readCommand(newsockfd, ssl);
	string Path;
	UserUtilDb userdb(username);
	vector< pair<string, string> > filePaths;
	userdb.getFilesAndPathForFolderPath(folderName, version, filePaths);
	for(int i = 0; i < filePaths.size(); i++)
	{
		string FullPath = rootDir;
		FullPath += filePaths[i].second;
		FullPath += "_";
		FullPath += version;
		const char *a = FullPath.c_str();
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
		writeCommand(newsockfd, ssl, filePaths[i].second);
		writeCommand(newsockfd, ssl, sizefile);
		fileTransfer(fd, newsockfd, ssl);
	}
	writeCommand(newsockfd, ssl, "Done");
}

void Server::shareData(int newsockfd, SSL* ssl)
{
	string username = readCommand(newsockfd, ssl);
	vector<string> files;
	while(true)
	{
		string file = readCommand(newsockfd, ssl);
		if(file == "Done")
			break;
		files.push_back(file);
	}
	while(true)
	{
		string user = readCommand(newsockfd, ssl);
		if(user == "Done")
			break;
		string perm = readCommand(newsockfd, ssl);
		UserUtilDb userdb(user);
		string Path;
		UserUtilDb userdb1(username);
		for(int i = 0; i < files.size(); i++)
		{
			userdb1.getPathGivenFile(files[i], Path);
			string version;
			userdb1.pathToLatestVersion(Path, version);
			userdb1.addToTableOwned(files[i], version, "FILE", Path, user);
			userdb.addToTableShared(files[i], username, perm, "FILE", Path);
		}
	}
}

void Server::unshareData(int newsockfd, SSL *ssl)
{
	string owner = readCommand(newsockfd, ssl);
	string user = readCommand(newsockfd, ssl);
	string file = readCommand(newsockfd, ssl);
	UserUtilDb userdb(user);
	userdb.removeFromShared(file, owner);
	UserUtilDb userdb1(owner);
	string Path;
	userdb1.getPathGivenFile(file, Path);
	userdb.removeSharedFromOwned(user, Path);
}

void Server::downloadSharedFile(int newsockfd, SSL* ssl)
{
	string user = readCommand(newsockfd, ssl);
	string owner = readCommand(newsockfd, ssl);
	string fileName = readCommand(newsockfd, ssl);
	UserUtilDb userdb(owner);
	string Path;
	userdb.getPathGivenFile(fileName, Path);
	string version;
	userdb.pathToLatestVersion(Path, version);
	string fullPath = rootDir;
	fullPath += Path;
	fullPath += "_";
	fullPath += version;
	const char *a = fullPath.c_str();
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
	writeCommand(newsockfd, ssl, sizefile);
	fileTransfer(fd, newsockfd, ssl);
	
}

void Server::registerUser(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	string password = readCommand(newsockfd, ssl);
	string name = readCommand(newsockfd, ssl);
	string localPath = "";
	int worked = 2;
	worked = logindb.addUser(username, name, password, localPath);
	if(worked == 0)
	{
		cout<<"DB error :(\n";
		writeCommand(newsockfd, ssl, "Invalid");
	}
	else if(worked == 1)
		writeCommand(newsockfd, ssl, "Unsuccessful");
	else if(worked == 2)
	{
		UserUtilDb newUserDb(username);
	    int worked1 = newUserDb.createOwned(); //0:NOT CREATED 1:CREATED
	    int worked2 = newUserDb.createShared(); //0:NOT CREATED 1:CREATED
		if(worked1 && worked2)
		{
			string s = rootDir;
			s += username;
			path p(s);
			create_directories(p);
			writeCommand(newsockfd, ssl, "Successful");
		}
		else
		{
			cout<<"DB error :(\n";
			writeCommand(newsockfd, ssl, "Invalid");
		}
	}
}

void Server::loginUser(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	string password = readCommand(newsockfd, ssl);
	int worked = 2;
	string name;
	worked = logindb.passCheck(username, password, name);
	if(worked == 0)
	{
		cout<<"DB error :(\n";
		writeCommand(newsockfd, ssl, "Invalid");
	}
	else if(worked == 1)
		writeCommand(newsockfd, ssl, "Invalid");
	else if(worked == 2)
	{
		UserUtilDb userdb(username);
		vector< pair<string, string> > filesAndVersions;
		userdb.getAllPathandVersions(filesAndVersions);
		writeCommand(newsockfd, ssl, "Valid");
		writeCommand(newsockfd, ssl, name);
		for(int i = 0; i < filesAndVersions.size(); i++)
		{
			writeCommand(newsockfd, ssl, filesAndVersions[i].first);
			writeCommand(newsockfd, ssl, filesAndVersions[i].second);
		}
		writeCommand(newsockfd, ssl, "Done");
	}
}

void Server::deleteUser(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	int worked = logindb.delUser(username);
	string Path = rootDir;
	Path += username;
	remove_all(path(Path));
}

void Server::searchUsers(int newsockfd, SSL* ssl)
{
	string searchTerm = readCommand(newsockfd, ssl);
	vector<string> listOfUsers;
	int worked = logindb.Usersquery(searchTerm, listOfUsers);
	if(worked == 0)
	{
		cout<<"DB error :(\n";
		writeCommand(newsockfd, ssl, "Done");
	}
	else
	{
		for(int i = 0; i < listOfUsers.size(); i++)
		{
			writeCommand(newsockfd, ssl, listOfUsers[i]);
		}
		writeCommand(newsockfd, ssl, "Done");
	}
}

void Server::changePermissions(int newsockfd, SSL* ssl)
{
	string owner = readCommand(newsockfd, ssl);
	string sharee = readCommand(newsockfd, ssl);
	string fileName = readCommand(newsockfd, ssl);
	string perm = readCommand(newsockfd, ssl);
	UserUtilDb userdb(sharee);
	UserUtilDb userdb1(owner);
	string Path;
	userdb1.getPathGivenFile(fileName, Path);
	cout << Path << " ";
	userdb.addToTableShared(fileName, owner, perm, "FILE", Path);
}

void Server::sendSharedFiles(int newsockfd, SSL* ssl)
{
	string user = readCommand(newsockfd, ssl);
	UserUtilDb userdb(user);
	vector<tpl> sharedFiles;
	userdb.getAllPathsShareePermissions(sharedFiles);
	string currentUser = "";
	for(int i = 0; i < sharedFiles.size(); i++)
	{
		cout << sharedFiles[i].first <<"\n";
		cout << sharedFiles[i].second <<"\n";
		cout << sharedFiles[i].third <<"\n";
		if(sharedFiles[i].second != currentUser)
		{
			writeCommand(newsockfd, ssl, "New");
			writeCommand(newsockfd, ssl, sharedFiles[i].second);
			currentUser = sharedFiles[i].second;
			writeCommand(newsockfd, ssl, sharedFiles[i].first);
			writeCommand(newsockfd, ssl, sharedFiles[i].third);
		}
		else
		{
			writeCommand(newsockfd, ssl, sharedFiles[i].first);
			writeCommand(newsockfd, ssl, sharedFiles[i].third);
		}
	}
	writeCommand(newsockfd, ssl, "Done");
	vector<pair<string, string> > shareList;
	userdb.getAllPathsSharedandOwners(shareList);
	currentUser = "";
	for(int i = 0; i < shareList.size(); i++)
	{
		cout << shareList[i].first <<"\n";
		cout << shareList[i].second <<"\n";
		if(shareList[i].second != currentUser)
		{
			writeCommand(newsockfd, ssl, "New");
			writeCommand(newsockfd, ssl, shareList[i].second);
			currentUser = shareList[i].second;
			writeCommand(newsockfd, ssl, sharedFiles[i].first);
		}
		else
			writeCommand(newsockfd, ssl, shareList[i].first);
	}
	writeCommand(newsockfd, ssl, "Done");
}

void Server::syncFiles(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	UserUtilDb userdb(username);
	vector< pair<string, string> > fileNames;
	userdb.getAllPathsAndLatestVersion(fileNames);
	for(int i = 0; i < fileNames.size(); i++)
	{
		cout << fileNames[i].first <<"\n";
		cout << fileNames[i].second << "\n";
		writeCommand(newsockfd, ssl, fileNames[i].first);
		writeCommand(newsockfd, ssl, fileNames[i].second);
		if(fileNames[i].first.back() == '/')
		{
			string change = readCommand(newsockfd, ssl);
			if(change == "upload")
			{
				string version = readCommand(newsockfd, ssl);
				string name = path(fileNames[i].first).filename().string();
				userdb.addToTableOwned(name, version, "FOLDER", fileNames[i].first);
			}
		}
		else
		{
			string change = readCommand(newsockfd, ssl);
			if(change == "upload")
			{
				string version = readCommand(newsockfd, ssl);
				string name = path(fileNames[i].first).filename().string();
				userdb.addToTableOwned(name, version, "FILE", fileNames[i].first);
				string fullPath = rootDir;
				fullPath += fileNames[i].first;
				fullPath += "_";
				fullPath += version;
				ofstream f;
				f.open(fullPath, ios::out);
				string sizefile = readCommand(newsockfd, ssl);
				long int size = stol(sizefile);
				getFile(newsockfd, ssl, f, size);
				
			}
			else if(change == "download")
			{
				string fullPath = rootDir;
				fullPath += fileNames[i].first;
				fullPath += "_";
				fullPath += fileNames[i].second;
				const char *a = fullPath.c_str();
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
				writeCommand(newsockfd, ssl, sizefile);
				fileTransfer(fd, newsockfd, ssl);
			}
		}
	}
	writeCommand(newsockfd, ssl, "Done");
}

void Server::deleteFile(int newsockfd, SSL *ssl)
{
	string username = readCommand(newsockfd, ssl);
	string fileName = readCommand(newsockfd, ssl);
	UserUtilDb userdb(username);
	//userdb.delUser(username);
	string Path = rootDir;
	Path += fileName;
	remove_all(path(Path));
}

bool Server::readData(int newsockfd, SSL *ssl)
{

	string command = readCommand(newsockfd, ssl);
	if (command == DOWNLOADFILE)
	{
		downloadFile(newsockfd, ssl);
	}
	
	else if (command == DOWNLOADFOLDER)
	{
		downloadFolder(newsockfd, ssl);
	}

	else if (command == SHARE)
	{
		shareData(newsockfd, ssl);
	}

	else if(command == UNSHARE)
	{
		unshareData(newsockfd, ssl);
	}

	else if (command == UPLOADFILE)
	{
		uploadFile(newsockfd, ssl);
		
	}
	
	else if (command == UPLOADFOLDER)
	{
		uploadFolder(newsockfd, ssl);
		
	}
	
	else if (command == REGISTER)
	{
		registerUser(newsockfd, ssl);
		
	}
	
	else if (command == LOGIN)
	{
		loginUser(newsockfd, ssl);
	}
	
	else if (command == DELETEFILE)
	{
		deleteFile(newsockfd, ssl);
	}
	
	else if (command == DELETEUSER)
	{
		deleteUser(newsockfd, ssl);
	}
	
	else if(command == SEARCH)
	{
		searchUsers(newsockfd, ssl);
	}
	
	else if(command == CHANGEPERMISSION)
	{
		changePermissions(newsockfd, ssl);
	}
	
	else if(command == OPENSHARE)
	{
		sendSharedFiles(newsockfd, ssl);
	}
	
	else if(command == DOWNLOADSHARED)
	{
		downloadSharedFile(newsockfd, ssl);
	}
	
	else if(command == SYNC)
	{
		syncFiles(newsockfd, ssl);
	}
	
	else if(command == CLOSE)
	{
		return false;
	}
	
	return true;
	
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
	SSL_free(ssl);

}



void Server::ChildProcess(int sockfd, int newsockfd, SSL *ssl)
{
	close(sockfd);    
	char* a;
	int opt = 0;
	logindb.open();
	logindb.create();
	bool keepGoing = true;
 	while (keepGoing){
 			printf("%s\n","Receiving Data from client..");
 			keepGoing = readData(newsockfd, ssl);
 	}


 	 
}

SSL_CTX* Server::InitServerCTX()
{
	SSL_METHOD *method;
    SSL_CTX *ctx;
    OpenSSL_add_all_algorithms();		/* load & register all cryptos, etc. */
    SSL_load_error_strings();			/* load all error messages */
    method = const_cast<SSL_METHOD *>(SSLv3_server_method());		/* create new server-method instance */
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
			 newsockfd = accept(sockfd, (struct sockaddr *) &cl_addr, (socklen_t*) &len); // create connection with a client

			 if (newsockfd < 0) {
     			exit(1);
			}
			else
			{
				SSL *ssl;
				inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, 100);
				ssl = SSL_new(ctx);         					/* get new SSL state with context */
				SSL_set_fd(ssl, newsockfd);						/* set connection socket to SSL state */
				if ((childpid = fork() ) == 0){
					if ( SSL_accept(ssl) == -1 )					/* do SSL-protocol accept */
						printf("Error accepting SSL connection!\n");
					ChildProcess(sockfd, newsockfd, ssl);
				}
			}

			 close(newsockfd);
		}
		close(sockfd);
}

char* Server::stringToChar(string &s)
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


