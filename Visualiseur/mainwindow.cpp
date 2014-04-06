#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setFixedSize(800,600);
    p_scene = new QGraphicsScene(this);
    p_view = new QGraphicsView(p_scene, this);

    QPixmap background(QCoreApplication::applicationDirPath() +
                                   "/img/sol.jpg");
    p_scene->setBackgroundBrush(QBrush(background));
    p_view->setGeometry(10,10,510,510);

    p_addPosition = new QPushButton("Ajouter position", this);
    p_addPosition->setGeometry(590,90, 120, 30);

    p_deletePos = new QPushButton("Réinitialiser positions", this);
    p_deletePos->setGeometry(590,130, 120, 30);

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

    if (posX > numberOfColumns) {
        QMessageBox::warning(this, "Erreur", "L'abscisse entrée est trop grande.");
        return;
    }

    if (posY > numberOfRows) {
        QMessageBox::warning(this, "Erreur", "L'ordonnée entrée est trop grande.");
        return;
    }

    if (prob < 0.0 || prob > 1.0) {
        QMessageBox::warning(this, "Erreur", "La probabilité choisie doit être comprise entre 0 et 1.");
        return;
    }

    QBrush brush(Qt::red);
    if (prob >= 0.9) {
        brush.setStyle(Qt::SolidPattern);
    } else if (prob >= 0.8) {
        brush.setStyle(Qt::Dense1Pattern);
    } else if (prob >= 0.7) {
        brush.setStyle(Qt::Dense2Pattern);
    } else if (prob >= 0.6) {
        brush.setStyle( Qt::Dense3Pattern);
    } else if (prob >= 0.5) {
        brush.setStyle(Qt::Dense4Pattern);
    } else if (prob >= 0.4) {
        brush.setStyle(Qt::Dense5Pattern);
    } else if (prob >= 0.2) {
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

}



