#ifndef DBTAREASCONTROLLER_H
#define DBTAREASCONTROLLER_H

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQueryModel>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

class DBtareasController
{
public:
    DBtareasController(const QString &path);
    ~DBtareasController();

    bool isOpen() const;
    bool createTableTareas();
    bool insertTarea(QJsonObject jsonObject);
    bool removeTarea(const QString &tareaPrincVar);
    bool updateTarea(QJsonObject jsonObject);
    bool checkIfTareaExists(const QString &tareaPrincVar);
    QJsonArray getTareas();
    QJsonArray getTareas(QString key, QString value);
    QJsonArray getTareas(QStringList keys_where, QString values_where);
    int getCountTableRows();
    QStringList getOneColumnValues(const QString &column);
private:
    QSqlDatabase m_db;
    QJsonObject jsonTareaType;
    void fillJsonType();
    QString SQLQueryGetString(const QString &sqlquery);
    QJsonArray SQLQueryGetArray(const QString &sqlquery);
};

#endif // DBTAREASCONTROLLER_H
