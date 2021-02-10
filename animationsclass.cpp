#include "animationsclass.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QWidget>
#include <QObject>

AnimationsClass::AnimationsClass()
{

}

void AnimationsClass::fadeInAnimation(QWidget *w)
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(w);
    w->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(1000);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void AnimationsClass::fadeOutAnimation(QWidget *w)
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(w);
    w->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(1000);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::OutBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    QObject::connect(a,SIGNAL(finished()),w->parentWidget(),SLOT(hide()));
}
