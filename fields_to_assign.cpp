#include "fields_to_assign.h"
#include "marca.h"
#include "zona.h"
#include "gestor.h"
#include "causa.h"
#include "longitud.h"
#include "calibre.h"
#include "emplazamiento.h"
#include "ui_fields_to_assign.h"
#include "new_table_structure.h"
#include "navegador.h"
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include "itac.h"
#include "mapas.h"
#include "global_variables.h"
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include "observationseletionscreen.h"
#include "globalfunctions.h"
#include "informe_instalacion_servicios.h"

Fields_to_Assign::Fields_to_Assign(QWidget *parent, QString empresa) :
    QDialog(parent),
    ui(new Ui::Fields_to_Assign)
{
    ui->setupUi(this);
    web_browser = new Navegador();
    ui->pb_web_browser->hide();
    this->setWindowTitle("Campos a Asignar a Varias Tareas");

    this->empresa = empresa;
    //    ui->l_geolocalizacion->hide();
    //    ui->label_2->hide();

    iniciateVariables();

    QStringList ordenes, estados, prioridades;
    estados = mapaEstados.keys();
    estados.prepend("POR DEFECTO");
    ordenes << "POR DEFECTO" << "DIARIAS" << "MASIVAS" << "ESPECIALES";
    prioridades << "POR DEFECTO" << "ALTA" << "MEDIA"  << "BAJA" << "HIBERNAR";
    ui->l_prioridad->addItems(prioridades);
    ui->cb_estado_tarea->addItems(estados);
    ui->cb_tiporden->addItems(ordenes);

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    ui->cb_zonas->setGraphicsEffect(effect);
}

Fields_to_Assign::~Fields_to_Assign()
{
    delete ui;
}

void Fields_to_Assign::iniciateVariables(){

    mapaEstados.insert("Abierta", "IDLE");
    mapaEstados.insert("Cita", "IDLE CITA");
    mapaEstados.insert("En Batería", "IDLE TO_BAT");
    mapaEstados.insert("Ejecutada", "DONE");
    mapaEstados.insert("Cerrada", "CLOSED");
    mapaEstados.insert("Informada", "INFORMADA");
    mapaEstados.insert("Requerida", "REQUERIDA");


    QStringList zonasLista, listaCalibres, listaLongitudes, causasLista,
            listaAnomalias, listaGestores, listaItacs, lista_tipo_radio;


    lista_tipo_radio.append("R3");
    lista_tipo_radio.append("R4");
    lista_tipo_radio.append("W4");
    lista_tipo_radio.append("LRW");

    QJsonArray jsonArray = Calibre::readCalibres();
    for (int i=0; i < jsonArray.size(); i++) {
        QString cal_v, cod_v;
        cal_v = jsonArray.at(i).toObject().value(calibre_calibres).toString();
        listaCalibres.append(cal_v);
    }

    GlobalFunctions gf(this, empresa);
    listaItacs = gf.getItacsList();

    jsonArray = Gestor::readGestores();
    for (int i=0; i < jsonArray.size(); i++) {
        QString cod_v, gestor_v;
        cod_v = jsonArray.at(i).toObject().value(codigo_gestor_gestores).toString();
        gestor_v = jsonArray.at(i).toObject().value(gestor_gestores).toString();
        listaGestores.append(gestor_v);
    }
    listaGestores.prepend("POR DEFECTO");
    ui->le_gestor->addItems(listaGestores);

    jsonArray = Causa::readCausas();
    for (int i=0; i < jsonArray.size(); i++) {
        QString causa_v, anomalia_v;
        anomalia_v = jsonArray.at(i).toObject().value(codigo_causa_causas).toString();
        causa_v = jsonArray.at(i).toObject().value(causa_causas).toString();
        causasLista.append(anomalia_v + " - " + causa_v);
    }

    jsonArray = Longitud::readLongitudes();
    for (int i=0; i < jsonArray.size(); i++) {
        QString long_v, cod_v;
        long_v = jsonArray.at(i).toObject().value(longitud_longitudes).toString();
        listaLongitudes.append(long_v);
    }

    jsonArray = Zona::readZonas();
    for (int i=0; i < jsonArray.size(); i++) {
        QString zona_v, cod_v;
        zona_v = jsonArray.at(i).toObject().value(zona_zonas).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_zona_zonas).toString();

        zonasLista.append(cod_v + " - " + zona_v);
        zonasLista.sort();
    }
    zonasLista.prepend("POR DEFECTO");
    ui->cb_zonas->clear();
    ui->cb_zonas->addItems(zonasLista);

    QStringList dias;
