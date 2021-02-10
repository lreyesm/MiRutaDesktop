#ifndef ZONA_H
#define ZONA_H

#include <QWidget>
#include "structure_zona.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Zona;
}

class Zona : public QWidget
{
    Q_OBJECT

public:
    explicit Zona(QWidget *parent = nullptr, bool newOne = true, QString empresa = "");
    ~Zona();

    static QJsonArray readZonas();
    static bool writeZonas(QJsonArray zonas);
    static QStringList getListaZonas();
signals:
    void script_excecution_result(int);
    void update_tableZonas(bool);

public slots:
    void setData(QJsonObject o){
        zona = o;
        populateView(zona);
    };
    void populateView(QJsonObject o);

    void delete_zona_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_zona_request(QStringList keys, QStringList values);
    void update_zona_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

    void on_pb_geolocalizacion_clicked();

    void setGeoCode(const QString geocode);
    void eraseMarker();
private:
    Ui::Zona *ui;
    QJsonObject zona;
    database_comunication database_com;
    bool subirZona(QString codigo);
    QString guardarDatos();
    void subirTodasLasZonas();
    bool eliminarZona(QString cod);
    QString empresa = "";
};

#endif // ZONA_H
