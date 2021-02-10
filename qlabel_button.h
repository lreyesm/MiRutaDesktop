#ifndef QLABEL_BUTTON_H
#define QLABEL_BUTTON_H

#include <QLabel>
#include <QMouseEvent>

class QLabel_Button : public QLabel
{
    Q_OBJECT
public:
    explicit QLabel_Button(QWidget *parent = nullptr);
    ~QLabel_Button();


public slots:
    void setChecked(bool check);
    void setCheckedBackground(QColor color);
signals:
    void clicked();
    void doubleClicked();
    void rightClicked();
    void leftClicked();
    void send_text(QString);

protected slots:
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *e);
    
private:
    bool checked = false;
    QString checked_background="";
    QFont font_style;
};

#endif // QLABEL_BUTTON_H
