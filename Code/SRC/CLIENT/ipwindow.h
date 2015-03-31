#ifndef IPWINDOW_H
#define IPWINDOW_H

#include <QMainWindow>
#include "loginwindow.h"
#include "client.h"
namespace Ui {
class IPWindow;
}

class IPWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit IPWindow(QWidget *parent = 0);
    ~IPWindow();
    
private slots:
    void on_connect_button_clicked();

private:
    Ui::IPWindow *ui;
    Client c;
    LoginWindow *child;
};

#endif // IPWINDOW_H
