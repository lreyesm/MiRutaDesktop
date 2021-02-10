#ifndef SCREEN_TABLE_PIEZAS_H
#define SCREEN_TABLE_PIEZAS_H
#include "pieza.h"
#include <QMainWindow>
#include "database_comunication.h"
#include <QStandardItemModel>
#include <QTimer>

namespace Ui {
class Screen_Table_Piezas;
}

class Screen_Table_Piezas : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_Piezas(QWidget *parent = nullptr, bool show = true, bool adding_l = false);
    ~Screen_Table_Piezas();

    void addAlreadyAssingPiezas(QString piezas);
public slots:
    void getPiezasFromServer(bool view = true);

signals:
    void piezasReceived(database_comunication::serverRequestType);
    void script_excecution_result(int);
    void add_piezas(QStringList);

protected slots:
    void resizeEvent(QResizeEvent *event);
private slots:
    void on_pb_nueva_pieza_clicked();

    void populateTable(database_comunication::serverRequestType tipo);
    void setTableView();
    void on_sectionClicked(int logicalIndex);
    void serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo);
    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pb_add_pieza_clicked();

    void getPiezaAdded(const QString pieza_added);
    void on_pb_eliminar_todas_clicked();

    void on_pb_eliminar_una_clicked();

private:
    Ui::Screen_Table_Piezas *ui;
    database_comunication database_com;
    Pieza *onePiezaScreen;
    QJsonArray jsonArrayAllPiezas;
    QStandardItemModel* model  = nullptr;
    bool serverAlredyAnswered =false, connected_header_signal=false;
    void fixModelForTable(QJsonArray jsonArray);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, QString type);
    QJsonArray ordenarPor(QJsonArray jsonArray, QString field, int type);
    QTimer timer;
    bool adding = false;
};

#endif // SCREEN_TABLE_PIEZAS_H
