#include "ruta.h"
#include "ui_ruta.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Ruta::Ruta(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Ruta)
{
    ui->setupUi(this);
    this->setWindowTitle("Ruta");
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

Ruta::~Ruta()
{
    delete ui;
}

void Ruta::populateView(QJsonObject o)
{
    QString ruta_m, portal_m, cod_m, radio_portal, barrio, municipio, distrito, calle;
    ruta_m = o.value(ruta_rutas).toString();
    cod_m = o.value(codigo_ruta_rutas).toString();
    barrio = o.value(barrio_rutas).toString();
    distrito = o.value(distrito_rutas).toString();
    portal_m = o.value(portal_rutas).toString();
    radio_portal = o.value(radio_portal_rutas).toString();
    calle = o.value(calle_rutas).toString();
    municipio = o.value(municipio_rutas).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_ruta->setText(ruta_m);
    ui->le_portal->setText(portal_m);
    ui->le_radio_portal->setText(radio_portal);
    ui->le_calle->setText(calle);
    ui->le_barrio->setText(barrio);
    ui->le_distrito->setText(distrito);
    ui->le_municipio->setText(municipio);
}

QString Ruta::guardarDatos(){
    QString ruta_m, portal_m, cod_m, radio_portal, barrio, municipio, distrito, calle;
    ruta_m =  ui->le_ruta->text();
    cod_m = ui->le_codigo->text();
    portal_m =  ui->le_portal->text();
    radio_portal = ui->le_radio_portal->text();
    calle =  ui->le_calle->text();
    distrito = ui->le_distrito->text();
    barrio =  ui->le_barrio->text();
    municipio = ui->le_municipio->text();

    ruta.insert(portal_rutas, portal_m);
    ruta.insert(radio_portal_rutas, radio_portal);

    ruta.insert(barrio_rutas, barrio);
    ruta.insert(distrito_rutas, distrito);

    ruta.insert(calle_rutas, calle);
    ruta.insert(municipio_rutas, municipio);

    ruta.insert(ruta_rutas, ruta_m);
    ruta.insert(codigo_ruta_rutas, cod_m);

    return cod_m;
}


void Ruta::create_ruta_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_RUTA,keys,values);
}
void Ruta::update_ruta_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_RUTA,keys,values);
}

void Ruta::delete_ruta_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_RUTA,keys,values);
}

bool Ruta::writeRutas(QJsonArray jsonArray){
    QFile *data_base = new QFile(rutas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QString Ruta::getZonaRutaFromCodEmplazamiento(QString cod_emplazamiento){
    QJsonObject jsonObject = getRutaObjectFromCodEmplamiento(cod_emplazamiento);
    if(!jsonObject.isEmpty()){
        QString zona = jsonObject.value(barrio_rutas).toString();
        return zona;
    }
    return "";
}
QJsonObject Ruta::getRutaObjectFromCodEmplamiento(QString cod_emplazamiento){
    if(cod_emplazamiento.contains("-")){
        cod_emplazamiento=cod_emplazamiento.split("-").at(0).trimmed();
    }
    GlobalFunctions gf(nullptr, GlobalFunctions::readEmpresaSelected());

    QJsonObject jsonObject = gf.getRutaFromServer(cod_emplazamiento);
    return jsonObject;
}

QStringList Ruta::getListaRutas(){
    GlobalFunctions gf(nullptr, GlobalFunctions::readEmpresaSelected());
    QStringList lista = gf.getRutasList();
    return  lista;
}
bool Ruta::subirRuta(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ruta.insert(date_time_modified_rutas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(ruta);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Ruta::script_excecution_result,q,&QEventLoop::exit);

    GlobalFunctions gf(nullptr, GlobalFunctions::readEmpresaSelected());
    if(gf.checkIfRutaExist(codigo)){
        update_ruta_request(keys, values);
    }
    else{
        create_ruta_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::ruta_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_ruta_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_ruta_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Ruta::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_RUTA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_ruta"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_ruta_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_ruta_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_ruta"))
            {
                result = database_comunication::script_result::ruta_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_RUTA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_ruta_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_ruta"))
            {
                result = database_comunication::script_result::ruta_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_RUTA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_ruta"))
        {
            result = database_comunication::script_result::delete_ruta_failed;

        }
        else
        {
            if(ba.contains("success ok delete_ruta"))
            {
                result = database_comunication::script_result::delete_ruta_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Ruta::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaRutas().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirRuta(cod_m);
    emit update_tableRutas(true);
    this->close();
}
void Ruta::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirRuta(cod_m);
    emit update_tableRutas(true);
    this->close();
}
void Ruta::on_pb_borrar_clicked()
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

        eliminarRuta(cod_m);
        emit update_tableRutas(true);
        this->close();
    }


}
bool Ruta::eliminarRuta(QString codigo){
//    subirTodasLasRutas();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ruta.insert(date_time_modified_rutas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(ruta);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Ruta::script_excecution_result,q,&QEventLoop::exit);

    if(getListaRutas().contains(codigo)){
        delete_ruta_request(keys, values);
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

    case database_comunication::script_result::delete_ruta_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_ruta_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Ruta::subirTodasLasRutas()
{
    QMap<QString, QString> mapaTiposDeRuta;
    mapaTiposDeRuta.insert("001","TXORIERRI");
    mapaTiposDeRuta.insert("002","DURANGUESADO");
    mapaTiposDeRuta.insert("003","LEA-ARTIBAI");
    mapaTiposDeRuta.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeRuta.insert("005","ENCARTACIONES - RUTA MINERA");
    mapaTiposDeRuta.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeRuta.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeRuta.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeRuta.values().at(i);
        QString key = mapaTiposDeRuta.keys().at(i);
        jsonObject.insert(codigo_ruta_rutas, key);
        jsonObject.insert(ruta_rutas, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirRuta(cod);
    }
    emit update_tableRutas(true);
    this->close();
}
