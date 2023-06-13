#ifndef gestionVoyage_H
#define gestionVoyage_H

#include <QMainWindow>
#include <QLabel>
#include <QtDebug>
#include <QLineEdit>
#include "voyage.h"
#include <QGridLayout>
#include "arduino.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class gestionVoyage;
}
QT_END_NAMESPACE

class gestionVoyage : public QMainWindow
{
    Q_OBJECT
public:
    gestionVoyage(QWidget *parent = nullptr);
    ~gestionVoyage();

    void sorter(QString s);

    void charts();

    void style();

    void controle();

    void rotate();

    QList<QLabel *> HeadBar();

    void table();

    void controleur_grid(int i, int j, QLineEdit *l, QList<Voyage> V);

    void rechercher();
    void profit();
    void graphics();
private slots:

    void on_pushButton_ajouter_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_update_clicked();

    void on_pushButton_Search_clicked();

    void on_pushButton_3_clicked();

    void on_comboBox_sort_currentTextChanged(const QString &arg1);

    void on_pushButton_clear_clicked();

    void on_lineEdit_rechercher_textChanged(const QString &arg1);

    void on_comboBox_lieu_From_currentTextChanged(const QString &arg1);

    void on_comboBox_lieu_To_currentTextChanged(const QString &arg1);

    void on_dateEdit_datedep_userDateChanged(const QDate &date);

    void update_label();

private:
    Ui::gestionVoyage *ui;
    Voyage voy;
    QByteArray data;
    Arduino A;
    float lineargradx = 1;
    float lineargrady = 1;
};

#endif // gestionVoyage_H
