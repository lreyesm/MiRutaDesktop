#ifndef OBTENER_GEOLOCALIZACION_H
#define OBTENER_GEOLOCALIZACION_H

#include <QDialog>

namespace Ui {
class Obtener_Geolocalizacion;
}

class Obtener_Geolocalizacion : public QDialog
{
    Q_OBJECT

public:
    explicit Obtener_Geolocalizacion(QWidget *parent = nullptr);
    ~Obtener_Geolocalizacion();
signals:
    void coordenadas_obtenidas(QString);
private slots:
    void on_buttonBox_accepted();

    void on_le_enlace_con_coordenadas_textChanged(const QString &arg1);

    void on_pb_tutorial_clicked();

private:
    Ui::Obtener_Geolocalizacion *ui;
    QString get_coordinades_From_Link_GoogleMaps(const QString &link_google_maps);
    QString coords = "...";
};

#endif // OBTENER_GEOLOCALIZACION_H
