#ifndef SCREEN_UPLOAD_CONTADORES_H
#define SCREEN_UPLOAD_CONTADORES_H

#include <QMainWindow>
#include "counter.h"
#include <QStandardItemModel>
#include <QJsonArray>
#include "structure_contador.h"
#include <QDesktopWidget>
#include "database_comunication.h"

namespace Ui {
class Screen_Upload_Contadores;
}

class Screen_Upload_Contadores : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Upload_Contadores(QWidget *parent = nullptr, QString empresa = "");
    ~Screen_Upload_Contadores();


signals:
    void mouse_pressed();
    void mouse_Release();
    void sendData(QJsonObject);
    void filledContadores();
    void closedTableContadores();
    void contadoresReceived(database_comunication::serverRequestType);
    void script_excecution_result(int);
    void tablePressed();
    void closing();
    void sectionPressed();
    void hidingLoading();
    void updateContadores();
    void setLoadingTextSignal(QString);

public slots:
    void fixModelForTable(QJsonArray json);
    void populateTable(database_comunication::serverRequestType tipo);
    void get_user_selected(QString u);

    void setLoadingText(QString mess);
private slots:
    void on_pb_create_new_contador_clicked();
    void on_pb_cruz_clicked();
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
    void closeEvent(QCloseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
    {
        Q_UNUSED(e);
        emit closing();
    }
    void on_pb_load_contadores_from_excel_clicked();
    void on_pb_upload_contadores_from_excel_clicked();

    void on_sectionClicked(int logicalIndex);
    void on_pb_asignar_operario_clicked();

    void on_pb_maximize_clicked();

    void on_pb_minimize_clicked();
    void setTableView();

    void serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo);
    void update_contadores_fields_request();
    void on_tableView_pressed(const QModelIndex &index);

    void getMenuClickedItem(int selected);
    void get_equipo_selected(QString u);

    void getMenuSectionClickedItem(int selection);
    void addRemoveFilterList(QString value);
    void showFilterWidgetOptions(bool offset = true);
    void filterColumnField();
    void delete_contadores_request();
    void on_pb_eliminar_clicked();
    bool deleteContadores(QStringList lista_numeros_serie);
    void filtrarEnTabla(bool checked);

private:
    Ui::Screen_Upload_Contadores *ui;
//    Counter *contador = nullptr;
    database_comunication database_com;
    other_task_screen *oneTareaScreen=nullptr;
    QStandardItemModel* model;
    QJsonArray jsonArrayInTable, jsonArrayAll, jsonArrayInTableExcel, jsonArrayInTableFiltered;
    QJsonObject jsonObjectValues, jsonInfoContadoresAmount;
    int countContadores = 0;
    int lastSelectedRow = -1;

    bool serverAlredyAnswered = false;
    QTimer start_moving_screen;
    int init_pos_x;
    int init_pos_y;
    bool first_move = true;
    void hide_loading();
    void show_loading(QString mess);
    QJsonArray parse_to_QjsonArray(QString rutaToDATFile);
    QMap <QString,QString> mapa_contador;
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
    bool checkIfFieldIsValid(QString var);
    bool connected_header_signal = false, lastOrderString =false;
    QString operatorName="", equipoName="";
    QString empresa = "";
    database_comunication database_comunication;
    QStringList keys, values;

    QJsonArray getCurrentJsonArrayInTable();
    QMap<QString, QString> fillMapForFixModel(QStringList &listHeaders);
    QStringList getFieldValues(QString field);
    bool filtering= false;
    QString lastSectionField= "", lastSectionCliked= "";
    QStringList filterColumnList;
    void ordenarPor(QString field, int type);
    void ordenarPor(QString field, QString type);
    QPoint lastCursorPos;
    QString getScrollBarStyle();
    bool solo_tabla_actual = true;

    QString defaultQuery = numero_serie_contadores +" <> 'NULL'";
    QString lastQuery = defaultQuery;
    int last_id_start = 0;
    int currentPages = 0;
    int currentPage = 1;
    QString currentGestor = "";
    int limit_pagination = LIMIT;

    bool getContadoresCustomQuery(QString query, int id_start = 0);
    bool filter_enabled = false;
    QJsonArray fixJsonToLastModel(QJsonArray jsonArray);
    QString getQueyStatus();
};

#endif // SCREEN_UPLOAD_CONTADORES_H
