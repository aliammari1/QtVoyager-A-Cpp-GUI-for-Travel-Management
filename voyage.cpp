#include "voyage.h"

//------------------------------constructeur voyage sans arguments---------------------------------------
Voyage::Voyage()
{
    flightref = "";
    lieudep = "";
    lieuarr = "";
    airline = "";
    datedep = QDate::currentDate();
    datearr = QDate::currentDate();
    montant = 0.0;
    nbper = 0;
}
//-------------------------------constructeur voyage avec des arguments----------------------------------
Voyage::Voyage(QString flightref, QString lieudep, QString lieuarr, QDate datedep, QDate datearr, QString airline, float montant, int nbper)
{
    this->flightref = flightref;
    this->lieudep = lieudep;
    this->lieuarr = lieuarr;
    this->airline = airline;
    this->datedep = datedep;
    this->datearr = datearr;
    this->montant = montant;
    this->nbper = nbper;
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
//--------------------------------voyage showing all table cells--------------------------------------
/*
QSqlQueryModel * Voyage::afficher()
{
    QSqlQueryModel * model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM VOYAGES");
    model->setHeaderData(0,Qt::Horizontal,QObject::tr("flightref"));
    model->setHeaderData(1,Qt::Horizontal,QObject::tr("lieudep"));
    model->setHeaderData(2,Qt::Horizontal,QObject::tr("lieuarr"));
    model->setHeaderData(3,Qt::Horizontal,QObject::tr("datedep"));
    model->setHeaderData(4,Qt::Horizontal,QObject::tr("datearr"));
    model->setHeaderData(5,Qt::Horizontal,QObject::tr("airline"));
    model->setHeaderData(6,Qt::Horizontal,QObject::tr("montant"));
    model->setHeaderData(7,Qt::Horizontal,QObject::tr("nbper"));
    return model;
}
*/
//---------------------------voyage delete-----------------------------------
bool Voyage::supprimer(QString ref)
{
    QSqlQuery query;
    bool t = search(ref);
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
    bool t = search(ref);
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
bool Voyage::search(QString ref)
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

QList<Voyage> Voyage::getDatabaseValues()
{
    QList<Voyage> Q = {};
    QSqlQuery query;
    query.exec("SELECT * FROM VOYAGES");
    int size = 0;
    while (query.next())
        size++;
    int it = 0;
    query.first();
    while (it < size)
    {
        flightref = query.value(0).toString();
        lieudep = query.value(1).toString();
        lieuarr = query.value(2).toString();
        datedep = query.value(3).toDate();
        datearr = query.value(4).toDate();
        airline = query.value(5).toString();
        montant = query.value(6).toFloat();
        nbper = query.value(7).toInt();
        Q.append(Voyage(flightref, lieudep, lieuarr, datedep, datearr, airline, montant, nbper));
        query.next();
        it++;
    }
    return Q;
}

QList<Voyage> Voyage::getDatabaseValues_tri(QString order)
{
    QList<Voyage> Q = {};
    QSqlQuery query;
    query.exec("SELECT * FROM VOYAGES ORDER BY " + order);
    int size = 0;
    while (query.next())
        size++;
    int it = 0;
    query.first();
    while (it < size)
    {
        flightref = query.value(0).toString();
        lieudep = query.value(1).toString();
        lieuarr = query.value(2).toString();
        datedep = query.value(3).toDate();
        datearr = query.value(4).toDate();
        airline = query.value(5).toString();
        montant = query.value(6).toFloat();
        nbper = query.value(7).toInt();
        Q.append(Voyage(flightref, lieudep, lieuarr, datedep, datearr, airline, montant, nbper));
        query.next();
        it++;
    }
    return Q;
}
QList<Voyage> Voyage::getDatabaseValues_recherche(QString recher)
{
    QList<Voyage> Q;
    QSqlQuery query;
    QString sqlQuery = "SELECT * FROM VOYAGES WHERE FLIGHTREF LIKE :recher OR LIEUDEP LIKE :recher OR LIEUARR LIKE :recher OR DATEARR LIKE :recher OR DATEDEP LIKE :recher OR AIRLINE LIKE :recher OR MONTANT LIKE :recher OR NBPER LIKE :recher";
    query.prepare(sqlQuery);
    query.bindValue(":recher", recher + "%");
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

int Voyage::getDatabaseValue(QList<Voyage> Q, QString s)
{
    QSqlQuery query;
    Voyage v;
    query.prepare("SELECT " + s + " FROM VOYAGES");
    query.exec();
    int size = 0;
    while (query.next())
    {
        size++;
    }
    int it = 0;
    query.first();
    while (it < size)
    {
        v.flightref = query.value(0).toString();
        v.lieudep = query.value(1).toString();
        v.lieuarr = query.value(2).toString();
        v.datedep = query.value(3).toDate();
        v.datearr = query.value(4).toDate();
        v.airline = query.value(5).toString();
        v.montant = query.value(6).toFloat();
        v.nbper = query.value(7).toInt();
        Q.append(v);
        it++;
        query.next();
    }
    return size;
}

int Voyage::getNeededDatabaseValue(QString s, QString condition)
{
    QSqlQuery query;
    query.prepare("SELECT " + s + " FROM VOYAGES WHERE   " + s + "= '" + condition + "'");
    query.exec();

    int size = 0;
    while (query.next())
        size++;
    return size;
}

QSqlQueryModel *Voyage::sort(QString s)
{
    QSqlQuery query;
    QSqlQueryModel *model = new QSqlQueryModel();
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
float Voyage::calculerCoutMoyen(QString Sdep, QString Sarr)
{

    QList<float> Q = {};
    QSqlQuery query;
    int size = 0;
    query.exec("SELECT MONTANT FROM VOYAGES WHERE (LIEUDEP= '" + Sdep + "') AND (LIEUARR= '" + Sarr + "')");
    while (query.next())
    {
        size++;
    }
    int it = 0;
    query.first();
    while (it < size)
    {
        Q.append(query.value(0).toFloat());
        it++;
        query.next();
    }
    int s = 0;
    for (int i = 0; i < size; i++)
        s += Q[i];
    if (size == 0)
        return 0;
    float moy = (float)s / size;
    return moy;
}

int Voyage::row_number()
{
    QSqlQuery query;
    int size = 0;
    query.prepare("SELECT * FROM VOYAGES");
    query.exec();
    while (query.next())
        size++;
    return size;
}
