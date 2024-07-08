#ifndef VOYAGE_H
#define VOYAGE_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>
#include <QPrinter>
#include <QPainter>
#include <QPaintDevice>
#include <algorithm>

class Voyage
{
    QString flightref, lieudep, lieuarr, airline;
    QDate datedep, datearr;
    float montant;
    int nbper;

public:
    // Constructeurs
    Voyage();
    Voyage(QString, QString, QString, QDate, QDate, QString, float, int);

    // Getters
    QString getFlightref() { return flightref; }
    QString getLieudep() { return lieudep; }
    QString getLieuarr() { return lieuarr; }
    QString getAirline() { return airline; }
    QDate getDatedep() { return datedep; }
    QDate getDatearr() { return datearr; }
    float getMontant() { return montant; }
    int getNbper() { return nbper; }

    // Setters
    void setFlightref(QString f) { flightref = f; }
    void setLieudep(QString ld) { lieudep = ld; }
    void setLieuarr(QString la) { lieuarr = la; }
    void setAirline(QString a) { airline = a; }
    void setDatedep(QDate dd) { datedep = dd; }
    void setDatearr(QDate da) { datearr = da; }
    void setMontant(float m) { montant = m; }
    void setNbper(int n) { nbper = n; }

    // CRUD
    bool ajouter() const;
    bool supprimer(QString) const;
    bool update(QString ref) const;
    bool searchByFlightRef(QString ref) const;
    QList<Voyage> getAllVoyages() const;
    QList<Voyage> searchVoyages(QString recher) const;
    QList<Voyage> getAllVoyagesSorted(QString order) const;
    QList<int> getVoyageCount(QString s,QList<QString>& v) const;
    float calculateAverageCost(QString Sdep, QString Sarr) const;
};

#endif // VOYAGE_H
