#ifndef MARCA_H
#define MARCA_H

#include <QWidget>
#include "structure_marca.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Marca;
}

class Marca : public QWidget
{
    Q_OBJECT

public:
    explicit Marca(QWidget *parent = nullptr, bool newOne = true);
    ~Marca();

    static QJsonArray readMarcas();
    static bool writeMarcas(QJsonArray marcas);
    static QStringList getListaMarcas();
signals:
    void script_excecution_result(int);
    void update_tableMarcas(bool);

public slots:
    void setData(QJsonObject o){
        marca = o;
        populateView(marca);
    };
    void populateView(QJsonObject o);

    void delete_marca_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_marca_request(QStringList keys, QStringList values);
    void update_marca_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Marca *ui;
    QJsonObject marca;
    database_comunication database_com;
    bool subirMarca(QString codigo);
    QString guardarDatos();
    void subirTodasLasMarcas();
    bool eliminarMarca(QString cod);
};

#endif // MARCA_H
