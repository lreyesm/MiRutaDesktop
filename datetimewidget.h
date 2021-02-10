#ifndef DATETIMEWIDGET_H
#define DATETIMEWIDGET_H

#include <QWidget>
#include <QMap>
#include <QDateTime>
#include "qlabel_button.h"

#define X_POS_LABEL_DAY_INIT 23
#define Y_POS_LABEL_DAY_INIT 0
#define X_POS_LABEL_DAY_INC 31
#define Y_POS_LABEL_DAY_INC 30

namespace Ui {
class DateTimeWidget;
}

class DateTimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeWidget(QWidget *parent = nullptr);
    ~DateTimeWidget();

    void setCurrentDateTime();
    QDateTime getFechaHora();
public slots:
    void showCenter();

    void setDateTime(QDateTime dt);
signals:
    void fechaHora(QDateTime);
    void settedFecha();

private slots:
    void on_l_up_hour_clicked();
    void on_l_down_hour_clicked();
    void on_l_up_minutes_clicked();
    void on_l_down_minutes_clicked();
    void on_l_up_am_pm_clicked();
    void on_l_down_am_pm_clicked();
    //    void on_l_up_day_clicked();
    //    void on_l_down_day_clicked();
    void on_l_up_month_clicked();
    void on_l_down_month_clicked();
    void on_l_up_year_clicked();
    void on_l_down_year_clicked();
    void onClickedDay(QString day_text);
    void on_l_am_pm_clicked();
    void on_pb_ok_clicked();

private:
    Ui::DateTimeWidget *ui;
    bool am_pm = false; //false am - true pm
    QDateTime fecha_hora = QDateTime::currentDateTime();
    QMap<QString, int> mapMonth;
    void fillJsonDate();
    void fillDaysLabel(int month);
    int selected_day = 1;
    void clearWidgets(QLayout *layout);
    void deleteAllChilds();
    QString dateTimeProcessInPC(QDateTime dt);
    QString dateTimeProcessInRaspi(QDateTime dt);
    void unmarkAllOtherDays();
    void setSelectedDay(int day);
};

#endif // DATETIMEWIDGET_H
