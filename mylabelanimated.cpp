#include "mylabelanimated.h"
#include <QTimer>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

MyLabelAnimated::MyLabelAnimated(QWidget *parent)
    :QLabel(parent)
{
    //    this->setMouseTracking(true);
    //    connect(this, SIGNAL(objectNameChanged(const QString&)), this, SLOT(setFotoInit(const QString&)));
}
MyLabelAnimated::~MyLabelAnimated()
{
    //    delete animation;
}

void MyLabelAnimated::setBackgroundImage(const QString image_filename)
{
    if(QFile::exists(":/icons/" + image_filename + ".png")){
         this->setStyleSheet("border-image: url(:/icons/"+ image_filename +".png);");
    }
    else{
         qDebug()<<"No se ha creado icono de este label (image_filename.png)";
    }
}

void MyLabelAnimated::setClickedOption(int option)
{
    m_clickedOption = option;
}
void MyLabelAnimated::setShadowEffect()
{
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(2);
    this->setGraphicsEffect(effect);
}

void MyLabelAnimated::hideChilds(){
    //    menu_expanded->hide();
}
void MyLabelAnimated::setFotoInit(const QString& n)
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
void MyLabelAnimated::setAnimationInit()
{
    if(this->width() > 120){
        anim_scale = 5;
    }
    int anim_scale_width = this->width() * static_cast<float>(anim_scale)/100;
    int anim_scale_height = this->height() * static_cast<float>(anim_scale)/100;

    startValue = this->geometry();
    endValue = QRect(startValue.x()+anim_scale_width,startValue.y() +anim_scale_height
                     ,startValue.width() - anim_scale_width*2,startValue.height() - anim_scale_height*2);
    minSize= this->minimumSize();
    if(((minSize.width() -  anim_scale_width*2) > 0) && ((minSize.height() - anim_scale_height*2) > 0)){
        this->setMinimumSize(minSize.width() -  anim_scale_width*2, minSize.height() - anim_scale_height*2);
    }
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
}
void MyLabelAnimated::setAnimationEnd()
{
    //    disconnect(animation, &QPropertyAnimation::finished, this, &MyLabelAnimated::setAnimationEnd);
    //    disconnect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    delete animation;
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(endValue);
    animation->setEndValue(startValue);
    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}
void MyLabelAnimated::finalizadaAnimacion()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animationOnGoing = false;
    this->setMinimumSize(minSize);
    delete animation;
    QTimer::singleShot(50, this, SLOT(emitirClicked()));
    //    emit mouseLeftClicked();
}
void MyLabelAnimated::emitirClicked()
{
    emit clicked();
    emit clickedOption(m_clickedOption);
    emit mouseLeftClicked(this->objectName());
}

void MyLabelAnimated::startAnimation(int delay, int scale, int anim_tim)
{
    //hacer un single shot con delay
    //    setScaledContents(true);
    anim_time = anim_tim;
    anim_scale = scale;
    setAnimationInit();
    animationOnGoing = true;
    connect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    //    connect(animation, &QPropertyAnimation::finished, this, &MyLabelAnimated::setAnimationEnd);

}

void MyLabelAnimated::mousePressEvent(QMouseEvent *ev)
{
    //    if(QApplication::mouseButtons()==Qt::RightButton){
    //        emit mouseRightClicked();
    //    }
    //    if(QApplication::mouseButtons()==Qt::LeftButton){
    emit pressed();
    if(!animationOnGoing){
        //        if(!isChecked()){
        //            setChecked(true);
        //        }
        startAnimation(0);
    }

    //    }
}
void MyLabelAnimated::setChecked(bool checked){
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
bool MyLabelAnimated::isChecked(){
    return activated_state;
}
void MyLabelAnimated::mouseMoveEvent(QMouseEvent *ev)
{
    emit mouseEntered(ev->pos());
}
