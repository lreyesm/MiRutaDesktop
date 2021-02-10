#include "operario.h"
#include "ui_operario.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "screen_tabla_tareas.h"
#include "new_table_structure.h"
#include "jornada_operario.h"
#include "global_variables.h"
#include "globalfunctions.h"

Operario::Operario(QWidget *parent, bool newOne, QString empresa) :
    QWidget(parent),
    ui(new Ui::Operario)
{
    ui->setupUi(this);
    this->setWindowTitle("Fontanero");
    this->empresa = empresa;

    if(newOne){
        ui->pb_agregar->show();
        ui->pb_actualizar->hide();
        ui->pb_borrar->hide();
        ui->pb_jornada->hide();
    }else{
        ui->pb_agregar->hide();
        ui->pb_actualizar->show();
        ui->pb_borrar->show();
        ui->pb_jornada->show();
    }
    QStringList list_premisos;
    list_premisos << "TODOS" << "NINGUNO";
    //ui->l_permisos->addItems(list_premisos);

    connect(ui->lb_foto, &MyLabelPhoto::selectedPhoto, this, &Operario::selectedPhoto);
}

Operario::~Operario()
{
    delete ui;
}

void Operario::selectedPhoto(QPixmap pixmap)
{
    scalePhoto(pixmap);
    this->photoSelected = true;

    savePhotoLocal(pixmap);
}
void Operario::scalePhoto(QPixmap pixmap){
    QSize size = pixmap.size();
    int max_height = 200;
    int max_width = 200;
    double ratio;
    if(size.width() > size.height()){
        ratio = static_cast<double>(size.height())/ static_cast<double>(size.width());
        max_height = max_width * ratio;
    }else{
        ratio = static_cast<double>(size.width())/ static_cast<double>(size.height());
        max_width = max_height * ratio;
    }
    ui->lb_foto->setScaledContents(true);
    ui->lb_foto->setMaximumSize(max_width, max_height);
}
bool Operario::loadPhotoLocal(){ //retorna true si la encontro
    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_operario->text())){
        QString ruta = "C:/Mi_Ruta/Empresas/" + empresa + "/fotos_operarios";
        QString path = ruta + "/" + ui->le_operario->text().trimmed()+"_operario" + ".jpg";
        if(QFile::exists(path)){
            scalePhoto(QPixmap(path));
            ui->lb_foto->setPixmap(QPixmap(path));
            return true;
        }else{
            return false;
        }
    }
    return false;
}
void Operario::savePhotoLocal(QPixmap pixmap){
    QImage img = pixmap.toImage();
    //SALVAR EN PC

    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_operario->text())){
        QString ruta = "C:/Mi_Ruta/Empresas/" + empresa + "/fotos_operarios";
        QDir dir(ruta);
        if(!dir.exists()){
            dir.mkpath(ruta);
        }
        img.save(ruta + "/" + ui->le_operario->text().trimmed()+"_operario" + ".jpg");
    }
}
bool Operario::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
void Operario::populateView(QJsonObject o)
{
    QString operario_m, nombre_m, clave, telefono, tareas, cod_m, apellidos;

    operario_m = o.value(operario_operarios).toString();//usuario
    cod_m = o.value(codigo_operario_operarios).toString();
    nombre_m = o.value(nombre_operarios).toString();
    clave = o.value(clave_operarios).toString();
    telefono = o.value(telefono_operarios).toString();
    tareas = o.value(tareas_operarios).toString();
    apellidos = o.value(apellidos_operarios).toString();
    nombre_foto = o.value(foto_operarios).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_operario->setText(operario_m);
    ui->le_nombre->setText(nombre_m);
    ui->le_clave->setText(clave);
    ui->le_telefono->setText(telefono);
    ui->le_apellidos->setText(apellidos);

    if(checkIfFieldIsValid(operario_m)){
        ui->lb_foto->setEnabled(true);
    }else{
        ui->lb_foto->setEnabled(false);
    }
    if(!descargarPhoto()){ //si no hay foto en servidor
        if(loadPhotoLocal()){//si hay foto local
            photoSelected = true; //subo foto al actualizar
        }
    }
}
//Ojo añadir tarea cuando se use en esta logica
QString Operario::guardarDatos(){
    QString operario_m, nombre_m, clave, telefono, permisos, cod_m, apellidos;

    operario_m = ui->le_operario->text().trimmed();
    cod_m = ui->le_codigo->text().trimmed();
    nombre_m = ui->le_nombre->text();
    clave = ui->le_clave->text();
    telefono = ui->le_telefono->text();
    apellidos = ui->le_apellidos->text();

    if(operario_m != operario.value(operario_operarios).toString()){
        update_tareas = true;
        old_operario = operario.value(operario_operarios).toString();
    }
    operario.insert(operario_operarios, operario_m);
    operario.insert(codigo_operario_operarios, cod_m);
    operario.insert(nombre_operarios, nombre_m);
    operario.insert(clave_operarios, clave);
    operario.insert(telefono_operarios, telefono);
    operario.insert(apellidos_operarios, apellidos);
    operario.insert(edad_operarios, 0);

    return cod_m;
}


