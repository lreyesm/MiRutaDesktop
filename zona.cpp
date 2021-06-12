#include "zona.h"
#include "ui_zona.h"
#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "mapa_zonas.h"
#include "globalfunctions.h"
#include <QCompleter>

Zona::Zona(QWidget *parent, bool newOne, QString empresa) :
    QWidget(parent),
    ui(new Ui::Zona)
{
    ui->setupUi(this);
    this->setWindowTitle("Sector P");
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
//    dias << "LUNES" << "MARTES" << "MIÉRCOLES" << "JUEVES"<< "VIERNES" << "SÁBADO"<< "DOMINGO";
    dias << "1" << "2" << "3" << "4"<< "5";
    QCompleter *completer = new QCompleter(dias, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_dia_predeterminado->setCompleter(completer);
}

Zona::~Zona()
{
    delete ui;
}

void Zona::populateView(QJsonObject o)
{
    QString zona_m, modelo_m, cod_m, geocode_m, dia_predeterminado;
    zona_m = o.value(zona_zonas).toString();
    cod_m = o.value(codigo_zona_zonas).toString();
    geocode_m = o.value(geolocalizacion_zonas).toString();
    dia_predeterminado = o.value(dia_predeterminado_zonas).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_zona->setText(zona_m);
    ui->le_geolocalizacion->setText(geocode_m);
    ui->le_dia_predeterminado->setText(dia_predeterminado);
}

QString Zona::guardarDatos(){
    QString zona_m, modelo_m, cod_m, geocode_m, dia_predeterminado;
    zona_m =  ui->le_zona->text();
    cod_m = ui->le_codigo->text();
    geocode_m = ui->le_geolocalizacion->text();
    dia_predeterminado = ui->le_dia_predeterminado->text();

    zona.insert(zona_zonas, zona_m);
    zona.insert(codigo_zona_zonas, cod_m);
    zona.insert(geolocalizacion_zonas, geocode_m);
    zona.insert(dia_predeterminado_zonas, dia_predeterminado);
    return cod_m;
}


void Zona::create_zona_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_ZONA,keys,values);
}
void Zona::update_zona_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_ZONA,keys,values);
}

void Zona::delete_zona_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_ZONA,keys,values);
}

bool Zona::writeZonas(QJsonArray jsonArray){
    QFile data_base(zonas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Zona::readZonas(){
    QJsonArray jsonArray;
    QFile data_base(zonas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QString Zona::getDayOfZona(QString zona_selected){
    QJsonArray jsonArray = readZonas();
    for (int i= 0; i < jsonArray.size(); i++) {
        QJsonObject jsonObject = jsonArray.at(i).toObject();
        if(zona_selected.contains(jsonObject.value(codigo_zona_zonas).toString()
                                  + " - " + jsonObject.value(zona_zonas).toString().trimmed())){
            return jsonObject.value(dia_predeterminado_zonas).toString();
        }
    }
    return "";
}

QStringList Zona::getListaZonas(){
    QJsonArray jsonArray = readZonas();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_zona_zonas).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Zona::subirZona(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    zona.insert(date_time_modified_zonas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(zona);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop q;

    connect(this, &Zona::script_excecution_result,&q,&QEventLoop::exit);

    if(getListaZonas().contains(codigo)){
        update_zona_request(keys, values);
    }
    else{
        create_zona_request(keys, values);
    }

    bool res = false;
    switch(q.exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::zona_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_zona_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_zona_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    return res;
}

void Zona::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_ZONA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_zona"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_zona_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_zona_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_zona"))
            {
                result = database_comunication::script_result::zona_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_ZONA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_zona_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_zona"))
            {
                result = database_comunication::script_result::zona_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_ZONA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_zona"))
        {
            result = database_comunication::script_result::delete_zona_failed;

        }
        else
        {
            if(ba.contains("success ok delete_zona"))
            {
                result = database_comunication::script_result::delete_zona_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Zona::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaZonas().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirZona(cod_m);
    emit update_tableZonas(true);
    this->close();
}
void Zona::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirZona(cod_m);
    emit update_tableZonas(true);
    this->close();
}
void Zona::on_pb_borrar_clicked()
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

        eliminarZona(cod_m);
//        subirTodasLasZonas();
        emit update_tableZonas(true);
        this->close();
    }


}
bool Zona::eliminarZona(QString codigo){
//    subirTodasLasZonas();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    zona.insert(date_time_modified_zonas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(zona);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" <<"empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop q;

    connect(this, &Zona::script_excecution_result,&q,&QEventLoop::exit);

    if(getListaZonas().contains(codigo)){
        delete_zona_request(keys, values);
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

    case database_comunication::script_result::delete_zona_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_zona_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    return res;
}
void Zona::subirTodasLasZonas()
{
    QMap<QString, QString> mapaTiposDeZona;
    mapaTiposDeZona.insert("001","TXORIERRI");
    mapaTiposDeZona.insert("002","DURANGUESADO");
    mapaTiposDeZona.insert("003","LEA-ARTIBAI");
    mapaTiposDeZona.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeZona.insert("005","ENCARTACIONES - ZONA MINERA");
    mapaTiposDeZona.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeZona.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeZona.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeZona.values().at(i);
        QString key = mapaTiposDeZona.keys().at(i);
        jsonObject.insert(codigo_zona_zonas, key);
        jsonObject.insert(zona_zonas, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirZona(cod);
    }
    emit update_tableZonas(true);
    this->close();
}
void Zona::setGeoCode(const QString geocode)
{
    ui->le_geolocalizacion->setText(geocode);
}
void Zona::eraseMarker()
{
    ui->le_geolocalizacion->setText("");
}
void Zona::on_pb_geolocalizacion_clicked()
{
    Mapa_Zonas *mapa = new Mapa_Zonas(nullptr, ui->le_geolocalizacion->text());
    mapa->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(mapa, &Mapa_Zonas::settedMarker, this, &Zona::setGeoCode);
    QObject::connect(mapa, &Mapa_Zonas::erase_marker, this, &Zona::eraseMarker);
    mapa->show();
}
