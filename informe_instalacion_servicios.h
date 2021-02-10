#ifndef INFORME_INSTALACION_SERVICIOS_H
#define INFORME_INSTALACION_SERVICIOS_H

#include <QMainWindow>
#include <QJsonObject>

namespace Ui {
class Informe_Instalacion_Servicios;
}

class Informe_Instalacion_Servicios : public QMainWindow
{
    Q_OBJECT

public:
    explicit Informe_Instalacion_Servicios(QWidget *parent = nullptr, QString empresa = "");
    ~Informe_Instalacion_Servicios();

    QString nullity_check(QString q);
    bool checkIfFieldIsValid(QString var);
    QStringList getCheckBoxeSuministros();
    void setCheckBoxesSuministros(QStringList list);
    void setCheckBoxesServicios(QStringList list);
    QStringList getCheckBoxeServicios();
public slots:
    void setData(QJsonObject o);
signals:
    void finalizado_informe(int);
    void sendData(QJsonObject);
private slots:
    void on_pb_aceptar_clicked();

    void on_pb_cancelar_clicked();

private:
    Ui::Informe_Instalacion_Servicios *ui;
    QJsonObject jsonObject;
    void guardarDatos();
    QString empresa = "";
};

#endif // INFORME_INSTALACION_SERVICIOS_H
