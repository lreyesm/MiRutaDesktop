#ifndef MYTABLEVIEW_H
#define MYTABLEVIEW_H

#include <QTableView>
#include <QMouseEvent>

class MyTableView : public QTableView
{
    Q_OBJECT
public:
    explicit MyTableView(QWidget *parent = nullptr);

signals:
    void rightCliked(QMouseEvent*);
    void leftCliked(QMouseEvent*);
protected slots:
    void mousePressEvent(QMouseEvent *ev){
        if(ev->buttons()==Qt::RightButton){
            emit rightCliked(ev);
        }
        else if(ev->buttons()==Qt::LeftButton){
            emit leftCliked(ev);
        }
        QTableView::mousePressEvent(ev);
    }

public slots:
};

#endif // MYTABLEVIEW_H
