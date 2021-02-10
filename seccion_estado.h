#ifndef SECCION_ESTADO_H
#define SECCION_ESTADO_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class Seccion_Estado;
}

class Seccion_Estado : public QWidget
{
    Q_OBJECT

public:
    explicit Seccion_Estado(QWidget *parent = nullptr, QJsonObject jsonObject = QJsonObject());
    ~Seccion_Estado();
    void populateView();
    static QJsonObject setMalEstadoEnGeneral(QJsonObject itac);
    static QJsonObject setExcelenteEnGeneral(QJsonObject itac);

signals:
    void accept();
    void reject();
    void itac_json(QJsonObject);

private slots:
    void on_rb_desague_si_toggled(bool checked);

    void on_pb_aceptar_clicked();
    void on_pb_cancelar_clicked();

    void on_rb_Poca_toggled(bool checked);

    void on_rb_Nula_toggled(bool checked);

    void on_rb_Natural_toggled(bool checked);

    void on_rb_Suficiente_toggled(bool checked);


    void on_rb_sin_comprobar_toggled(bool checked);

private:
    Ui::Seccion_Estado *ui;
    void hideAll();
    bool checkIfFieldIsValid(QString var);
    QJsonObject itac;
    void guardarDatos();
};

#endif // SECCION_ESTADO_H
