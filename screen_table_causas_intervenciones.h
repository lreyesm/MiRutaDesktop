#ifndef SCREEN_TABLE_CAUSAS_INTERVENCIONES_H
#define SCREEN_TABLE_CAUSAS_INTERVENCIONES_H
#include <QMainWindow>
#include "causa.h"
#include <QStandardItemModel>
#include "database_comunication.h"

namespace Ui {
class Screen_Table_Causas_Intervenciones;
}

class Screen_Table_Causas_Intervenciones : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_Causas_Intervenciones(QWidget *parent = nullptr, bool show = true);
    ~Screen_Table_Causas_Intervenciones();

    void getCausasFromServer(bool view = true);

signals:
    void causasReceived(database_comunication::serverRequestType);
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
    Ui::Screen_Table_Causas_Intervenciones *ui;
    database_comunication database_com;
    Causa *oneCausaScreen;
    QStandardItemModel* model  = NULL;
    bool serverAlredyAnswered =false, connected_header_signal=false;
    QJsonArray jsonArrayAllCausas;
    void setTableView();
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
};

#endif // SCREEN_TABLE_CAUSAS_INTERVENCIONES_H
