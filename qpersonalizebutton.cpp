#include "qpersonalizebutton.h"

QPersonalizeButton::QPersonalizeButton(QWidget *parent)
    :QPushButton (parent)
{

}

void QPersonalizeButton::setModelIndex(QModelIndex i){
    index = i;
}

void QPersonalizeButton::mouseDoubleClickEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
   emit doubleClickedLabel(index);
}
