#include "voyage.h"

//------------------------------constructeur voyage sans arguments---------------------------------------
Voyage::Voyage()
    : flightref(""), lieudep(""), lieuarr(""), airline(""), datedep(QDate::currentDate()), datearr(QDate::currentDate()), montant(0.0), nbper(0)
{
}

//-------------------------------constructeur voyage avec des arguments----------------------------------
Voyage::Voyage(QString flightref, QString lieudep, QString lieuarr, QDate datedep, QDate datearr, QString airline, float montant, int nbper)
    : flightref(flightref), lieudep(lieudep), lieuarr(lieuarr), airline(airline), datedep(datedep), datearr(datearr), montant(montant), nbper(nbper)
{
}

//---------------------------create voyage----------------------------------
bool Voyage::ajouter()
{

    QSqlQuery query;
    QString m = QString::number(montant);
    QString n = QString::number(nbper);
    query.prepare(" INSERT INTO VOYAGES (FLIGHTREF, LIEUDEP, LIEUARR, DATEDEP, DATEARR, AIRLINE, MONTANT, NBPER) "
                  " VALUES (:flightref, :lieudep, :lieuarr, :datedep, :datearr, :airline, :montant, :nbper) ");
    query.bindValue(":flightref", flightref);
    query.bindValue(":lieudep", lieudep);
    query.bindValue(":lieuarr", lieuarr);
    query.bindValue(":datedep", datedep);
    query.bindValue(":datearr", datearr);
    query.bindValue(":airline", airline);
    query.bindValue(":montant", m);
    query.bindValue(":nbper", n);
    return query.exec();
}

//---------------------------voyage delete-----------------------------------
bool Voyage::supprimer(QString ref)
{
    QSqlQuery query;
    bool t = searchByFlightRef(ref);
    if (t)
    {
        query.prepare("DELETE FROM VOYAGES WHERE FLIGHTREF= :ref");
        query.bindValue(":ref", ref);
        query.exec();
    }
    return t;
}

//---------------------------voyage update-----------------------------------
bool Voyage::update(QString ref)
{
    QSqlQuery query;
    bool t = searchByFlightRef(ref);
    if (t)
    {
        query.prepare("UPDATE VOYAGES SET LIEUDEP= :lieudep ,LIEUARR= :lieuarr ,DATEDEP= :datedep ,DATEARR= :datearr ,AIRLINE= :airline ,MONTANT= :montant ,NBPER= :nbper WHERE FLIGHTREF= :ref");
        query.bindValue(":ref", ref);
        query.bindValue(":lieudep", lieudep);
        query.bindValue(":lieuarr", lieuarr);
        query.bindValue(":datedep", datedep);
        query.bindValue(":datearr", datearr);
        query.bindValue(":airline", airline);
        query.bindValue(":montant", QString::number(montant));
        query.bindValue(":nbper", QString::number(nbper));
        query.exec();
    }
    return t;
}

//----------------------------------voyage search---------------------------------------
bool Voyage::searchByFlightRef(QString ref)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM VOYAGES WHERE FLIGHTREF= '" + ref + "'");
    query.exec();
    bool t = query.first();
    if (t)
    {
        lieudep = query.value(1).toString();
        lieuarr = query.value(2).toString();
        datedep = query.value(3).toDate();
        datearr = query.value(4).toDate();
        airline = query.value(5).toString();
        montant = query.value(6).toFloat();
        nbper = query.value(7).toInt();
    }
    return t;
}

QList<Voyage> Voyage::getAllVoyages()
{
    QList<Voyage> voyages;
    QSqlQuery query("SELECT * FROM VOYAGES");
    while (query.next())
    {
        QString flightref = query.value(0).toString();
        QString lieudep = query.value(1).toString();
        QString lieuarr = query.value(2).toString();
        QDate datedep = query.value(3).toDate();
        QDate datearr = query.value(4).toDate();
        QString airline = query.value(5).toString();
        float montant = query.value(6).toFloat();
        int nbper = query.value(7).toInt();
        voyages.append(Voyage(flightref, lieudep, lieuarr, datedep, datearr, airline, montant, nbper));
    }
    return voyages;
}

QList<Voyage> Voyage::getAllVoyagesSorted(QString order)
{
    QList<Voyage> voyages;
    QSqlQuery query;
    QString sqlQuery = "SELECT * FROM VOYAGES ORDER BY " + order;
    query.prepare(sqlQuery);
    query.exec();
    while (query.next())
    {
        QString flightref = query.value(0).toString();
        QString lieudep = query.value(1).toString();
        QString lieuarr = query.value(2).toString();
        QDate datedep = query.value(3).toDate();
        QDate datearr = query.value(4).toDate();
        QString airline = query.value(5).toString();
        float montant = query.value(6).toFloat();
        int nbper = query.value(7).toInt();
        voyages.append(Voyage(flightref, lieudep, lieuarr, datedep, datearr, airline, montant, nbper));
    }
    return voyages;
}

QList<Voyage> Voyage::searchVoyages(QString recher)
{
    QList<Voyage> Q;
    QSqlQuery query;
    QString sqlQuery = "SELECT * FROM VOYAGES WHERE FLIGHTREF LIKE :recher OR LIEUDEP LIKE :recher OR LIEUARR LIKE :recher OR DATEARR LIKE :recher OR DATEDEP LIKE :recher OR AIRLINE LIKE :recher OR MONTANT LIKE :recher OR NBPER LIKE :recher";
    query.prepare(sqlQuery);
    query.bindValue(":recher", "%" + recher + "%");
    if (query.exec())
    {
        while (query.next())
        {
            QString flightref = query.value(0).toString();
            QString lieudep = query.value(1).toString();
            QString lieuarr = query.value(2).toString();
            QDate datedep = query.value(3).toDate();
            QDate datearr = query.value(4).toDate();
            QString airline = query.value(5).toString();
            float montant = query.value(6).toFloat();
            int nbper = query.value(7).toInt();
            Q.append(Voyage(flightref, lieudep, lieuarr, datedep, datearr, airline, montant, nbper));
        }
    }
    return Q;
}

QList<int> Voyage::getVoyageCount(QString s,QList<QString>& v)
{
    QList<int> chartData;
    QSqlQuery query, query1;
    QString condition = "";
    query.prepare("SELECT " + s + "  FROM VOYAGES");
    query.exec();
    while(query.next())
    {
        qDebug() << query.value(0).toString();
        condition = query.value(0).toString();
        v.append(condition);
        query1.prepare("SELECT COUNT(" + s + ")  FROM VOYAGES WHERE " + s + " = :condition");
        query1.bindValue(":condition", condition);
        query1.exec();
        query1.first();
        chartData.append(query1.value(0).toInt());
    }
    qDebug() << chartData;
    return chartData;
}

/*
QSqlQueryModel* Voyage::sort(QString s)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM VOYAGES ORDER BY " + s);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("flightref"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("lieudep"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("lieuarr"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("datedep"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("datearr"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("airline"));
    model->setHeaderData(6, Qt::Horizontal, QObject::tr("montant"));
    model->setHeaderData(7, Qt::Horizontal, QObject::tr("nbper"));
    return model;
}
*/

float Voyage::calculateAverageCost(QString Sdep, QString Sarr)
{
    QSqlQuery query;
    query.prepare("SELECT AVG(MONTANT) FROM VOYAGES WHERE LIEUDEP = :dep AND LIEUARR = :arr");
    query.bindValue(":dep", Sdep);
    query.bindValue(":arr", Sarr);
    query.exec();
    return query.first() ? query.value(0).toFloat() : 0;
}
