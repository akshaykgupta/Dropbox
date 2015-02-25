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

	void Register(Client&, string&, string&); // register a new User u
	bool login(Client&, string&, string&);  // login? for the current user
	void download(User&, Client& , string&); // download given file for the given user
	void upload(Client&, string&); // upload current file for the given user
	void share(User&, User& ,Client&, string&); // share current file between the 2 users
	void Delete(User&, Client&, string&); // delete current file from the user's database

};



#endif



