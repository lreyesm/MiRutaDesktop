#include "longitud.h"
#include "ui_longitud.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Longitud::Longitud(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Longitud)
{
    ui->setupUi(this);
    this->setWindowTitle("Longitud");
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

Longitud::~Longitud()
{
    delete ui;
}

void Longitud::populateView(QJsonObject o)
{
    QString longitud_m, modelo_m, cod_m;
    longitud_m = o.value(longitud_longitudes).toString();
    cod_m = o.value(codigo_longitud_longitudes).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_longitud->setText(longitud_m);
}

QString Longitud::guardarDatos(){
    QString longitud_m, modelo_m, cod_m;
    longitud_m =  ui->le_longitud->text();
    cod_m = ui->le_codigo->text();

    longitud.insert(longitud_longitudes, longitud_m);
    longitud.insert(codigo_longitud_longitudes, cod_m);
    return cod_m;
}


void Longitud::create_longitud_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_LONGITUD,keys,values);
}
void Longitud::update_longitud_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_LONGITUD,keys,values);
}

void Longitud::delete_longitud_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_LONGITUD,keys,values);
}

bool Longitud::writeLongitudes(QJsonArray jsonArray){
    QFile *data_base = new QFile(longitudes_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Longitud::readLongitudes(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(longitudes_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QStringList Longitud::getListaLongitudes(){
    QJsonArray jsonArray = readLongitudes();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_longitud_longitudes).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Longitud::subirLongitud(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    longitud.insert(date_time_modified_longitudes, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(longitud);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Longitud::script_excecution_result,q,&QEventLoop::exit);

    if(getListaLongitudes().contains(codigo)){
        update_longitud_request(keys, values);
    }
    else{
        create_longitud_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::longitud_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_longitud_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_longitud_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Longitud::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_LONGITUD)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_longitud"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_longitud_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_longitud_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_longitud"))
            {
                result = database_comunication::script_result::longitud_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_LONGITUD)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_longitud_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_longitud"))
            {
                result = database_comunication::script_result::longitud_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_LONGITUD)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_longitud"))
        {
            result = database_comunication::script_result::delete_longitud_failed;

        }
        else
        {
            if(ba.contains("success ok delete_longitud"))
            {
                result = database_comunication::script_result::delete_longitud_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Longitud::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaLongitudes().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirLongitud(cod_m);
    emit update_tableLongitudes(true);
    this->close();
}
void Longitud::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirLongitud(cod_m);
    emit update_tableLongitudes(true);
    this->close();
}
void Longitud::on_pb_borrar_clicked()
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

        eliminarLongitud(cod_m);
//        subirTodasLasLongitudes();
        emit update_tableLongitudes(true);
        this->close();
    }


}
bool Longitud::eliminarLongitud(QString codigo){
//    subirTodasLasLongitudes();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    longitud.insert(date_time_modified_longitudes, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(longitud);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Longitud::script_excecution_result,q,&QEventLoop::exit);

    if(getListaLongitudes().contains(codigo)){
        delete_longitud_request(keys, values);
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

    case database_comunication::script_result::delete_longitud_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_longitud_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Longitud::subirTodasLasLongitudes()
{
    QMap<QString, QString> mapaTiposDeLongitud;
    mapaTiposDeLongitud.insert("001","TXORIERRI");
    mapaTiposDeLongitud.insert("002","DURANGUESADO");
    mapaTiposDeLongitud.insert("003","LEA-ARTIBAI");
    mapaTiposDeLongitud.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeLongitud.insert("005","ENCARTACIONES - LONGITUD MINERA");
    mapaTiposDeLongitud.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeLongitud.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeLongitud.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeLongitud.values().at(i);
        QString key = mapaTiposDeLongitud.keys().at(i);
        jsonObject.insert(codigo_longitud_longitudes, key);
        jsonObject.insert(longitud_longitudes, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirLongitud(cod);
    }
    emit update_tableLongitudes(true);
    this->close();
}
