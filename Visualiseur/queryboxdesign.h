#ifndef QUERYBOXDESIGN_H
#define QUERYBOXDESIGN_H

#include <QDialog>

namespace Ui {
class QueryBoxDesign;
}

class QueryBoxDesign : public QDialog
{
    Q_OBJECT

public:
    explicit QueryBoxDesign(QWidget *parent = 0);
    ~QueryBoxDesign();

private:
    Ui::QueryBoxDesign *ui;
};

#endif // QUERYBOXDESIGN_H
