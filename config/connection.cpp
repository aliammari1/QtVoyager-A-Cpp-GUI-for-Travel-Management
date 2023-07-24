#include "connection.h"

Connection::Connection()
{
}

bool Connection::createconnection()
{
    bool test = false;
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("Projet2A");
    db.setUserName("Ali");
    db.setPassword("password");
    if (db.open())
        test = true;
    return test;
}

void Connection::closeConnection()
{
    db.close();
}
