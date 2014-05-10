#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    thread = new QThread();
    reader = new Reader();
    blueBox = new BluetoothBox(reader);

    reader->moveToThread(thread);
    connect(reader, SIGNAL(sendPoint(int,int,double)),
            this, SLOT(setPointProb(int,int,double)));
    connect(reader, SIGNAL(workRequested()), thread, SLOT(start()));
    connect(thread, SIGNAL(started()), reader, SLOT(doWork()));
    connect(reader, SIGNAL(finished()),
            thread, SLOT(quit()) , Qt::DirectConnection);
    connect(reader, SIGNAL(finished()),
            this, SLOT(fileFinished()));
    connect(reader, SIGNAL(sendReset()), this, SLOT(resetPositions()));

    setFixedSize(800,600);
    p_scene = new QGraphicsScene(this);
    p_view = new QGraphicsView(p_scene, this);

    QPixmap background(QCoreApplication::applicationDirPath() +
                                   "/img/sol.jpg");

    p_scene->setBackgroundBrush(QBrush(background));
    p_view->setGeometry(10,10,505,505);

    p_addPosition = new QPushButton("Ajouter position", this);
    p_addPosition->setGeometry(590,90, 120, 30);

    p_deletePos = new QPushButton("Réinitialiser positions", this);
    p_deletePos->setGeometry(590,130, 120, 30);

    p_openFile = new QPushButton("Ouvrir fichier", this);
    p_openFile->setGeometry(590, 170, 120, 30);

    p_changeTableSize = new QPushButton("Changer taille table", this);
    p_changeTableSize->setGeometry(50, 560, 120, 30);

    p_openBluetooth = new QPushButton("Connexion à l'appareil", this);
    p_openBluetooth->setGeometry(590, 210, 120, 30);

    pauseButton = new QPushButton("", this);
    pauseButton->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/img/pause.png"));
    pauseButton->setGeometry(590, 250, 30, 30);
    pauseButton->setDisabled(true);

    stopButton = new QPushButton("", this);
    stopButton->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/img/stop.png"));
    stopButton->setGeometry(630, 250, 30, 30);
    stopButton->setDisabled(true);

    p_quit = new QPushButton("Quitter", this);
    p_quit->setGeometry(600, 560, 100, 30);

    p_addPosX = new QLineEdit(this);
    p_addPosY = new QLineEdit(this);
    p_addPosP = new QLineEdit(this);

    p_addPosX->setGeometry(580, 50, 40, 30);
    p_addPosY->setGeometry(630, 50, 40, 30);
    p_addPosP->setGeometry(680, 50, 40, 30);

    p_addPosX->setPlaceholderText("Abscisse");
    p_addPosY->setPlaceholderText("Ordonnée");
    p_addPosP->setPlaceholderText("Proba");


    QObject::connect(p_addPosition, SIGNAL(clicked()),
                     this, SLOT(sendPointProbValues()));
    QObject::connect(this, SIGNAL(pointProbValues(int,int,double)),
                     this, SLOT(setPointProb(int,int,double)));
    QObject::connect(p_deletePos, SIGNAL(clicked()),
                     this, SLOT(resetPositions()));
    QObject::connect(p_quit, SIGNAL(clicked()),
                     qApp, SLOT(quit()));
    QObject::connect(p_openFile, SIGNAL(clicked()),
                     this, SLOT(readMatrixFromFile()));
    QObject::connect(p_openBluetooth, SIGNAL(clicked()),
                     this, SLOT(readMatrixFromConnection()));
    QObject::connect(p_changeTableSize, SIGNAL(clicked()),
                     this, SLOT(changeTableSize()));
    QObject::connect(pauseButton, SIGNAL(clicked()),
                     this, SLOT(pauseRead()));
    QObject::connect(stopButton, SIGNAL(clicked()),
                     this, SLOT(stopRead()));


}


void MainWindow::startWithValues(int aX, int aY, int tX, int tY) {
    int i,j;
    QPen linesPen(Qt::blue);
    sizeRectX = 500/aX;
    sizeRectY = 500/aY;
    numberOfRows = aY;
    numberOfColumns = aX;

    qreal sizeOfTableX = tX * sizeRectX;
    qreal sizeOfTableY = tY * sizeRectY;
    for (i=1; i < aX; i++) {
        p_scene->addLine(sizeRectX*i, 0, sizeRectX*i, 500, linesPen);
    }

    for (j=1; j < aY; j++) {
        p_scene->addLine(0, sizeRectY*j, 500, sizeRectY*j, linesPen);
    }
    QPixmap woodPix(QCoreApplication::applicationDirPath() +
                                   "/img/table.jpg");
    table = p_scene->addRect( (aX/2 - tX/2) * sizeRectX, (aY/2 - tY/2) * sizeRectY,
                      sizeOfTableX, sizeOfTableY, QPen(), QBrush(woodPix));

    this->show();

}

