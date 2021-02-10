#include "mywidgetshine.h"
#include <QGraphicsDropShadowEffect>
#include "global_variables.h"

MyWidgetShine::MyWidgetShine(QWidget *parent) : QWidget(parent)
{
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    this->setGraphicsEffect(effect);
}
