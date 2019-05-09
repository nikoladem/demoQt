#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QMessageBox>

#include "machine.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Machine mchn;

    MainWindow w(&mchn);
    w.show();

    if (!mchn.loadParameters())
    {
        QString str("Couldn't open frequency file in current dir: " + QDir::currentPath());
        QMessageBox::warning(&w, "Warning", str);
        return 2;   // ERROR_FILE_NOT_FOUND
    }

    return a.exec();
}
