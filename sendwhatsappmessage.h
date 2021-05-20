#ifndef SENDWHATSAPPMESSAGE_H
#define SENDWHATSAPPMESSAGE_H

#include <QWidget>

namespace Ui {
class SendWhatsappMessage;
}

class SendWhatsappMessage : public QWidget
{
    Q_OBJECT

public:
    explicit SendWhatsappMessage(QWidget *parent = nullptr, QString gestor = "",
                                 QString dir = "", QString abonado = "",
                                 QString phone1 = "", QString phone2 = "");
    ~SendWhatsappMessage();

    void setPhones(QString phone1, QString phone2);
private slots:
    void on_pb_aceptar_clicked();
    void on_pb_close_clicked();
    void on_pb_cancelar_clicked();

    void setCountryCode(QString item);
private:
    Ui::SendWhatsappMessage *ui;
    QString countryCode = "";
    QString phone1 = "";
    QString phone2 = "";
};

#endif // SENDWHATSAPPMESSAGE_H
