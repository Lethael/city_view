#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QtSql>
#include <QCloseEvent>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = 0);
    ~Connection();
    /*  Mutators    */
    void setDb(const QSqlDatabase &value);
    void setDatabasename(const QString &value);
    void setDriver(const QString &value);
    void setHostname(const QString &value);
    void setUsername(const QString &value);
    void setPasswd(const QString &value);

    bool setConnection(QString &driver, QString &databasename, QString &hostname, QString &username, QString &passwd);
    bool allowTransaction();
    bool setAutocommit();
    bool commit();
    bool rollback();
    bool rmConnection();

    QStringList getDrivers() const;

private :
    QSqlDatabase db;
    QString databasename;
    QString driver;
    QString hostname;
    QString username;
    QString passwd;

    /*  Accessors   */
    QSqlDatabase getDb() const;
    QString getDatabasename() const;
    QString getDriver() const;
    QString getHostname() const;
    QString getUsername() const;
    QString getPasswd() const;

signals:
    void Cnx_onClose();

public slots:
    void closeEvent(QCloseEvent *e);

};

#endif // CONNECTION_H
