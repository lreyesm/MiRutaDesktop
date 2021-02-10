#ifndef OPERATOR_SELECTION_SCREEN_H
#define OPERATOR_SELECTION_SCREEN_H

#include <QDialog>
#include <QTimer>
#include <database_comunication.h>

namespace Ui {
class Operator_Selection_Screen;
}

class Operator_Selection_Screen : public QDialog
{
    Q_OBJECT

public:
    explicit Operator_Selection_Screen(QWidget *parent = 0, QString empresa = "");
    ~Operator_Selection_Screen();

    void moveCenter();

public slots:
    void getOperariosFromServer();

signals:
    void OperatorsFilled(database_comunication::serverRequestType);
    void user(QString);

    void mouse_pressed();
    void mouse_Release();

private:
    Ui::Operator_Selection_Screen *ui;
    database_comunication database_com;
    QJsonArray jsonArray;

    QTimer start_moving_screen;
    int init_pos_x;
    int init_pos_y;
    bool first_move = true;
    QString empresa = "";

private slots:
    void serverAnswer(QByteArray, database_comunication::serverRequestType);
    void populateComboBox(database_comunication::serverRequestType);
    void on_buttonBox_accepted();
    void on_pb_close_clicked();

    void on_drag_screen();
    void on_drag_screen_released();
    void on_start_moving_screen_timeout();

    void mousePressEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
    {
        Q_UNUSED(e);
//        emit mouse_pressed();
    }

    void mouseReleaseEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
    {
        Q_UNUSED(e);
//        emit mouse_Release();
    }
    void on_pb_aceptar_clicked();
    void on_pb_cancelar_clicked();
};

#endif // OPERATOR_SELECTION_SCREEN_H
