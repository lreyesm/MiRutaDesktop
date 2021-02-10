#ifndef PIEZA_H
#define PIEZA_H

#include "structure_pieza.h"
#include <QWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Pieza;
}

class Pieza : public QWidget
{
    Q_OBJECT

public:
    explicit Pieza(QWidget *parent = nullptr, bool newOne = true);
    ~Pieza();

    static QJsonArray readPiezas();
    static bool writePiezas(QJsonArray piezas);
    static QStringList getListaPiezas();
signals:
    void script_excecution_result(int);
    void update_tablePiezas(bool);

public slots:
    void setData(QJsonObject o){
        pieza = o;
        populateView(pieza);
    };
    void populateView(QJsonObject o);

    void delete_pieza_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_pieza_request(QStringList keys, QStringList values);
    void update_pieza_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Pieza *ui;
    QJsonObject pieza;
    database_comunication database_com;
    bool subirPieza(QString codigo);
    QString guardarDatos();
    void subirTodasLasPiezas();
    bool eliminarPieza(QString cod);
};

#endif // PIEZA_H
