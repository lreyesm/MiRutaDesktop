#ifndef SCREEN_TABLE_INFOS_H
#define SCREEN_TABLE_INFOS_H

#include <QMainWindow>
#include "info.h"
#include <QStandardItemModel>
#include "database_comunication.h"

namespace Ui {
class Screen_Table_Infos;
}

class Screen_Table_Infos : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_Infos(QWidget *parent = nullptr, bool show = true, QString empresa = "");
    ~Screen_Table_Infos();

    void getInfosFromServer(bool view = true);

signals:
    void infosReceived(database_comunication::serverRequestType);
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
    Ui::Screen_Table_Infos *ui;
    database_comunication database_com;
    Info *oneInfoScreen;
    QStandardItemModel* model  = nullptr;
    bool serverAlredyAnswered =false, connected_header_signal=false;
    QJsonArray jsonArrayAllInfos;
    void setTableView();
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
    QString empresa = "";
};

#endif // SCREEN_TABLE_INFOS_H
