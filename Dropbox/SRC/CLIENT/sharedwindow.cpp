#include "sharedwindow.h"
#include "ui_sharedwindow.h"

SharedWindow::SharedWindow(UserInterface *parWin, Client *cl, middleEnd *mid, string n, string un, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SharedWindow)
{
    ui->setupUi(this);
    parentWindow = parWin;
    c = cl;
    me = mid;
    name = n;
    username = un;
    back = false;
    rootDir = username;
    rootDir += "/";
    c->writeCommand("openshare");
    c->writeCommand(username);
    vector<QString> leftShareParents;
    vector< vector< pair<QString, QString> > > leftShareChildren;
    vector< pair<QString, QString> > leftChildren;
    int numCount = 0;
    while(true)
    {
        string s = c->readCommand();
        if(s == "Done")
            break;
        else if(s == "New")
        {
            s = c->readCommand();
            leftShareParents.push_back(QString::fromStdString(s));
            numCount++;
            cout<<"In User";
            if(numCount > 1)
            {
                leftShareChildren.push_back(leftChildren);
                leftChildren.clear();
            }
            //new parent item widget
        }
        else
        {
            cout <<"In file";
            string permission = c->readCommand();
            leftChildren.push_back(make_pair(QString::fromStdString(path(s).filename().string()), QString::fromStdString(permission)));
            //add child to current parent
        }
    }
    if(numCount >= 1)
    {
        leftShareChildren.push_back(leftChildren);
        leftChildren.clear();
    }
    vector<QString> rightShareParents;
    vector< vector<QString> > rightShareChildren;
    vector<QString> rightChildren;
    numCount = 0;
    cout << "Here\n";
    while(true)
    {
        string s = c->readCommand();
        if(s == "Done")
            break;
        else if(s == "New")
        {
            s = c->readCommand();
            rightShareParents.push_back(QString::fromStdString(s));
            numCount++;
            if(numCount > 1)
            {
                rightShareChildren.push_back(rightChildren);
                rightChildren.clear();
            }
            //new parent item widget
        }
        else
        {
            rightChildren.push_back(QString::fromStdString(path(s).filename().string()));
            //add child to current parent
        }
    }
    cout <<"Here\n";
    if(numCount >= 1)
    {
        rightShareChildren.push_back(rightChildren);
        rightChildren.clear();
    }
    ui->filesSharedWidget->setColumnCount(2);
    cout <<"here\n";
    createTreeWidget(ui->filesSharedWidget, leftShareParents, leftShareChildren);
    cout<<"here\n";
    createTreeWidget(ui->sharedFilesWidget, rightShareParents, rightShareChildren);
    cout <<"here\n";
}

void SharedWindow::closeEvent(QCloseEvent *event)
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

SharedWindow::~SharedWindow()
{
    delete ui;
}

void SharedWindow::createTreeWidget(QTreeWidget* thisWidget, vector<QString> shareParents, vector< vector< pair<QString, QString> > > shareChildren)
{
    for(int i = 0; i < shareParents.size(); i++)
    {
        QTreeWidgetItem* parentItem = new QTreeWidgetItem(thisWidget);
        parentItem->setText(0,shareParents[i]);
        parentItem->setText(1, "");
        for (int j = 0; j < shareChildren[i].size(); j++)
        {
            QTreeWidgetItem* childItem = new QTreeWidgetItem();
            childItem->setText(0, shareChildren[i][j].first);
            childItem->setText(1, shareChildren[i][j].second);
            parentItem->addChild(childItem);
        }
    }
}

void SharedWindow::createTreeWidget(QTreeWidget* thisWidget, vector<QString> shareParents, vector< vector<QString> > shareChildren)
{
    for(int i = 0; i < shareParents.size(); i++)
    {
        QTreeWidgetItem* parentItem = new QTreeWidgetItem(thisWidget);
        parentItem->setText(0,shareParents[i]);
        parentItem->setText(1, "");
        for (int j = 0; j < shareChildren[i].size(); j++)
        {
            QTreeWidgetItem* childItem = new QTreeWidgetItem();
            childItem->setText(0, shareChildren[i][j]);
            parentItem->addChild(childItem);
        }
    }
}


void SharedWindow::on_backButton_clicked()
{
    back = true;
    parentWindow->show();
    this->close();
}

void SharedWindow::on_permissionButton_clicked()
{
    QTreeWidgetItem* selectedFile;
    selectedFile = *(ui->filesSharedWidget->selectedItems().begin());
    string filename = selectedFile->text(0).toUtf8().constData();
    string sharee = selectedFile->parent()->text(0).toUtf8().constData();
    if(selectedFile->childCount() == 0)
    {
        QString perm = QInputDialog::getText(this,"Permissions","Enter permission (r/w)");
        string permission = perm.toUtf8().constData();
        if(permission == "r" || permission == "w")
        {
            me->changePermissions(c, username, sharee, filename, permission);
            selectedFile->setText(1, perm);
        }
    }
}

void SharedWindow::on_unshareButton_clicked()
{
    QTreeWidgetItem* selectedFile;
    selectedFile = *(ui->filesSharedWidget->selectedItems().begin());
    string filename = selectedFile->text(0).toUtf8().constData();
    string sharee = selectedFile->parent()->text(0).toUtf8().constData();
    if(selectedFile->childCount() == 0)
    {
        QString conf = QInputDialog::getText(this,"Unshare","Are you sure? (Yes/No)");
        string confirmation = conf.toUtf8().constData();
        if(confirmation == "Yes" || confirmation == "yes")
        {
            me->unshare(c, username, sharee, filename);
            selectedFile->parent()->removeChild(selectedFile);
            delete selectedFile;
        }
    }
}

void SharedWindow::on_downloadSharedButton_clicked()
{
    QTreeWidgetItem* selectedFile;
    selectedFile = *(ui->sharedFilesWidget->selectedItems().begin());
    string filename = selectedFile->text(0).toUtf8().constData();
    string owner = selectedFile->parent()->text(0).toUtf8().constData();
    me->downloadShared(c, username, owner, filename);
}
