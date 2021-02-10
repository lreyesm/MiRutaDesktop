#ifndef SCREEN_TABLE_GESTORES_H
#define SCREEN_TABLE_GESTORES_H

#include <QMainWindow>
#include "gestor.h"
#include <QStandardItemModel>
#include "database_comunication.h"

namespace Ui {
class Screen_Table_Gestores;
}

class Screen_Table_Gestores : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_Gestores(QWidget *parent = nullptr, bool show = true, QString empresa = "");
    ~Screen_Table_Gestores();

    void getGestoresFromServer(bool view = true);

signals:
    void gestoresReceived(database_comunication::serverRequestType);
    void script_excecution_result(int);
    void updateTareas();
public slots:
    void getGestores();
    void fixModelForTable(QJsonArray);
    void populateTable(database_comunication::serverRequestType tipo);
private slots:
    void on_pb_nueva_clicked();

    void serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_sectionClicked(int logicalIndex);
    void on_updateTareas();
private:
    Ui::Screen_Table_Gestores *ui;
    database_comunication database_com;
    Gestor *oneGestorScreen;
    QStandardItemModel* model  = nullptr;
    bool serverAlredyAnswered =false, connected_header_signal=false;
    QJsonArray jsonArrayAllGestores;
    void setTableView();
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
    QString empresa = "";
    QStringList keys, values;
};

#endif // SCREEN_TABLE_GESTORES_H
