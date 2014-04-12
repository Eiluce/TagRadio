#include "queryboxdesign.h"
#include "ui_queryboxdesign.h"

QueryBoxDesign::QueryBoxDesign(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QueryBoxDesign)
{
    ui->setupUi(this);
}

QueryBoxDesign::~QueryBoxDesign()
{
    delete ui;
}
