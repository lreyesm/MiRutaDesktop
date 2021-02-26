#include "itac.h"
#include "ui_itac.h"

#include "itac.h"
#include "ui_itac.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QDateTime>
#include <QtCore>
#include <QMessageBox>
#include "mapa_zonas.h"
#include "screen_tabla_tareas.h"
#include "new_table_structure.h"
#include "zoompicture.h"
#include "zona.h"
#include "equipo_operario.h"
#include "operario.h"
#include "gestor.h"
#include <QGraphicsDropShadowEffect>
#include "global_variables.h"
#include "seccion_acceso_y_ubicacion.h"
#include "seccion_llaves_de_puertas.h"
#include "seccion_estado.h"
#include "seccion_estado_de_tuberias.h"
#include "seccion_estado_de_las_valvulas.h"
#include "resumen_estado_instalacion.h"
#include "ruta.h"
#include <QDesktopServices>
#include <QPdfWriter>
#include <QPainter>
#include "processesclass.h"
#include "QProgressIndicator.h"
#include "globalfunctions.h"
#include "mylabelshine.h"

ITAC::ITAC(QWidget *parent, bool newOne, QString empresa, QJsonObject jsonDefaultFields) :
    QWidget(parent),
    ui(new Ui::ITAC)
{
    ui->setupUi(this);
    this->setWindowTitle("ITAC");
    this->empresa = empresa;

    this->newOne = newOne;
    if(newOne){
        ui->pb_agregar->show();
        ui->pb_crear_pdf->hide();
        ui->pb_actualizar->hide();
        ui->pb_borrar->hide();
        ui->le_codigo->setText(jsonDefaultFields.value(codigo_itac_itacs).toString());
        ui->le_itac->setText(jsonDefaultFields.value(itac_itacs).toString());
        ui->le_zona->setText(jsonDefaultFields.value(zona_itacs).toString());
        ui->l_equipo->setText(jsonDefaultFields.value(equipo_itacs).toString());
        ui->l_operario->setText(jsonDefaultFields.value(operario_itacs).toString());
        ui->le_geolocalizacion->setText(jsonDefaultFields.value(geolocalizacion_itacs).toString());
        ui->l_prioridad->setText(jsonDefaultFields.value(prioridad_itacs).toString());

        QString gestor = jsonDefaultFields.value(gestor_itacs).toString();
        if(checkIfFieldIsValid(gestor) && gestor.toLower()!="todos"){
            //            qDebug()<<"gestor de itac"<<gestor<<"-------------------------------------------";
            ui->l_gestor->setText(gestor);
            ui->l_gestor->set_Enabled(false);
        }
    }else{
        ui->pb_agregar->hide();
        ui->pb_actualizar->show();
        ui->pb_crear_pdf->show();
        ui->pb_borrar->show();
    }
    connect(ui->lb_foto_1, &MyLabelPhoto::selectedPhotoName, this, &ITAC::selectedPhotoName);
    connect(ui->lb_foto_1, &MyLabelPhoto::selectedPhoto, this, &ITAC::selectedPhoto);
    connect(ui->lb_foto_2, &MyLabelPhoto::selectedPhotoName, this, &ITAC::selectedPhotoName);
    connect(ui->lb_foto_2, &MyLabelPhoto::selectedPhoto, this, &ITAC::selectedPhoto);
    connect(ui->lb_foto_3, &MyLabelPhoto::selectedPhotoName, this, &ITAC::selectedPhotoName);
    connect(ui->lb_foto_3, &MyLabelPhoto::selectedPhoto, this, &ITAC::selectedPhoto);
    connect(ui->lb_foto_4, &MyLabelPhoto::selectedPhotoName, this, &ITAC::selectedPhotoName);
    connect(ui->lb_foto_4, &MyLabelPhoto::selectedPhoto, this, &ITAC::selectedPhoto);
    connect(ui->lb_foto_5, &MyLabelPhoto::selectedPhotoName, this, &ITAC::selectedPhotoName);
    connect(ui->lb_foto_5, &MyLabelPhoto::selectedPhoto, this, &ITAC::selectedPhoto);
    connect(ui->lb_foto_6, &MyLabelPhoto::selectedPhotoName, this, &ITAC::selectedPhotoName);
    connect(ui->lb_foto_6, &MyLabelPhoto::selectedPhoto, this, &ITAC::selectedPhoto);
    connect(ui->lb_foto_7, &MyLabelPhoto::selectedPhotoName, this, &ITAC::selectedPhotoName);
    connect(ui->lb_foto_7, &MyLabelPhoto::selectedPhoto, this, &ITAC::selectedPhoto);
    connect(ui->lb_foto_8, &MyLabelPhoto::selectedPhotoName, this, &ITAC::selectedPhotoName);
    connect(ui->lb_foto_8, &MyLabelPhoto::selectedPhoto, this, &ITAC::selectedPhoto);
    connect(ui->lb_foto_9, &MyLabelPhoto::selectedPhotoName, this, &ITAC::selectedPhotoName);
    connect(ui->lb_foto_9, &MyLabelPhoto::selectedPhoto, this, &ITAC::selectedPhoto);

    initializeITAC();

    QStringList prioridades;
    prioridades << "MEDIA" << "BAJA" << "ALTA"<< "HIBERNAR";
    ui->l_prioridad->addItems(prioridades);

    ui->pb_play_audio->hide();

    this->setAttribute(Qt::WA_DeleteOnClose);
}

ITAC::~ITAC()
{
    delete ui;
}

QJsonObject ITAC::sumAllSections(QJsonObject itac_l){
    QString section1 = itac_l.value(puntuacion_seccion1_itacs).toString();
    QString section2 = itac_l.value(puntuacion_seccion2_itacs).toString();
    QString section3 = itac_l.value(puntuacion_seccion3_itacs).toString();
    QString section4 = itac_l.value(puntuacion_seccion4_itacs).toString();
    QString section5 = itac_l.value(puntuacion_seccion5_itacs).toString();

    QStringList sections;
    sections << section1 << section2 << section3 << section4 <<section5;
    float puntuacion = 0;
    QString section;
    foreach(section, sections){
        if(checkIfFieldIsValid(section)){
            bool ok;
            float value = section.toFloat(&ok);
            if(ok){
                puntuacion+= value;
            }
        }
    }
    itac_l.insert(puntuacion_itacs, QString::number(puntuacion, 'f', 2));
    return itac_l;
}
void ITAC::initializeITAC(){
    QStringList listaGestores, listaZonas,listaOperarios, listaEquiposOperarios;

    QJsonArray jsonArray = Gestor::readGestores();
    for (int i=0; i < jsonArray.size(); i++) {
        QString gestores_v, cod_v;
        gestores_v = jsonArray.at(i).toObject().value(gestor_gestores).toString();
        listaGestores.append(gestores_v);
    }
    ui->l_gestor->addItems(listaGestores);

    jsonArray = Equipo_Operario::readEquipo_Operarios();
    for (int i=0; i < jsonArray.size(); i++) {
        QString equipo_v, cod_v;
        equipo_v = jsonArray.at(i).toObject().value(equipo_operario_equipo_operarios).toString();
        listaEquiposOperarios.append(equipo_v);
    }
    ui->l_equipo->addItems(listaEquiposOperarios);

    jsonArray = Operario::readOperarios();
    for (int i=0; i < jsonArray.size(); i++) {
        QString operario_v, cod_v;
        operario_v = jsonArray.at(i).toObject().value(operario_operarios).toString();
        listaOperarios.append(operario_v);
    }
    ui->l_operario->addItems(listaOperarios);

    jsonArray = Zona::readZonas();
    for (int i=0; i < jsonArray.size(); i++) {
        QString zona_v, cod_v;
        zona_v = jsonArray.at(i).toObject().value(zona_zonas).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_zona_zonas).toString();

        listaZonas.append(cod_v + " - " + zona_v);
        listaZonas.sort(); //"" - menor a mayor
    }
    QCompleter *completer = new QCompleter(listaZonas, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    completer->setFilterMode(Qt::MatchContains);
    connect(completer, SIGNAL(highlighted(QString)), this, SLOT(onZonaSelected(QString)));
    ui->le_zona->setCompleter(completer);

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    ui->pb_acceso_ubicacion->setGraphicsEffect(effect);

    QGraphicsDropShadowEffect* effect2 = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect2->setBlurRadius(20);
    effect2->setOffset(1);
    effect2->setColor(color_blue_app);
    ui->pb_tuberias->setGraphicsEffect(effect2);

    QGraphicsDropShadowEffect* effect3 = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect3->setBlurRadius(20);
    effect3->setOffset(1);
    effect3->setColor(color_blue_app);
    ui->pb_valvulas->setGraphicsEffect(effect3);

    QGraphicsDropShadowEffect* effect4 = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect4->setBlurRadius(20);
    effect4->setOffset(1);
    effect4->setColor(color_blue_app);
    ui->pb_llaves_puertas->setGraphicsEffect(effect4);

    QGraphicsDropShadowEffect* effect5 = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect5->setBlurRadius(20);
    effect5->setOffset(1);
    effect5->setColor(color_blue_app);
    ui->pb_estado->setGraphicsEffect(effect5);
}

