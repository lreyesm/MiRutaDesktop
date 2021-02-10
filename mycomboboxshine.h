#ifndef MYCOMBOBOXSHINE_H
#define MYCOMBOBOXSHINE_H

#include <QComboBox>

class MyComboBoxShine : public QComboBox
{
    Q_OBJECT
public:
    explicit MyComboBoxShine(QWidget *parent = nullptr);

    QStringList items();
    QString text();
    void removeSelected();
    bool contains(QString item);
signals:

};

#endif // MYCOMBOBOXSHINE_H
