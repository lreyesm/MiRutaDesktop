#ifndef QPERSONALIZEBUTTON_H
#define QPERSONALIZEBUTTON_H

#include <QPushButton>
#include <QModelIndex>

class QPersonalizeButton : public QPushButton
{

    Q_OBJECT
public:
    QPersonalizeButton(QWidget *);
    void setModelIndex(QModelIndex i);

signals:
    void doubleClickedLabel(QModelIndex);

protected:
    void mouseDoubleClickEvent(QMouseEvent *ev);

private:
    bool editable;
    QModelIndex index;


};

#endif // QPERSONALIZEBUTTON_H
