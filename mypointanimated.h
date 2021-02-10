#ifndef MYPOINTANIMATED_H
#define MYPOINTANIMATED_H

#include <QLabel>
#include <QMouseEvent>
#include <QPropertyAnimation>

#define ANIM_TIME 150
#define ANIM_SCALE 10

class MyPointAnimated : public QLabel
{
    Q_OBJECT
public:
    explicit MyPointAnimated(QWidget *parent = nullptr);
     ~MyPointAnimated();
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
    void startAnimation(int delay = 0, int scale =ANIM_SCALE, int anim_time =ANIM_TIME);

    void setChecked(bool checked);
    bool isChecked();
    bool isOngoingAnimation();
signals:
    void doubleClickedLabel();
    void mouseRightClicked();
    void mouseLeftClicked();
    void clicked();
    void mouseEntered(QPoint);

protected:
    void mousePressEvent(QMouseEvent *ev);
//    void mouseMoveEvent(QMouseEvent *ev);

private slots:
    void setAnimationEnd();
    void finalizadaAnimacion();
    void setFotoInit(const QString&);
    void emitirClicked();
private:
    QString info ="";
    bool activated_state=false;
    bool animationOnGoing =false;
    QPropertyAnimation *animation=nullptr;
    QRect startValue;
    QRect endValue;
    QSize maxSize;
    int anim_scale = ANIM_SCALE;
    int anim_time = ANIM_TIME;

};

#endif // MYPOINTANIMATED_H
