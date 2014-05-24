#include "tablesizebox.h"

TableSizeBox::TableSizeBox(int nbCol, int nbLin, QWidget *parent) :
    QWidget(parent)
{

    sizeX = nbCol;
    sizeY = nbLin;

    setFixedSize(400,400);

    p_scene = new QGraphicsScene(this);
    p_view = new QGraphicsView(p_scene, this);

    table = 0;

    p_instructions = new QLabel(this);
    p_instructions->setText("Entrez les coordonnées de deux coins opposés");
    p_instructions->setGeometry(75,10,300,30);

    p_point1= new QLabel(this);
    p_point1->setText("Coin 1");
    p_point1->setGeometry(275, 50, 100, 30);

    p_point2 = new QLabel(this);
    p_point2->setText("Coin 2");
    p_point2->setGeometry(275, 120, 100, 30);

    p_p1X = new QLineEdit(this);
    p_p1Y = new QLineEdit(this);
    p_p2X = new QLineEdit(this);
    p_p2Y = new QLineEdit(this);

    p_p1X->setGeometry(275, 85, 40, 30);
    p_p1Y->setGeometry(325, 85, 40, 30);
    p_p2X->setGeometry(275, 155, 40, 30);
    p_p2Y->setGeometry(325, 155, 40, 30);

    p_p1X->setPlaceholderText("Abs");
    p_p1Y->setPlaceholderText("Ord");
    p_p2X->setPlaceholderText("Abs");
    p_p2Y->setPlaceholderText("Ord");


    p_ok = new QPushButton("Valider", this);
    p_quit = new QPushButton("Annuler", this);
    p_viewB = new QPushButton("Aperçu",this);

    p_ok->setToolTip("Envoie les valeurs choisies");
    p_ok->setCursor(Qt::PointingHandCursor);
    p_ok->setGeometry(50,360,100,20);

    p_viewB->setToolTip("Affiche un aperçu des valeurs choisies");
    p_viewB->setCursor(Qt::PointingHandCursor);
    p_viewB->setGeometry(275, 200, 80, 30);

    p_quit->setToolTip("Annule l'opération");
    p_quit->setCursor(Qt::PointingHandCursor);
    p_quit->setGeometry(155,360,100,20);

    QPixmap background(QCoreApplication::applicationDirPath() +
                                   "/img/sol.png");
    if (background.isNull()) {
       background = QPixmap(QCoreApplication::applicationDirPath() +
                                    "/img/sol.jpg");
    }

    p_scene->setBackgroundBrush(QBrush(background));
    p_view->setGeometry(10,50,255,255);

    sizeRectX = 250/nbCol;
    sizeRectY = 250/nbLin;


    int i,j;

    QPen linesPen(Qt::blue);

    for (i=1; i < nbCol; i++) {
        p_scene->addLine(sizeRectX*i, 0, sizeRectX*i, 250, linesPen);
    }

    for (j=1; j < nbLin; j++) {
        p_scene->addLine(0, sizeRectY*j, 250, sizeRectY*j, linesPen);
    }


    QObject::connect(p_quit, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(p_viewB, SIGNAL(clicked()),
                     this, SLOT(viewValues()));
    QObject::connect(p_p2Y, SIGNAL(returnPressed()),
                     this, SLOT(viewValues()));
    QObject::connect(p_ok, SIGNAL(clicked()),
                     this, SLOT(checkAndSendValues()));

}

bool TableSizeBox::checkValues(QString p1X, QString p1Y, QString p2X, QString p2Y) {
    bool ok;

    if (p1X.isEmpty() || p1Y.isEmpty() ||
        p2X.isEmpty() || p2Y.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs doivent être remplis.");
        return false;
    }
    int pos1X = p1X.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Erreur", "Les champs doivent être entiers");
        return false;
    }
    int pos1Y = p1Y.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Erreur", "Les champs doivent être entiers.");
        return false;
    }
    int pos2X = p2X.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Erreur", "Les champs doivent être entiers.");
        return false;
    }
    int pos2Y = p2Y.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Erreur", "Les champs doivent être entiers.");
        return false;
    }
    if (qMax(pos1X,pos2X) > sizeX || qMin(pos1X, pos2X) < 1
        || qMax(pos1Y, pos2Y) > sizeY || qMin(pos1Y, pos2Y) < 1) {
        QMessageBox::warning(this, "Erreur",
                             "Les valeurs entrées doivent être comprises dans la grille.");
        return false;
    }
    return true;
}

void TableSizeBox::viewValues() {



    QString p1X = p_p1X->text();
    QString p1Y = p_p1Y->text();
    QString p2X = p_p2X->text();
    QString p2Y = p_p2Y->text();

    if (!checkValues(p1X, p1Y, p2X, p2Y)) {
        return;
    }

    int pos1X = p1X.toInt(), pos1Y = p1Y.toInt(),
            pos2X = p2X.toInt(), pos2Y = p2Y.toInt();

    QPixmap woodPix(QCoreApplication::applicationDirPath() +
                                   "/img/table.png");
    if (woodPix.isNull()) {
        woodPix = QPixmap(QCoreApplication::applicationDirPath() +
                                    "/img/table.jpg");
    }

    if (table != 0) {
        p_scene->removeItem(table);
    }
    int topX = qMin(pos1X, pos2X);
    int topY = qMin(pos1Y, pos2Y);
    int sizeX = (qMax(pos1X, pos2X) - topX + 1) * sizeRectX;
    int sizeY = (qMax(pos1Y, pos2Y) - topY + 1) * sizeRectY;

    table = p_scene->addRect( (topX - 1) * sizeRectX, (topY - 1) * sizeRectY,
                      sizeX, sizeY, QPen(), QBrush(woodPix));

}

void TableSizeBox::checkAndSendValues() {
    QString p1X = p_p1X->text();
    QString p1Y = p_p1Y->text();
    QString p2X = p_p2X->text();
    QString p2Y = p_p2Y->text();

    if (!checkValues(p1X, p1Y, p2X, p2Y)) {
        return;
    }

    emit sendTableValues(p1X.toInt(), p1Y.toInt(), p2X.toInt(), p2Y.toInt());
    this->close();
}

