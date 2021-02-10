#include "calibre.h"
#include "ui_calibre.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Calibre::Calibre(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Calibre)
{
    ui->setupUi(this);
    this->setWindowTitle("Calibre");
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

Calibre::~Calibre()
{
    delete ui;
}

void Calibre::populateView(QJsonObject o)
{
    QString calibre_m, modelo_m, cod_m;
    calibre_m = o.value(calibre_calibres).toString();
    cod_m = o.value(codigo_calibre_calibres).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_calibre->setText(calibre_m);
}

QString Calibre::guardarDatos(){
    QString calibre_m, modelo_m, cod_m;
    calibre_m =  ui->le_calibre->text();
    cod_m = ui->le_codigo->text();

    calibre.insert(calibre_calibres, calibre_m);
    calibre.insert(codigo_calibre_calibres, cod_m);
    return cod_m;
}


void Calibre::create_calibre_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_CALIBRE,keys,values);
}
void Calibre::update_calibre_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_CALIBRE,keys,values);
}

void Calibre::delete_calibre_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_CALIBRE,keys,values);
}

bool Calibre::writeCalibres(QJsonArray jsonArray){
    QFile *data_base = new QFile(calibres_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Calibre::readCalibres(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(calibres_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QStringList Calibre::getListaCalibres(){
    QJsonArray jsonArray = readCalibres();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_calibre_calibres).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Calibre::subirCalibre(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    calibre.insert(date_time_modified_calibres, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(calibre);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Calibre::script_excecution_result,q,&QEventLoop::exit);

    if(getListaCalibres().contains(codigo)){
        update_calibre_request(keys, values);
    }
    else{
        create_calibre_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::calibre_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_calibre_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_calibre_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Calibre::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_CALIBRE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_calibre"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_calibre_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_calibre_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_calibre"))
            {
                result = database_comunication::script_result::calibre_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_CALIBRE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_calibre_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_calibre"))
            {
                result = database_comunication::script_result::calibre_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_CALIBRE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_calibre"))
        {
            result = database_comunication::script_result::delete_calibre_failed;

        }
        else
        {
            if(ba.contains("success ok delete_calibre"))
            {
                result = database_comunication::script_result::delete_calibre_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Calibre::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty() && !this->isHidden()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaCalibres().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirCalibre(cod_m);
    emit update_tableCalibres(true);
    this->close();
}
void Calibre::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty() && !this->isHidden()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirCalibre(cod_m);
    emit update_tableCalibres(true);
    this->close();
}
void Calibre::on_pb_borrar_clicked()
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

        eliminarCalibre(cod_m);
//        subirTodasLasCalibres();
        emit update_tableCalibres(true);
        this->close();
    }


}
bool Calibre::eliminarCalibre(QString codigo){
//    subirTodasLasCalibres();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    calibre.insert(date_time_modified_calibres, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(calibre);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Calibre::script_excecution_result,q,&QEventLoop::exit);

    if(getListaCalibres().contains(codigo)){
        delete_calibre_request(keys, values);
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

    case database_comunication::script_result::delete_calibre_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_calibre_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Calibre::subirTodasLasCalibres()
{
    QMap<QString, QString> mapaTiposDeCalibre;
    mapaTiposDeCalibre.insert("001","TXORIERRI");
    mapaTiposDeCalibre.insert("002","DURANGUESADO");
    mapaTiposDeCalibre.insert("003","LEA-ARTIBAI");
    mapaTiposDeCalibre.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeCalibre.insert("005","ENCARTACIONES - CALIBRE MINERA");
    mapaTiposDeCalibre.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeCalibre.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeCalibre.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeCalibre.values().at(i);
        QString key = mapaTiposDeCalibre.keys().at(i);
        jsonObject.insert(codigo_calibre_calibres, key);
        jsonObject.insert(calibre_calibres, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirCalibre(cod);
    }
    emit update_tableCalibres(true);
    this->close();
}
