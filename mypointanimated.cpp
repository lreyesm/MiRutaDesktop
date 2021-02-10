#include "mypointanimated.h"
#include <QTimer>

MyPointAnimated::MyPointAnimated(QWidget *parent)
    :QLabel(parent)
{
//    this->setMouseTracking(true);
//    connect(this, SIGNAL(objectNameChanged(const QString&)), this, SLOT(setFotoInit(const QString&)));
}
MyPointAnimated::~MyPointAnimated()
{
    //    delete animation;
}

void MyPointAnimated::setFotoInit(const QString& n)
{
    disconnect(this, SIGNAL(objectNameChanged(const QString&)), this, SLOT(setFotoInit(const QString&)));
    info = n;
    if(!info.isEmpty()){
        if(info.contains("_")){
            QStringList name = info.split("_");
            if(name.size() >= 2){
                name.removeAt(0);
                info = name.join("_").trimmed();
            }
        }
        if(QFile::exists(":/icons/" + info + "_off.png")){
            this->setPixmap(QPixmap(":/icons/" + info + "_off.png"));
        }
    }
}
void MyPointAnimated::setAnimationInit()
{

    startValue = this->geometry();
    endValue = QRect(startValue.x()-anim_scale,startValue.y() -anim_scale
                     ,startValue.width() +anim_scale*2,startValue.height() +anim_scale*2);
    maxSize= this->maximumSize();
    this->setMaximumSize(maxSize.width() +  anim_scale*2, maxSize.height() +anim_scale*2);
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
}
void MyPointAnimated::setAnimationEnd()
{
    //    disconnect(animation, &QPropertyAnimation::finished, this, &MyPointAnimated::setAnimationEnd);
    //    disconnect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    delete animation;
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(endValue);
    animation->setEndValue(startValue);
    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}
void MyPointAnimated::finalizadaAnimacion()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animationOnGoing = false;
    this->setMaximumSize(maxSize);
    delete animation;
    QTimer::singleShot(50, this, SLOT(emitirClicked()));
    //    emit mouseLeftClicked();
}
void MyPointAnimated::emitirClicked()
{
    emit clicked();
}

void MyPointAnimated::startAnimation(int delay, int scale, int anim_tim)
{
     Q_UNUSED(delay);
    //hacer un single shot con delay
    anim_time = anim_tim;
    anim_scale = scale;
    setAnimationInit();
    animationOnGoing = true;
    connect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    //    connect(animation, &QPropertyAnimation::finished, this, &MyPointAnimated::setAnimationEnd);

}
bool MyPointAnimated::isOngoingAnimation(){
    return animationOnGoing;
}
void MyPointAnimated::mousePressEvent(QMouseEvent *ev)
{
    //    if(QApplication::mouseButtons()==Qt::RightButton){
    //        emit mouseRightClicked();
    //    }
    //    if(QApplication::mouseButtons()==Qt::LeftButton){
    emit mouseLeftClicked();

    if(!animationOnGoing){
//        if(!isChecked()){
//            setChecked(true);
//        }
        startAnimation(0);
    }

    //    }
}
void MyPointAnimated::setChecked(bool checked){
    if(checked){
        activated_state = true;
        if(QFile::exists(":/icons/" + info + "_on.png")){
            this->setPixmap(QPixmap(":/icons/" + info + "_on.png"));
        }
    }else{
        activated_state = false;
        if(QFile::exists(":/icons/" + info + "_off.png")){
            this->setPixmap(QPixmap(":/icons/" + info + "_off.png"));
        }
    }
}
bool MyPointAnimated::isChecked(){
    return activated_state;
}
//void MyPointAnimated::mouseMoveEvent(QMouseEvent *ev)
//{
//    emit mouseEntered(ev->pos());
//}
