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

	void register(User&, Client&); // register a new User u
	bool login(Client&, string&, string&);  // login? for the current user
	void download(User&, Client& , char*); // download given file for the given user
	void upload(User&, Client&, char*); // upload current file for the given user
	void share(User&, User&, Client&, char*); // share current file between the 2 users
	void delete(User&, Client&, char*); // delete current file from the user's database

};



#endif



