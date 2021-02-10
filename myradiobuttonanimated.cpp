#include "myradiobuttonanimated.h"
#include <QTimer>

MyRadioButtonAnimated::MyRadioButtonAnimated(QWidget *parent)
    :QLabel(parent)
{
    this->setMouseTracking(true);
    radioPicture = new MyPointAnimated(parent);
    radioPicture->setPixmap(QPixmap(":/icons/selection_point.png"));
    radioPicture->setScaledContents(true);
    radioPicture->setFixedSize(14,15);

    radioPicture->hide();
    connect(this, SIGNAL(objectNameChanged(const QString&)), this, SLOT(setFotoInit(const QString&)));
    connect(radioPicture, SIGNAL(mouseLeftClicked()), this, SLOT(startAnimationFromChild()));
}

MyRadioButtonAnimated::~MyRadioButtonAnimated()
{
    //    delete animation;
}

void MyRadioButtonAnimated::setFotoInit(const QString& n)
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
void MyRadioButtonAnimated::setAnimationInit()
{

    startValue = this->geometry();
    endValue = QRect(startValue.x()/*-anim_scale*/,startValue.y() -anim_scale
                     ,startValue.width()/* +anim_scale*2*/,startValue.height() /*+anim_scale*2*/);
    maxSize= this->maximumSize();
    this->setMaximumSize(maxSize.width() +  anim_scale*2, maxSize.height() +anim_scale*2);
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
}
void MyRadioButtonAnimated::setAnimationEnd()
{
    //    disconnect(animation, &QPropertyAnimation::finished, this, &MyRadioButtonAnimated::setAnimationEnd);
    //    disconnect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(endValue);
    animation->setEndValue(startValue);
    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}
void MyRadioButtonAnimated::finalizadaAnimacion()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    this->setMaximumSize(maxSize);
    delete animation;
    animationOnGoing = false;
    QTimer::singleShot(50, this, SLOT(emitirClicked()));
    //    emit clicked();
    //    emit mouseLeftClicked();
}

void MyRadioButtonAnimated::startAnimationFromChild()
{
    if(!animationOnGoing){
        if(!isChecked()){
            setChecked(true);
        }
        startAnimation(0);
        //        radioPicture->startAnimation(); //Comentado porque fue el emisor de animacion
    }
}

void MyRadioButtonAnimated::emitirClicked()
{
    emit clicked();
}

void MyRadioButtonAnimated::startAnimation(int delay, int scale, int anim_tim)
{
    //hacer un single shot con delay
    anim_time = anim_tim;
    anim_scale = scale;
    setAnimationInit();
    animationOnGoing = true;
    connect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    //    connect(animation, &QPropertyAnimation::finished, this, &MyRadioButtonAnimated::setAnimationEnd);

}

void MyRadioButtonAnimated::mousePressEvent(QMouseEvent *ev)
{
    //    if(QApplication::mouseButtons()==Qt::RightButton){
    //        emit mouseRightClicked();
    //    }
    //    if(QApplication::mouseButtons()==Qt::LeftButton){
    if(!animationOnGoing){
        if(!isChecked()){
            setChecked(true);
        }
        startAnimation(0);
//        radioPicture->setGeometry(child_startValue);
        radioPicture->startAnimation(0, 8);
    }

    //    }
}

void MyRadioButtonAnimated::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    QString ob = this->objectName();
    movePointPosition(event->pos());

}

void MyRadioButtonAnimated::movePointPosition(QPoint pos){
    QString ob = this->objectName();
    int pos_x = pos.x(), pos_y = pos.y(),
            width_m = (int)((float)this->size().width() / 2),
            height_m = this->size().height();
    radioPicture->setGeometry(pos_x + width_m - 9, pos_y + height_m - 15,
                              radioPicture->width(), radioPicture->height());
}
void MyRadioButtonAnimated::setChecked(bool checked){
    if(checked){
        activated_state = true;
        movePointPosition(this->pos());
        radioPicture->show();
        if(QFile::exists(":/icons/" + info + "_on.png")){
            this->setPixmap(QPixmap(":/icons/" + info + "_on.png"));
        }
        if(!radioPicture->isOngoingAnimation())
        radioPicture->startAnimation(0, 8);
    }else{
        activated_state = false;
        radioPicture->hide();
        if(QFile::exists(":/icons/" + info + "_off.png")){
            this->setPixmap(QPixmap(":/icons/" + info + "_off.png"));
        }
    }
}
bool MyRadioButtonAnimated::isChecked(){
    return activated_state;
}

