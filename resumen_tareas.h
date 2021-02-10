#ifndef RESUMEN_TAREAS_H
#define RESUMEN_TAREAS_H

#include <QWidget>

#include <QtCore>
#include <QListWidgetItem>
#include "database_comunication.h"
#include <QGeoCoordinate>

namespace Ui {
class Resumen_Tareas;
}

class Resumen_Tareas : public QWidget
{
    Q_OBJECT

public:
    explicit Resumen_Tareas(QWidget *parent = nullptr, QJsonArray jsonArray = QJsonArray(), QString empresa = "");
    ~Resumen_Tareas();
    void setTareas(QJsonArray jsonArray);
public slots:
    void focusTareaCoords(QJsonArray jsonArray);
    void focusTarea(QString tipo, bool centerMap = true);
    void showWithAnimation();
signals:
    void closing();
    void moveMapCenter(QString);
    void openTarea(QString);
    void updateTareas(QJsonObject);
    void script_excecution_result(int);

    void mouse_pressed();
    void mouse_Release();

protected slots:
    void resizeEvent(QResizeEvent *e);
    void mousePressEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
    {
        Q_UNUSED(e);
        emit mouse_pressed();
    }

    void mouseReleaseEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
    {
        Q_UNUSED(e);
        emit mouse_Release();
    }
private slots:
    void on_pb_cruz_clicked();

    void on_pb_openTarea_clicked();

    void on_pb_hibernar_clicked();

    void setHibernateDate(QDate date);
    void setHibernateTime(QTime time);
    void hibernateTarea();
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    bool updateTareaField();
    void update_tareas_fields_request();
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void setOperarioSelected(QString operario);
    void setEquipoSelected(QString equipo);

    void on_listWidget_tareas_tipo_itemClicked(QListWidgetItem *item);

    void on_listWidget_tareas_emplazamiento_itemClicked(QListWidgetItem *item);

    void on_listWidget_tareas_emplazamiento_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_tareas_tipo_itemDoubleClicked(QListWidgetItem *item);

    void on_drag_screen();
    void on_drag_screen_released();
    void on_start_moving_screen_timeout();

    void on_pb_minimizar_clicked();
private:
    Ui::Resumen_Tareas *ui;
    QJsonArray jsonArrayTareas, jsonArrayTareasEmplazamiento;
    QJsonObject lastTareasSelected;
    void fillListWidgetWithTypes(QJsonArray jsonArray);
    bool checkIfFieldIsValid(QString var);
    QMap<QString, int> mapa_tareas_views;
    QMap<QString, QString> mapa_tipos_geocodes;
    QMap<QString, int> mapa_tipos_description;
    int tipo_t_counter =0;
    QString last_tipo_t_selected="";
    QString getViewOfTarea(QJsonObject jsonObject);
    QDate date_selected;
    QTime time_selected;
    QString operario_selected = "", equipo_selected="";
    database_comunication database_com;
    QStringList keys, values;
    QString empresa = "";

    QString getStringFromCoord(QGeoCoordinate coords);
    QGeoCoordinate getCoordsFromString(QString string, double &xcoord, double &ycoord);
    QListWidgetItem *getItemView(QJsonObject jsonObject, QListWidgetItem *item);
    QListWidgetItem *getItemView(QJsonObject jsonObject);

    QTimer start_moving_screen;
    int init_pos_x;
    int init_pos_y;
    bool first_move = true;

};

#endif // RESUMEN_TAREAS_H
