#include "mylabelspinner.h"
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include "global_variables.h"
#include <QDebug>
#include <QScrollArea>

MyLabelSpinner::MyLabelSpinner(QWidget *parent) : ClickableLabel(parent)
{
    connect(this, &ClickableLabel::clicked, this, &MyLabelSpinner::showHideSpinner);

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(10);
    effect->setOffset(3);
//    effect->setColor(color_blue_app);
    this->setGraphicsEffect(effect);

}
void MyLabelSpinner::hideEvent(QHideEvent *event){
    emit hiding();
    hideSpinnerList();
    QLabel::hideEvent(event);
}
void MyLabelSpinner::showEvent(QShowEvent *event){
    emit showing();
    QLabel::showEvent(event);
}
void MyLabelSpinner::showHideSpinner(){
    if(shown){
        hideSpinnerList();
    }else {
        showSpinnerList();
    }
}
void MyLabelSpinner::setButtons(){

    if(button_expand != nullptr){
        delete button_expand;
        button_expand = nullptr;
    }
    int height = this->height();

    button_expand = new ClickableLabel(this->parentWidget());
    button_expand->setStyleSheet(this->styleSheet());
    button_expand->setBackgroundImage(("icons/spinner_down.png"));
    button_expand->setAlignment(Qt::AlignCenter);
    button_expand->setScaledContents(true);
    connect(button_expand, &ClickableLabel::clicked,
            this, &MyLabelSpinner::showHideSpinner);
    connect(this, &MyLabelSpinner::showing,
            button_expand, &ClickableLabel::show);
    connect(this, &MyLabelSpinner::hiding,
            button_expand, &ClickableLabel::hide);
    button_expand->setFixedSize(height, height);
    button_expand->move(this->pos().x() + this->width() - height
                        , this->pos().y() /*+ height/2*/);
    if(!this->isHidden()){
        button_expand->show();
    }else{
        button_expand->hide();
    }
    button_expand->setEnabled(this->isEnabled());
}
void MyLabelSpinner::resizeEvent(QResizeEvent *event)
{
    QLabel::resizeEvent(event);
    setButtons();
    if(widgetSpinnerList){
        widgetSpinnerList->setFixedSize(this->width(), widgetSpinnerList->height());
        fillSpinnerWidget();
    }
    if(backgroundLabel){
        backgroundLabel->setFixedSize(widgetSpinnerList->size());
    }
    //    for (int i=0; i < buttons.size(); i++) {
    //        buttons[i]->setWidth(static_cast<int>((static_cast<double>(this->width()*9)/10)));
    //    }
    emit width_Changed(static_cast<int>((static_cast<double>(this->width()*9)/10)));
}

void MyLabelSpinner::moveEvent(QMoveEvent *event)
{
    QLabel::moveEvent(event);
    setButtons();
    if(widgetSpinnerList){
        widgetSpinnerList->move(this->pos().x(), this->pos().y() + this->height());
    }
    if(backgroundLabel){
        backgroundLabel->move(this->pos().x(), this->pos().y() + this->height());
    }
}
void MyLabelSpinner::addSpinnerList(QStringList list, bool show)
{
    if(list.isEmpty()){
        return;
    }
    QString item;
    foreach(item , list){
        addSpinnerItem(item);
    }
    if(show/* && !this->text().isEmpty()*/){
        fillSpinnerWidget();
        showSpinnerList();
    }
}
QString MyLabelSpinner::currentText(){
    return this->text();
}
void MyLabelSpinner::addItems(QStringList list, bool show)
{
    if(list.isEmpty()){
        return;
    }
    setSpinnerList(list, show);
}

void MyLabelSpinner::appendItems(QStringList list, bool show)
{
    if(list.isEmpty()){
        return;
    }
    QString item;
    foreach(item, list){
        if (!hasSpinnerItem(item)) {
            spinnerList.prepend(item);
        }
    }
    addItems(spinnerList, show);
}

void MyLabelSpinner::setSpinnerList(QStringList list, bool show)
{
    if(list.isEmpty()){
        return;
    }
    for(int i=0; i < list.size(); i++){
        if(list.at(i).trimmed().isEmpty()){
            list.removeAt(i);
            i--;
        }
    }
    spinnerList = list;
    fillSpinnerWidget();
    if(show){
        showSpinnerList();//para mostrar automaticamente al llenar lista
    }
    if(this->text().isEmpty())
    {
        this->setText(list.first());

    }
    this->setToolTip(this->text());
}
bool MyLabelSpinner::addItem(QString item) //Retorna true si añadio el item
{
    if(!spinnerList.contains(item)){
        spinnerList.prepend(item);
        setSpinnerList(spinnerList, true);
        return true;
    }
    return false;
}
bool MyLabelSpinner::addSpinnerItem(QString item, bool show) //Retorna true si añadio el item
{
    if(!spinnerList.contains(item)){
        spinnerList.prepend(item);
        setSpinnerList(spinnerList, show);
        return true;
    }
    return false;
}
void MyLabelSpinner::removeItems(QStringList items){
    QString item;
    foreach(item, items){
        removeItem(item);
    }
}
void MyLabelSpinner::removeItems(){
    hideSpinnerList();
    spinnerList.clear();
    this->clear();
}

