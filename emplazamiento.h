#ifndef EMPLAZAMIENTO_H
#define EMPLAZAMIENTO_H


#include <QWidget>
#include "structure_emplazamiento.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Emplazamiento;
}

class Emplazamiento : public QWidget
{
    Q_OBJECT

public:
    explicit Emplazamiento(QWidget *parent = nullptr, bool newOne = true);
    ~Emplazamiento();

    static QJsonArray readEmplazamientos();
    static bool writeEmplazamientos(QJsonArray emplazamientos);
    static QStringList getListaEmplazamientos();
signals:
    void script_excecution_result(int);
    void update_tableEmplazamientos(bool);

public slots:
    void setData(QJsonObject o){
        emplazamiento = o;
        populateView(emplazamiento);
    };
    void populateView(QJsonObject o);

    void delete_emplazamiento_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_emplazamiento_request(QStringList keys, QStringList values);
    void update_emplazamiento_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Emplazamiento *ui;
    QJsonObject emplazamiento;
    database_comunication database_com;
    bool subirEmplazamiento(QString codigo);
    QString guardarDatos();
    void subirTodasLasEmplazamientos();
    bool eliminarEmplazamiento(QString cod);
};


#endif // EMPLAZAMIENTO_H
