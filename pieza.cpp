#include "pieza.h"
#include "ui_pieza.h"
#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Pieza::Pieza(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Pieza)
{
    ui->setupUi(this);
    this->setWindowTitle("Pieza");
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

Pieza::~Pieza()
{
    delete ui;
}

void Pieza::populateView(QJsonObject o)
{
    QString pieza_m, fecha_uso_m, estado_m, cod_m;
    pieza_m = o.value(pieza_piezas).toString();
    cod_m = o.value(codigo_pieza_piezas).toString();
    fecha_uso_m =  o.value(fecha_utilizacion).toString();
    estado_m =  o.value(state_pieza).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_pieza->setText(pieza_m);
}

QString Pieza::guardarDatos(){
    QString pieza_m, fecha_uso_m, estado_m, cod_m;
    pieza_m =  ui->le_pieza->text();
    cod_m = ui->le_codigo->text();

    pieza.insert(pieza_piezas, pieza_m);
    pieza.insert(codigo_pieza_piezas, cod_m);
    return cod_m;
}


void Pieza::create_pieza_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_PIEZA,keys,values);
}
void Pieza::update_pieza_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_PIEZA,keys,values);
}

void Pieza::delete_pieza_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_PIEZA,keys,values);
}

bool Pieza::writePiezas(QJsonArray jsonArray){
    QFile *data_base = new QFile(piezas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Pieza::readPiezas(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(piezas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QStringList Pieza::getListaPiezas(){
    QJsonArray jsonArray = readPiezas();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_pieza_piezas).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Pieza::subirPieza(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    pieza.insert(date_time_modified_piezas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(pieza);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Pieza::script_excecution_result,q,&QEventLoop::exit);

    if(getListaPiezas().contains(codigo)){
        update_pieza_request(keys, values);
    }
    else{
        create_pieza_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::pieza_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_pieza_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_pieza_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}

void Pieza::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_PIEZA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_pieza"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_pieza_to_server_failed;
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_pieza_to_server_failed;
            }
        }
        else
        {
            if(ba.contains("success ok update_pieza"))
            {
                result = database_comunication::script_result::pieza_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_PIEZA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_pieza_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_pieza"))
            {
                result = database_comunication::script_result::pieza_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_PIEZA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_pieza"))
        {
            result = database_comunication::script_result::delete_pieza_failed;

        }
        else
        {
            if(ba.contains("success ok delete_pieza"))
            {
                result = database_comunication::script_result::delete_pieza_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Pieza::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaPiezas().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirPieza(cod_m);
    emit update_tablePiezas(true);
    this->close();
}
void Pieza::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirPieza(cod_m);
    emit update_tablePiezas(true);
    this->close();
}
void Pieza::on_pb_borrar_clicked()
{
//    subirTodasLasPiezas();
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    GlobalFunctions gf(this);
    if(gf.showQuestion(this, "Confirmacion", "Seguro desea eliminar el modelo?",
                             QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){

        eliminarPieza(cod_m);
        emit update_tablePiezas(true);
        this->close();
    }
}
bool Pieza::eliminarPieza(QString codigo){
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    pieza.insert(date_time_modified_piezas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(pieza);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Pieza::script_excecution_result,q,&QEventLoop::exit);

    if(getListaPiezas().contains(codigo)){
        delete_pieza_request(keys, values);
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

    case database_comunication::script_result::delete_pieza_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_pieza_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Pieza::subirTodasLasPiezas()
{

    QMap<QString, QString> mapaTiposDePieza;
    QStringList listaTiposDePieza;

    listaTiposDePieza.append("ALARGADERA 1\" CORTA");
    listaTiposDePieza.append("ALARGADERA 1/2 CORTA");
    listaTiposDePieza.append("ALARGADERA 1/2 LARGA");
    listaTiposDePieza.append("ALARGADERA 115-190 DN15");
    listaTiposDePieza.append("ALARGADERA 115-190 DN20");
    listaTiposDePieza.append("ALARGADERA 3/4");
    listaTiposDePieza.append("BRIDA CIEGA 100");
    listaTiposDePieza.append("BRIDA CIEGA 50");
    listaTiposDePieza.append("BRIDA CIEGA 65");
    listaTiposDePieza.append("BRIDA CIEGA 80");
    listaTiposDePieza.append("CODO 90º IGUAL LATON DN-20");
    listaTiposDePieza.append("CODO 90º IGUAL LATON DN-25");
    listaTiposDePieza.append("CODO 90º IGUAL LATON DN-32");
    listaTiposDePieza.append("CODO 90º RH LATON DN-20");
    listaTiposDePieza.append("CODO 90º RH LATON DN-25");
    listaTiposDePieza.append("CODO 90º RH LATON DN-32");
    listaTiposDePieza.append("CODO 90º RH LATON DN-40");
    listaTiposDePieza.append("CODO 90º RM LATON DN-20");
    listaTiposDePieza.append("CODO 90º RM LATON DN-25");
    listaTiposDePieza.append("CODO 90º RM LATON DN-32");
    listaTiposDePieza.append("CODO 90º RM LATON DN-40");
    listaTiposDePieza.append("CODO COBRE HH DN-15 90º");
    listaTiposDePieza.append("CODO COBRE HH DN-18 90º");
    listaTiposDePieza.append("CODO PIPA 15-1/2");
    listaTiposDePieza.append("CODOS 1/2 H-H");
    listaTiposDePieza.append("CODOS 1/2 M/H");
    listaTiposDePieza.append("CODOS 3/4 H-H");
    listaTiposDePieza.append("CODOS 3/4 M-H");
    listaTiposDePieza.append("ENLACE POLIETILENO ½” RM");
    listaTiposDePieza.append("ENLACE POLIETILENO ¾” RM");
    listaTiposDePieza.append("ENLACE POLIETILENO 1” RM");
    listaTiposDePieza.append("ENLACE POLIETILENO 1”1/2 RM");
    listaTiposDePieza.append("ENLACE POLIETILENO 1”1/4 RM");
    listaTiposDePieza.append("ENLACE POLIETILENO 2” RM");
    listaTiposDePieza.append("ENLACE RH LATON DN-20");
    listaTiposDePieza.append("ENLACE RH LATON DN-25");
    listaTiposDePieza.append("ENLACE RH LATON DN-32");
    listaTiposDePieza.append("ENLACE RH LATON DN-40");
    listaTiposDePieza.append("ENLACE RH LATON DN63 2\"");
    listaTiposDePieza.append("ENLACE RM LATON DN-20 (1/2)");
    listaTiposDePieza.append("ENLACE RM LATON DN-25 (3/4\")");
    listaTiposDePieza.append("ENLACE RM LATON DN-32 (1\")");
    listaTiposDePieza.append("ENLACE RM LATON DN-40 (1\"1/4)");
    listaTiposDePieza.append("ENLACE RM LATON DN-50 (1\"1/2)");
    listaTiposDePieza.append("ENLACE RM LATON DN63 2”");
    listaTiposDePieza.append("GEBOS 1\" (TUBO-TUBO) - (TUBO-ROSCA)");
    listaTiposDePieza.append("GEBOS 1\"1/2 (TUBO-TUBO)");
    listaTiposDePieza.append("GEBOS 1\"1/4 (TUBO-TUBO)");
    listaTiposDePieza.append("GEBOS 1/2 (TUBO-TUBO) (TUBO-ROSCA)");
    listaTiposDePieza.append("GEBOS 3/4 (TUBO-TUBO) (TUBO-ROSCA)");
    listaTiposDePieza.append("JUNTA GOMA PLANA DN150");
    listaTiposDePieza.append("JUNTA PLANA EPDM DN100");
    listaTiposDePieza.append("JUNTA PLANA EPDM DN50");
    listaTiposDePieza.append("JUNTA PLANA EPDM DN65");
    listaTiposDePieza.append("JUNTA PLANA EPDM DN80");
    listaTiposDePieza.append("LATIGUILLO 1/2 H-H 10  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 H-H 15  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 H-H 20  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 H-H 25  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 H-H 30  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 H-H 35  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 H-H 40  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 H-H 50  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 M-H 10  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 M-H 15  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 M-H 20  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 M-H 25  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 M-H 30  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 M-H 35CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 M-H 40  CM");
    listaTiposDePieza.append("LATIGUILLO 1/2 M-H 50  CM");
    listaTiposDePieza.append("LATIGUILLO 3/4 H-H 40  CM");
    listaTiposDePieza.append("LATIGUILLO 3/4 H-H 50  CM");
    listaTiposDePieza.append("LATIGUILLO 3/4 M-H 40  CM");
    listaTiposDePieza.append("LATIGUILLO 3/4 M-H 50CM");
    listaTiposDePieza.append("LLAVEO DE PASO DE BATERIA DE ESCUADRA CON PLACA");
    listaTiposDePieza.append("LLAVEO DE PASO DE BATERIA DE ESCUADRA SIN PLACA");
    listaTiposDePieza.append("MACHON 1/2");
    listaTiposDePieza.append("MACHON 3/4");
    listaTiposDePieza.append("MANGUITO 1\"");
    listaTiposDePieza.append("MANGUITO 1/2");
    listaTiposDePieza.append("MANGUITO 3/4");
    listaTiposDePieza.append("MANGUITO COBRE DN-15");
    listaTiposDePieza.append("MANGUITO COBRE DN-18");
    listaTiposDePieza.append("MANGUITO COBRE DN-28");
    listaTiposDePieza.append("MANGUITO COBRE DN-42");
    listaTiposDePieza.append("MANGUITO HEMBRA 15X1/2 (SAMBRA)");
    listaTiposDePieza.append("MANGUITO HEMBRA 18X1/2 (SAMBRA)");
    listaTiposDePieza.append("MANGUITO LATON DN-20");
    listaTiposDePieza.append("MANGUITO LATON DN-25");
    listaTiposDePieza.append("MANGUITO LATON DN-32");
    listaTiposDePieza.append("MANGUITO MACHO 18X1/2");
    listaTiposDePieza.append("ML TUBERIA  COBRE DN-12*1");
    listaTiposDePieza.append("ML TUBERIA  COBRE DN-15*1");
    listaTiposDePieza.append("ML TUBERIA  COBRE DN-18*1");
    listaTiposDePieza.append("RACOR 1\"");
    listaTiposDePieza.append("RACOR 1\" 1/4\"");
    listaTiposDePieza.append("RACOR 1\"1/2\"");
    listaTiposDePieza.append("RACOR 2\"");
    listaTiposDePieza.append("RACOR 3/4");
    listaTiposDePieza.append("RACOR 7/8");
    listaTiposDePieza.append("RACOR 7/8 CORTO");
    listaTiposDePieza.append("RACOR CORTO 3/4");
    listaTiposDePieza.append("RACOR SOLDAR 15-3/4");
    listaTiposDePieza.append("RACOR SOLDAR 18-3/4");
    listaTiposDePieza.append("REDUCCION 1 1/2-1 1/4 MH");
    listaTiposDePieza.append("REDUCCION 1 1/4 - 1 MH");
    listaTiposDePieza.append("REDUCCION 1\"1/4M-3/4H");
    listaTiposDePieza.append("REDUCCION 1\"M-1/2H");
    listaTiposDePieza.append("REDUCCION 1\"M-3/4H");
    listaTiposDePieza.append("REDUCCION 1/2M-3/8H");
    listaTiposDePieza.append("REDUCCION 13-100");
    listaTiposDePieza.append("REDUCCION 2\"-1 1/2\"MH");
    listaTiposDePieza.append("REDUCCION 3/4H-1/2M");
    listaTiposDePieza.append("REDUCCION 3/4M-1/2H");
    listaTiposDePieza.append("REDUCCION 3/4M-1/2H LARGA");
    listaTiposDePieza.append("REDUCCION 3/4M-1/2M");
    listaTiposDePieza.append("REDUCCION 3/8M-1/2H");
    listaTiposDePieza.append("REDUCCION 7/8-3/4 LISA");
    listaTiposDePieza.append("REDUCCION COBRE 18-15");
    listaTiposDePieza.append("TAPON 1\"");
    listaTiposDePieza.append("TAPON 1\"1/2");
    listaTiposDePieza.append("TAPON 1\"1/4");
    listaTiposDePieza.append("TAPON 1/2\"");
    listaTiposDePieza.append("TAPON 2\"");
    listaTiposDePieza.append("TAPON 3/4");
    listaTiposDePieza.append("TAPON 7/8");
    listaTiposDePieza.append("TE COBRE DN-15");
    listaTiposDePieza.append("TE COBRE DN-18");
    listaTiposDePieza.append("TE LATON 1/2");
    listaTiposDePieza.append("TE LATON 3/4");
    listaTiposDePieza.append("TERMINAL BRONCE SOLDAR RH DN-12* 1/2\"");
    listaTiposDePieza.append("TERMINAL BRONCE SOLDAR RH DN-15* 1/2\"");
    listaTiposDePieza.append("TERMINAL BRONCE SOLDAR RH DN-15* 3/4\"");
    listaTiposDePieza.append("TERMINAL BRONCE SOLDAR RH DN-18* 1/2\"");
    listaTiposDePieza.append("TERMINAL BRONCE SOLDAR RH DN-18* 3/4\"");
    listaTiposDePieza.append("TORNILLO 80MM PARA TUERCA METRICA 24");
    listaTiposDePieza.append("TORNILLO EXAGONAL 933 6.8 16X60");
    listaTiposDePieza.append("TUERCA DE BATERIA IZQUIERDA-DERECHA 1\"-1\"");
    listaTiposDePieza.append("TUERCA DE BATERIA IZQUIERDA-DERECHA 3/4-3/4");
    listaTiposDePieza.append("TUERCA DE BATERIA IZQUIERDA-DERECHA 3/4-7/8");
    listaTiposDePieza.append("TUERCA DE BATERIA IZQUIERDA-DERECHA 5/8-3/4");
    listaTiposDePieza.append("TUERCA DE BATERIA IZQUIERDA-DERECHA 5/8-7/8");
    listaTiposDePieza.append("TUERCA EXAGONAL 6.8 M - 16");
    listaTiposDePieza.append("VALVULA BOLA LATON PALANCA 1 1/2\"");
    listaTiposDePieza.append("VALVULA BOLA LATON PALANCA 1 1/4\"");
    listaTiposDePieza.append("VALVULA BOLA LATON PALANCA 1\"");
    listaTiposDePieza.append("VALVULA BOLA LATON PALANCA 1/2\"");
    listaTiposDePieza.append("VALVULA BOLA LATON PALANCA 2\"");
    listaTiposDePieza.append("VALVULA BOLA LATON PALANCA 3/4\"");


    for(int i=0; i< listaTiposDePieza.size();i++){
        QJsonObject jsonObject;
        QString codigo = QString::number(i+1);
        while(codigo.size() < 3){
            codigo.prepend("0");
        }
        jsonObject.insert(codigo_pieza_piezas, codigo);
        jsonObject.insert(pieza_piezas,listaTiposDePieza.at(i));
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirPieza(cod);
    }
    emit update_tablePiezas(true);
    this->close();
}
