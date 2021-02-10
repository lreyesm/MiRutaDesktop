#include "selectionorder.h"
#include "ui_selectionorder.h"
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

SelectionOrder::SelectionOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectionOrder)
{
    ui->setupUi(this);
    QStringList ordenes;
    ordenes << "DIARIAS" << "MASIVAS" << "ESPECIALES";
    ui->l_orden->addItems(ordenes);

    this->move(parent->width()/2-this->width()/2, parent->height()/2-this->height()/2);
}

SelectionOrder::~SelectionOrder()
{
    delete ui;
}


void SelectionOrder::on_pb_cancelar_clicked()
{
    this->close();
}

void SelectionOrder::on_pb_aceptar_clicked()
{   
    QString order = ui->l_orden->text();
    this->close();
    emit order_selected(order);
}
