#ifndef RUTA_H
#define RUTA_H


#include <QWidget>
#include "structure_ruta.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Ruta;
}

class Ruta : public QWidget
{
    Q_OBJECT

public:
    explicit Ruta(QWidget *parent = nullptr, bool newOne = true);
    ~Ruta();

    static bool writeRutas(QJsonArray rutas);
    static QStringList getListaRutas();
    static QJsonObject getRutaObjectFromCodEmplamiento(QString cod_emplazamiento);
    static QString getZonaRutaFromCodEmplazamiento(QString cod_emplazamiento);
signals:
    void script_excecution_result(int);
    void update_tableRutas(bool);

public slots:
    bool subirRuta(QString codigo);
    void setData(QJsonObject o){
        ruta = o;
        populateView(ruta);
    };
    void populateView(QJsonObject o);

    void delete_ruta_request(QStringList keys, QStringList values);
    bool eliminarRuta(QString cod);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_ruta_request(QStringList keys, QStringList values);
    void update_ruta_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Ruta *ui;
    QJsonObject ruta;
    database_comunication database_com;

    QString guardarDatos();
    void subirTodasLasRutas();

};


#endif // RUTA_H
