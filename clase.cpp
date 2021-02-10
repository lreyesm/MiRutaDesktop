#include "clase.h"
#include "ui_clase.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Clase::Clase(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Clase)
{
    ui->setupUi(this);
    this->setWindowTitle("Clase");
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

Clase::~Clase()
{
    delete ui;
}

void Clase::populateView(QJsonObject o)
{
    QString clase_m, modelo_m, cod_m;
    clase_m = o.value(clase_clases).toString();
    cod_m = o.value(codigo_clase_clases).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_clase->setText(clase_m);
}

QString Clase::guardarDatos(){
    QString clase_m, modelo_m, cod_m;
    clase_m =  ui->le_clase->text();
    cod_m = ui->le_codigo->text();

    clase.insert(clase_clases, clase_m);
    clase.insert(codigo_clase_clases, cod_m);
    return cod_m;
}


void Clase::create_clase_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_CLASE,keys,values);
}
void Clase::update_clase_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_CLASE,keys,values);
}

void Clase::delete_clase_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_CLASE,keys,values);
}

bool Clase::writeClases(QJsonArray jsonArray){
    QFile *data_base = new QFile(clases_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Clase::readClases(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(clases_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QStringList Clase::getListaClases(){
    QJsonArray jsonArray = readClases();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_clase_clases).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Clase::subirClase(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    clase.insert(date_time_modified_clases, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(clase);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Clase::script_excecution_result,q,&QEventLoop::exit);

    if(getListaClases().contains(codigo)){
        update_clase_request(keys, values);
    }
    else{
        create_clase_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::clase_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_clase_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_clase_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Clase::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_CLASE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_clase"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_clase_to_server_failed;
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_clase_to_server_failed;
            }
        }
        else
        {
            if(ba.contains("success ok update_clase"))
            {
                result = database_comunication::script_result::clase_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_CLASE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_clase_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_clase"))
            {
                result = database_comunication::script_result::clase_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_CLASE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_clase"))
        {
            result = database_comunication::script_result::delete_clase_failed;

        }
        else
        {
            if(ba.contains("success ok delete_clase"))
            {
                result = database_comunication::script_result::delete_clase_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Clase::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaClases().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirClase(cod_m);
    emit update_tableClases(true);
    this->close();
}
void Clase::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirClase(cod_m);
    emit update_tableClases(true);
    this->close();
}
void Clase::on_pb_borrar_clicked()
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

        eliminarClase(cod_m);
//        subirTodasLasClases();
        emit update_tableClases(true);
        this->close();
    }


}
bool Clase::eliminarClase(QString codigo){
//    subirTodasLasClases();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    clase.insert(date_time_modified_clases, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(clase);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Clase::script_excecution_result,q,&QEventLoop::exit);

    if(getListaClases().contains(codigo)){
        delete_clase_request(keys, values);
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

    case database_comunication::script_result::delete_clase_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_clase_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Clase::subirTodasLasClases()
{
    QMap<QString, QString> mapaTiposDeClase;
    mapaTiposDeClase.insert("B", "Fijo en Boca de Riego");
    mapaTiposDeClase.insert("C", "Combinado");
    mapaTiposDeClase.insert("M", "Chorro Múltiple");
    mapaTiposDeClase.insert("R", "Volumetrico con Radio Emisor");
    mapaTiposDeClase.insert("S", "Chorro Multiple con Radio Emisor");
    mapaTiposDeClase.insert("T", "Chorro Único con Radio Emisor");
    mapaTiposDeClase.insert("U", "Chorro Único");
    mapaTiposDeClase.insert("V", "Volumétrico sin Radio Emisor");
    mapaTiposDeClase.insert("W", "Woltman");


    for(int i=0; i< mapaTiposDeClase.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeClase.values().at(i);
        QString key = mapaTiposDeClase.keys().at(i);
        jsonObject.insert(codigo_clase_clases, key);
        jsonObject.insert(clase_clases, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirClase(cod);
    }
    emit update_tableClases(true);
    this->close();
}
