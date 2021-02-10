#ifndef CANTIDAD_PIEZAS_H
#define CANTIDAD_PIEZAS_H

#include <QDialog>

namespace Ui {
class Cantidad_Piezas;
}

class Cantidad_Piezas : public QDialog
{
    Q_OBJECT

public:
    explicit Cantidad_Piezas(QWidget *parent = nullptr, QString pieza = "...");
    ~Cantidad_Piezas();

signals:
    void add_pieza(QString);
private slots:
    void on_buttonBox_accepted();

private:
    Ui::Cantidad_Piezas *ui;
};

#endif // CANTIDAD_PIEZAS_H
