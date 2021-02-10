#include "navegador.h"
#include "ui_navegador.h"

Navegador::Navegador(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Navegador)
{

    ui->setupUi(this);

    ui->le_site->setText("https://www.google.com");
    QString address = ui->le_site->text();
//    try{
//    ui->webView->load(QUrl(address));
//    ui->webView->load(QUrl("https://www.google.com"));
//    }catch  e){

//    }

}

Navegador::~Navegador()
{
    delete ui;
}

void Navegador::on_pb_go_to_googleMaps_clicked()
{
//    ui->webView->load(QUrl("https://www.google.com/maps/"));
}

void Navegador::on_pb_go_clicked()
{
//    ui->webView->
    QString address = ui->le_site->text();
    if(!address.contains("https://www.")){
        address.insert(0, "https://www.");
    }
//    ui->webView->load(QUrl(address));
    ui->le_site->setText("ooo");
}
