#ifndef SCREEN_TABLE_CLIENTES_H
#define SCREEN_TABLE_CLIENTES_H

#include <QMainWindow>
#include "cliente.h"
#include <QStandardItemModel>
#include "database_comunication.h"

namespace Ui {
class Screen_Table_Clientes;
}

class Screen_Table_Clientes : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_Clientes(QWidget *parent = nullptr, bool show = true, QString empresa = "");
    ~Screen_Table_Clientes();

    void getClientesFromServer(bool view = true);

    void setEmpresa(QString emp);
signals:
    void clientesReceived(database_comunication::serverRequestType);
    void script_excecution_result(int);
    void updateTareas();
    void closing();
public slots:
    void fixModelForTable(QJsonArray);
    void populateTable(database_comunication::serverRequestType tipo);

protected slots:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_pb_nueva_clicked();

    void serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_sectionClicked(int logicalIndex);
    void on_updateTareas();
private:
    Ui::Screen_Table_Clientes *ui;
    database_comunication database_com;
    Cliente *oneClienteScreen;
    QStandardItemModel* model = nullptr;
    bool serverAlredyAnswered = false, connected_header_signal=false;
    QJsonArray jsonArrayAllClientes;
    void setTableView();
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
    QString empresa = "";
};
#endif // SCREEN_TABLE_CLIENTES_H
