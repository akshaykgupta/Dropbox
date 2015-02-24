/*
Rough buildSets()

Things to add to server.h :

HEADERS
#include <dirent.h>    

DECLARATIONS
private:
	set<User*> Users;

Things to add to User.cpp and File.cpp:
Setters for User and File class. Right now I'm adding stuff assuming data is public.

*/

void server::buildSets() {
    struct dirent *userEntry;
    DIR *dir;
    if ((dir = opendir(".") != NULL))     // system call to open directory
	{
		while ((userEntry = readdir (dir)) != NULL)   //system call to read file name from directory
		{
			//printf("\n%s",entry->d_name);
			if(userEntry.d_type == DT_DIR)
			{
				string dirName(userEntry.d_name);
				UserInfo i;
				i.username = dirName;
				//TODO set password
				User u;
				u.info = i;
				struct dirent *files;
				DIR *userDir;
				char path[256];
				strcat(path, "./");
				strcat(path, userEntry.d_name);
				//In a similar way users directory will be opened and file names will be read.
			}
		}
	}
	else
	{
		cout << "Could not open directory.\n";
	}
}