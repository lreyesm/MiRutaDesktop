#include "datetimewidget.h"
#include "ui_datetimewidget.h"
#include "processesclass.h"
#include <QDebug>

DateTimeWidget::DateTimeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DateTimeWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    fillJsonDate();

    setCurrentDateTime(); //pone fecha en fecha_hora
}

DateTimeWidget::~DateTimeWidget()
{
    delete ui;
}

void DateTimeWidget::showCenter(){

    this->move(53, 0);
    this->show();
    this->raise();
}

void DateTimeWidget::setCurrentDateTime(){
    setDateTime(fecha_hora);
}

void DateTimeWidget::setDateTime(QDateTime dt){
    ui->l_year->setText(QString::number(dt.date().year()));
    fillDaysLabel(dt.date().month());
    setSelectedDay(dt.date().day());

    int h = dt.time().hour();
    int m = dt.time().minute();
    if(h >= 12){
        am_pm = true;
        if(h > 12){
            h -= 12;
        }
    }else{
        am_pm = false;
        if(h == 0){
            h=12;
        }
    }
    ui->l_hour->setText(QString::number(h));
    QString min = QString::number(m);
    if(min.size() < 2){
        min.prepend("0");
    }
    ui->l_minutes->setText(min);
    if(am_pm){
        ui->l_am_pm->setText("pm");
    }else {
        ui->l_am_pm->setText("am");
    }
}

void DateTimeWidget::setSelectedDay(int day){
    unmarkAllOtherDays();
    int c= ui->widget_days->children().size();
    for (int i=0; i < c; i++) {
        if(static_cast<QLabel_Button*>((ui->widget_days->
                                        children().at(i)))->text() == QString::number(day)){
            static_cast<QLabel_Button*>((ui->widget_days->
                                         children().at(i)))->setChecked(true);
            selected_day = day;
        }
    }
}
void DateTimeWidget::deleteAllChilds(){
    int c= ui->widget_days->children().size();
    for (int i=0; i < c; i++) {
        disconnect(static_cast<QLabel_Button*>((ui->widget_days->
                                                children().at(i))),
                   &QLabel_Button::send_text, this, &DateTimeWidget::onClickedDay);
        static_cast<QLabel_Button*>((ui->widget_days->
                                     children().at(i)))->deleteLater();

    }
}
void DateTimeWidget::unmarkAllOtherDays(){
    int c= ui->widget_days->children().size();
    for (int i=0; i < c; i++) {
        static_cast<QLabel_Button*>((ui->widget_days->
                                     children().at(i)))->setChecked(false);

    }
}
void DateTimeWidget::clearWidgets(QLayout * layout) {
//    if (!layout)
//        return;
//    while (auto item = layout->takeAt(0)) {
//        delete item->widget();
//        clearWidgets(item->layout());
//    }
}
void DateTimeWidget::fillDaysLabel(int month){
    deleteAllChilds();
    QString m = mapMonth.key(month);
    ui->l_month->setText(m);
    int y = ui->l_year->text().toInt();
    int d = 1;
    QDate date;
    int x_pos = X_POS_LABEL_DAY_INIT;
    int y_pos = Y_POS_LABEL_DAY_INIT;

    date.setDate(y,month,1);
    if(date.dayOfWeek() < 7){
        x_pos += date.dayOfWeek()*X_POS_LABEL_DAY_INC;
    }else{

    }

    while(date.setDate(y,month,d)){
        QLabel_Button *day_label = new QLabel_Button(ui->widget_days);
        day_label->setText(QString::number(d));
        day_label->move(x_pos, y_pos);
        day_label->setAlignment(Qt::AlignHCenter| Qt::AlignVCenter);
        QFont font = day_label->font();
        font.setFamily("Segoe UI");
        font.setPointSize(14);
        day_label->setFont(font);
        day_label->setCheckedBackground(QColor(54, 141, 204));
        day_label->setFixedSize(30, 30);
        day_label->show();
        connect(day_label, &QLabel_Button::send_text, this, &DateTimeWidget::onClickedDay);
        x_pos+=X_POS_LABEL_DAY_INC;
        if(x_pos >= X_POS_LABEL_DAY_INIT + 7*X_POS_LABEL_DAY_INC){
            y_pos+= Y_POS_LABEL_DAY_INC;
            x_pos = X_POS_LABEL_DAY_INIT;
        }
        d++;
    }
    setSelectedDay(1);
}
void DateTimeWidget::onClickedDay(QString day_text){
    unmarkAllOtherDays();
    selected_day = day_text.toInt();
}

