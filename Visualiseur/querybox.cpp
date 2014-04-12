#include "querybox.h"

QueryBox::QueryBox(QWidget *parent) :
    QWidget(parent)
{

    setFixedSize(400,170);

    p_ok = new QPushButton("Lancer", this);
    p_quit = new QPushButton("Quitter", this);
    p_arrayX = new QLineEdit(this);
    p_arrayY = new QLineEdit(this);
    p_tableX = new QLineEdit(this);
    p_tableY = new QLineEdit(this);
    p_layout = new QFormLayout(this);

    p_ok->setToolTip("Lance le programme avec les valeurs spécifiées");
    p_ok->setCursor(Qt::PointingHandCursor);
    p_ok->setGeometry(90,120,100,20);

    p_quit->setToolTip("Quitte le programme");
    p_quit->setCursor(Qt::PointingHandCursor);
    p_quit->setGeometry(195,120,100,20);

    p_arrayX->setMaxLength(4);
    p_arrayY->setMaxLength(4);
    p_tableX->setMaxLength(4);
    p_tableY->setMaxLength(4);

    p_layout->addRow("Nombre de colonnes : ", p_arrayX);
    p_layout->addRow("Nombre de lignes   : ", p_arrayY);
    p_layout->addRow("Largeur table : ", p_tableX);
    p_layout->addRow("Hauteur table : ", p_tableY);

    QObject::connect(p_ok, SIGNAL(clicked()), this, SLOT(checkValues()));
    QObject::connect(p_quit, SIGNAL(clicked()), qApp, SLOT(quit()));
    QObject::connect(this, SIGNAL(sendValues(int,int,int,int)), this, SLOT(close()));

}

void QueryBox::checkValues() {

    QString aX, aY, tX, tY;

    aX = p_arrayX->text();
    aY = p_arrayY->text();
    tX = p_tableX->text();
    tY = p_tableY->text();

    if (aX.isEmpty() || aY.isEmpty() || tX.isEmpty() || tY.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs doivent être remplis.");
        return;
    }
    bool isNumeric = false;
    int arrayX = aX.toInt(&isNumeric);
    if (!isNumeric) {
        QMessageBox::warning(this, "Erreur", "Les champs doivent être entiers.");
        return;
    }
    int arrayY = aY.toInt(&isNumeric);
    if (!isNumeric) {
        QMessageBox::warning(this, "Erreur", "Les champs doivent être entiers.");
        return;
    }
    int tableX = tX.toInt(&isNumeric);
    if (!isNumeric) {
        QMessageBox::warning(this, "Erreur", "Les champs doivent être entiers.");
        return;
    }
    int tableY = tY.toInt(&isNumeric);
    if (!isNumeric) {
        QMessageBox::warning(this, "Erreur", "Les champs doivent être entiers.");
        return;
    }
    emit sendValues(arrayX, arrayY, tableX, tableY);
}
