#ifndef CLIENTE_H
#define CLIENTE_H

#include <QWidget>
#include "structure_cliente.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Cliente;
}

class Cliente : public QWidget
{
    Q_OBJECT

public:
    explicit Cliente(QWidget *parent = nullptr, bool newOne = true, QString empresa = "");
    ~Cliente();

    static QJsonArray readClientes();
    static bool writeClientes(QJsonArray clientes);
    static QStringList getListaClientes();
    static QStringList getListaUsuarios();
    static bool checkIfFieldIsValid(QString var);
    static bool checkEmailValid(QString email);
signals:
    void script_excecution_result(int);
    void update_tableClientes(bool);
    void updateTareas();

public slots:
    void setData(QJsonObject o){
        cliente = o;
        populateView(cliente);
    };
    void populateView(QJsonObject o);

    void delete_cliente_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_cliente_request(QStringList keys, QStringList values);
    void update_cliente_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

    void selectedPhoto(QPixmap pixmap);
    void upload_cliente_image_request();
    void download_cliente_image_request();
    void on_le_cliente_editingFinished();

    void update_tareas_fields_request();

    void on_pb_restrict_clicked();

    void setRestrictions(QString restrictions);
private:
    Ui::Cliente *ui;
    QJsonObject cliente;
    database_comunication database_com;
    bool subirCliente(QString codigo);
    QString guardarDatos();
    void subirTodasLasClientes();
    bool eliminarCliente(QString cod);
    QString empresa = "";
    void scalePhoto(QPixmap pixmap);
    bool loadPhotoLocal();
    void savePhotoLocal(QPixmap pixmap);

    bool photoSelected = false;
    bool descargarPhoto();
    bool subirPhoto();
    QString nombre_foto = "";
    QStringList keys, values;
    bool update_tareas = false;
    QString old_cliente = "";
    bool update_fields(QStringList numeros_internos_list, QJsonObject campos);

};


#endif // CLIENTE_H
