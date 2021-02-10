#ifndef OBSERVACION_H
#define OBSERVACION_H

#include <QWidget>
#include "structure_observacion.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Observacion;
}

class Observacion : public QWidget
{
    Q_OBJECT

public:
    explicit Observacion(QWidget *parent = nullptr, bool newOne = true);
    ~Observacion();

    static QJsonArray readObservaciones();
    static bool writeObservaciones(QJsonArray observaciones);
    static QStringList getListaObservaciones();
signals:
    void script_excecution_result(int);
    void update_tableObservaciones(bool);

public slots:
    void setData(QJsonObject o){
        observacion = o;
        populateView(observacion);
    };
    void populateView(QJsonObject o);

    void delete_observacion_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_observacion_request(QStringList keys, QStringList values);
    void update_observacion_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Observacion *ui;
    QJsonObject observacion;
    database_comunication database_com;
    bool subirObservacion(QString codigo);
    QString guardarDatos();
    void subirTodasLasObservaciones();
    bool eliminarObservacion(QString cod);
};

#endif // OBSERVACION_H
