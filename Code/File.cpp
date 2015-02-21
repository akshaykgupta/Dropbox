//TODO File.cpp

#include <iostream>
#include <fstream>
#include <set>
#include "stdlib"

using namespace std;

class File {
private:
	string fileName;
	int versions;
	set<Delta> deltas;
	User *owner;
	set<User*> allowedUsers;
	char* fileContents;
public:
	char* getContents();
	populate(char* contents);
};