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
#include <QTextStream>
#include <QFileDialog>
#include <QLabel>

#include <iostream>

#include <querybox.h>
#include <tablesizebox.h>
#include <reader.h>
#include <bluetoothbox.h>
#include <../matrice_calibration/matriceCalibration.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void startWithValues(int aX, int aY, int tX, int tY);
    void sendPointProbValues();
    void setPointProb(int posX, int posY, double prob);
    void resetPositions();
    void readMatrixFromFile();
    void changeTableSize();
    void modifyTableSize(int p1X, int p1Y, int p2X, int p2Y);
    void pauseRead();
    void stopRead();
    void fileFinished();
    void setDate(QString date);
    void clickNextDate();

signals:
    void pointProbValues(int posX, int posY, double prob);
    void goToNextDate();

private:
    QThread *thread;
    Reader *reader;

    QGraphicsView *p_view;
    QGraphicsScene *p_scene;

    QPushButton *p_addPosition;
    QPushButton *p_deletePos;
    QPushButton *p_openFile;
    QPushButton *p_changeTableSize;
    QPushButton *p_quit;
    QPushButton *stopButton;
    QPushButton *pauseButton;
    QPushButton *stepButton;

    QLabel *currentDate;

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
