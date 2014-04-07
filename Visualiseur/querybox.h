#ifndef QUERYBOX_H
#define QUERYBOX_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QApplication>
#include <QFormLayout>
#include <QMessageBox>

class QueryBox : public QWidget
{
    Q_OBJECT
public:
    explicit QueryBox(QWidget *parent = 0);

signals:
    void sendValues(int arrayX, int arrayY, int tableX, int tableY);
public slots:
    void checkValues();

private:
    QPushButton *p_ok;
    QPushButton *p_quit;
    QFormLayout *p_layout;
    QLineEdit *p_arrayX;
    QLineEdit *p_arrayY;
    QLineEdit *p_tableX;
    QLineEdit *p_tableY;

};

#endif // QUERYBOX_H
