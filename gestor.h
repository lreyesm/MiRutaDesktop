#ifndef GESTOR_H
#define GESTOR_H


#include <QWidget>
#include "structure_gestor.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Gestor;
}

class Gestor : public QWidget
{
    Q_OBJECT

public:
    explicit Gestor(QWidget *parent = nullptr, bool newOne = true, QString empresa = "");
    ~Gestor();

    static QJsonObject getGestorJsonObject(QString gest);
    static QJsonArray readGestores();
    static bool writeGestores(QJsonArray gestores);
    static QStringList getListaGestores();
    static QStringList getListaNombresGestores();
signals:
    void script_excecution_result(int);
    void update_tableGestores(bool);
    void updateTareas();
public slots:
    void setData(QJsonObject o){
        gestor = o;
        populateView(gestor);
    };
    void populateView(QJsonObject o);

    void delete_gestor_request(QStringList keys, QStringList values);
    QPixmap getPhotoGestor();
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_gestor_request(QStringList keys, QStringList values);
    void update_gestor_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

    void selectedPhoto(QPixmap pixmap);
    void upload_gestor_image_request();
    void download_gestor_image_request();
    void on_le_gestor_editingFinished();

    void update_tareas_fields_request();
private:
    Ui::Gestor *ui;
    QJsonObject gestor;
    database_comunication database_com;
    bool subirGestor(QString codigo);
    QString guardarDatos();
    void subirTodasLasGestores();
    bool eliminarGestor(QString cod);
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
    bool update_tareas = false;
    QString old_gestor = "";
    bool update_fields(QStringList numeros_internos_list, QJsonObject campos);
    QPixmap logo = QPixmap();
};

#endif // GESTOR_H
