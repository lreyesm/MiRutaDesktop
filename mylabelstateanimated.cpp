#include "mylabelstateanimated.h"
#include <QTimer>


MyLabelStateAnimated::MyLabelStateAnimated(QWidget *parent)
    :QLabel(parent)
{
    this->setMouseTracking(true);

    QPoint p = this->pos();
    connect(this, SIGNAL(objectNameChanged(const QString&)), this, SLOT(setFotoInit(const QString&)));
}

MyLabelStateAnimated::~MyLabelStateAnimated()
{
    //    delete animation;
}

void MyLabelStateAnimated::setFotoInit(const QString& n)
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
void MyLabelStateAnimated::setAnimationInit()
{

    startValue = this->geometry();
    endValue = QRect(startValue.x()-anim_scale,startValue.y() -anim_scale
                     ,startValue.width() +anim_scale*2,startValue.height() +anim_scale*2);
    maxSize= this->maximumSize();
    if(((maxSize.width() +  anim_scale*2) < 10000) && ((maxSize.height() +anim_scale*2) < 10000)){
        this->setMaximumSize(maxSize.width() +  anim_scale*2, maxSize.height() +anim_scale*2);
    }
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
}
void MyLabelStateAnimated::setAnimationEnd()
{
    //    disconnect(animation, &QPropertyAnimation::finished, this, &MyLabelStateAnimated::setAnimationEnd);
    //    disconnect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    delete animation;
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(endValue);
    animation->setEndValue(startValue);
    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}
void MyLabelStateAnimated::finalizadaAnimacion()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    this->setMaximumSize(maxSize);
    delete animation;
    animationOnGoing = false;
    QTimer::singleShot(50, this, SLOT(emitirClicked()));
}

void MyLabelStateAnimated::startAnimationFromChild()
{
    if(!animationOnGoing){
        if(!isChecked()){
            setChecked(true);
        }
        startAnimation(true);
    }
}

void MyLabelStateAnimated::emitirClicked()
{
    if(emit_enabled){
        emit clicked();
        emit send_text(this->text());
    }
}

void MyLabelStateAnimated::startAnimation(bool emit_clicked, int scale, int anim_tim)
{
    //hacer un single shot con delay
    animationOnGoing = true;
    emit_enabled = emit_clicked;
    anim_time = anim_tim;
    anim_scale = scale;
    setAnimationInit();  
    connect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    //    connect(animation, &QPropertyAnimation::finished, this, &MyLabelStateAnimated::setAnimationEnd);

}

void MyLabelStateAnimated::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    label_size = this->size();
    if(!this->is_Enabled()){
        return;
    }
    if(!animationOnGoing){
        if(!isChecked()){
            setChecked(true);
        }else {
            setChecked(false);
        }
        if(myanimationEnabled){
            startAnimation(true);
        }else{
            emitirClicked();
        }
    }
}

void MyLabelStateAnimated::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
}
void MyLabelStateAnimated::setChecked(bool checked){
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
bool MyLabelStateAnimated::isChecked(){
    return activated_state;
}
