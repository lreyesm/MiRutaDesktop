#include "mylineeditautocomplete.h"
#include <QResizeEvent>
#include <QGraphicsDropShadowEffect>
#include "global_variables.h"
MyLineEditAutoComplete::MyLineEditAutoComplete(QWidget *parent) : QLineEdit(parent)
{
    connect(this, &MyLineEditAutoComplete::textEdited, this, &MyLineEditAutoComplete::text_Edited);
    connect(&timerChangeThreshold, &QTimer::timeout, this, &MyLineEditAutoComplete::emitTextEdited);

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    this->setGraphicsEffect(effect);
}

void MyLineEditAutoComplete::setButtons(){

    if(button_clear){
        button_clear->deleteLater();
        button_clear = nullptr;
    }
    int height = this->height()*0.5;
    this->setStyleSheet(this->styleSheet()+QString("background-color: rgb(255, 255, 255);"
                                                   "border-radius: "+ QString::number(height/5) +"px;"));

    button_clear = new ClickableLabel(this->parentWidget());
    button_clear->setStyleSheet(QStringLiteral("QLabel{"
                                               "border-radius: 3px;"
                                               "}"
                                               "QLabel:hover:!pressed{"
                                               "background-color: #EEEEEE; "
                                               "}"));
    button_clear->setPixmap(QPixmap(":/icons/close.png"));
    button_clear->setAlignment(Qt::AlignCenter);
    connect(button_clear, &ClickableLabel::clicked, this, &MyLineEditAutoComplete::clear);
    connect(button_clear, &ClickableLabel::clicked, this, &MyLineEditAutoComplete::hideAutoCompleteList);
    button_clear->setFixedSize(height, height);
    button_clear->move(this->pos().x() + this->width() - height*1.5
                       , this->pos().y() + height/2);
    button_clear->show();
}
void MyLineEditAutoComplete::setAutoCompleteBackground(QString styleSheet)
{
    if(!backgroundLabel){
        backgroundLabel = new QLabel(this->parentWidget());
    }
    if(styleSheet.isEmpty()){
        backgroundLabel->setStyleSheet(QStringLiteral(
                                           "background-color: rgb(77, 77, 77);"
                                           "border-radius: 5px"));
    }
    else{
        backgroundLabel->setStyleSheet(styleSheet);
    }
    backgroundLabel->move(this->pos().x(), this->pos().y() + this->height());
    backgroundLabel->setFixedSize(widgetAutoComplete->size());
}
void MyLineEditAutoComplete::resizeEvent(QResizeEvent *event)
{
    QLineEdit::resizeEvent(event);
    setButtons();
    if(widgetAutoComplete){
        widgetAutoComplete->setFixedSize(this->width(), widgetAutoComplete->height());
    }
    if(backgroundLabel){
        backgroundLabel->setFixedSize(widgetAutoComplete->size());
    }
}

void MyLineEditAutoComplete::moveEvent(QMoveEvent *event)
{
    QLineEdit::moveEvent(event);
    setButtons();
    if(widgetAutoComplete){
        widgetAutoComplete->move(this->pos().x(), this->pos().y() + this->height());
    }
    if(backgroundLabel){
        backgroundLabel->move(this->pos().x(), this->pos().y() + this->height());
    }
}

void MyLineEditAutoComplete::addAutoCompleteList(QStringList list, bool show)
{
    QString item;
    foreach(item , list){
        addAutoCompleteItem(item);
    }
    if(show/* && !this->text().isEmpty()*/){
        fillAutoCompleteWidget();
        showAutoCompleteList();
    }
}

void MyLineEditAutoComplete::setAutoCompleteList(QStringList list, bool show)
{
    for(int i=0; i < list.size(); i++){
        if(list.at(i).trimmed().isEmpty()){
            list.removeAt(i);
            i--;
        }
    }
    autoCompleteList = list;
    if(show/* && !this->text().isEmpty()*/){
        fillAutoCompleteWidget();
        showAutoCompleteList();
    }
}

