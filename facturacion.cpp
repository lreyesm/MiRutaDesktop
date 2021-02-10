#include "facturacion.h"
#include "ui_facturacion.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Facturacion::Facturacion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Facturacion)
{
    ui->setupUi(this);

    QStringList listFiles = getFilesListIdExp();
    ui->cb_desde->addItems(listFiles);
}

Facturacion::~Facturacion()
{
    delete ui;
}

QStringList Facturacion::getFilesListIdExp(const QString &filter)
{
    QDir dir = QDir::current();
    dir.setPath(dir.path() + "/Ficheros Informados");
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    dir.setNameFilters(QStringList()<<"*." + filter);
    QStringList listFiles = dir.entryList();
    for (int i =0; i < listFiles.size(); i++) {
        listFiles[i].remove("GCT").remove("." +filter);
    }

    return listFiles;
}

QStringList Facturacion::getFilesList(const QString &filter)
{
    QDir dir = QDir::current();
    dir.setPath(dir.path() + "/Ficheros Informados");
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    dir.setNameFilters(QStringList()<<"*." + filter);
    QStringList listFiles = dir.entryList();
    return listFiles;
}

void Facturacion::on_cb_desde_currentIndexChanged(const QString &arg1)
{
    QStringList listFiles = getFilesList();
    QString filename;
    for (int i =0; i < listFiles.size(); i++) {
        if(listFiles.at(i).contains(arg1)){
            filename = listFiles.at(i);
        }
    }
    listFiles = getFilesListIdExp();
    for (int i =0; i < listFiles.size(); i++) {
        if(arg1 > listFiles.at(i)){
            listFiles.removeAt(i);
            i--;
        }
    }

    ui->l_fecha_desde->setText(getFecha(filename));

    ui->cb_hasta->clear();
    ui->cb_hasta->addItems(listFiles);
}

QString Facturacion::getFecha(const QString &filename)
{
    QDir dir = QDir::current();
    dir.setPath(dir.path() + "/Ficheros Informados");
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    QFile file(dir.path() + "/" + filename);
    if(file.exists()){
        QFileInfo fileInfo; fileInfo.setFile(file);
        QDateTime created = fileInfo.lastModified();
        return created.toString(formato_fecha_hora_new_view);
    }
    return "...";
}

void Facturacion::on_cb_hasta_currentIndexChanged(const QString &arg1)
{
    QStringList listFiles = getFilesList();
    QString filename;
    for (int i =0; i < listFiles.size(); i++) {
        if(listFiles.at(i).contains(arg1)){
            filename = listFiles.at(i);
        }
    }
    QString fecha_hasta = getFecha(filename);
    ui->l_fecha_hasta->setText(fecha_hasta);

    QString fecha_desde =  ui->l_fecha_desde->text();
    QDateTime date_desde = QDateTime::fromString(fecha_desde, formato_fecha_hora_new_view);
    QDateTime date_hasta = QDateTime::fromString(fecha_hasta, formato_fecha_hora_new_view);
    if(date_hasta < date_desde && (ui->cb_desde->currentText() != arg1)){
        GlobalFunctions::showWarning(this, "Fecha mas antigua", "La fecha de creación del fichero final es menor a la del incicial");
    }
}

void Facturacion::on_buttonBox_accepted()
{
    QStringList listSelected;
    QString desde = ui->cb_desde->currentText();
    QString hasta = ui->cb_hasta->currentText();

    QStringList listFiles = getFilesListIdExp();
    for (int i =0; i < listFiles.size(); i++) {
        if(desde <= listFiles.at(i) && listFiles.at(i) <= hasta){
            listSelected << listFiles.at(i);
        }
    }
    emit selectedFacturation(listSelected);
}
