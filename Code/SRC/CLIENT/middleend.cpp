#include "middleend.h"

bool middleEnd::Register(Client* c, string& username, string& password, string& name)
{
    c->writeCommand("register");
    c->writeCommand(username);
    c->writeCommand(password);
    c->writeCommand(name);
    return c->verifyUser("Successful");
}


bool middleEnd::login(Client* c, string& username, string& password, string& name)
{
    c->writeCommand("login");
    c->writeCommand(username);
    c->writeCommand(password);
    bool valid = c->verifyUser("Valid");
    if(valid)
        name = c->readCommand();
    return valid;
}


void middleEnd::downloadFile(Client* c, string& username, string& fileName, string& version, string& filePath)
{
    c->writeCommand("downloadfile");
    c->writeCommand(username);
    c->writeCommand(fileName);
    c->writeCommand(version);
    cout << filePath <<"\n";
    c->readFromServer(filePath);
}

void middleEnd::downloadFolder(Client* c, string& username, string& folderName, string& version, string& folderPath)
{
    c->writeCommand("downloadfolder");
    c->writeCommand(username);
    c->writeCommand(folderName);
    c->writeCommand(version);
    while(true)
    {
        string pathName = c->readCommand();
        if(pathName == "Done")
            break;
        path p(pathName);
        create_directories(p.parent_path());
        c->readFromServer(pathName);
    }
}

int middleEnd::uploadFile(Client *c, string& username, string& fileName, string& version)
{
    c->writeCommand("uploadfile");
    c->writeCommand(username);
    c->writeCommand(version);
    char *a = c->stringToChar(fileName);
    int fd = open(a, O_RDONLY);
    if (fd < 0)
    {
        printf("%s\n","File IO error");
        exit(1);
    }

    long int pos = lseek(fd, 0L, SEEK_END);
    char sizefile[64];
    sprintf(sizefile, "%ld", pos);
    cout<<sizefile<<"\n";
    close(fd);
    c->writeCommand(a);
    c->writeCommand(sizefile);
    string permission = c->readCommand();
    if(permission == "continue")
    {
        c->writeToServer(a);
        return 1;
    }
    else if(permission == "discontinue")
        return 0;
    else if(permission == "disallowed")
        return 2;

}

int middleEnd::helperUploadFile(Client *c, string fileName, string localPath)
{
    char *a = c->stringToChar(localPath);
    int fd = open(a, O_RDONLY);
    if (fd < 0)
    {
        printf("%s\n","File IO error");
        exit(1);
    }

    long int pos = lseek(fd, 0L, SEEK_END);
    char sizefile[64];
    sprintf(sizefile, "%ld", pos);
    cout<<sizefile<<"\n";
    close(fd);
    c->writeCommand(fileName);
    c->writeCommand(sizefile);
    string permission = c->readCommand();
    if(permission == "continue")
    {
        c->writeToServer(a);
        return 1;
    }
    else if(permission == "discontinue")
        return 0;
    else if(permission == "disallowed")
        return 2;

}

int middleEnd::uploadFolder(Client *c, string& username, path folderName, string pathTillNow)
{
    c->writeCommand("Folder");
    string tempPath = pathTillNow;
    tempPath += folderName.filename().string();
    c->writeCommand(tempPath);
    tempPath += "/";
    int tempWorked = 1;
    for(directory_iterator file(folderName); file != directory_iterator(); file++)
    {
        path current(file->path());
        if(!is_directory(current))
        {
            c->writeCommand("File");
            string filepath = tempPath;
            filepath += current.filename().string();
            cout << filepath <<"\n";
            string localPath = current.string();
            cout << localPath << "\n";
            tempWorked = helperUploadFile(c, filepath, localPath);
            if(tempWorked != 1)
                return tempWorked;
        }
        else
        {
            tempWorked = uploadFolder(c, username, current, tempPath);
            if(tempWorked != 1)
                return tempWorked;
        }
    }
    return tempWorked;
}