bool MyLineEditAutoComplete::addAutoCompleteItem(QString item, bool show) //Retorna true si añadio el item
{
    if(!autoCompleteList.contains(item)){
        autoCompleteList.prepend(item);
        if(show/* && !this->text().isEmpty()*/){
            fillAutoCompleteWidget();
            showAutoCompleteList();
        }
        return true;
    }
    return false;
}

bool MyLineEditAutoComplete::hasAutoComplteItem(QString item)
{
    if(autoCompleteList.contains(item)){
        return true;
    }
    return false;
}

QStringList MyLineEditAutoComplete::getAutoCompleteList()
{
    return autoCompleteList;
}

QString MyLineEditAutoComplete::getAutoCompleteItem(int pos)
{
    if(autoCompleteList.size() > pos){
        return autoCompleteList.at(pos);
    }
    return "";
}

void MyLineEditAutoComplete::showAutoCompleteList()
{
    if(!widgetAutoComplete){
        fillAutoCompleteWidget();
    }
    setAutoCompleteBackground();
    backgroundLabel->show();

    widgetAutoComplete->show();
    widgetAutoComplete->raise();
}

void MyLineEditAutoComplete::hideAutoCompleteList()
{
    if(backgroundLabel)
        backgroundLabel->hide();
    if(widgetAutoComplete)
        widgetAutoComplete->hide();
}

void MyLineEditAutoComplete::text_Edited(QString text)
{
    timerChangeThreshold.stop();
    timerChangeThreshold.setInterval(CHANGETHRESHOLD);
    timerChangeThreshold.start();

    textToEmit = text;
    if(textToEmit.isEmpty()){
        hideAutoCompleteList();
    }
}

void MyLineEditAutoComplete::stopChangeThresholdAndHide(){
    timerChangeThreshold.stop();
    hideAutoCompleteList();
}
void MyLineEditAutoComplete::emitTextEdited()
{
    timerChangeThreshold.stop();
    if(!textToEmit.isEmpty()){
        emit textModified(textToEmit);
    }
}

void MyLineEditAutoComplete::fillAutoCompleteWidget()
{
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setObjectName("v_layout");
    QString item;

    if(widgetAutoComplete){
        clearAutoCompleteWidget();
    }
    widgetAutoComplete = new QWidget(this->parentWidget());
    widgetAutoComplete->setFixedSize(this->width(), 0);
    vlayout->setAlignment(Qt::AlignCenter);
    widgetAutoComplete->setLayout(vlayout);

    foreach (item, autoCompleteList) {
        ClickableLabel *button = new ClickableLabel();
        button->setText(item);
        connect(button, &ClickableLabel::textClicked, this, &MyLineEditAutoComplete::emitTextSelected);
        connect(button, &ClickableLabel::clicked, this, &MyLineEditAutoComplete::hideAutoCompleteList);
        //        button->setFlat(true);
        button->setStyleSheet(QStringLiteral("border-radius: 4px;"
                                             //                                  "background-color: rgb(255, 77, 77);"
                                             "color: rgb(255, 255, 255);"));
        button->setFont(this->font());
        widgetAutoComplete->layout()->addWidget(button);
        widgetAutoComplete->setFixedSize(widgetAutoComplete->width(),
                                         widgetAutoComplete->height() + ITEMHEIGHT);
        button->setFixedSize(widgetAutoComplete->width()*9/10, ITEMHEIGHT*3/4);
    }
    widgetAutoComplete->move(this->pos().x(), this->pos().y() + this->height());
}

void MyLineEditAutoComplete::emitTextSelected(QString item){
    timerChangeThreshold.stop();
    emit itemSelected(item);
    this->setText(item);
}
void MyLineEditAutoComplete::clearAutoCompleteWidget()
{
    int c = widgetAutoComplete->children().size();
    for (int i = 0; i < c; i++) {
        QWidget *w = static_cast<QWidget*>(widgetAutoComplete->children().at(i));
        if(w){
            if(w->isWidgetType()){
                w->deleteLater();
            }
        }
    }
    widgetAutoComplete->layout()->deleteLater();
    widgetAutoComplete->deleteLater();
    widgetAutoComplete = nullptr;
}