void ITAC::selectedPhoto(QPixmap pixmap)
{
    scalePhoto(pixmap, photoSelectedObjectName);
    this->photoSelected = true;

    savePhotoLocal(pixmap, photoSelectedObjectName);
}
void ITAC::selectedPhotoName(QString object_name)
{
    this->photoSelectedObjectName = object_name;
}
QPixmap ITAC::scalePhoto(QPixmap pixmap, int max_height, int max_width){
    QSize size = pixmap.size();
    double ratio;
    if(size.width() > size.height()){
        ratio = static_cast<double>(size.height())/ static_cast<double>(size.width());
        max_height = max_width * ratio;
    }else{
        ratio = static_cast<double>(size.width())/ static_cast<double>(size.height());
        max_width = max_height * ratio;
    }
    return pixmap.scaled(max_width, max_height);
}
void ITAC::scalePhoto(QPixmap pixmap, QString object_name){
    QSize size = pixmap.size();
    int max_height = 150;
    int max_width = 150;
    double ratio;
    if(size.width() > size.height()){
        ratio = static_cast<double>(size.height())/ static_cast<double>(size.width());
        max_height = max_width * ratio;
    }else{
        ratio = static_cast<double>(size.width())/ static_cast<double>(size.height());
        max_width = max_height * ratio;
    }
    QLabel *label = ui->scrollArea->findChild<QLabel*>(object_name);
    if(label){
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
        effect->setBlurRadius(20);
        effect->setOffset(1);
        effect->setColor(color_blue_app);
        label->setGraphicsEffect(effect);

        label->setScaledContents(true);
        label->setMinimumSize(max_width, max_height);
        label->setMaximumSize(max_width, max_height);
    }
}
bool ITAC::loadPhotoLocal(int currentPhoto){ //retorna true si la encontro
    bool retorno = false;
    if(currentPhoto > 0 && currentPhoto < 10){
        QString cod_emplazamiento = ui->le_codigo->text().trimmed();
        QString gestor = ui->l_gestor->currentText().trimmed();
        if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(cod_emplazamiento)){
            QString ruta = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/" + gestor + "/fotos_ITACs/" + cod_emplazamiento;

            QString path = ruta +"/"+cod_emplazamiento + "_foto_"+ QString::number(currentPhoto) + ".jpg";
            if(QFile::exists(path)){
                MyLabelPhoto *label = ui->scrollArea->findChild<MyLabelPhoto*>("lb_foto_" + QString::number(currentPhoto));
                if(label){
                    if(QFile::exists(path)){
                        scalePhoto(QPixmap(path), label->objectName());
                        label->setPixmap(QPixmap(path));
                        retorno = true;
                    }
                }
            }
        }
    }
    return retorno;
}
bool ITAC::loadPhotoLocal(){ //retorna true si la encontro
    bool retorno = false;
    QString cod_emplazamiento = ui->le_codigo->text().trimmed();
    QString gestor = ui->l_gestor->currentText().trimmed();
    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(cod_emplazamiento)){
        QString ruta = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/" + gestor + "/fotos_ITACs/" + cod_emplazamiento;
        for (int i=1; i<= 9; i++) {
            QString path = ruta +"/"+cod_emplazamiento + "_foto_"+ QString::number(i) + ".jpg";
            if(QFile::exists(path)){
                MyLabelPhoto *label = ui->scrollArea->findChild<MyLabelPhoto*>("lb_foto_" + QString::number(i));
                if(label){
                    if(QFile::exists(path)){
                        scalePhoto(QPixmap(path), label->objectName());
                        label->setPixmap(QPixmap(path));
                        retorno = true;
                    }
                }
            }
        }
    }
    return retorno;
}
bool ITAC::savePhotoLocal(QPixmap pixmap, QString name){
    QImage img = pixmap.toImage();
    //SALVAR EN PC
    QString cod_emplazamiento = ui->le_codigo->text().trimmed();
    QString gestor = ui->l_gestor->currentText().trimmed();
    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(cod_emplazamiento)){
        QString ruta = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/" + gestor + "/fotos_ITACs/" + cod_emplazamiento;
        QDir dir(ruta);
        if(!dir.exists()){
            dir.mkpath(ruta);
        }
        QString fullPath = ruta + "/" + cod_emplazamiento + "_foto_" + name.at(name.length() - 1) + ".jpg";
        if(img.save(fullPath)){
            return true;
        }
    }
    return false;
}
bool ITAC::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

