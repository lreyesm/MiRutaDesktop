#include "mywidgetanimated.h"
#include <QDebug>
#include <QTimer>

MyWidgetAnimated::MyWidgetAnimated(QWidget *parent)
    :QOpenGLWidget(parent)
{
    //    this->move(0,0);
    //    this->setFixedSize(0, 0);///poner alto del padre (Toda la pantalla) y ancho 0
    //    setScaledContents(true);
    //    this->setMouseTracking(true);
    //    QTimer::singleShot(500, this, SLOT(setSizeInit()));
}
MyWidgetAnimated::~MyWidgetAnimated()
{
    //    delete animation;
}
void MyWidgetAnimated::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
}
void MyWidgetAnimated::focusInEvent(QFocusEvent *event)
{
    QWidget::focusInEvent(event);
}
void MyWidgetAnimated::setBackgroundImage(const QString image_filename)
{
    this->setStyleSheet("border-image: url(:/icons/"+ image_filename +".png);");
}

void MyWidgetAnimated::setAnimationParameters(int delay, int scale, int bounce, int time, QRect start, QRect end)
{
    Q_UNUSED(delay);
    anim_scale = scale;
    bounce_scale = bounce;
    anim_time = time;
    startValue = start;
    endValue = end;
    parametersSeted = true;
}


void MyWidgetAnimated::setSizeInit()
{
    this->setGeometry(QRect(0,0, 10, 0));
}

void MyWidgetAnimated::showWithAnimation()
{
    if(!animationOnGoing){
        if(this->isHidden()){
            this->setFixedSize(0,0);
            this->show();
        }
        this->raise();
        setChecked(true);
        if(parametersSeted){
            startAnimation(0, anim_scale, anim_time, startValue, endValue);
        }else {
            startAnimation(0);
        }
    }
}

void MyWidgetAnimated::startAnimation(int delay, int scale, int anim_tim, QRect start, QRect end)
{
    Q_UNUSED(delay);
    //hacer un single shot con delay
    anim_time = anim_tim;
    anim_scale = scale;
    setAnimationInit(start, end);
    animationOnGoing = true;
    connect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    //    connect(animation, &QPropertyAnimation::finished, this, &MyWidgetAnimated::setAnimationEnd);
}

void MyWidgetAnimated::setAnimationInit(QRect start, QRect end)
{
    QString parentName = parentWidget()->objectName();
    if(start.isEmpty()){
        startValue = QRect(0,0, 0, parentWidget()->height()+10);
    }else {
        startValue = start;
    }
    if(end.isEmpty()){
        endValue= QRect(startValue.x(),startValue.y() //posicion sinmodificar
                        ,startValue.width() + anim_scale + bounce_scale /*Ancho de icono de menu +50 (efecto rebote)*/
                        ,startValue.height()+ 0 /*El alto lo dejo como esta*/);
    }else {
        endValue = end;
    }
    midValue = QRect(startValue.x(),startValue.y() //posicion sinmodificar
                     ,startValue.width() + anim_scale /*Ancho de icono de menu*/
                     ,startValue.height()+ 0 /*El alto lo dejo como esta*/);

    this->setMaximumSize(startValue.width() + anim_scale + bounce_scale, startValue.height());
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
}
void MyWidgetAnimated::setAnimationEnd()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    delete animation;
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration((int)((float)anim_time/2));
    animation->setStartValue(endValue);
    animation->setEndValue(midValue);
    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}
void MyWidgetAnimated::finalizadaAnimacion()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->stop();
    animationOnGoing = false;
    delete animation;
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(16777215);
    animation->setStartValue(midValue);
    animation->setEndValue(midValue);
//    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
//    this->setFixedSize(midValue.size());
}

void MyWidgetAnimated::hideMenu(){
    if(animation != nullptr){
        animation->stop();
        delete animation;
        animation = nullptr;
        animationOnGoing = false;
    }
    this->hide();
//    for (int i=0; i < children().size();i++) { //Esto no funciono
//      //  if(children().at(i)->isWidgetType()){
//            connect(this, SIGNAL(hideAll()), (MyLabelAnimated*)children().at(i), SLOT(hideChilds()));
//       // }
//    }
//    emit hideAll();
}











void MyWidgetAnimated::finalizadaAnimacionEnd()
{    
    disconnect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacionEnd()));
    animation->stop();
    animationOnGoing = false;
    delete animation;
    //Aqui puedo emitir que se mostro el menu, (con una demora singleshot por supuesto)
}







void MyWidgetAnimated::resizeWidget(){

    qDebug()<< parent()->objectName() <<" Size After:  "<< QString::number(parentWidget()->size().width()) << ", " << QString::number(parentWidget()->size().height());
    float width_ratio = (float)parentWidget()->size().width()/1920;
    float height_ratio = (float)parentWidget()->size().height()/1080;
    int width_icon = 72;
    int height_icon = 1078;
    int width_resize = width_icon * width_ratio;
    int height_resize = height_icon * height_ratio;
    this->setFixedSize(width_resize, height_resize);
    //   this->setFixedSize(72, 1000);
    this->move(0,0);
}


void MyWidgetAnimated::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    emit mouseLeftClicked();
    if(!animationOnGoing){

    }
}

void MyWidgetAnimated::setOnIcon()
{
    if(!isChecked()){
        setChecked(true);
    }else {
        setChecked(false);
    }
}

void MyWidgetAnimated::setChecked(bool checked){
    if(checked){
        activated_state = true;
    }else{
        activated_state = false;
    }
}
bool MyWidgetAnimated::isChecked(){
    return activated_state;
}
//void MyWidgetAnimated::mouseMoveEvent(QMouseEvent *ev)
//{
//    emit mouseEntered(ev->pos());
//}
