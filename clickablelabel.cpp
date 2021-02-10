#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent)
{

}

void ClickableLabel::setWidth(int w)
{
    this->setFixedWidth(w);
}

void ClickableLabel::setBackgroundImage(QString pix)
{
    this->setPixmap(QPixmap(pix));
}

void ClickableLabel::mousePressEvent(QMouseEvent *e)
{
    emit textClicked(this->text());
    emit objectClicked(this->objectName());
    emit clicked();
}
