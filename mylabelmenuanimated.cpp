#include "mylabelmenuanimated.h"
#include <QTimer>
#include <QDebug>

MyLabelMenuAnimated::MyLabelMenuAnimated(QWidget *parent)
    :QLabel(parent)
{
    while (parent != nullptr/* && parent->objectName()!="Tabla"*/) {
        if(parent->parentWidget() == nullptr){
            break;
        }
        parent = parent->parentWidget();
    }
    menu_expanded = new MyWidgetMenuAnimated(parent);
//    menu_expanded->setPixmap(QPixmap(":/icons/selection_point.png"));
//    menu_expanded->setScaledContents(true);
//    menu_expanded->setFixedSize(14,15);

    menu_expanded->hide();

    connect(this, SIGNAL(objectNameChanged(const QString&)), this, SLOT(setChildOwner(const QString&)));

//    connect(this, SIGNAL(objectNameChanged(const QString&)), this, SLOT(setFotoInit(const QString&)));
//    this->setMouseTracking(true);
}

MyLabelMenuAnimated::~MyLabelMenuAnimated()
{
    //    delete animation;
}

void MyLabelMenuAnimated::setBackgroundImage(const QString image_filename)
{
    this->setStyleSheet("border-image: url(:/icons/"+ image_filename +".png);");
}

void MyLabelMenuAnimated::emitActionToTablaClass(QString action)
{
    emit actionPress(action);
}

void MyLabelMenuAnimated::setChildOwner(const QString& n)
{
    if(!QFile::exists(":/icons/menu_extra_"+this->objectName()+".png")){
        qDebug()<<"No se ha creado icono de este objeto menu (objectName.png)";
    }
//    QString p = menu_expanded->parentWidget()->objectName();
    menu_expanded->setOwner(n);
    connect(menu_expanded,SIGNAL(actionPressed(QString)),this,SLOT(emitActionToTablaClass(QString)));
}
void MyLabelMenuAnimated::setFotoInit(const QString& n)
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
void MyLabelMenuAnimated::setAnimationInit()
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
void MyLabelMenuAnimated::setAnimationEnd()
{
    //    disconnect(animation, &QPropertyAnimation::finished, this, &MyLabelMenuAnimated::setAnimationEnd);
    //    disconnect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    delete animation;
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(endValue);
    animation->setEndValue(startValue);
    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}
void MyLabelMenuAnimated::finalizadaAnimacion()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animationOnGoing = false;
    this->setMinimumSize(minSize);
    delete animation;
    QTimer::singleShot(50, this, SLOT(emitirClicked()));
    //    emit mouseLeftClicked();
}
void MyLabelMenuAnimated::emitirClicked()
{
    emit clicked();
    emit endAnimation(objectName());

    QPixmap pixmap;
//    if(QFile::exists(":/icons/menu_extra_"+this->objectName()+".png")){
//        pixmap = QPixmap(":/icons/menu_extra_"+this->objectName()+".png");
//        menu_expanded->setBackgroundImage("menu_extra_"+this->objectName());
//    }else{
//        qDebug()<<"No se ha creado icono de este objeto menu (objectName.png) no vera menu";
//    }

    menu_expanded->show();
    menu_expanded->raise();

    int witdh_icon_menu_expand = menu_expanded->geometry().width();
    int height_icon_menu_expand = menu_expanded->geometry().height();
    int bounce = 50;
//    if(!pixmap.isNull()){
//        witdh_icon_menu_expand = pixmap.width();
//        height_icon_menu_expand = pixmap.height();
//    }
    QPoint pos_global = this->parentWidget()->mapToGlobal(this->pos());//El mapeo debe ser desde el padre de este widget
//    (porque vas a pasarle una posicion que esta dentro de el)

    int posicion_boton_x = this->parentWidget()->pos().x() + this->parentWidget()->width();

    QString  name = this->parentWidget()->objectName();
    int posicion_boton_y = pos_global.y();
    menu_expanded->setAnimationParameters(0, witdh_icon_menu_expand, bounce, ANIM_TIME_W,
                                          QRect(posicion_boton_x,posicion_boton_y
                                                , 1, height_icon_menu_expand),//start
                                          QRect(posicion_boton_x, posicion_boton_y, //pos
                                                witdh_icon_menu_expand + bounce, height_icon_menu_expand));//end
    menu_expanded->showWithAnimation();
}

void MyLabelMenuAnimated::hideChilds(){
    menu_expanded->hideChilds();
}
void MyLabelMenuAnimated::startAnimation(int delay, int scale, int anim_tim)
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
    //    connect(animation, &QPropertyAnimation::finished, this, &MyLabelMenuAnimated::setAnimationEnd);

}

void MyLabelMenuAnimated::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
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
void MyLabelMenuAnimated::setChecked(bool checked){
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
bool MyLabelMenuAnimated::isChecked(){
    return activated_state;
}
void MyLabelMenuAnimated::mouseMoveEvent(QMouseEvent *ev)
{
    emit mouseEntered(ev->pos());
}
