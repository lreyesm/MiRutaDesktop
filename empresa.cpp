#include "empresa.h"
#include "ui_empresa.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "global_variables.h"
#include "mapa_zonas.h"
#include "globalfunctions.h"

Empresa::Empresa(QWidget *parent, bool newOne) :
    QWidget(parent),
    ui(new Ui::Empresa)
{
    ui->setupUi(this);
    this->setWindowTitle("Empresa");
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

    connect(ui->lb_foto, &MyLabelPhoto::selectedPhoto, this, &Empresa::selectedPhoto);
}

Empresa::~Empresa()
{
    delete ui;
}
void Empresa::selectedPhoto(QPixmap pixmap)
{
    scalePhoto(pixmap);
    this->photoSelected = true;

    savePhotoLocal(pixmap);
}
void Empresa::scalePhoto(QPixmap pixmap){
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
bool Empresa::loadPhotoLocal(){ //retorna true si la encontro
    if(!ui->le_empresa->text().trimmed().isEmpty()){
        QString ruta = "C:/Mi_Ruta/Empresas/" + ui->le_empresa->text().trimmed() + "/Logo";
        QString path = ruta + "/" + ui->le_empresa->text().trimmed()+"_logo" + ".jpg";
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
void Empresa::savePhotoLocal(QPixmap pixmap){
    QImage img = pixmap.toImage();
    //SALVAR EN PC

    if(!ui->le_empresa->text().trimmed().isEmpty()){
        QString ruta = "C:/Mi_Ruta/Empresas/" + ui->le_empresa->text().trimmed() + "/Logo";
        QDir dir(ruta);
        if(!dir.exists()){
            dir.mkpath(ruta);
        }
        img.save(ruta + "/" + ui->le_empresa->text().trimmed()+"_logo" + ".jpg");
    }
}
bool Empresa::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
void Empresa::populateView(QJsonObject o)
{
    QString empresa_m, descripcion, cod_m, permisos, geolocalizacion;
    empresa_m = o.value(empresa_empresas).toString();
    cod_m = o.value(codigo_empresa_empresas).toString();
    geolocalizacion = o.value(geolocalizacion_empresas).toString();
    descripcion = o.value(descripcion_empresas).toString();
    permisos = o.value(permisos_empresas).toString();
    nombre_foto = o.value(foto_empresas).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_empresa->setText(empresa_m);
    ui->le_geolocalizacion->setText(geolocalizacion);
    ui->l_permisos->setText(permisos);
    ui->pt_descripcion_general->setPlainText(descripcion);


    if(ui->le_empresa->text().trimmed().isEmpty()){
        ui->lb_foto->setEnabled(false);
    }else{
        ui->lb_foto->setEnabled(true);
    }
    if(!descargarPhoto()){ //si no hay foto en servidor
        if(loadPhotoLocal()){//si hay foto local
            photoSelected = true; //subo foto al actualizar
        }
    }
}

QString Empresa::guardarDatos(){
    QString empresa_m, descripcion, cod_m, permisos, geolocalizacion;
    empresa_m =  ui->le_empresa->text().trimmed();
    cod_m = ui->le_codigo->text();
    geolocalizacion =  ui->le_geolocalizacion->text();
    descripcion = ui->pt_descripcion_general->toPlainText();
    permisos =  ui->l_permisos->text();

    empresa.insert(empresa_empresas, empresa_m);
    empresa.insert(codigo_empresa_empresas, cod_m);

    empresa.insert(geolocalizacion_empresas, geolocalizacion);
    empresa.insert(descripcion_empresas, descripcion);
    empresa.insert(permisos_empresas, permisos);

    return cod_m;
}


void Empresa::create_empresa_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_EMPRESA,keys,values);
}
void Empresa::update_empresa_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_EMPRESA,keys,values);
}
void Empresa::delete_empresa_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_EMPRESA,keys,values);
}
void Empresa::download_empresa_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_EMPRESA_IMAGE,keys,values);
}
void Empresa::upload_empresa_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPLOAD_EMPRESA_IMAGE,keys,values);
}

