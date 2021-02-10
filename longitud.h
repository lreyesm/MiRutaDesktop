#ifndef LONGITUD_H
#define LONGITUD_H

#include <QWidget>
#include "structure_longitud.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Longitud;
}

class Longitud : public QWidget
{
    Q_OBJECT

public:
    explicit Longitud(QWidget *parent = nullptr, bool newOne = true);
    ~Longitud();

    static QJsonArray readLongitudes();
    static bool writeLongitudes(QJsonArray longitudes);
    static QStringList getListaLongitudes();
signals:
    void script_excecution_result(int);
    void update_tableLongitudes(bool);

public slots:
    void setData(QJsonObject o){
        longitud = o;
        populateView(longitud);
    };
    void populateView(QJsonObject o);

    void delete_longitud_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_longitud_request(QStringList keys, QStringList values);
    void update_longitud_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Longitud *ui;
    QJsonObject longitud;
    database_comunication database_com;
    bool subirLongitud(QString codigo);
    QString guardarDatos();
    void subirTodasLasLongitudes();
    bool eliminarLongitud(QString cod);
};

#endif // LONGITUD_H