void middleEnd::share(Client* c, string& username, vector<string>* files, vector< pair<string, string> >& shareUserList)
{
    c->writeCommand("share");
    c->writeCommand(username);
    cout << (int) files->size() <<"\n";
    for(int i = 0; i < files->size(); i++)
    {
        cout<<files->at(i) << "\n";
        c->writeCommand(files->at(i));
    }
    c->writeCommand("Done");
    for(int i = 0; i < shareUserList.size(); i++)
    {
        cout<<shareUserList[i].first<<"\n";
        cout<<shareUserList[i].second<<"\n";
        c->writeCommand(shareUserList[i].first);
        c->writeCommand(shareUserList[i].second);
    }
    c->writeCommand("Done");
    /* share file between user a and user b */
}

void middleEnd::deleteFile(Client *c, string& username, string& fileName)
{
    c->writeCommand("deletefile");
    c->writeCommand(username);
    c->writeCommand(fileName);
    /* delete file from client c */
}

void middleEnd::deleteUser(Client *c, string& username)
{
    c->writeCommand("deleteuser");
    c->writeCommand(username);
    /* delete user from client c */
}

void middleEnd::searchUsers(Client *c, string& searchTerm, vector<string>& listOfUsers)
{
    c->writeCommand("search");
    c->writeCommand(searchTerm);
    while(true)
    {
        string s = c->readCommand();
        if(s == "Done")
            break;
        listOfUsers.push_back(s);
    }
}

void middleEnd::changePermissions(Client *c, string& owner, string& sharee, string& fileName, string& perm)
{
    c->writeCommand("changepermission");
    c->writeCommand(owner);
    c->writeCommand(sharee);
    c->writeCommand(fileName);
    c->writeCommand(perm);
}

void middleEnd::unshare(Client *c, string& owner, string& sharee, string& fileName)
{
    c->writeCommand("unshare");
    c->writeCommand(owner);
    c->writeCommand(sharee);
    c->writeCommand(fileName);
}

void middleEnd::downloadShared(Client *c, string& user, string& owner, string& fileName)
{
    c->writeCommand("downloadshared");
    c->writeCommand(user);
    c->writeCommand(owner);
    c->writeCommand(fileName);
    string Path = user;
    Path += "/";
    Path += fileName;
    c->readFromServer(Path);
}

void middleEnd::syncFiles(Client *c, string& username)
{
    c->writeCommand("sync");
    c->writeCommand(username);
    while(true)
    {
        string fileName = c->readCommand();
        if(fileName == "Done")
            break;
        string version = c->readCommand();
        string thisVersion;
        if(exists(path(fileName)))
        {
            time_t t = last_write_time(path(fileName));
            cout << t <<"\n";
            thisVersion = currentDateTime(t);
            cout << thisVersion <<"\n";
        }
        else
        {
            if(fileName.back() == '/')
                create_directories(path(fileName));
            else
                create_directories(path(fileName).parent_path());
        }
        if(!exists(path(fileName)) || thisVersion < version)
        {
            c->writeCommand("download");
            if(fileName.back() == '/')
            {
                last_write_time(path(fileName), time(0));
            }
            else
            {
                c->readFromServer(fileName);
            }
        }
        else if(thisVersion > version)
        {
            c->writeCommand("upload");
            if(fileName.back() == '/')
            {
                c->writeCommand(thisVersion);
            }
            else
            {
                c->writeCommand(thisVersion);
                char *a = c->stringToChar(fileName);
                int fd = open(a, O_RDONLY);
                if (fd < 0)
                {
                    printf("%s\n","File IO error");
                    exit(1);
                }

                long int pos = lseek(fd, 0L, SEEK_END);
                char sizefile[64];
                sprintf(sizefile, "%ld", pos);
                cout<<sizefile<<"\n";
                close(fd);
                c->writeCommand(sizefile);
                c->writeToServer(a);
            }
        }
        else
        {
            c->writeCommand("same");
        }
    }
}