void DateTimeWidget::fillJsonDate(){
    mapMonth.insert("ENERO", 1);
    mapMonth.insert("FEBRERO" , 2);
    mapMonth.insert("MARZO", 3);
    mapMonth.insert("ABRIL", 4);
    mapMonth.insert("MAYO", 5);
    mapMonth.insert("JUNIO", 6);
    mapMonth.insert("JULIO", 7);
    mapMonth.insert("AGOSTO", 8);
    mapMonth.insert("SEPTIEMBRE", 9);
    mapMonth.insert("OCTUBRE", 10);
    mapMonth.insert("NOVIEMBRE", 11);
    mapMonth.insert("DICIEMBRE", 12);
}

void DateTimeWidget::on_l_up_hour_clicked()
{
    int h = ui->l_hour->text().toInt();
    h++;
    if(h > 12){
        h = 1;
    }
    if(h == 12){
        if(am_pm){
            //            on_l_up_day_clicked();
        }
        on_l_up_am_pm_clicked();
    }
    ui->l_hour->setText(QString::number(h));
}
void DateTimeWidget::on_l_down_hour_clicked()
{
    int h = ui->l_hour->text().toInt();
    h--;
    if(h < 1){
        h = 12;
    }
    if(h == 11){
        if(!am_pm){
            //            on_l_down_day_clicked();
        }
        on_l_up_am_pm_clicked();
    }
    ui->l_hour->setText(QString::number(h));
}

void DateTimeWidget::on_l_up_minutes_clicked()
{
    int m = ui->l_minutes->text().toInt();
    m++;
    if(m > 59){
        m = 0;
        on_l_up_hour_clicked();
    }
    QString min = QString::number(m);
    if(min.size() < 2){
        min.prepend("0");
    }
    ui->l_minutes->setText(min);
}
void DateTimeWidget::on_l_down_minutes_clicked()
{
    int m = ui->l_minutes->text().toInt();
    m--;
    if(m < 0){
        m = 59;
        on_l_down_hour_clicked();
    }
    QString min = QString::number(m);
    if(min.size() < 2){
        min.prepend("0");
    }
    ui->l_minutes->setText(min);
}

void DateTimeWidget::on_l_up_am_pm_clicked()
{
    am_pm = !am_pm;
    if(am_pm){
        ui->l_am_pm->setText("pm");
    }else{
        ui->l_am_pm->setText("am");
    }
}
void DateTimeWidget::on_l_down_am_pm_clicked()
{
    am_pm = !am_pm;
    if(am_pm){
        ui->l_am_pm->setText("pm");
    }else{
        ui->l_am_pm->setText("am");
    }
}

//void DateTimeWidget::on_l_up_day_clicked()
//{
//    int d = ui->l_day->text().toInt();
//    int m = mapMonth.value(ui->l_month->text());
//    int y = ui->l_year->text().toInt();
//    d++;
//    QDate date;
//    if(!date.setDate(y,m,d)){
//        d = 1;
//        on_l_up_month_clicked();
//    }
//    ui->l_day->setText(QString::number(d));
//}
//void DateTimeWidget::on_l_down_day_clicked()
//{
//    int d = ui->l_day->text().toInt();
//    int m = mapMonth.value(ui->l_month->text());
//    m--; //si bajo debo verificar el ultimo dia del mes anterior
//    if(m < 1){
//        m = 12;
//    }
//    int y = ui->l_year->text().toInt();
//    d--;
//    QDate date;
//    if(d < 1){
//        d = 31;
//        while(!date.setDate(y,m,d)){
//            d--;
//        }
//        on_l_down_month_clicked();
//    }
//    ui->l_day->setText(QString::number(d));
//}

void DateTimeWidget::on_l_up_month_clicked()
{
    selected_day = 1;
    QString m = ui->l_month->text();
    int m_int = mapMonth.value(m);
    m_int ++;
    if(m_int > 12){
        m_int = 1;
        on_l_up_year_clicked();
    }
    fillDaysLabel(m_int);
}
void DateTimeWidget::on_l_down_month_clicked()
{
    selected_day = 1;
    QString m = ui->l_month->text();
    int m_int = mapMonth.value(m);
    m_int --;
    if(m_int < 1){
        m_int = 12;
        on_l_down_year_clicked();
    }
    fillDaysLabel(m_int);
}

