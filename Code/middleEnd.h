#ifndef INTERMEDIATE
#define INTERMEDIATE 

#include "server.h"
#include "client.h"


/* upload, download, share */

// register, login, download, upload, share, delete, 



class middleEnd{
private:
	//static const int dataBaseId;


public:

	void Register(string&, string&, string&); // register a new User u
	bool login(string&, string&);  // login? for the current user
	void download(string&, string&); // download given file for the given user
	void upload(string&, string&); // upload current file for the given user
	void share(string&, string&, string&); // share current file between the 2 users
	void deleteFile(string&, string&); // delete current file from the user's database
	void deleteUser(string&);
};



#endif



