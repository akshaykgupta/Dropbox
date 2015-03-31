#ifndef SHAREDWINDOW_H
#define SHAREDWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <boost/filesystem.hpp>
#include <unordered_map>
#include <vector>
#include "client.h"
#include "middleend.h"


namespace Ui {
class SharedWindow;
}
class UserInterface;
class SharedWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit SharedWindow(UserInterface *parWin, Client *cl, middleEnd *mid, string n = "Barun", string un = "barun", QWidget *parent = 0);
    ~SharedWindow();
    
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_backButton_clicked();

    void on_permissionButton_clicked();

    void on_unshareButton_clicked();

    void on_downloadSharedButton_clicked();

private:
    Ui::SharedWindow *ui;
    UserInterface *parentWindow;

    bool back;
    Client *c;
    middleEnd *me;
    string username;
    string name;
    string rootDir;

    void createTreeWidget(QTreeWidget* thisWidget, vector<QString> shareParents, vector< vector< pair<QString, QString> > > shareChildren);
    void createTreeWidget(QTreeWidget* thisWidget, vector<QString> shareParents, vector< vector<QString> > shareChildren);

};
#include "userinterface.h"
#endif // SHAREDWINDOW_H
