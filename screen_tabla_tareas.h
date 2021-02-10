#ifndef SCREEN_TABLA_TAREAS_H
#define SCREEN_TABLA_TAREAS_H

#include <QDialog>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardItemModel>
#include "database_comunication.h"
#include "other_task_screen.h"
#include <operator_selection_screen.h>
#include "fields_to_assign.h"
#include "calendardialog.h"

namespace Ui {
class screen_tabla_tareas;
}

class screen_tabla_tareas : public QDialog
{
    Q_OBJECT

public:
    explicit screen_tabla_tareas(QWidget *parent = nullptr, QString empresa = "");
    ~screen_tabla_tareas();
    QString get_current_date_in_format(QDate date = QDate::currentDate(),
                                              QTime time_init = QTime::currentTime(),
                                              QTime time_end = QTime::currentTime());
    static int lastIDSAT;
    static int lastNUMFICHERO_EXPORTACION;
    static int lastIDExp;
    static int emailPermission;

    static QString lastSync;


    static QString parse_tipo_tarea(QString anomalia, QString calibre, QString marca = "");
    static bool checkIfFieldIsValid(QString var);

    static void readVariablesInBD();
    static void writeVariablesInBD();
    QString composeNumExp(int num);
    QStandardItemModel* getModel(){return model;}
    QJsonArray getAllTask(){return jsonArrayAllTask;}
    database_comunication database_com;
    QStringList keys,values;

    void setRutaFile(QString dir);
    static bool checkValidDirFields(QJsonObject jsonObject);
    void insertTareasInSQLite(QJsonArray jsonArray);
    bool compareJsonObjectByDateModified(QJsonObject jsonObject_old, QJsonObject jsonObject_new);

    QJsonObject checkIfDuplicateByInformation(QJsonObject o);
signals:
    void setLoadingTextSignal(QString);
    void hidingLoading();
    void sendData(QJsonObject);
    void script_excecution_result(int);
    void upload_finished();
    void task_delete_excecution_result(int);
    void updateOtherTable();
    void mouse_pressed();
    void mouse_Release();
    void closing();

public slots:
    bool on_pb_load_data_from_file_clicked();

    void set_file_type(int t)
    {
        file_type = t;
    }
    void set_tipo_orden(QString o)
    {
        selection_Order = o;
    }


    void show_loading(QString mess = "Cargando");

    void hide_loading();

    void get_fields_selected(QMap<QString, QString> map_received);
protected slots:
    void resizeEvent(QResizeEvent *event);

    void showEvent(QShowEvent *event);
private slots:
    void serverAnswer(QByteArray, database_comunication::serverRequestType);
    void populateTable(/*QByteArray, */database_comunication::serverRequestType);
    void on_tableView_doubleClicked(const QModelIndex &index);

    QJsonArray parse_to_QjsonArray(QString path);

    QString fill_with_spaces(QString str, int size);
    QString parse_calibre(QString anomalia, QString calibre);
    void on_pb_load_all_task_to_server_clicked();
    void on_pb_insert_task_to_specific_operator_clicked();
    void get_user_selected(QString);
    void on_pb_insert_task_to_one_operator_clicked();
    void on_pb_insert_all_tasks_to_one_operator_clicked();
    void on_pb_export_to_excel_clicked();

    void conection_timeout()
    {
        Script_excecution_result = database_comunication::script_result::timeout;
    }
    void update_script_execution_result(int b)
    {
        Script_excecution_result = b;
        emit upload_finished();
    }
    void to_test(int b)
    {
        Script_excecution_result = b;
    }

    void on_pb_close_clicked();

    void delete_one_task()
    {
        database_com.serverRequest(database_comunication::serverRequestType::DELETE_TAREA,keys,values);
    }

    void on_drag_screen();
    void on_drag_screen_released();
    void on_start_moving_screen_timeout();

    void mousePressEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
    {
        Q_UNUSED(e);
        emit mouse_pressed();
    }

    void mouseReleaseEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
    {
        Q_UNUSED(e);
        emit mouse_Release();
    }

    void on_pb_assign_fields_to_multiple_tasks_clicked();

    void get_order_selected(QString);
    void get_date_selected(QDate);

    void on_cb_overwrite_tasks_clicked(bool checked);

    void on_pb_id_orden_inicial_clicked();

    void updateIDOrdenes(QString id_orden);
    void setTableView();
    void setLoadingText(QString mess);
    void on_pb_minimize_clicked();
    void on_pb_maximize_clicked();
    void on_pb_cruz_clicked();
private:
    QString selection_Order, selected_status, selected_status_2;
    QDate selection_date;
    QString get_date_from_status(QJsonObject object, QString status);
    QJsonObject set_date_from_status(QJsonObject object, QString status, QString date);
    QString setDirToExplorer();
    QJsonObject buscarCitasEnObservaciones(QJsonObject jsonObject);
    QJsonObject setHora(QJsonObject jsonObject, QString hora_string, QString string_before_time, bool second_hour, QString hora_second);
    QJsonObject setFechaHora(QJsonObject jsonObject, QString fecha_string, QString hora_string, QString string_before_time, bool second_hour, QString hora_second);
    QMap <QString,QString> map_days_week;
    QJsonObject buscarGeolocalizacion(QJsonObject jsonObject);

    QJsonObject buscarTelefonosEnObservaciones(QJsonObject jsonObject);
    QJsonObject buscarTelefonosEnObservaciones_excel(QJsonObject jsonObject);
    bool checkIfAllNumbers(QString string);
    void fixModelForTable(QJsonArray jsonArray);

    bool isUpdateNeeded(QJsonObject jsonObject_viejo, QJsonObject jsonObject_nuevo);
    QJsonObject get_JObject_from_JArray(QJsonArray jsonArray, QString numIn);

    QString obtenerPrefijoDeSerie(QString serie);
    QMap<QString, QString> mapExcelImport(QStringList listHeaders);

    QString empresa = "";
    bool make_replacement=false;
    Ui::screen_tabla_tareas *ui;
    Operator_Selection_Screen *seleccionOperarioScreen =nullptr;
    Fields_to_Assign *Fields_to_Assign_Dialog;
    QJsonArray jsonArrayAllTask, jsonArrayAllTaskCopySaved, jsonArrayDoneTasks,
    jsonArrayOneOperatorTasks, jsonArrayAllPrincipalVariables,
    jsonArrayFilteredTasks, jsonArrayAllTaskinServer;
    QStringList modifieds_dates;
    QStandardItemModel* model;
    bool serverAlredyAnswered, loaded_from_file;

    int Script_excecution_result;
    char jsonArraySelected;
    char jsonArrayLastSelected=0;
    QString rutaToDATFile,operatorName, tipo_orden = "DIARIAS";

    QMap <QString,QString> map, mapa_tipos_tareas;

    QMap <int,QString> map_months;

    void populate_map();
    QTimer start_moving_screen;
    int init_pos_x;
    int init_pos_y;
    bool first_move = true;

    QMap <QString,QString> fields_selected;
    int file_type = -1;
    QTimer timer;

};

#endif // SCREEN_TABLA_TAREAS_H
