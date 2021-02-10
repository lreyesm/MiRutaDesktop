#ifndef SECCION_ESTADO_DE_TUBERIAS_H
#define SECCION_ESTADO_DE_TUBERIAS_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class Seccion_Estado_de_Tuberias;
}

class Seccion_Estado_de_Tuberias : public QWidget
{
    Q_OBJECT

public:
    explicit Seccion_Estado_de_Tuberias(QWidget *parent = nullptr, QJsonObject jsonObject = QJsonObject());
    ~Seccion_Estado_de_Tuberias();
    static QJsonObject setExcelenteEnGeneral(QJsonObject itac);
    static QJsonObject setMalEstadoEnGeneral(QJsonObject itac);
signals:
    void itac_json(QJsonObject);
    void accept();
    void reject();

private slots:
    void on_pb_aceptar_clicked();
    void on_pb_cancelar_clicked();

private:
    Ui::Seccion_Estado_de_Tuberias *ui;

    QJsonObject itac;
    void guardarDatos();
    void populateView();
    bool checkIfFieldIsValid(QString var);

};

#endif // SECCION_ESTADO_DE_TUBERIAS_H