void ITAC::populateView(QJsonObject o, bool downloadPhotos)
{
    ui->pb_actualizar->setEnabled(false);

    QString nombre_firmante, carnet_firmante, zona_m, equipo_m, operaio_m, gestor_m, itac_m, geolocalizacion_m, cod_m, direccion_oficina,
            acceso, descripcion, nombre_empresa_administracion,
            nombre_responsable_administracion, telefono_fijo_administracion,
            telefono_movil_administracion, correo_administracion,
            nombre_presidente, vivienda_presidente, telefono_fijo_presidente, telefono_movil_presidente, correo_presidente,
            nombre_encargado, vivienda_encargado, telefono_fijo_encargado, telefono_movil_encargado, correo_encargado,
            descripcion_foto_1, descripcion_foto_2, descripcion_foto_3, descripcion_foto_4,
            descripcion_foto_5, descripcion_foto_6, descripcion_foto_7, descripcion_foto_8, prioridad;


    nombres_fotos.append(o.value(foto_1_itacs).toString());
    nombres_fotos.append(o.value(foto_2_itacs).toString());
    nombres_fotos.append(o.value(foto_3_itacs).toString());
    nombres_fotos.append(o.value(foto_4_itacs).toString());
    nombres_fotos.append(o.value(foto_5_itacs).toString());
    nombres_fotos.append(o.value(foto_6_itacs).toString());
    nombres_fotos.append(o.value(foto_7_itacs).toString());
    nombres_fotos.append(o.value(foto_8_itacs).toString());
    nombres_fotos.append(o.value(foto_9_itacs).toString());

    nombre_firmante = o.value(nombre_firmante_itacs).toString().trimmed();// gestor
    carnet_firmante = o.value(carnet_firmante_itacs).toString().trimmed();// gestor

    zona_m = o.value(zona_itacs).toString().trimmed();// gestor
    operaio_m = o.value(operario_itacs).toString().trimmed();// gestor
    equipo_m = o.value(equipo_itacs).toString().trimmed();// gestor

    gestor_m = o.value(gestor_itacs).toString().trimmed();// gestor
    itac_m = o.value(itac_itacs).toString().trimmed();//identificador
    cod_m = o.value(codigo_itac_itacs).toString().trimmed();// cod emplazamiento

    geolocalizacion_m = o.value(geolocalizacion_itacs).toString().trimmed();
    acceso = o.value(acceso_itacs).toString();
    descripcion = o.value(descripcion_itacs).toString();
    nombre_empresa_administracion = o.value(nombre_empresa_administracion_itacs).toString();
    nombre_responsable_administracion = o.value(nombre_responsable_administracion_itacs).toString();
    telefono_fijo_administracion = o.value(telefono_fijo_administracion_itacs).toString();
    telefono_movil_administracion = o.value(telefono_movil_administracion_itacs).toString();
    direccion_oficina = o.value(direccion_oficina_administracion_itacs).toString();
    correo_administracion = o.value(correo_administracion_itacs).toString();
    nombre_presidente = o.value(nombre_presidente_itacs).toString();
    vivienda_presidente = o.value(vivienda_presidente_itacs).toString();
    telefono_fijo_presidente = o.value(telefono_fijo_presidente_itacs).toString();
    telefono_movil_presidente = o.value(telefono_movil_presidente_itacs).toString();
    correo_presidente = o.value(correo_presidente_itacs).toString();
    nombre_encargado = o.value(nombre_encargado_itacs).toString();
    vivienda_encargado = o.value(vivienda_encargado_itacs).toString();
    telefono_fijo_encargado = o.value(telefono_fijo_encargado_itacs).toString();
    telefono_movil_encargado = o.value(telefono_movil_encargado_itacs).toString();
    correo_encargado = o.value(correo_encargado_itacs).toString();

    descripcion_foto_1 = o.value(descripcion_foto_1_itacs).toString();
    descripcion_foto_2 = o.value(descripcion_foto_2_itacs).toString();
    descripcion_foto_3 = o.value(descripcion_foto_3_itacs).toString();
    descripcion_foto_4 = o.value(descripcion_foto_4_itacs).toString();
    descripcion_foto_5 = o.value(descripcion_foto_5_itacs).toString();
    descripcion_foto_6 = o.value(descripcion_foto_6_itacs).toString();
    descripcion_foto_7 = o.value(descripcion_foto_7_itacs).toString();
    descripcion_foto_8 = o.value(descripcion_foto_8_itacs).toString();

    prioridad = o.value(prioridad_itacs).toString().trimmed();

    if(checkIfFieldIsValid(prioridad)){
        ui->l_prioridad->setText(prioridad);
    }else{
        ui->l_prioridad->setText("BAJA");
    }

    if(checkIfFieldIsValid(gestor_m)){
        ui->l_gestor->setText(gestor_m);
        ui->l_gestor->set_Enabled(false);
    }
    ui->le_codigo->setText(cod_m);
    ui->le_itac->setText(itac_m);

    ui->le_nombre_firmante->setText(nombre_firmante);
    ui->le_carnet_firmante->setText(carnet_firmante);

    ui->l_equipo->setText(equipo_m);
    ui->l_operario->setText(operaio_m);
    ui->le_zona->setText(zona_m);

    ui->le_geolocalizacion->setText(geolocalizacion_m);
    ui->pt_acceso->setPlainText(acceso);
    ui->pt_descripcion_general->setPlainText(descripcion);
    ui->le_nombre_empresa->setText(nombre_empresa_administracion);
    ui->le_nombre_responsable->setText(nombre_responsable_administracion);
    ui->le_telefono_fijo_empresa->setText(telefono_fijo_administracion);
    ui->le_telefono_movil_empresa->setText(telefono_movil_administracion);
    ui->le_direccion_de_oficina->setText(direccion_oficina);
    ui->le_correo_empresa->setText(correo_administracion);

    ui->le_nombre_presidente->setText(nombre_presidente);
    ui->le_vivienda_presidente->setText(vivienda_presidente);
    ui->le_telefono_fijo_presidente->setText(telefono_fijo_presidente);
    ui->le_telefono_movil_presidente->setText(telefono_movil_presidente);
    ui->le_correo_presidente->setText(correo_presidente);

    ui->le_nombre_conserje->setText(nombre_encargado);
    ui->le_vivienda_encargado->setText(vivienda_encargado);
    ui->le_telefono_fijo_conserje->setText(telefono_fijo_encargado);
    ui->le_telefono_movil_conserje->setText(telefono_movil_encargado);
    ui->le_correo_conserje->setText(correo_encargado);

    ui->pt_descripcion_foto_1->setPlainText(descripcion_foto_1);
    ui->pt_descripcion_foto_2->setPlainText(descripcion_foto_2);
    ui->pt_descripcion_foto_3->setPlainText(descripcion_foto_3);
    ui->pt_descripcion_foto_4->setPlainText(descripcion_foto_4);
    ui->pt_descripcion_foto_5->setPlainText(descripcion_foto_5);
    ui->pt_descripcion_foto_6->setPlainText(descripcion_foto_6);
    ui->pt_descripcion_foto_7->setPlainText(descripcion_foto_7);
    ui->pt_descripcion_foto_8->setPlainText(descripcion_foto_8);

    if(checkIfFieldIsValid(cod_m)){
        ui->lb_foto_1->setEnabled(true);
        ui->lb_foto_2->setEnabled(true);
        ui->lb_foto_3->setEnabled(true);
        ui->lb_foto_4->setEnabled(true);
        ui->lb_foto_5->setEnabled(true);
        ui->lb_foto_6->setEnabled(true);
        ui->lb_foto_7->setEnabled(true);
        ui->lb_foto_8->setEnabled(true);
        ui->lb_foto_9->setEnabled(true);
    }else{
        ui->lb_foto_1->setEnabled(false);
        ui->lb_foto_2->setEnabled(false);
        ui->lb_foto_3->setEnabled(false);
        ui->lb_foto_4->setEnabled(false);
        ui->lb_foto_5->setEnabled(false);
        ui->lb_foto_6->setEnabled(false);
        ui->lb_foto_7->setEnabled(false);
        ui->lb_foto_8->setEnabled(false);
        ui->lb_foto_9->setEnabled(false);
    }

    if(downloadPhotos){
        loadPhotoLocal();//si hay foto local
        checkDisponibleAudio();


        if(!descargarPhoto()){ //si no hay foto en servidor
            photoSelected = true; //subo foto al actualizar
        }
        if(descargarAudio()){
            ui->pb_play_audio->show();
        }
    }
    ui->pb_actualizar->setEnabled(true);
}
bool ITAC::checkDisponibleAudio()
{
    QString audio = itac.value(audio_detalle).toString().trimmed();

    if(checkIfFieldIsValid(audio)){
        QString audio_dir_file = getLocalDirofCurrentItac() + "/" + audio;
        QFile file(audio_dir_file);
        if(file.exists()){
            ui->pb_play_audio->show();
            return true;
        }else{
            ui->pb_play_audio->hide();
        }
    }
    return false;
}
bool ITAC::descargarPhoto(){
    bool retorno = true;
    QString cod_emplazamiento = ui->le_codigo->text().trimmed();
    QString gestor = ui->l_gestor->currentText().trimmed();
    for (int i=1; i<= 9;i++) {
        if(checkIfFieldIsValid(nombres_fotos.at(i-1)) && checkIfFieldIsValid(ui->le_codigo->text())){
            descargando_foto_x = i;
            QStringList keys, values;
            keys << "GESTOR"<< "codigo_emplazamiento" << "nombre_foto"<< "empresa";
            values << gestor << cod_emplazamiento << cod_emplazamiento+"_foto_"+QString::number(i)+".jpg" << empresa;

            MyLabelPhoto *label = ui->scrollArea->findChild<MyLabelPhoto*>("lb_foto_" + QString::number(descargando_foto_x));
            displayLoading(label);

            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                    this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
            //OJO ESPERAR POR RESPUESTA
            QEventLoop q;

            connect(this, &ITAC::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;
            QTimer::singleShot(DELAY, this, &ITAC::download_itac_image_request);

            switch (q.exec())
            {
            case database_comunication::script_result::timeout:
                emit hidingLoading();
                label->setDefaultPhoto(true);
                loadPhotoLocal(i);
                retorno = false;
                break;

            case database_comunication::script_result::download_itac_image_failed:
                emit hidingLoading();
                label->setDefaultPhoto(true);
                loadPhotoLocal(i);
                retorno = false;
                break;

            case database_comunication::script_result::download_itac_image_picture_doesnt_exists:
                emit hidingLoading();
                label->setDefaultPhoto(true);
                loadPhotoLocal(i);
                retorno = false;
                break;

            case database_comunication::script_result::ok:
                emit hidingLoading();
                break;
            }
        }
    }
    return retorno;
}
bool ITAC::subirPhoto(){
    bool retorno = false;
    QString cod_emplazamiento = ui->le_codigo->text().trimmed();
    QString gestor = ui->l_gestor->currentText().trimmed();
    for (int i=1; i<= 9;i++) {
        MyLabelPhoto *label = static_cast<MyLabelPhoto*>(
                    ui->scrollArea->findChild<QLabel*>("lb_foto_" + QString::number(i)));
        if(label){
            retorno = true;
            if((photoSelected || changedCodeItac) && checkIfFieldIsValid(empresa)
                    && checkIfFieldIsValid(cod_emplazamiento)
                    && label->pixmap() != nullptr && (!label->isDefaultPhoto() &&
                                                      (label->changedPhoto || changedCodeItac)))
            {
                QImage image;
                QByteArray byteArray;
                QString foto;
                QBuffer *buffer;
                QStringList keys, values;

                image = label->pixmap()->toImage();
                buffer = new QBuffer(&byteArray);
                image.save(buffer,"jpg");
                foto = QString::fromLatin1(byteArray.toBase64().data());
                delete buffer;

                keys.clear();
                keys << "foto"<< "GESTOR"<< "codigo_emplazamiento" << "campo_foto" << "nombre_foto"<< "empresa";
                values.clear();
                values << foto << gestor << cod_emplazamiento << "foto_" + QString::number(i)
                       << cod_emplazamiento +"_foto_" + QString::number(i) + ".jpg" << empresa;

                for(int i = 0, reintentos = 0; i < 1; i++)
                {
                    QEventLoop q;

                    connect(this, &ITAC::script_excecution_result,&q,&QEventLoop::exit);

                    this->keys = keys;
                    this->values = values;

                    //ojo cambiar por upload
                    QTimer::singleShot(DELAY, this, &ITAC::upload_itac_image_request);

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
                            retorno = false;
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
                            retorno = false;
                        }
                        break;
                    case database_comunication::script_result::ok:
                        break;
                    }
                }
            }
        }
    }
    return retorno;
}

