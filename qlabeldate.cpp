#include "qlabeldate.h"
#include "calendardialog.h"
#include "global_variables.h"

QLabelDate::QLabelDate(QWidget *parent) : QLabel(parent)
{

}

void QLabelDate::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    emit doubleClicked();
}

void QLabelDate::get_date_selected(QDate date)
{
    QDateTime dt;
    dt.setDate(date);
    this->setText(dt.toString(formato_fecha_hora_new_view));
}

void QLabelDate::mousePressEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
{
    Q_UNUSED(e);
    emit clicked();
    CalendarDialog *calendarDialog = new CalendarDialog();
    connect(calendarDialog, &CalendarDialog::date_selected ,this, &QLabelDate::get_date_selected);

    if(calendarDialog->exec()){
        emit send_text_date(this->text());
    }
}
