#ifndef SELECT_FILE_TO_DOWNLOAD_H
#define SELECT_FILE_TO_DOWNLOAD_H

#include <QDialog>
#include "database_comunication.h"


namespace Ui {
class Select_File_to_Download;
}

class Select_File_to_Download : public QDialog
{
    Q_OBJECT

public:
    explicit Select_File_to_Download(QWidget *parent = nullptr, QString gestor = "", QString empresa = "");
    ~Select_File_to_Download();

public slots:
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void getFilesWorkFromServer();
    void getFilesClientFromServer();
signals:
    void work_Selected_file(QString);
    void files_in_return_folder(QStringList);
private slots:
    void on_buttonBox_accepted();

    void fillComboBoxListFiles(QStringList files);
    void on_pb_aceptar_clicked();
    void on_pb_cancelar_clicked();
    void on_pb_close_clicked();
private:
    Ui::Select_File_to_Download *ui;
    database_comunication database_com;
    QString gestor;
    QString empresa;
};

#endif // SELECT_FILE_TO_DOWNLOAD_H
