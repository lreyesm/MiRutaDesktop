#include "observacion.h"
#include "ui_observacion.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Observacion::Observacion(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Observacion)
{
    ui->setupUi(this);
    this->setWindowTitle("Observacion");
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

Observacion::~Observacion()
{
    delete ui;
}

void Observacion::populateView(QJsonObject o)
{
    QString observacion_m, origen_m, cod_m;
    observacion_m = o.value(observacion_observaciones).toString();
    origen_m = o.value(origen_observaciones).toString();
    cod_m = o.value(codigo_observacion_observaciones).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_observacion->setText(observacion_m);
    ui->le_origen->setText(origen_m);
    if(origen_m.isEmpty()){
        ui->le_origen->setText("RADIO");
    }
}

QString Observacion::guardarDatos(){
    QString observacion_m, origen_m, cod_m;
    observacion_m =  ui->le_observacion->text();
    origen_m = ui->le_origen->text();
    cod_m = ui->le_codigo->text();

    observacion.insert(origen_observaciones, origen_m);
    observacion.insert(observacion_observaciones, observacion_m);
    observacion.insert(codigo_observacion_observaciones, cod_m);
    return cod_m;
}


void Observacion::create_observacion_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_OBSERVACION,keys,values);
}
void Observacion::update_observacion_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_OBSERVACION,keys,values);
}

void Observacion::delete_observacion_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_OBSERVACION,keys,values);
}

