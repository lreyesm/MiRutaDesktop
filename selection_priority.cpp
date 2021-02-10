#include "selection_priority.h"
#include "ui_selection_priority.h"

Selection_Priority::Selection_Priority(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Selection_Priority)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    QStringList prioridades;
    prioridades << "BAJA" << "MEDIA" << "ALTA" << "HIBERNAR";
    ui->comboBox->addItems(prioridades);
}

Selection_Priority::~Selection_Priority()
{
    delete ui;
}

void Selection_Priority::on_pb_aceptar_clicked()
{
    this->close();
    emit accepted();
    emit priorityselected(ui->comboBox->currentText().trimmed());
}

void Selection_Priority::on_pb_cancelar_clicked()
{
    emit rejected();
    this->close();
}
void Selection_Priority::on_pb_close_clicked()
{
    this->close();
    emit reject();
}
