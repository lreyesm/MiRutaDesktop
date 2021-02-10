#ifndef FIELDS_TO_ASSIGN_H
#define FIELDS_TO_ASSIGN_H

#include <QDialog>
#include <QMap> //HashMap
#include <navegador.h>
#include <QJsonArray>
#include <QCompleter>

namespace Ui {
class Fields_to_Assign;
}

class Fields_to_Assign : public QDialog
{
    Q_OBJECT

public:
    explicit Fields_to_Assign(QWidget *parent = nullptr, QString empresa = "");
    ~Fields_to_Assign();
    void setJsonArrayContadores(QJsonArray);
    QString get_coordinades_From_Link_GoogleMaps(const QString &link_google_maps);

signals:
    void fields_selected(QMap<QString, QString>);
    void closing();

protected slots:
    void closeEvent(QCloseEvent *);

private slots:
    void on_buttonBox_accepted();
    void on_pb_web_browser_clicked();

    void on_pb_open_google_maps_clicked();

    void fill_counter_data(QString completion);

    void setGeoCode(const QString geocode);
    void setGeoCodeHand(const QString geocode);
    void eraseHomeMarker();
    void eraseHandMarker();

    void fill_anomaly_data(QString completion);
    void fill_itac_data(QString cod_emplazamiento);
    void on_pb_aceptar_clicked();

    void on_pb_cancelar_clicked();

    void on_pb_add_observacion_clicked();
    void setObservation(QString obs);
    void on_pb_erase_observacion_clicked();

private:
    Ui::Fields_to_Assign *ui;
    QMap<QString, QString> fields;
    Navegador *web_browser;
    QJsonArray jsonArrayContadores;
    void iniciateVariables();
    QMap<QString, QString> mapaTiposDeClase,mapaTiposDeMarca, mapaEstados;
    QCompleter *completer;
    QCompleter *completer_anomalias, *completer_intervenciones, *completer_itacs;
    QString observacion_seleccionada;
    QString empresa = "";
};

#endif // FIELDS_TO_ASSIGN_H
