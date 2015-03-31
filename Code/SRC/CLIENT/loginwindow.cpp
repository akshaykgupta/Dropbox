#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(IPWindow *parWin, Client *cl, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    c = cl;
    parentWindow = parWin;
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::closeEvent(QCloseEvent *event)
{
    c->writeCommand("close");
    c->closeConnection();
    event->accept();
}

void LoginWindow::on_registerButton_clicked()
{
    this->hide();
    child = new Registration(this, c, &me);
    child->show();
}

void LoginWindow::on_loginButton_clicked()
{
    QString name,pass;
    name = ui->uname_edit->text();
    string username = name.toUtf8().constData();
    pass = ui->pass_edit->text();
    string password = pass.toUtf8().constData();
    if(name.isEmpty() || name.isNull() || pass.isEmpty() || pass.isNull())
        ui->label_error->setText("Empty Field");
    string Name;
    bool valid = me.login(c, username, password, Name);
    if(valid)
    {
        this->hide();
        uiWindow = new UserInterface(this, c, &me, Name, username);
        cout<<"hi\n";
        ui->uname_edit->setText("");
        ui->pass_edit->setText("");
        uiWindow->show();
    }
    else
    {
        ui->label_error->setText("Incorrect Username or Password");
    }
}


void LoginWindow::on_backConnectButton_clicked()
{
    parentWindow->show();
    this->close();
}
