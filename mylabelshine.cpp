#include "mylabelshine.h"
#include <QGraphicsDropShadowEffect>
#include "global_variables.h"

MyLabelShine::MyLabelShine(QWidget *parent) : QLabel(parent)
{
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    this->setGraphicsEffect(effect);
}
