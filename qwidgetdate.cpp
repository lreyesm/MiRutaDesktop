#include "qwidgetdate.h"

#include "calendardialog.h"
#include "global_variables.h"
#include <QHBoxLayout>


QWidgetDate::QWidgetDate(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(145, 19);
    this->setStyleSheet(QStringLiteral("background-color: #EFEFEF;"
                                        "border-radius: 3px;"
                                        "color:rgb(50, 50, 50);"));
    label = new QLabel(this);
    label->setFixedSize(145, 19);
    label->setStyleSheet(QStringLiteral("background-color: #EFEFEF;"
                                        "border-radius: 3px;"
                                        "color:rgb(50, 50, 50);"));
    label->setAlignment(Qt::AlignLeft);
    label->setAlignment(Qt::AlignVCenter);
    QFont font("Segoe UI", 10);
    label->setFont(font);
    label->show();

    clear_button = new MyLabelAnimated(this);
    clear_button->setStyleSheet(QStringLiteral("background-color: #EFEFEF;"
                                               "border-radius: 3px;"));
    clear_button->setPixmap(QPixmap(":/icons/close.png"));
    clear_button->setScaledContents(true);
    clear_button->setFixedSize(16, 16);
    clear_button->show();
    clear_button->move(125,2);
    QObject::connect(clear_button, &MyLabelAnimated::clicked, label, &QLabel::clear);
}

void QWidgetDate::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    emit doubleClicked();
}

void QWidgetDate::get_date_selected(QDate date)
{
    QDateTime dt;
    dt.setDate(date);
    this->setText(dt.toString(formato_fecha_hora_new_view));
}

void QWidgetDate::setText(QString text) ///al reimplementar esta funcion deja de funcionar el evento pressed
{
    label->setText(text);
}

QString QWidgetDate::text() ///al reimplementar esta funcion deja de funcionar el evento pressed
{
    return label->text();
}


void QWidgetDate::mousePressEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
{
    Q_UNUSED(e);
    emit clicked();
    CalendarDialog *calendarDialog = new CalendarDialog();
    connect(calendarDialog, &CalendarDialog::date_selected ,this, &QWidgetDate::get_date_selected);

    if(calendarDialog->exec()){
        emit send_text_date(this->text());
    }
}
