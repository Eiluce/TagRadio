#ifndef TABLESIZEBOX_H
#define TABLESIZEBOX_H

#include <QWidget>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QApplication>
#include <QLineEdit>
#include <QPen>
#include <QMessageBox>
#include <QLabel>
#include <QGraphicsItem>

class TableSizeBox : public QWidget
{
    Q_OBJECT
public:
    explicit TableSizeBox(int nbCol, int nbLin, QWidget *parent = 0);

signals:
    void sendTableValues(int p1X, int p1Y, int p2X, int p2Y);
public slots:
    void viewValues();
    void checkAndSendValues();
private:    
    bool checkValues(QString p1X, QString p1Y, QString p2X, QString p2Y);

    QPushButton *p_ok;
    QPushButton *p_quit;
    QPushButton *p_viewB;

    int sizeX;
    int sizeY;

    qreal sizeRectX;
    qreal sizeRectY;

    QLabel *p_instructions;
    QLabel *p_point1;
    QLabel *p_point2;

    QLineEdit *p_p1X;
    QLineEdit *p_p1Y;
    QLineEdit *p_p2X;
    QLineEdit *p_p2Y;

    QGraphicsScene *p_scene;
    QGraphicsView *p_view;

    QGraphicsItem *table;
};

#endif // TABLESIZEBOX_H
