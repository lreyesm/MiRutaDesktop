#include "qlabel_button.h"

//Esta clase se esta usando exclusivamente para los dias del calendario
QLabel_Button::QLabel_Button(QWidget *parent)
{
    this->setParent(parent);
}

QLabel_Button::~QLabel_Button()
{
    this->deleteLater();
}

void QLabel_Button::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    emit doubleClicked();
}

void QLabel_Button::mousePressEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
{
    Q_UNUSED(e);
    emit clicked();
    emit send_text(this->text());
    if(checked) {
        setChecked(false);
    }else{
        setChecked(true);
    }
}

void QLabel_Button::setChecked(bool check){
    if(checked_background.isEmpty()){
        return;
    }
    this->checked = check;
    if(checked){
        this->setStyleSheet(checked_background + "color: rgb(255, 255, 255);  border-radius: 5px;");
        this->setFont(font_style);
    }else{
        this->setStyleSheet("color: rgb(100, 100, 100);");
        this->setFont(font_style);
    }
}

void QLabel_Button::setCheckedBackground(QColor color)
{
    QString n_color = QString::number(color.red()) + ", " + QString::number(color.green()) + ", " + QString::number(color.blue());
    checked_background = "background-color: rgb(" + n_color + ");";
    font_style = font();
}
