#include "ipwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IPWindow w;
    w.show();
    
    return a.exec();
}
