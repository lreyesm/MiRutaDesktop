#ifndef MYRADIOBUTTONANIMATED_H
#define MYRADIOBUTTONANIMATED_H

#include "mypointanimated.h"

#define ANIM_TIME_RB 150
#define ANIM_SCALE_RB 10

class MyRadioButtonAnimated : public QLabel
{
    Q_OBJECT
public:
    explicit MyRadioButtonAnimated(QWidget *parent = nullptr);
     ~MyRadioButtonAnimated();
    void setEditable(bool);
    void setTextInfo(QString information);
    bool isActivatedState(){
        return activated_state;
    }
    void setActivadeState(bool state){
        activated_state = state;
    }
    void setAnimationInit();
    void setAnimation(QPropertyAnimation *);
    void startAnimation(int delay = 0, int scale =ANIM_SCALE_RB, int anim_time =ANIM_TIME_RB);

    void setChecked(bool checked);
    bool isChecked();
    void movePointPosition(QPoint pos);
signals:
    void doubleClickedLabel();
    void mouseRightClicked();
    void mouseLeftClicked();
    void clicked();
    void mouseEntered(QPoint);

protected:
    void mousePressEvent(QMouseEvent *ev);
//    void mouseMoveEvent(QMouseEvent *ev);
//    void leaveEvent(QEvent *event);
//    void enterEvent(QEvent *event);
    void moveEvent(QMoveEvent *event);

private slots:
    void setAnimationEnd();
    void finalizadaAnimacion();
    void setFotoInit(const QString&);
    void startAnimationFromChild();
    void emitirClicked();
private:
    QString info ="";
    bool activated_state=false, enteredEvent=false;
    bool animationOnGoing =false;
    QPropertyAnimation *animation=nullptr, *animation_child=nullptr;
    QRect startValue, child_startValue;
    QRect endValue, child_endValue;
    QSize maxSize, child_size;
    int anim_scale = ANIM_SCALE_RB;
    int anim_time = ANIM_TIME_RB;
    MyPointAnimated *radioPicture;

};

#endif // MYRADIOBUTTONANIMATED_H