bool Empresa::writeEmpresas(QJsonArray jsonArray){
    QFile *data_base = new QFile(empresas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QString Empresa::getCurrentFieldEmpresa(QString name_empresa){
    QJsonArray jsonArray = Empresa::readEmpresas();
    for (int i=0; i < jsonArray.size(); i++) {
        QString empresa_v, nombre_empresa_v, cod_v;
        nombre_empresa_v = jsonArray.at(i).toObject().value(nombre_empresa_empresas).toString();
        if(nombre_empresa_v == name_empresa){
            empresa_v = jsonArray.at(i).toObject().value(empresa_empresas).toString();
            return empresa_v;
        }
    }
    return "";
}
QJsonArray Empresa::readEmpresas(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(empresas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QStringList Empresa::getListaEmpresas(){
    QJsonArray jsonArray = readEmpresas();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_empresa_empresas).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return lista;
}

QStringList Empresa::getListaNombresEmpresas()
{
    QJsonArray jsonArray = readEmpresas();
    QStringList lista;
    QString nombre;
    for (int i =0; i < jsonArray.size(); i++) {
        nombre = jsonArray.at(i).toObject().value(nombre_empresa_empresas).toString();
        if(!lista.contains(nombre)){
            lista.append(nombre);
        }
    }
    return lista;
}

bool Empresa::descargarPhoto(){
     bool retorno = false;
    if(checkIfFieldIsValid(nombre_foto) && !ui->le_empresa->text().trimmed().isEmpty()){
        QStringList keys, values;
        keys << "empresa" << "nombre_foto";
        values << ui->le_empresa->text().trimmed() << nombre_foto;

        int cant_fotos = 1;

        for(int i = 0, reintentos = 0; i < cant_fotos; i++)
        {
            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
            //OJO ESPERAR POR RESPUESTA
            QEventLoop q;

            connect(this, &Empresa::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;
            QTimer::singleShot(DELAY, this, SLOT(download_empresa_image_request()));

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

            case database_comunication::script_result::download_empresa_image_failed:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                    i = cant_fotos;
                }
                break;

            case database_comunication::script_result::download_empresa_image_picture_doesnt_exists:

                break;

            case database_comunication::script_result::ok:
                retorno = true;
                break;
            }
        }
    }
    return retorno;
}
bool Empresa::subirPhoto(){
    bool retorno = false;
    if(photoSelected && !ui->le_empresa->text().trimmed().isEmpty() && ui->lb_foto->pixmap() != nullptr)
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
        keys << "foto" << "empresa";
        values.clear();
        values << foto << ui->le_empresa->text().trimmed();

        for(int i = 0, reintentos = 0; i < 1; i++)
        {

            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                    this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

            QEventLoop q;

            connect(this, &Empresa::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;

            //ojo cambiar por upload
            QTimer::singleShot(DELAY, this, &Empresa::upload_empresa_image_request);

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

bool Empresa::subirEmpresa(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    empresa.insert(date_time_modified_empresas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(empresa);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Empresa::script_excecution_result,q,&QEventLoop::exit);

    if(getListaEmpresas().contains(codigo)){
        update_empresa_request(keys, values);
    }
    else{
        create_empresa_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::empresa_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_empresa_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_empresa_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    subirPhoto();
    return res;
}

void Empresa::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_EMPRESA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_empresa"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_empresa_to_server_failed;
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_empresa_to_server_failed;
            }
        }
        else
        {
            if(ba.contains("success ok update_empresa"))
            {
                result = database_comunication::script_result::empresa_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_EMPRESA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_empresa_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_empresa"))
            {
                result = database_comunication::script_result::empresa_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_EMPRESA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_empresa"))
        {
            result = database_comunication::script_result::delete_empresa_failed;

        }
        else
        {
            if(ba.contains("success ok delete_empresa"))
            {
                result = database_comunication::script_result::delete_empresa_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPLOAD_EMPRESA_IMAGE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success upload_image_empresa"))
        {
            result = database_comunication::script_result::upload_empresa_image_failed;

        }
        else
        {
            if(ba.contains("success ok upload_image_empresa"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    if(tipo == database_comunication::DOWNLOAD_EMPRESA_IMAGE)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success download_empresa_image"))
        {
            if(ba.contains("o se pudo obtener imagen de empresa"))
                result = database_comunication::script_result::download_empresa_image_failed;
            else if(ba.contains("o existe imagen de empresa"))
                result = database_comunication::script_result::download_empresa_image_picture_doesnt_exists;
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

void Empresa::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaEmpresas().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirEmpresa(cod_m);
    emit update_tableEmpresas(true);
    this->close();
}
void Empresa::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirEmpresa(cod_m);
    emit update_tableEmpresas(true);
    this->close();
}
void Empresa::on_pb_borrar_clicked()
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

        eliminarEmpresa(cod_m);
        //        subirTodasLasEmpresas();
        emit update_tableEmpresas(true);
        this->close();
    }
}
bool Empresa::eliminarEmpresa(QString codigo){
    //    subirTodasLasEmpresas();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    empresa.insert(date_time_modified_empresas, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(empresa);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "";
    QString temp = QString::fromUtf8(ba);
    values << temp;

    QEventLoop *q = new QEventLoop();

    connect(this, &Empresa::script_excecution_result,q,&QEventLoop::exit);

    if(getListaEmpresas().contains(codigo)){
        delete_empresa_request(keys, values);
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

    case database_comunication::script_result::delete_empresa_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_empresa_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Empresa::subirTodasLasEmpresas()
{
    QMap<QString, QString> mapaTiposDeEmpresa;
    mapaTiposDeEmpresa.insert("001", "CNEURO");
    mapaTiposDeEmpresa.insert("002", "GECONTA");


    for(int i=0; i< mapaTiposDeEmpresa.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeEmpresa.values().at(i);
        QString key = mapaTiposDeEmpresa.keys().at(i);
        jsonObject.insert(codigo_empresa_empresas, key);
        jsonObject.insert(empresa_empresas, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirEmpresa(cod);
    }
    emit update_tableEmpresas(true);
    this->close();
}
void Empresa::setGeoCode(const QString geocode)
{
    ui->le_geolocalizacion->setText(geocode);
}
void Empresa::eraseMarker()
{
    ui->le_geolocalizacion->setText("");
}
void Empresa::on_pb_geolocalizacion_clicked()
{
    Mapa_Zonas *mapa = new Mapa_Zonas(nullptr, ui->le_geolocalizacion->text());
    QObject::connect(mapa, &Mapa_Zonas::settedMarker, this, &Empresa::setGeoCode);
    QObject::connect(mapa, &Mapa_Zonas::erase_marker, this, &Empresa::eraseMarker);
    mapa->show();
}


void Empresa::on_le_empresa_editingFinished()
{
    if(ui->le_empresa->text().trimmed().isEmpty()){
        ui->lb_foto->setEnabled(false);
    }else{
        ui->lb_foto->setEnabled(true);
    }
}
