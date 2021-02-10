#ifndef SCREEN_TABLE_ITACS_H
#define SCREEN_TABLE_ITACS_H

#include <QMainWindow>
#include "itac.h"
#include <QStandardItemModel>
#include "database_comunication.h"
#include <QTimer>

namespace Ui {
class Screen_Table_ITACs;
}

class Screen_Table_ITACs : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_ITACs(QWidget *parent = nullptr, bool show = true, QString empresa = "", QString gestor = "");
    ~Screen_Table_ITACs();
    bool getItacsFromServer(QString empresa, QString query, int limit, int id_start);
    bool getItacsValuesFieldCustomQueryServer(QString empresa, QString column, QString query);
    bool getItacsValuesFieldServer(QString empresa, QString column);
    bool getItacsAmountFromServer(QString empresa, QString query, int limit = LIMIT);
    void addItemsToPaginationInfo(int sizeShowing);
    QString getQueyStatus();

signals:
    void hidingLoading();
    void setLoadingTextSignal(QString);
    void itacsReceived(database_comunication::serverRequestType);
    void script_excecution_result(int);
    void updateTableTareas();
    void tablePressed();
    void closing();
    void sectionPressed();
    void mouse_pressed();
    void mouse_Release();

protected slots:
    void mousePressEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event)
    {
        on_pb_maximize_clicked();
        QWidget::mouseDoubleClickEvent(event);
    }
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

    void mouseReleaseEvent(QMouseEvent *e);
public slots:
    void getITACs();
    void fixModelForTable(QJsonArray);
    void populateTable(database_comunication::serverRequestType tipo);
    void setGestor(QString);
    void updateItacsInTable();

private slots:
    void on_drag_screen();
    void on_drag_screen_released();
    void on_start_moving_screen_timeout();
    void setTableView();
    void on_pb_nueva_clicked();

    void serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_sectionClicked(int logicalIndex);
    void update_Table_Tareas();

    void update_itacs_fields_request();
    bool updateITACs(QStringList lista_cod_emplazamientos, QJsonObject campos);
    void on_tableView_pressed(const QModelIndex &index);

    void getMenuClickedItem(int selected);
    void on_actionAsignar_a_un_operario_triggered();
    void on_actionAsignar_a_un_equipo_triggered();
    void get_equipo_selected(QString u);
    void get_user_selected(QString u);
    void filterColumnField();
    void getMenuSectionClickedItem(int selection);
    void addRemoveFilterList(QString value);
    void showFilterWidgetOptions(bool offset = true);
    void on_actionAsignar_campos_triggered();
    void updateSelectedFields(QJsonObject campos);
    void on_pb_eliminar_clicked();
    void on_actionDescargar_Fotos();
    void on_actionMostrarEnMapa();
    void update_Table_ITACs();
    void openItacX(QString cod_emplazamiento);
    void setJsonArrayFilterbyPerimeter(QJsonArray jsonArray);
    void filtrarEnTabla(bool checked);
    void get_itacs_amount_request();
    void get_itacs_request();
    void get_all_column_values_request();
    void get_all_column_values_custom_query_request();
    void on_pb_next_pagination_clicked();

    void on_pb_previous_pagination_clicked();

    void moveToPage(QString page);
    void checkPaginationButtons();
    void setLoadingText(QString mess);
    void hide_loading();
    void on_pb_maximize_clicked();
    void on_pb_minimize_clicked();
    void on_pb_cruz_clicked();
    void on_pb_inicio_clicked();

    void on_pb_export_data_clicked();

private:
    Ui::Screen_Table_ITACs *ui;
    database_comunication database_com;
    ITAC *oneITACScreen;
    QStandardItemModel* model = nullptr;
    bool serverAlredyAnswered =false, connected_header_signal=false;
    QJsonArray jsonArrayInTable, jsonArrayAll, jsonArrayInTableFiltered;
    QJsonObject jsonObjectValues, jsonInfoItacsAmount;
    int countItacs = 0;

    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
    QString empresa = "";
    QString gestor = "";
    QStringList keys, values;
    void abrirItacX(int index);
    QString operatorName = "";
    QString equipoName = "";
    QJsonArray getCurrentJsonArrayInTable();
    QMap<QString, QStringList> fillMapForFixModel(QStringList &listHeaders);
    QStringList getFieldValues(QStringList fields);
    bool filtering = false;
    bool checkIfFieldIsValid(QString var);
    QStringList filterColumnList;
    QString lastSectionCliked;
    QStringList lastSectionFields;
    QPoint lastCursorPos;
    QString getScrollBarStyle();
    QTimer timer;
    QJsonArray ordenarPor(QJsonArray jsonArray, QStringList fields, QString type);
    bool solo_tabla_actual = true;

    QString defaultQuery = codigo_itac_itacs +" <> 'NULL'";
    QString lastQuery = defaultQuery;
    int last_id_start = 0;
    int currentPages = 0;
    int currentPage = 1;
    QString currentGestor = "";
    int limit_pagination = LIMIT;

    bool getItacsCustomQuery(QString query, int id_start = 0);
    bool filter_enabled = false;
    void show_loading(QString mess);
    QJsonArray fixJsonToLastModel(QJsonArray jsonArray);
    int lastSelectedRow = -1;

    QTimer start_moving_screen;
    int init_pos_x;
    int init_pos_y;
    bool first_move = true;
    QString crearFicheroTxtConTabla(QJsonArray jsonArray, QString ruta_y_nombre_file);
    QString crearFicheroDATConTabla(QJsonArray jsonArray, QString ruta_y_nombre_file);
};


#endif // SCREEN_TABLE_ITACS_H
