#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

class TableView : public QTableView
{
    Q_OBJECT
public:
    TableView(QWidget *parent = 0);
signals:
    void doubleClickedComboBox();

protected slots:
    void mouseDoubleClickEvent(QMouseEvent *e){
        emit doubleClickedComboBox();
    }
//    void mousePressEvent(QMouseEvent *e){
//        emit doubleClickedComboBox();
//    }
};

#endif // TABLEVIEW_H
