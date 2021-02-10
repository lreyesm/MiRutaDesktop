#ifndef QLABELDATE_H
#define QLABELDATE_H

#include <QLabel>
#include <QDateTime>

class QLabelDate : public QLabel
{
    Q_OBJECT
public:
    explicit QLabelDate(QWidget *parent = nullptr);

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
};

#endif // QLABELDATE_H
