#ifndef ANIMATIONSCLASS_H
#define ANIMATIONSCLASS_H

#include <QtCore>

class AnimationsClass
{
public:
    AnimationsClass();

    static void fadeInAnimation(QWidget *);
    static void fadeOutAnimation(QWidget *);
};

#endif // ANIMATIONSCLASS_H
