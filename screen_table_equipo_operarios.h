#ifndef SCREEN_TABLE_EQUIPO_OPERARIOS_H
#define SCREEN_TABLE_EQUIPO_OPERARIOS_H

#include <QMainWindow>
#include "equipo_operario.h"
#include <QStandardItemModel>
#include "database_comunication.h"

namespace Ui {
class Screen_Table_Equipo_Operarios;
}

class Screen_Table_Equipo_Operarios : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_Equipo_Operarios(QWidget *parent = nullptr, bool show = true, QString empresa="");
    ~Screen_Table_Equipo_Operarios();

    void getEquipo_OperariosFromServer(bool view = true);

signals:
    void equipo_operariosReceived(database_comunication::serverRequestType);
    void script_excecution_result(int);
    void updateTareas();
public slots:
    void fixModelForTable(QJsonArray);
    void populateTable(database_comunication::serverRequestType tipo);
private slots:
    void on_pb_nueva_clicked();

    void serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_sectionClicked(int logicalIndex);
    void on_updateTareas();
private:
    Ui::Screen_Table_Equipo_Operarios *ui;
    database_comunication database_com;
    Equipo_Operario *oneEquipo_OperarioScreen;
    QStandardItemModel* model  = NULL;
    bool serverAlredyAnswered =false, connected_header_signal=false;
    QJsonArray jsonArrayAllEquipo_Operarios;
    void setTableView();
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
    QString empresa="";
};

#endif // SCREEN_TABLE_EQUIPO_OPERARIOS_H
