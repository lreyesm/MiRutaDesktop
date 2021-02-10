#include "mylabelmenuchildanimated.h"
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
//Importante --- Se debe crear icono con el nombre de este objecto (this.objectName)
MyLabelMenuChildAnimated::MyLabelMenuChildAnimated(QWidget *parent)
    :QLabel(parent)
{
    while (parent != nullptr/* && parent->objectName()!="Tabla"*/) {
        if(parent->parentWidget() == nullptr){
            break;
        }
        parent = parent->parentWidget();
    }
    baseWidget = parent;
    menu_expanded = new MyWidgetMenuAnimated(parent);
    //    menu_expanded->setPixmap(QPixmap(":/icons/selection_point.png"));
    //    menu_expanded->setFixedSize(14,15);
//    this->setScaledContents(false);
    menu_expanded->hide();

    connect(this, SIGNAL(objectNameChanged(const QString&)), this, SLOT(setChildOwner(const QString&)));

    //    connect(this, SIGNAL(objectNameChanged(const QString&)), this, SLOT(setFotoInit(const QString&)));
    //    this->setMouseTracking(true);
}

MyLabelMenuChildAnimated::~MyLabelMenuChildAnimated()
{
    //    delete animation;
}

void MyLabelMenuChildAnimated::setBackgroundImage(const QString image_filename)
{
    this->setStyleSheet("border-image: url(:/icons/"+ image_filename +".png);");
}

void MyLabelMenuChildAnimated::emitActionToTablaClass(QString action)
{
    emit actionPress(action);
}

void MyLabelMenuChildAnimated::setChildOwner(const QString& n)
{
    if(!QFile::exists(":/icons/menu_extra_"+this->objectName()+".png")){
        qDebug()<<"No se ha creado icono de este objeto menu (objectName.png)";
    }
    //    QString p = menu_expande d->parentWidget()->objectName();
    menu_expanded->setOwner(n);
    connect(menu_expanded,SIGNAL(actionPressed(QString)),this,SLOT(emitActionToTablaClass(QString)));
}
void MyLabelMenuChildAnimated::setFotoInit(const QString& n)
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
void MyLabelMenuChildAnimated::setAnimationInit()
{
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
void MyLabelMenuChildAnimated::setAnimationEnd()
{
    //    disconnect(animation, &QPropertyAnimation::finished, this, &MyLabelMenuChildAnimated::setAnimationEnd);
    //    disconnect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    delete animation;
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(endValue);
    animation->setEndValue(startValue);
    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}
void MyLabelMenuChildAnimated::finalizadaAnimacion()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animationOnGoing = false;
    this->setMinimumSize(minSize);
    delete animation;
    QTimer::singleShot(50, this, SLOT(emitirClicked()));
    //    emit mouseLeftClicked();
}
void MyLabelMenuChildAnimated::emitirClicked()
{
    emit clicked();
    emit endAnimation(objectName());

//    QPixmap pixmap;
//    if(QFile::exists(":/icons/menu_extra_"+this->objectName()+".png")){
//        pixmap = QPixmap(":/icons/menu_extra_"+this->objectName()+".png");
//        menu_expanded->setBackgroundImage("menu_extra_"+this->objectName());
//    }else {
//        qDebug()<<"No se ha creado icono de este objeto (objectName.png)";
//    }
    menu_expanded->show();
    menu_expanded->raise();

    int witdh_icon_menu_expand = menu_expanded->width();
    int height_icon_menu_expand = menu_expanded->height();
    int bounce = 50;
//    if(!pixmap.isNull()){
//        witdh_icon_menu_expand = pixmap.width();
//        height_icon_menu_expand = pixmap.height();
//    }
    QPoint pos_in_parent = this->pos();
    QPoint pos_global = parentWidget()->mapToGlobal(pos_in_parent);//El mapeo debe ser desde el padre de este widget
    //    (porque vas a pasarle una posicion que esta dentro de el)
    pos_global = baseWidget->mapFromGlobal(pos_global);//posicion desde el widget base (padre sin padre)

    int posicion_boton_x = pos_global.x() + this->width() + 20 /*Valor adicional al borde del boton*/;

    QString  name = baseWidget->objectName();
    name = parentWidget()->objectName();

    int middleScreen =(int)((float)QApplication::desktop()->screen(
                                QApplication::desktop()->screenNumber())->height()/2);
    int posicion_boton_y = pos_global.y();
    if(posicion_boton_y > middleScreen){ //Si el label esta por debajo de la mitad de la pantalla
        //desplegar menu desde una posicion superior y termine en el boton por la parte de abajo del menu
        posicion_boton_y = posicion_boton_y - height_icon_menu_expand + this->height();
    }
    menu_expanded->setAnimationParameters(0/*delay*/, witdh_icon_menu_expand, bounce, ANIM_TIME_W,
                                          QRect(posicion_boton_x,posicion_boton_y
                                                , 1, height_icon_menu_expand),//start
                                          QRect(posicion_boton_x, posicion_boton_y, //pos
                                                witdh_icon_menu_expand + bounce, height_icon_menu_expand));//end

    menu_expanded->setFocusPolicy(Qt::StrongFocus);
    menu_expanded->showWithAnimation();
    menu_expanded->setFocus();
}

void MyLabelMenuChildAnimated::hideChilds(){
    menu_expanded->hideChilds();
}
void MyLabelMenuChildAnimated::startAnimation(int delay, int scale, int anim_tim)
{
    Q_UNUSED(delay);
    //hacer un single shot con delay
    //    setScaledContents(true);
    anim_time = anim_tim;
    anim_scale = scale;
    setAnimationInit();
    animationOnGoing = true;
    connect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    //    connect(animation, &QPropertyAnimation::finished, this, &MyLabelMenuChildAnimated::setAnimationEnd);

}

void MyLabelMenuChildAnimated::generateAnimation(){
    emit mouseLeftClicked(this->objectName());
    if(!animationOnGoing){
        startAnimation(0);
    }
}
void MyLabelMenuChildAnimated::mousePressEvent(QMouseEvent *ev)
{
    //    if(QApplication::mouseButtons()==Qt::RightButton){
    //        emit mouseRightClicked();
    //    }
    //    if(QApplication::mouseButtons()==Qt::LeftButton){
    emit mouseLeftClicked(this->objectName());
    //    emit actionPress(objectName());

    if(!animationOnGoing){
        //        if(!isChecked()){
        //            setChecked(true);
        //        }
        startAnimation(0);
    }

    //    }
}
void MyLabelMenuChildAnimated::setChecked(bool checked){
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
bool MyLabelMenuChildAnimated::isChecked(){
    return activated_state;
}
//void MyLabelMenuChildAnimated::mouseMoveEvent(QMouseEvent *ev)
//{
//    emit mouseEntered(ev->pos());
//}
