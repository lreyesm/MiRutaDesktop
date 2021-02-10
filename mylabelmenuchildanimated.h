#ifndef MYLABELMENUCHILDANIMATED_H
#define MYLABELMENUCHILDANIMATED_H

#include <QLabel>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include "mywidgetmenuanimated.h"

#define ANIM_TIME_LM 150
#define ANIM_SCALE_LM 10

class MyLabelMenuChildAnimated : public QLabel
{
    Q_OBJECT
public:
    explicit MyLabelMenuChildAnimated(QWidget *parent = nullptr);
     ~MyLabelMenuChildAnimated();
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
    void startAnimation(int delay = 0, int scale =ANIM_SCALE_LM, int anim_time =ANIM_TIME_LM);

    void setChecked(bool checked);
    bool isChecked();
    void setBackgroundImage(const QString image_filename);

public slots:
    void hideChilds();

    void generateAnimation();
signals:
    void actionPress(const QString);
    void doubleClickedLabel();
    void mouseRightClicked(const QString);
    void mouseLeftClicked(const QString);
    void clicked();
    void endAnimation(QString);
    void mouseEntered(QPoint);

protected:
    void mousePressEvent(QMouseEvent *ev);
//    void mouseMoveEvent(QMouseEvent *ev);

private slots:
    void setAnimationEnd();
    void finalizadaAnimacion();
    void setFotoInit(const QString&);
    void emitirClicked();
    void setChildOwner(const QString &n);
    void emitActionToTablaClass(QString action);
private:
    QString info ="";
    bool activated_state=false;
    bool animationOnGoing =false;
    QPropertyAnimation *animation=nullptr;
    QRect startValue;
    QRect endValue;
    QSize maxSize, minSize;
    int anim_scale = ANIM_SCALE_LM;
    int anim_time = ANIM_TIME_LM;
    MyWidgetMenuAnimated *menu_expanded;
    QWidget *baseWidget = nullptr; ///widget padre mayor en esta pantalla

};

#endif // MYLABELMENUCHILDANIMATED_H
