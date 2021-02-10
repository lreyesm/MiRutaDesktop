#include "mycheckbox.h"

MyCheckBox::MyCheckBox(QWidget *parent) : QCheckBox(parent)
{
    connect(this, &QCheckBox::toggled, this, &MyCheckBox::on_Toggle_CheckBox);
}

void MyCheckBox::onTextSelectedChanged(QString text)
{
    if(this->text().contains(text, Qt::CaseInsensitive)){
        this->show();
    }else{
        this->hide();
        this->setChecked(false);
    }
}

void MyCheckBox::on_Toggle_CheckBox(bool checked){
    Q_UNUSED(checked);
    emit toggleCheckBox(this->text());
}

void MyCheckBox::set_Checked(bool checked){
    if(!this->isHidden()){
        this->setChecked(checked);
    }
}
