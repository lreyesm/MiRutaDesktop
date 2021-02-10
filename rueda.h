#ifndef RUEDA_H
#define RUEDA_H


#include <QWidget>
#include "structure_rueda.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Rueda;
}

class Rueda : public QWidget
{
    Q_OBJECT

public:
    explicit Rueda(QWidget *parent = nullptr, bool newOne = true);
    ~Rueda();

    static QJsonArray readRuedas();
    static bool writeRuedas(QJsonArray ruedas);
    static QStringList getListaRuedas();
signals:
    void script_excecution_result(int);
    void update_tableRuedas(bool);

public slots:
    void setData(QJsonObject o){
        rueda = o;
        populateView(rueda);
    };
    void populateView(QJsonObject o);

    void delete_rueda_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_rueda_request(QStringList keys, QStringList values);
    void update_rueda_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Rueda *ui;
    QJsonObject rueda;
    database_comunication database_com;
    bool subirRueda(QString codigo);
    QString guardarDatos();
    void subirTodasLasRuedas();
    bool eliminarRueda(QString cod);
};


#endif // RUEDA_H
