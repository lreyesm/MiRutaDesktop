#ifndef MYWIDGETANIMATED_H
#define MYWIDGETANIMATED_H

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QPainter>
#include <QStyleOption>

#ifndef ANIM_TIME_W
#define ANIM_TIME_W 200  //Tiempo de animacion
#endif
#define ANIM_SCALE_W 235  //Ancho de icono de menu
#define BOUNCE_SCALE_W 50  //Ancho de rebote de menu

class MyWidgetAnimated : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit MyWidgetAnimated(QWidget *parent = nullptr);
     ~MyWidgetAnimated();

    void setEditable(bool);
    void setTextInfo(QString information);
    bool isActivatedState(){
        return activated_state;
    }
    void setActivadeState(bool state){
        activated_state = state;
    }
    void setAnimationInit(QRect start = QRect(), QRect end = QRect());
    void setAnimation(QPropertyAnimation *);
    void startAnimation(int delay = 0, int scale =ANIM_SCALE_W, int anim_time =ANIM_TIME_W, QRect start = QRect(), QRect end = QRect());

    void setChecked(bool checked);
    bool isChecked();

    void setBackgroundImage(const QString image_filename);
    void setAnimationParameters(int delay, int scale, int time, QRect start, QRect end);
    void setAnimationParameters(int delay, int scale, int bounce, int time, QRect start, QRect end);
public slots:
    void showWithAnimation();

    void hideMenu();
signals:
    void doubleClickedLabel();
    void mouseRightClicked();
    void mouseLeftClicked();
    void clicked();
    void hideAll();
    void mouseEntered(QPoint);

protected:
    void mousePressEvent(QMouseEvent *ev);
//    void mouseMoveEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *e){//sin esto no se ve el fondo seteado
        Q_UNUSED(e);
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }
protected slots:
    void focusOutEvent(QFocusEvent *event);
    void focusInEvent(QFocusEvent *event);
private slots:
    void setAnimationEnd();
    void finalizadaAnimacion();
    void resizeWidget();
    void setOnIcon();
    void finalizadaAnimacionEnd();
    void setSizeInit();
private:
    QString info ="";
    bool activated_state=false;
    bool parametersSeted=false;
    bool animationOnGoing =false;
    QPropertyAnimation *animation=nullptr;
    QRect startValue;
    QRect midValue;
    QRect endValue;
    QSize maxSize;
    int anim_scale = ANIM_SCALE_W;
    int bounce_scale = BOUNCE_SCALE_W;
    int anim_time = ANIM_TIME_W;

};

#endif // MYWIDGETANIMATED_H
