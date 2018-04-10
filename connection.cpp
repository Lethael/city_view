//Just test git
#include "connection.h"
#include <QDebug>

Connection::Connection(QObject *parent) :
    QObject(parent)
{
}

Connection::~Connection()
{
    db.close();
    qDebug() << "Close";
}

bool Connection::setConnection(QString &driver, QString &databasename, QString &hostname, QString &username, QString &passwd)
{
    rmConnection();
    db = QSqlDatabase::addDatabase(driver);
    db.setDatabaseName(databasename);
    db.setHostName(hostname);
    db.setUserName(username);
    db.setPassword(passwd);

    if(!db.open()) {
        return false;
    } else {
        qDebug() << true;
        return true;
    }
}

bool Connection::allowTransaction()
{
    /* Method QSqlDatabase::transaction() doesn't work with QMYSQL Driver
    if(!db.transaction())
        return false; */
    QSqlQuery q;
    if(!q.exec("SET autocommit=0;"))
    {
        qDebug() << "false transaction";
        return false;
    }
    if(!q.exec("START TRANSACTION;"))
    {
        qDebug() << "false transaction in Start";
        return false;
    }
    else
    {
        qDebug() << "true transaction";
        return true;
    }
}

bool Connection::setAutocommit()
{
    /* Set Autocommit default mode */
    QSqlQuery q;
    if(!q.exec("SET autocommit=1;"))
        return false;
    else
        return true;
}

bool Connection::commit()
{
    /*Method QSqlDatabase::commit() doesn't work with QMYSQL Driver
    if(!db.commit())
        return false; */
    QSqlQuery q;
    if(!q.exec("COMMIT;"))
    {
        qDebug() << false;
        return false;
    }
    else
    {
        qDebug() << true;
        return true;
    }
}

bool Connection::rollback()
{
    /*Method QSqlDatabase::rollback() doesn't work with QMYSQL Driver
    if(!db.rollback())
        return false; */
    QSqlQuery q;
    if(!q.exec("ROLLBACK;"))
        return false;
    else
        return true;
}
bool Connection::rmConnection()
{
    if(!db.contains("qt_sql_default_connection"))
        return false;
    db.close();
    return true;
}

QStringList Connection::getDrivers() const
{
    QStringList drivers = QSqlDatabase::drivers();
    return drivers;
}


/*  Accessors   Mutators    */
QSqlDatabase Connection::getDb() const
{
    return db;
}

void Connection::setDb(const QSqlDatabase &value)
{
    db = value;
}
QString Connection::getDatabasename() const
{
    return databasename;
}

void Connection::setDatabasename(const QString &value)
{
    databasename = value;
}
QString Connection::getDriver() const
{
    return driver;
}

void Connection::setDriver(const QString &value)
{
    driver = value;
}
QString Connection::getHostname() const
{
    return hostname;
}

void Connection::setHostname(const QString &value)
{
    hostname = value;
}
QString Connection::getUsername() const
{
    return username;
}

void Connection::setUsername(const QString &value)
{
    username = value;
}
QString Connection::getPasswd() const
{
    return passwd;
}

void Connection::setPasswd(const QString &value)
{
    passwd = value;
}

/*  END ACCESSORS   MUTATORS    */

void Connection::closeEvent(QCloseEvent *e)
{
    emit Cnx_onClose();
    e->accept();
}




