#ifndef ITAC_H
#define ITAC_H

#include <QWidget>
#include "structure_itac.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"
#include "global_variables.h"
#include <QLabel>

class MyLabelPhoto;

namespace Ui {
class ITAC;
}

class ITAC : public QWidget
{
    Q_OBJECT

public:
    explicit ITAC(QWidget *parent = nullptr, bool newOne = true, QString empresa = "",
                  QJsonObject jsonDefaultFields = QJsonObject());
    ~ITAC();

    static bool writeITACs(QJsonArray itacs);
    static QStringList getListaITACs();
    static QJsonObject getITACJsonObjectFromCode(QString codigo);
    static QString getGeoCodeFromCodeItac(QString codigo);
    static QString formatCodeEmplazamiento(QString cod);
    static QJsonObject sumAllSections(QJsonObject itac_l);
    bool uploadItac(QString codigo);
    void Pdf_creator(QJsonObject o = QJsonObject(), bool show = false);
    QMap<QString, QStringList> fillMapForFixModel(QStringList &listHeaders);
    QPixmap scalePhoto(QPixmap pixmap, int max_height, int max_width);
    QString getLocalDirofCurrentItac();
    bool descargarAudio();

    void show_loading(QWidget *parent, QPoint pos, QColor color = color_blue_app, int w = 40, int h= 40);
    void displayLoading(MyLabelPhoto *label);
    QString getColorString(QColor color, bool hex = true);
    void loadLocalPhotos();
    bool checkDisponibleAudio();
    bool loadPhotoLocal(int currentPhoto);
signals:
    void hidingLoading();
    void script_excecution_result(int);
    void update_tableITACs(bool);
    void updateTableTareas();
    void changedGeoCode(QString);
    void closing();
    void setLoadingTextSignal(QString);

public slots:
    void setData(QJsonObject o, bool downloadPhotos = true){
        itac = o;
        populateView(itac, downloadPhotos);
    };
    void populateView(QJsonObject o = QJsonObject(), bool downloadPhotos = true);

    void delete_itac_request(QStringList keys, QStringList values);

    bool eliminarITAC(QString cod);
    bool subirITAC(QString codigo);

protected slots:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_itac_request(QStringList keys, QStringList values);
    void update_itac_request(QStringList keys, QStringList values);
    void download_itac_image_request();
    void upload_itac_image_request();
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

    void on_pb_geolocalizacion_clicked();

    void setGeoCode(const QString geocode);
    void eraseMarker();
    void selectedPhoto(QPixmap pixmap);
    void selectedPhotoName(QString object_name);

    void update_tareas_fields_request();
    void on_pb_foto_1_clicked();
    void on_pb_foto_2_clicked();
    void on_pb_foto_3_clicked();
    void on_pb_foto_4_clicked();
    void on_pb_foto_5_clicked();
    void on_pb_foto_6_clicked();
    void on_pb_foto_7_clicked();
    void on_pb_foto_8_clicked();

    void on_pb_acceso_ubicacion_clicked();

    void on_pb_llaves_puertas_clicked();

    void on_pb_estado_clicked();

    void on_pb_tuberias_clicked();

    void on_pb_valvulas_clicked();

    void getJsonModified(QJsonObject jsonObject);
    void on_le_zona_selectionChanged();

    void initializeITAC();
    void on_pb_resumen_de_estado_clicked();

    void delete_itac_request();
    void setSelectedState(QString state);
    void on_le_codigo_textEdited(const QString &arg1);

    void on_pb_crear_pdf_clicked();

    void on_pb_mostrar_carpeta_clicked();

    void on_pb_play_audio_clicked();
    void download_audio_request();
    void reproducirAudio();
    void update_itac_with_id_request(QStringList keys, QStringList values);
    void setLoadingText(QString mess);
    void hide_loading();
private:
    Ui::ITAC *ui;
    QJsonObject itac;
    database_comunication database_com;

    QString guardarDatos();
    void subirTodasLasITACs();
    QString empresa;
    void scalePhoto(QPixmap pixmap, QString object_name);
    bool loadPhotoLocal();
    bool savePhotoLocal(QPixmap pixmap, QString name);
    static bool checkIfFieldIsValid(QString var);
    bool photoSelected = false;////subo foto al actualizar si no existe
    QString photoSelectedObjectName = "";
    bool descargarPhoto();
    bool subirPhoto();
    QStringList nombres_fotos;
    QStringList keys, values;
    int descargando_foto_x = 0;
    bool geo_modified = false;
    bool updateTareas();
    QString gestor = "", stateSelected = "";
    void showInExplorer(const QString &path);
    bool newOne = false;
    bool changedCodeItac = false;
    void show_loading(QString mess);
};


#endif // ITAC_H
