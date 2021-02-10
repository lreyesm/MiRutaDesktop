#ifndef QWIDGETDATE_H
#define QWIDGETDATE_H

#include <QPushButton>
#include <QLabel>
#include <QDateTime>
#include "mylabelanimated.h"

class QWidgetDate : public QWidget
{
    Q_OBJECT
public:
    explicit QWidgetDate(QWidget *parent = nullptr);

    QString text();
    void setText(QString text);
signals:
    void clicked();
    void send_text_date(QString);
    void doubleClicked();

protected slots:
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

public slots:

private slots:
    void get_date_selected(QDate date);

private:
    QLabel *label;
    MyLabelAnimated *clear_button;
};
#endif // QWIDGETDATE_H
