#ifndef TIPO_H
#define TIPO_H


#include <QWidget>
#include "structure_tipo.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Tipo;
}

class Tipo : public QWidget
{
    Q_OBJECT

public:
    explicit Tipo(QWidget *parent = nullptr, bool newOne = true);
    ~Tipo();

    static QJsonArray readTipos();
    static bool writeTipos(QJsonArray tipos);
    static QStringList getListaTipos();
signals:
    void script_excecution_result(int);
    void update_tableTipos(bool);

public slots:
    void setData(QJsonObject o){
        tipo = o;
        populateView(tipo);
    };
    void populateView(QJsonObject o);

    void delete_tipo_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_tipo_request(QStringList keys, QStringList values);
    void update_tipo_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Tipo *ui;
    QJsonObject tipo;
    database_comunication database_com;
    bool subirTipo(QString codigo);
    QString guardarDatos();
    void subirTodasLasTipos();
    bool eliminarTipo(QString cod);
};


#endif // TIPO_H
