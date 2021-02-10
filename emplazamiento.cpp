#include "emplazamiento.h"
#include "ui_emplazamiento.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Emplazamiento::Emplazamiento(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Emplazamiento)
{
    ui->setupUi(this);
    this->setWindowTitle("Emplazamiento");
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

Emplazamiento::~Emplazamiento()
{
    delete ui;
}

void Emplazamiento::populateView(QJsonObject o)
{
    QString emplazamiento_m, resto_m, cod_m;
    emplazamiento_m = o.value(emplazamiento_emplazamientos).toString();
    resto_m = o.value(resto_emplazamientos).toString();
    cod_m = o.value(codigo_emplazamiento_emplazamientos).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_resto->setText(resto_m);
    ui->le_emplazamiento->setText(emplazamiento_m);
}

QString Emplazamiento::guardarDatos(){
    QString resto_m, emplazamiento_m, cod_m;
    emplazamiento_m = ui->le_emplazamiento->text();
    resto_m =  ui->le_resto->text();
    cod_m = ui->le_codigo->text();

    emplazamiento.insert(emplazamiento_emplazamientos, emplazamiento_m);
    emplazamiento.insert(codigo_emplazamiento_emplazamientos, cod_m);
    emplazamiento.insert(resto_emplazamientos, resto_m);
    return cod_m;
}


void Emplazamiento::create_emplazamiento_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_EMPLAZAMIENTO,keys,values);
}
void Emplazamiento::update_emplazamiento_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_EMPLAZAMIENTO,keys,values);
}

void Emplazamiento::delete_emplazamiento_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_EMPLAZAMIENTO,keys,values);
}

bool Emplazamiento::writeEmplazamientos(QJsonArray jsonArray){
    QFile *data_base = new QFile(emplazamientos_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Emplazamiento::readEmplazamientos(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(emplazamientos_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QStringList Emplazamiento::getListaEmplazamientos(){
    QJsonArray jsonArray = readEmplazamientos();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_emplazamiento_emplazamientos).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Emplazamiento::subirEmplazamiento(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    emplazamiento.insert(date_time_modified_emplazamientos, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(emplazamiento);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Emplazamiento::script_excecution_result,q,&QEventLoop::exit);

    if(getListaEmplazamientos().contains(codigo)){
        update_emplazamiento_request(keys, values);
    }
    else{
        create_emplazamiento_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::emplazamiento_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_emplazamiento_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_emplazamiento_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Emplazamiento::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_EMPLAZAMIENTO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_emplazamiento"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_emplazamiento_to_server_failed;
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_emplazamiento_to_server_failed;
            }
        }
        else
        {
            if(ba.contains("success ok update_emplazamiento"))
            {
                result = database_comunication::script_result::emplazamiento_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_EMPLAZAMIENTO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_emplazamiento_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_emplazamiento"))
            {
                result = database_comunication::script_result::emplazamiento_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_EMPLAZAMIENTO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_emplazamiento"))
        {
            result = database_comunication::script_result::delete_emplazamiento_failed;

        }
        else
        {
            if(ba.contains("success ok delete_emplazamiento"))
            {
                result = database_comunication::script_result::delete_emplazamiento_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Emplazamiento::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaEmplazamientos().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirEmplazamiento(cod_m);
    emit update_tableEmplazamientos(true);
    this->close();
}
void Emplazamiento::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirEmplazamiento(cod_m);
    emit update_tableEmplazamientos(true);
    this->close();
}
void Emplazamiento::on_pb_borrar_clicked()
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

        eliminarEmplazamiento(cod_m);
//        subirTodasLasEmplazamientos();
        emit update_tableEmplazamientos(true);
        this->close();
    }


}
bool Emplazamiento::eliminarEmplazamiento(QString codigo){
//    subirTodasLasEmplazamientos();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    emplazamiento.insert(date_time_modified_emplazamientos, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(emplazamiento);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Emplazamiento::script_excecution_result,q,&QEventLoop::exit);

    if(getListaEmplazamientos().contains(codigo)){
        delete_emplazamiento_request(keys, values);
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

    case database_comunication::script_result::delete_emplazamiento_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_emplazamiento_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Emplazamiento::subirTodasLasEmplazamientos()
{
    QMap<QString, QString> mapaTiposDeRestoEmplazamiento;
    mapaTiposDeRestoEmplazamiento.insert("AC", "ERA");
    mapaTiposDeRestoEmplazamiento.insert("AR", "QUETA");
    mapaTiposDeRestoEmplazamiento.insert("BA", "Batería Cuarto de Contadores");
    mapaTiposDeRestoEmplazamiento.insert("BT", "Batería Escalera"); //preguntar por este
    mapaTiposDeRestoEmplazamiento.insert("CA", "LDERA");
    mapaTiposDeRestoEmplazamiento.insert("CB", "INADO");
    mapaTiposDeRestoEmplazamiento.insert("CO", "CINA");
    mapaTiposDeRestoEmplazamiento.insert("ES", "CALERA");
    mapaTiposDeRestoEmplazamiento.insert("FA", "CHADA");
    mapaTiposDeRestoEmplazamiento.insert("FR", "EGADERA");
    mapaTiposDeRestoEmplazamiento.insert("GA", "RAJE");
    mapaTiposDeRestoEmplazamiento.insert("KA", "LLE");
    mapaTiposDeRestoEmplazamiento.insert("LO", "CAL");
    mapaTiposDeRestoEmplazamiento.insert("NI", "CHO");
    mapaTiposDeRestoEmplazamiento.insert("PA", "TIO");
    mapaTiposDeRestoEmplazamiento.insert("SO", "TANO");
    mapaTiposDeRestoEmplazamiento.insert("TE", "RRAZA");
    mapaTiposDeRestoEmplazamiento.insert("TR", "ASTERO");
    mapaTiposDeRestoEmplazamiento.insert("VE", "NTANA");
    mapaTiposDeRestoEmplazamiento.insert("WC", "W.C.");
    mapaTiposDeRestoEmplazamiento.insert("CS", "SETA");
    mapaTiposDeRestoEmplazamiento.insert("CU", "ADRA");
    mapaTiposDeRestoEmplazamiento.insert("HA", "LL");
    mapaTiposDeRestoEmplazamiento.insert("TX", "OKO");
    mapaTiposDeRestoEmplazamiento.insert("PR", "ED");


    for(int i=0; i< mapaTiposDeRestoEmplazamiento.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeRestoEmplazamiento.values().at(i);
        QString key = mapaTiposDeRestoEmplazamiento.keys().at(i);
        jsonObject.insert(codigo_emplazamiento_emplazamientos, key);
        jsonObject.insert(resto_emplazamientos, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirEmplazamiento(cod);
    }
    emit update_tableEmplazamientos(true);
    this->close();
}
