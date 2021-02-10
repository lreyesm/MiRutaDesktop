#ifndef MYCHECKBOX_H
#define MYCHECKBOX_H

#include <QCheckBox>

class MyCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    explicit MyCheckBox(QWidget *parent = nullptr);

signals:
    void toggleCheckBox(QString);

public slots:
    void onTextSelectedChanged(QString);
    void set_Checked(bool checked);

private slots:
    void on_Toggle_CheckBox(bool checked);
};

#endif // MYCHECKBOX_H
