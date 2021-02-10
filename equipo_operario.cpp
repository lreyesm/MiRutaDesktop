#include "equipo_operario.h"
#include "ui_equipo_operario.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "screen_tabla_tareas.h"
#include "new_table_structure.h"
#include "operator_selection_screen.h"

#include "globalfunctions.h"

Equipo_Operario::Equipo_Operario(QWidget *parent, bool newOne, QString empresa) :
    QWidget(parent),
    ui(new Ui::Equipo_Operario)
{
    ui->setupUi(this);
    this->setWindowTitle("Equipo de Fontaneros");
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

Equipo_Operario::~Equipo_Operario()
{
    delete ui;
}

void Equipo_Operario::populateView(QJsonObject o)
{
    QString equipo_operario_m, telefono, cod_m, nombre_encargado,
            cantidad_tareas, operarios, descripcion;
    equipo_operario_m = o.value(equipo_operario_equipo_operarios).toString();
    cod_m = o.value(codigo_equipo_operario_equipo_operarios).toString();
    telefono = o.value(telefono_equipo_operarios).toString();
    nombre_encargado = o.value(nombre_encargado_equipo_operarios).toString();
    cantidad_tareas = o.value(cantidad_tareas_equipo_operarios).toString();
    operarios = o.value(operarios_equipo_operarios).toString();
    QStringList fontaneros = operarios.split("\n");
    fontaneros.removeAll("");
    descripcion = o.value(descripcion_equipo_operarios).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_equipo_operario->setText(equipo_operario_m);
    ui->le_telefono->setText(telefono);
    ui->le_nombre_encargado->setText(nombre_encargado);
    ui->le_cantidad_tareas->setText(cantidad_tareas);
    ui->l_operarios->addItems(fontaneros);
    ui->pt_descripcion->setPlainText(descripcion);
}

QString Equipo_Operario::guardarDatos(){
    QString equipo_operario_m, telefono, cod_m, nombre_encargado,
            cantidad_tareas, operarios = "", descripcion;
    equipo_operario_m =  ui->le_equipo_operario->text();
    cod_m = ui->le_codigo->text();
    telefono =  ui->le_telefono->text();
    nombre_encargado = ui->le_nombre_encargado->text();
    cantidad_tareas =  ui->le_cantidad_tareas->text();
    if(!ui->l_operarios->getSpinnerList().isEmpty()){
        operarios = ui->l_operarios->getSpinnerList().join("\n");
    }
    descripcion = ui->pt_descripcion->toPlainText();

    if(equipo_operario_m != equipo_operario.value(equipo_operario_equipo_operarios).toString()){
        update_tareas = true;
        old_equipo = equipo_operario.value(equipo_operario_equipo_operarios).toString();
    }
    equipo_operario.insert(equipo_operario_equipo_operarios, equipo_operario_m);
    equipo_operario.insert(codigo_equipo_operario_equipo_operarios, cod_m);
    equipo_operario.insert(telefono_equipo_operarios, telefono);
    equipo_operario.insert(nombre_encargado_equipo_operarios, nombre_encargado);
    equipo_operario.insert(cantidad_tareas_equipo_operarios, cantidad_tareas);
    equipo_operario.insert(operarios_equipo_operarios, operarios);
    equipo_operario.insert(descripcion_equipo_operarios, descripcion);
    return cod_m;
}


void Equipo_Operario::create_equipo_operario_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_EQUIPO_OPERARIO,keys,values);
}
void Equipo_Operario::update_equipo_operario_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_EQUIPO_OPERARIO,keys,values);
}

void Equipo_Operario::delete_equipo_operario_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_EQUIPO_OPERARIO,keys,values);
}

