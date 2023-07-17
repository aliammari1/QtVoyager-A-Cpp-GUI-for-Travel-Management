#include "comboadditem.h"
#include "ui_comboadditem.h"

comboAddItem::comboAddItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::comboAddItem)
{
    ui->setupUi(this);
}

comboAddItem::~comboAddItem()
{
    delete ui;
}
