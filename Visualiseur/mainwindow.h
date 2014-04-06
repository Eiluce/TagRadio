#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPen>
#include <QPixmap>
#include <QImage>
#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QMessageBox>

#include <iostream>

#include <querybox.h>
#include <../matrice_calibration/matriceCalibration.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

public slots:
    void startWithValues(int aX, int aY, int tX, int tY);
    void sendPointProbValues();
    void setPointProb(int posX, int posY, double prob);
    void resetPositions();

signals:
    void pointProbValues(int posX, int posY, double prob);

private:
    QGraphicsView *p_view;
    QGraphicsScene *p_scene;

    QPushButton *p_addPosition;
    QPushButton *p_deletePos;
    QPushButton *p_quit;

    QLineEdit *p_addPosX;
    QLineEdit *p_addPosY;
    QLineEdit *p_addPosP;

    QGraphicsItem *table;

    int numberOfRows;
    int numberOfColumns;
    qreal sizeRectX;
    qreal sizeRectY;
    QList<QGraphicsItem*> listPositions;


};

#endif // MAINWINDOW_H
