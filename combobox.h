#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>

class ComboBox : public QComboBox
{
    Q_OBJECT
public:
    ComboBox(QWidget *parent = nullptr);
    void setIndex(QModelIndex i){index = i;}

    QStringList getList(){return info_list;}
signals:
    void doubleClickedComboBox(QModelIndex);

public slots:
    void setList(QAbstractItemModel *m);

protected slots:
    void mouseDoubleClickEvent(QMouseEvent *e){
        Q_UNUSED(e);
        emit doubleClickedComboBox(index);
    }
private:
    QModelIndex index;
    QStringList info_list;
};

#endif // COMBOBOX_H
