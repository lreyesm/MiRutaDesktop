#ifndef SCREEN_TABLE_OBSERVACIONES_H
#define SCREEN_TABLE_OBSERVACIONES_H

#include <QMainWindow>
#include "observacion.h"
#include <QStandardItemModel>
#include "database_comunication.h"
#include <QTimer>

namespace Ui {
class Screen_Table_Observaciones;
}

class Screen_Table_Observaciones : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_Observaciones(QWidget *parent = nullptr, bool show = true);
    ~Screen_Table_Observaciones();

    void getObservacionesFromServer(bool view = true);

signals:
    void observacionesReceived(database_comunication::serverRequestType);
    void script_excecution_result(int);

public slots:
    void fixModelForTable(QJsonArray);
    void populateTable(database_comunication::serverRequestType tipo);
protected slots:
    void resizeEvent(QResizeEvent *event);
private slots:
    void on_pb_nueva_clicked();
    void setTableView();
    void serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo);
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_sectionClicked(int logicalIndex);
private:
    Ui::Screen_Table_Observaciones *ui;
    database_comunication database_com;
    Observacion *oneObservacionScreen;
    QStandardItemModel* model  = NULL;
    bool serverAlredyAnswered =false, connected_header_signal=false;
    QJsonArray jsonArrayAllObservaciones;

    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
    QTimer timer;
};

#endif // SCREEN_TABLE_OBSERVACIONES_H
