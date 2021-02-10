#ifndef INFO_H
#define INFO_H


#include <QWidget>
#include "structure_info.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Info;
}

class Info : public QWidget
{
    Q_OBJECT

public:
    explicit Info(QWidget *parent = nullptr, bool newOne = true, QString empresa = "");
    ~Info();

    static QJsonArray readInfos();
    static bool writeInfos(QJsonArray infos);
    static QStringList getListaInfos();
signals:
    void script_excecution_result(int);
    void update_tableInfos(bool);

public slots:
    void setData(QJsonObject o){
        info = o;
        populateView(info);
    };
    void populateView(QJsonObject o);

    void delete_info_request(QStringList keys, QStringList values);
    void actualizarInfoInServer(QJsonObject o);
    bool getInfoInServer();
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_info_request(QStringList keys, QStringList values);
    void update_info_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

    void get_info_request();
private:
    Ui::Info *ui;
    QJsonObject info;
    database_comunication database_com;
    bool subirInfo(QString codigo);
    QString guardarDatos();
    void subirTodasLasInfos();
    bool eliminarInfo(QString cod);
    QString empresa = "";
};


#endif // INFO_H
