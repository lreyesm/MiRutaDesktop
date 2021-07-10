#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <QDataStream>
#include <QFile>
#include <QLayout>
#include <QWidget>
#include <QtCore>
#include "database_comunication.h"

#include <QMessageBox>

class GlobalFunctions : public QObject
{
    Q_OBJECT
public:
    explicit GlobalFunctions(QObject *parent = nullptr, QString empresa = "");

    enum{MAYOR_A_MENOR, MENOR_A_MAYOR};
    GlobalFunctions();
    static QString gestor_selected_file;
    static QString empresa_selected_file;

    static QString readGestorSelected();
    static void writeGestorSelected(QString gestor);
    static void writeEmpresaSelected(QString empresa);
    static QString readEmpresaSelected();

    static bool checkIfFieldIsValid(QString var);
    static void clearWidgets(QLayout *layout);
    static void deleteAllChilds(QWidget *w);
    static bool deleteChild(QWidget *w, QString childObjectName);
    static QString convertJsonObjectToString(QJsonObject jsonObject);
    static QJsonObject convertStringToJsonObject(QString jsonObject_string);
    static QJsonArray convertStringToJsonArray(QString jsonArray_string);
    static void sortStringList(QStringList &list, int orden = MENOR_A_MAYOR);

    bool checkIfTareaExist(QString field, QString value);
    bool checkIfTareaExist(QJsonObject jsonObject);
    bool checkIfTareaExist(QString value);
    bool checkIfCounterExist(QString field, QString value);
    bool checkIfCounterExist(QString value);
    bool checkIfItacExist(QString field, QString value);
    bool checkIfItacExist(QString value);
    bool checkIfRutaExist(QString field, QString value);
    bool checkIfRutaExist(QString value);


    QStringList getTareasList();
    QStringList getTareasList(QString field, QString value);
    QJsonArray getTareasFields(QStringList fields, QString field, QString value);
    QJsonArray getTareasFields(QStringList fields, QString query);
    QJsonObject getTareaFromServer(QString field, QString value);
    QJsonObject getTareaFromServer(QJsonObject jsonObject);
    QJsonArray getTareasFromServer(QString field, QString value);

    QJsonObject getContadorFromServer(QString field, QString value);
    QJsonObject getContadorFromServer(QString value);
    QStringList getContadoresList();
    QJsonArray getContadoresFromServer(QString field, QString value);

    QStringList getItacsList();
    QJsonArray getItacsFromServer(QString field, QString value);
    QJsonObject getItacFromServer(QString field, QString value);
    QJsonObject getItacFromServer(QString value);
    QJsonArray getItacsFields(QStringList fields, QString field, QStringList values);
    QJsonArray getItacsFields(QStringList fields, QString field, QString value);
    QJsonArray getItacsFields(QStringList fields);
    QJsonArray getItacsFromServer(QStringList fields, QStringList values);
    QJsonArray getItacsFromServer(QString field, QStringList values);

    QJsonArray getRutasFields(QStringList fields);
    QJsonArray getRutasFields(QStringList fields, QString field, QString value);
    QJsonArray getRutasFields(QStringList fields, QString field, QStringList values);
    QJsonArray getRutasFromServer(QString field, QString value);
    QJsonObject getRutaFromServer(QString field, QString value);
    QJsonObject getRutaFromServer(QString value);
    QStringList getRutasList();

    QStringList getContadoresList(bool disponibles, QString serie="");

    static void showMessage(QWidget *parent, const QString title, const QString mess);

    int showQuestion(QWidget *parent = nullptr, const QString title = "Pregunta", const QString mess = "?",
                     int acceptOption = QMessageBox::Ok, int cancelOption= QMessageBox::Cancel);


    int showQuestion(QWidget *parent, const QString title, const QString mess,
                     int acceptOption, int cancelOption,
                     QString acceptText, QString cancelText);

    static void showWarning(QWidget *parent, const QString title, const QString mess);

    static void showInExplorer(const QString &path);

    static void execJavaScriptCode(QString code);

    static void setDelay(int delay_ms);

    static QString getDirOfTask(QJsonObject jsonObject);

    static QString changeFechaFormat(QString f);

signals:
    void script_excecution_result(int);

public slots:    


private slots:    
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);

    bool getValuesFieldCustomQueryServer(QString empresa, QString tabla, QString column, QString query);
    bool getValuesFieldServer(QString empresa, QString tabla, QString column);
    bool getMultipleValuesFieldsServer(QString empresa, QString tabla, QJsonObject json_fields);
    bool getMultipleValuesFieldsCustomQueryServer(QString empresa, QString tabla, QJsonObject json_fields, QString query);

    void get_multiple_values_fields_request();
    void get_multiple_values_fields_custom_query_request();
    void get_all_column_values_request();
    void get_all_column_values_custom_query_request();

    void get_tareas_request();
    void get_tareas_amount_request();
    void get_contadores_request();
    void get_contadores_amount_request();
    void get_itacs_amount_request();
    void get_itacs_request();

    bool getContadoresCustomQuery(QString query, int id_start);
    bool getItacsCustomQuery(QString query, int id_start);
    bool getTareasCustomQuery(QString query, int id_start);
    bool getRutasCustomQuery(QString query, int id_start);

    bool getItacsFromServer(QString empresa, QString query, int limit, int id_start);
    bool getItacsAmountFromServer(QString empresa, QString query, int limit = LIMIT);
    bool getContadoresFromServer(QString empresa, QString query, int limit, int id_start);
    bool getContadoresAmountFromServer(QString empresa, QString query, int limit = LIMIT);
    bool getTareasFromServer(QString empresa, QString query, int limit, int id_start);
    bool getTareasAmountFromServer(QString empresa, QString query, int limit = LIMIT);
    bool getRutasFromServer(QString query, int limit, int id_start);
    bool getRutasAmountFromServer(QString query, int limit = LIMIT);

    void get_rutas_amount_request();
    void get_rutas_request();

    bool getRutasValuesFieldServer(QString column);
    bool getRutasValuesFieldCustomQueryServer(QString column, QString query);
private:
    database_comunication database_com;
    QStringList keys, values;
    int count = 0;
    QJsonObject jsonInfoAmount, jsonObjectValues;
    QJsonArray jsonArrayAll;

    int currentPage = 1;
    QString lastQuery= "";
    int last_id_start = 0;
    int limit_pagination = 500;
    QString empresa = "";
};

#endif // GLOBALFUNCTIONS_H
