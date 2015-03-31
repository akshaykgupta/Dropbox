#include "sharewindow.h"
#include "ui_sharewindow.h"

ShareWindow::ShareWindow(UserInterface *parWin, Client *cl, middleEnd *mid, vector<string>* file, string n, string un, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ShareWindow)
{
    ui->setupUi(this);
    parentWindow = parWin;
    c = cl;
    me = mid;
    filename = file;
    name = n;
    username = un;
    for(int i = 0;  i < file->size(); i++)
    {
        ui->fileWidget->addItem(QString::fromStdString(file->at(i)));
    }
    back = false;
    rootDir = username;
    rootDir += "/";
}

ShareWindow::~ShareWindow()
{
    delete ui;
}

void ShareWindow::closeEvent(QCloseEvent *event)
{
    if(back)
    {
        event->accept();
    }
    else
    {
        c->writeCommand("close");
        event->accept();
    }
}

void ShareWindow::on_addUserButton_clicked()
{
    QList<QListWidgetItem *> userList;
    userList = ui->userWidget->selectedItems();
    QString perm = QInputDialog::getText(this,"Permissions","Enter permission (r/w)");
    string permission = perm.toUtf8().constData();
    if(permission != "r" && permission != "w")
    {
            ui->errorLabel->setText("Invalid input.");
            return;
    }
    else
    {
            ui->errorLabel->setText("");
    }

    for(QList<QListWidgetItem *>::Iterator it = userList.begin(); it != userList.end(); it++)
    {
        QString u = (*it)->text();
        string us = u.toUtf8().constData();
        userPerms[us] = permission;
        ui->shareUserBox->addItem(u);
    }
}

void ShareWindow::on_removeUserButton_clicked()
{
    QList<QListWidgetItem *> userList;
    userList = ui->userWidget->selectedItems();
    for(QList<QListWidgetItem *>::iterator it = userList.begin(); it != userList.end(); it++)
    {
        QString rem = (*it)->text();
        ui->shareUserBox->removeItem(ui->shareUserBox->findText(rem));
        userPerms.erase((*it)->text().toUtf8().constData());
    }
}

void ShareWindow::on_goBackButton_clicked()
{
    back = true;
    filename->clear();
    parentWindow->show();
    this->close();
}

void ShareWindow::on_goButton_clicked()
{
    QString ent = ui->searchEdit->text();
    string searchTerm = ent.toUtf8().constData();
    vector<string> listOfUsers;
    me->searchUsers(c, searchTerm, listOfUsers);
    ui->userWidget->clear();
    for(int i = 0; i < listOfUsers.size(); i++)
    {
        ui->userWidget->addItem(QString::fromStdString(listOfUsers[i]));

    }
}

void ShareWindow::on_shareFilesButton_clicked()
{
    typedef unordered_map<string, string>::iterator umIt;
    vector< pair<string, string> > shareUserList;
    for(umIt it = userPerms.begin(); it != userPerms.end(); it++)
    {
        shareUserList.push_back(*it);
        cout<<(*it).first << "\n" << (*it).second <<"\n";
    }
    me->share(c, username, filename, shareUserList);
    userPerms.clear();
    shareUserList.clear();
}

