#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QDirModel>
#include <QTreeWidget>
#include <QInputDialog>
#include <vector>
#include <QTreeWidgetItem>
#include <unordered_map>
#include <unordered_set>
#include <QStringList>
#include "client.h"
#include "middleend.h"
#include "sharewindow.h"
#include "sharedwindow.h"

namespace Ui {
class UserInterface;
}

class LoginWindow;
class UserInterface : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit UserInterface(LoginWindow *parWin, Client *cl, middleEnd *mid, string n = "Barun", string un = "barun", QWidget *parent = 0);
    ~UserInterface();
    
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_uploadButton_clicked();


    void on_downloadButton_clicked();


    void on_logoutButton_clicked();

    void on_makeDirButton_clicked();

    void on_delDirButton_clicked();

    void on_historyButton_clicked();

    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_shareButton_clicked();

    void on_sharedButton_clicked();

    void on_clearButton_clicked();

    void on_syncButton_clicked();

    void on_deleteButton_clicked();

    void on_deleteAccountButton_clicked();

private:

    Ui::UserInterface *ui;
    QDirModel *model;
    LoginWindow *parentWindow;
    ShareWindow *child;
    SharedWindow *otherchild;
    QTreeWidgetItem *root;
    string rootDir;
    bool logout;
    vector< vector<string> > filesAndVersions;
    unordered_map<string, int> fileHash;
    int numFiles;
    vector<string> files;
    vector< vector<string> > foldersAndVersions;
    unordered_map<string, int> folderHash;
    int numFolders;
    vector<string> folders;
    vector<string> selectedFiles;
    Client *c;
    middleEnd *me;
    string name;
    string username;
    unordered_set<string> fileSet;
    string prevVersion;
    string prevSelectedFile;
    void WidgetAddFile(QString fileName);
    void WidgetAddDir(QTreeWidgetItem* parent,path curr);
    void addroot(QString name, const std::vector<QString> &children);
    void addchild(QTreeWidgetItem *parent ,QString name);
    void create_QT_Tree_widget(QString &parentDirectory, QStringList &allFiles);

};
#include "loginwindow.h"
#endif // USERINTERFACE_H