void MainWindow::sendPointProbValues() {
    bool ok;
    QString posXT = p_addPosX->text();
    QString posYT = p_addPosY->text();
    QString posPT = p_addPosP->text();
    if (posXT.isEmpty() || posYT.isEmpty() || posPT.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs doivent être remplis.");
        return;
    }
    int posX = posXT.toInt(&ok) - 1;
    if (!ok) {
        QMessageBox::warning(this, "Erreur", "Les champs X et Y doivent être entiers.");
        return;
    }
    int posY = posYT.toInt(&ok) - 1;
    if (!ok) {
        QMessageBox::warning(this, "Erreur", "Les champs X et Y doivent être entiers.");
        return;
    }
    double posP = posPT.toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Erreur", "Le champ P doit contenir un nombre.");
        return;
    }

    emit pointProbValues(posX, posY, posP);
}


void MainWindow::setPointProb(int posX, int posY, double prob){

    if (posX > numberOfColumns || posX < 0) {
        QMessageBox::warning(this, "Erreur", "L'abscisse entrée est hors de la grille.");
        return;
    }

    if (posY > numberOfRows || posY < 0) {
        QMessageBox::warning(this, "Erreur", "L'ordonnée entrée est hors de la grille.");
        return;
    }

    if (prob < 0.0 || prob > 1.0) {
        QMessageBox::warning(this, "Erreur", "La probabilité choisie doit être comprise entre 0 et 1.");
        return;
    }

    QBrush brush(Qt::red);
    if (prob >= 0.8) {
        brush.setStyle(Qt::SolidPattern);
    } else if (prob >= 0.6) {
        brush.setStyle(Qt::Dense1Pattern);
    } else if (prob >= 0.5) {
        brush.setStyle(Qt::Dense2Pattern);
    } else if (prob >= 0.4) {
        brush.setStyle( Qt::Dense3Pattern);
    } else if (prob >= 0.3) {
        brush.setStyle(Qt::Dense4Pattern);
    } else if (prob >= 0.2) {
        brush.setStyle(Qt::Dense5Pattern);
    } else if (prob >= 0.1) {
        brush.setStyle(Qt::Dense6Pattern);
    } else {
        brush.setStyle(Qt::Dense7Pattern);
    }
    QGraphicsItem *itemAtPos = p_scene->itemAt(posX * sizeRectX + 1, posY * sizeRectY + 1, QTransform());
    if (itemAtPos != 0 && itemAtPos != table) {
        listPositions.removeOne(itemAtPos);
        p_scene->removeItem(itemAtPos);
    }
    if (prob != 0.0) {
        listPositions.push_back(p_scene->addRect(posX * sizeRectX, posY * sizeRectY,
                                                 sizeRectX, sizeRectY, QPen(Qt::blue), brush));
    }

}

void MainWindow::resetPositions() {
    for (QList<QGraphicsItem*>::iterator iter = listPositions.begin();
         iter != listPositions.end(); ++iter) {
         p_scene->removeItem(*iter);
    }
    listPositions.clear();

}

void MainWindow::readMatrixFromFile() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Ouvrir"), "", tr("Files(*.txt)"));
    if (filename == 0) {
        return;
    }
    reader->abort();
    thread->wait();
    reader->setFilename(filename);
    p_openFile->setDisabled(true);
    p_openBluetooth->setDisabled(true);
    pauseButton->setDisabled(false);
    stopButton->setDisabled(false);
    reader->requestWork(false);

}

void MainWindow::readMatrixFromConnection() {
    reader->abort();
    thread->wait();
    blueBox->show();
    p_openFile->setDisabled(true);
    p_openBluetooth->setDisabled(true);
    pauseButton->setDisabled(false);
    stopButton->setDisabled(false);

}

MainWindow::~MainWindow() {
    reader->abort();
    thread->wait();
    delete thread;
    delete reader;
}


void MainWindow::changeTableSize() {
    TableSizeBox *tableBox = new TableSizeBox(numberOfColumns, numberOfRows);
    tableBox->show();
    QObject::connect(tableBox, SIGNAL(sendTableValues(int,int,int,int)),
                     this, SLOT(modifyTableSize(int,int,int,int)));
}

void MainWindow::modifyTableSize(int p1X, int p1Y, int p2X, int p2Y) {
    p_scene->removeItem(table);
    QPixmap woodPix(QCoreApplication::applicationDirPath() +
                                   "/img/table.jpg");
    int topX = qMin(p1X, p2X);
    int topY = qMin(p1Y, p2Y);
    int sizeX = (qMax(p1X, p2X) - topX + 1) * sizeRectX;
    int sizeY = (qMax(p1Y, p2Y) - topY + 1) * sizeRectY;
    table = p_scene->addRect( (topX - 1) * sizeRectX, (topY - 1) * sizeRectY,
                      sizeX, sizeY, QPen(), QBrush(woodPix));
}

void MainWindow::pauseRead() {
    reader->pauseWork(pauseButton);
}

void MainWindow::stopRead() {
    reader->abort();
}

void MainWindow::fileFinished() {
    pauseButton->setDisabled(true);
    stopButton->setDisabled(true);
    p_openFile->setDisabled(false);
    p_openBluetooth->setDisabled(false);

}

