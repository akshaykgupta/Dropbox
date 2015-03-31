#ifndef MIDDLEEND_H
#define MIDDLEEND_H
#include "client.h"
#include <vector>
#include <ctime>

class middleEnd{
private:
    //static const int dataBaseId;


public:

    bool Register(Client*, string&, string&, string&); // register a new User u
    bool login(Client*, string&, string&, string&);  // login? for the current user
    void downloadFile(Client*, string&, string&, string&, string&); // download given file for the given user
    void downloadFolder(Client*, string&, string&, string&, string&); // download given file for the given user
    int uploadFile(Client*, string&, string&, string&); // upload current file for the given user
    int helperUploadFile(Client *c, string fileName, string localPath);
    int uploadFolder(Client*, string&, path, string); // upload current file for the given user
    void share(Client*, string&, vector<string>*, vector< pair<string, string> >&); // share current file between the 2 users
    void deleteFile(Client*, string&, string&); // delete current file from the user's database
    void deleteUser(Client*, string&);
    void searchUsers(Client*, string&, vector<string>&);
    void changePermissions(Client*, string&, string&, string&, string&);
    void unshare(Client*, string&, string&, string&);
    void downloadShared(Client*, string&, string&, string&);
    void syncFiles(Client*, string&);

    const std::string currentDateTime(time_t now) {
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
        // for more information about date/time format
        strftime(buf, sizeof(buf), "%Y_%m_%d_%H_%M_%S", &tstruct);
        return buf;
    }
};

#endif // MIDDLEEND_H
