#ifndef SCREEN_TABLA_CONTADORES_H
#define SCREEN_TABLA_CONTADORES_H

#include <QWidget>
#include "counter.h"
#include <QStandardItemModel>
#include <QJsonArray>
#include "structure_contador.h"
#include <QDesktopWidget>
#include "database_comunication.h"

#include "mylineeditshine.h"

namespace Ui {
class Screen_tabla_contadores;
}

class Screen_tabla_contadores : public QWidget
{
    Q_OBJECT

public:
    explicit Screen_tabla_contadores(QWidget *parent = nullptr, QString empresa = "");
    ~Screen_tabla_contadores();

    static QJsonObject getContadorFrom(QJsonArray jsonArray, QString field, QString value);
    bool getContadoresFromServer(QString empresa, QString query, int limit, int id_start);
    bool getContadoresValuesFieldCustomQueryServer(QString empresa, QString column, QString query);
    bool getContadoresValuesFieldServer(QString empresa, QString column);
    bool getContadoresAmountFromServer(QString empresa, QString query, int limit = LIMIT);
    void addItemsToPaginationInfo(int sizeShowing);

    void setLoadingText(QString mess);
signals:
    void setLoadingTextSignal(QString);
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

public slots:
    void getContadores();
    void fixModelForTable(QJsonArray json);
    void populateTable(database_comunication::serverRequestType tipo);
    void get_user_selected(QString u);

    bool updateContadores(QStringList lista_numeros_serie, QJsonObject campos);
protected slots:
    void showEvent(QShowEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event)
    {
        on_pb_maximize_clicked();
        QWidget::mouseDoubleClickEvent(event);
    }
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
        emit closing();
        QWidget::closeEvent(e);
    }
private slots:
    void on_pb_create_new_contador_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_pb_cruz_clicked();
    void on_drag_screen();
    void on_drag_screen_released();
    void on_start_moving_screen_timeout();

    void on_pb_load_contadores_from_excel_clicked();

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
    void on_actionAsignar_a_un_operario_triggered();
    void on_actionAsignar_a_un_equipo_triggered();
    void on_actionAsignar_campos_triggered();
    void updateSelectedFields(QJsonObject campos);
    void getMenuSectionClickedItem(int selection);
    void addRemoveFilterList(QString value);
    void showFilterWidgetOptions(bool offset = true);
    void filterColumnField();
    void delete_contadores_request();
    void on_pb_eliminar_clicked();
    bool deleteContadores(QStringList lista_numeros_serie);
    void filtrarEnTabla(bool checked);

    void get_contadores_amount_request();
    void get_contadores_request();
    void get_all_column_values_request();
    void get_all_column_values_custom_query_request();

    void on_pb_next_pagination_clicked();
    void on_pb_previous_pagination_clicked();
    void moveToPage(QString page);
    void checkPaginationButtons();

    void updateContadoresInTable();
    void on_pb_inicio_clicked();

    void on_pb_export_contadores_to_excel_clicked();

    void addCheckBoxes(QStringList values);
    void triggerGetCheckBoxesValues();
    void updateCheckBoxes(QString value);
private:
    Ui::Screen_tabla_contadores *ui;
//    Counter *contador = nullptr;
    QStringList campos_de_fechas;
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

    void openContadorX(QJsonObject o);
    QJsonArray getCurrentJsonArrayInTable();
    QMap<QString, QString> fillMapForFixModel(QStringList &listHeaders);
    QStringList getFieldValues(QString field, QString searchValue);
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
    void export_jsonArray_to_excel(QJsonArray jsonArray, QMap<QString, QString> mapa_exportacion, QStringList listHeaders);

    QTimer timerAutocomplete;
    QTimer timerAutocompleteCheckBoxes;
    QString searchString = "";
    QWidget *widgetValues;
    QCheckBox *cb_todos = nullptr;
    MyLineEditShine *lineEdit = nullptr;
};

#endif // SCREEN_TABLA_CONTADORES_H
