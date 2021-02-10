#ifndef RESTRICTION_H
#define RESTRICTION_H

#include <QWidget>
#include <QJsonDocument>
#include "database_comunication.h"
#include "global_variables.h"

namespace Ui {
class Restriction;
}

class Restriction : public QWidget
{
    Q_OBJECT

public:
    explicit Restriction(QWidget *parent = nullptr, QString empresa = "");
    ~Restriction();

signals:
    void script_excecution_result(int);
    void selectedRestriction(QString field, QString value);
    void hidingLoading();
private slots:
    void on_pb_aceptar_clicked();

    void on_pb_cancelar_clicked();

    void fillValuesWithColumn(QString column);
    void serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo);
    void get_all_column_values_request();
    void getColumnValues();
    void fillValues();

private:
    Ui::Restriction *ui;
    QMap<QString, QString> fillMap();
    void fillSpinnerColumn();
    database_comunication database_com;
    QJsonObject jsonObjectAnswer;
    QStringList keys, values;

    QString empresa="";
    void show_loading(QWidget *parent, QPoint pos, QColor color = color_blue_app, int w = 40, int h = 40);
};

#endif // RESTRICTION_H
