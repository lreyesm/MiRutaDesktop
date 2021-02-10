#include "cantidad_piezas.h"
#include "ui_cantidad_piezas.h"

Cantidad_Piezas::Cantidad_Piezas(QWidget *parent, QString pieza) :
    QDialog(parent),
    ui(new Ui::Cantidad_Piezas)
{
    ui->setupUi(this);
    ui->l_pieza->setText(pieza);
}

Cantidad_Piezas::~Cantidad_Piezas()
{
    delete ui;
}

void Cantidad_Piezas::on_buttonBox_accepted()
{
    QString cant = QString::number(ui->sp_cantidad->value());
    QString pieza = cant + " - " + ui->l_pieza->text();
    emit add_pieza(pieza);
    this->close();
}
