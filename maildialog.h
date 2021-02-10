#ifndef MAILDIALOG_H
#define MAILDIALOG_H

#include <QDialog>
#include <QFile>
#include <QCompleter>

namespace Ui {
class MailDialog;
}

class MailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MailDialog(QWidget *parent = nullptr);
    ~MailDialog();

signals:
    void send_my_email(QString);
    void send_my_clave(QString);
    void send_destiny_email(QString);

private slots:
    void on_buttonBox_accepted();
    void autoCompletePassword(QString);

private:
    Ui::MailDialog *ui;
    void readMails();
    void writeMails(QString, QString);
    QFile *data_base;
    QStringList mails, passwords;
    QCompleter *completer;
    bool checkIfMailExist(QString mail);
};

#endif // MAILDIALOG_H
