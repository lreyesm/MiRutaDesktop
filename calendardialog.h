#ifndef CALENDARDIALOG_H
#define CALENDARDIALOG_H

#include <QDialog>
#include <QDate>
#include "datetimewidget.h"

namespace Ui {
class CalendarDialog;
}

class CalendarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalendarDialog(QWidget *parent = nullptr, bool show_horas = false, QString empresa = "", bool show_init_time = true);
    ~CalendarDialog();

signals:
    void date_selected(QDate);
    void time_selected_init(QTime);
    void time_selected_end(QTime);
    void operator_selected(QString);
    void equipo_selected(QString);
private slots:

    void on_te_hora_inicio_userTimeChanged(const QTime &time);
    void on_pb_asign_operators_clicked();
    void get_user_selected(const QString &user);
    void on_pb_aceptar_clicked();

    void on_pb_cancelar_clicked();

    void get_team_selected(const QString &team);
    void on_pb_asign_team_clicked();
private:
    Ui::CalendarDialog *ui;
    QString operario_selected, team_selected;
    DateTimeWidget *dt;
    QString empresa = "";
};

#endif // CALENDARDIALOG_H