void Operario::create_operario_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_OPERARIO,keys,values);
}
void Operario::update_operario_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_OPERARIO,keys,values);
}

void Operario::delete_operario_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_OPERARIO,keys,values);
}
void Operario::download_operario_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_OPERARIO_IMAGE,keys,values);
}
void Operario::upload_operario_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPLOAD_OPERARIO_IMAGE,keys,values);
}
bool Operario::writeOperarios(QJsonArray jsonArray){
    QFile *data_base = new QFile(operarios_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Operario::readOperarios(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(operarios_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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
QStringList Operario::getListaUsuarios(){
    QJsonArray jsonArray = readOperarios();
    QStringList lista;
    QString usuario;
    for (int i =0; i < jsonArray.size(); i++) {
        usuario = jsonArray.at(i).toObject().value(operario_operarios).toString();
        if(!lista.contains(usuario)){
            lista.append(usuario);
        }
    }
    return  lista;
}
QStringList Operario::getListaOperarios(){
    QJsonArray jsonArray = readOperarios();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_operario_operarios).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Operario::descargarPhoto(){
    bool retorno = false;
    if(checkIfFieldIsValid(nombre_foto) && checkIfFieldIsValid(ui->le_operario->text())){
        QStringList keys, values;
        keys << "empresa" << "operario";
        values << empresa << ui->le_operario->text().trimmed();

        int cant_fotos = 1;

        for(int i = 0, reintentos = 0; i < cant_fotos; i++)
        {
            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
            //OJO ESPERAR POR RESPUESTA
            QEventLoop q;

            connect(this, &Operario::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;
            QTimer::singleShot(DELAY, this, &Operario::download_operario_image_request);

            switch (q.exec())
            {
            case database_comunication::script_result::timeout:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    GlobalFunctions gf(this);
                    GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                    i = cant_fotos;
                }
                break;

            case database_comunication::script_result::download_operario_image_failed:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    GlobalFunctions gf(this);
                    GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                    i = cant_fotos;
                }
                break;

            case database_comunication::script_result::download_operario_image_picture_doesnt_exists:

                break;

            case database_comunication::script_result::ok:
                retorno = true;
                break;
            }
        }
    }
    return retorno;
}
bool Operario::subirPhoto(){
    bool retorno = false;
    if(photoSelected && checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_operario->text()) && ui->lb_foto->pixmap() != nullptr)
    {
        QImage image;
        QByteArray byteArray;
        QString foto;
        QBuffer *buffer;
        QStringList keys, values;

        image = ui->lb_foto->pixmap()->toImage();
        buffer = new QBuffer(&byteArray);
        image.save(buffer,"jpg");
        foto = QString::fromLatin1(byteArray.toBase64().data());
        delete buffer;

        keys.clear();
        keys << "foto" << "empresa" << "operario" << "codigo_operario";
        values.clear();
        values << foto << empresa << ui->le_operario->text().trimmed() << ui->le_codigo->text().trimmed();

        for(int i = 0, reintentos = 0; i < 1; i++)
        {

            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                    this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

            QEventLoop q;

            connect(this, &Operario::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;

            //ojo cambiar por upload
            QTimer::singleShot(DELAY, this, &Operario::upload_operario_image_request);

            switch (q.exec())
            {
            case database_comunication::script_result::timeout:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    GlobalFunctions gf(this);
                    GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                    i = 1;
                }
                break;
            case database_comunication::script_result::upload_empresa_image_failed:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    GlobalFunctions gf(this);
                    GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                    i = 7;
                }
                break;
            case database_comunication::script_result::ok:
                retorno = true;
                break;
            }
        }
    }
    return retorno;
}

bool Operario::subirOperario(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    operario.insert(date_time_modified_operarios, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(operario);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa;

    QEventLoop *q = new QEventLoop();

    connect(this, &Operario::script_excecution_result,q,&QEventLoop::exit);

    if(getListaOperarios().contains(codigo)){
        if(update_tareas){
            update_tareas = false;
            QStringList numInternos;
            QJsonObject campos;

            GlobalFunctions gf(this, empresa);
            numInternos = gf.getTareasList(OPERARIO, old_operario);

            campos.insert(OPERARIO, operario.value(
                              operario_operarios).toString());
            campos.insert(date_time_modified, QDateTime::currentDateTime().toString(formato_fecha_hora));
            update_fields(numInternos, campos);
            emit updateTareas();
        }
        update_operario_request(keys, values);
    }
    else{
        if(getListaUsuarios().contains(operario.value(operario_operarios).toString())){
            GlobalFunctions gf(this);
            GlobalFunctions::showWarning(this,"Ya existe","Ya existe este usuario, introduzca otro");
            return false;
        }else{
            create_operario_request(keys, values);
        }
    }

    bool res = true;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::operario_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_operario_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_operario_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    subirPhoto();
    return res;
}
void Operario::update_tareas_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA_FIELDS,keys,values);
}
bool Operario::update_fields(QStringList numeros_internos_list, QJsonObject campos){
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

    connect(this, &Operario::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Operario::update_tareas_fields_request);

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
void Operario::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
    //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_OPERARIO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_operario"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_operario_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_operario_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_operario"))
            {
                result = database_comunication::script_result::operario_to_server_ok;
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
    else if(tipo == database_comunication::UPDATE_OPERARIO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_operario_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_operario"))
            {
                result = database_comunication::script_result::operario_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_OPERARIO)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_operario"))
        {
            result = database_comunication::script_result::delete_operario_failed;

        }
        else
        {
            if(ba.contains("success ok delete_operario"))
            {
                result = database_comunication::script_result::delete_operario_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPLOAD_OPERARIO_IMAGE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success upload_operario_image"))
        {
            result = database_comunication::script_result::upload_empresa_image_failed;

        }
        else
        {
            if(ba.contains("success ok upload_operario_image"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    if(tipo == database_comunication::DOWNLOAD_OPERARIO_IMAGE)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success download_operario_image"))
        {
            if(ba.contains("o se pudo obtener imagen de operario"))
                result = database_comunication::script_result::download_operario_image_failed;
            else if(ba.contains("o existe imagen de operario"))
                result = database_comunication::script_result::download_operario_image_picture_doesnt_exists;
        }
        else
        {
            QString foto = QString::fromUtf8(ba);
            QImage temp_img = database_comunication::getImageFromString(foto);
            if(temp_img.isNull())
            {
                //                ui->lb_foto->setPixmap((QPixmap("User_Big.png")));
            }
            else
            {
                QPixmap pixmap = QPixmap::fromImage(temp_img);
                scalePhoto(pixmap);
                ui->lb_foto->setPixmap(pixmap);
                savePhotoLocal(pixmap);
            }
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}

void Operario::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaOperarios().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    if(subirOperario(cod_m)){
        emit update_tableOperarios(true);
        this->close();
    }
}
void Operario::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(subirOperario(cod_m)){
        emit update_tableOperarios(true);
        this->close();
    }
}
void Operario::on_pb_borrar_clicked()
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

        eliminarOperario(cod_m);
        emit update_tableOperarios(true);
        this->close();
    }


}
bool Operario::eliminarOperario(QString codigo){
    //    subirTodasLasOperarios();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    operario.insert(date_time_modified_operarios, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(operario);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop *q = new QEventLoop();

    connect(this, &Operario::script_excecution_result,q,&QEventLoop::exit);

    if(getListaOperarios().contains(codigo)){
        delete_operario_request(keys, values);
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

    case database_comunication::script_result::delete_operario_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_operario_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Operario::subirTodasLasOperarios()
{
    QMap<QString, QString> mapaTiposDeOperario;
    mapaTiposDeOperario.insert("001","TXORIERRI");
    mapaTiposDeOperario.insert("002","DURANGUESADO");
    mapaTiposDeOperario.insert("003","LEA-ARTIBAI");
    mapaTiposDeOperario.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeOperario.insert("005","ENCARTACIONES - OPERARIO MINERA");
    mapaTiposDeOperario.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeOperario.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeOperario.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeOperario.values().at(i);
        QString key = mapaTiposDeOperario.keys().at(i);
        jsonObject.insert(codigo_operario_operarios, key);
        jsonObject.insert(operario_operarios, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirOperario(cod);
    }
    emit update_tableOperarios(true);
    this->close();
}

void Operario::on_le_operario_editingFinished()
{
    if(checkIfFieldIsValid(ui->le_operario->text())){
        ui->lb_foto->setEnabled(true);
    }else{
        ui->lb_foto->setEnabled(false);
    }
}

void Operario::on_pb_jornada_clicked()
{
    Jornada_Operario *jornada = new Jornada_Operario(nullptr, operario);
    jornada->show();
}
