#include "gestor.h"
#include "ui_gestor.h"

#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "new_table_structure.h"
#include "screen_tabla_tareas.h"
#include "global_variables.h"
#include "globalfunctions.h"

Gestor::Gestor(QWidget *parent, bool newOne, QString empresa) :
    QWidget(parent),
    ui(new Ui::Gestor)
{
    ui->setupUi(this);
    this->setWindowTitle("Gestor");

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

    connect(ui->lb_foto, &MyLabelPhoto::selectedPhoto, this, &Gestor::selectedPhoto);
}

Gestor::~Gestor()
{
    delete ui;
}


QPixmap Gestor::getPhotoGestor(){
    if(descargarPhoto()){
        return logo;
    }
    return QPixmap();
}
bool Gestor::descargarPhoto(){
    bool retorno = false;
    if(checkIfFieldIsValid(nombre_foto) && checkIfFieldIsValid(ui->le_gestor->text()) && checkIfFieldIsValid(gestor.value(foto_gestores).toString())){
        QStringList keys, values;
        keys << "empresa" << "gestor" << "image_name";
        values << empresa << ui->le_gestor->text().trimmed() << gestor.value(foto_gestores).toString();

        int cant_fotos = 1;

        for(int i = 0, reintentos = 0; i < cant_fotos; i++)
        {
            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                    this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
            //OJO ESPERAR POR RESPUESTA
            QEventLoop q;

            connect(this, &Gestor::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;
            QTimer::singleShot(DELAY, this, &Gestor::download_gestor_image_request);

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

            case database_comunication::script_result::download_gestor_image_failed:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                    i = cant_fotos;
                }
                break;

            case database_comunication::script_result::download_gestor_image_picture_doesnt_exists:

                break;

            case database_comunication::script_result::ok:
                retorno = true;
                break;
            }
        }
    }
    return retorno;
}
void Gestor::selectedPhoto(QPixmap pixmap)
{
    scalePhoto(pixmap);
    this->photoSelected = true;

    savePhotoLocal(pixmap);
}
void Gestor::scalePhoto(QPixmap pixmap){
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
bool Gestor::loadPhotoLocal(){ //retorna true si la encontro
    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_gestor->text())){
        QString ruta = "C:/Mi_Ruta/Empresas/" + empresa + "/Gestores/" + ui->le_gestor->text().trimmed() + "/Logo";
        QString path = ruta + "/" + ui->le_gestor->text().trimmed()+"_logo" + ".jpg";
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
void Gestor::savePhotoLocal(QPixmap pixmap){
    QImage img = pixmap.toImage();
    //SALVAR EN PC

    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_gestor->text())){
        QString ruta = "C:/Mi_Ruta/Empresas/" + empresa + "/Gestores/" + ui->le_gestor->text().trimmed() + "/Logo";
        QDir dir(ruta);
        if(!dir.exists()){
            dir.mkpath(ruta);
        }
        img.save(ruta + "/" + ui->le_gestor->text().trimmed()+"_logo" + ".jpg");
    }
}
bool Gestor::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

