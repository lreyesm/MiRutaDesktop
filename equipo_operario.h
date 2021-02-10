#ifndef EQUIPO_OPERARIO_H
#define EQUIPO_OPERARIO_H

#include <QWidget>
#include "structure_equipo_operario.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "database_comunication.h"

namespace Ui {
class Equipo_Operario;
}

class Equipo_Operario : public QWidget
{
    Q_OBJECT

public:
    explicit Equipo_Operario(QWidget *parent = nullptr, bool newOne = true, QString empresa="");
    ~Equipo_Operario();

    static QJsonArray readEquipo_Operarios();
    static bool writeEquipo_Operarios(QJsonArray equipo_operarios);
    static QStringList getListaEquipo_Operarios();
    static QStringList getListaNombresEquipo_Operarios();
signals:
    void script_excecution_result(int);
    void update_tableEquipo_Operarios(bool);
    void updateTareas();
public slots:
    void setData(QJsonObject o){
        equipo_operario = o;
        populateView(equipo_operario);
    };
    void populateView(QJsonObject o);

    void delete_equipo_operario_request(QStringList keys, QStringList values);
private slots:
    void on_pb_agregar_clicked();

    void on_pb_actualizar_clicked();

    void create_equipo_operario_request(QStringList keys, QStringList values);
    void update_equipo_operario_request(QStringList keys, QStringList values);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void on_pb_borrar_clicked();

    void on_l_add_operario_clicked();

    void on_l_delete_operario_clicked();

    void checkOperarioTeam(QString user);
    void on_le_codigo_textEdited(const QString &arg1);

    void update_tareas_fields_request();
private:
    Ui::Equipo_Operario *ui;
    QJsonObject equipo_operario;
    database_comunication database_com;
    bool subirEquipo_Operario(QString codigo);
    QString guardarDatos();
    void subirTodasLasEquipo_Operarios();
    bool eliminarEquipo_Operario(QString cod);
    QString empresa = "";
    bool update_fields(QStringList numeros_internos_list, QJsonObject campos);
    QStringList keys, values;
    bool update_tareas = false;
    QString old_equipo="";
};

#endif // EQUIPO_OPERARIO_H
