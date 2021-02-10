#ifndef CALIBRE_H
#define CALIBRE_H


#include <QWidget>
#include "structure_calibre.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Calibre;
}

class Calibre : public QWidget
{
    Q_OBJECT

public:
    explicit Calibre(QWidget *parent = nullptr, bool newOne = true);
    ~Calibre();

    static QJsonArray readCalibres();
    static bool writeCalibres(QJsonArray calibres);
    static QStringList getListaCalibres();
signals:
    void script_excecution_result(int);
    void update_tableCalibres(bool);

public slots:
    void setData(QJsonObject o){
        calibre = o;
        populateView(calibre);
    };
    void populateView(QJsonObject o);

    void delete_calibre_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_calibre_request(QStringList keys, QStringList values);
    void update_calibre_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Calibre *ui;
    QJsonObject calibre;
    database_comunication database_com;
    bool subirCalibre(QString codigo);
    QString guardarDatos();
    void subirTodasLasCalibres();
    bool eliminarCalibre(QString cod);
};

#endif // CALIBRE_H