bool Observacion::writeObservaciones(QJsonArray jsonArray){
    QFile *data_base = new QFile(observaciones_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Observacion::readObservaciones(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(observaciones_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QStringList Observacion::getListaObservaciones(){
    QJsonArray jsonArray = readObservaciones();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_observacion_observaciones).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Observacion::subirObservacion(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    observacion.insert(date_time_modified_observaciones, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(observacion);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Observacion::script_excecution_result,q,&QEventLoop::exit);

    if(getListaObservaciones().contains(codigo)){
        update_observacion_request(keys, values);
    }
    else{
        create_observacion_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::observacion_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_observacion_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_observacion_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Observacion::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_OBSERVACION)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_observacion"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_observacion_to_server_failed;
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_observacion_to_server_failed;
            }
        }
        else
        {
            if(ba.contains("success ok update_observacion"))
            {
                result = database_comunication::script_result::observacion_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_OBSERVACION)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_observacion_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_observacion"))
            {
                result = database_comunication::script_result::observacion_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_OBSERVACION)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_observacion"))
        {
            result = database_comunication::script_result::delete_observacion_failed;

        }
        else
        {
            if(ba.contains("success ok delete_observacion"))
            {
                result = database_comunication::script_result::delete_observacion_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Observacion::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaObservaciones().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirObservacion(cod_m);
    emit update_tableObservaciones(true);
    this->close();
}
void Observacion::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirObservacion(cod_m);
    emit update_tableObservaciones(true);
    this->close();
}
void Observacion::on_pb_borrar_clicked()
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

        eliminarObservacion(cod_m);
//        subirTodasLasObservaciones();
        emit update_tableObservaciones(true);
        this->close();
    }


}
bool Observacion::eliminarObservacion(QString codigo){
//    subirTodasLasObservaciones();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    observacion.insert(date_time_modified_observaciones, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(observacion);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Observacion::script_excecution_result,q,&QEventLoop::exit);

    if(getListaObservaciones().contains(codigo)){
        delete_observacion_request(keys, values);
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

    case database_comunication::script_result::delete_observacion_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_observacion_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Observacion::subirTodasLasObservaciones()
{
    QMap<QString, QString> mapaTiposDeObservaciones;
    mapaTiposDeObservaciones.insert("AC1", "INMUEBLE CERRADO SIN  LLAVES PARA ACCEDER");
    mapaTiposDeObservaciones.insert("AC2", "INMUEBLE CERRADO DEJADA NOTA DE AVISO");
    mapaTiposDeObservaciones.insert("AV1", "AUSENTE DEJADA NOTA DE AVISO");
    mapaTiposDeObservaciones.insert("GS1", "NO HAY AGUA NI INSTALACION");
    mapaTiposDeObservaciones.insert("GS2", "PRECINTADO POR AUTORIDAD");
    mapaTiposDeObservaciones.insert("GS3", "PRECINTADO POR CABB");
    mapaTiposDeObservaciones.insert("GS4", "TOMA ANULADA");
    mapaTiposDeObservaciones.insert("GS5", "TOMA TAPONADA");
    mapaTiposDeObservaciones.insert("GS6",	"TOMAN AGUA POR OTRO CONTADOR");
    mapaTiposDeObservaciones.insert("LL1",	"DESPUES DEL CONTADOR");
    mapaTiposDeObservaciones.insert("LL2",	"EMPOTRADA");
    mapaTiposDeObservaciones.insert("LL3",	"EN MAL ESTADO");
    mapaTiposDeObservaciones.insert("LL4",	"INACCESIBLE");
    mapaTiposDeObservaciones.insert("LL5",	"NO CIERRA");
    mapaTiposDeObservaciones.insert("LL6",	"NO EXISTE");
    mapaTiposDeObservaciones.insert("LL7",	"NO HAY LLAVE DE PASO GENERAL");
    mapaTiposDeObservaciones.insert("LL8",	"NO HAY LLAVES DE LA BATERIA");
    mapaTiposDeObservaciones.insert("NN1",	"ES CONDENSACION");
    mapaTiposDeObservaciones.insert("NN2",	"FILTROS SUCIOS");
    mapaTiposDeObservaciones.insert("NN3",	"LLAVE DE PASO CERRADA");
    mapaTiposDeObservaciones.insert("NN4",	"TUBERIA ROTA");
    mapaTiposDeObservaciones.insert("PC1",	"NO DEJAN ACCEDER AL INMUEBLE");
    mapaTiposDeObservaciones.insert("PC2",	"NO DEJAN CAMBIAR EL CONTADOR");
    mapaTiposDeObservaciones.insert("R01",	"ALBAÑILERIA. SEPARAR DE LA PARED");
    mapaTiposDeObservaciones.insert("R02",	"ALBANILERIA. TIENEN QUE PICAR");
    mapaTiposDeObservaciones.insert("R03",	"AMPLIAR ESPACIO PARA COLOCAR");
    mapaTiposDeObservaciones.insert("R04",	"ARQUETA CERRADA");
    mapaTiposDeObservaciones.insert("R05",	"BATERIA EN MAL ESTADO");
    mapaTiposDeObservaciones.insert("R06",	"BATERIA MAL MARCADA");
    mapaTiposDeObservaciones.insert("R07",	"BATERIA SIN MARCAR");
    mapaTiposDeObservaciones.insert("R08",	"COLOCAR LLAVE ANTIRRETORNO");
    mapaTiposDeObservaciones.insert("R09",	"CONTADOR EMPOTRADO");
    mapaTiposDeObservaciones.insert("R10",	"CONTADOR ENTRE REJAS");
    mapaTiposDeObservaciones.insert("R11",	"CONTADOR ROSCADO");
    mapaTiposDeObservaciones.insert("R12",	"DESCUBRIR");
    mapaTiposDeObservaciones.insert("R13",	"ELEVAR CONTADOR. MUY PROFUNDO");
    mapaTiposDeObservaciones.insert("R14",	"INEXISTENTE. PENDIENTE EJECUTAR");
    mapaTiposDeObservaciones.insert("R15",	"INSTALACION AL REVES");
    mapaTiposDeObservaciones.insert("R16",	"MAL ESTADO DE CONSERVACION ACTUAL");
    mapaTiposDeObservaciones.insert("R17",	"NO HAY PUNTO EN LA BATERIA");
    mapaTiposDeObservaciones.insert("R18",	"NO HAY TOMAS LIBRES");
    mapaTiposDeObservaciones.insert("R19",	"NO PREVISTA AYUNTAMIENTO");
    mapaTiposDeObservaciones.insert("R20",	"RETIRAR MUEBLES");
    mapaTiposDeObservaciones.insert("R21",	"TAMAÑO ARQUETA INSUFICIENTE");
    mapaTiposDeObservaciones.insert("R22",	"TAMAÑO NICHO INSUFICIENTE");
    mapaTiposDeObservaciones.insert("R23",	"TUBERIAS EN MAL ESTADO");
    mapaTiposDeObservaciones.insert("TF1",	"COMUNICA CONTINUAMENTE");
    mapaTiposDeObservaciones.insert("TF2",	"DEJADO AVISO EN CONTESTADOR");
    mapaTiposDeObservaciones.insert("TF3",	"LLAMARA EL CLIENTE");
    mapaTiposDeObservaciones.insert("TF4",	"NO CONTESTAN APAGADO RESTRINGIDO");
    mapaTiposDeObservaciones.insert("TF5",	"TELEFONO INCORRECTO");
    mapaTiposDeObservaciones.insert("UB1",	"CONTADOR INACCESIBLE");
    mapaTiposDeObservaciones.insert("UB2",	"NO SE LOCALIZA EL CONTADOR");
    mapaTiposDeObservaciones.insert("V01",	"VARIOS");
    mapaTiposDeObservaciones.insert("X01",	"EMISOR RECONFIGURADO, YA INST");
    mapaTiposDeObservaciones.insert("X02",	"EMISOR RECONFIGURADO Y REINSTALADO");
    mapaTiposDeObservaciones.insert("W01",	"CITA A CONCRETAR POR ABONADO");
    mapaTiposDeObservaciones.insert("W02",	"CITA CONCERTADA PARA FECHA PROXIMA");
    mapaTiposDeObservaciones.insert("W03",	"CONTACTADO. AUSENTE, PENDIENTE DE REGRESO");
    mapaTiposDeObservaciones.insert("W04",	"ADECUACION DE LA INSTALACION EN CURSO");
    mapaTiposDeObservaciones.insert("C01",	"CONTADOR FACILIDATO POR INTERESADO");
    mapaTiposDeObservaciones.insert("C02",	"EL CONTADOR RETIRADO FUGABA");
    mapaTiposDeObservaciones.insert("C03",	"CONTADOR LEVANTADO SIN TOTALIZADOR");
    mapaTiposDeObservaciones.insert("C04",	"CONTADOR LEVANTADO DESTRUIDO");
    mapaTiposDeObservaciones.insert("C05",	"CONTADOR LEVANTADO QUEMADO");
    mapaTiposDeObservaciones.insert("C06",	"CONTADOR LEVANTADO PARADO");
    mapaTiposDeObservaciones.insert("C07",	"RECTIFICADOS DATOS DEL CONTADOR INSTALADO");
    mapaTiposDeObservaciones.insert("C08",	"EL CONTADOR INSTALADO FUGA");
    mapaTiposDeObservaciones.insert("C09",	"ACTUALIZADOS DATOS DEL CONTADOR INSTALADO");
    mapaTiposDeObservaciones.insert("CEG",	"CAMBIADO EN GARANTÍA");
    mapaTiposDeObservaciones.insert("X03",	"EMISOR DESAPARECIDO Y NO SE RECIBE");
    mapaTiposDeObservaciones.insert("X04",	"EMISOR DESAPARECIDO Y SE RECIBE");
    mapaTiposDeObservaciones.insert("X05",	"EMISOR NO OPERATIVO");
    mapaTiposDeObservaciones.insert("X06",	"EMISOR DESTRUIDO");
    mapaTiposDeObservaciones.insert("X07",	"EMISOR FUNCIONAM. INCORRECTO");
    mapaTiposDeObservaciones.insert("X08",	"RECTIFICADOS DATOS DEL EMISOR INSTALADO");
    mapaTiposDeObservaciones.insert("XEG",	"CAMBIADA EN GARANTÍA");
    mapaTiposDeObservaciones.insert("G07",	"AGUA PROPIA");
    mapaTiposDeObservaciones.insert("P03",	"INSTALACION EN OBRAS");
    mapaTiposDeObservaciones.insert("F01",	"FUGA VISIBLE ANTES DEL CONTADOR");
    mapaTiposDeObservaciones.insert("F02",	"FUGA VISIBLE DESPUÉS DEL CONTADOR");
    mapaTiposDeObservaciones.insert("F03",	"FUGA NO VISIBLE. CONTADOR AVANZA");
    mapaTiposDeObservaciones.insert("G09",	"SOBRE PRESION");
    mapaTiposDeObservaciones.insert("G10",	"FALTA DE PRESION");
    mapaTiposDeObservaciones.insert("UB3",	"DIRECCIÓN DE SUMINISTRO RECTIFICADA");
    mapaTiposDeObservaciones.insert("NN5",	"CORTE GENERAL EN VIA PUBLICA");


    for(int i=0; i< mapaTiposDeObservaciones.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeObservaciones.values().at(i);
        QString key = mapaTiposDeObservaciones.keys().at(i);
        jsonObject.insert(codigo_observacion_observaciones, key);
        jsonObject.insert(observacion_observaciones, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirObservacion(cod);
    }
    emit update_tableObservaciones(true);
    this->close();
}
