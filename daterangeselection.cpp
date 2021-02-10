#include "daterangeselection.h"
#include "ui_daterangeselection.h"
#include "calendardialog.h"
#include "global_variables.h"
#include <QGraphicsDropShadowEffect>

DateRangeSelection::DateRangeSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DateRangeSelection)
{
    ui->setupUi(this);

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    ui->pb_fecha_inicial->setGraphicsEffect(effect);
    QGraphicsDropShadowEffect* eff = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    eff->setBlurRadius(20);
    eff->setOffset(1);
    eff->setColor(color_blue_app);
    ui->pb_fecha_final->setGraphicsEffect(eff);

    this->move(parent->width()/2-this->width()/2, parent->height()/2-this->height()/2);
}

DateRangeSelection::~DateRangeSelection()
{
    delete ui;
}

void DateRangeSelection::on_pb_aceptar_clicked()
{
    if(!fecha_final.isValid() || !fecha_inicio.isValid()){
        return;
    }
    QStringList dates;
    QDate date = fecha_inicio;
    while(date <= fecha_final){
        dates << date.toString(formato_fecha);
        date = date.addDays(1);
    }
    emit dates_range(dates);
    this->close();
}
void DateRangeSelection::on_pb_close_clicked()
{
    this->close();
}

void DateRangeSelection::on_pb_cancelar_clicked()
{
    this->close();
}
void DateRangeSelection::on_pb_fecha_inicial_clicked()
{
    CalendarDialog *calendarDialog = new CalendarDialog(nullptr, false);
    connect(calendarDialog,SIGNAL(date_selected(QDate)),this,SLOT(get_date_init_selected(QDate)));

    calendarDialog->show();
}
void DateRangeSelection::on_pb_fecha_final_clicked()
{
    CalendarDialog *calendarDialog = new CalendarDialog(nullptr, false);
    connect(calendarDialog,SIGNAL(date_selected(QDate)),this,SLOT(get_date_end_selected(QDate)));

    calendarDialog->show();

}

void DateRangeSelection::get_date_init_selected(QDate d)
{
    fecha_inicio = d;
    ui->pb_fecha_inicial->setText(d.toString(formato_fecha_hora_new_view_sin_hora));
    if(!fecha_final.isValid()){
        fecha_final = d;
        ui->pb_fecha_final->setText(d.toString(formato_fecha_hora_new_view_sin_hora));
    }
}

void DateRangeSelection::get_date_end_selected(QDate d)
{
    fecha_final = d;
    ui->pb_fecha_final->setText(d.toString(formato_fecha_hora_new_view_sin_hora));
    if(!fecha_inicio.isValid()){
        fecha_inicio = d;
        ui->pb_fecha_inicial->setText(d.toString(formato_fecha_hora_new_view_sin_hora));
    }
}
