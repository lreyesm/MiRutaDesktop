#include "sendwhatsappmessage.h"
#include "ui_sendwhatsappmessage.h"
#include <QDesktopServices>
#include <QJsonArray>
#include "database_comunication.h"

SendWhatsappMessage::SendWhatsappMessage(QWidget *parent, QString gestor, QString dir, QString abonado, QString phone1, QString phone2) :
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

    QString messageText = "";
    if(gestor == "CABB"){
        messageText += "\nUR-KONTAGAILUAN hurrengo esku-hartzeari buruzko oharra.";

        messageText += "\n\nGeconta Medidores de Fluidos SL-k, Bilbao Bizkaia";
        messageText += "\nUr Partzuergoaren entrepresa kolaboratzaile gisa,";
        messageText += "\ninformatzen zaitu " + dir + ", kalean,";
        messageText += "\nabonatu zenbakia " + abonado + ", ";
        messageText += "\nur-hotzaren kontagailuarekin lotura duen ";
        messageText += "\nesku-hartze bat egin behar dugula.";
        messageText += "\nArren eskatzen dizugu gurekin harremanetan";
        messageText += "\njar zaitezela bisita eguna eta ordua adosteko.";

        messageText += "\n\n\nAviso de próxima intervención en CONTADOR DE AGUA.";

        messageText += "\n\nGeconta Medidores de fluidos SL, como empresa ";
        messageText += "\ncolaboradora del " + gestor + ", le informa que estamos ";
        messageText += "\npendientes de una intervención que implica ";
        messageText += "\nal contador de agua fría de ";
        messageText += "\n" + dir + ", abonado " + abonado + ".";
        messageText += "\nLe rogamos que se ponga en contacto con nosotros,";
        messageText += "\npara coordinar fecha y hora de visita.";
    }
    else{
         messageText = "GESTOR: " + gestor;
         messageText += "\n\nAviso de próximo cambio de contador en:";
         messageText += "\n\nDIRECCIÓN: " + dir;
         messageText += "\nABONADO: " + abonado;
         messageText += "\n\n";
    }
    // QString messageText = "GESTOR: " + gestor;
    // messageText += "\n\nAviso de próximo cambio de contador en:";
    // messageText += "\n\nDIRECCIÓN: " + dir;
    // messageText += "\nABONADO: " + abonado;
    // messageText += "\n\n";

    ui->pt_message->setPlainText(messageText);
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

