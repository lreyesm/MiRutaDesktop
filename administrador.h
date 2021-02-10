#ifndef ADMINISTRADOR_H
#define ADMINISTRADOR_H

#include <QWidget>
#include "structure_administrador.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Administrador;
}

class Administrador : public QWidget
{
    Q_OBJECT

public:
    explicit Administrador(QWidget *parent = nullptr, bool newOne = true, QString empresa = "");
    ~Administrador();

    static QJsonArray readAdministradores();
    static bool writeAdministradores(QJsonArray administradores);
    static QStringList getListaAdministradores();
    static QStringList getListaUsuarisAdministradores();
signals:
    void script_excecution_result(int);
    void update_tableAdministradores(bool);

public slots:
    void setData(QJsonObject o){
        administrador = o;
        populateView(administrador);
    };
    void populateView(QJsonObject o);

    void delete_administrador_request(QStringList keys, QStringList values);

private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_administrador_request(QStringList keys, QStringList values);
    void update_administrador_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

    void selectedPhoto(QPixmap pixmap);
    void upload_administrador_image_request();
    void download_administrador_image_request();
    void on_le_administrador_editingFinished();

private:
    Ui::Administrador *ui;
    QJsonObject administrador;
    database_comunication database_com;
    bool subirAdministrador(QString codigo);
    QString guardarDatos();
    void subirTodasLasAdministradores();
    bool eliminarAdministrador(QString cod);
    QString empresa = "";
    void scalePhoto(QPixmap pixmap);
    bool loadPhotoLocal();
    void savePhotoLocal(QPixmap pixmap);
    bool checkIfFieldIsValid(QString var);
    bool photoSelected = false;
    bool descargarPhoto();
    bool subirPhoto();
    QString nombre_foto = "";
    QStringList keys, values;
};

#endif // ADMINISTRADOR_H
