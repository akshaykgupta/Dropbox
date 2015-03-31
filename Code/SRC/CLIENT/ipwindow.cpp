#include "ipwindow.h"
#include "ui_ipwindow.h"

IPWindow::IPWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IPWindow)
{
    ui->setupUi(this);
}

IPWindow::~IPWindow()
{
    delete ui;
}

void IPWindow::on_connect_button_clicked()
{
    QString ip = ui->ipaddress->text();
    QString port = ui->portno->text();
    QByteArray ba = ip.toLocal8Bit();
    const char * ipAdd = ba.data();
    int portNo = port.toInt();
    bool succeeded = c.connectToServer(ipAdd, portNo);
    if(succeeded)
    {
        this->hide();
        child = new LoginWindow(this, &c);
        child->show();
    }
    else
    {
        ui->connect_label->setText("Unable to connect. Try Again.");
    }
}
