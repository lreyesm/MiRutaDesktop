#include "maildialog.h"
#include "ui_maildialog.h"


MailDialog::MailDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MailDialog)
{
    ui->setupUi(this);
    data_base = new QFile("mails.dat");
    if(!data_base->exists()){
        if(data_base->open(QIODevice::WriteOnly)){
            QDataStream out(data_base);
            data_base->close();
        }
    }
    readMails();
}

MailDialog::~MailDialog()
{
    delete ui;
}

void MailDialog::on_buttonBox_accepted()
{
    writeMails(ui->le_mi_correo->text(), ui->le_mi_clave->text());

    emit send_my_email(ui->le_mi_correo->text());
    emit send_my_clave(ui->le_mi_clave->text());
    emit send_destiny_email(ui->le_destinatario->text());
}

void MailDialog::readMails(){

    if(data_base->open(QIODevice::ReadOnly))
    {
        QDataStream in(data_base);
        QString mail, password;
        while(!data_base->atEnd()){

            in>>mail;
            in>>password;

            mails<<mail;
            passwords<<password;
        }

        data_base->close();

        if(!mails.isEmpty() && !passwords.isEmpty()){
            completer = new QCompleter(mails, this);
            completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
            completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
            ui->le_mi_correo->setCompleter(completer);

            connect(completer,SIGNAL(highlighted(QString)),this, SLOT(autoCompletePassword(QString)));
        }
    }
}

void MailDialog::writeMails(QString mail, QString password){
    if(checkIfMailExist(mail)){
        return;
    }
    if(data_base->exists()) {
        if(data_base->open(QIODevice::Append))
        {
            QDataStream out(data_base);
            out<<mail;
            out<<password;
            data_base->close();
        }
    }
}

bool MailDialog::checkIfMailExist(QString mailselected){ //devuelve verdarero si correo ya existe

    QStringList mailsInDB;
    if(data_base->open(QIODevice::ReadOnly))
    {
        QDataStream in(data_base);
        QString mail, password;
        while(!data_base->atEnd()){

            in>>mail;
            in>>password;
            mailsInDB<<mail;

        }
        data_base->close();

        if(mailsInDB.contains(mailselected)){
            return true;
        }
    }
    return false;
}
void MailDialog::autoCompletePassword(QString text_completed){
    if(mails.contains(text_completed)){
        int i = mails.indexOf(text_completed);
        if(passwords.length()> i){
            ui->le_mi_clave->setText(passwords.at(i));
        }
    }
}
