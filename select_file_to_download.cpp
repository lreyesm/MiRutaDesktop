#include "select_file_to_download.h"
#include "ui_select_file_to_download.h"

Select_File_to_Download::Select_File_to_Download(QWidget *parent, QString gestor, QString empresa) :
    QDialog(parent),
    ui(new Ui::Select_File_to_Download)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    this->gestor = gestor;
    this->empresa = empresa;
}

Select_File_to_Download::~Select_File_to_Download()
{
    delete ui;
}
void Select_File_to_Download::on_pb_aceptar_clicked()
{
    on_buttonBox_accepted();
    this->close();
    emit accepted();
}

void Select_File_to_Download::on_pb_cancelar_clicked()
{
    emit rejected();
    this->close();
}
void Select_File_to_Download::on_pb_close_clicked()
{
    this->close();
    emit reject();
}

void Select_File_to_Download::on_buttonBox_accepted()
{
    QString selection = ui->cb_selections->currentText();
    emit work_Selected_file(selection);
}
void Select_File_to_Download::getFilesWorkFromServer()
{
    QStringList keys, values;
    keys << "empresa";
    values << empresa;
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_FILES_WORK,keys,values);
}

void Select_File_to_Download::getFilesClientFromServer()
{
    QStringList keys, values;
    keys << "gestor" << "empresa";
    values << gestor << empresa;
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_FILES_CLIENT_WORK,keys,values);
}

void Select_File_to_Download::fillComboBoxListFiles(QStringList files)
{
    for (int i=0; i < files.size(); i++) {
        if(!files.at(i).contains("Trabajo_Salvado") && !files.at(i).contains("dia", Qt::CaseInsensitive)){
            files.removeAt(i);
            i--;
        }
    }
    ui->cb_selections->clear();
    files.prepend("Ninguno");
    ui->cb_selections->addItems(files);
}
void Select_File_to_Download::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    //    QString test = QString::fromUtf8(ba);
    //    ui->plainTextEdit->setPlainText(test);
    if(tipo == database_comunication::GET_FILES_WORK) //esto no se usa en esta app
    {
        ba.chop(2);
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
        QString str = QString::fromUtf8(ba).replace("\n","");
        QStringList files;
        if(str.contains(" -- ")){
            files = str.split(" -- ");
        }
        fillComboBoxListFiles(files);

        //        emit OperatorsFilled(tipo);
    }

    if(tipo == database_comunication::GET_FILES_CLIENT_WORK)
    {
        ba.chop(2);
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
        QString str = QString::fromUtf8(ba).replace("\n","");
        QStringList files;
        if(str.contains(" -- ")){
            files = str.split(" -- ");
        }
        emit files_in_return_folder(files);
        //        emit OperatorsFilled(tipo);
    }
}
