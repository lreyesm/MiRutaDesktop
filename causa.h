#ifndef CAUSA_H
#define CAUSA_H

#include <QWidget>
#include "structure_causa.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Causa;
}

class Causa : public QWidget
{
    Q_OBJECT

public:
    explicit Causa(QWidget *parent = nullptr, bool newOne = true);
    ~Causa();

    static QJsonArray readCausas();
    static bool writeCausas(QJsonArray causas);
    static QJsonArray readAnomalias();
    static bool writeAnomalias(QJsonArray anomalias);
    static QStringList getListaCausas();
    static QJsonObject getCausaObject(QString codigo_causa);
    static QString getTipoTareaFromCodeCausa(QString codigo_causa);
    static QString getAccionOrdenadaFromCodeCausa(QString codigo_causa);
    static QString getARealizarFromCodeCausa(QString codigo_causa);
    static QString getIntervencionFromCodeCausa(QString codigo_causa);
signals:
    void script_excecution_result(int);
    void update_tableCausas(bool);

public slots:
    void setData(QJsonObject o){
        causa = o;
        populateView(causa);
    };
    void populateView(QJsonObject o);

    void delete_causa_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_causa_request(QStringList keys, QStringList values);
    void update_causa_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Causa *ui;
    QJsonObject causa;
    database_comunication database_com;
    bool subirCausa(QString codigo);
    QString guardarDatos();
    void subirTodasLasCausas();
    bool eliminarCausa(QString cod);
};

#endif // CAUSA_H
