#include "userinterface.h"
#include "ui_userinterface.h"

UserInterface::UserInterface(LoginWindow *parWin, Client *cl, middleEnd *mid, string n, string un, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UserInterface)
{
    ui->setupUi(this);
    parentWindow = parWin;
    c = cl;
    me = mid;
    name = n;
    username = un;
    string message = "Welcome ";
    message += name;
    message += "!";
    QString welcomeMessage = QString::fromStdString(message);
    ui->welcome_label->setText(welcomeMessage);
    logout = false;
    model = new QDirModel(this);
    model->setReadOnly(false);
    model->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);
    ui->clientDirView->setModel(model);
    QModelIndex index = model->index("/home/btech/cs1130773");
    ui->clientDirView->setRootIndex(index);
    ui->clientDirView->expand(index);
    ui->clientDirView->scrollTo(index);
    ui->clientDirView->setCurrentIndex(index);
    ui->clientDirView->resizeColumnToContents(0);
    string prev = "";
    numFiles = 0;
    numFolders = 0;
    QString parentDirectory;
    QStringList allFiles;
    QString u = QString::fromStdString(un);
    parentDirectory = u;
    while(true)
    {
        string s = c->readCommand();
        if(s == "Done")
            break;
        if(s != prev)
        {
            if(s.back() == '/')
            {
                cout << "HI\n";
                foldersAndVersions.resize(numFolders+1);
                u = QString::fromStdString(s);
                prev = s;
                s.pop_back();
                folders.push_back(s);
                folderHash[s] = numFolders;
                string t = c->readCommand();
                foldersAndVersions[numFolders].push_back(t);
                numFolders++;
            }
            else
            {
                filesAndVersions.resize(numFiles+1);
                files.push_back(s);
                u = QString::fromStdString(s);
                allFiles << u;
                fileHash[s] = numFiles;
                string t = c->readCommand();
                filesAndVersions[numFiles].push_back(t);
                numFiles++;
                prev = s;
            }
        }
        else
        {
            if(s.back() == '/')
            {
                string t = c->readCommand();
                foldersAndVersions[numFolders-1].push_back(t);
            }
            else
            {
                string t = c->readCommand();
                filesAndVersions[numFiles-1].push_back(t);
            }
        }
    }
    create_QT_Tree_widget(parentDirectory, allFiles);
    rootDir = username;
    rootDir += "/";
}

UserInterface::~UserInterface()
{
    delete ui;
}

void UserInterface::create_QT_Tree_widget(QString &parentDirectory, QStringList &allFiles)
{
    root = new QTreeWidgetItem(ui->serverDirWidget);
    root->setText(0, parentDirectory);
    foreach(QString currentFile, allFiles)
    {

        QStringList pathDecomp = currentFile.split("/");
        QTreeWidgetItem *parentItem = root;
        foreach(QString pathIterator, pathDecomp)
        {
            if(pathIterator != parentDirectory && !pathIterator.isEmpty())
            {
                bool found = false;
                for(int i = 0; i < parentItem->childCount(); i++ )
                {
                    if(parentItem->child(i)->text(0) == pathIterator)
                    {
                        found = true;
                        parentItem = parentItem->child(i);
                        break;
                    }
                }
                if(found == false)
                {
                    QTreeWidgetItem *itm = new QTreeWidgetItem();
                    itm->setText(0, pathIterator);
                    parentItem->addChild(itm);
                    parentItem = itm;
                }
            }
        }

    }
}


void UserInterface::addroot(QString name,const std::vector<QString> &children)
{
    root = new QTreeWidgetItem(ui->serverDirWidget);
    root->setText(0,name);
    ui->serverDirWidget->addTopLevelItem(root);
    for(int i=0;i<children.size();i++)
        addchild(root,children[i]);
}

void UserInterface::addchild(QTreeWidgetItem *parent, QString name)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem();
    itm->setText(0,name);
    parent->addChild(itm);
}


void UserInterface::closeEvent(QCloseEvent *event)
{
    if(logout)
    {
        parentWindow->show();
        event->accept();
    }
    else
    {
        c->writeCommand("close");
        event->accept();
    }
}

