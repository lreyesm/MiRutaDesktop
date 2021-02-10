#include "tipo.h"
#include "ui_tipo.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Tipo::Tipo(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Tipo)
{
    ui->setupUi(this);
    this->setWindowTitle("Tipo");
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

Tipo::~Tipo()
{
    delete ui;
}

void Tipo::populateView(QJsonObject o)
{
    QString tipo_m, modelo_m, cod_m;
    tipo_m = o.value(tipo_tipos).toString();
    cod_m = o.value(codigo_tipo_tipos).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_tipo->setText(tipo_m);
}

QString Tipo::guardarDatos(){
    QString tipo_m, modelo_m, cod_m;
    tipo_m =  ui->le_tipo->text();
    cod_m = ui->le_codigo->text();

    tipo.insert(tipo_tipos, tipo_m);
    tipo.insert(codigo_tipo_tipos, cod_m);
    return cod_m;
}


void Tipo::create_tipo_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_TIPO,keys,values);
}
void Tipo::update_tipo_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TIPO,keys,values);
}

void Tipo::delete_tipo_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_TIPO,keys,values);
}

bool Tipo::writeTipos(QJsonArray jsonArray){
    QFile data_base(tipos_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base.exists()) {
        if(data_base.open(QIODevice::WriteOnly))
        {
            data_base.seek(0);
            QDataStream out(&data_base);
            out<<jsonArray;
            data_base.close();
            return true;
        }
    }
    return false;
}

QJsonArray Tipo::readTipos(){
    QJsonArray jsonArray;
    QFile data_base(tipos_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base.exists()) {
        if(data_base.open(QIODevice::ReadOnly))
        {
            QDataStream in(&data_base);
            in>>jsonArray;
            data_base.close();
        }
    }
    return jsonArray;
}

QStringList Tipo::getListaTipos(){
    QJsonArray jsonArray = readTipos();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_tipo_tipos).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Tipo::subirTipo(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    tipo.insert(date_time_modified_tipos, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(tipo);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop q;

    connect(this, &Tipo::script_excecution_result,&q,&QEventLoop::exit);

    if(getListaTipos().contains(codigo)){
        update_tipo_request(keys, values);
    }
    else{
        create_tipo_request(keys, values);
    }

    bool res = false;
    switch(q.exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::tipo_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_tipo_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_tipo_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    return res;
}

void Tipo::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_TIPO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_tipo"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_tipo_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_tipo_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_tipo"))
            {
                result = database_comunication::script_result::tipo_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_TIPO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_tipo_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_tipo"))
            {
                result = database_comunication::script_result::tipo_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_TIPO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_tipo"))
        {
            result = database_comunication::script_result::delete_tipo_failed;

        }
        else
        {
            if(ba.contains("success ok delete_tipo"))
            {
                result = database_comunication::script_result::delete_tipo_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Tipo::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaTipos().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirTipo(cod_m);
    emit update_tableTipos(true);
    this->close();
}
void Tipo::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirTipo(cod_m);
    emit update_tableTipos(true);
    this->close();
}
void Tipo::on_pb_borrar_clicked()
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

        eliminarTipo(cod_m);
//        subirTodasLasTipos();
        emit update_tableTipos(true);
        this->close();
    }


}
bool Tipo::eliminarTipo(QString codigo){
//    subirTodasLasTipos();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    tipo.insert(date_time_modified_tipos, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(tipo);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop q;

    connect(this, &Tipo::script_excecution_result,&q,&QEventLoop::exit);

    if(getListaTipos().contains(codigo)){
        delete_tipo_request(keys, values);
    }
    else{

        return false;
    }

    bool res = false;
    switch(q.exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::delete_tipo_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_tipo_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }

    return res;
}
void Tipo::subirTodasLasTipos()
{
    QStringList lista_tipo_fluido;
    lista_tipo_fluido.append("FRIA");
    lista_tipo_fluido.append("CALIENTE");
    lista_tipo_fluido.append("ENERGIA");
    lista_tipo_fluido.append("ELECTRONICO");
    lista_tipo_fluido.append("MBUS");
    lista_tipo_fluido.append("GAS");
    lista_tipo_fluido.append("FRIGORIAS");
    lista_tipo_fluido.append("GENERAL");


    for(int i=0; i< lista_tipo_fluido.size();i++){
        QJsonObject jsonObject;
        QString codigo = QString::number(i+1);
        while(codigo.size() < 3){
            codigo.prepend("0");
        }
        jsonObject.insert(codigo_tipo_tipos, codigo);
        jsonObject.insert(tipo_tipos,lista_tipo_fluido.at(i));
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirTipo(cod);
    }
    emit update_tableTipos(true);
    this->close();
}
