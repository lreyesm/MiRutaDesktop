#include "rueda.h"
#include "ui_rueda.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Rueda::Rueda(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Rueda)
{
    ui->setupUi(this);
    this->setWindowTitle("Rueda");
    if(newOne){
        ui->pb_agregar->show();
        ui->pb_actualizar->hide();
        ui->pb_borrar->hide();
    }else{
        ui->pb_agregar->hide();
        ui->pb_actualizar->show();
        ui->pb_borrar->show();
    }
}

Rueda::~Rueda()
{
    delete ui;
}

void Rueda::populateView(QJsonObject o)
{
    QString rueda_m, modelo_m, cod_m;
    rueda_m = o.value(rueda_ruedas).toString();
    cod_m = o.value(codigo_rueda_ruedas).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_rueda->setText(rueda_m);
}

QString Rueda::guardarDatos(){
    QString rueda_m, modelo_m, cod_m;
    rueda_m =  ui->le_rueda->text();
    cod_m = ui->le_codigo->text();

    rueda.insert(rueda_ruedas, rueda_m);
    rueda.insert(codigo_rueda_ruedas, cod_m);
    return cod_m;
}


void Rueda::create_rueda_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_RUEDA,keys,values);
}
void Rueda::update_rueda_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_RUEDA,keys,values);
}

void Rueda::delete_rueda_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_RUEDA,keys,values);
}

bool Rueda::writeRuedas(QJsonArray jsonArray){
    QFile *data_base = new QFile(ruedas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::WriteOnly))
        {
            data_base->seek(0);
            QDataStream out(data_base);
            out<<jsonArray;
            data_base->close();
            return true;
        }
    }
    return false;
}

QJsonArray Rueda::readRuedas(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(ruedas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::ReadOnly))
        {
            QDataStream in(data_base);
            in>>jsonArray;
            data_base->close();
        }
    }
    return jsonArray;
}

QStringList Rueda::getListaRuedas(){
    QJsonArray jsonArray = readRuedas();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_rueda_ruedas).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Rueda::subirRueda(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    rueda.insert(date_time_modified_ruedas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(rueda);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Rueda::script_excecution_result,q,&QEventLoop::exit);

    if(getListaRuedas().contains(codigo)){
        update_rueda_request(keys, values);
    }
    else{
        create_rueda_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::rueda_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_rueda_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_rueda_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Rueda::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_RUEDA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_rueda"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_rueda_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_rueda_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_rueda"))
            {
                result = database_comunication::script_result::rueda_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_RUEDA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_rueda_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_rueda"))
            {
                result = database_comunication::script_result::rueda_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_RUEDA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_rueda"))
        {
            result = database_comunication::script_result::delete_rueda_failed;

        }
        else
        {
            if(ba.contains("success ok delete_rueda"))
            {
                result = database_comunication::script_result::delete_rueda_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Rueda::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaRuedas().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirRueda(cod_m);
    emit update_tableRuedas(true);
    this->close();
}
void Rueda::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirRueda(cod_m);
    emit update_tableRuedas(true);
    this->close();
}
void Rueda::on_pb_borrar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    GlobalFunctions gf(this);
    if(gf.showQuestion(this, "Confirmacion", "Seguro desea eliminar el modelo?",
                             QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){

        eliminarRueda(cod_m);
//        subirTodasLasRuedas();
        emit update_tableRuedas(true);
        this->close();
    }


}
bool Rueda::eliminarRueda(QString codigo){
//    subirTodasLasRuedas();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    rueda.insert(date_time_modified_ruedas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(rueda);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Rueda::script_excecution_result,q,&QEventLoop::exit);

    if(getListaRuedas().contains(codigo)){
        delete_rueda_request(keys, values);
    }
    else{
        delete q;
        return false;
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::delete_rueda_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_rueda_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Rueda::subirTodasLasRuedas()
{
    QMap<QString, QString> mapaTiposDeRueda;
    mapaTiposDeRueda.insert("001","TXORIERRI");
    mapaTiposDeRueda.insert("002","DURANGUESADO");
    mapaTiposDeRueda.insert("003","LEA-ARTIBAI");
    mapaTiposDeRueda.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeRueda.insert("005","ENCARTACIONES - RUEDA MINERA");
    mapaTiposDeRueda.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeRueda.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeRueda.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeRueda.values().at(i);
        QString key = mapaTiposDeRueda.keys().at(i);
        jsonObject.insert(codigo_rueda_ruedas, key);
        jsonObject.insert(rueda_ruedas, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirRueda(cod);
    }
    emit update_tableRuedas(true);
    this->close();
}
