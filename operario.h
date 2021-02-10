#ifndef OPERARIO_H
#define OPERARIO_H

#include <QWidget>
#include "structure_operario.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Operario;
}

class Operario : public QWidget
{
    Q_OBJECT

public:
    explicit Operario(QWidget *parent = nullptr, bool newOne = true, QString empresa = "");
    ~Operario();

    static QJsonArray readOperarios();
    static bool writeOperarios(QJsonArray operarios);
    static QStringList getListaOperarios();
    static QStringList getListaUsuarios();
    static bool checkIfFieldIsValid(QString var);
signals:
    void script_excecution_result(int);
    void update_tableOperarios(bool);
    void updateTareas();

public slots:
    void setData(QJsonObject o){
        operario = o;
        populateView(operario);
    };
    void populateView(QJsonObject o);

    void delete_operario_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_operario_request(QStringList keys, QStringList values);
    void update_operario_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

    void selectedPhoto(QPixmap pixmap);
    void upload_operario_image_request();
    void download_operario_image_request();
    void on_le_operario_editingFinished();

    void update_tareas_fields_request();
    void on_pb_jornada_clicked();

private:
    Ui::Operario *ui;
    QJsonObject operario;
    database_comunication database_com;
    bool subirOperario(QString codigo);
    QString guardarDatos();
    void subirTodasLasOperarios();
    bool eliminarOperario(QString cod);
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
    QString old_operario = "";
    bool update_fields(QStringList numeros_internos_list, QJsonObject campos);

};

#endif // OPERARIO_H
