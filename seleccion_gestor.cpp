#include "seleccion_gestor.h"
#include "ui_seleccion_gestor.h"
#include <QScreen>
#include "globalfunctions.h"

Seleccion_Gestor::Seleccion_Gestor(QWidget *parent, QStringList list, bool todos) :
    QDialog(parent),
    ui(new Ui::Seleccion_Gestor)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowTitle("Gestores");
    if(todos){
        list.prepend("Todos");
    }
    ui->comboBox->addItems(list);
}

Seleccion_Gestor::~Seleccion_Gestor()
{
    delete ui;
}
void Seleccion_Gestor::moveCenter(){
    QRect rect = QGuiApplication::screens().first()->geometry();
    this->move(rect.width()/2 - this->width()/2,
               rect.height()/2 - this->height()/2 - 20);
}
void Seleccion_Gestor::setGestors(QStringList list)
{
    ui->comboBox->addItems(list);
}

void Seleccion_Gestor::on_pb_close_clicked()
{
    this->close();
    emit reject();
}

void Seleccion_Gestor::on_pb_aceptar_clicked()
{
    QString seleccion = ui->comboBox->currentText();
    GlobalFunctions::writeGestorSelected(seleccion);
    emit selected_gestor(seleccion);
    emit accept();
}

void Seleccion_Gestor::on_pb_cancelar_clicked()
{
    on_pb_close_clicked();
}
