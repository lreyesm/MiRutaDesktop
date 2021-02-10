#include "resumen_estado_instalacion.h"
#include "ui_resumen_estado_instalacion.h"

Resumen_Estado_Instalacion::Resumen_Estado_Instalacion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Resumen_Estado_Instalacion)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
}

Resumen_Estado_Instalacion::~Resumen_Estado_Instalacion()
{
    delete ui;
}
void Resumen_Estado_Instalacion::on_pb_aceptar_clicked()
{
    emit accepted();
    if(ui->rb_excelente->isChecked()){
        emit stateSelected("EXCELENTE");

    }else if(ui->rb_mal_estado->isChecked()){
        emit stateSelected("MAL ESTADO");
    }
    this->close();
}

void Resumen_Estado_Instalacion::on_pb_cancelar_clicked()
{
    emit rejected();
    this->close();
}
void Resumen_Estado_Instalacion::on_pb_close_clicked()
{
    this->close();
    emit reject();
}
