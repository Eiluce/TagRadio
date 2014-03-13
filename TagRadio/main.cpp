#include "mainwindow.h"
#include "bluetooth.h"
#include <QApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    Bluetooth blue;

    return a.exec();
    //return 0;
}
