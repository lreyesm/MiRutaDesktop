#ifndef OTHER_TASK_SCREEN_H
#define OTHER_TASK_SCREEN_H

#include <QDialog>

#include <QTimer>

#include <QJsonObject>
#include "database_comunication.h"
#include "smtp.h"
#include <QCompleter>
#include "global_variables.h"

class my_label;

namespace Ui {
class other_task_screen;
}

class other_task_screen : public QDialog
{
    Q_OBJECT

public:
    explicit other_task_screen(QWidget *parent = nullptr, bool sin_revisar = false, bool mostrar_botones = true, QString empresa = "");
    ~other_task_screen();
    bool populateView(bool load_photos = true);
    bool static checkIfFieldIsValid(QString var);
    static QStringList readModifiedTasks();
    static void writeModifiedTasks(QStringList);
    static void writeJsonArrayTasks(QJsonArray jsonArray);
    static QJsonArray readJsonArrayTasks();
    static QString administrator_loged;
    static bool checkIfOnlyNumbers(QString tel);
    static bool conexion_activa;
    bool foto_cambiada = false;
    void toogleEdit(bool, bool new_task = false);
    void setShowMesageBox(bool b){showMesageBox = b;}
    QStringList files;
    QString filename = "", myMail, destinyMail, myClave;
    Smtp * smtp;
    QTimer timer;
    QString dir_foto_antes_instalacion;
    QString dir_foto_despues_instalacion;
    QString dir_foto_lectura;
    QString dir_foto_numero_serie;
    QString dir_foto_firma;
    QString dir_foto_empresa;
    QString dir_foto_incidencia_1;
    QString dir_foto_incidencia_2;
    QString dir_foto_incidencia_3;
    QTime selection_time_end, selection_time_init;
    QDate selection_date;
    QMap <int,QString> map_months;
    QMap <int,QString> map_days_week;
    QString get_current_date_in_format(QDate date = QDate::currentDate(),
                                              QTime time_init = QTime::currentTime(),
                                       QTime time_end = QTime::currentTime());

    void setFileDirSelected(QString file_name);
    QString get_date_from_status(QJsonObject object, QString status);
    QJsonObject set_date_from_status(QJsonObject object, QString status, QString date, QTime time = QTime::currentTime());

    static int getJsonObjectPositionInJsonArray(QJsonArray jsonArray, QJsonObject jsonObject);
    static QJsonArray updateTaskIfNeeded(QJsonArray jsonArray, QJsonObject jsonObject, int index);
    static bool compareJsonObjectByDateModified(QJsonObject jsonObject_old, QJsonObject jsonObject_new);

    void createAutoPDF(bool show = false, bool force_create = false);
    QString transformarFecha(QString fecha_old_view);
    QString transformarFecha_to_old_view(QString fecha_new_view);
    QString convertirTipoOrdenToLong(QString orden_short);
    QString convertirTipoOrdenToShort(QString orden_long);

    static void writeTrabajosCargados(QStringList lista_trabajos_cargados);
    static QStringList readTrabajosCargados();
    static void writeficherosDescargados(QStringList lista_ficheros_descargados);
    static QStringList readficherosDescargados();
    static QString getDirWithNames(QDir dir_fotos, QStringList names);
    static QString buscarCarpetaDeTrabajoPendiente();
    static QString buscarCarpetaDeFotos();
    static QStringList getFilesWithNamesInDir(QStringList names, QDir dir_file);
    void setLogoType(QPixmap logo);
    static void ordenarLista(QStringList &array, QString order);
    QString empresa = "";
    static QString getStringFromPhoto(QImage image);
    static QString getValidGeoCode(QJsonObject jsonObject);
    void getLogoType();

    void scalePhoto(QPixmap pixmap, QLabel *label, int max_w, int max_h);
    bool descargarAudio();
    QString getLocalDirofCurrentTask();
    void loadLocalPhotos();
    void show_loading(QWidget *parent, QPoint pos = QPoint(0,0),
                      QColor color = color_blue_app, int w = 40, int h = 40,
                      bool show_white_background = true);
    void displayLoading(my_label *label);
    static QString getColorString(QColor color, bool hex = true);

