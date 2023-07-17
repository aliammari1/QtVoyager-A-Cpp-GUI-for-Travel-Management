#ifndef COMBOADDITEM_H
#define COMBOADDITEM_H

#include <QWidget>

namespace Ui {
class comboAddItem;
}

class comboAddItem : public QWidget
{
    Q_OBJECT

public:
    explicit comboAddItem(QWidget *parent = nullptr);
    ~comboAddItem();

private:
    Ui::comboAddItem *ui;
};

#endif // COMBOADDITEM_H
