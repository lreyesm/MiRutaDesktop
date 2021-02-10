#include "fields_to_assing_itacs.h"
#include "ui_fields_to_assing_itacs.h"

#include "zona.h"
#include "equipo_operario.h"
#include "operario.h"
#include "gestor.h"
#include <QCompleter>
#include <QGraphicsDropShadowEffect>
#include "global_variables.h"
#include "structure_itac.h"
#include "seccion_acceso_y_ubicacion.h"
#include "seccion_llaves_de_puertas.h"
#include "seccion_estado.h"
#include "seccion_estado_de_tuberias.h"
#include "seccion_estado_de_las_valvulas.h"
#include "mapa_zonas.h"

Fields_to_Assing_ITACs::Fields_to_Assing_ITACs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Fields_to_Assing_ITACs)
{
    ui->setupUi(this);
    initializeMaps();
}

Fields_to_Assing_ITACs::~Fields_to_Assing_ITACs()
{
    delete ui;
}

void Fields_to_Assing_ITACs::on_pb_actualizar_clicked()
{
    QString nombre_firmante, carnet_firmante, zona_m, equipo_m, operaio_m, gestor_m, itac_m, geolocalizacion_m, cod_m, direccion_oficina,
            acceso, descripcion, nombre_empresa_administracion,
            nombre_responsable_administracion, telefono_fijo_administracion,
            telefono_movil_administracion, correo_administracion,
            nombre_presidente, vivienda_presidente, telefono_fijo_presidente, telefono_movil_presidente, correo_presidente,
            nombre_encargado, vivienda_encargado, telefono_fijo_encargado, telefono_movil_encargado, correo_encargado;

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

    if(!gestor_m.isEmpty()){
        itac.insert(gestor_itacs, gestor_m);
    }
    if(!itac_m.isEmpty()){
        itac.insert(itac_itacs, itac_m);
    }

    if(!equipo_m.isEmpty()){
        itac.insert(equipo_itacs, equipo_m);
    }
    if(!zona_m.isEmpty()){
        itac.insert(zona_itacs, zona_m);
    }

    if(!operaio_m.isEmpty()){
        itac.insert(operario_itacs, operaio_m);
    }

    if(!geolocalizacion_m.isEmpty()){
        itac.insert(geolocalizacion_itacs, geolocalizacion_m);
    }

    if(!acceso.isEmpty()){
        itac.insert(acceso_itacs, acceso);
    }

    if(!descripcion.isEmpty()){
        itac.insert(descripcion_itacs, descripcion);
    }

    if(!nombre_empresa_administracion.isEmpty()){
        itac.insert(nombre_empresa_administracion_itacs, nombre_empresa_administracion);
    }

    if(!nombre_responsable_administracion.isEmpty()){
        itac.insert(nombre_responsable_administracion_itacs, nombre_responsable_administracion);
    }

    if(!telefono_fijo_administracion.isEmpty()){
        itac.insert(telefono_fijo_administracion_itacs, telefono_fijo_administracion);
    }

    if(!telefono_movil_administracion.isEmpty()){
        itac.insert(telefono_movil_administracion_itacs, telefono_movil_administracion);
    }

    if(!direccion_oficina.isEmpty()){
        itac.insert(direccion_oficina_administracion_itacs, direccion_oficina);
    }

    if(!correo_administracion.isEmpty()){
        itac.insert(correo_administracion_itacs, correo_administracion);
    }

    if(!nombre_presidente.isEmpty()){
        itac.insert(nombre_presidente_itacs, nombre_presidente);
    }

    if(!vivienda_presidente.isEmpty()){
        itac.insert(vivienda_presidente_itacs, vivienda_presidente);
    }

    if(!telefono_fijo_presidente.isEmpty()){
        itac.insert(telefono_fijo_presidente_itacs, telefono_fijo_presidente);
    }

    if(!telefono_movil_presidente.isEmpty()){
        itac.insert(telefono_movil_presidente_itacs, telefono_movil_presidente);
    }

    if(!correo_presidente.isEmpty()){
        itac.insert(correo_presidente_itacs, correo_presidente);
    }

    if(!nombre_encargado.isEmpty()){
        itac.insert(nombre_encargado_itacs, nombre_encargado);
    }

    if(!vivienda_encargado.isEmpty()){
        itac.insert(vivienda_encargado_itacs, vivienda_encargado);
    }

    if(!telefono_fijo_encargado.isEmpty()){
        itac.insert(telefono_fijo_encargado_itacs, telefono_fijo_encargado);
    }

    if(!telefono_movil_encargado.isEmpty()){
        itac.insert(telefono_movil_encargado_itacs, telefono_movil_encargado);
    }

    if(!correo_encargado.isEmpty()){
        itac.insert(correo_encargado_itacs, correo_encargado);
    }
    this->close();
    emit fields_selected(itac);   
}

