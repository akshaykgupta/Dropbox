#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QMainWindow>
#include <QCloseEvent>
#include "client.h"
#include "middleend.h"
namespace Ui {
class Registration;
}
class LoginWindow;
class Registration : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Registration(LoginWindow *parWin, Client *cl, middleEnd *mid, QWidget *parent = 0);
    ~Registration();

protected:
    void closeEvent(QCloseEvent *event);
    
private slots:
    void on_regButton_clicked();

    void on_backButton_clicked();

private:
    Ui::Registration *ui;
    Client *c;
    middleEnd *me;
    LoginWindow *parentWindow;
};
#include "loginwindow.h"
#endif // REGISTRATION_H

