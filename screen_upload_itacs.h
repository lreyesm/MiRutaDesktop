#ifndef SCREEN_UPLOAD_ITACS_H
#define SCREEN_UPLOAD_ITACS_H

#include <QMainWindow>
#include "itac.h"
#include <QStandardItemModel>
#include "database_comunication.h"
#include <QTimer>

namespace Ui {
class Screen_Upload_Itacs;
}

class Screen_Upload_Itacs : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Upload_Itacs(QWidget *parent = nullptr, QString empresa = "");
    ~Screen_Upload_Itacs();

    static QJsonArray getJsonArrayInFile(QString filename);
    void getITACsFromFile(QString filename);
    QJsonArray deleteItacsFromJsonArray(QJsonArray jsonArray, QStringList codigos_itacs);
private slots:
    void on_pb_subir_clicked();

signals:
    void itacsReceived(database_comunication::serverRequestType);
    void script_excecution_result(int);
    void updateTableTareas();
    void tablePressed();
    void closing();
    void mouse_pressed();
    void sectionPressed();

protected slots:
    void mousePressEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);

    void resizeEvent(QResizeEvent *event);
public slots:
    void fixModelForTable(QJsonArray);
    void populateTable(database_comunication::serverRequestType tipo);
    void setGestor(QString);
private slots:
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
    void openItacX(QString cod_emplazamiento);
    void setJsonArrayFilterbyPerimeter(QJsonArray jsonArray);
    void filtrarEnTabla(bool checked);
private:
    Ui::Screen_Upload_Itacs *ui;
    database_comunication database_com;
    ITAC *oneITACScreen;
    QStandardItemModel* model = nullptr;
    bool serverAlredyAnswered =false, connected_header_signal=false;
    QJsonArray jsonArrayAllITACs, jsonArrayInTableFiltered;

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

};

#endif // SCREEN_UPLOAD_ITACS_H
