#ifndef RESUMEN_ESTADO_INSTALACION_H
#define RESUMEN_ESTADO_INSTALACION_H

#include <QDialog>

namespace Ui {
class Resumen_Estado_Instalacion;
}

class Resumen_Estado_Instalacion : public QDialog
{
    Q_OBJECT

public:
    explicit Resumen_Estado_Instalacion(QWidget *parent = nullptr);
    ~Resumen_Estado_Instalacion();
signals:
    void stateSelected(QString);
private slots:
    void on_pb_aceptar_clicked();

    void on_pb_cancelar_clicked();

    void on_pb_close_clicked();
private:
    Ui::Resumen_Estado_Instalacion *ui;
};

#endif // RESUMEN_ESTADO_INSTALACION_H
