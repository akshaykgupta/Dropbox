#include "registration.h"
#include "ui_registration.h"

Registration::Registration(LoginWindow *parWin, Client *cl, middleEnd *mid, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Registration)
{
    ui->setupUi(this);
    c = cl;
    me = mid;
    parentWindow = parWin;
    ui->backButton->hide();
}

Registration::~Registration()
{
    delete ui;
}

void Registration::closeEvent(QCloseEvent *event)
{
    c->writeCommand("close");
    event->accept();
}

void Registration::on_regButton_clicked()
{
    QString uname = ui->username_reg->text();
    string username = uname.toUtf8().constData();
    QString pass = ui->password_reg->text();
    string password = pass.toUtf8().constData();
    QString name = ui->name_reg->text();
    string Name = name.toUtf8().constData();
    QString confPass = ui->confpass_reg->text();
    string conf = confPass.toUtf8().constData();
    if(uname.isEmpty() || uname.isNull() || pass.isEmpty() || pass.isNull())
        ui->reg_error_label->setText("Field empty");
    if(name.isEmpty() || name.isNull() || confPass.isEmpty() || confPass.isNull())
        ui->reg_error_label->setText("Field empty");
    else if(pass.length() < 6)
        ui->reg_error_label->setText("Password must be at least 6 letters long");
    else if(pass != confPass)
        ui->reg_error_label->setText("Passwords do not match");
    else {
        QRegExp digitMatch("[A-Za-z0-9]*[0-9][A-Za-z0-9]*");
        QRegExp lowerCaseLetterMatch("[A-Za-z0-9]*[a-z][A-Za-z0-9]*");
        QRegExp upperCaseLetterMatch("[A-Za-z0-9]*[A-Z][A-Za-z0-9]*");
        if(digitMatch.exactMatch(pass) && lowerCaseLetterMatch.exactMatch(pass) && upperCaseLetterMatch.exactMatch(pass))
        {
            bool valid = me->Register(c, username, password, Name);
            if(!valid)
                ui->reg_error_label->setText("Username already taken");
            else
            {
                ui->reg_error_label->setText("Successful Login");
                string homePath = username;
                create_directory(path(homePath));
                ui->regButton->hide();
                ui->backButton->show();
            }
        }
        else
            ui->reg_error_label->setText("Password must contain a digit, a lowercase and an uppercase letter.");
    }

}

void Registration::on_backButton_clicked()
{
    this->hide();
    parentWindow->show();
}