void UserInterface::on_uploadButton_clicked()
{
    QString file = model->filePath(this->ui->clientDirView->currentIndex());
    string s = file.toUtf8().constData();
    path p(s);
    string m = rootDir;
    m += p.filename().string();
    if(file.isEmpty() || file.isNull())
        return;
    cout<<s<<"\n";
    if(s == "-1")
        return;
    int worked=0;
    if(!is_directory(p))
    {
         time_t now = time(0);
         string version = me->currentDateTime(now);
         worked = me->uploadFile(c, username, s, version);
         if(fileHash.find(m) == fileHash.end())
         {
             fileHash[m] = filesAndVersions.size();
             vector<string> t;
             t.push_back(version);
             filesAndVersions.push_back(t);
         }
         else
         {
             filesAndVersions[fileHash[m]].insert(filesAndVersions[fileHash[m]].begin(), version);
         }
    }
    else
    {
         c->writeCommand("uploadfolder");
         c->writeCommand(username);
         time_t now = time(0);
         string version = me->currentDateTime(now);
         c->writeCommand(version);
         string l = "";
         worked = me->uploadFolder(c, username, p, l);
         c->writeCommand("Done");
         if(folderHash.find(m) == folderHash.end())
         {
             folderHash[m] = foldersAndVersions.size();
             vector<string> t;
             t.push_back(version);
             foldersAndVersions.push_back(t);
         }
         else
         {
             foldersAndVersions[folderHash[m]].insert(foldersAndVersions[folderHash[m]].begin(), version);
         }
    }
    if(worked == 1)
    {
        bool found = false;
        for(int i = 0; i < root->childCount(); i++ )
        {
            if(root->child(i)->text(0) == QString::fromStdString(p.filename().string()))
                found = true;
        }
        if(!found)
        {
            if(!is_directory(p))
                WidgetAddFile(QString::fromStdString(p.filename().string()));
            else
                WidgetAddDir(root,p);
        }
    }
    else if(worked == 2)
    {
       cout<<"Not\n";
    }
    else
    {
       cout<<"not done\n";
    }
}

void UserInterface::on_downloadButton_clicked()
{
    QModelIndex index = ui->serverDirWidget->currentIndex();
    QString currentVersion = ui->historyBox->currentText();
    QList<QTreeWidgetItem*> selList = ui->serverDirWidget->selectedItems();
    QTreeWidgetItem *selectedWidget = *(selList.begin());
    string ver = currentVersion.toUtf8().constData();
    QString file = ui->serverDirWidget->model()->data(index).toString();
    string fileName = file.toUtf8().constData();
    string fullPath = fileName;
    QTreeWidgetItem *parent = selectedWidget->parent();
    while(parent != NULL)
    {
        string parentPath = (parent->text(0)).toUtf8().constData();
        parentPath += "/";
        fullPath.insert(0, parentPath);
        parent = parent->parent();
    }
    if(currentVersion.isEmpty() || currentVersion.isNull() || file.isEmpty() || file.isNull())
    {
        ui->displayBoard->setText("Select a version from the drop-down menu.");
        ui->historyBox->clear();
        if(selectedWidget->childCount() == 0)
        {
            int ind = fileHash[fullPath];
            for(int i = 0; i < filesAndVersions[ind].size(); i++)
            {
                QString u = QString::fromStdString(filesAndVersions[ind][i]);
                ui->historyBox->addItem(u);
            }
        }
        else
        {
            int ind = folderHash[fullPath];
            for(int i = 0; i < foldersAndVersions[ind].size(); i++)
            {
                QString u = QString::fromStdString(foldersAndVersions[ind][i]);
                ui->historyBox->addItem(u);
            }
        }
    }
    else if(fileName != prevSelectedFile)
    {
        ui->displayBoard->setText("Select a version from the drop-down menu.");
        ui->historyBox->clear();
        if(selectedWidget->childCount() == 0)
        {
            int ind = fileHash[fullPath];
            cout << filesAndVersions[ind].size() <<"\n";
            for(int i = 0; i < filesAndVersions[ind].size(); i++)
            {
                QString u = QString::fromStdString(filesAndVersions[ind][i]);
                ui->historyBox->addItem(u);
            }
        }
        else
        {
            int ind = folderHash[fullPath];
            for(int i = 0; i < foldersAndVersions[ind].size(); i++)
            {
                QString u = QString::fromStdString(foldersAndVersions[ind][i]);
                ui->historyBox->addItem(u);
            }
        }
    }
    else if(fileName == prevSelectedFile)
    {
        ui->displayBoard->clear();
        if(selectedWidget->childCount() == 0)
        {
            cout<<fileName <<"\n";
            string filePath = fullPath;
            me->downloadFile(c, username, fullPath, ver, filePath);
        }
        else
        {
            string filePath = fullPath;
            me->downloadFolder(c, username, fullPath, ver, filePath);
        }
    }
    prevVersion = ver;
    prevSelectedFile = fileName;
}


void UserInterface::on_logoutButton_clicked()
{
    logout = true;
    this->close();
}


void UserInterface::on_makeDirButton_clicked()
{
    QModelIndex index = ui->clientDirView->currentIndex();
    if(!index.isValid()) return;
    QString name = QInputDialog::getText(this,"Name","Enter a name");
    if(name.isEmpty()) return;
    model->mkdir(index,name);
}


void UserInterface::on_delDirButton_clicked()
{
    QModelIndex index = ui->clientDirView->currentIndex();
    if(!index.isValid()) return;
    if(model->fileInfo(index).isDir())
    {
        model->rmdir(index);
    }
    else
    {
        model->remove(index);
    }
}


