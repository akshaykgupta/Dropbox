#include "middleEnd.h"

bool middleEnd::Register(Client& c, string& username, string& password)
{
	c.writeCommand("register");
	c.writeCommand(username);
	c.writeCommand(password);
	return c.verifyUser("Successful");

}



bool middleEnd::login(Client& c, string& username, string& password)
{
	c.writeCommand("login");
	c.writeCommand(username);

	c.writeCommand(password);
	return c.verifyUser("Valid");
}


void middleEnd::download(string& username, Client& c, string& fileName)
{
	c.writeCommand("download");
	
	if (!c.checkIfAllowed(username, fileName))
	{
		printf("%s\n", "Permission Denied");
		exit(1);
	}

	c.readFromServer();

	// make changes to u?
}


void middleEnd::upload(Client& c, string& fileName)
{
	c.writeCommand("upload");
	c.writeToServer(fileName);
	// make changes to u?
}


void middleEnd::share(Client& c, string& fileName, string& uA, string& uB)
{
	
	c.writeCommand("share");
	c.writeCommand(uA);
	c.writeCommand(uB);
	c.writeCommand(fileName);
	/* share file between user a and user b */
}

void middleEnd::Delete(Client& c, string& fileName)
{
	c.writeCommand("delete");
	c.writeCommand(fileName);

	/* delete file from client c */
}