#ifndef DATESSCHEDULE_H
#define DATESSCHEDULE_H

#include <QWidget>
#include <QJsonArray>
#include <QStandardItemModel>
#include <QDateTime>

namespace Ui {
class DatesSchedule;
}

class DatesSchedule : public QWidget
{
    Q_OBJECT

public:
    explicit DatesSchedule(QWidget *parent = nullptr, QString empresa = "");
    ~DatesSchedule();

signals:
    void setLoadingTextSignal(QString);
    void hidingLoading();
private slots:
    void setTableView();
    void on_pb_update_clicked();

    void show_loading(QString mess);
    void setLoadingText(QString mess);
    void hide_loading();
    void selectDate();
    void get_date_selected(QDate d);
    void on_pb_select_day_clicked();

private:
    Ui::DatesSchedule *ui;
    void fixModelForTable(QJsonArray jsonArray);
    QStandardItemModel* model  = nullptr;
    QString empresa;
    QJsonArray requestData();
    QDate dateSelected = QDate::currentDate();
};

#endif // DATESSCHEDULE_H
