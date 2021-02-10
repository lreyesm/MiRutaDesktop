#include "cliente.h"
#include "ui_cliente.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "screen_tabla_tareas.h"
#include "new_table_structure.h"
#include "global_variables.h"
#include "restrictions.h"
#include "globalfunctions.h"

Cliente::Cliente(QWidget *parent, bool newOne, QString empresa) :
    QWidget(parent),
    ui(new Ui::Cliente)
{
    ui->setupUi(this);
    this->setWindowTitle("Cliente");
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
    QStringList list_premisos;
    list_premisos << "TODOS" << "NINGUNO";
    //ui->l_permisos->addItems(list_premisos);

    connect(ui->lb_foto, &MyLabelPhoto::selectedPhoto, this, &Cliente::selectedPhoto);
    connect(ui->lb_foto, &MyLabelPhoto::selectedPhoto, this, &Cliente::selectedPhoto);
}

Cliente::~Cliente()
{
    delete ui;
}

void Cliente::selectedPhoto(QPixmap pixmap)
{
    scalePhoto(pixmap);
    this->photoSelected = true;

    savePhotoLocal(pixmap);
}
void Cliente::scalePhoto(QPixmap pixmap){
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
bool Cliente::loadPhotoLocal(){ //retorna true si la encontro
    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_cliente->text())){
        QString ruta = "C:/Mi_Ruta/Empresas/" + empresa + "/fotos_clientes";
        QString path = ruta + "/" + ui->le_cliente->text().trimmed()+"_cliente" + ".jpg";
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
void Cliente::savePhotoLocal(QPixmap pixmap){
    QImage img = pixmap.toImage();
    //SALVAR EN PC

    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_cliente->text())){
        QString ruta = "C:/Mi_Ruta/Empresas/" + empresa + "/fotos_clientes";
        QDir dir(ruta);
        if(!dir.exists()){
            dir.mkpath(ruta);
        }
        img.save(ruta + "/" + ui->le_cliente->text().trimmed()+"_cliente" + ".jpg");
    }
}
bool Cliente::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
void Cliente::populateView(QJsonObject o)
{
    QString cliente_m, nombre_m, clave, telefono, tareas, cod_m, direccion;

    cliente_m = o.value(cliente_clientes).toString();//usuario
    cod_m = o.value(codigo_cliente_clientes).toString();
    nombre_m = o.value(nombre_clientes).toString();
    clave = o.value(clave_clientes).toString();
    telefono = o.value(telefono_clientes).toString();
    direccion = o.value(direccion_clientes).toString();
    nombre_foto = o.value(foto_clientes).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_cliente->setText(cliente_m);
    ui->le_nombre->setText(nombre_m);
    ui->le_clave->setText(clave);
    ui->le_telefono->setText(telefono);
    ui->le_direccion->setText(direccion);

    if(checkIfFieldIsValid(cliente_m)){
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
QString Cliente::guardarDatos(){
    QString cliente_m, nombre_m, clave, telefono, cod_m, direccion;

    cliente_m = ui->le_cliente->text().trimmed();
    cod_m = ui->le_codigo->text().trimmed();
    nombre_m = ui->le_nombre->text();
    clave = ui->le_clave->text();
    telefono = ui->le_telefono->text();
    direccion = ui->le_direccion->text();

    QString count_login = cliente.value(count_logins_clientes).toString();
    if(!checkIfFieldIsValid(count_login)){
        cliente.insert(count_logins_clientes, "0");
    }

//    if(cliente_m != cliente.value(cliente_clientes).toString()){
//        update_tareas = true;
//        old_cliente = cliente.value(cliente_clientes).toString();
//    }
    cliente.insert(cliente_clientes, cliente_m);
    cliente.insert(codigo_cliente_clientes, cod_m);
    cliente.insert(nombre_clientes, nombre_m);
    cliente.insert(clave_clientes, clave);
    cliente.insert(telefono_clientes, telefono);
    cliente.insert(direccion_clientes, direccion);

    return cod_m;
}


void Cliente::create_cliente_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_CLIENTE,keys,values);
}
void Cliente::update_cliente_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_CLIENTE,keys,values);
}

