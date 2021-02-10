#ifndef SCREEN_TABLE_MARCAS_H
#define SCREEN_TABLE_MARCAS_H

#include <QMainWindow>
#include "marca.h"
#include <QStandardItemModel>
#include "database_comunication.h"

namespace Ui {
class Screen_Table_Marcas;
}

class Screen_Table_Marcas : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_Marcas(QWidget *parent = nullptr, bool show = true);
    ~Screen_Table_Marcas();

    void getMarcasFromServer(bool view = true);

signals:
    void marcasReceived(database_comunication::serverRequestType);
    void script_excecution_result(int);

public slots:
    void fixModelForTable(QJsonArray);
    void populateTable(database_comunication::serverRequestType tipo);
private slots:
    void on_pb_nueva_clicked();

    void serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_sectionClicked(int logicalIndex);
private:
    Ui::Screen_Table_Marcas *ui;
    database_comunication database_com;
    Marca *oneMarcaScreen;
    QStandardItemModel* model  = NULL;
    bool serverAlredyAnswered =false, connected_header_signal=false;
    QJsonArray jsonArrayAllMarcas;
    void setTableView();
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
};

#endif // SCREEN_TABLE_MARCAS_H
