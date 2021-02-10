#ifndef DATERANGESELECTION_H
#define DATERANGESELECTION_H

#include <QWidget>
#include <QDate>

namespace Ui {
class DateRangeSelection;
}

class DateRangeSelection : public QWidget
{
    Q_OBJECT

public:
    explicit DateRangeSelection(QWidget *parent = nullptr);
    ~DateRangeSelection();

signals:
    void dates_range(QStringList);

private slots:
    void on_pb_fecha_final_clicked();
    void on_pb_fecha_inicial_clicked();
    void get_date_init_selected(QDate);
    void get_date_end_selected(QDate);

    void on_pb_cancelar_clicked();
    void on_pb_aceptar_clicked();
    void on_pb_close_clicked();
private:
    Ui::DateRangeSelection *ui;
    QDate fecha_inicio;
    QDate fecha_final;
};

#endif // DATERANGESELECTION_H
