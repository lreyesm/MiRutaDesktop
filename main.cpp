#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("MiRuta");
    a.setOrganizationName("inglreyesm");
    a.setOrganizationDomain("mraguas.com");
    MainWindow w;
    w.show();

    return a.exec();
}