    bool checkDisponibleAudio();
    void loadLocalPhoto(int currentPhoto);
    void clearTask();
signals:
    void hidingLoading();
    void mouse_pressed();
    void mouse_Release();
    void script_excecution_result(int);
    void imagesDownloaded();
    void file_downloaded_result(int);
     void file_downloaded_dir(QString);
    void task_upload_excecution_result(int);
    void task_upload_excecution_end(QJsonArray);
    void photo_downloaded();
    void AllInternalNumbersFilled(database_comunication::serverRequestType);
    void tarea_revisada(QString);
    void closing();
    void hideMenuFast(QString);
    void updateITACs();
    void setLoadingTextSignal(QString);

public slots:
    void on_pb_close_clicked();
    void on_pb_update_server_info_clicked();
    void create_task_request(QStringList keys, QStringList values);

    void create_task_request();
    void update_task_request(QStringList keys, QStringList values);
    void update_task_request();
    void download_task_image_request(QStringList keys, QStringList values);
    void download_task_image_request();
    void upload_task_image_request();
    static QString nullity_check(QString q)
    {
        if(q.trimmed() == "null"  || q.trimmed() == "NULL")
            return "";
        else
            return q;
    }
    void clear_all_pictures();
    void show_loading(QString mess = "Subiendo...");
    void hide_loading();
    void download_save_work_file_request(QStringList keys, QStringList values);
    void upload_save_work_file_request(QStringList keys, QStringList values);
    void updateTaskInJsonArrayAllLocal(QJsonArray jsonArray, QJsonObject jsonObject);

protected slots:
    void mousePressEvent(QMouseEvent *e); ///al reimplementar esta funcion deja de funcionar el evento pressed
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
private slots:
    void getData(QJsonObject);
    void on_pb_edit_clicked();
    void serverAnswer(QByteArray, database_comunication::serverRequestType tipo);
    void conection_timeout()
    {
        Script_excecution_result = database_comunication::script_result::timeout;
    }
    void update_script_execution_result(int b)
    {
        Script_excecution_result = b;
    }

    void on_drag_screen();
    void on_drag_screen_released();
    void on_start_moving_screen_timeout();

    void on_pb_cerrar_tarea_clicked();
    void on_pb_crear_pdf_clicked();
    void on_pb_imprimir_pdf_clicked();
    void on_pb_enviar_pdf_clicked();
    void mailSent(QString status);
    void my_mail(QString mail);
    void destiny_mail(QString mail);
    void my_clave(QString clave);
    void close_message();
    void on_pb_abrir_ubicacion_en_mapa_clicked();
    void on_cb_editar_fotos_toggled(bool checked);
    void fill_counter_data(QString serie);
    void fill_counter_data_devuelto(QString serie);
    void on_pb_maximizar_clicked();

    void on_pb_minimizar_clicked();

    void on_pb_agregar_cita_clicked();

    void get_time_selected_end(QTime t);
    void get_time_selected_init(QTime t);
    void get_date_selected(QDate d);
    void get_users_selected(QString user);
    bool on_pb_tarea_sin_revisar_clicked();

    void on_pb_eliminar_cita_clicked();

    void on_le_status_tarea_editingFinished();

    void fill_Emplazamiento_data(QString string_completed);
    void fill_Emplazamiento_data_devuelto(QString string_completed);

    void on_le_telefono2_editingFinished();

    void on_le_numero_serie_contador_editingFinished();

    void on_le_numero_serie_contador_devuelto_editingFinished();

    void setear_coordenadas(QString coords);
    void on_pb_ver_pdf_clicked();

    void on_le_marca_devuelta_textChanged(const QString &arg1);

    void on_le_marca_contador_textChanged(const QString &arg1);

    void on_le_numero_serie_contador_devuelto_textChanged(const QString &arg1);

    void on_le_numero_serie_contador_textChanged(const QString &arg1);

    void on_le_nuevo_citas_textChanged(const QString &arg1);

    void on_le_operario_textChanged(const QString &arg1);


    void on_cb_estado_tarea_currentIndexChanged(const QString &arg1);

    void on_le_ubicacion_en_bateria_editingFinished();

    void fill_Resultados_data(QString string_completed);
    void fill_Causas_data(QString string_completed);
    void fill_Resultados();
    void fill_Causas();

