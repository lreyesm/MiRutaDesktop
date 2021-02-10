#ifndef SCREEN_TABLE_RUTAS_H
#define SCREEN_TABLE_RUTAS_H


#include <QMainWindow>
#include "ruta.h"
#include <QStandardItemModel>
#include "database_comunication.h"
#include <QTimer>

namespace Ui {
class Screen_Table_Rutas;
}

class Screen_Table_Rutas : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_Rutas(QWidget *parent = nullptr, bool show = true);
    ~Screen_Table_Rutas();

    bool getRutasFromServer(QString query, int limit, int id_start);
    bool getRutasValuesFieldCustomQueryServer(QString column, QString query);
    bool getRutasValuesFieldServer(QString column);
    bool getRutasAmountFromServer(QString query, int limit = LIMIT);
    void addItemsToPaginationInfo(int sizeShowing);

signals:
    void rutasReceived(database_comunication::serverRequestType);
    void script_excecution_result(int);
    void hidingLoading();
    void setLoadingTextSignal(QString);
    void mouse_pressed();
    void mouse_Release();
    void tablePressed();
    void closing();
    void sectionPressed();

public slots:
    void getRutas();
    void fixModelForTable(QJsonArray);
    void populateTable(database_comunication::serverRequestType tipo);

protected slots:
    void resizeEvent(QResizeEvent *event);
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
    void showEvent(QShowEvent *event);
private slots:
    void on_pb_nueva_clicked();

    void serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_sectionClicked(int logicalIndex);

    void on_pb_next_pagination_clicked();
    void on_pb_previous_pagination_clicked();
    void moveToPage(QString page);
    void checkPaginationButtons();

    void get_rutas_amount_request();
    void get_rutas_request();
    void get_all_column_values_request();
    void get_all_column_values_custom_query_request();

    void updateRutasInTable(bool b = true);
    void setTableView();
    void getMenuSectionClickedItem(int selection);
    void addRemoveFilterList(QString value);
    void filtrarEnTabla(bool checked);
    QStringList getFieldValues(QString field);
    void filterColumnField();
    void showFilterWidgetOptions(bool offset=true);
    void on_drag_screen();
    void on_start_moving_screen_timeout();
    void on_drag_screen_released();
    void on_tableView_pressed(const QModelIndex &index);

    void on_pb_maximize_clicked();
    void on_pb_cruz_clicked();
    void on_pb_minimize_clicked();
    void on_pb_inicio_clicked();

private:
    Ui::Screen_Table_Rutas *ui;
    database_comunication database_com;
    Ruta *oneRutaScreen;
    QStandardItemModel* model  = nullptr;
    bool serverAlredyAnswered =false, connected_header_signal=false;
    QJsonArray jsonArrayInTable, jsonArrayAll, jsonArrayInTableExcel, jsonArrayInTableFiltered;
    QJsonObject jsonObjectValues, jsonInfoRutasAmount;
    int countRutas = 0;


    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
    QJsonArray loadRutasExcel();
    bool checkIfFieldIsValid(QString var);
    bool checkValidJsonObjectFields(QJsonObject jsonObject);
    QMap<QString, QString> mapExcelImport(QStringList listHeaders);
    void cargarDesdeExcel();
    void show_loading(QString mess);
    void setLoadingText(QString mess);
    void hide_loading();
    void fixRutasWrongCode();

    QString defaultQuery = codigo_ruta_rutas +" <> 'NULL'";
    QString lastQuery = defaultQuery;
    int last_id_start = 0;
    int currentPages = 0;
    int currentPage = 1;
    QString currentGestor = "";
    int limit_pagination = LIMIT;

    bool getRutasCustomQuery(QString query, int id_start = 0);
    bool filter_enabled = false;
    QJsonArray fixJsonToLastModel(QJsonArray jsonArray);
    QString getQueyStatus();
    QStringList keys, values;
    QJsonArray getCurrentJsonArrayInTable();
    bool filtering = false;
    int lastSelectedRow = -1;
    QTimer timer;

    QString lastSectionField="", lastSectionCliked="";
    QStringList filterColumnList;
    QPoint lastCursorPos;
    QMap<QString, QString> fillMapForFixModel(QStringList &listHeaders);
    bool solo_tabla_actual=true;
    QString getScrollBarStyle();

    QTimer start_moving_screen;
    int init_pos_x;
    int init_pos_y;
    bool first_move = true;
};

#endif // SCREEN_TABLE_RUTAS_H
