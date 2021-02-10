#include "idordenassign.h"
#include "ui_idordenassign.h"
#include <QMessageBox>
#include "globalfunctions.h"

IDOrdenAssign::IDOrdenAssign(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IDOrdenAssign)
{
    ui->setupUi(this);
}

IDOrdenAssign::~IDOrdenAssign()
{
    delete ui;
}

void IDOrdenAssign::moveCenter(){
    QRect rect = parentWidget()->geometry();
    this->move(rect.width()/2 - this->width()/2,
               rect.height()/2 - this->height()/2 - 20);
}

void IDOrdenAssign::on_pb_close_clicked()
{
    emit rejected();
    this->close();
}

void IDOrdenAssign::on_pb_aceptar_clicked()
{
    QString seleccion = ui->le_id_orden_inicial->text();
    bool ok;
    seleccion.toInt(&ok);
    if(!ok){
        GlobalFunctions::showWarning(this, "Campo Inválido", "Solo puede insertar números como ID Orden");
        return;
    }
    this->close();

    emit id_selected(seleccion);
    emit accepted();

}

void IDOrdenAssign::on_pb_cancelar_clicked()
{
    on_pb_close_clicked();
}
