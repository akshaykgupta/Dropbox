#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "client.h"
#include "middleend.h"
#include "registration.h"
#include "userinterface.h"

namespace Ui {
class LoginWindow;
}
class IPWindow;
class LoginWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit LoginWindow(IPWindow *parWin, Client *cl, QWidget *parent = 0);
    ~LoginWindow();
    
protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_registerButton_clicked();

    void on_loginButton_clicked();

    void on_backConnectButton_clicked();

private:
    Ui::LoginWindow *ui;
    Client *c;
    middleEnd me;
    Registration *child;
    UserInterface *uiWindow;
    IPWindow *parentWindow;
};
#include "ipwindow.h"
#endif // LOGINWINDOW_H
