#ifndef INTERMEDIATE
#define INTERMEDIATE 

#include "server.h"
#include "client.h"


/* upload, download, share */

// register, login, download, upload, share, delete, 



class middleEnd{
private:
	
public:
	

	void register();
	void login();
	void download(Client& , char*);
	void upload(Client&, char*);
	void share(Client&, char*);
	void delete(Client&, char*);

};



#endif



