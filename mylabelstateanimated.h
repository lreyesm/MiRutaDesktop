#ifndef MYLABELSTATEANIMATED_H
#define MYLABELSTATEANIMATED_H

#include <QLabel>
#include <QMouseEvent>
#include <QPropertyAnimation>

#define ANIM_TIME_RB 150
#define ANIM_SCALE_RB 10

class MyLabelStateAnimated : public QLabel
{
    Q_OBJECT
public:
    explicit MyLabelStateAnimated(QWidget *parent = nullptr);
     ~MyLabelStateAnimated();
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
    void startAnimation(bool emit_clicked = true, int scale =ANIM_SCALE_RB, int anim_time =ANIM_TIME_RB);

    void setChecked(bool checked);
    bool isChecked();
    bool is_Enabled(){return enabled;}
    void set_Enabled(bool enable){ enabled = enable;}
signals:
    void doubleClickedLabel();
    void mouseRightClicked();
    void mouseLeftClicked();
    void clicked();
    void send_text(QString);
    void mouseEntered(QPoint);

protected:
    void mousePressEvent(QMouseEvent *ev);
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
    QSize maxSize, child_size, label_size;
    int anim_scale = ANIM_SCALE_RB;
    int anim_time = ANIM_TIME_RB;
    bool enabled=true;
    bool emit_enabled = true;
    bool myanimationEnabled = true;

};

#endif // MYLABELSTATEANIMATED_H
