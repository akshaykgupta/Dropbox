#include "middleEnd.h"

bool middleEnd::Register(string& username, string& password, string& name)
{
	c->writeCommand("register");
	c->writeCommand(username);
	c->writeCommand(password);
	c->writeCommand(name);
	return c->verifyUser("Successful");

}



bool middleEnd::login(string& username, string& password)
{
	c->writeCommand("login");
	c->writeCommand(username);

	c->writeCommand(password);
	return c->verifyUser("Valid");
}


void middleEnd::download(string& username, string& fileName)
{
	c->writeCommand("download");
	
	if (!c->checkIfAllowed(username, fileName))
	{
		printf("%s\n", "Permission Denied");
		exit(1);
	}

	c->readFromServer();

	// make changes to u?
}


void middleEnd::upload(string& username, string& fileName)
{
	c->writeCommand("upload");
	c->writeCommand(username);
	c->writeCommand(fileName);
	// make changes to u?
}


void middleEnd::share(string& fileName, string& uA, string& uB)
{
	
	c->writeCommand("share");
	c->writeCommand(uA);
	c->writeCommand(uB);
	c->writeCommand(fileName);
	/* share file between user a and user b */
}

void middleEnd::deleteFile(string& username, string& fileName)
{
	c->writeCommand("delete");
	c->writeCommand(username);
	c->writeCommand(fileName);
	/* delete file from client c */
}

void middleEnd::deleteUser(string& username, string& fileName)
{
	c->writeCommand("delete");
	c->writeCommand(username);
	/* delete user from client c */
}