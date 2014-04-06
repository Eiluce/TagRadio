#include "mainwindow.h"
#include "querybox.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QueryBox *b = new QueryBox();
    MainWindow *w = new MainWindow();
    b->show();

    QObject::connect(b, SIGNAL(sendValues(int,int,int,int)), w, SLOT(startWithValues(int,int,int,int)));
    return a.exec();
}
