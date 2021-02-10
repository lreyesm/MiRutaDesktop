#ifndef FIELDS_TO_ASSING_ITACS_H
#define FIELDS_TO_ASSING_ITACS_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class Fields_to_Assing_ITACs;
}

class Fields_to_Assing_ITACs : public QWidget
{
    Q_OBJECT

public:
    explicit Fields_to_Assing_ITACs(QWidget *parent = nullptr);
    ~Fields_to_Assing_ITACs();

signals:
    void fields_selected(QJsonObject);

private slots:
    void on_pb_actualizar_clicked();

    void getJsonModified(QJsonObject jsonObject);
    void on_pb_valvulas_clicked();

    void on_pb_tuberias_clicked();

    void on_pb_estado_clicked();

    void on_pb_llaves_puertas_clicked();

    void on_pb_acceso_ubicacion_clicked();

    void setGeoCode(const QString geocode);
    void eraseMarker();
    void on_pb_geolocalizacion_clicked();

private:
    Ui::Fields_to_Assing_ITACs *ui;
    void initializeMaps();
    QJsonObject itac;
    bool geo_modified = false;
};

#endif // FIELDS_TO_ASSING_ITACS_H
