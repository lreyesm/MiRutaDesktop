#ifndef SECCION_LLAVES_DE_PUERTAS_H
#define SECCION_LLAVES_DE_PUERTAS_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class Seccion_LLaves_de_Puertas;
}

class Seccion_LLaves_de_Puertas : public QWidget
{
    Q_OBJECT

public:
    explicit Seccion_LLaves_de_Puertas(QWidget *parent = nullptr, QJsonObject jsonObject = QJsonObject());
    ~Seccion_LLaves_de_Puertas();

    void populateView();
signals:
    void accept();
    void reject();
    void itac_json(QJsonObject);

private slots:

    void on_rb_llave_especifica_toggled(bool checked);

    void on_rb_llaves_maestras_toggled(bool checked);

    void on_rb_llaves_de_exterior_toggled(bool checked);

    void on_pb_aceptar_clicked();
    void on_pb_cancelar_clicked();
    void on_rb_sin_comprobar_toggled(bool checked);

private:
    Ui::Seccion_LLaves_de_Puertas *ui;
    void hideAll();
    bool checkIfFieldIsValid(QString var);
    QJsonObject itac;
    void guardarDatos();
};

#endif // SECCION_LLAVES_DE_PUERTAS_H
