#include <iostream>
#include <stdlib.h>
#include "File.cpp"
using namespace std;

// Personal Info of User 

struct UserInfo {
	string username;  
	string password;
	int uid;
	// TODO personal info
	int nameToId (string& username);
};

class User {
private:
	
	UserInfo info;
	vector<File> files;
	
public:
	void download(char* contents);
	void upload(string fileName, File& file);
};