    void on_pb_add_pieza_clicked();

    void agregarPiezas(QStringList piezas);
    void on_pb_eliminar_piezas_clicked();

    void onActionPress(QString action);
    void hideMenu(const QString from);
    void setGeoCode(const QString geocode);
    void setGeoCodeHand(const QString geocode);
    void eraseHomeMarker();
    void eraseHandMarker();

    void on_le_causa_origen_textChanged(const QString &arg1);

    void populateDataView();
    void update_tareas_fields_request();

    void on_pb_ITAC_clicked();

    void updateTareasFromServer();
    void changeFoto();
    void updateITACsFromServer();
    void get_team_selected(QString team);
    void checkHibernacionSelected(QString item, QString previous);
    bool hibernateTarea();
    bool updateTareas();
    bool updateITAC(QStringList lista_cod_emplazamientos, QJsonObject campos);
    void update_itacs_fields_request();
    void updateITACsGeoCode();

    void on_pb_add_observacion_clicked();

    void on_pb_erase_observacion_clicked();

    void setObservation(QString obs);
    void on_le_codigo_geolocalizacion_textChanged(const QString &arg1);

    void setGeoCodeByCodEmplazamiento();
    void createAlternativePhotos();
    void on_pb_mostrar_carpeta_clicked();

    void download_audio_request();
    void on_pb_play_audio_clicked();

    void reproducirAudio();
    void initializeMaps();

    void setLoadingText(QString mess);
    void getContadoresList(QString serie ="");
    void on_le_numero_serie_contador_devuelto_textEdited(const QString &arg1);

    void requestContadoresList();
    void on_le_numero_serie_contador_textEdited(const QString &arg1);

    void on_le_numero_interno_textEdited(const QString &arg1);

private:
    Ui::other_task_screen *ui;
    QTimer timerChangingGeoCode;
    static QString last_operario, last_fecha_ejecucion;
    QJsonObject o, tarea_a_actualizar;
    database_comunication database_com;
    bool photo_request,showMesageBox;
    int Script_excecution_result;
    QStringList keys,values;
    QTimer start_moving_screen;
    int init_pos_x;
    int init_pos_y;
    bool first_move = true;

    QCompleter *completer_numeros_serie, *completer_numeros_serie_devueltos, *completer_emplazamientoDV, *completer_emplazamiento, *completer_resultados, *completer_causas;
    QMap<QString, QString> mapaTiposDeClase,mapaTiposDeMarca, mapaTiposDeRestoEmplazamiento, mapaEstados;
    QStringList lista_tipo_fluido, lista_tipo_radio;
    void Pdf_creator(QPixmap Before_installation, QPixmap After_installation, QPixmap Lectura,
                     QPixmap Firma, QPixmap Empresa, QString Direccion, QString Num_abonado,
                     QString tels, QString Observaciones, QString Cambiado, QString Num_serie,
                     QString Calibre, bool incidencia_normal = false, bool show = true);

    void Pdf_printer(QPixmap Before_installation, QPixmap After_installation, QPixmap Lectura, QPixmap Firma, QPixmap Empresa, QString Direccion, QString Num_abonado, QString tels, QString Observaciones, QString Cambiado, QString Num_serie, QString Calibre, bool incidencia_normal = false);

    QString file_download_dir_selected, coordenadas_obtenidas, operario_selected, equipo_selected;
    void highlightMapIcon();
    bool highlight_lineEdits = false;

    int currentPhotoLooking;
    QString get_coordinades_From_Link_GoogleMaps(const QString &link_google_maps);


    void getPhotosLocal();
    void setPhotosLocal();
    QJsonObject checkIfDuplicateByInformation(QJsonObject o);
    void configuraAndCreatePdf(bool show = true);
    bool checkIfExistFoto(QJsonObject o);
    QString guardar_cambios();
    void checkAndFillEmptyField();
    QString changeCausaOrigen(QString anom, bool change_le_causa_origen = false);
    bool geoCodeChanged = false;
    QString observacion_seleccionada="";
    QString lastSerieRequested = "";
    bool devuelto = true;
    bool closing_window = false;
};

#endif // OTHER_TASK_SCREEN_H
