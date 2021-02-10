#ifndef SECCION_ESTADO_DE_LAS_VALVULAS_H
#define SECCION_ESTADO_DE_LAS_VALVULAS_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class Seccion_Estado_de_las_Valvulas;
}

class Seccion_Estado_de_las_Valvulas : public QWidget
{
    Q_OBJECT

public:
    explicit Seccion_Estado_de_las_Valvulas(QWidget *parent = nullptr, QJsonObject jsonObject = QJsonObject());
    ~Seccion_Estado_de_las_Valvulas();
    void populateView();

    static QJsonObject setExcelenteEnGeneral(QJsonObject itac);
    static QJsonObject setMalEstadoEnGeneral(QJsonObject itac);
signals:
    void itac_json(QJsonObject);
    void accept();
    void reject();

private slots:
    void on_pb_aceptar_clicked();
    void on_pb_cancelar_clicked();
    void on_rb_general_no_tiene_toggled(bool checked);

    void on_rb_entrada_no_tiene_toggled(bool checked);

    void on_rb_salida_no_tiene_toggled(bool checked);

    void on_rb_antiretorno_no_tiene_toggled(bool checked);

private:
    Ui::Seccion_Estado_de_las_Valvulas *ui;
    QJsonObject itac;
    void guardarDatos();
    bool checkIfFieldIsValid(QString var);
};

#endif // SECCION_ESTADO_DE_LAS_VALVULAS_H
