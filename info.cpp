#include "info.h"
#include "ui_info.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Info::Info(QWidget *parent, bool newOne, QString empresa) :
    QWidget(parent),
    ui(new Ui::Info)
{
    ui->setupUi(this);
    this->setWindowTitle("Info");
    this->empresa = empresa;
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

Info::~Info()
{
    delete ui;
}

bool Info::getInfoInServer()
{
    QStringList keys, values;
    QEventLoop *q = new QEventLoop();

    connect(this, &Info::script_excecution_result,q,&QEventLoop::exit);

    get_info_request();

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::get_infos_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    }
    delete q;
    return res;
}
void Info::actualizarInfoInServer(QJsonObject o)
{
    populateView(o);
    on_pb_actualizar_clicked();
}

void Info::populateView(QJsonObject o)
{
    info = o;
    QString info_m, idexp, cod_m, idsat, idOrden;
    info_m = o.value(info_infos).toString();
    cod_m = o.value(codigo_info_infos).toString();
    idexp = o.value(lastIDExportacion_infos).toString();
    idsat = o.value(lastIDSAT_infos).toString();
    idOrden = o.value(lastIDOrden_infos).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_info->setText(info_m);
    ui->le_idexp->setText(idexp);
    ui->le_idsat->setText(idsat);
    ui->le_idorden->setText(idOrden);
}

QString Info::guardarDatos(){
    QString info_m, idexp, cod_m, idsat, idOrden;
    info_m =  ui->le_info->text();
    cod_m = ui->le_codigo->text();
    idexp =  ui->le_idexp->text();
    idsat = ui->le_idsat->text();
    idOrden = ui->le_idorden->text();

    info.insert(info_infos, info_m);
    info.insert(codigo_info_infos, cod_m);
    info.insert(lastIDExportacion_infos, idexp);
    info.insert(lastIDSAT_infos, idsat);
    info.insert(lastIDOrden_infos, idOrden);

    return cod_m;
}

void Info::get_info_request()
{
    QStringList keys, values;
    keys << "empresa";
    values << empresa.toLower();
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_INFOS,keys,values);
}

void Info::create_info_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_INFO,keys,values);
}
void Info::update_info_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_INFO,keys,values);
}

void Info::delete_info_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_INFO,keys,values);
}

bool Info::writeInfos(QJsonArray jsonArray){
    QFile *data_base = new QFile(infos_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Info::readInfos(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(infos_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QStringList Info::getListaInfos(){
    QJsonArray jsonArray = readInfos();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_info_infos).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Info::subirInfo(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    info.insert(date_time_modified_infos, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(info);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop *q = new QEventLoop();

    connect(this, &Info::script_excecution_result,q,&QEventLoop::exit);

    if(getListaInfos().contains(codigo)){
        update_info_request(keys, values);
    }
    else{
        create_info_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::info_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_info_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_info_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Info::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::GET_INFOS)
    {
        ba.remove(0,2);
        ba.chop(2);

        if(ba.contains("not success get_infos"))
        {
            result = database_comunication::script_result::get_infos_failed;
        }
        else
        {
            QJsonArray jsonArrayAllInfos = database_comunication::getJsonArray(ba);
            writeInfos(jsonArrayAllInfos);
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::CREATE_INFO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_info"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_info_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_info_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_info"))
            {
                result = database_comunication::script_result::info_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_INFO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_info_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_info"))
            {
                result = database_comunication::script_result::info_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_INFO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_info"))
        {
            result = database_comunication::script_result::delete_info_failed;

        }
        else
        {
            if(ba.contains("success ok delete_info"))
            {
                result = database_comunication::script_result::delete_info_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Info::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty() && !this->isHidden()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaInfos().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirInfo(cod_m);
    emit update_tableInfos(true);
    this->close();
}
void Info::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty() && !this->isHidden()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirInfo(cod_m);
    emit update_tableInfos(true);
    this->close();
}
void Info::on_pb_borrar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty() && !this->isHidden()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    GlobalFunctions gf(this);
    if(gf.showQuestion(this, "Confirmacion", "Seguro desea eliminar el modelo?",
                             QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){

        eliminarInfo(cod_m);
//        subirTodasLasInfos();
        emit update_tableInfos(true);
        this->close();
    }


}
bool Info::eliminarInfo(QString codigo){
//    subirTodasLasInfos();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    info.insert(date_time_modified_infos, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(info);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop *q = new QEventLoop();

    connect(this, &Info::script_excecution_result,q,&QEventLoop::exit);

    if(getListaInfos().contains(codigo)){
        delete_info_request(keys, values);
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

    case database_comunication::script_result::delete_info_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_info_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Info::subirTodasLasInfos()
{
    QMap<QString, QString> mapaTiposDeInfo;
    mapaTiposDeInfo.insert("001","TXORIERRI");
    mapaTiposDeInfo.insert("002","DURANGUESADO");
    mapaTiposDeInfo.insert("003","LEA-ARTIBAI");
    mapaTiposDeInfo.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeInfo.insert("005","ENCARTACIONES - INFO MINERA");
    mapaTiposDeInfo.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeInfo.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeInfo.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeInfo.values().at(i);
        QString key = mapaTiposDeInfo.keys().at(i);
        jsonObject.insert(codigo_info_infos, key);
        jsonObject.insert(info_infos, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirInfo(cod);
    }
    emit update_tableInfos(true);
    this->close();
}
