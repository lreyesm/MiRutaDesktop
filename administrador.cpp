#include "administrador.h"
#include "ui_administrador.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "globalfunctions.h"

Administrador::Administrador(QWidget *parent, bool newOne, QString empresa) :
    QWidget(parent),
    ui(new Ui::Administrador)
{
    ui->setupUi(this);
    this->setWindowTitle("Administrador");
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
    ui->l_permisos->addItems(list_premisos);

    connect(ui->lb_foto, &MyLabelPhoto::selectedPhoto, this, &Administrador::selectedPhoto);

    this->setAttribute(Qt::WA_DeleteOnClose);
}

Administrador::~Administrador()
{
    delete ui;
}

void Administrador::selectedPhoto(QPixmap pixmap)
{
    scalePhoto(pixmap);
    this->photoSelected = true;

    savePhotoLocal(pixmap);
}
void Administrador::scalePhoto(QPixmap pixmap){
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
bool Administrador::loadPhotoLocal(){ //retorna true si la encontro
    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_administrador->text())){
        QString ruta = "C:/Mi_Ruta/Empresas/" + empresa + "/fotos_administradores";
        QString path = ruta + "/" + ui->le_administrador->text().trimmed()+"_administrador" + ".jpg";
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
void Administrador::savePhotoLocal(QPixmap pixmap){
    QImage img = pixmap.toImage();
    //SALVAR EN PC

    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_administrador->text())){
        QString ruta = "C:/Mi_Ruta/Empresas/" + empresa + "/fotos_administradores";
        QDir dir(ruta);
        if(!dir.exists()){
            dir.mkpath(ruta);
        }
        img.save(ruta + "/" + ui->le_administrador->text().trimmed()+"_administrador" + ".jpg");
    }
}
bool Administrador::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
void Administrador::populateView(QJsonObject o)
{
    QString administrador_m, nombre_m, clave, telefono, permisos, cod_m;

    administrador_m = o.value(administrador_administradores).toString();//usuario
    cod_m = o.value(codigo_administrador_administradores).toString();
    nombre_m = o.value(nombre_administradores).toString();
    clave = o.value(clave_administradores).toString();
    telefono = o.value(telefono_administradores).toString();
    permisos = o.value(permisos_administradores).toString();
    nombre_foto = o.value(foto_administradores).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_administrador->setText(administrador_m);
    ui->le_nombre->setText(nombre_m);
    ui->le_clave->setText(clave);
    ui->le_telefono->setText(telefono);
    ui->l_permisos->setText(permisos);

    if(checkIfFieldIsValid(administrador_m)){
        ui->lb_foto->setEnabled(true);
    }else{
        ui->lb_foto->setEnabled(false);
    }
    if(!descargarPhoto()){ //si no hay foto en servidor
        if(loadPhotoLocal()){//si hay foto local
            photoSelected = true; //subo foto al actualizar
        }
    }
    if(permisos == "PRIMARIO"){
        ui->l_permisos->setEnabled(false);
        ui->pb_borrar->setEnabled(false);
    }
}

QString Administrador::guardarDatos(){
    QString administrador_m, nombre_m, clave, telefono, permisos, cod_m;

    administrador_m = ui->le_administrador->text().trimmed();
    cod_m = ui->le_codigo->text().trimmed();
    nombre_m = ui->le_nombre->text();
    clave = ui->le_clave->text();
    telefono = ui->le_telefono->text();
    permisos = ui->l_permisos->text();

    administrador.insert(administrador_administradores, administrador_m);
    administrador.insert(codigo_administrador_administradores, cod_m);
    administrador.insert(nombre_administradores, nombre_m);
    administrador.insert(clave_administradores, clave);
    administrador.insert(telefono_administradores, telefono);
    administrador.insert(permisos_administradores, permisos);
    return cod_m;
}


void Administrador::create_administrador_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_ADMINISTRADOR,keys,values);
}
void Administrador::update_administrador_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_ADMINISTRADOR,keys,values);
}

