#include "rightclickedsection.h"
#include "ui_rightclickedsection.h"
#include <QScreen>

RightClickedSection::RightClickedSection(QWidget *parent, QPoint pos) :
    QWidget(parent),
    ui(new Ui::RightClickedSection)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    QRect rect = QGuiApplication::screens().first()->geometry();
    if((this->width() + pos.x()) >= rect.width()){
        pos.setX(pos.x() - this->width());
    }
    if((this->height() + pos.y()) >= rect.height()){
        pos.setY(pos.y() - this->height());
    }
    this->move(pos);
}

RightClickedSection::~RightClickedSection()
{
    delete ui;
}

void RightClickedSection::on_pb_filtrar_clicked()
{
    this->close();
    emit clickPressed(FILTRAR);
}

void RightClickedSection::on_pb_sort_ascending_clicked()
{
    this->close();
    emit clickPressed(ORDENAR_ASCENDENTE);
}

void RightClickedSection::on_pb_sort_descending_clicked()
{
    this->close();
    emit clickPressed(ORDENAR_DESCENDENTE);
}
