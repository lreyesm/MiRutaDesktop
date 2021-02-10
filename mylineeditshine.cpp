#include "mylineeditshine.h"
#include <QGraphicsDropShadowEffect>
#include "global_variables.h"

MyLineEditShine::MyLineEditShine(QWidget *parent) : QLineEdit(parent)
{
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    this->setGraphicsEffect(effect);
}
