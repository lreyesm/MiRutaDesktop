#include "databaseoptions.h"
#include "ui_databaseoptions.h"
#include "globalfunctions.h"

DatabaseOptions::DatabaseOptions(QWidget *parent, int cantActualConfigurada) :
    QWidget(parent),
    ui(new Ui::DatabaseOptions)
{
    ui->setupUi(this);
    ui->le_tareas_por_pagina->setText(QString::number(cantActualConfigurada));
    this->move(parent->width()/2-this->width()/2, parent->height()/2-this->height()/2);
    ui->l_warning->hide();
}

DatabaseOptions::~DatabaseOptions()
{
    delete ui;
}

void DatabaseOptions::on_pb_close_clicked()
{
    this->close();
}

void DatabaseOptions::on_pb_aceptar_clicked()
{
    bool ok;
    int cant = ui->le_tareas_por_pagina->text().toInt(&ok);
    if(ok || (cant > 0)){
        emit sendTareasPorPagina(cant);
    }
    else{
        GlobalFunctions::showWarning(this->parentWidget(), "Error", "No se pudo cambiar configuración");
    }
    this->close();
}

void DatabaseOptions::on_pb_cancelar_clicked()
{
    this->close();
}

void DatabaseOptions::on_le_tareas_por_pagina_textChanged(const QString &arg1)
{
    bool ok;
    int cant = arg1.toInt(&ok);
    if(ok){
        if(cant > 3000){
            ui->l_warning->show();
        }
        else{
            ui->l_warning->hide();
        }
    }
}
