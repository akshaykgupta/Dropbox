#ifndef SHAREWINDOW_H
#define SHAREWINDOW_H

#include <QMainWindow>
#include <boost/filesystem.hpp>
#include <unordered_map>
#include <vector>
#include "client.h"
#include "middleend.h"

namespace Ui {
class ShareWindow;
}
class UserInterface;
class ShareWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ShareWindow(UserInterface *parWin, Client *cl, middleEnd *mid, vector<string>* file, string n = "Barun", string un = "barun", QWidget *parent = 0);
    ~ShareWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_addUserButton_clicked();

    void on_removeUserButton_clicked();

    void on_goBackButton_clicked();

    void on_goButton_clicked();

    void on_shareFilesButton_clicked();

private:
    Ui::ShareWindow *ui;
    UserInterface *parentWindow;

    bool back;
    Client *c;
    middleEnd *me;
    string username;
    string name;
    vector<string> *filename;
    unordered_map<string, string> userPerms;
    string rootDir;
};
#include "userinterface.h"
#endif // SHAREWINDOW_H