//    dias << "POR DEFECTO" << "LUNES" << "MARTES" << "MIÉRCOLES" << "JUEVES"<< "VIERNES" << "SÁBADO"<< "DOMINGO";
    dias << "POR DEFECTO" << "1" << "2" << "3" << "4"<< "5";
    ui->cb_dia_predeterminado->clear();
    ui->cb_dia_predeterminado->addItems(dias);

    jsonArray = Marca::readMarcas();
    for (int i=0; i < jsonArray.size(); i++) {
        QString marca_v, modelo_v, cod_v;
        marca_v = jsonArray.at(i).toObject().value(marca_marcas).toString();
        modelo_v = jsonArray.at(i).toObject().value(modelo_marcas).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_marca_marcas).toString();

        mapaTiposDeMarca.insert(cod_v, marca_v + " - " + modelo_v);
    }
    QStringList marcas;
    for (int i=0;i<mapaTiposDeMarca.size();i++) {

        marcas<< mapaTiposDeMarca.keys().at(i) + " - " + mapaTiposDeMarca.values().at(i);
    }

    completer_itacs = new QCompleter(listaItacs, this);
    completer_itacs->setCaseSensitivity(Qt::CaseInsensitive); completer_itacs->setFilterMode(Qt::MatchContains);
    ui->le_codigo_de_geolocalizacion->setCompleter(completer_itacs);
    connect(completer_itacs,SIGNAL(highlighted(QString)),this,SLOT(fill_itac_data(QString)));

    completer = new QCompleter(marcas, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_MARCAS->setCompleter(completer);
    ui->le_MARCAS_dv->setCompleter(completer);

    completer = new QCompleter(causasLista, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_intervencion->setCompleter(completer);
    //    connect(completer_intervenciones,SIGNAL(highlighted(QString)),this,SLOT(fill_anomaly_data(QString)));

    completer = new QCompleter(marcas, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_MARCAS->setCompleter(completer);

    completer = new QCompleter(listaCalibres, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_CALIBRE->setCompleter(completer);
    ui->le_CALIBRE_real->setCompleter(completer);

    completer = new QCompleter(listaLongitudes, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_LONGITUD->setCompleter(completer);
    ui->le_LONGITUD_dv->setCompleter(completer);

    completer = new QCompleter(lista_tipo_radio, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_radio_retirado->setCompleter(completer);
    ui->le_radio_instalado->setCompleter(completer);

    //    connect(completer,SIGNAL(highlighted(QString)),this,SLOT(fill_counter_data(QString)));
}
void Fields_to_Assign::fill_itac_data(QString cod_emplazamiento)
{
    if(!cod_emplazamiento.isEmpty()){
        GlobalFunctions gf(this, empresa);
        QJsonObject itac = gf.getItacFromServer(cod_emplazamiento);
        QString cod_v, geoCode;
        if(!itac.isEmpty()){
            geoCode = itac.value(geolocalizacion_itacs).toString().trimmed();
            ui->l_geolocalizacion->setText(geoCode);
        }
    }
}
void Fields_to_Assign::fill_anomaly_data(QString anomaly)
{
    anomaly = anomaly.split("-").at(0).trimmed();
    QJsonObject jsonCausa = Causa::getCausaObject(anomaly);

    qDebug()<<jsonCausa;
    fields.insert(ANOMALIA, anomaly);
    //    fields.insert(AREALIZAR, jsonCausa.value(arealizar_causas).toString());
    //    fields.insert(INTERVENCION, jsonCausa.value(causa_causas).toString());//intervencion
    //    fields.insert(accion_ordenada, jsonCausa.value(accion_ordenada_causas).toString());
}

void Fields_to_Assign::fill_counter_data(QString completion)
{
    Q_UNUSED(completion);
    //    if(completion.contains(" - ")){
    //        QString cod_marca = completion.split(" - ").at(0);
    //    }
}

void Fields_to_Assign::setJsonArrayContadores(QJsonArray jsonarray){
    jsonArrayContadores = jsonarray;
}
void Fields_to_Assign::on_buttonBox_accepted()
{
    if(!ui->le_poblacion->text().isEmpty()){
        fields.insert(poblacion, ui->le_poblacion->text());
    }
    if(!ui->le_calle->text().isEmpty()){
        fields.insert(calle, ui->le_calle->text());
    }
    if(!ui->le_numero_portal->text().isEmpty()){
        fields.insert(numero, ui->le_numero_portal->text());
    }
    if(!ui->cb_estado_tarea->currentText().isEmpty() && ui->cb_estado_tarea->currentText()!= "POR DEFECTO"){
        fields.insert(status_tarea, mapaEstados.value(ui->cb_estado_tarea->currentText(), "IDLE"));
    }
    if(!ui->le_codigo_de_geolocalizacion->text().isEmpty()){
        fields.insert(codigo_de_geolocalizacion, ui->le_codigo_de_geolocalizacion->text());
    }
    if(!ui->cb_zonas->currentText().isEmpty() && ui->cb_zonas->currentText()!= "POR DEFECTO"){
        fields.insert(zona, ui->cb_zonas->currentText());
    }
    if(!ui->cb_dia_predeterminado->currentText().isEmpty() && ui->cb_dia_predeterminado->currentText()!= "POR DEFECTO"){
        fields.insert(dia_predeterminado, ui->cb_dia_predeterminado->currentText());
    }
    if(!ui->l_prioridad->text().isEmpty() && ui->l_prioridad->currentText()!= "POR DEFECTO"){
        fields.insert(prioridad, ui->l_prioridad->text());
    }
    if(!ui->le_intervencion->text().isEmpty()){
        fill_anomaly_data(ui->le_intervencion->text());
    }
    if(!ui->le_gestor->text().isEmpty() && ui->le_gestor->currentText()!= "POR DEFECTO"){
        fields.insert(GESTOR, ui->le_gestor->text());
    }
    if(!ui->l_geolocalizacion->text().isEmpty() && !ui->l_geolocalizacion->text().contains("...")){
        fields.insert(geolocalizacion, ui->l_geolocalizacion->text());
        QString url = "https://maps.google.com/?q=" + ui->l_geolocalizacion->text();
        fields.insert(url_geolocalizacion, url);
    }
    if(!ui->l_geolocalizacion_mano->text().isEmpty() && !ui->l_geolocalizacion_mano->text().contains("...")){
        fields.insert(codigo_de_localizacion, ui->l_geolocalizacion_mano->text());
        QString url = "https://maps.google.com/?q=" + ui->l_geolocalizacion_mano->text();
        fields.insert(url_geolocalizacion, url);
    }
    if(!ui->cb_tiporden->currentText().isEmpty() && ui->cb_tiporden->currentText()!="POR DEFECTO"){
        fields.insert(TIPORDEN, ui->cb_tiporden->currentText());
    }
    if(!ui->le_CALIBRE->text().isEmpty()){
        fields.insert(calibre_toma, ui->le_CALIBRE->text());
    }
    if(!ui->le_CALIBRE_real->text().isEmpty()){
        fields.insert(calibre_real, ui->le_CALIBRE_real->text());
    }
    if(!ui->le_LONGITUD->text().isEmpty()){
        fields.insert(LARGO, ui->le_LONGITUD->text());
    }
    if(!ui->le_MARCAS->text().isEmpty()){
        fields.insert(marca_contador, ui->le_MARCAS->text());
    }
    if(!ui->le_MARCAS_dv->text().isEmpty()){
        fields.insert(marca_devuelta, ui->le_MARCAS_dv->text());
    }
    if(!ui->le_OBSERVACIONES->text().isEmpty()){
        fields.insert(observaciones, ui->le_OBSERVACIONES->text());
    }
    if(!ui->le_EMPLAZAMIENTO->text().isEmpty()){
        fields.insert(emplazamiento, ui->le_EMPLAZAMIENTO->text());
    }
    if(!ui->le_ACCESO->text().isEmpty()){
        fields.insert(acceso, ui->le_ACCESO->text());
    }
    if(!ui->le_LONGITUD_dv->text().isEmpty()){
        fields.insert(largo_devuelto, ui->le_LONGITUD_dv->text());
    }
    if(!ui->le_BIS->text().isEmpty()){
        fields.insert(BIS, ui->le_BIS->text());
    }
    if(!ui->le_resultado->text().isEmpty()){
        fields.insert(resultado, ui->le_resultado->text());
    }
    if(!ui->le_observaciones_devueltas->items().isEmpty()){
        QStringList list = ui->le_observaciones_devueltas->items();
        if(!list.isEmpty()){
            fields.insert(observaciones_devueltas, list.join("\n"));
        }
    }
    emit fields_selected(fields);
}

void Fields_to_Assign::on_pb_web_browser_clicked()
{
    web_browser->show();
}

QString Fields_to_Assign::get_coordinades_From_Link_GoogleMaps(const QString &link_google_maps)
{
    //    QString link_google_maps = "https://www.google.com/maps/dir//23.0504409,-82.5170422/@23.0382777,-82.5225353,13z";
    if(link_google_maps.contains("dir//")){
        if(link_google_maps.split("dir//").size() >= 2){
            QString posicion_string = link_google_maps.split("dir//").at(1);
            if(posicion_string.contains("/@")){
                QString latLang = posicion_string.split("/@").at(0);
                return latLang;
            }
        }
    }
    return "...";
}

void Fields_to_Assign::closeEvent(QCloseEvent *e)
{
    emit closing();
    QWidget::closeEvent(e);
}


void Fields_to_Assign::on_pb_open_google_maps_clicked()
{
    QJsonObject jsonObject;
    if(!ui->l_geolocalizacion->text().isEmpty() && !ui->l_geolocalizacion->text().contains("...")){
        jsonObject.insert(geolocalizacion, ui->l_geolocalizacion->text());
    }
    if(!ui->l_geolocalizacion_mano->text().isEmpty() && !ui->l_geolocalizacion_mano->text().contains("...")){
        jsonObject.insert(codigo_de_localizacion, ui->l_geolocalizacion_mano->text());
    }

    int width = ui->le_gestor->pos().x() + ui->le_gestor->width() + 2;

    Mapas *mapa = new Mapas(this, jsonObject);
    mapa->setFixedWidth(width);
    QObject::connect(mapa, &Mapas::settedMarker, this, &Fields_to_Assign::setGeoCode);
    QObject::connect(mapa, &Mapas::settedMarkerHand, this, &Fields_to_Assign::setGeoCodeHand);
    QObject::connect(mapa, &Mapas::erase_marker_hand, this, &Fields_to_Assign::eraseHandMarker);
    QObject::connect(mapa, &Mapas::erase_marker_home, this, &Fields_to_Assign::eraseHomeMarker);

    QObject::connect(this, &Fields_to_Assign::closing, mapa, &Mapas::close);
    QObject::connect(ui->pb_open_google_maps, &QPushButton::clicked, mapa, &Mapas::close);
    mapa->show();
}

void Fields_to_Assign::setGeoCode(const QString geocode)
{
    ui->l_geolocalizacion->setText(geocode);
}

void Fields_to_Assign::setGeoCodeHand(const QString geocode)
{
    ui->l_geolocalizacion_mano->setText(geocode);

}

void Fields_to_Assign::eraseHomeMarker()
{
    ui->l_geolocalizacion->setText("...");
}

void Fields_to_Assign::eraseHandMarker()
{
    ui->l_geolocalizacion_mano->setText("...");
}

void Fields_to_Assign::on_pb_aceptar_clicked()
{
    on_buttonBox_accepted();
    emit accept();
    this->close();
}

void Fields_to_Assign::on_pb_cancelar_clicked()
{
    emit rejected();
    this->close();
}

void Fields_to_Assign::setObservation(QString obs){
    observacion_seleccionada = obs;
}
void Fields_to_Assign::on_pb_add_observacion_clicked()
{
    ObservationSeletionScreen *observaciones = new ObservationSeletionScreen(this);
    connect(observaciones, &ObservationSeletionScreen::selected_observation, this, &Fields_to_Assign::setObservation);
    observaciones->moveCenter();
    if(observaciones->exec()){
        if(!ui->le_observaciones_devueltas->contains(observacion_seleccionada)){
            ui->le_observaciones_devueltas->addItem(observacion_seleccionada);
        }
    }
}

void Fields_to_Assign::on_pb_erase_observacion_clicked()
{
    ui->le_observaciones_devueltas->removeSelected();
}

void Fields_to_Assign::getData(QJsonObject fields_received)
{
    QStringList keys = fields_received.keys();
    for(int i=0; i < keys.size(); i++){
        QString key = keys.at(i);
        QString value = fields_received.value(key).toString().trimmed();
        if(GlobalFunctions::checkIfFieldIsValid(value)
                && key != numero_abonado && key != ID_SAT){
            fields.insert(key, value);
        }
    }
}

void Fields_to_Assign::on_pb_validacion_clicked()
{
    Informe_Instalacion_Servicios *informe = new Informe_Instalacion_Servicios(this, empresa);
    QJsonObject jsonObject;
    QStringList keys = fields.keys();
    for(int i=0; i < keys.size(); i++){
        QString key = keys.at(i);
        QString value = fields.value(key).trimmed();
        if(GlobalFunctions::checkIfFieldIsValid(value)
                && key != numero_abonado && key != ID_SAT){
            jsonObject.insert(key, value);
        }
    }
    informe->setData(jsonObject);
    informe->showWidgetData(false);

    QEventLoop loop;
    connect(informe, &Informe_Instalacion_Servicios::sendData, this,&Fields_to_Assign::getData);
    connect(informe, &Informe_Instalacion_Servicios::finalizado_informe, &loop,&QEventLoop::exit);
    connect(this, &Fields_to_Assign::closing, informe, &Informe_Instalacion_Servicios::close);
    informe->show();
    switch (loop.exec())
    {
    case QDialog::Accepted:
        break;
    case QDialog::Rejected:
        break;
    }
}
