#include "mycomboboxshine.h"
#include <QGraphicsDropShadowEffect>
#include "global_variables.h"

MyComboBoxShine::MyComboBoxShine(QWidget *parent) : QComboBox(parent)
{
//    this->setStyleSheet("QComboBox {"
//                        "color: black;"
//                        "font: 14px;"
//                        "padding: 1px 0px 1px 3px; /* This (useless) line resolves a bug with the font color */"
//                        " }"
//                        "QComboBox:focus {"
//                        "color: red;"
//                        "}"
//                        "QComboBox::drop-down "
//                        " {"
//                        "border: 0px;"
//                        "}"
//                        "QComboBox::down-arrow {"
//                        "image: url(:/icons/spinner_down.png);"
//                        "width: 24px;"
//                        "height: 20px;"
//                        "}");
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    this->setGraphicsEffect(effect);
}

QStringList MyComboBoxShine::items(){
    QStringList list;
    for(int i= 0; i < this->count(); i++){
        list << this->itemText(i);
    }
    return list;
}

QString MyComboBoxShine::text()
{
    return this->currentText();
}

bool MyComboBoxShine::contains(QString item){
    if(items().contains(item)){
        return true;
    }
    return false;
}

void MyComboBoxShine::removeSelected(){
    QString text = this->currentText();
    for(int i= 0; i < this->count(); i++){
        if(this->itemText(i) == text){
            this->removeItem(i);
        }
    }
}