void Administrador::delete_administrador_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_ADMINISTRADOR,keys,values);
}
void Administrador::download_administrador_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_ADMINISTRADOR_IMAGE,keys,values);
}
void Administrador::upload_administrador_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPLOAD_ADMINISTRADOR_IMAGE,keys,values);
}
bool Administrador::writeAdministradores(QJsonArray jsonArray){
    QFile *data_base = new QFile(administradores_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Administrador::readAdministradores(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(administradores_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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
QStringList Administrador::getListaUsuarisAdministradores(){
    QJsonArray jsonArray = readAdministradores();
    QStringList lista;
    QString admin;
    for (int i =0; i < jsonArray.size(); i++) {
        admin = jsonArray.at(i).toObject().value(administrador_administradores).toString();
        if(!lista.contains(admin)){
            lista.append(admin);
        }
    }
    return  lista;
}
QStringList Administrador::getListaAdministradores(){
    QJsonArray jsonArray = readAdministradores();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_administrador_administradores).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Administrador::descargarPhoto(){
    bool retorno = false;
    if(checkIfFieldIsValid(nombre_foto) && checkIfFieldIsValid(ui->le_administrador->text())){
        QStringList keys, values;
        keys << "empresa" << "administrador";
        values << empresa << ui->le_administrador->text().trimmed();

        int cant_fotos = 1;

        for(int i = 0, reintentos = 0; i < cant_fotos; i++)
        {
            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
            //OJO ESPERAR POR RESPUESTA
            QEventLoop q;

            connect(this, &Administrador::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;
            QTimer::singleShot(DELAY, this, &Administrador::download_administrador_image_request);

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

            case database_comunication::script_result::download_administrador_image_failed:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    GlobalFunctions gf(this);
                    GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                    i = cant_fotos;
                }
                break;

            case database_comunication::script_result::download_administrador_image_picture_doesnt_exists:

                break;

            case database_comunication::script_result::ok:
                retorno = true;
                break;
            }
        }
    }
    return retorno;
}
bool Administrador::subirPhoto(){
    bool retorno = false;
    if(photoSelected && checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_administrador->text()) && ui->lb_foto->pixmap() != nullptr)
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
        keys << "foto" << "empresa" << "administrador";
        values.clear();
        values << foto << empresa << ui->le_administrador->text().trimmed();

        for(int i = 0, reintentos = 0; i < 1; i++)
        {

            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                    this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

            QEventLoop q;

            connect(this, &Administrador::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;

            //ojo cambiar por upload
            QTimer::singleShot(DELAY, this, &Administrador::upload_administrador_image_request);

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
            case database_comunication::script_result::upload_administrador_image_failed:
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

bool Administrador::subirAdministrador(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    administrador.insert(date_time_modified_administradores, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(administrador);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa;

    QEventLoop *q = new QEventLoop();

    connect(this, &Administrador::script_excecution_result,q,&QEventLoop::exit);

    if(getListaAdministradores().contains(codigo)){
        update_administrador_request(keys, values);
    }
    else{
        if(getListaUsuarisAdministradores().contains(
                    administrador.value(administrador_administradores).toString())){
            GlobalFunctions gf(this);
            GlobalFunctions::showWarning(this,"Ya existe","Ya existe este administrador, introduzca otro");
            return false;
        }else{
            create_administrador_request(keys, values);
        }
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::administrador_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_administrador_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_administrador_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    subirPhoto();
    return res;
}

void Administrador::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
    //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_ADMINISTRADOR)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_administrador"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_administrador_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_administrador_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_administrador"))
            {
                result = database_comunication::script_result::administrador_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_ADMINISTRADOR)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_administrador_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_administrador"))
            {
                result = database_comunication::script_result::administrador_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_ADMINISTRADOR)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_administrador"))
        {
            result = database_comunication::script_result::delete_administrador_failed;

        }
        else
        {
            if(ba.contains("success ok delete_administrador"))
            {
                result = database_comunication::script_result::delete_administrador_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPLOAD_ADMINISTRADOR_IMAGE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success upload_administrador_image"))
        {
            result = database_comunication::script_result::upload_administrador_image_failed;

        }
        else
        {
            if(ba.contains("success ok upload_administrador_image"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    if(tipo == database_comunication::DOWNLOAD_ADMINISTRADOR_IMAGE)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success download_administrador_image"))
        {
            if(ba.contains("o se pudo obtener imagen de administrador"))
                result = database_comunication::script_result::download_administrador_image_failed;
            else if(ba.contains("o existe imagen de administrador"))
                result = database_comunication::script_result::download_administrador_image_picture_doesnt_exists;
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

void Administrador::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaAdministradores().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    if(subirAdministrador(cod_m)){
        emit update_tableAdministradores(true);
        this->close();
    }
}
void Administrador::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(subirAdministrador(cod_m)){
        emit update_tableAdministradores(true);
        this->close();
    }
}
void Administrador::on_pb_borrar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    GlobalFunctions gf(this, empresa);
    if(gf.showQuestion(this, "Confirmacion", "Seguro desea eliminar el modelo?",
                       QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){

        eliminarAdministrador(cod_m);
        //        subirTodasLasAdministradores();
        emit update_tableAdministradores(true);
        this->close();
    }


}
bool Administrador::eliminarAdministrador(QString codigo){
    //    subirTodasLasAdministradores();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    administrador.insert(date_time_modified_administradores, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(administrador);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop *q = new QEventLoop();

    connect(this, &Administrador::script_excecution_result,q,&QEventLoop::exit);

    if(getListaAdministradores().contains(codigo)){
        delete_administrador_request(keys, values);
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

    case database_comunication::script_result::delete_administrador_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_administrador_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Administrador::subirTodasLasAdministradores()
{
    QMap<QString, QString> mapaTiposDeAdministrador;
    mapaTiposDeAdministrador.insert("001","TXORIERRI");
    mapaTiposDeAdministrador.insert("002","DURANGUESADO");
    mapaTiposDeAdministrador.insert("003","LEA-ARTIBAI");


    for(int i=0; i< mapaTiposDeAdministrador.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeAdministrador.values().at(i);
        QString key = mapaTiposDeAdministrador.keys().at(i);
        jsonObject.insert(codigo_administrador_administradores, key);
        jsonObject.insert(administrador_administradores, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirAdministrador(cod);
    }
    emit update_tableAdministradores(true);
    this->close();
}

void Administrador::on_le_administrador_editingFinished()
{
    if(checkIfFieldIsValid(ui->le_administrador->text())){
        ui->lb_foto->setEnabled(true);
    }else{
        ui->lb_foto->setEnabled(false);
    }
}
