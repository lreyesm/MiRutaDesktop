#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent = nullptr);

public slots:
    void setWidth(int);
    void setBackgroundImage(QString);
signals:
    void textClicked(QString);
    void objectClicked(QString);
    void clicked();
public slots:

protected slots:
    void mousePressEvent(QMouseEvent *e);
};

#endif // CLICKABLELABEL_H