bool Gestor::subirPhoto(){
    bool retorno = false;
    if(photoSelected && checkIfFieldIsValid(empresa) && checkIfFieldIsValid(ui->le_gestor->text()) && ui->lb_foto->pixmap() != nullptr)
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
        keys << "foto" << "empresa" << "gestor";
        values.clear();
        values << foto << empresa << ui->le_gestor->text().trimmed();

        for(int i = 0, reintentos = 0; i < 1; i++)
        {

            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                    this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

            QEventLoop q;

            connect(this, &Gestor::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;

            //ojo cambiar por upload
            QTimer::singleShot(DELAY, this, &Gestor::upload_gestor_image_request);

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

void Gestor::populateView(QJsonObject o)
{
    QString gestor_m, permisos_m, cod_m;
    gestor_m = o.value(gestor_gestores).toString();
    cod_m = o.value(codigo_gestor_gestores).toString();
    permisos_m = o.value(permisos_gestores).toString();
    nombre_foto = o.value(foto_gestores).toString();

    ui->le_codigo->setText(cod_m);
    ui->le_gestor->setText(gestor_m);
    ui->l_permisos->setText(permisos_m);

    if(checkIfFieldIsValid(gestor_m)){
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

QString Gestor::guardarDatos(){
    QString gestor_m, permisos_m, cod_m;
    gestor_m =  ui->le_gestor->text();
    cod_m = ui->le_codigo->text();
    permisos_m = ui->l_permisos->text();

    if(gestor_m != gestor.value(gestor_gestores).toString()){
        update_tareas = true;
        old_gestor = gestor.value(gestor_gestores).toString();
    }
    gestor.insert(gestor_gestores, gestor_m);
    gestor.insert(codigo_gestor_gestores, cod_m);
    gestor.insert(permisos_gestores, permisos_m);
    return cod_m;
}


void Gestor::create_gestor_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_GESTOR,keys,values);
}
void Gestor::update_gestor_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_GESTOR,keys,values);
}

void Gestor::delete_gestor_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_GESTOR,keys,values);
}
void Gestor::download_gestor_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_GESTOR_IMAGE,keys,values);
}
void Gestor::upload_gestor_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPLOAD_GESTOR_IMAGE,keys,values);
}
bool Gestor::writeGestores(QJsonArray jsonArray){
    QFile *data_base = new QFile(gestores_descargados); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonArray Gestor::readGestores(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(gestores_descargados); // ficheros .dat se puede utilizar formato txt tambien
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
QJsonObject Gestor::getGestorJsonObject(QString gest)
{
    QJsonArray jsonArray = readGestores();
    QString gest_l;
    for (int i =0; i < jsonArray.size(); i++) {
        gest_l = jsonArray.at(i).toObject().value(gestor_gestores).toString();
        if(gest_l == gest){
            return jsonArray.at(i).toObject();
        }
    }
    return QJsonObject();
}
QStringList Gestor::getListaNombresGestores(){
    QJsonArray jsonArray = readGestores();
    QStringList lista;
    QString gest;
    for (int i =0; i < jsonArray.size(); i++) {
        gest = jsonArray.at(i).toObject().value(gestor_gestores).toString();
        if(!lista.contains(gest)){
            lista.append(gest);
        }
    }
    return  lista;
}
QStringList Gestor::getListaGestores(){
    QJsonArray jsonArray = readGestores();
    QStringList lista;
    QString cod;
    for (int i =0; i < jsonArray.size(); i++) {
        cod = jsonArray.at(i).toObject().value(codigo_gestor_gestores).toString();
        if(!lista.contains(cod)){
            lista.append(cod);
        }
    }
    return  lista;
}
bool Gestor::subirGestor(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    gestor.insert(date_time_modified_gestores, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(gestor);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa;

    QEventLoop *q = new QEventLoop();

    connect(this, &Gestor::script_excecution_result,q,&QEventLoop::exit);

    if(getListaGestores().contains(codigo)){
        if(update_tareas){
            update_tareas = false;
            QStringList numInternos;
            QJsonObject campos;
            GlobalFunctions gf(this, empresa);
            numInternos = gf.getTareasList(GESTOR, old_gestor);
            campos.insert(GESTOR, gestor.value(
                              gestor_gestores).toString());
            campos.insert(date_time_modified, QDateTime::currentDateTime().toString(formato_fecha_hora));
            update_fields(numInternos, campos);
            emit updateTareas();
        }
        update_gestor_request(keys, values);
    }
    else{
        if(getListaNombresGestores().contains(
                    gestor.value(gestor_gestores).toString())){
            GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este gestor, introduzca otro");
            return false;
        }else{
            create_gestor_request(keys, values);
        }
    }

    bool res = true;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::gestor_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_gestor_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_gestor_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    subirPhoto();
    return res;
}
void Gestor::update_tareas_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA_FIELDS,keys,values);
}

bool Gestor::update_fields(QStringList numeros_internos_list, QJsonObject campos){
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

    connect(this, &Gestor::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Gestor::update_tareas_fields_request);

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

void Gestor::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_GESTOR)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_gestor"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_gestor_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_gestor_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_gestor"))
            {
                result = database_comunication::script_result::gestor_to_server_ok;
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
    else if(tipo == database_comunication::UPDATE_GESTOR)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_gestor_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_gestor"))
            {
                result = database_comunication::script_result::gestor_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_GESTOR)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_gestor"))
        {
            result = database_comunication::script_result::delete_gestor_failed;

        }
        else
        {
            if(ba.contains("success ok delete_gestor"))
            {
                result = database_comunication::script_result::delete_gestor_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPLOAD_GESTOR_IMAGE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success upload_gestor_image"))
        {
            result = database_comunication::script_result::upload_empresa_image_failed;

        }
        else
        {
            if(ba.contains("success ok upload_gestor_image"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    if(tipo == database_comunication::DOWNLOAD_GESTOR_IMAGE)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success download_gestor_image"))
        {
            if(ba.contains("o se pudo obtener imagen de gestor"))
                result = database_comunication::script_result::download_gestor_image_failed;
            else if(ba.contains("o existe imagen de gestor"))
                result = database_comunication::script_result::download_gestor_image_picture_doesnt_exists;
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
                logo = QPixmap::fromImage(temp_img);
                scalePhoto(logo);
                ui->lb_foto->setPixmap(logo);
                savePhotoLocal(logo);
            }
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}

void Gestor::on_pb_agregar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(getListaGestores().contains(cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    if(subirGestor(cod_m)){
        emit update_tableGestores(true);
        this->close();
    }
}
void Gestor::on_pb_actualizar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    if(subirGestor(cod_m)){
        emit update_tableGestores(true);
        this->close();
    }
}
void Gestor::on_pb_borrar_clicked()
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

        eliminarGestor(cod_m);
        //        subirTodasLasGestores();
        emit update_tableGestores(true);
        this->close();
    }


}
bool Gestor::eliminarGestor(QString codigo){
    //    subirTodasLasGestores();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    gestor.insert(date_time_modified_gestores, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(gestor);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop *q = new QEventLoop();

    connect(this, &Gestor::script_excecution_result,q,&QEventLoop::exit);

    if(getListaGestores().contains(codigo)){
        delete_gestor_request(keys, values);
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

    case database_comunication::script_result::delete_gestor_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_gestor_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void Gestor::subirTodasLasGestores()
{
    QMap<QString, QString> mapaTiposDeGestor;
    mapaTiposDeGestor.insert("001","TXORIERRI");
    mapaTiposDeGestor.insert("002","DURANGUESADO");
    mapaTiposDeGestor.insert("003","LEA-ARTIBAI");
    mapaTiposDeGestor.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeGestor.insert("005","ENCARTACIONES - GESTOR MINERA");
    mapaTiposDeGestor.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeGestor.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeGestor.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeGestor.values().at(i);
        QString key = mapaTiposDeGestor.keys().at(i);
        jsonObject.insert(codigo_gestor_gestores, key);
        jsonObject.insert(gestor_gestores, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirGestor(cod);
    }
    emit update_tableGestores(true);
    this->close();
}

void Gestor::on_le_gestor_editingFinished()
{
    if(checkIfFieldIsValid(ui->le_gestor->text())){
        ui->lb_foto->setEnabled(true);
    }else{
        ui->lb_foto->setEnabled(false);
    }
}
