#include "middleEnd.h"

void middleEnd::register(User& u, Client& c)
{

}



bool middleEnd::login(Client& c, string& username, string& password)
{
	c.writeCommand("login");
	c.writeCommand(username);
	c.writeCommand(password);
	//bool valid;
	/*
	valid = c.verifyUser();
	*/
	return c.verifyUser();
}


void middleEnd::download(User& u, Client& c, char* fileName)
{
	if (!c.checkIfAllowed(u, fileName))
	{
		printf("%s\n", "Permission Denied");
		exit(1);
	}

	c.writeCommand("download");
	c.writeCommand(fileName);
	c.readFromServer();

	// make changes to u?
}


void middleEnd::upload(User& u, Client& c, char* fileName)
{
	c.writeCommand("upload");
	c.writeToServer(fileName);
	// make changes to u?
}


void middleEnd::share(User& uA, User& uB, Client& c, char* fileName)
{
	
	c.writeCommand("share");
	c.sendUserInfo(uA);
	c.sendUserInfo(uB);
	c.fileName(fileName);
	/* share file between user a and user b */
}

void middleEnd::delete(User& u,Client& c, char* fileName)
{
	if (!u.hasFile())
	{
		printf("%s\n","File not found");
		exit(1);
	}

	/* delete file from client c */
}