void Cliente::delete_cliente_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_CLIENTE,keys,values);
}
void Cliente::download_cliente_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_CLIENTE_IMAGE,keys,values);
}
void Cliente::upload_cliente_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPLOAD_CLIENTE_IMAGE,keys,values);
}
bool Cliente::writeClientes(QJsonArray jsonArray){
    QFile *data_base = new QFile(clientes_descargados); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Cliente::readClientes(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(clientes_descargados); // ficheros .dat se puede utilizar formato txt tambien
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
QStringList Cliente::getListaUsuarios(){
    QJsonArray jsonArray = readClientes();
    QStringList lista;
    QString usuario;
    for (int i =0; i < jsonArray.size(); i++) {
        usuario = jsonArray.at(i).toObject().value(cliente_clientes).toString();
        if(!lista.contains(usuario)){
            lista.append(usuario);
        }
    }
    return  lista;
}
QStringList Cliente::getListaClientes(){
    QJsonArray jsonArray = readClientes();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_cliente_clientes).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Cliente::descargarPhoto(){
    bool retorno = false;
    if(checkIfFieldIsValid(nombre_foto) && checkIfFieldIsValid(ui->le_cliente->text())){
        QStringList keys, values;
        keys << "empresa" << "cliente";
        values << empresa << ui->le_cliente->text().trimmed();

        int cant_fotos = 1;

        for(int i = 0, reintentos = 0; i < cant_fotos; i++)
        {
            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
            //OJO ESPERAR POR RESPUESTA
            QEventLoop q;

            connect(this, &Cliente::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;
            QTimer::singleShot(DELAY, this, &Cliente::download_cliente_image_request);

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

            case database_comunication::script_result::download_cliente_image_failed:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                    i = cant_fotos;
                }
                break;

            case database_comunication::script_result::download_cliente_image_picture_doesnt_exists:

                break;

            case database_comunication::script_result::ok:
                retorno = true;
                break;
            }
        }
    }
    return retorno;
}
bool Cliente::subirPhoto(){
    bool retorno = false;
    if(photoSelected && checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_cliente->text()) && ui->lb_foto->pixmap() != nullptr)
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
        keys << "foto" << "empresa" << "cliente" << "codigo_cliente";
        values.clear();
        values << foto << empresa << ui->le_cliente->text().trimmed() << ui->le_codigo->text().trimmed();

        for(int i = 0, reintentos = 0; i < 1; i++)
        {

            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                    this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

            QEventLoop q;

            connect(this, &Cliente::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;

            //ojo cambiar por upload
            QTimer::singleShot(DELAY, this, &Cliente::upload_cliente_image_request);

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

bool Cliente::subirCliente(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    cliente.insert(date_time_modified_clientes, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(cliente);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa;

    QEventLoop *q = new QEventLoop();

    connect(this, &Cliente::script_excecution_result,q,&QEventLoop::exit);

    if(getListaClientes().contains(codigo)){
        update_cliente_request(keys, values);
    }
    else{
        if(getListaUsuarios().contains(cliente.value(cliente_clientes).toString())){
            GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este usuario, introduzca otro");
            return false;
        }else{
            create_cliente_request(keys, values);
        }
    }

    bool res = true;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::cliente_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_cliente_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_cliente_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    subirPhoto();
    return res;
}
void Cliente::update_tareas_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA_FIELDS,keys,values);
}
bool Cliente::update_fields(QStringList numeros_internos_list, QJsonObject campos){
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

    connect(this, &Cliente::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Cliente::update_tareas_fields_request);

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
void Cliente::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_CLIENTE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_cliente"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_cliente_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_cliente_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_cliente"))
            {
                result = database_comunication::script_result::cliente_to_server_ok;
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
    else if(tipo == database_comunication::UPDATE_CLIENTE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_cliente_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_cliente"))
            {
                result = database_comunication::script_result::cliente_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_CLIENTE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_cliente"))
        {
            result = database_comunication::script_result::delete_cliente_failed;

        }
        else
        {
            if(ba.contains("success ok delete_cliente"))
            {
                result = database_comunication::script_result::delete_cliente_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPLOAD_CLIENTE_IMAGE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success upload_cliente_image"))
        {
            result = database_comunication::script_result::upload_empresa_image_failed;

        }
        else
        {
            if(ba.contains("success ok upload_cliente_image"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    if(tipo == database_comunication::DOWNLOAD_CLIENTE_IMAGE)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success download_cliente_image"))
        {
            if(ba.contains("o se pudo obtener imagen de cliente"))
                result = database_comunication::script_result::download_cliente_image_failed;
            else if(ba.contains("o existe imagen de cliente"))
                result = database_comunication::script_result::download_cliente_image_picture_doesnt_exists;
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

void Cliente::on_pb_agregar_clicked()
{
    if(!checkEmailValid(ui->le_cliente->text())){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Usuario Inválido","El usuario debe ser un email válido");
        return;
    }
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaClientes().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    if(subirCliente(cod_m)){
        emit update_tableClientes(true);
        this->close();
    }
}
void Cliente::on_pb_actualizar_clicked()
{
    if(!checkEmailValid(ui->le_cliente->text())){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Usuario Inválido","El usuario debe ser un email válido");
        return;
    }
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(subirCliente(cod_m)){
        emit update_tableClientes(true);
        this->close();
    }
}
void Cliente::on_pb_borrar_clicked()
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

        eliminarCliente(cod_m);
        emit update_tableClientes(true);
        this->close();
    }


}
bool Cliente::eliminarCliente(QString codigo){
    //    subirTodasLasClientes();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    cliente.insert(date_time_modified_clientes, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(cliente);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop *q = new QEventLoop();

    connect(this, &Cliente::script_excecution_result,q,&QEventLoop::exit);

    if(getListaClientes().contains(codigo)){
        delete_cliente_request(keys, values);
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

    case database_comunication::script_result::delete_cliente_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_cliente_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Cliente::subirTodasLasClientes()
{
    QMap<QString, QString> mapaTiposDeCliente;
    mapaTiposDeCliente.insert("001","TXORIERRI");
    mapaTiposDeCliente.insert("002","DURANGUESADO");
    mapaTiposDeCliente.insert("003","LEA-ARTIBAI");
    mapaTiposDeCliente.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeCliente.insert("005","ENCARTACIONES - CLIENTE MINERA");
    mapaTiposDeCliente.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeCliente.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeCliente.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeCliente.values().at(i);
        QString key = mapaTiposDeCliente.keys().at(i);
        jsonObject.insert(codigo_cliente_clientes, key);
        jsonObject.insert(cliente_clientes, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirCliente(cod);
    }
    emit update_tableClientes(true);
    this->close();
}

void Cliente::on_le_cliente_editingFinished()
{
    if(checkEmailValid(ui->le_cliente->text())){
        ui->lb_foto->setEnabled(true);
        ui->pb_actualizar->setEnabled(true);
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this, "Email invalido", "El usuario del cliente debe ser un email valido");
        ui->lb_foto->setEnabled(false);
        ui->pb_actualizar->setEnabled(false);
    }
}
bool Cliente::checkEmailValid(QString email){
    QStringList split = email.split("@");
    if(split.size() > 1){
        if(email.contains("@") && split[1].contains(".")){
            return true;
        }
    }else{
        return false;
    }
    return false;
}


void Cliente::on_pb_restrict_clicked()
{
    Restrictions *res = new Restrictions(nullptr, cliente, empresa);
    connect(res, &Restrictions::setRestrictions, this, &Cliente::setRestrictions);
    res->show();
}
void Cliente::setRestrictions(QString restrictions){
    cliente.insert(permisos_clientes, restrictions);
}
