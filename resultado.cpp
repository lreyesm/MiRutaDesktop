#include "resultado.h"
#include "ui_resultado.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Resultado::Resultado(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Resultado)
{
    ui->setupUi(this);
    this->setWindowTitle("Resultado");
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

Resultado::~Resultado()
{
    delete ui;
}

void Resultado::populateView(QJsonObject o)
{
    QString resultado_m, modelo_m, cod_m;
    resultado_m = o.value(resultado_resultados).toString();
    cod_m = o.value(codigo_resultado_resultados).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_resultado->setText(resultado_m);
}

QString Resultado::guardarDatos(){
    QString resultado_m, modelo_m, cod_m;
    resultado_m =  ui->le_resultado->text();
    cod_m = ui->le_codigo->text();

    resultado.insert(resultado_resultados, resultado_m);
    resultado.insert(codigo_resultado_resultados, cod_m);
    return cod_m;
}


void Resultado::create_resultado_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_RESULTADO,keys,values);
}
void Resultado::update_resultado_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_RESULTADO,keys,values);
}

void Resultado::delete_resultado_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_RESULTADO,keys,values);
}

bool Resultado::writeResultados(QJsonArray jsonArray){
    QFile *data_base = new QFile(resultados_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Resultado::readResultados(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(resultados_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QStringList Resultado::getListaResultados(){
    QJsonArray jsonArray = readResultados();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_resultado_resultados).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Resultado::subirResultado(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    resultado.insert(date_time_modified_resultados, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(resultado);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Resultado::script_excecution_result,q,&QEventLoop::exit);

    if(getListaResultados().contains(codigo)){
        update_resultado_request(keys, values);
    }
    else{
        create_resultado_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::resultado_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_resultado_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_resultado_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Resultado::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_RESULTADO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_resultado"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_resultado_to_server_failed;
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_resultado_to_server_failed;
            }
        }
        else
        {
            if(ba.contains("success ok update_resultado"))
            {
                result = database_comunication::script_result::resultado_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_RESULTADO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_resultado_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_resultado"))
            {
                result = database_comunication::script_result::resultado_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_RESULTADO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_resultado"))
        {
            result = database_comunication::script_result::delete_resultado_failed;

        }
        else
        {
            if(ba.contains("success ok delete_resultado"))
            {
                result = database_comunication::script_result::delete_resultado_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Resultado::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaResultados().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirResultado(cod_m);
    emit update_tableResultados(true);
    this->close();
}
void Resultado::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirResultado(cod_m);
    emit update_tableResultados(true);
    this->close();
}
void Resultado::on_pb_borrar_clicked()
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

        eliminarResultado(cod_m);
//        subirTodasLasResultados();
        emit update_tableResultados(true);
        this->close();
    }


}
bool Resultado::eliminarResultado(QString codigo){
//    subirTodasLasResultados();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    resultado.insert(date_time_modified_resultados, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(resultado);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Resultado::script_excecution_result,q,&QEventLoop::exit);

    if(getListaResultados().contains(codigo)){
        delete_resultado_request(keys, values);
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

    case database_comunication::script_result::delete_resultado_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_resultado_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Resultado::subirTodasLasResultados()
{
//    QMap<QString, QString> mapaTiposDeResultado;
//    mapaTiposDeResultado.insert("001","TXORIERRI");
//    mapaTiposDeResultado.insert("002","DURANGUESADO");
//    mapaTiposDeResultado.insert("003","LEA-ARTIBAI");
//    mapaTiposDeResultado.insert("004","MUNGIALDE"); //preguntar por este
//    mapaTiposDeResultado.insert("005","ENCARTACIONES - RESULTADO MINERA");
//    mapaTiposDeResultado.insert("006","ARRATIA-NERBIOI");
//    mapaTiposDeResultado.insert("007","MARGEN IZQUIERDA" );


//    for(int i=0; i< mapaTiposDeResultado.size();i++){
//        QJsonObject jsonObject;
//        QString value = mapaTiposDeResultado.values().at(i);
//        QString key = mapaTiposDeResultado.keys().at(i);
//        jsonObject.insert(codigo_resultado_resultados, key);
//        jsonObject.insert(resultado_resultados, value);
//        this->setData(jsonObject);
//        QString cod = guardarDatos();
//        subirResultado(cod);
//    }
    QJsonArray jsonRes = readResultados();
    for (int i=0; i< jsonRes.size();i++) {
        QJsonObject jsonObject = jsonRes.at(i).toObject();

        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirResultado(cod);
    }
    emit update_tableResultados(true);
    this->close();
}