QString ITAC::guardarDatos(){
    QString nombre_firmante, carnet_firmante, zona_m, equipo_m, operaio_m, gestor_m, itac_m, geolocalizacion_m, cod_m, direccion_oficina,
            acceso, descripcion, nombre_empresa_administracion,
            nombre_responsable_administracion, telefono_fijo_administracion,
            telefono_movil_administracion, correo_administracion,
            nombre_presidente, vivienda_presidente, telefono_fijo_presidente, telefono_movil_presidente, correo_presidente,
            nombre_encargado, vivienda_encargado, telefono_fijo_encargado, telefono_movil_encargado, correo_encargado,
            descripcion_foto_1, descripcion_foto_2, descripcion_foto_3, descripcion_foto_4,
            descripcion_foto_5, descripcion_foto_6, descripcion_foto_7, descripcion_foto_8, prioridad;

    nombre_firmante = ui->le_nombre_firmante->text().trimmed();
    carnet_firmante = ui->le_carnet_firmante->text().trimmed();

    zona_m = ui->le_zona->text().trimmed();
    equipo_m =  ui->l_equipo->text().trimmed();
    operaio_m = ui->l_operario->text().trimmed();

    gestor_m = ui->l_gestor->text().trimmed();
    itac_m =  ui->le_itac->text().trimmed();
    cod_m = ui->le_codigo->text().trimmed();

    geolocalizacion_m = ui->le_geolocalizacion->text().trimmed();
    acceso = ui->pt_acceso->toPlainText();
    descripcion = ui->pt_descripcion_general->toPlainText();
    nombre_empresa_administracion = ui->le_nombre_empresa->text();
    nombre_responsable_administracion = ui->le_nombre_responsable->text();
    telefono_fijo_administracion = ui->le_telefono_fijo_empresa->text();
    telefono_movil_administracion = ui->le_telefono_movil_empresa->text();
    direccion_oficina = ui->le_direccion_de_oficina->text();
    correo_administracion = ui->le_correo_empresa->text();
    nombre_presidente = ui->le_nombre_presidente->text();
    vivienda_presidente = ui->le_vivienda_presidente->text();
    telefono_fijo_presidente = ui->le_telefono_fijo_presidente->text();
    telefono_movil_presidente = ui->le_telefono_movil_presidente->text();
    correo_presidente = ui->le_correo_presidente->text();
    nombre_encargado = ui->le_nombre_conserje->text();
    vivienda_encargado = ui->le_vivienda_encargado->text();
    telefono_fijo_encargado = ui->le_telefono_fijo_conserje->text();
    telefono_movil_encargado = ui->le_telefono_movil_conserje->text();
    correo_encargado = ui->le_correo_conserje->text();

    descripcion_foto_1 = ui->pt_descripcion_foto_1->toPlainText();
    descripcion_foto_2 = ui->pt_descripcion_foto_2->toPlainText();
    descripcion_foto_3 = ui->pt_descripcion_foto_3->toPlainText();
    descripcion_foto_4 = ui->pt_descripcion_foto_4->toPlainText();
    descripcion_foto_5 = ui->pt_descripcion_foto_5->toPlainText();
    descripcion_foto_6 = ui->pt_descripcion_foto_6->toPlainText();
    descripcion_foto_7 = ui->pt_descripcion_foto_7->toPlainText();
    descripcion_foto_8 = ui->pt_descripcion_foto_8->toPlainText();

    prioridad = ui->l_prioridad->currentText();

    itac.insert(ultima_modificacion_itacs, "ESCRITORIO "+other_task_screen::administrator_loged);

    itac.insert(prioridad_itacs, prioridad);

    itac.insert(gestor_itacs, gestor_m);
    itac.insert(itac_itacs, itac_m);
    itac.insert(codigo_itac_itacs, cod_m);


    itac.insert(carnet_firmante_itacs, carnet_firmante);
    itac.insert(nombre_firmante_itacs, nombre_firmante);

    itac.insert(equipo_itacs, equipo_m);
    itac.insert(zona_itacs, zona_m);
    itac.insert(operario_itacs, operaio_m);

    itac.insert(geolocalizacion_itacs, geolocalizacion_m);
    itac.insert(acceso_itacs, acceso);
    itac.insert(descripcion_itacs, descripcion);
    itac.insert(nombre_empresa_administracion_itacs, nombre_empresa_administracion);
    itac.insert(nombre_responsable_administracion_itacs, nombre_responsable_administracion);
    itac.insert(telefono_fijo_administracion_itacs, telefono_fijo_administracion);
    itac.insert(telefono_movil_administracion_itacs, telefono_movil_administracion);
    itac.insert(direccion_oficina_administracion_itacs, direccion_oficina);
    itac.insert(correo_administracion_itacs, correo_administracion);
    itac.insert(nombre_presidente_itacs, nombre_presidente);
    itac.insert(vivienda_presidente_itacs, vivienda_presidente);
    itac.insert(telefono_fijo_presidente_itacs, telefono_fijo_presidente);
    itac.insert(telefono_movil_presidente_itacs, telefono_movil_presidente);
    itac.insert(correo_presidente_itacs, correo_presidente);
    itac.insert(nombre_encargado_itacs, nombre_encargado);
    itac.insert(vivienda_encargado_itacs, vivienda_encargado);
    itac.insert(telefono_fijo_encargado_itacs, telefono_fijo_encargado);
    itac.insert(telefono_movil_encargado_itacs, telefono_movil_encargado);
    itac.insert(correo_encargado_itacs, correo_encargado);

    itac.insert(descripcion_foto_1_itacs, descripcion_foto_1);
    itac.insert(descripcion_foto_2_itacs, descripcion_foto_2);
    itac.insert(descripcion_foto_3_itacs, descripcion_foto_3);
    itac.insert(descripcion_foto_4_itacs, descripcion_foto_4);
    itac.insert(descripcion_foto_5_itacs, descripcion_foto_5);
    itac.insert(descripcion_foto_6_itacs, descripcion_foto_6);
    itac.insert(descripcion_foto_7_itacs, descripcion_foto_7);
    itac.insert(descripcion_foto_8_itacs, descripcion_foto_8);

    itac = sumAllSections(itac);

    return cod_m;
}


void ITAC::create_itac_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_ITAC,keys,values);
}
void ITAC::update_itac_with_id_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_ITAC_WITH_ID,keys,values);
}

void ITAC::update_itac_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_ITAC,keys,values);
}

void ITAC::delete_itac_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_ITAC,keys,values);
}
void ITAC::delete_itac_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_ITAC,keys,values);
}
void ITAC::download_itac_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_ITAC_IMAGE,keys,values);
}
void ITAC::update_tareas_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA_FIELDS,keys,values);
}
void ITAC::upload_itac_image_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPLOAD_ITAC_IMAGE,keys,values);
}

