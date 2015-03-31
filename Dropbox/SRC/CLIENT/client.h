#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <boost/filesystem.hpp>
#define DELIMITER '\0'
using namespace std;
using namespace boost::filesystem;

class Client{

private:

    sockaddr_in addr, cl_addr;
    int sockfd;
    SSL *ssl;
    bool exec,connected;
    void sendFilename(char*);
    void fileTransfer(int);
    void readFile(string&);

    SSL_CTX* InitCTX();
    void ShowCerts();

public:


    Client() {
    exec = true;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        exec = false;
}

    bool connectToServer(const char* serverAddr,int);

    void readFromServer(string&);
    void writeToServer(char* );  // file Name
    void writeCommand(char *); // write a char* to the server
    void writeCommand(string&);
    string readCommand();
    bool verifyUser(string ); // TODO
    bool checkIfAllowed(string&, string&);
    char* stringToChar(string s);
    void closeConnection();

};

#endif // CLIENT_H
