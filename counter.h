#ifndef COUNTER_H
#define COUNTER_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "other_task_screen.h"
#include <QDesktopWidget>
namespace Ui {
class Counter;
}

class Counter : public QWidget
{
    Q_OBJECT


public:
    explicit Counter(QWidget *parent = nullptr, QString empresa = "");
    ~Counter();

    QJsonArray fixToView(QJsonArray jsonarray);
    QJsonObject populateFixView(QJsonObject o);
    QMap<QString, QString> getMapaMarcas(){return mapaTiposDeMarca;}
    QMap<QString, QString> getMapaClases(){return mapaTiposDeClase;}

    void setOperariosDisponibles(QStringList operarios);
    static QString eliminarNumerosAlFinal(QString string);
    static QString eliminarCharacteresAlFinal(QString string);

    static bool writeCounters(QJsonArray jsonArray);
    static QJsonArray readCounters();

    static bool writeSeries(QStringList list);
    static QStringList readSeries();
    static QStringList getSeriesWithJsonArray(QJsonArray jsonArray);
    bool getAllSerialNumbers();
signals:
    void setLoadingTextSignal(QString);
    void hidingLoading();
    void sendData(QJsonObject);
    void updateTablecontadores();

    void mouse_pressed();
    void mouse_Release();
    void script_excecution_result(int);
    void closing();

public slots:
    void populateView(QJsonObject o);
    void update_contador_request(QStringList keys, QStringList values);
    void create_contador_request(QStringList keys, QStringList values);

protected slots:
    void initializeMaps();
    void closeEvent(QCloseEvent *event);
private slots:
    void setLoadingText(QString mess);
    void on_pb_create_new_or_update_contador_clicked();

    void on_pb_cruz_clicked();


    void on_drag_screen();
    void on_drag_screen_released();
    void on_start_moving_screen_timeout();

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
    void on_radioButton_clicked();


    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void get_all_serial_numbers_request();
private:
    Ui::Counter *ui;
    QJsonObject contador;
    database_comunication database_com;
    QMap<QString, QString> mapaTiposDeClase,mapaTiposDeMarca;
    QStringList lista_tipo_fluido, lista_tipo_radio, keys, values;


    QTimer start_moving_screen;
    int init_pos_x;
    int init_pos_y;
    bool first_move = true;
    void show_loading(QString mess);
    void hide_loading();
    bool subirContador(QString serie);
    int getFirstLetterPosition(QString string);
    QString empresa;

    bool checkIfFieldIsValid(QString var);

};

#endif // COUNTER_H