void MyLabelSpinner::set_Enabled(bool enable)
{
    this->setEnabled(enable);
    if(button_expand){
        button_expand->setEnabled(enable);
    }
}
bool MyLabelSpinner::removeItem(QString item){
    if(hasSpinnerItem(item)){
        hideSpinnerList();
        this->clear();
        spinnerList.removeAll(item);
        addItems(spinnerList);
        return true;
    }
    return false;
}
bool MyLabelSpinner::hasSpinnerItem(QString item)
{
    if(spinnerList.contains(item)){
        return true;
    }
    return false;
}
QStringList MyLabelSpinner::items()
{
    return spinnerList;
}

QStringList MyLabelSpinner::getSpinnerList()
{
    return spinnerList;
}

QString MyLabelSpinner::getSpinnerItem(int pos)
{
    if(spinnerList.size() > pos){
        return spinnerList.at(pos);
    }
    return "";
}

void MyLabelSpinner::clearSpinnerWidget()
{
    int c = widgetSpinnerList->children().size();
    for (int i = 0; i < c; i++) {
        QWidget *w = static_cast<QWidget*>(widgetSpinnerList->children().at(i));
        if(w){
            if(w->isWidgetType()){
                w->deleteLater();
            }
        }
    }
    widgetSpinnerList->layout()->deleteLater();
    widgetSpinnerList->deleteLater();
    widgetSpinnerList = nullptr;
}
void MyLabelSpinner::fillSpinnerWidget()
{
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setObjectName("v_layout");
    vlayout->setContentsMargins(10,3,0,0);
    vlayout->setSpacing(5);
    QString item;

    if(widgetSpinnerList){
        clearSpinnerWidget();
    }
    widgetSpinnerList = new QWidget(this->parentWidget());
    QWidget *widgetSpinnerListValues = new QWidget;

    int width = this->width();
    widgetSpinnerListValues->setFixedWidth(width*9/10);
    widgetSpinnerList->setFixedWidth(width);

    vlayout->setAlignment(Qt::AlignCenter);
    widgetSpinnerListValues->setLayout(vlayout);

    foreach (item, spinnerList) {
        ClickableLabel *button = new ClickableLabel();
        button->setText(item);
        button->setToolTip(item);
        connect(button, &ClickableLabel::textClicked, this, &MyLabelSpinner::emitTextSelected);
        connect(button, &ClickableLabel::clicked, this, &MyLabelSpinner::hideSpinnerList);
        connect(this, &MyLabelSpinner::width_Changed, button, &ClickableLabel::setWidth);
        //        button->setFlat(true);
        button->setAlignment(Qt::AlignCenter);
        button->setStyleSheet(QStringLiteral(
                                  "QLabel{"
                                  "border-radius: 4px;"
                                  "background-color: rgb(77, 77, 77);"
                                  "color: rgb(255, 255, 255);"
                                  "}"
                                  "QToolTip{"
                                  "border-radius: 4px;"
                                  "background-color: rgb(77, 77, 77);"
                                  "color: rgb(255, 255, 255);"
                                  "}"
                                  "QLabel:hover:!pressed{"
                                  "background-color: rgb(100, 100, 100);"
                                  "}"
                                  ));
        button->setFont(this->font());
        button->setFixedSize(width*4/5, ITEMHEIGHT*3/4);
        widgetSpinnerListValues->layout()->addWidget(button);

    }
    QScrollArea *scroll = new QScrollArea;
    QVBoxLayout *layout = new QVBoxLayout(widgetSpinnerList);
    layout->setAlignment(Qt::AlignCenter);
    layout->setMargin(0);
    if(spinnerList.size() < 6){
        widgetSpinnerList->setFixedSize(widgetSpinnerList->width(),
                                        (spinnerList.size()) * (ITEMHEIGHT));
    }else{
        widgetSpinnerList->setFixedSize(widgetSpinnerList->width(),
                                        (5) * (ITEMHEIGHT));
        layout->setContentsMargins(0, 8, 8, 2);
    }
    scroll->setWidget(widgetSpinnerListValues);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet(getScrollBarStyle());
    layout->addWidget(scroll);
    widgetSpinnerList->move(this->pos().x(), this->pos().y() + this->height());
    widgetSpinnerList->hide();
}
QString MyLabelSpinner::getScrollBarStyle(){
    QString style =
            "QScrollBar:vertical{"
            "border: 2px #777777;"
            "background-color: solid #777777;"
            "border-radius: 5px;"
            "width: 10px;"
            "margin: 3px 0px 3px 0px;"
            "}"

            "QScrollBar::handle:vertical{"
            "background-color: #777777;"
            "border-radius: 5px;"
            "min-height: 20px;"
            "}"

            "QScrollBar::add-line:vertical{"
            "border: 2px solid white;"
            "background: solid white;"
            "border-radius: 5px;"
            "height 10px;"
            "subcontrol-position: bottom;"
            "subcontrol-origin: margin;"
            "}"

            "QScrollBar::sub-line:vertical{"
            "border: 2px solid white;"
            "background: solid white;"
            "border-radius: 5px;"
            "height 10px;"
            "subcontrol-position: top;"
            "subcontrol-origin: margin;"
            "}"

            "QScrollBar:horizontal{"
            "border: 2px #777777;"
            "background-color: solid #777777;"
            "border-radius: 5px;"
            "height: 10px;"
            "margin: 3px 0px 3px 0px;"
            "}"

            "QScrollBar::handle:horizontal{"
            "background-color: #777777;"
            "border-radius: 5px;"
            "min-height: 20px;"
            "}"

            "QScrollBar::add-line:horizontal{"
            "border: 2px solid white;"
            "background: solid white;"
            "border-radius: 5px;"
            "width 10px;"
            "subcontrol-position: right;"
            "subcontrol-origin: margin;"
            "}"

            "QScrollBar::sub-line:horizontal{"
            "border: 2px solid white;"
            "background: solid white;"
            "border-radius: 5px;"
            "width 10px;"
            "subcontrol-position: left;"
            "subcontrol-origin: margin;"
            "}";

    return style;
}