bool ITAC::writeITACs(QJsonArray jsonArray){
    QFile *data_base = new QFile(itacs_descargadas); // ficheros .dat se puede utilizar formato txt tambien
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

QJsonObject ITAC::getITACJsonObjectFromCode(QString codigo){
    GlobalFunctions gf(nullptr, GlobalFunctions::readEmpresaSelected());
    QJsonObject jsonObject = gf.getItacFromServer(codigo_itac_itacs, codigo);
    return  QJsonObject();
}
QString ITAC::getGeoCodeFromCodeItac(QString codigo){
    QJsonObject jsonObject = getITACJsonObjectFromCode(codigo);
    if(jsonObject.isEmpty()){
        return "";
    }else{
        return jsonObject.value(geolocalizacion_itacs).toString();
    }
}
QString ITAC::formatCodeEmplazamiento(QString cod){
    if(cod.contains(".")){
        if(cod.split(".").size() > 1){
            QString ruta = cod.split(".").at(0).trimmed();
            QString portal = cod.split(".").at(1).trimmed();
            while(ruta.size() < 6){
                ruta.prepend("0");
            }
            return ruta + "." + portal;
        }
    }
    return cod;
}
QStringList ITAC::getListaITACs(){
    GlobalFunctions gf(nullptr, GlobalFunctions::readEmpresaSelected());
    QStringList lista = gf.getItacsList();
    return lista;
}

bool ITAC::uploadItac(QString codigo) //para subirlos de txt
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    itac.insert(date_time_modified_itacs, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(itac);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop *q = new QEventLoop();

    connect(this, &ITAC::script_excecution_result,q,&QEventLoop::exit);

    GlobalFunctions gf(this, empresa);
    if(gf.checkIfItacExist(codigo_itac_itacs, codigo)){
        update_itac_request(keys, values);
    }
    else{
        create_itac_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::itac_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_itac_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_itac_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
bool ITAC::subirITAC(QString codigo){

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    itac.insert(date_time_modified_itacs, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(itac);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    QEventLoop *q = new QEventLoop();

    connect(this, &ITAC::script_excecution_result,q,&QEventLoop::exit);

    if(changedCodeItac){
        update_itac_with_id_request(keys, values);
    }else{
        GlobalFunctions gf(this, empresa);
        if(gf.checkIfItacExist(codigo_itac_itacs, codigo)){
            update_itac_request(keys, values);
        }
        else{
            create_itac_request(keys, values);
        }
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::itac_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_itac_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_itac_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    subirPhoto();
//    if(geo_modified){
        updateTareas();
        emit changedGeoCode(ui->le_geolocalizacion->text().trimmed());
        emit updateTableTareas();
//    }
    return res;
}

void ITAC::closeEvent(QCloseEvent *event)
{
    emit closing();
    QWidget::closeEvent(event);
}
bool ITAC::updateTareas(){
    QString numIn, geoCode = ui->le_geolocalizacion->text().trimmed();
    QString cod_emplazamiento_itac = ui->le_codigo->text().trimmed();
    QStringList numInternos;
    QJsonObject numeros_internos;
    QJsonObject campos;
    QJsonObject tarea;
    QString cod_emplazamiento_tarea;

    GlobalFunctions gf(this, empresa);
    numInternos = gf.getTareasList(codigo_de_geolocalizacion, cod_emplazamiento_itac);

    for (int i=0; i < numInternos.size(); i++) {
        numIn = numInternos.at(i);
        numeros_internos.insert(QString::number(i), numIn);
    }
    if(numeros_internos.isEmpty()){
        return true;
    }
    campos.insert(url_geolocalizacion, "https://maps.google.com/?q="+geoCode);
    campos.insert(codigo_de_localizacion, geoCode);
    campos.insert(geolocalizacion, geoCode);
    campos.insert(date_time_modified, QDateTime::currentDateTime().toString(formato_fecha_hora));

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

    connect(this, &ITAC::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &ITAC::update_tareas_fields_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::update_tareas_fields_to_server_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}
void ITAC::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
        //GlobalFunctions::showWarning(this,"Información del servidor actualizada","Entro: tipo -> "+ QString::number(tipo)+"Respuesta: "+ respuesta);
    //    ui->plainTextEdit->setPlainText(respuesta);
    if(tipo == database_comunication::CREATE_ITAC)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success create_itac"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_itac_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_itac_to_server_failed;
                qDebug()<<"Revisar si los campos de MySQL tienen valor por defecto NULL";
            }
        }
        else
        {
            if(ba.contains("success ok update_itac"))
            {
                result = database_comunication::script_result::itac_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_ITAC)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_itac_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_itac"))
            {
                result = database_comunication::script_result::itac_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_ITAC_WITH_ID){
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_itac_to_server_failed;
        }
        else
        {
            if(ba.contains("success ok update_itac"))
            {
                result = database_comunication::script_result::itac_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_ITAC)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success delete_itac"))
        {
            result = database_comunication::script_result::delete_itac_failed;

        }
        else
        {
            if(ba.contains("success ok delete_itac"))
            {
                result = database_comunication::script_result::delete_itac_ok;
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
    else if(tipo == database_comunication::UPLOAD_ITAC_IMAGE)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success upload_itac_image"))
        {
            result = database_comunication::script_result::upload_empresa_image_failed;
        }
        else
        {
            if(ba.contains("success ok upload_itac_image"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    else if(tipo == database_comunication::DOWNLOAD_ITAC_IMAGE)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        //        qDebug()<<respuesta;
        if(ba.contains("ot success download_itac_image"))
        {
            if(ba.contains("o se pudo obtener imagen de itac"))
                result = database_comunication::script_result::download_itac_image_failed;
            else if(ba.contains("o existe imagen de itac"))
                result = database_comunication::script_result::download_itac_image_picture_doesnt_exists;
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
                if(descargando_foto_x != 0){
                    MyLabelPhoto *label = ui->scrollArea->findChild<MyLabelPhoto*>("lb_foto_" + QString::number(descargando_foto_x));
                    scalePhoto(pixmap, label->objectName());
                    label->setPixmap(pixmap);
                    savePhotoLocal(pixmap, label->objectName());
                }
            }
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}

void ITAC::on_pb_agregar_clicked()
{
    show_loading("Creando ITAC...");
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty() && !this->isHidden()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    GlobalFunctions gf(this, empresa);
    if(gf.checkIfItacExist(codigo_itac_itacs, cod_m)){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Ya existe","Ya existe este codigo, introduzca otro");
        return;
    }
    subirITAC(cod_m);
    emit update_tableITACs(true);
    hide_loading();
    this->close();
}
void ITAC::on_pb_actualizar_clicked()
{
    show_loading("Actualizando...");
    ui->pb_actualizar->setEnabled(false);
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty() && !this->isHidden()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    subirITAC(cod_m);
    emit update_tableITACs(true);
    hide_loading();
    this->close();
}
void ITAC::on_pb_borrar_clicked()
{
    QString cod_m = guardarDatos();
    if(cod_m.isEmpty() && !this->isHidden()){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Sin codigo","El codigo no puede estar vacio, por favor introduzca codigo");
        return;
    }
    GlobalFunctions gf(this, empresa);
    if(gf.showQuestion(this, "Confirmacion", "Seguro desea eliminar el modelo?",
                             QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){

        show_loading("Eliminando...");
        eliminarITAC(cod_m);
        emit update_tableITACs(true);
        hide_loading();
        this->close();
    }
}
bool ITAC::eliminarITAC(QString codigo){
    //    subirTodasLasITACs();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    itac.insert(date_time_modified_itacs, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(itac);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &ITAC::script_excecution_result,q,&QEventLoop::exit);

    GlobalFunctions gf(this, empresa);
    if(gf.checkIfItacExist(codigo_itac_itacs, codigo)){
        QTimer::singleShot(DELAY, this, SLOT(delete_itac_request()));
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

    case database_comunication::script_result::delete_itac_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::delete_itac_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    return res;
}
void ITAC::subirTodasLasITACs()
{
    QMap<QString, QString> mapaTiposDeITAC;
    mapaTiposDeITAC.insert("001","TXORIERRI");
    mapaTiposDeITAC.insert("002","DURANGUESADO");
    mapaTiposDeITAC.insert("003","LEA-ARTIBAI");
    mapaTiposDeITAC.insert("004","MUNGIALDE"); //preguntar por este
    mapaTiposDeITAC.insert("005","ENCARTACIONES - itac MINERA");
    mapaTiposDeITAC.insert("006","ARRATIA-NERBIOI");
    mapaTiposDeITAC.insert("007","MARGEN IZQUIERDA" );


    for(int i=0; i< mapaTiposDeITAC.size();i++){
        QJsonObject jsonObject;
        QString value = mapaTiposDeITAC.values().at(i);
        QString key = mapaTiposDeITAC.keys().at(i);
        jsonObject.insert(codigo_itac_itacs, key);
        jsonObject.insert(itac_itacs, value);
        this->setData(jsonObject);
        QString cod = guardarDatos();
        subirITAC(cod);
    }
    emit update_tableITACs(true);
    this->close();
}

void ITAC::setGeoCode(const QString geocode)
{
    ui->le_geolocalizacion->setText(geocode);
    geo_modified = true;
}
void ITAC::eraseMarker()
{
    ui->le_geolocalizacion->setText("");
    geo_modified = true;
}
void ITAC::on_pb_geolocalizacion_clicked()
{
    Mapa_Zonas *mapa = new Mapa_Zonas(nullptr, ui->le_geolocalizacion->text());
    QObject::connect(mapa, &Mapa_Zonas::settedMarker, this, &ITAC::setGeoCode);
    QObject::connect(mapa, &Mapa_Zonas::erase_marker, this, &ITAC::eraseMarker);
    mapa->show();
}

void ITAC::on_pb_foto_1_clicked()
{
    if(ui->lb_foto_1->pixmap()!=nullptr){
        ZoomPicture *zoom = new ZoomPicture(nullptr, *ui->lb_foto_1->pixmap());
        connect(zoom, &ZoomPicture::rotate, ui->lb_foto_1, &MyLabelPhoto::photoRotated);
        zoom->setTextInfo(ui->pt_descripcion_foto_1->toPlainText());
        zoom->show();
    }
}
void ITAC::on_pb_foto_2_clicked()
{
    if(ui->lb_foto_2->pixmap()!=nullptr){
        ZoomPicture *zoom = new ZoomPicture(nullptr, *ui->lb_foto_2->pixmap());
        connect(zoom, &ZoomPicture::rotate, ui->lb_foto_2, &MyLabelPhoto::photoRotated);
        zoom->setTextInfo(ui->pt_descripcion_foto_2->toPlainText());
        zoom->show();
    }
}
void ITAC::on_pb_foto_3_clicked()
{
    if(ui->lb_foto_3->pixmap()!=nullptr){
        ZoomPicture *zoom = new ZoomPicture(nullptr, *ui->lb_foto_3->pixmap());
        connect(zoom, &ZoomPicture::rotate, ui->lb_foto_3, &MyLabelPhoto::photoRotated);
        zoom->setTextInfo(ui->pt_descripcion_foto_3->toPlainText());
        zoom->show();
    }
}
void ITAC::on_pb_foto_4_clicked()
{
    if(ui->lb_foto_4->pixmap()!=nullptr){
        ZoomPicture *zoom = new ZoomPicture(nullptr, *ui->lb_foto_4->pixmap());
        connect(zoom, &ZoomPicture::rotate, ui->lb_foto_4, &MyLabelPhoto::photoRotated);
        zoom->setTextInfo(ui->pt_descripcion_foto_4->toPlainText());
        zoom->show();
    }
}
void ITAC::on_pb_foto_5_clicked()
{
    if(ui->lb_foto_5->pixmap()!=nullptr){
        ZoomPicture *zoom = new ZoomPicture(nullptr, *ui->lb_foto_5->pixmap());
        connect(zoom, &ZoomPicture::rotate, ui->lb_foto_5, &MyLabelPhoto::photoRotated);
        zoom->setTextInfo(ui->pt_descripcion_foto_5->toPlainText());
        zoom->show();
    }
}
void ITAC::on_pb_foto_6_clicked()
{
    if(ui->lb_foto_6->pixmap()!=nullptr){
        ZoomPicture *zoom = new ZoomPicture(nullptr, *ui->lb_foto_6->pixmap());
        connect(zoom, &ZoomPicture::rotate, ui->lb_foto_6, &MyLabelPhoto::photoRotated);
        zoom->setTextInfo(ui->pt_descripcion_foto_6->toPlainText());
        zoom->show();
    }
}
void ITAC::on_pb_foto_7_clicked()
{
    if(ui->lb_foto_7->pixmap()!=nullptr){
        ZoomPicture *zoom = new ZoomPicture(nullptr, *ui->lb_foto_7->pixmap());
        connect(zoom, &ZoomPicture::rotate, ui->lb_foto_7, &MyLabelPhoto::photoRotated);
        zoom->setTextInfo(ui->pt_descripcion_foto_7->toPlainText());
        zoom->show();
    }
}
void ITAC::on_pb_foto_8_clicked()
{
    if(ui->lb_foto_8->pixmap()!=nullptr){
        ZoomPicture *zoom = new ZoomPicture(nullptr, *ui->lb_foto_8->pixmap());
        connect(zoom, &ZoomPicture::rotate, ui->lb_foto_8, &MyLabelPhoto::photoRotated);
        zoom->setTextInfo(ui->pt_descripcion_foto_8->toPlainText());
        zoom->show();
    }
}

void ITAC::getJsonModified(QJsonObject jsonObject){
    itac = jsonObject;
}
void ITAC::on_pb_acceso_ubicacion_clicked()
{
    Seccion_Acceso_y_Ubicacion *seccion = new Seccion_Acceso_y_Ubicacion(nullptr, itac);
    connect(seccion, &Seccion_Acceso_y_Ubicacion::itac_json,
            this, &ITAC::getJsonModified);
    seccion->show();
}

void ITAC::on_pb_llaves_puertas_clicked()
{
    Seccion_LLaves_de_Puertas *seccion = new Seccion_LLaves_de_Puertas(nullptr, itac);
    connect(seccion, &Seccion_LLaves_de_Puertas::itac_json,
            this, &ITAC::getJsonModified);
    seccion->show();
}

void ITAC::on_pb_estado_clicked()
{
    Seccion_Estado *seccion = new Seccion_Estado(nullptr, itac);
    connect(seccion, &Seccion_Estado::itac_json,
            this, &ITAC::getJsonModified);
    seccion->show();
}

void ITAC::on_pb_tuberias_clicked()
{
    Seccion_Estado_de_Tuberias *seccion = new Seccion_Estado_de_Tuberias(nullptr, itac);
    connect(seccion, &Seccion_Estado_de_Tuberias::itac_json,
            this, &ITAC::getJsonModified);
    seccion->show();
}

void ITAC::on_pb_valvulas_clicked()
{
    Seccion_Estado_de_las_Valvulas *seccion = new Seccion_Estado_de_las_Valvulas(nullptr, itac);
    connect(seccion, &Seccion_Estado_de_las_Valvulas::itac_json,
            this, &ITAC::getJsonModified);
    seccion->show();
}


void ITAC::on_le_zona_selectionChanged()
{
    if(ui->le_zona->text().isEmpty()){
        if(ui->le_zona->completer()){
            QPoint pos = ui->le_zona->mapToGlobal(QPoint(0,0));
            ui->le_zona->completer()->popup()->setFixedWidth(ui->le_zona->width());
            ui->le_zona->completer()->popup()->move(pos.x(),
                                                    pos.y() + ui->le_zona->height());
        }
    }
}
void ITAC::setSelectedState(QString state){
    stateSelected = state;
    if(!stateSelected.isEmpty()){
        if(stateSelected == "EXCELENTE"){
            itac = Seccion_Estado::setExcelenteEnGeneral(itac);
            itac = Seccion_Estado_de_las_Valvulas::setExcelenteEnGeneral(itac);
            itac = Seccion_Estado_de_Tuberias::setExcelenteEnGeneral(itac);
        }else if(stateSelected == "MAL ESTADO"){
            itac = Seccion_Estado::setMalEstadoEnGeneral(itac);
            itac = Seccion_Estado_de_las_Valvulas::setMalEstadoEnGeneral(itac);
            itac = Seccion_Estado_de_Tuberias::setMalEstadoEnGeneral(itac);
        }
    }
}
void ITAC::on_pb_resumen_de_estado_clicked()
{
    Resumen_Estado_Instalacion *resumen = new Resumen_Estado_Instalacion(this);
    connect(resumen, &Resumen_Estado_Instalacion::stateSelected,
            this, &ITAC::setSelectedState);
    resumen->move(this->width()/2-resumen->width()/2,
                  this->height()/2-resumen->height()/2);
    resumen->exec();
}

void ITAC::on_le_codigo_textEdited(const QString &arg1)
{
    if(!newOne){
        if(itac.value(codigo_itac_itacs).toString()!=arg1){
            changedCodeItac = true;
        }else{
            changedCodeItac = false;
        }
    }

    QString cod_emplazamiento = arg1;
    QString zona_l = Ruta::getZonaRutaFromCodEmplazamiento(cod_emplazamiento);
    if(zona_l.isEmpty()){
        //        zona_l = Ruta::getZonaRutaFromCodEmplazamiento(o.value(poblacion).toString().toUpper());
        //        if(!zona_l.isEmpty()){
        //            o.insert(zona, zona_l);
        //        }
    }else{
        itac.insert(zona_itacs, zona_l);
        ui->le_zona->setText(zona_l);
    }
}
QMap <QString,QStringList> ITAC::fillMapForFixModel(QStringList &listHeaders){
    QMap <QString,QStringList> mapa;
    QStringList values;


    values << gestor_itacs;
    mapa.insert("GESTOR",values);

    values.clear();
    values << itac_itacs;
    mapa.insert("DIRECCIÓN", values);

    values.clear();
    values << zona_itacs;
    mapa.insert("SECTOR P", values);

    values.clear();
    values << codigo_itac_itacs;
    mapa.insert("C.EMPLAZAMIENTO", values);

    values.clear();
    values << nombre_empresa_administracion_itacs << nombre_responsable_administracion_itacs
           << telefono_fijo_administracion_itacs << telefono_movil_administracion_itacs << direccion_oficina_administracion_itacs;
    mapa.insert("ADMINISTRACIÓN", values);//+telefono
    values.clear();
    values << nombre_presidente_itacs << vivienda_presidente_itacs << telefono_movil_presidente_itacs << telefono_fijo_presidente_itacs;
    mapa.insert("PRESIDENTE O COLABORADOR", values);//+vivienda y telefono
    values.clear();
    values << nombre_encargado_itacs << vivienda_encargado_itacs << telefono_fijo_encargado_itacs << telefono_movil_encargado_itacs;
    mapa.insert("ENCARGADO O CONSERJE", values);//+vivienda y telefono
    values.clear();
    values << acceso_itacs << descripcion_itacs;
    mapa.insert("NOTA GLOBAL", values); // + Descripcion DE INSTALACION

    //SECCION 1
    values.clear();
    values << extra_acceso_ubicacion_itacs;
    mapa.insert("ACCESO",values);

    //SECCION 2
    values.clear();
    values << tipo_llave_itacs << extras_llaves_itacs;
    mapa.insert("LLAVES DE ACCESO", values); // + extras_llaves_itacs

    //SECCION 3
    values.clear();
    values << espacio_para_trabajar_itacs;
    mapa.insert("ESPACIO PARA TRABAJAR",values);
    values.clear();
    values << desague_itacs << extras_desague_itacs;
    mapa.insert("DESAGÜE",values); // + extras_desague_itacs
    values.clear();
    values << iluminacion_itacs << extras_iluminacion_itacs;
    mapa.insert("ILUMINACIÓN",values);

    //SECCION 4
    values.clear();
    values << tubo_de_alimentacion_itacs;
    mapa.insert("TUBO DE ALIMENTACIÓN",values);
    values.clear();
    values << colector_itacs;
    mapa.insert("COLECTOR",values);
    values.clear();
    values << tuberias_de_entrada_contador_itacs;
    mapa.insert("TUBERÍAS DE ENTRADA",values);
    values.clear();
    values << tuberias_de_salida_contador_itacs;
    mapa.insert("TUBERÍAS DE SALIDA",values);

    //SECCION 4
    values.clear();
    values << valvula_general_itacs << extras_valvula_general_itacs;
    mapa.insert("VÁLVULA GENERAL",values); // + extras_valvula_general_itacs
    values.clear();
    values << valvula_entrada_itacs << extras_valvula_entrada_itacs;
    mapa.insert("VÁLVULA DE ENTRADA",values); // +extras_valvula_entrada_itacs
    values.clear();
    values << valvula_salida_itacs << extras_valvula_salida_itacs;
    mapa.insert("VÁLVULA DE SALIDA",values); // +extras_valvula_salida_itacs
    values.clear();
    values << valvula_antiretorno_itacs << extras_valvula_antiretorno_itacs;
    mapa.insert("VÁLVULA DE ANTIRETORNO",values); // +extras_valvula_antiretorno_itacs

    listHeaders  << "GESTOR" << "DIRECCIÓN" << "SECTOR P" <<"C.EMPLAZAMIENTO"
                 << "ADMINISTRACIÓN" << "PRESIDENTE O COLABORADOR" <<  "ENCARGADO O CONSERJE" << "NOTA GLOBAL"
                 << "ACCESO"  <<"LLAVES DE ACCESO"
                 << "ESPACIO PARA TRABAJAR"  <<  "DESAGÜE"  <<  "ILUMINACIÓN"
                 << "TUBO DE ALIMENTACIÓN"  <<  "COLECTOR"  <<  "TUBERÍAS DE ENTRADA"  << "TUBERÍAS DE SALIDA"
                 << "VÁLVULA GENERAL"  <<  "VÁLVULA DE ENTRADA"  <<  "VÁLVULA DE SALIDA"  <<"VÁLVULA DE ANTIRETORNO";

    return mapa;
}

void ITAC::Pdf_creator(QJsonObject itac, bool show)
{

    QStringList listHeaders;
    QMap <QString,QStringList> mapa = fillMapForFixModel(listHeaders);

    QString dir_string, obs_string, descripcion_string;

    QString cod_emplazamiento  = itac.value(codigo_itac_itacs).toString().trimmed();
    QString gestor = itac.value(gestor_itacs).toString().trimmed();
    QString nombre_firmante = itac.value(nombre_firmante_itacs).toString().trimmed();// gestor
    QStringList descripcion_foto_x;
    descripcion_foto_x.append(itac.value(descripcion_foto_1_itacs).toString());
    descripcion_foto_x.append(itac.value(descripcion_foto_2_itacs).toString());
    descripcion_foto_x.append(itac.value(descripcion_foto_3_itacs).toString());
    descripcion_foto_x.append(itac.value(descripcion_foto_4_itacs).toString());
    descripcion_foto_x.append(itac.value(descripcion_foto_5_itacs).toString());
    descripcion_foto_x.append(itac.value(descripcion_foto_6_itacs).toString());
    descripcion_foto_x.append(itac.value(descripcion_foto_7_itacs).toString());
    descripcion_foto_x.append(itac.value(descripcion_foto_8_itacs).toString());

    QString filename = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/" + gestor + "/fotos_ITACs/" + cod_emplazamiento+ "/Pdf_de_ITAC.pdf";
    QFile f(filename);
    f.open(QIODevice::WriteOnly);
    QPdfWriter pdf(&f);
    pdf.setPageSize(QPagedPaintDevice::A4);
    QPainter painter(&pdf);
    painter.setPen(Qt::black);

    QJsonObject g = Gestor::getGestorJsonObject(gestor);
    Gestor *gestor_screen = new Gestor(nullptr, false, empresa);
    gestor_screen->setData(g);
    //    QString ruta_logo = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/" + gestor + "/Logo/" + ;
    QPixmap logo = gestor_screen->getPhotoGestor();
    logo = scalePhoto(logo, 1500, 1500);
    gestor_screen->deleteLater();

    painter.drawPixmap(QRect(7500,100,logo.width(),logo.height()),logo);

    int count_y_pos = 5;
    for (int n = 0; n < listHeaders.size(); n++) {

        QJsonObject jsonObject = itac;
        QString header = listHeaders.at(n);
        QStringList header_values = mapa.value(header);
        QString header_value;
        QString column_info = "";
        foreach(header_value, header_values){
            QString value = jsonObject.value(header_value).toString().trimmed();
            if(checkIfFieldIsValid(value)){
                column_info += value + ", ";
            }
        }
        if(!column_info.trimmed().isEmpty()){
            column_info = column_info.trimmed();
            column_info.remove(column_info.size()-1, 1);

            column_info.prepend(header + QString(" :  "));

            if(column_info.size() > 120){
                column_info.truncate(120);
                column_info.append("...");
            }
            painter.drawText(500,count_y_pos*250,column_info);
            count_y_pos++;
        }

    }

    pdf.newPage();

    int count_x_pos=0;
    count_y_pos=0;
    if(checkIfFieldIsValid(empresa) && checkIfFieldIsValid(cod_emplazamiento)){
        QString ruta = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/" + gestor + "/fotos_ITACs/" + cod_emplazamiento;
        for (int i=1; i<= 9; i++) {
            QString path = ruta +"/"+cod_emplazamiento + "_foto_"+ QString::number(i) + ".jpg";
            if(QFile::exists(path)){
                QPixmap pixmap = QPixmap(path);
                int max_w = 3000, max_h = 3000;
                pixmap = scalePhoto(pixmap, max_w, max_h);
                QString descripcion ="";
                if(i < 9){
                    descripcion = descripcion_foto_x.at(i);
                    if(!descripcion.isEmpty()){
                        descripcion.prepend(" :  ");
                    }
                }else{
                    if(!nombre_firmante.isEmpty()){
                        descripcion = "FIRMANTE -> "+nombre_firmante;
                        descripcion.prepend(" :  ");
                    }
                }
                if(descripcion.size() > 38){
                    descripcion.truncate(38);
                    descripcion.append("...");
                }

                painter.drawText((count_x_pos * max_w*1.1) + 500,count_y_pos*max_w*1.2 + 1000, "FOTO "+ QString::number(i) + descripcion);
                painter.drawPixmap(QRect((count_x_pos * max_w*1.1) + 500,count_y_pos*max_w*1.2 + 1200,pixmap.width(),pixmap.height()), pixmap);
                count_x_pos++;
                if(count_x_pos>=3){
                    count_x_pos=0;
                    count_y_pos++;
                }
            }
        }
    }

    //    painter.drawText(500,200,Direccion);
    //    painter.drawText(500,400,"NUMERO DE ABONADO: " + Num_abonado
    //                     + "  Tel: "+ tels);
    //    painter.drawPixmap(QRect(7500,100,1500,800),Empresa);
    //    painter.drawText(500,600,"OBSERVACIONES: " + Observaciones);
    //    painter.drawText(500,800,"CAMBIADO POR: " + Cambiado);
    //    painter.drawText(500,1000,"NUMERO DE SERIE: " + Num_serie);
    //    painter.drawText(3000,1000,"CALIBRE: " + Calibre);

    //    painter.setPen(Qt::black);
    //    painter.drawText(1300,2000,string1);
    //    painter.drawPixmap(QRect(500,2200,4000,5000),Before_installation);

    //    painter.drawText(4950,2000,string2);
    //    painter.drawPixmap(QRect(5100,2200,4000,5000),After_installation);

    //    painter.drawText(1500,7900,string3);
    //    painter.drawPixmap(QRect(500,8000,4000,5000),Lectura);

    //    painter.drawText(5100,8100,"NOMBRE DEL FIRMANTE: " +nom_firmante);
    //    painter.drawText(5550,8500,"NUMERO DE CARNET: " +num_carnet_firmante);
    //    painter.drawText(6800,8900,"Firma");

    //    painter.drawPixmap(QRect(6050,9000,2000,2500),Firma);

    //    if(checkIfFieldIsValid(num_serie_antes)){

    painter.end();

    f.close();

    if(show){
        GlobalFunctions gf(this);
    if(gf.showQuestion(this,"Creado PDF", "El PDF ha sido creado correctamente.¿Desea abrirlo?",
                                    QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){
            bool c = QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
            if(!c){
                GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this, "Error abriendo", "No se encoontró el archivo PDF de esta tarea");
            }
        }
    }
}

void ITAC::on_pb_crear_pdf_clicked()
{
    ui->pb_crear_pdf->setEnabled(false);
    Pdf_creator(itac, true);
    ui->pb_crear_pdf->setEnabled(true);
}

void ITAC::on_pb_mostrar_carpeta_clicked()
{

    QString cod_emplazamiento = itac.value(codigo_itac_itacs).toString().trimmed();
    QString gestor = ui->l_gestor->currentText().trimmed();
    if(!checkIfFieldIsValid(gestor)){
        gestor = "Sin_Gestor";
    }
    QString path = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/" + gestor + "/fotos_ITACs/" + cod_emplazamiento;
    QDir dir;
    dir.setPath(path);
    if(!dir.exists()){
        dir.setPath(path);
    }
    showInExplorer(dir.path());
}
void ITAC::showInExplorer(const QString &path)
{
    QDesktopServices::openUrl( QUrl::fromLocalFile(path) );
}
bool ITAC::descargarAudio(){
    QString audio = itac.value(audio_detalle_itacs).toString().trimmed();
    QString gestor_actual = itac.value(gestor_itacs).toString().trimmed();
    QString codigo_itac = itac.value(codigo_itac_itacs).toString().trimmed();

    bool retorno = false;

    if(checkIfFieldIsValid(audio)){
        QStringList keys, values;
        keys << "url" << "dirLocal";
        values << "Empresas/"+empresa+"/Gestores/"+gestor_actual+"/fotos_ITACs/" + codigo_itac + "/" + audio;
        values << getLocalDirofCurrentItac();

        connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        //OJO ESPERAR POR RESPUESTA
        QEventLoop q;

        connect(this, &ITAC::script_excecution_result,&q,&QEventLoop::exit);

        this->keys = keys;
        this->values = values;
        QTimer::singleShot(DELAY, this, &ITAC::download_audio_request);

        switch (q.exec())
        {
        case database_comunication::script_result::timeout:
            retorno = false;
            break;

        case database_comunication::script_result::download_audio_failed:
            retorno = false;
            break;

        case database_comunication::script_result::download_audio_doesnt_exists:
            retorno = false;
            break;

        case database_comunication::script_result::ok:
            break;
        }
    }
    return retorno;

}
void ITAC::download_audio_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_FILE,keys,values);
}

void ITAC::displayLoading(MyLabelPhoto *label){
    if(label->isDefaultPhoto()){
        label->setText(" ");
    }
    QPoint p = label->pos();
    p.setX(p.x() + label->width()/2);
    p.setY(p.y() + label->height()/2);
    show_loading(label->parentWidget(), p);
}
void ITAC::show_loading(QWidget *parent, QPoint pos, QColor color, int w, int h){
    emit hidingLoading();

    QLabel *label_back = new QLabel(parent);

    connect(this, &ITAC::hidingLoading, label_back, &QLabel::hide);
    connect(this, &ITAC::hidingLoading, label_back, &QLabel::deleteLater);
    int border = 12;
    label_back->setFixedSize(w + border, h + border);
    pos.setX(pos.x()-w/2);
    pos.setY(pos.y()-h/2);
    label_back->move(pos);
    QString circle_radius_string = QString::number(static_cast<int>((w+border)/2));
    //    QString colorRBG = getColorString(color);
    label_back->setStyleSheet("background-color: #FFFFFF;"
                              "border-radius: "+circle_radius_string+"px;"
                              /*"border: 1px solid #"+colorRBG+";"*/);
    label_back->show();

    QProgressIndicator *pi = new QProgressIndicator(parent);
    connect(this, &ITAC::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &ITAC::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color);
    pi->setFixedSize(w, h);
    pi->startAnimation();
    pos.setX(pos.x()+border/2 + 1);
    pos.setY(pos.y()+border/2 + 1);
    pi->move(pos);
    pi->show();
}
QString ITAC::getColorString(QColor color, bool hex){
    int r, g, b;
    color.getRgb(&r, &g, &b);
    QString rgb = "";
    if(hex){
        rgb = QString::number(r, 16) + QString::number(g, 16) + QString::number(b, 16);
    }else{
        rgb = QString::number(r) + "," + QString::number(g) + "," + QString::number(b);
        rgb = "rgb("+ rgb +")";
    }
    return rgb;
}
QString ITAC::getLocalDirofCurrentItac(){

    QString path = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/";
    QString gestor_actual = itac.value(gestor_itacs).toString().trimmed();
    QString cod_emplazamiento = itac.value(codigo_itac_itacs).toString().trimmed();

    if(!checkIfFieldIsValid(gestor_actual)){
        gestor_actual = "Sin_Gestor";
    }
    QDir dir;
    dir.setPath(path + gestor_actual + "/fotos_ITACs/" + cod_emplazamiento);
    if(!dir.exists()){
        dir.mkpath(path + gestor_actual + "/fotos_ITACs/" + cod_emplazamiento);
    }
    return dir.path();
}

void ITAC::loadLocalPhotos(){
    QStringList fotos;
    QList<QLabel *> labels;
    fotos << foto_1_itacs << foto_2_itacs << foto_3_itacs << foto_4_itacs
          << foto_5_itacs << foto_6_itacs << foto_7_itacs << foto_8_itacs << foto_9_itacs;

    labels.append(ui->lb_foto_1);
    labels.append(ui->lb_foto_2);
    labels.append(ui->lb_foto_3);
    labels.append(ui->lb_foto_4);
    labels.append(ui->lb_foto_5);
    labels.append(ui->lb_foto_6);
    labels.append(ui->lb_foto_7);
    labels.append(ui->lb_foto_8);
    labels.append(ui->lb_foto_9);

    for(int i=0; i< fotos.size(); i++) {
        QString foto = fotos.at(i);
        QString local_foto = itac.value(foto).toString();
        if(checkIfFieldIsValid(local_foto)){
            QString path = getLocalDirofCurrentItac() + "/" + local_foto;
            if(QFile::exists(path)){
                labels[i]->setPixmap(QPixmap(path));
            }
        }
    }
}

void ITAC::on_pb_play_audio_clicked()
{
    QString audio = itac.value(audio_detalle_itacs).toString().trimmed();
    if(checkIfFieldIsValid(audio)){
        ui->pb_play_audio->setText("Cargando Audio...");
        ui->pb_play_audio->setIcon(QIcon(":/icons/loading.png"));
        QTimer::singleShot(100, this, SLOT(reproducirAudio()));
    }else{

    }
}
void ITAC::reproducirAudio(){
    QString audio = itac.value(audio_detalle_itacs).toString().trimmed();
    QString audio_dir_file = getLocalDirofCurrentItac() + "/" + audio;
    QFile file(audio_dir_file);
    if(file.exists()){
        QString pr = audio_dir_file;
        ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 2000, true);
        ui->pb_play_audio->setText("Reproducir Detalles");
        ui->pb_play_audio->setIcon(QIcon(":/icons/play.png"));
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"No encontrado", "Audio no disponible");
        ui->pb_play_audio->hide();
    }
}

void ITAC::show_loading(QString mess){
    emit hidingLoading();

    QWidget *widget_blur = new QWidget(this);
    widget_blur->setFixedSize(this->size()+QSize(0,30));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &ITAC::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &ITAC::hidingLoading, widget_blur, &QWidget::deleteLater);

    MyLabelShine *label_loading_text = new MyLabelShine(widget_blur);
    label_loading_text->setStyleSheet("background-color: rgb(255, 255, 255);"
                                      "color: rgb(54, 141, 206);"
                                      "border-radius: 10px;"
                                      "font: italic 14pt \"Segoe UI Semilight\";");

    QRect rect = widget_blur->geometry();
    label_loading_text->setText(mess);
    label_loading_text->setFixedSize(400, 150);
    label_loading_text->move(rect.width()/2
                             - label_loading_text->size().width()/2,
                             rect.height()/2
                             -  label_loading_text->size().height()/2);
    label_loading_text->setMargin(20);
    label_loading_text->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    label_loading_text->show();
    connect(this, &ITAC::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &ITAC::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &ITAC::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &ITAC::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &ITAC::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}
void ITAC::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void ITAC::hide_loading(){
    emit hidingLoading();
}
