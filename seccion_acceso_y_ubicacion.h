#ifndef SECCION_ACCESO_Y_UBICACION_H
#define SECCION_ACCESO_Y_UBICACION_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class Seccion_Acceso_y_Ubicacion;
}

class Seccion_Acceso_y_Ubicacion : public QWidget
{
    Q_OBJECT

public:
    explicit Seccion_Acceso_y_Ubicacion(QWidget *parent = nullptr, QJsonObject jsonObject = QJsonObject());
    ~Seccion_Acceso_y_Ubicacion();

signals:
    void itac_json(QJsonObject);
    void accept();
    void reject();

private slots:
    void on_pb_aceptar_clicked();
    void on_pb_cancelar_clicked();

    void on_rb_Exterior_toggled(bool checked);

    void on_rb_Dentro_de_Edificio_toggled(bool checked);

private:
    Ui::Seccion_Acceso_y_Ubicacion *ui;
    QJsonObject itac;
    void guardarDatos();
    void populateView();
    bool checkIfFieldIsValid(QString var);
    void hideAll();
};

#endif // SECCION_ACCESO_Y_UBICACION_H