void MyLabelSpinner::emitTextSelected(QString item){
    QString previousText = this->text();
    this->setToolTip(item);
    this->setText(item);
    emit itemSelected(item);
    if(previousText != item){
        emit currentTextChangedGetPrevious(item, previousText);
        emit currentTextChanged(item);
        emit currentIndexChanged(item);
    }

}
void MyLabelSpinner::showSpinnerList()
{
    if(spinnerList.isEmpty()){
        return;
    }
    shown = true;
    if(!widgetSpinnerList){
        fillSpinnerWidget();
    }
    setSpinnerBackground();
    backgroundLabel->show();

    widgetSpinnerList->show();
    QString style = backgroundLabel->styleSheet();
    widgetSpinnerList->setStyleSheet(style);
    widgetSpinnerList->raise();
}

void MyLabelSpinner::setSpinnerBackground(QString styleSheet)
{
    QPoint thisPos = this->mapToGlobal(QPoint(0,0));
    QPoint expandPos = thisPos;
    expandPos.setY(thisPos.y()+this->height());
    if(!backgroundLabel){
        backgroundLabel = new QLabel(this->parentWidget());
        backgroundLabel->setStyleSheet(QStringLiteral(
                                           "background-color: rgb(77, 77, 77);"
                                           "border-radius: 5px"));

        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
        effect->setBlurRadius(30);
        backgroundLabel->setGraphicsEffect(effect);
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
    //    backgroundLabel->move(expandPos);

    backgroundLabel->setFixedSize(widgetSpinnerList->width(), widgetSpinnerList->height() + 6);
}
void MyLabelSpinner::hideSpinnerList()
{
    shown = false;
    if(backgroundLabel)
        backgroundLabel->hide();
    if(widgetSpinnerList)
        widgetSpinnerList->hide();
}
void MyLabelSpinner::animateUp(QWidget *widget)
{
    previousGeometry = widget->geometry();
    QRect startValue = widget->geometry();
    QRect endValue = QRect(startValue.x(),startValue.y()
                           ,startValue.width(), 0);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->setDuration(ANIM_TIME);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
}
void MyLabelSpinner::animateDown(QWidget *widget)
{
    previousGeometry = widget->geometry();
    QRect endValue = widget->geometry();
    QRect startValue = QRect(endValue.x(),endValue.y()
                             ,endValue.width(), 0);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->setDuration(ANIM_TIME);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
}
void MyLabelSpinner::finalizadaAnimacion()
{
    this->setGeometry(previousGeometry);
}
