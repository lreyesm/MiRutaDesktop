#include "causa.h"
#include "ui_causa.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Causa::Causa(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Causa)
{
    ui->setupUi(this);
    this->setWindowTitle("Causa");
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

Causa::~Causa()
{
    delete ui;
}

void Causa::populateView(QJsonObject o)
{
    QString causa_m, arealizar_m, cod_m, accion_m, pautas_m, tipo_m, dependencia_m, resultados_m;

    cod_m = o.value(codigo_causa_causas).toString();
    arealizar_m = o.value(arealizar_causas).toString();
    causa_m = o.value(causa_causas).toString(); //Intervencion
    accion_m = o.value(accion_ordenada_causas).toString();
    pautas_m = o.value(pautas_ejecucion_causas).toString();
    tipo_m = o.value(tipo_tarea_causas).toString();
    dependencia_m = o.value(dependencia_calibre_causas).toString();
    resultados_m = o.value(resultados_posibles_causas).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_arealizar->setText(arealizar_m);
    ui->le_causa->setText(causa_m);
    ui->le_accion_ordenada->setText(accion_m);
    ui->pt_pautas->setPlainText(pautas_m);
    ui->le_tipo_tarea->setText(tipo_m);
    ui->le_dependencia_calibre->setText(dependencia_m);
    ui->le_resultados_posibles->setText(resultados_m);
}

QString Causa::guardarDatos(){
    QString causa_m, arealizar_m, cod_m, accion_m, pautas_m, tipo_m, dependencia_m, resultados_m;

    cod_m = ui->le_codigo->text();
    arealizar_m =  ui->le_arealizar->text();
    causa_m =  ui->le_causa->text();
    accion_m = ui->le_accion_ordenada->text();
    pautas_m =  ui->pt_pautas->toPlainText();
    tipo_m = ui->le_tipo_tarea->text();
    dependencia_m =  ui->le_dependencia_calibre->text();
    resultados_m = ui->le_resultados_posibles->text();

    causa.insert(codigo_causa_causas, cod_m);
    causa.insert(arealizar_causas, arealizar_m);
    causa.insert(causa_causas, causa_m);
    causa.insert(accion_ordenada_causas, accion_m);
    causa.insert(pautas_ejecucion_causas, pautas_m);
    causa.insert(tipo_tarea_causas, tipo_m);
    causa.insert(dependencia_calibre_causas, dependencia_m);
    causa.insert(resultados_posibles_causas, resultados_m);

    return cod_m;
}


void Causa::create_causa_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_CAUSA,keys,values);
}
void Causa::update_causa_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_CAUSA,keys,values);
}

void Causa::delete_causa_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_CAUSA,keys,values);
}

bool Causa::writeCausas(QJsonArray jsonArray){
    QFile *data_base = new QFile(causas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Causa::readCausas(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(causas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QString Causa::getIntervencionFromCodeCausa(QString codigo_causa){
    QJsonObject causaJsonObject = getCausaObject(codigo_causa);
    if(!causaJsonObject.isEmpty()){
        return causaJsonObject.value(causa_causas).toString();
    }
    return "";
}
QString Causa::getARealizarFromCodeCausa(QString codigo_causa){
    QJsonObject causaJsonObject = getCausaObject(codigo_causa);
    if(!causaJsonObject.isEmpty()){
        return causaJsonObject.value(arealizar_causas).toString();
    }
    return "";
}
QString Causa::getAccionOrdenadaFromCodeCausa(QString codigo_causa){
    QJsonObject causaJsonObject = getCausaObject(codigo_causa);
    if(!causaJsonObject.isEmpty()){
        return causaJsonObject.value(accion_ordenada_causas).toString();
    }
    return "";
}
QString Causa::getTipoTareaFromCodeCausa(QString codigo_causa){
    QJsonObject causaJsonObject = getCausaObject(codigo_causa);
    if(!causaJsonObject.isEmpty()){
        return causaJsonObject.value(tipo_tarea_causas).toString();
    }
    return "";
}

QJsonObject Causa::getCausaObject(QString codigo_causa){
    QStringList listaCausas = getListaCausas();
    QJsonArray jsonArray = readCausas();
    QJsonObject causaJsonObject;
    if(listaCausas.contains(codigo_causa.trimmed())){
        for (int i = 0; i < jsonArray.size(); i++) {
            causaJsonObject = jsonArray.at(i).toObject();
            if(causaJsonObject.value(codigo_causa_causas).toString().trimmed()
                    == codigo_causa.trimmed()){
                return causaJsonObject;
            }
        }
    }
    return QJsonObject();
}

QStringList Causa::getListaCausas(){
    QJsonArray jsonArray = readCausas();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_causa_causas).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Causa::subirCausa(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    causa.insert(date_time_modified_causas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(causa);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Causa::script_excecution_result,q,&QEventLoop::exit);

    if(getListaCausas().contains(codigo)){
        update_causa_request(keys, values);
    }
    else{
        create_causa_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::causa_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_causa_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_causa_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Causa::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_CAUSA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_causa"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_causa_to_server_failed;
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_causa_to_server_failed;
            }
        }
        else
        {
            if(ba.contains("success ok update_causa"))
            {
                result = database_comunication::script_result::causa_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_CAUSA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_causa_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_causa"))
            {
                result = database_comunication::script_result::causa_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_CAUSA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_causa"))
        {
            result = database_comunication::script_result::delete_causa_failed;

        }
        else
        {
            if(ba.contains("success ok delete_causa"))
            {
                result = database_comunication::script_result::delete_causa_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Causa::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaCausas().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirCausa(cod_m);
    emit update_tableCausas(true);
    this->close();
}
void Causa::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirCausa(cod_m);
    emit update_tableCausas(true);
    this->close();
}
void Causa::on_pb_borrar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    GlobalFunctions gf(this);
    if(gf.showQuestion(this, "Confirmacion", "Seguro desea eliminar el la causa?",
                             QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){

        eliminarCausa(cod_m);
//        subirTodasLasCausas();
        emit update_tableCausas(true);
        this->close();
    }


}
bool Causa::eliminarCausa(QString codigo){
//    subirTodasLasCausas();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    causa.insert(date_time_modified_causas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(causa);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Causa::script_excecution_result,q,&QEventLoop::exit);

    if(getListaCausas().contains(codigo)){
        delete_causa_request(keys, values);
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

    case database_comunication::script_result::delete_causa_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_causa_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Causa::subirTodasLasCausas()
{
    QMap<QString, QString> mapaTiposDeCausa;
    mapaTiposDeCausa.insert("001","TXORIERRI");
    mapaTiposDeCausa.insert("002","DURANGUESADO");
    mapaTiposDeCausa.insert("003","LEA-ARTIBAI");
    mapaTiposDeCausa.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeCausa.insert("005","ENCARTACIONES - CAUSA MINERA");
    mapaTiposDeCausa.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeCausa.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeCausa.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeCausa.values().at(i);
        QString key = mapaTiposDeCausa.keys().at(i);
        jsonObject.insert(codigo_causa_causas, key);
        jsonObject.insert(causa_causas, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirCausa(cod);
    }
    emit update_tableCausas(true);
    this->close();
}
