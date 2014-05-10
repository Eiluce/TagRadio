#include "bluetoothbox.h"

BluetoothBox::BluetoothBox(Reader *reader,QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(400,200);

    p_ok = new QPushButton("Ok", this);
    p_quit = new QPushButton("Annuler", this);
    p_startProgram = new QPushButton("Rechercher appareil");

    this->reader = reader;


    p_ok->setCursor(Qt::PointingHandCursor);
    p_ok->setGeometry(170,170,100,20);

    p_quit->setCursor(Qt::PointingHandCursor);
    p_quit->setGeometry(285,170,100,20);

    p_startProgram->setCursor(Qt::PointingHandCursor);
    p_startProgram->setGeometry(285, 20, 100, 20);


    QObject::connect(p_quit, SIGNAL(clicked()), this, SLOT(closeReader()));
    QObject::connect(p_quit, SIGNAL(clicked()), this, SLOT(close()));
}

void BluetoothBox::closeReader() {
    reader->abort();
}
