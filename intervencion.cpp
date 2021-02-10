#include "intervencion.h"
#include "ui_intervencion.h"

Intervencion::Intervencion(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Intervencion)
{
    ui->setupUi(this);
}

Intervencion::~Intervencion()
{
    delete ui;
}