bool Equipo_Operario::writeEquipo_Operarios(QJsonArray jsonArray){
    QFile *data_base = new QFile(equipo_operarios_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Equipo_Operario::readEquipo_Operarios(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(equipo_operarios_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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
QStringList Equipo_Operario::getListaNombresEquipo_Operarios(){
    QJsonArray jsonArray = readEquipo_Operarios();
    QStringList lista;
    QString team;
    for (int i =0; i < jsonArray.size(); i++) {
        team = jsonArray.at(i).toObject().value(equipo_operario_equipo_operarios).toString();
        if(!lista.contains(team)){
            lista.append(team);
        }
    }
    return  lista;
}
QStringList Equipo_Operario::getListaEquipo_Operarios(){
    QJsonArray jsonArray = readEquipo_Operarios();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_equipo_operario_equipo_operarios).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Equipo_Operario::subirEquipo_Operario(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    equipo_operario.insert(date_time_modified_equipo_operarios, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(equipo_operario);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop *q = new QEventLoop();

    connect(this, &Equipo_Operario::script_excecution_result,q,&QEventLoop::exit);

    if(getListaEquipo_Operarios().contains(codigo)){
        if(update_tareas){
            update_tareas = false;
            QStringList numInternos;
            QJsonObject campos;

            GlobalFunctions gf(this, empresa);
            numInternos = gf.getTareasList(equipo, old_equipo);
            campos.insert(equipo, equipo_operario.value(
                              equipo_operario_equipo_operarios).toString());
            campos.insert(date_time_modified, QDateTime::currentDateTime().toString(formato_fecha_hora));
            update_fields(numInternos, campos);
            emit updateTareas();
        }
        update_equipo_operario_request(keys, values);
    }
    else{
        if(getListaNombresEquipo_Operarios().contains(
                    equipo_operario.value(equipo_operario_equipo_operarios).toString())){
            GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este equipo, introduzca otro");
            return false;
        }else{
            create_equipo_operario_request(keys, values);
        }
    }

    bool res = true;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::equipo_operario_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_equipo_operario_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_equipo_operario_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Equipo_Operario::update_tareas_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA_FIELDS,keys,values);
}
bool Equipo_Operario::update_fields(QStringList numeros_internos_list, QJsonObject campos){
    QJsonObject numeros_internos;
    for (int i=0; i < numeros_internos_list.size(); i++) {
        numeros_internos.insert(QString::number(i),
                                numeros_internos_list.at(i));
    }
    if(numeros_internos.isEmpty() || campos.isEmpty()){
        return false;
    }
    QStringList keys, values;
    QJsonDocument d;
    d.setObject(numeros_internos);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp_fields, temp_numins = QString::fromUtf8(ba);

    d.setObject(campos);
    ba = d.toJson(QJsonDocument::Compact);
    temp_fields = QString::fromUtf8(ba);

    keys << "json_numin" << "json_fields" << "empresa";
    values << temp_numins << temp_fields << empresa.toLower();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Equipo_Operario::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Equipo_Operario::update_tareas_fields_request);

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

    case database_comunication::script_result::update_tareas_fields_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}

void Equipo_Operario::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_EQUIPO_OPERARIO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_equipo_operario"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_equipo_operario_to_server_failed;
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_equipo_operario_to_server_failed;
            }
        }
        else
        {
            if(ba.contains("success ok update_equipo_operario"))
            {
                result = database_comunication::script_result::equipo_operario_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_TAREA_FIELDS)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success update_tarea_fields"))
        {
            result = database_comunication::script_result::update_tareas_fields_to_server_failed;
        }
        else
        {
            if(ba.contains("success ok update_tarea_fields"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_EQUIPO_OPERARIO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_equipo_operario_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_equipo_operario"))
            {
                result = database_comunication::script_result::equipo_operario_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_EQUIPO_OPERARIO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_equipo_operario"))
        {
            result = database_comunication::script_result::delete_equipo_operario_failed;

        }
        else
        {
            if(ba.contains("success ok delete_equipo_operario"))
            {
                result = database_comunication::script_result::delete_equipo_operario_ok;
            }
        }
    }

    emit script_excecution_result(result);
}

void Equipo_Operario::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaEquipo_Operarios().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    if(subirEquipo_Operario(cod_m)){
        emit update_tableEquipo_Operarios(true);
        this->close();
    }
}
void Equipo_Operario::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(subirEquipo_Operario(cod_m)){
        emit update_tableEquipo_Operarios(true);
        this->close();
    }
}
void Equipo_Operario::on_pb_borrar_clicked()
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

        eliminarEquipo_Operario(cod_m);
        emit update_tableEquipo_Operarios(true);
        this->close();
    }


}
bool Equipo_Operario::eliminarEquipo_Operario(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    equipo_operario.insert(date_time_modified_equipo_operarios, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(equipo_operario);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop *q = new QEventLoop();

    connect(this, &Equipo_Operario::script_excecution_result,q,&QEventLoop::exit);

    if(getListaEquipo_Operarios().contains(codigo)){
        delete_equipo_operario_request(keys, values);
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

    case database_comunication::script_result::delete_equipo_operario_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_equipo_operario_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Equipo_Operario::subirTodasLasEquipo_Operarios()
{
    QMap<QString, QString> mapaTiposDeEquipo_Operario;
    mapaTiposDeEquipo_Operario.insert("B", "Fijo en Boca de Riego");

    for(int i=0; i< mapaTiposDeEquipo_Operario.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeEquipo_Operario.values().at(i);
        QString key = mapaTiposDeEquipo_Operario.keys().at(i);
        jsonObject.insert(codigo_equipo_operario_equipo_operarios, key);
        jsonObject.insert(equipo_operario_equipo_operarios, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirEquipo_Operario(cod);
    }
    emit update_tableEquipo_Operarios(true);
    this->close();
}
void Equipo_Operario::checkOperarioTeam(QString user){
    QJsonArray jsonArray = readEquipo_Operarios();
    for (int i =0; i< jsonArray.size(); i++) {
        QString team = jsonArray.at(i).toObject().
                value(equipo_operario_equipo_operarios).toString();
        QString usuarios = jsonArray.at(i).toObject().
                value(operarios_equipo_operarios).toString();
        if(equipo_operario.value(equipo_operario_equipo_operarios).
                toString() != team && usuarios.contains(user)){
            GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo añadiendo","Este fontanero ya pertenece al "
                                 + team);
            return;
        }
    }
    ui->l_operarios->addItem(user);
}
void Equipo_Operario::on_l_add_operario_clicked()
{
    Operator_Selection_Screen *seleccionOperarioScreen =
            new Operator_Selection_Screen(this, empresa);
    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,&Operator_Selection_Screen::user, this, &Equipo_Operario::checkOperarioTeam);
    seleccionOperarioScreen->move(this->width()/2 -seleccionOperarioScreen->width()/2,
                                  -100);
    seleccionOperarioScreen->show();
}

void Equipo_Operario::on_l_delete_operario_clicked()
{
    ui->l_operarios->removeItem(ui->l_operarios->currentText());
}

void Equipo_Operario::on_le_codigo_textEdited(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->le_equipo_operario->clear();
    ui->l_operarios->removeItems();
}
