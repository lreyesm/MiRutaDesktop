#ifndef EMPRESA_H
#define EMPRESA_H

#include <QWidget>
#include "structure_empresa.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Empresa;
}

class Empresa : public QWidget
{
    Q_OBJECT

public:
    explicit Empresa(QWidget *parent = nullptr, bool newOne = true);
    ~Empresa();

    static QJsonArray readEmpresas();
    static bool writeEmpresas(QJsonArray empresas);
    static QStringList getListaEmpresas();
    static QStringList getListaNombresEmpresas();
    static QString getCurrentFieldEmpresa(QString name_empresa);
signals:
    void script_excecution_result(int);
    void update_tableEmpresas(bool);

public slots:
    void setData(QJsonObject o){
        empresa = o;
        populateView(empresa);
    };
    void populateView(QJsonObject o);

    void delete_empresa_request(QStringList keys, QStringList values);

private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_empresa_request(QStringList keys, QStringList values);
    void update_empresa_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

    void on_pb_geolocalizacion_clicked();

    void setGeoCode(const QString geocode);
    void eraseMarker();
    void selectedPhoto(QPixmap pixmap);
    void on_le_empresa_editingFinished();

    void upload_empresa_image_request();
    void download_empresa_image_request();
private:
    Ui::Empresa *ui;
    QJsonObject empresa;
    database_comunication database_com;
    bool subirEmpresa(QString codigo);
    QString guardarDatos();
    void subirTodasLasEmpresas();
    bool eliminarEmpresa(QString cod);
    bool photoSelected = false;
    void savePhotoLocal(QPixmap pixmap);
    bool loadPhotoLocal();
    void scalePhoto(QPixmap pixmap);
    bool subirPhoto();
    QStringList keys, values;
    QString nombre_foto = "";
    bool checkIfFieldIsValid(QString var);
    bool descargarPhoto();
};


#endif // EMPRESA_H