void Fields_to_Assing_ITACs::initializeMaps(){
    QStringList listaGestores, listaZonas,listaOperarios, listaEquiposOperarios;

    QJsonArray jsonArray = Gestor::readGestores();
    for (int i=0; i < jsonArray.size(); i++) {
        QString gestores_v, cod_v;
        gestores_v = jsonArray.at(i).toObject().value(gestor_gestores).toString();
        listaGestores.append(gestores_v);
    }
    ui->l_gestor->addItems(listaGestores);
    ui->l_gestor->clear();

    jsonArray = Equipo_Operario::readEquipo_Operarios();
    for (int i=0; i < jsonArray.size(); i++) {
        QString equipo_v, cod_v;
        equipo_v = jsonArray.at(i).toObject().value(equipo_operario_equipo_operarios).toString();
        listaEquiposOperarios.append(equipo_v);
    }
    ui->l_equipo->addItems(listaEquiposOperarios);
    ui->l_equipo->clear();

    jsonArray = Operario::readOperarios();
    for (int i=0; i < jsonArray.size(); i++) {
        QString operario_v, cod_v;
        operario_v = jsonArray.at(i).toObject().value(operario_operarios).toString();
        listaOperarios.append(operario_v);
    }
    ui->l_operario->addItems(listaOperarios);
    ui->l_operario->clear();

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
void Fields_to_Assing_ITACs::getJsonModified(QJsonObject jsonObject){
    itac = jsonObject;
}
void Fields_to_Assing_ITACs::on_pb_acceso_ubicacion_clicked()
{
    Seccion_Acceso_y_Ubicacion *seccion = new Seccion_Acceso_y_Ubicacion(nullptr, itac);
    connect(seccion, &Seccion_Acceso_y_Ubicacion::itac_json,
            this, &Fields_to_Assing_ITACs::getJsonModified);
    seccion->show();
}

void Fields_to_Assing_ITACs::on_pb_llaves_puertas_clicked()
{
    Seccion_LLaves_de_Puertas *seccion = new Seccion_LLaves_de_Puertas(nullptr, itac);
    connect(seccion, &Seccion_LLaves_de_Puertas::itac_json,
            this, &Fields_to_Assing_ITACs::getJsonModified);
    seccion->show();
}

void Fields_to_Assing_ITACs::on_pb_estado_clicked()
{
    Seccion_Estado *seccion = new Seccion_Estado(nullptr, itac);
    connect(seccion, &Seccion_Estado::itac_json,
            this, &Fields_to_Assing_ITACs::getJsonModified);
    seccion->show();
}

void Fields_to_Assing_ITACs::on_pb_tuberias_clicked()
{
    Seccion_Estado_de_Tuberias *seccion = new Seccion_Estado_de_Tuberias(nullptr, itac);
    connect(seccion, &Seccion_Estado_de_Tuberias::itac_json,
            this, &Fields_to_Assing_ITACs::getJsonModified);
    seccion->show();
}

void Fields_to_Assing_ITACs::on_pb_valvulas_clicked()
{
    Seccion_Estado_de_las_Valvulas *seccion = new Seccion_Estado_de_las_Valvulas(nullptr, itac);
    connect(seccion, &Seccion_Estado_de_las_Valvulas::itac_json,
            this, &Fields_to_Assing_ITACs::getJsonModified);
    seccion->show();
}

void Fields_to_Assing_ITACs::setGeoCode(const QString geocode)
{
    ui->le_geolocalizacion->setText(geocode);
    geo_modified = true;
}
void Fields_to_Assing_ITACs::eraseMarker()
{
    ui->le_geolocalizacion->setText("");
    geo_modified = true;
}
void Fields_to_Assing_ITACs::on_pb_geolocalizacion_clicked()
{
    Mapa_Zonas *mapa = new Mapa_Zonas(nullptr, ui->le_geolocalizacion->text());
    QObject::connect(mapa, &Mapa_Zonas::settedMarker, this, &Fields_to_Assing_ITACs::setGeoCode);
    QObject::connect(mapa, &Mapa_Zonas::erase_marker, this, &Fields_to_Assing_ITACs::eraseMarker);
    mapa->show();
}