void UserInterface::on_historyButton_clicked()
{
    ui->historyBox->clear();
    QModelIndex index = ui->serverDirWidget->currentIndex();
    QString currentVersion = ui->historyBox->currentText();
    QList<QTreeWidgetItem*> selList = ui->serverDirWidget->selectedItems();
    QTreeWidgetItem *selectedWidget = *(selList.begin());
    string ver = currentVersion.toUtf8().constData();
    QString file = ui->serverDirWidget->model()->data(index).toString();
    string fileName = file.toUtf8().constData();
    string fullPath = fileName;
    QTreeWidgetItem *parent = selectedWidget->parent();
    while(parent != NULL)
    {
        string parentPath = (parent->text(0)).toUtf8().constData();
        parentPath += "/";
        fullPath.insert(0, parentPath);
        parent = parent->parent();
    }
    if(selectedWidget->childCount() == 0)
    {
        int ind = fileHash[fullPath];
        for(int i = 0; i < filesAndVersions[ind].size(); i++)
        {
            QString u = QString::fromStdString(filesAndVersions[ind][i]);
            ui->historyBox->addItem(u);
        }
    }
    else
    {
        int ind = folderHash[fullPath];
        for(int i = 0; i < foldersAndVersions[ind].size(); i++)
        {
            QString u = QString::fromStdString(foldersAndVersions[ind][i]);
            ui->historyBox->addItem(u);
        }
    }
}

void UserInterface::on_addButton_clicked()
{
    QModelIndex index = ui->serverDirWidget->currentIndex();
    QString f = ui->serverDirWidget->model()->data(index).toString();
    string file = f.toUtf8().constData();
    cout<<file<<"\n";
    QList<QTreeWidgetItem*> selList = ui->serverDirWidget->selectedItems();
    QTreeWidgetItem *selectedWidget = *(selList.begin());
    if(selectedWidget->childCount() == 0)
    {
        if(fileSet.insert(file).second == true)
        {
            ui->shareBox->addItem(f);
            cout<<"Reached in here\n";
        }
    }
}


void UserInterface::on_removeButton_clicked()
{
    QModelIndex index = ui->serverDirWidget->currentIndex();
    QString f = ui->serverDirWidget->model()->data(index).toString();
    string file = f.toUtf8().constData();
    if(fileSet.erase(file) == 1)
    {
        ui->shareBox->removeItem(ui->shareBox->findText(f));
        cout<<"Deleted\n";
    }
}

void UserInterface::on_shareButton_clicked()
{
    if(fileSet.empty())
    {
        return;
    }
    for(unordered_set<string>::iterator it = fileSet.begin(); it != fileSet.end(); it++)
    {
        selectedFiles.push_back(*it);
    }
    child = new ShareWindow(this, c, me, &selectedFiles, name, username);
    fileSet.clear();
    ui->shareBox->clear();
    this->hide();
    child->show();
}
void UserInterface::WidgetAddFile(QString fileName)
{

        QTreeWidgetItem *itm = new QTreeWidgetItem();
        itm->setText(0,fileName);
        root->addChild(itm);
}

void UserInterface::WidgetAddDir(QTreeWidgetItem *parent, path src)
{

    QTreeWidgetItem * itm = new QTreeWidgetItem();
    itm->setText(0,QString::fromStdString(src.filename().string()));
    parent->addChild(itm);
    for(directory_iterator file(src);file != directory_iterator();file++)
    {
        path curr(file->path());
        if(! is_directory(curr))
             addchild(itm,QString::fromStdString(curr.filename().string()));
        else
        {
            WidgetAddDir(itm,curr);
        }
     }
}

void UserInterface::on_clearButton_clicked()
{
    fileSet.clear();
    ui->shareBox->clear();
}

void UserInterface::on_sharedButton_clicked()
{
    otherchild = new SharedWindow(this, c, me, name, username);
    this->hide();
    otherchild->show();
}

void UserInterface::on_syncButton_clicked()
{
    me->syncFiles(c, username);
}

void UserInterface::on_deleteButton_clicked()
{
    ui->historyBox->clear();
    QModelIndex index = ui->serverDirWidget->currentIndex();
    QString currentVersion = ui->historyBox->currentText();
    QList<QTreeWidgetItem*> selList = ui->serverDirWidget->selectedItems();
    QTreeWidgetItem *selectedWidget = *(selList.begin());
    string ver = currentVersion.toUtf8().constData();
    QString file = ui->serverDirWidget->model()->data(index).toString();
    string fileName = file.toUtf8().constData();
    string fullPath = fileName;
    QTreeWidgetItem *parent = selectedWidget->parent();
    while(parent != NULL)
    {
        string parentPath = (parent->text(0)).toUtf8().constData();
        parentPath += "/";
        fullPath.insert(0, parentPath);
        parent = parent->parent();
    }
    me->deleteFile(c, username, fullPath);
    delete selectedWidget;
}

void UserInterface::on_deleteAccountButton_clicked()
{
    QString perm = QInputDialog::getText(this,"Account Deletion","Are you completely sure? (Yes/No)");
    string permission = perm.toUtf8().constData();
    if(permission == "Yes" || permission == "yes")
    {
        me->deleteUser(c, username);
        parentWindow->show();
        logout = true;
        this->close();
    }
}
