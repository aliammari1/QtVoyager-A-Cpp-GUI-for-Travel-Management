#include "voyage.h"


Voyage::Voyage()
    : flightref(""), lieudep(""), lieuarr(""), airline(""), datedep(QDate::currentDate()), datearr(QDate::currentDate()), montant(0.0), nbper(0)
{
}


Voyage::Voyage(QString flightref, QString lieudep, QString lieuarr, QDate datedep, QDate datearr, QString airline, float montant, int nbper)
    : flightref(flightref), lieudep(lieudep), lieuarr(lieuarr), airline(airline), datedep(datedep), datearr(datearr), montant(montant), nbper(nbper)
{
}


bool Voyage::ajouter() const
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


bool Voyage::supprimer(QString ref) const
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


bool Voyage::update(QString ref) const
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


bool Voyage::searchByFlightRef(QString ref) const
{
    QSqlQuery query;
    query.prepare("SELECT * FROM VOYAGES WHERE FLIGHTREF= '" + ref + "'");
    return query.exec();
}

QList<Voyage> Voyage::getAllVoyages() const
{
    QList<Voyage> voyages;
    QSqlQuery query("SELECT * FROM VOYAGES");
    while (query.next())
    {
        voyages.append(Voyage(query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), query.value(3).toDate(), query.value(4).toDate(), query.value(5).toString(), query.value(6).toFloat(), query.value(7).toInt()));
    }
    return voyages;
}

QList<Voyage> Voyage::getAllVoyagesSorted(QString order) const
{
    QList<Voyage> voyages;
    QSqlQuery query;
    QString sqlQuery = "SELECT * FROM VOYAGES ORDER BY " + order;
    query.prepare(sqlQuery);
    query.exec();
    while (query.next())
    {
        voyages.append(Voyage(query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), query.value(3).toDate(), query.value(4).toDate(), query.value(5).toString(), query.value(6).toFloat(), query.value(7).toInt()));
    }
    return voyages;
}

QList<Voyage> Voyage::searchVoyages(QString recher) const
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
            Q.append(Voyage(query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), query.value(3).toDate(), query.value(4).toDate(), query.value(5).toString(), query.value(6).toFloat(), query.value(7).toInt()));
        }
    }
    return Q;
}

QList<int> Voyage::getVoyageCount(QString s,QList<QString>& v) const
{
    QList<int> chartData;
    QSqlQuery query;
    QSqlQuery query1;
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


float Voyage::calculateAverageCost(QString Sdep, QString Sarr) const
{
    QSqlQuery query;
    query.prepare("SELECT AVG(MONTANT) FROM VOYAGES WHERE LIEUDEP = :dep AND LIEUARR = :arr");
    query.bindValue(":dep", Sdep);
    query.bindValue(":arr", Sarr);
    query.exec();
    return query.first() ? query.value(0).toFloat() : 0;
}
