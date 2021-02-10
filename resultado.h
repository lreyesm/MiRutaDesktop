#ifndef RESULTADO_H
#define RESULTADO_H


#include <QWidget>
#include "structure_resultado.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Resultado;
}

class Resultado : public QWidget
{
    Q_OBJECT

public:
    explicit Resultado(QWidget *parent = nullptr, bool newOne = true);
    ~Resultado();

    static QJsonArray readResultados();
    static bool writeResultados(QJsonArray resultados);
    static QStringList getListaResultados();
signals:
    void script_excecution_result(int);
    void update_tableResultados(bool);

public slots:
    void setData(QJsonObject o){
        resultado = o;
        populateView(resultado);
    };
    void populateView(QJsonObject o);

    void delete_resultado_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_resultado_request(QStringList keys, QStringList values);
    void update_resultado_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

private:
    Ui::Resultado *ui;
    QJsonObject resultado;
    database_comunication database_com;
    bool subirResultado(QString codigo);
    QString guardarDatos();
    void subirTodasLasResultados();
    bool eliminarResultado(QString cod);
};

#endif // RESULTADO_H
