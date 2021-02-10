#include "marca.h"
#include "ui_marca.h"
#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Marca::Marca(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Marca)
{
    ui->setupUi(this);
    this->setWindowTitle("Marca");
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

Marca::~Marca()
{
    delete ui;
}

void Marca::populateView(QJsonObject o)
{
    QString marca_m, modelo_m, cod_m;
    marca_m = o.value(marca_marcas).toString();
    modelo_m = o.value(modelo_marcas).toString();
    cod_m = o.value(codigo_marca_marcas).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_modelo->setText(modelo_m);
    ui->le_marca->setText(marca_m);
}

QString Marca::guardarDatos(){
    QString marca_m, modelo_m, cod_m;
    marca_m =  ui->le_marca->text();
    modelo_m = ui->le_modelo->text();
    cod_m = ui->le_codigo->text();

    marca.insert(marca_marcas, marca_m);
    marca.insert(modelo_marcas, modelo_m);
    marca.insert(codigo_marca_marcas, cod_m);
    return cod_m;
}


void Marca::create_marca_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_MARCA,keys,values);
}
void Marca::update_marca_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_MARCA,keys,values);
}

void Marca::delete_marca_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_MARCA,keys,values);
}

bool Marca::writeMarcas(QJsonArray jsonArray){
    QFile *data_base = new QFile(marcas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Marca::readMarcas(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(marcas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QStringList Marca::getListaMarcas(){
    QJsonArray jsonArray = readMarcas();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_marca_marcas).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Marca::subirMarca(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    marca.insert(date_time_modified_marcas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(marca);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Marca::script_excecution_result,q,&QEventLoop::exit);

    if(getListaMarcas().contains(codigo)){
        update_marca_request(keys, values);
    }
    else{
        create_marca_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::marca_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_marca_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_marca_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Marca::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_MARCA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_marca"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_marca_to_server_failed;
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_marca_to_server_failed;
            }
        }
        else
        {
            if(ba.contains("success ok update_marca"))
            {
                result = database_comunication::script_result::marca_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_MARCA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_marca_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_marca"))
            {
                result = database_comunication::script_result::marca_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_MARCA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_marca"))
        {
            result = database_comunication::script_result::delete_marca_failed;

        }
        else
        {
            if(ba.contains("success ok delete_marca"))
            {
                result = database_comunication::script_result::delete_marca_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Marca::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaMarcas().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirMarca(cod_m);
    emit update_tableMarcas(true);
    this->close();
}
void Marca::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirMarca(cod_m);
    emit update_tableMarcas(true);
    this->close();
}
void Marca::on_pb_borrar_clicked()
{
    //    subirTodasLasMarcas();
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    GlobalFunctions gf(this);
    if(gf.showQuestion(this, "Confirmacion", "Seguro desea eliminar el modelo?",
                             QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){

        eliminarMarca(cod_m);
        emit update_tableMarcas(true);
        this->close();
    }
}
bool Marca::eliminarMarca(QString codigo){
//    subirTodasLasMarcas();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    marca.insert(date_time_modified_marcas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(marca);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Marca::script_excecution_result,q,&QEventLoop::exit);

    if(getListaMarcas().contains(codigo)){
        delete_marca_request(keys, values);
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

    case database_comunication::script_result::delete_marca_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_marca_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Marca::subirTodasLasMarcas()
{
    QMap<QString, QString> mapaTiposDeMarca;
    mapaTiposDeMarca.insert("001","ELSTER - HELIX 4000");
    mapaTiposDeMarca.insert("002","ELSTER - DELAUNET C");
    mapaTiposDeMarca.insert("003","ELSTER - ZENIT S200");
    mapaTiposDeMarca.insert("004","ITRON - FLOSTAR M"); //preguntar por este
    mapaTiposDeMarca.insert("005","ITRON - MSD CYBLE");
    mapaTiposDeMarca.insert("006","ELSTER - COSMOS WPW");
    mapaTiposDeMarca.insert("007","SAPPEL - AQUILA V3 V4" );
    mapaTiposDeMarca.insert("008","SAPPEL - ALTAIR V3 V4" );
    mapaTiposDeMarca.insert("009","ITRON - MSD NO CYBLE" );
    mapaTiposDeMarca.insert("010","ELSTER - S130" ); //preguntar por este
    mapaTiposDeMarca.insert("011","ITRON - TR");
    mapaTiposDeMarca.insert("012","ITRON - MEDIS CYBLE" );
    mapaTiposDeMarca.insert("013","ITRON - WOLTMANN");
    mapaTiposDeMarca.insert("014","ITRON - WOLTEX" );
    mapaTiposDeMarca.insert("015","ITRON - CTWV");
    mapaTiposDeMarca.insert("016","ITRON - COMBINADO" ); //preguntar por este
    mapaTiposDeMarca.insert("017","ELSTER - DELAUNET EXTRA" );
    mapaTiposDeMarca.insert("018","ELSTER - S200 NO REED" );
    mapaTiposDeMarca.insert("019","ELSTER - S100" );
    mapaTiposDeMarca.insert("020","ELSTER - S150");
    mapaTiposDeMarca.insert("021","ELSTER - WOLTMANN NO REED" );
    mapaTiposDeMarca.insert("022","ITRON - MEDIS UNIMAG" );
    mapaTiposDeMarca.insert("023","ITRON - TA.3F" );
    mapaTiposDeMarca.insert("024","SENSUS - CONTAGUA-DN_13_1" ); //preguntar por este
    mapaTiposDeMarca.insert("025","ITRON - FLODIS" );
    mapaTiposDeMarca.insert("026","EBRO - EBRO" );
    mapaTiposDeMarca.insert("027","ITRON - NARVAL");
    mapaTiposDeMarca.insert("028","BRUNATA - BRUNATA" );
    mapaTiposDeMarca.insert("029","WEHRLE - WEHRLE");
    mapaTiposDeMarca.insert("030","JANZ - CONTHIDRA" ); //preguntar por este
    mapaTiposDeMarca.insert("031","SISMA - SISMA" );
    mapaTiposDeMarca.insert("032","LORENZ - LORENZ" );
    mapaTiposDeMarca.insert("033","?" );
    mapaTiposDeMarca.insert("034","SENSUS - MEISTREAM" );
    mapaTiposDeMarca.insert("035","SIEMENS - MAG 8000");
    mapaTiposDeMarca.insert("036","SAPPEL - AQUARIUS" ); //preguntar por este
    mapaTiposDeMarca.insert("037","ITRON - IRRIMAG" );
    mapaTiposDeMarca.insert("038","ISTA - DOMAQUA");
    mapaTiposDeMarca.insert("039","ELSTER - S220 STANDARD");
    mapaTiposDeMarca.insert("040","ELSTER - Y250RI" );
    mapaTiposDeMarca.insert("041","ELSTER - S220_100");
    mapaTiposDeMarca.insert("042","ITRON - AQUADIS PLUS");
    mapaTiposDeMarca.insert("043","ELSTER -  WAI R1000-R12");
    mapaTiposDeMarca.insert("044","SENSUS - 620C" ); //preguntar por este
    mapaTiposDeMarca.insert("045","POWOGAZ - JS CLASE C");
    mapaTiposDeMarca.insert("046","POWOGAZ - WI INCENDIOS" );
    mapaTiposDeMarca.insert("047","ARAD - OCTAVE" );
    mapaTiposDeMarca.insert("048","ELSTER - H5000" );
    mapaTiposDeMarca.insert("049","ELSTER - Y290" );
    mapaTiposDeMarca.insert("050","ELSTER - V200" ); //preguntar por este
    mapaTiposDeMarca.insert("051","ELSTER - Y250MRI" );
    mapaTiposDeMarca.insert("052","SAPPEL - WP-MFD" );
    mapaTiposDeMarca.insert("053","ITRON - INTELLIS" );
    mapaTiposDeMarca.insert("054","SAPPEL - HYDRUS" );
    mapaTiposDeMarca.insert("055","ELSTER - CSJ_S2000" );
    mapaTiposDeMarca.insert("056","ELSTER - R 1200" ); //preguntar por este
    mapaTiposDeMarca.insert("057","ELSTER - Y290M" );
    mapaTiposDeMarca.insert("058","ELSTER - M120" );
    mapaTiposDeMarca.insert("059","SAPPEL - WP G_ESFERA ESTANDAR");
    mapaTiposDeMarca.insert("060","GENERICO - DN:13-15-20 MM");
    mapaTiposDeMarca.insert("061","GENERICO - DN:25-30-40 MM");
    mapaTiposDeMarca.insert("062","GENERICO - DN:50-65-80-100");
    mapaTiposDeMarca.insert("063","GENERICO - DN:125-150-200 M");
    mapaTiposDeMarca.insert("070","SV_RTK - CRONOS"); //preguntar por este
    mapaTiposDeMarca.insert("071","DIEHL - ALTAIR V3 V4");

    for(int i=0; i< mapaTiposDeMarca.size();i++){
        QJsonObject jsonObject;
        jsonObject.insert(codigo_marca_marcas,mapaTiposDeMarca.keys().at(i));
        QString value = mapaTiposDeMarca.values().at(i);
        if(value.contains(" - ")){
            QString marc, mod;
            marc = value.split(" - ").at(0);
            mod = value.split(" - ").at(1);
            jsonObject.insert(marca_marcas, marc);
            jsonObject.insert(modelo_marcas, mod);
        }
        else {
            jsonObject.insert(marca_marcas, value);
            jsonObject.insert(modelo_marcas, value);
        }
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirMarca(cod);
    }
    emit update_tableMarcas(true);
    this->close();
}