void DateTimeWidget::on_l_up_year_clicked()
{
    int y = ui->l_year->text().toInt();
    y++;
    ui->l_year->setText(QString::number(y));
    QString m = ui->l_month->text();
    int m_int = mapMonth.value(m);
    fillDaysLabel(m_int);
}
void DateTimeWidget::on_l_down_year_clicked()
{
    int y = ui->l_year->text().toInt();
    y--;
    ui->l_year->setText(QString::number(y));
    QString m = ui->l_month->text();
    int m_int = mapMonth.value(m);
    fillDaysLabel(m_int);
}

QString DateTimeWidget::dateTimeProcessInPC(QDateTime dt){
    QString date_string = "date " + dt.date().toString("dd-MM-yyyy");
    QString time_string = "time " + dt.time().toString("hh:mm:ss AP");

    QString str = ProcessesClass::executeProcess(this, date_string, ProcessesClass::WINDOWS);

    str = ProcessesClass::executeProcess(this, time_string, ProcessesClass::WINDOWS);

    return str;
}

QString DateTimeWidget::dateTimeProcessInRaspi(QDateTime dt){
    //sudo date -s "21 APR 2020 19:45:00"
        QMap<QString, int> mapMonthEnglish;
        mapMonthEnglish.insert("JANUARY", 1);
        mapMonthEnglish.insert("FEBRUARY" , 2);
        mapMonthEnglish.insert("MARCH", 3);
        mapMonthEnglish.insert("APRIL", 4);
        mapMonthEnglish.insert("MAY", 5);
        mapMonthEnglish.insert("JUNE", 6);
        mapMonthEnglish.insert("JULY", 7);
        mapMonthEnglish.insert("AUGUST", 8);
        mapMonthEnglish.insert("SEPTEMBER", 9);
        mapMonthEnglish.insert("OCTOBER", 10);
        mapMonthEnglish.insert("NOVEMBER", 11);
        mapMonthEnglish.insert("DECEMBER", 12);

        QString day =  " \"" + QString::number(dt.date().day());
        QString month = " " + mapMonthEnglish.key(dt.date().month()).left(3);
        QString year = " " + QString::number(dt.date().year());

        QString date_string = "sudo date -s" + day + month + year;
        QString time_string = " " + dt.time().toString("HH:mm:ss") + "\"";
        QProcess *proc_ovpn = new QProcess(this);
        proc_ovpn->setProcessChannelMode(QProcess::MergedChannels);

        proc_ovpn->start("sh",QStringList() << "-c" << date_string + time_string);

        if(!proc_ovpn->waitForStarted()) //default wait time 30 sec
            qDebug() << " cannot start process ";

        int waitTime = 500 ; //60 sec
        if (!proc_ovpn->waitForFinished(waitTime))
            qDebug() << "timeout .. ";

        QString str(proc_ovpn->readAllStandardOutput());
        return str;
}

void DateTimeWidget::on_l_am_pm_clicked()
{
    on_l_up_am_pm_clicked();
}
QDateTime DateTimeWidget::getFechaHora(){
    int min = ui->l_minutes->text().toInt();
    int h = ui->l_hour->text().toInt();
    if(am_pm && h!=12){
        h+=12;
    }
    if(!am_pm && h==12){
        h=0;
    }

    int d = selected_day;
    int m = mapMonth.value(ui->l_month->text());
    int y = ui->l_year->text().toInt();

    fecha_hora.setDate(QDate(y, m, d));
    fecha_hora.setTime(QTime(h, min));

    emit fechaHora(fecha_hora);
    return fecha_hora;
}
void DateTimeWidget::on_pb_ok_clicked()
{
    int min = ui->l_minutes->text().toInt();
    int h = ui->l_hour->text().toInt();
    if(am_pm && h!=12){
        h+=12;
    }
    if(!am_pm && h==12){
        h=0;
    }

    int d = selected_day;
    int m = mapMonth.value(ui->l_month->text());
    int y = ui->l_year->text().toInt();

    fecha_hora.setDate(QDate(y, m, d));
    fecha_hora.setTime(QTime(h, min));

    dateTimeProcessInRaspi(fecha_hora);

    emit settedFecha();

    this->close();
}
