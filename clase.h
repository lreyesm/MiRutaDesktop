#ifndef CLASE_H
#define CLASE_H


#include <QWidget>
#include "structure_clase.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Clase;
}

class Clase : public QWidget
{
    Q_OBJECT

public:
    explicit Clase(QWidget *parent = nullptr, bool newOne = true);
    ~Clase();

    static QJsonArray readClases();
    static bool writeClases(QJsonArray clases);
    static QStringList getListaClases();
signals:
    void script_excecution_result(int);
    void update_tableClases(bool);

public slots:
    void setData(QJsonObject o){
        clase = o;
        populateView(clase);
    };
    void populateView(QJsonObject o);

    void delete_clase_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_clase_request(QStringList keys, QStringList values);
    void update_clase_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Clase *ui;
    QJsonObject clase;
    database_comunication database_com;
    bool subirClase(QString codigo);
    QString guardarDatos();
    void subirTodasLasClases();
    bool eliminarClase(QString cod);
};


#endif // CLASE_H
