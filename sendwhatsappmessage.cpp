#include "sendwhatsappmessage.h"
#include "ui_sendwhatsappmessage.h"
#include <QDesktopServices>
#include <QJsonArray>
#include "database_comunication.h"

SendWhatsappMessage::SendWhatsappMessage(QWidget *parent, QString phone1, QString phone2) :
    QWidget(parent),
    ui(new Ui::SendWhatsappMessage)
{
    ui->setupUi(this);

    setPhones(phone1, phone2);

    QFile file(":/files/country_code.txt");
    QByteArray data_json;
    QJsonArray jsonArray;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        data_json = file.readAll();
        jsonArray = database_comunication::getJsonArray(data_json);
        file.close();
    }
    QString spainItem;
    QStringList items;
    for (int i=0; i< jsonArray.size(); i++) {
        QJsonObject jsonObject = jsonArray.at(i).toObject();
        //{"name":"Israel","dial_code":"+972","code":"IL"}
        QString country = jsonObject.value("name_es").toString();
        QString code = jsonObject.value("dial_code").toString();
        QString item = country + "  " + code;
        items.append(item);
        if(country == "España"){
            spainItem = item;
        }

    }
    items.sort();
    ui->l_spinner_country->addItems(items);
    ui->l_spinner_country->setText(spainItem);
    setCountryCode(spainItem);
    connect(ui->l_spinner_country, &MyLabelSpinner::itemSelected, this, &SendWhatsappMessage::setCountryCode);

    this->move(parent->width()/2-this->width()/2, parent->height()/2-this->height()/2);
}

SendWhatsappMessage::~SendWhatsappMessage()
{
    delete ui;
}

void SendWhatsappMessage::setPhones(QString phone1, QString phone2){
    if(!phone1.isEmpty()){
        this->phone1 = phone1;
        phone1 = "+" + countryCode + " " + phone1;
        ui->cb_phone_1->setText(phone1);

    }else{
        ui->cb_phone_1->hide();
        ui->cb_phone_2->setChecked(true);
    }
    if(!phone2.isEmpty()){
        this->phone2 = phone2;
        phone2 = "+" + countryCode + " " + phone2;
        ui->cb_phone_2->setText(phone2);
    }else{
        ui->cb_phone_2->hide();
        ui->cb_phone_1->setChecked(true);
    }
}
void SendWhatsappMessage::setCountryCode(QString item)
{
    QStringList split = item.split("+");
    if(split.size() >= 2){
        countryCode = split.at(1);
        setPhones(phone1, phone2);
    }
}


void SendWhatsappMessage::on_pb_close_clicked()
{
    this->close();
}

void SendWhatsappMessage::on_pb_aceptar_clicked()
{
    QString number = "";
    if(ui->cb_phone_1->isChecked()){
        number = ui->cb_phone_1->text();
    }
    else{
        number = ui->cb_phone_2->text();
    }
    QString mess = ui->pt_message->toPlainText();
    QString link = "https://api.whatsapp.com/send?phone="+ number +
            "&text=" + mess.toUtf8()/*URLEncoder.encode(text, "UTF-8")*/;
    QDesktopServices::openUrl(QUrl(link));
    this->close();
}

void SendWhatsappMessage::on_pb_cancelar_clicked()
{
    this->close();
}

