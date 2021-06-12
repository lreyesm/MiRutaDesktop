#include "mapas_cercania.h"
#include "ui_mapas_cercania.h"


#include <QQmlContext>
#include "new_table_structure.h"
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>
#include "math.h"
#include "global_variables.h"
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include "clickablelabel.h"
#include <QMessageBox>
#include <QIntValidator>
#include "operario.h"
#include "mylineeditshine.h"
#include "mycheckbox.h"
#include <QScrollArea>
#include "animationsclass.h"
#include "mylabelshine.h"
#include "QProgressIndicator.h"
#include "globalfunctions.h"

Mapas_Cercania::Mapas_Cercania(QWidget *parent, QJsonArray jsonArray, QString empresa) :
    QWidget(parent),
    ui(new Ui::Mapas_Cercania)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    this->empresa = empresa;
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    ui->label->setGraphicsEffect(effect);

    QGraphicsDropShadowEffect* ef = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    ef->setBlurRadius(20);
    ef->setOffset(1);
    ef->setColor(color_blue_app);
    ui->l_mostrar_en_tabla->setGraphicsEffect(ef);

    QGraphicsDropShadowEffect* eff = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    eff->setBlurRadius(20);
    eff->setOffset(1);
    eff->setColor(color_blue_app);
    ui->l_cantidad_de_tareas->setGraphicsEffect(eff);

    jsonArrayTareas = jsonArray;
    QString geoCode = "";
    for (int i=0; i < jsonArrayTareas.size(); i++) {
        geoCode = jsonArrayTareas.at(i).toObject().
                value(codigo_de_localizacion).toString().trimmed();
        if(!checkIfFieldIsValid(geoCode)){
            geoCode = jsonArrayTareas.at(i).toObject().
                    value(geolocalizacion).toString().trimmed();
        }
        if(checkIfFieldIsValid(geoCode)){
            first_coords = getCoordsFromString(geoCode, first_latitud, first_longitud);
            if(first_coords.isValid()){
                break;
            }
        }
    }

    ui->quickWidget->setSource(QUrl("qrc:/qml/mapview.qml"));

    connect(ui->lineEdit, &MyLineEditAutoComplete::textModified, this, &Mapas_Cercania::searchSuggestions);
    connect(ui->lineEdit, &MyLineEditAutoComplete::itemSelected, this, &Mapas_Cercania::searchItem);

    //Mapbox (provider of maps)----------------------------------------------------------------------------------------------------------------------------------------
    //    QVariantMap parameters;
    //    parameters["mapbox.access_token"] = "pk.eyJ1IjoiMTMxOTk1YWRyaWFuIiwiYSI6ImNrMTJvcnQ5ajAweGgzY3FkZDZwdWsxbXUifQ.Yuj14-vJOFLd_U78K-iwOg";
    //    provider = new QGeoServiceProvider("mapbox",parameters);
    //----------------------------------------------------------------------------------------------------------------------------------------------

    //Here (provider of maps)-----------------------------------------------------------------------------------------------------------------------------------------
    QVariantMap parameters;  //QMap<QString,QVariant>
    parameters["here.app_id"] = "rJr3k1ADGxLWrJgauEa0";
    parameters["here.token"] = "nvFXd7lx_jURlmgQvhsGaw";
    parameters["here.proxy"] = "system";
    provider = new QGeoServiceProvider("here",parameters);
    //----------------------------------------------------------------------------------------------------------------------------------------------

    //Google Maps (provider of maps)-----------------------------------------------------------------------------------------------------------------------------------------
    //        QVariantMap parameters;  //QMap<QString,QVariant>
    //        parameters["googlemaps.google_maps_key"] = "AIzaSyB8Kny7pDcsLU9LbcKb9lMtEspHkWLqxKo";
    //        provider = new QGeoServiceProvider("googlemaps",parameters);
    //----------------------------------------------------------------------------------------------------------------------------------------------

    manager = provider->placeManager();

    timer_hibernacion.setInterval(180000);
    connect(&timer_hibernacion, &QTimer::timeout, this, &Mapas_Cercania::getOperariosFromServer);
    timer_hibernacion.start();

    ui->l_max_min->hide();

    QTimer::singleShot(DELAY, this, &Mapas_Cercania::fillMap);

    this->setAttribute(Qt::WA_DeleteOnClose);
}

Mapas_Cercania::~Mapas_Cercania()
{
    delete ui;
}
void Mapas_Cercania::fillMap(){
    show_loading("Cargando tareas...");
    if(first_coords.isValid()){
        zoom_level = fillMapWithGeolocalizations(jsonArrayTareas, first_coords);
    }
    hide_loading();
}

QString Mapas_Cercania::getMapPluginName(){
    return mapPluginName;
}
QString Mapas_Cercania::getMapPluginParameterName(){
    return mapPluginParameterName;
}
QString Mapas_Cercania::getMapPluginParameterValue(){
    return mapPluginParameterValue;
}
int Mapas_Cercania::getMapType(){
    return mapType;
}
void Mapas_Cercania::setCenter(QGeoCoordinate coords)
{
    if(marker_model){
        marker_model->setCenter(coords.latitude(), coords.longitude());
    }
}

void Mapas_Cercania::setZoomLevel(double zoom)
{
    if(marker_model){
        marker_model->setZoomLevel(zoom);
    }
}

void Mapas_Cercania::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    ui->quickWidget->setFixedSize(this->size());
    ui->quickWidget->move(0,0);

    ui->lineEdit->move(this->width()/2 - ui->lineEdit->width()/2, 10);

    ui->pb_close->move(this->width() - 50, 10);
    ui->pb_map_search->move(ui->lineEdit->pos().x() + ui->lineEdit->width() + 10,
                            ui->pb_map_search->pos().y());
    ui->l_mostrar_en_tabla->move(this->width() - 100, 10);
    ui->pb_show_filter_zona->move(this->width() - 150, 10);
    ui->widget_zona->move(this->width() - 310, 60);
    ui->l_cantidad_de_tareas->move(10, this->height() - 40);
    ui->pb_map_type_change->move(10, this->height() - 90);

}

bool Mapas_Cercania::checkHibernation(){ 
    QJsonObject numeros_internos, campos;
    QGeoCoordinate center;
    QString priority = "MEDIA";
    for (int i=0; i < tareas_hibernadas.keys().size(); i++) {
        QString numInterno = tareas_hibernadas.keys().at(i);
        QDateTime dt = tareas_hibernadas.value(numInterno);

        if(dt <= QDateTime::currentDateTime()){
            numeros_internos.insert(QString::number(i), numInterno);
            tareas_hibernadas.remove(numInterno);

            for (int i= 0; i < jsonArrayTareas.size(); i++) {
                QJsonObject jsonObject = jsonArrayTareas.at(i).toObject();
                QString numin = jsonObject.value(principal_variable).toString().trimmed(); //mano
                QString geocode = jsonObject.value(codigo_de_localizacion).toString().trimmed(); //mano
                if(!checkIfFieldIsValid(geocode)){
                    geocode = jsonObject.value(geolocalizacion).toString().trimmed();//casa
                }
                if(numin == numInterno){
                    double x, y;
                    center = getCoordsFromString(geocode, x, y);
                    QString field = jsonObject.value(hibernacion).toString().trimmed();
                    if(checkIfFieldIsValid(field)){
                        QStringList split = field.split("::");
                        if(split.size()>1){
                            priority = split.at(1).trimmed();
                        }
                    }
                    jsonObject.insert(prioridad, priority);
                    jsonArrayTareas[i] = jsonObject;
                    break;
                }
            }
        }
    }

    if(numeros_internos.isEmpty()){
        hide_loading();
        return false;
    }
    campos.insert(prioridad, priority);

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

    connect(this, &Mapas_Cercania::script_excecution_result,q,&QEventLoop::exit);
    QTimer::singleShot(DELAY, this, &Mapas_Cercania::update_tareas_fields_request);

    double previous_zoom = marker_model->zoomLevel();
    fillMapWithGeolocalizations(jsonArrayTareas, center);
    marker_model->setZoomLevel(previous_zoom);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        emit updateTareas(1);
        res = true;
        break;

    case database_comunication::script_result::update_tareas_fields_to_server_failed:
        res = false;
        break;
    }
    delete q;

    hide_loading();
    return res;
}
void Mapas_Cercania::update_tareas_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA_FIELDS,keys,values);
}
void Mapas_Cercania::getOperariosFromServer()
{
    show_loading("Actualizando ubicaciones...");
    QStringList keys, values;
    keys << "empresa";
    values << empresa.toLower();
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    connect(this,SIGNAL(OperatorsFilled(database_comunication::serverRequestType)),this,SLOT(populateComboBox(database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_OPERARIOS,keys,values);
}
void Mapas_Cercania::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    if(tipo == database_comunication::UPDATE_TAREA_FIELDS)
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
    else if(tipo == database_comunication::GET_OPERARIOS)
    {
        ba.remove(0,2);
        ba.chop(2);

        if(ba.contains("not success get_operarios"))
        {
            result = database_comunication::script_result::get_operarios_failed;
        }
        else
        {
            QJsonArray jsonArrayAllOperarios = database_comunication::getJsonArray(ba);
            Operario::writeOperarios(jsonArrayAllOperarios);
            result = database_comunication::script_result::ok;
            QTimer::singleShot(100, this, &Mapas_Cercania::checkHibernation);
        }
    }
    emit script_excecution_result(result);
}

void Mapas_Cercania::emitUpdateTareas(QJsonObject data){
    emit updateTareas(1);
    QString numInterno = data.value(principal_variable).toString();
    for (int i= 0; i < jsonArrayTareas.size(); i++) {
        QJsonObject jsonObject = jsonArrayTareas.at(i).toObject();
        QString numin = jsonObject.value(principal_variable).toString().trimmed(); //mano
        if(numin == numInterno){
            if(data.contains(equipo)){
                jsonObject.insert(equipo, data.value(equipo).toString());
            }
            if(data.contains(operario)){
                jsonObject.insert(operario, data.value(operario).toString());
            }
            jsonObject.insert(prioridad, data.value(prioridad).toString());
            jsonObject.insert(hibernacion, data.value(hibernacion).toString());
            jsonObject.insert(date_time_modified, data.value(date_time_modified));
            jsonArrayTareas[i] = jsonObject;
            break;
        }
    }
    double previous_zoom = marker_model->zoomLevel();
    fillMapWithGeolocalizations(jsonArrayTareas, marker_model->getCurrentCenter());
    marker_model->setZoomLevel(previous_zoom);
}
bool Mapas_Cercania::comparePriority(QString firstPriority, QString secondPriority){ //true si la seguda prioridad es mayor a la primera
    QMap<int, QString> priorities;
    priorities.insert(1, "HIBERNAR");
    priorities.insert(2, "BAJA");
    priorities.insert(3, "MEDIA");
    priorities.insert(4, "ALTA");

    if(priorities.key(firstPriority) < priorities.key(secondPriority)){
        return true;
    }
    return false;
}
bool Mapas_Cercania::tareaConCitaHoy(QJsonObject jsonObject){
    QString status = jsonObject.value(status_tarea).toString().trimmed(); //mano
    if(status.contains("CITA")){
        QString fecha_cita = jsonObject.value(fecha_hora_cita).toString().trimmed();
        if(checkIfFieldIsValid(fecha_cita)){
            QDateTime dt = QDateTime::fromString(fecha_cita, formato_fecha_hora);
            if(dt.isValid() && dt.date() == QDateTime::currentDateTime().date()){
                return true;
            }
        }
    }
    return false;
}
double Mapas_Cercania::fillMapWithGeolocalizations(QJsonArray jsonArray, QGeoCoordinate center, double perimetro){

    qDebug()<<"center: "<< center;
    double distance, max_distance = 0;
    double longitud, latitud;

    QJsonArray jsonArrayOperarios = Operario::readOperarios();
    jsonArrayTareasForTable = QJsonArray();

    delete marker_model;
    marker_model = new MarkerModelCercania(this, center.latitude(), center.longitude());
    marker_model->setMaxMarkers(jsonArray.size() + jsonArrayOperarios.size() + 1);

    int cant_mostradas =0;
    QStringList zonas;
    QList<QGeoCoordinate> lista_coordenadas;

    mapa_coords_operarios.clear();
    for (int i= 0; i < jsonArrayOperarios.size(); i++) {
        QString geocode = jsonArrayOperarios.at(i).toObject().value(geolocalizacion_operarios).toString().trimmed(); //operario
        if(checkIfFieldIsValid(geocode)){
            QGeoCoordinate coords = getCoordsFromString(geocode, latitud, longitud);
            if(mapa_coords_operarios.contains(coords.toString())){
                QJsonArray jsonArray = mapa_coords_operarios.value(coords.toString());
                jsonArray.append(jsonArrayOperarios.at(i).toObject());
                mapa_coords_operarios.insert(coords.toString(), jsonArray);
            }else{
                QJsonArray jsonArray;
                jsonArray.append(jsonArrayOperarios.at(i).toObject());
                mapa_coords_operarios.insert(coords.toString(), jsonArray);
                marker_model->moveMarker(coords);
            }
        }
    }
    for (int i= 0; i < jsonArray.size(); i++) {
        QJsonObject jsonObject = jsonArray.at(i).toObject();
        QString numInterno = jsonObject.value(principal_variable).toString().trimmed();
        QString tipo_tarea_l = jsonObject.value(tipo_tarea).toString().trimmed();
        QString prioridad_l = jsonObject.value(prioridad).toString().trimmed();
        QString geocode = jsonObject.value(codigo_de_localizacion).toString().trimmed(); //mano
        QString tipoOrden = jsonObject.value(TIPORDEN).toString().trimmed();
        QString radio = jsonObject.value(tipoRadio).toString().trimmed();
        QString cita_hoy = "";
        if(tareaConCitaHoy(jsonObject)){
            cita_hoy = "CITA";
        }
        if(completer_zonas == nullptr){
            QString zona_l = jsonObject.value(zona).toString().trimmed();
            if(checkIfFieldIsValid(zona_l) && !zonas.contains(zona_l)){
                zonas << zona_l;
            }
        }
        if(!checkIfFieldIsValid(geocode)){
            geocode = jsonObject.value(geolocalizacion).toString().trimmed();//casa
        }
        if(!checkIfFieldIsValid(prioridad_l)){
            prioridad_l = "MEDIA";//naranja
        }
        else if(prioridad_l == "HIBERNAR"){//gris
            QString field = jsonObject.value(hibernacion).toString().trimmed();
            if(checkIfFieldIsValid(field)){
                QStringList split = field.split("::");
                if(split.size()>1){
                    field = split.at(0).trimmed();
                }
                QDateTime dt = QDateTime::fromString(field, formato_fecha_hora);
                if(dt <= QDateTime::currentDateTime()){
                    if(split.size()>1){
                        prioridad_l = split.at(1).trimmed();
                    }else{
                        prioridad_l = "MEDIA";//naranja
                    }
                }else{
                    tareas_hibernadas.insert(numInterno, dt);
                }
            }
        }
        if(checkIfFieldIsValid(geocode)){
            QGeoCoordinate coords = getCoordsFromString(geocode, latitud, longitud);
            if(coords.isValid() && marker_model != nullptr)
            {
                if(!lista_coordenadas.contains(coords)){
                    distance = distanceInKmBetweenEarthCoordinates(center.latitude(), center.longitude(), latitud, longitud);
                    if((perimetro > 0 && distance*1000 <= perimetro) || (perimetro <= 0)){
                        //El perimetro esta en metros y la distancia en Km

                        mapa_coords_strings.insert(coords.toString(),tipo_tarea_l);
                        mapa_coords_priority.insert(coords.toString(),prioridad_l);
                        mapa_coords_ordenes.insert(coords.toString(),tipoOrden);
                        mapa_coords_radios.insert(coords.toString(),radio);
                        mapa_coords_citas.insert(coords.toString(),cita_hoy);

                        if(distance > max_distance){
                            max_distance = distance;
                        }
                        marker_model->setText(QString::number(i));
                        marker_model->moveMarker(coords);
                        lista_coordenadas.append(coords);
                        cant_mostradas++;
                        jsonArrayTareasForTable.append(jsonObject);
                    }
                }else{
                    cant_mostradas++;
                    jsonArrayTareasForTable.append(jsonObject);
                    if(comparePriority(mapa_coords_priority.value(coords.toString()), prioridad_l)
                            || checkIfFieldIsValid(cita_hoy)){
                        mapa_coords_priority.remove(coords.toString());
                        mapa_coords_priority.insert(coords.toString(),prioridad_l);
                        mapa_coords_ordenes.insert(coords.toString(),tipoOrden);
                        mapa_coords_radios.insert(coords.toString(),radio);
                        mapa_coords_citas.insert(coords.toString(),cita_hoy);
                    }
                }
            }
        }
    }
    if(completer_zonas == nullptr){
        completer_zonas = new QCompleter(zonas, this);
        completer_zonas->setCaseSensitivity(Qt::CaseInsensitive);
        completer_zonas->setFilterMode(Qt::MatchContains);
        ui->cb_zonas->addItems(zonas);
        connect(ui->cb_zonas, &MyComboBoxShine::currentTextChanged, this, &Mapas_Cercania::filterZona);
    }

    ui->l_cantidad_de_tareas->setText("Resultado: "+QString::number(cant_mostradas) + ((cant_mostradas != 1)?" tareas":" tarea"));

    double zoom = getZoomLevelWithDistance(max_distance*2.2);

    //    double dist = get_Meter_per_Pixel(zoom);

    if(zoom < 0){
        zoom = 18;
    }
    marker_model->setZoomLevel(zoom);

    marker_model->setMapType(mapType);

    marker_model->setText("Desarrollo");
    marker_model->moveMarker(QGeoCoordinate(23.0714,-82.4363));

    ui->quickWidget->rootContext()->setContextProperty("w",this);
    ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);

    QTimer::singleShot(300, this, &Mapas_Cercania::showResumen);
    return zoom;
}
void Mapas_Cercania::filterZona(QString zona_selected){
    QJsonArray jsonArrayFiltered;
    QGeoCoordinate first_coords;
    for (int i= 0; i < jsonArrayTareas.size(); i++) {
        QString zona_tarea = jsonArrayTareas.at(i).toObject().value(zona).toString().trimmed();
        if(zona_selected == zona_tarea){
            jsonArrayFiltered.append(jsonArrayTareas.at(i).toObject());
            if(!first_coords.isValid()){
                QString geoCode = jsonArrayTareas.at(i).toObject().
                        value(codigo_de_localizacion).toString().trimmed();
                if(!checkIfFieldIsValid(geoCode)){
                    geoCode = jsonArrayTareas.at(i).toObject().
                            value(geolocalizacion).toString().trimmed();
                }
                if(checkIfFieldIsValid(geoCode)){
                    first_coords = getCoordsFromString(geoCode, first_latitud, first_longitud);
                }
            }
        }
    }
    zoom_level = fillMapWithGeolocalizations(jsonArrayFiltered, first_coords);
}
void Mapas_Cercania::filterField(QStringList filterColumnList, QString field){
    QJsonArray jsonArrayFiltered;
    QGeoCoordinate first_coords;
    for (int i= 0; i < jsonArrayTareas.size(); i++) {
        QString field_tarea = jsonArrayTareas.at(i).toObject().value(field).toString().trimmed();
        if(filterColumnList.contains(field_tarea)){
            jsonArrayFiltered.append(jsonArrayTareas.at(i).toObject());
            if(!first_coords.isValid()){
                QString geoCode = jsonArrayTareas.at(i).toObject().
                        value(codigo_de_localizacion).toString().trimmed();
                if(!checkIfFieldIsValid(geoCode)){
                    geoCode = jsonArrayTareas.at(i).toObject().
                            value(geolocalizacion).toString().trimmed();
                }
                if(checkIfFieldIsValid(geoCode)){
                    first_coords = getCoordsFromString(geoCode, first_latitud, first_longitud);
                }
            }
        }
    }
    zoom_level = fillMapWithGeolocalizations(jsonArrayFiltered, first_coords);
}

void Mapas_Cercania::showResumen(){
    QPoint thisPos = this->mapToGlobal(QPoint(0,0));
    if(resumen == nullptr){
        resumen = new Resumen_Tareas(nullptr, jsonArrayTareasForTable, empresa);
        resumen->move(thisPos.x() + this->width() + 10, thisPos.y()-7);

        connect(this, &Mapas_Cercania::closing, resumen, &Resumen_Tareas::close);
        connect(resumen, &Resumen_Tareas::moveMapCenter,
                this, &Mapas_Cercania::moveMapCenter);
        connect(resumen, &Resumen_Tareas::openTarea,
                this, &Mapas_Cercania::openTareaWithNumin);
        connect(resumen, &Resumen_Tareas::updateTareas,
                this, &Mapas_Cercania::emitUpdateTareas);
        connect(this, &Mapas_Cercania::coords_selected, resumen, &Resumen_Tareas::focusTareaCoords);

        resumen->show();

    }else{
        resumen->setTareas(jsonArrayTareasForTable);
        if(resumen->isHidden()){
            resumen->setMaximumWidth(380);
            resumen->move(thisPos.x() + this->width() + 10, thisPos.y()-7);
            resumen->show();
        }
    }
}
void Mapas_Cercania::openTareaWithNumin(QString numin){
    emit openTarea(numin);
}

void Mapas_Cercania::moveMapCenter(QString geoCode){
    double xcoord, ycoord;
    getCoordsFromString(geoCode, xcoord, ycoord);
    if(marker_model){
        marker_model->setCenter(xcoord, ycoord);
        marker_model->setZoomLevel(18);
        zoom_level = 18;
        hideLabelInformation();
    }
}
void Mapas_Cercania::setCurrentZoomInQML(double zoom){
    if(marker_model){
        marker_model->setZoom(zoom);
    }
}
QString Mapas_Cercania::getTextWhitGeoCode(QGeoCoordinate coords){
    QString text = mapa_coords_strings.value(coords.toString());
    return text;
}

QString Mapas_Cercania::getCurrentPriorityMarker(QGeoCoordinate coords)
{
    QString priority;
    if(mapa_coords_priority.contains(coords.toString())){
        priority = mapa_coords_priority.value(coords.toString());
    }
    if(priority != "HIBERNAR" && mapa_coords_citas.contains(coords.toString())){
        QString cita_hoy = mapa_coords_citas.value(coords.toString());
        if(checkIfFieldIsValid(cita_hoy)){
            priority = cita_hoy;
        }
    }

    if(mapa_coords_radios.contains(coords.toString())){
        QString radio = mapa_coords_radios.value(coords.toString());
        if(checkIfFieldIsValid(radio)){
            priority.prepend("RADIO_");
        }
    }
    if(mapa_coords_ordenes.contains(coords.toString())){
        QString orden = mapa_coords_ordenes.value(coords.toString());
        if(orden.contains("D", Qt::CaseInsensitive)){
            priority.prepend("DIARIA_");
        }
    }

    if(mapa_coords_operarios.contains(coords.toString())){
        priority = "USER";
    }

    return "qrc:///icons/"+priority+"_priority_marker.png";
}
double Mapas_Cercania::get_Meter_per_Pixel(double zoom){
    double dist = getDistanceWithZoomLevel(zoom);
    double m_center_top = dist/(sqrt(2)); //raiz de dos si es del centro a la diagonal superior izqquierda //sin raiz de dos desde el centro al tope del widget

    double pixels_center_top = static_cast<double>(this->height())/2.0;
    double meter_per_pixel = m_center_top / pixels_center_top;
    return meter_per_pixel / 1.5; //1.6 prueba y error //1.5 distancia exacta bien
}
void Mapas_Cercania::printCoordenates(const QString &coordenada)
{
    double latitude1;
    double longitude1;
    centro_perimetro = getCoordsFromString(coordenada, latitude1, longitude1);
    qDebug()<<"centro_perimetro: "<< centro_perimetro;
    setPerimeterButtons();
}
void Mapas_Cercania::setPerimeterButtons(QPoint pos_center){
    tooltip_enable = false;
    clearPerimeterButtons_enable = false;
    if(pos_center == QPoint(-1,-1)){
        pos_center = this->mapFromGlobal(QCursor::pos());
    }
    last_center_pos = pos_center;
    int circle_radius = 8;
    int label_height = 25;
    int label_width = 100;
    //-----------------------------------------------------------------------------------------------------------
    QLabel *label_centro = new QLabel(this); //punto centro
    label_centro->setStyleSheet("background-color: #368DCE;"
                                "border-radius: "+QString::number(circle_radius)+"px;"
                                                                                 "border: 2px solid #FFFFFF;");
    QGraphicsDropShadowEffect* eff = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    eff->setBlurRadius(20);
    eff->setOffset(1);
    //    eff->setColor(color_blue_app);
    label_centro->setGraphicsEffect(eff);

    connect(this, &Mapas_Cercania::deleteLabelInformation, label_centro, &QLabel::deleteLater);
    connect(this, &Mapas_Cercania::deleteLabelInformation, eff, &QGraphicsDropShadowEffect::deleteLater);
    label_centro->setFixedSize(circle_radius*2, circle_radius*2);
    QPoint pos = pos_center;
    pos.setX(pos.x() - circle_radius);
    pos.setY(pos.y() - circle_radius);
    label_centro->move(pos);
    label_centro->show();

    //-----------------------------------------------------------------------------------------------------------
    QLabel *label = new QLabel(this); //Tag
    label->setStyleSheet("background-color: #FFFFFF;"
                         "border-radius: "+ QString::number(circle_radius/4, 'f', 0) +"px;"
                                                                                      "color:rgb(54, 141, 206);");
    label->setText("PERÍMETRO");
    label->setAlignment(Qt::AlignCenter);
    label->setToolTip("Inserte perímetro en metros");
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    label->setGraphicsEffect(effect);
    QFont font = label->font();
    font.setPointSize(10);
    font.setFamily("Segoe UI");
    label->setFont(font);

    connect(this, &Mapas_Cercania::deleteLabelInformation, label, &QLabel::deleteLater);
    connect(this, &Mapas_Cercania::deleteLabelInformation, effect, &QGraphicsDropShadowEffect::deleteLater);
    label->setFixedSize(label_width, label_height);
    pos = pos_center;
    pos.setX(pos.x() + circle_radius + 5);
    pos.setY(pos.y() -(label_height/2 + label_height + 5));
    label->move(pos);
    label->show();

    //-----------------------------------------------------------------------------------------------------------
    QLineEdit *lineEdit = new QLineEdit(this); //perimetro
    lineEdit->setStyleSheet("background-color: #FFFFFF;"
                            "border-radius: 3px;"
                            "color:rgb(77, 77, 77);");
    lineEdit->setFont(font);
    lineEdit->setAlignment(Qt::AlignCenter);
    QValidator *validator = new QIntValidator(0, 9999999, this);
    lineEdit->setValidator(validator);
    //    QString mask = "m";
    //    lineEdit->setInputMask(mask);
    lineEdit->setToolTip("Inserte perímetro en metros");
    QGraphicsDropShadowEffect* ef = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    ef->setBlurRadius(20);
    ef->setOffset(1);
    ef->setColor(color_blue_app);
    lineEdit->setGraphicsEffect(ef);

    connect(lineEdit, &QLineEdit::textChanged, this, &Mapas_Cercania::setPerimeter);
    connect(this, &Mapas_Cercania::deleteLabelInformation, lineEdit, &QLineEdit::deleteLater);
    connect(this, &Mapas_Cercania::deleteLabelInformation, ef, &QGraphicsDropShadowEffect::deleteLater);
    lineEdit->setFixedSize(label_width, label_height);
    pos = pos_center;
    pos.setX(pos.x() + circle_radius + 5);
    pos.setY(pos.y() - label_height/2);
    lineEdit->move(pos);
    lineEdit->show();
    lineEdit->setText(QString::number(perimetro, 'f', 0));

    QLabel *label_meter = new QLabel(this); //Tag
    label_meter->setStyleSheet("background-color: #FFFFFF;"
                               "color:rgb(77, 77, 77);");
    label_meter->setFont(font);
    label_meter->setText("m");
    connect(this, &Mapas_Cercania::deleteLabelInformation, label_meter, &QLabel::deleteLater);
    label_meter->setFixedSize(label_height, label_height);
    label_meter->move(lineEdit->pos().x() + lineEdit->width() - label_height,
                      lineEdit->pos().y());
    label_meter->show();
    //-----------------------------------------------------------------------------------------------------------
    QPushButton *pb_filtrar = new QPushButton(this); //perimetro
    pb_filtrar->setStyleSheet("background-color: #FFFFFF;"
                              "border-radius: 3px;"
                              "color:rgb(77, 77, 77);");
    pb_filtrar->setFont(font);
    pb_filtrar->setText(" Filtrar");
    pb_filtrar->setIcon(QIcon(":/icons/filter.png"));
    //    pb_filtrar->setIconSize(QSize(20,20));

    QGraphicsDropShadowEffect* e = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    e->setBlurRadius(20);
    e->setOffset(1);
    e->setColor(color_blue_app);
    pb_filtrar->setGraphicsEffect(e);
    pb_filtrar->setToolTip("Presione para filtrar con perímetro seteado");

    connect(this, &Mapas_Cercania::deleteLabelInformation, pb_filtrar, &QLabel::deleteLater);
    connect(this, &Mapas_Cercania::deleteLabelInformation, e, &QGraphicsDropShadowEffect::deleteLater);
    connect(pb_filtrar, &QPushButton::clicked, this, &Mapas_Cercania::filtrarPerimetro);
    pb_filtrar->setFixedSize(label_width, label_height);
    pos = pos_center;
    pos.setX(pos.x() + circle_radius + 5);
    pos.setY(pos.y() + label_height/2 + 5);
    pb_filtrar->move(pos);
    pb_filtrar->show();

    QTimer::singleShot(1000, this, &Mapas_Cercania::enableClearPerimeterButtons);
}
void Mapas_Cercania::enableClearPerimeterButtons(){
    clearPerimeterButtons_enable = true;
}

void Mapas_Cercania::setPerimeter(QString perimetro_string){
    emit perimeterChanged();
    perimetro_string.remove("m");

    bool ok;
    perimetro = perimetro_string.toDouble(&ok);

    if(!ok){
        return;
    }
    qDebug()<<"marker_model->zoomLevel(): "<<marker_model->zoomLevel();
    double meter_per_pixel = get_Meter_per_Pixel(marker_model->zoomLevel());
    qDebug()<<"meter_per_pixel: "<<meter_per_pixel;
    double radius = perimetro / meter_per_pixel;
    qDebug()<<"radius: "<<radius;
    int circle_radius = static_cast<int>(radius);
    int circle_inner_radius = static_cast<int>(radius*0.85);
    int circle_outer_radius = static_cast<int>(radius*1.15);
    qDebug()<<"circle_radius: "<<circle_radius;
    ClickableLabel *label_centro_inner = new ClickableLabel(this); //punto centro
    label_centro_inner->setStyleSheet("background-color: rgba(54,141,206,30%);"
                                      "border-radius: "+QString::number(circle_inner_radius)+"px;");

    ClickableLabel *label_centro = new ClickableLabel(this); //punto centro
    label_centro->setStyleSheet("background-color: rgba(54,141,206,18%);"
                                "border-radius: "+QString::number(circle_outer_radius)+"px;"
                                                                                       "border: 3px solid #FFFFFF;");
    QGraphicsDropShadowEffect* eff = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    eff->setBlurRadius(20);
    eff->setOffset(1);
    //    eff->setColor(color_blue_app);
    label_centro->setGraphicsEffect(eff);
    connect(label_centro, &ClickableLabel::clicked, label_centro_inner, &ClickableLabel::deleteLater);
    connect(label_centro, &ClickableLabel::clicked, label_centro, &ClickableLabel::deleteLater);
    connect(label_centro, &ClickableLabel::clicked, eff, &QGraphicsDropShadowEffect::deleteLater);
    connect(label_centro, &ClickableLabel::clicked, this, &Mapas_Cercania::hideLabelInformation);

    connect(this, &Mapas_Cercania::perimeterChanged, label_centro_inner, &ClickableLabel::deleteLater);
    connect(this, &Mapas_Cercania::deleteLabelInformation, label_centro_inner, &ClickableLabel::deleteLater);

    connect(this, &Mapas_Cercania::perimeterChanged, label_centro, &ClickableLabel::deleteLater);
    connect(this, &Mapas_Cercania::perimeterChanged, eff, &QGraphicsDropShadowEffect::deleteLater);
    connect(this, &Mapas_Cercania::deleteLabelInformation, label_centro, &ClickableLabel::deleteLater);
    connect(this, &Mapas_Cercania::deleteLabelInformation, eff, &QGraphicsDropShadowEffect::deleteLater);

    label_centro_inner->setFixedSize(circle_inner_radius*2, circle_inner_radius*2);
    label_centro->setFixedSize(circle_outer_radius*2, circle_outer_radius*2);

    QPoint pos = last_center_pos;
    pos.setX(pos.x() - circle_inner_radius);
    pos.setY(pos.y() - circle_inner_radius);
    label_centro_inner->move(pos);
    pos = last_center_pos;
    pos.setX(pos.x() - circle_outer_radius);
    pos.setY(pos.y() - circle_outer_radius);
    label_centro->move(pos);

    label_centro_inner->show();
    label_centro->show();
    label_centro->lower();
    label_centro_inner->lower();
    ui->quickWidget->lower();

    qDebug()<<"Perimetro: " << perimetro;
}
void Mapas_Cercania::filtrarPerimetro(){
    clearPerimeterButtons_enable=true;
    hideLabelInformation();
    if(centro_perimetro.isValid()){
        zoom_level = fillMapWithGeolocalizations(jsonArrayTareas, centro_perimetro, perimetro);
    }
}
QString Mapas_Cercania::getViewOfOperario(QJsonObject jsonObject){
    QString view = "";
    QString field = jsonObject.value(codigo_operario_operarios).toString();
    if(checkIfFieldIsValid(field)){
        view += field +" - ";
    }
    field = jsonObject.value(operario_operarios).toString();
    if(checkIfFieldIsValid(field)){
        view += field;
    }
    field = jsonObject.value(telefono_operarios).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nTelefono: " +field ;
    }
    field = jsonObject.value(nombre_operarios).toString() + " "
            + jsonObject.value(apellidos_operarios).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nNombre: " +field ;
    }
    return view;
}

QString Mapas_Cercania::getViewOfTarea(QJsonObject jsonObject){
    QString view = "";
    QString field = jsonObject.value(poblacion).toString();
    if(checkIfFieldIsValid(field)){
        view += field +", ";
    }
    field = jsonObject.value(calle).toString();
    if(checkIfFieldIsValid(field)){
        view += field +", ";
    }
    field = jsonObject.value(numero).toString();
    if(checkIfFieldIsValid(field)){
        view += field +" ";
    }
    field = jsonObject.value(BIS).toString();
    if(checkIfFieldIsValid(field)){
        view += field;
    }
    field = jsonObject.value(numero_abonado).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nABONADO " + field +", ";
    }
    field = jsonObject.value(nombre_cliente).toString();
    if(checkIfFieldIsValid(field)){
        view += field;
    }
    field = jsonObject.value(tipo_tarea).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nTAREA " + field +", ";
    }
    field = jsonObject.value(numero_serie_contador).toString();
    if(checkIfFieldIsValid(field)){
        view += " CONT "+ field;
    }
    field = jsonObject.value(telefono1).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nTELF1 "+ field + "   ";
    }
    field = jsonObject.value(telefono2).toString();
    if(checkIfFieldIsValid(field)){
        view += "TELF2 "+ field;
    }
    field = jsonObject.value(nuevo_citas).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nCITA "+ field;
    }
    field = jsonObject.value(prioridad).toString().trimmed();
    if(field == "HIBERNAR"){
        field = jsonObject.value(hibernacion).toString().trimmed();
        if(checkIfFieldIsValid(field)){
            QStringList split = field.split("::");
            if(split.size()>1){
                field = split.at(0).trimmed();
            }
            QDateTime dt = QDateTime::fromString(field, formato_fecha_hora);
            view += "\nHIBERNADA HASTA ->  "+ dt.toString(formato_fecha_hora_new_view);
        }
    }
    return view;
}
void Mapas_Cercania::fillTypesAndToolTips(QMap<QString, int> &tipos_t, QMap<QString, QString> &tipos_t_toolTips, QGeoCoordinate coords){
    double xcoord, ycoord;
    QString geoCode, numin;
    for (int i =0; i< jsonArrayTareas.size(); i++) {
        geoCode = jsonArrayTareas.at(i).toObject().value(codigo_de_localizacion).toString();
        if(!checkIfFieldIsValid(geoCode)){
            geoCode = jsonArrayTareas.at(i).toObject().value(geolocalizacion).toString();
        }
        if(checkIfFieldIsValid(geoCode)){
            getCoordsFromString(geoCode, xcoord, ycoord);
            QGeoCoordinate coords_tarea(xcoord, ycoord);
            qDebug()<<"coords_tarea: "<<coords_tarea;
            qDebug()<<"coords: "<<coords;
            if(coords_tarea.isValid() && coords_tarea == coords){
                QString tipo_t = jsonArrayTareas.at(i).toObject().value(tipo_tarea).toString().trimmed();
                QString toolTip = getViewOfTarea(jsonArrayTareas.at(i).toObject());
                if(tipos_t.keys().contains(tipo_t)){
                    int cant = tipos_t.value(tipo_t);
                    QString previousToolTip = tipos_t_toolTips.value(tipo_t);
                    previousToolTip += "\n\n" +toolTip;
                    cant++;
                    tipos_t.remove(tipo_t);
                    tipos_t.insert(tipo_t, cant);
                    if(cant < 11){
                        tipos_t_toolTips.remove(tipo_t);
                        tipos_t_toolTips.insert(tipo_t, previousToolTip);
                    }
                }else{
                    tipos_t.insert(tipo_t, 1);
                    tipos_t_toolTips.insert(tipo_t, toolTip);
                }
            }
        }
    }
}
void Mapas_Cercania::showLabelInformation(QGeoCoordinate coords){
    QPoint pos = this->mapFromGlobal(QCursor::pos());

    if(tooltip_enable && coords.isValid()){
        if(lastEnterCoords!=coords){
            hideLabelInformation();
            lastEnterCoords = coords;
        }
        qDebug()<<"lastEnterCoords: "<<lastEnterCoords;
        QMap<QString, int> tipos_t;
        QMap<QString, QString> tipos_t_toolTips;
        if(mapa_coords_operarios.contains(coords.toString())){
            QJsonArray jsonArray = mapa_coords_operarios.value(coords.toString());
            QString name = "";
            QString view = "";

            for (int i=0; i < jsonArray.size(); i++) {
                QJsonObject jsonObject = jsonArray.at(i).toObject();
                name = jsonObject.value(operario_operarios).toString().trimmed();
                view = getViewOfOperario(jsonObject);

                tipos_t.insert(name, 1);
                tipos_t_toolTips.insert(name, view);
            }

        }else{
            fillTypesAndToolTips(tipos_t, tipos_t_toolTips, coords);
        }

        if(!tipos_t.isEmpty()){
            if(tooltip){
                tooltip = false;
                QWidget *widget = new QWidget(this);
                QVBoxLayout *layout= new QVBoxLayout;
                layout->setAlignment(Qt::AlignCenter);
                widget->setLayout(layout);
                QString tipo_t = "";
                QString tipo_t_toolTip = "";
                for (int i = 0; i < tipos_t.keys().size(); i++) {
                    tipo_t = QString::number(tipos_t.value(tipos_t.keys().at(i))) + " - " + tipos_t.keys().at(i);
                    tipo_t_toolTip = tipos_t_toolTips.value(tipos_t_toolTips.keys().at(i));

                    ClickableLabel *label = new ClickableLabel();
                    label->setText(tipo_t);
                    label->setToolTip(tipo_t_toolTip);
                    label->setToolTipDuration(-1);
                    label->setStyleSheet("background-color: #FFFFFF;"
                                         "border-radius: 3px;"
                                         "color:rgb(54, 141, 206);"
                                         "font: 10pt \"Segoe UI\";");
                    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
                    effect->setBlurRadius(20);
                    effect->setOffset(1);
                    effect->setColor(color_blue_app);
                    label->setGraphicsEffect(effect);
                    connect(this, &Mapas_Cercania::deleteLabelInformation, label, &QLabel::deleteLater);
                    connect(this, &Mapas_Cercania::deleteLabelInformation, effect, &QGraphicsDropShadowEffect::deleteLater);
                    connect(label, &ClickableLabel::textClicked, this, &Mapas_Cercania::printCoordenatesMarkerAndType);
                    label->setAlignment(Qt::AlignCenter);
                    label->setFixedWidth(tipo_t.size()*8);
                    label->setFixedHeight(20);
                    widget->layout()->addWidget(label);

                    qDebug()<<(this->mapFromGlobal(QCursor::pos()));
                }
                widget->setFixedHeight((tipos_t.keys().size() <= 1)? 50 :tipos_t.keys().size() * 30);
                pos.setY(pos.y()-widget->height()/2);
                widget->move(pos);
                connect(this, &Mapas_Cercania::deleteLabelInformation, widget, &QWidget::deleteLater);
                widget->show();
                //                QTimer::singleShot(4000, this, &Mapas_Cercania::hideLabelInformation);
            }
        }
    }
}
void Mapas_Cercania::hideLabelInformation(){
    if(clearPerimeterButtons_enable){
        tooltip_enable = true;
        emit deleteLabelInformation();
        tooltip = true;
    }
    //    qDebug()<<"hideLabelInformation";
}
void Mapas_Cercania::printCoordenatesMarker(QGeoCoordinate coords)//coordenadas del marcador presionado
{
    //    QGeoCoordinate coords;
    qDebug()<<"On Marker "+ QString::number(coords.latitude()) + ", "+QString::number(coords.longitude());
    last_marker_clicked_coords = coords;
    //    QTimer::singleShot(150, this, &Mapas_Cercania::openTareaX);
    double x, y;
    QJsonArray jsonArray;
    QString geoCode;
    for (int i=0; i < jsonArrayTareas.size(); i++) {
        geoCode = jsonArrayTareas.at(i).toObject().value(codigo_de_localizacion).toString();
        if(!checkIfFieldIsValid(geoCode)){
            geoCode = jsonArrayTareas.at(i).toObject().value(geolocalizacion).toString();
        }
        if(checkIfFieldIsValid(geoCode)){
            qDebug()<< geoCode;
            if(getCoordsFromString(geoCode, x, y)==coords){
                jsonArray.append(jsonArrayTareas.at(i).toObject());
            }
        }
    }
    emit coords_selected(jsonArray);
    moveMapCenter(getStringFromCoord(coords));
}
void Mapas_Cercania::openTareaX(){ //Ahora mismo no la estoy usando
    if(last_marker_clicked_coords.isValid()){
        double xcoord, ycoord;
        QString geoCode, numin;
        for (int i =0; i< jsonArrayTareas.size(); i++) {
            geoCode = jsonArrayTareas.at(i).toObject().value(codigo_de_localizacion).toString();
            if(!checkIfFieldIsValid(geoCode)){
                geoCode = jsonArrayTareas.at(i).toObject().value(geolocalizacion).toString();
            }
            if(checkIfFieldIsValid(geoCode)){
                qDebug()<< geoCode;
                getCoordsFromString(geoCode, xcoord, ycoord);
                QGeoCoordinate coords_tarea(xcoord, ycoord);
                if(coords_tarea.isValid() && coords_tarea == last_marker_clicked_coords){
                    numin = jsonArrayTareas.at(i).toObject().value(principal_variable).toString().trimmed();
                    last_marker_clicked_coords = QGeoCoordinate();
                    emit openTarea(numin);
                    break;
                }
            }
        }
    }
}
void Mapas_Cercania::printCoordenatesMarkerAndType(QString tipo_t)//coordenadas del marcador presionado
{
    //    QGeoCoordinate coords;
    qDebug()<<"On Marker type "+ tipo_t;
    lastTipoTareaSelected = tipo_t.split("-").at(1).trimmed();
    QTimer::singleShot(150, this, &Mapas_Cercania::openTareaXconTipo);
}
void Mapas_Cercania::openTareaXconTipo(){
    if(lastEnterCoords.isValid()){
        double xcoord, ycoord;
        QString geoCode, numin, tipo_t;
        if(mapa_coords_operarios.contains(lastEnterCoords.toString())){
            QJsonArray jsonArray = mapa_coords_operarios.value(lastEnterCoords.toString());
            QJsonObject jsonObject;
            for (int i=0; i < jsonArray.size(); i++) {
                jsonObject = jsonArray.at(i).toObject();
                QString name = jsonObject.value(operario_operarios).toString().trimmed();
                if(lastTipoTareaSelected.contains(name)){
                    break;
                }

            }
            Operario *operario = new Operario(nullptr, false, empresa);
            connect(operario, &Operario::update_tableOperarios,this,
                    &Mapas_Cercania::getOperariosFromServer);
            operario->show();
            operario->setData(jsonObject);
        }else{
            for (int i =0; i< jsonArrayTareas.size(); i++) {
                geoCode = jsonArrayTareas.at(i).toObject().value(codigo_de_localizacion).toString();
                if(!checkIfFieldIsValid(geoCode)){
                    geoCode = jsonArrayTareas.at(i).toObject().value(geolocalizacion).toString();
                }
                if(checkIfFieldIsValid(geoCode)){
                    tipo_t = jsonArrayTareas.at(i).toObject().value(tipo_tarea).toString().trimmed();
                    qDebug()<< geoCode;
                    getCoordsFromString(geoCode, xcoord, ycoord);
                    QGeoCoordinate coords_tarea(xcoord, ycoord);
                    if(coords_tarea.isValid() && coords_tarea == lastEnterCoords && lastTipoTareaSelected == tipo_t){
                        numin = jsonArrayTareas.at(i).toObject().value(principal_variable).toString().trimmed();
                        //                lastEnterCoords = QGeoCoordinate();
                        emit openTarea(numin);
                        break;
                    }
                }
            }
        }
    }
}

bool Mapas_Cercania::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

QString Mapas_Cercania::getStringFromCoord(QGeoCoordinate coords){
    if(coords.isValid()){
        QString latitud = QString::number(coords.latitude(), 'f', 13);
        QString longitud = QString::number(coords.longitude(), 'f', 13);
        //        qDebug()<<"Coords: -> "<<coords;
        //        qDebug()<<"Conversion: -> " + latitud + "," + longitud;
        return latitud + "," + longitud;
    }
    return "";
}

QGeoCoordinate Mapas_Cercania::getCoordsFromString(QString string, double &xcoord, double &ycoord){
    QStringList split;
    split = string.split(",");
    if(split.length() > 1){
        QString x_string = split.at(0).trimmed();
        QString y_string = split.at(1).trimmed();
        bool x_ok, y_ok;
        xcoord = x_string.toDouble(&x_ok);
        ycoord = y_string.toDouble(&y_ok);
        //        qDebug()<<"xcoord: -> "+QString::number(xcoord, 'g', 13);
        //        qDebug() << "xcoord: " << QString("%1").arg(xcoord, 0, 'e', 13);
        //        qDebug()<<"ycoord: -> "+QString::number(ycoord, 'g', 13);
        //        qDebug()<<"x_string: -> "+x_string;
        //        qDebug()<<"y_string: -> "+y_string;
        if(x_ok && y_ok){
            return QGeoCoordinate(xcoord, ycoord);
        }
    }
    return QGeoCoordinate();
}

void Mapas_Cercania::initMarker(double xcoord, double ycoord){
    marker_model->setMaxMarkers(1);
    if(QGeoCoordinate(xcoord, ycoord).isValid()){
        marker_model->moveMarker(QGeoCoordinate(xcoord, ycoord));
    }
}

double Mapas_Cercania::getYcoordenade()
{
    return marker_model->yposition();
}
double Mapas_Cercania::getXcoordenade()
{
    return marker_model->xposition();
}
double Mapas_Cercania::getZoomLevel()
{
    return zoom_level;
}

void Mapas_Cercania::on_pb_home_casa_clicked()
{
    if(hand_home){
        hand_home = false;
        delete marker_model;
        marker_model = new MarkerModelCercania(this, xcoordenade,  ycoordenade);
        //        ui->pb_home_casa->setPixmap(QPixmap(":/icons/home.png"));
        if(QGeoCoordinate(xcoordenade, ycoordenade).isValid()){
            marker_model->moveMarker(QGeoCoordinate(xcoordenade, ycoordenade));
        }

    }else{
        hand_home = true;
        //        ui->pb_home_casa->setPixmap(QPixmap(":/icons/hand.png"));
        delete marker_model;
        marker_model = new MarkerModelCercania(this, xcoordenade_hand,  ycoordenade_hand);
        if(QGeoCoordinate(xcoordenade_hand, ycoordenade_hand).isValid()){
            marker_model->moveMarker(QGeoCoordinate(xcoordenade_hand, ycoordenade_hand));
        }
    }
    ui->quickWidget->rootContext()->setContextProperty("w",this);
    ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);
}

void Mapas_Cercania::on_pb_erase_clicked()
{
    delete marker_model;

    if(hand_home){
        marker_model = new MarkerModelCercania(this, xcoordenade_hand,  ycoordenade_hand);
        xcoordenade_hand = 1000;
        ycoordenade_hand = 1000;
        emit erase_marker_hand();
    }else{
        marker_model = new MarkerModelCercania(this, xcoordenade,  ycoordenade);
        xcoordenade = 1000;
        ycoordenade = 1000;
        emit erase_marker_home();
    }

    marker_model->setMaxMarkers(1);

    ui->quickWidget->rootContext()->setContextProperty("w",this);
    ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);
}

void Mapas_Cercania::on_l_max_min_clicked() ///ESta funcion no sirve en esta clase hay que modificar
{
    if(this->isMaximized()){
        this->showNormal();
        on_pb_home_casa_clicked();
        on_pb_home_casa_clicked();
        ui->lineEdit->setFixedSize(ui->lineEdit->size()/1.5);
        QFont font = ui->lineEdit->font();
        font.setPointSize(static_cast<int>(font.pointSize()/1.2));
        ui->lineEdit->setFont(font);
        ui->pb_map_search->setFixedSize(ui->lineEdit->height(), ui->lineEdit->height());
        ui->pb_map_search->move(ui->pb_map_search->pos().x(), ui->lineEdit->pos().y());
    }else{
        this->showMaximized();
        on_pb_home_casa_clicked();
        on_pb_home_casa_clicked();
        ui->lineEdit->setFixedSize(ui->lineEdit->size()*1.5);
        QFont font = ui->lineEdit->font();
        font.setPointSize(static_cast<int>(font.pointSize()*1.2));
        ui->lineEdit->setFont(font);
        ui->pb_map_search->setFixedSize(ui->lineEdit->height(), ui->lineEdit->height());
        ui->pb_map_search->move(ui->pb_map_search->pos().x(), ui->lineEdit->pos().y());
    }
}



void Mapas_Cercania::setAndEmitCoords(QGeoCoordinate coords){
    if(hand_home){
        QString coord = getStringFromCoord(coords);
        if(!coord.isEmpty()){
            emit settedMarkerHand(coord);
        }
        xcoordenade_hand = coords.latitude();
        ycoordenade_hand = coords.longitude();
    }else{
        QString coord = getStringFromCoord(coords);
        if(!coord.isEmpty()){
            emit settedMarker(coord);
        }
        xcoordenade = coords.latitude();
        ycoordenade = coords.longitude();
    }
}

void Mapas_Cercania::searchSuggestions(QString suggestion){
    qDebug()<<"Buscando -> " + suggestion;
    QPlaceSearchRequest request;
    request.setSearchTerm(suggestion);
    request.setSearchArea(QGeoCircle(QGeoCoordinate(40.4636688, -3.7492199)));
    request.setLimit(10);
    suggestionReply = manager->searchSuggestions(request);
    connect(suggestionReply, &QPlaceSearchSuggestionReply::finished, this, &Mapas_Cercania::handleSuggestionReply);
}
void Mapas_Cercania::searchResult(QString item, int maxResults)
{
    //instantiate request and set parameters
    QPlaceSearchRequest searchRequest;
    searchRequest.setSearchTerm(item);
    searchRequest.setSearchArea(QGeoCircle(QGeoCoordinate(40.4636688, -3.7492199)));
    searchRequest.setLimit(maxResults); //specify how many results are to be retrieved.
    //send off a search request
    searchReply = manager->search(searchRequest);

    //connect a slot to handle the reply
    connect(searchReply, &QPlaceSearchReply::finished, this, &Mapas_Cercania::handleSearchReply);
}

void Mapas_Cercania::searchItem(QString item)
{
    lastSearchItem = "";
    searchResult(item);
}

void Mapas_Cercania::handleSearchReply()
{
    if (searchReply->error() == QPlaceReply::NoError) {
        lastListResults = searchReply->results();
        foreach (const QPlaceSearchResult &result, lastListResults) {
            if (result.type() == QPlaceSearchResult::PlaceResult) {
                posSearch =0;
                QPlaceResult placeResult = result;
                qDebug() << "Name: " << placeResult.place().name();
                double xcoord, ycoord;
                xcoord = placeResult.place().location().coordinate().latitude();
                ycoord = placeResult.place().location().coordinate().longitude();
                qDebug() << "Coordinate latitude " << xcoord;
                qDebug() << "Coordinate logitude " << ycoord;
                qDebug() << "Street: " << placeResult.place().location().address().street();
                qDebug() << "Distance: " << placeResult.distance();

                if(QGeoCoordinate(xcoord,  ycoord).isValid()){
                    qDebug()<<"Valido";
                    zoom_level = fillMapWithGeolocalizations(jsonArrayTareas, QGeoCoordinate(xcoord,  ycoord));
                    marker_model->setZoomLevel(18);
                    setPerimeterButtons(QPoint(this->width()/2, this->height()/2));
                }
                else{
                    qDebug()<<"No valido";
                }
            }
        }
    }
    searchReply->deleteLater();  //discard reply
    searchReply = 0;
}

void Mapas_Cercania::handleSuggestionReply()
{
    qDebug()<<"handleSuggestionReply...";
    if (suggestionReply->error() == QPlaceReply::NoError) {
        qDebug()<<"QPlaceReply::NoError";
        ui->lineEdit->setAutoCompleteList(suggestionReply->suggestions());
        foreach (const QString &suggestion, suggestionReply->suggestions())
            qDebug() <<"Suggestion Init: -> "<< suggestion << "... -> Suggestion end";
    }else{
        qDebug()<<"QPlaceReply::Error";
    }
    suggestionReply->deleteLater(); //discard reply
    suggestionReply = 0;
}

void Mapas_Cercania::on_pb_map_search_clicked()
{
    ui->lineEdit->stopChangeThresholdAndHide();
    if(lastSearchItem == ui->lineEdit->text()){
        if(!lastSearchItem.isEmpty()){
            posSearch++;
            if(posSearch >= lastListResults.size()){
                posSearch =0;
            }
            QPlaceSearchResult result = lastListResults.at(posSearch);
            if (result.type() == QPlaceSearchResult::PlaceResult) {

                QPlaceResult placeResult = result;
                qDebug() << "Name: " << placeResult.place().name();
                double xcoord, ycoord;
                xcoord = placeResult.place().location().coordinate().latitude();
                ycoord = placeResult.place().location().coordinate().longitude();

                if(QGeoCoordinate(xcoord,  ycoord).isValid()){
                    qDebug()<<"Valido iteracion";
                    zoom_level = fillMapWithGeolocalizations(jsonArrayTareas, QGeoCoordinate(xcoord,  ycoord));
                    marker_model->setZoomLevel(18);
                    setPerimeterButtons(QPoint(this->width()/2, this->height()/2));
                }
                else{
                    qDebug()<<"No valido";
                }
            }
        }
    }else{
        lastSearchItem = ui->lineEdit->text();
        if(!lastSearchItem.isEmpty()){
            searchResult(lastSearchItem, 10);
        }
    }
}
double Mapas_Cercania::getZoomLevelWithDistance(double distance){ //zoom para ver esta distancia
    double zoom_d = log2((int)(40000 / (distance / 2)));
    int zoom_l = (int)(floor(zoom_d));
    return zoom_l;
}
double Mapas_Cercania::getDistanceWithZoomLevel(double zoom_l){ //zoom para ver esta distancia
    double distance = 80000/(pow(2, zoom_l));
    return distance * 1000; //*1000 metros
}
double Mapas_Cercania::degreesToRadians(double degrees) {
    return degrees * 3.141592 / 180;
}

double Mapas_Cercania::distanceInKmBetweenEarthCoordinates(double lat1, double lon1, double lat2, double lon2) {
    double earthRadiusKm = 6371;

    double dLat = degreesToRadians(lat2-lat1);
    double dLon = degreesToRadians(lon2-lon1);

    lat1 = degreesToRadians(lat1);
    lat2 = degreesToRadians(lat2);

    double a = sin(dLat/2) * sin(dLat/2) +
            sin(dLon/2) * sin(dLon/2) * cos(lat1) * cos(lat2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return earthRadiusKm * c;
}
void Mapas_Cercania::on_pb_close_clicked()
{
    disconnect(&timer_hibernacion, &QTimer::timeout, this, &Mapas_Cercania::getOperariosFromServer);
    timer_hibernacion.stop();
    emit closing();
    this->close();
}



void Mapas_Cercania::on_l_mostrar_en_tabla_clicked()
{
    if(jsonArrayTareasForTable.isEmpty()){
        GlobalFunctions::showMessage(this, "Sin Resultados", "No hay tareas para mostrar en tabla");
    }else{
        show_loading("Descargando tareas...");
        QStringList princ_vars;
        for (int i=0; i < jsonArrayTareasForTable.size(); i++) {
            QJsonObject jsonObject  = jsonArrayTareasForTable.at(i).toObject();
            QString princ_var = jsonObject.value(numero_interno).toString();
            if(!princ_vars.contains(princ_var)){
                princ_vars << princ_var;
            }
        }
        emit showJsonArrayInTable(princ_vars);
        on_pb_close_clicked();
    }
}



void Mapas_Cercania::on_pb_close_filter_zona_clicked()
{
    ui->widget_zona->close();
}

void Mapas_Cercania::on_pb_show_filter_zona_clicked()
{
    ui->widget_zona->show();
}

void Mapas_Cercania::on_pb_add_zonas_clicked()
{
    emit show_filter();
    filterColumnList.clear();
    lastSectionField = zona;
    showFilterWidgetOptions(zona);
}
void Mapas_Cercania::on_pb_add_blocks_clicked()
{
    emit show_filter();
    filterColumnList.clear();
    lastSectionField = dia_predeterminado;
    showFilterWidgetOptions(dia_predeterminado);
}

QStringList Mapas_Cercania::getFieldValues(QString field){
    QJsonArray jsonArray = jsonArrayTareas;
    QStringList values;
    QString value;
    for (int i=0; i < jsonArray.size(); i++) {
        value = jsonArray.at(i).toObject().value(field).toString();
        if(checkIfFieldIsValid(value)){
            if(!values.contains(value)){
                values << value;
            }
        }
    }
    values.sort();
    return values;
}

void Mapas_Cercania::filterColumnField(){

    if(filterColumnList.isEmpty()){
        return;
    }
    filterField(filterColumnList, lastSectionField);
    if(lastSectionField == zona){
        disconnect(ui->cb_zonas, &MyComboBoxShine::currentTextChanged, this, &Mapas_Cercania::filterZona);
        ui->cb_zonas->clear();
        ui->cb_zonas->addItems(filterColumnList);
        connect(ui->cb_zonas, &MyComboBoxShine::currentTextChanged, this, &Mapas_Cercania::filterZona);
    }
}
QString Mapas_Cercania::getScrollBarStyle(){
    QString style =
            "QScrollBar:vertical{"
            "border: 2px #777777;"
            "background-color: solid #777777;"
            "border-radius: 5px;"
            "width: 10px;"
            "margin: 3px 0px 3px 0px;"
            "}"

            "QScrollBar::handle:vertical{"
            "background-color: #777777;"
            "border-radius: 5px;"
            "min-height: 20px;"
            "}"

            "QScrollBar::add-line:vertical{"
            "border: 2px solid white;"
            "background: solid white;"
            "border-radius: 5px;"
            "height 10px;"
            "subcontrol-position: bottom;"
            "subcontrol-origin: margin;"
            "}"

            "QScrollBar::sub-line:vertical{"
            "border: 2px solid white;"
            "background: solid white;"
            "border-radius: 5px;"
            "height 10px;"
            "subcontrol-position: top;"
            "subcontrol-origin: margin;"
            "}";

    return style;
}
void Mapas_Cercania::showFilterWidgetOptions(QString field){

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(5);
    vlayout->setSpacing(0);
    vlayout->setObjectName("v_layout");
    vlayout->setAlignment(Qt::AlignCenter);

    QWidget *widgetValues = new QWidget;
    widgetValues->setStyleSheet("background-color: rgb(77, 77, 77);"
                                "border-radius: 5px;");
    widgetValues->setLayout(vlayout);

    MyLineEditShine *lineEdit = new MyLineEditShine();
    QCheckBox *cb_todos = new QCheckBox();
    cb_todos->setStyleSheet("color: rgb(255, 255, 255);"
                            "background-color: rgba(77, 77, 77);");
    QFont f = cb_todos->font();
    f.setFamily("Segoe UI");
    f.setPointSize(9);
    cb_todos->setText("Todos");
    cb_todos->setFont(f);

    int itemHeight = 35;
    QStringList values = getFieldValues(field);
    QString value;
    int width = 100;
    foreach(value, values){
        MyCheckBox *cb = new MyCheckBox();
        cb->setText(value);
        cb->setObjectName("cb_"+value);
        cb->setFixedHeight(itemHeight-5);
        cb->setStyleSheet("color: rgb(255, 255, 255);"
                          "background-color: rgba(77, 77, 77);");
        connect(cb, &MyCheckBox::toggleCheckBox, this, &Mapas_Cercania::addRemoveFilterList);
        connect(lineEdit, &MyLineEditShine::textChanged, cb, &MyCheckBox::onTextSelectedChanged);
        connect(cb_todos, &QCheckBox::toggled, cb, &MyCheckBox::set_Checked);

        QFont font = cb->font();
        font.setFamily("Segoe UI");
        font.setPointSize(9);

        cb->setFont(font);

        widgetValues->layout()->addWidget(cb);
        if((value.size() + 1) * 10  > width){
            if((value.size() + 1) * 10 < 500){
                width = (value.size() + 1) * 10;
            }
        }
    }
    QPushButton *button_filter = new QPushButton("FILTRAR  ");
    button_filter->setStyleSheet("color: rgb(255, 255, 255);"
                                 "background-color: rgba(77, 77, 77);");
    button_filter->setIcon(QIcon(":/icons/filter.png"));
    button_filter->setIconSize(QSize(20, 20));
    connect(button_filter, &QPushButton::clicked, this, &Mapas_Cercania::filterColumnField);

    QFont font = button_filter->font();
    font.setFamily("Segoe UI");
    font.setBold(true);
    font.setPointSize(10);

    button_filter->setFont(font);

    //    widgetValues->setFixedHeight(35 * (values.size() + 1));
    widgetValues->setFixedWidth(width);

    QScrollArea *scroll = new QScrollArea;
    QWidget *widget = new QWidget(this);

    widget->setFixedSize((widgetValues->width() > 500)? 580 : widgetValues->width() + 80,
                         (values.size() > 10)? 515: (values.size() * itemHeight + 170));

    lineEdit->setStyleSheet("color: rgb(77, 77, 77);"
                            "background-color: rgb(255, 255, 255);");
    lineEdit->setFixedHeight(20);
    lineEdit->setFixedWidth(widgetValues->width());

    QCompleter *completer = new QCompleter(values, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    lineEdit->setCompleter(completer);

    widget->setStyleSheet("background-color: rgba(77, 77, 77);"
                          "border-radius: 5px;");
    QVBoxLayout *layout = new QVBoxLayout(widget);

    scroll->setWidget(widgetValues);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet(getScrollBarStyle());

    layout->setMargin(15);
    layout->setSpacing(15);

    layout->addWidget(lineEdit);
    layout->addWidget(cb_todos);
    layout->addWidget(scroll);
    layout->addWidget(button_filter);

    layout->setAlignment(Qt::AlignCenter);

    QPoint lastCursorPos(this->width()/2, 150);
    QRect rect = this->geometry();
    if(lastCursorPos.x() > rect.width()/2){
        lastCursorPos.setX(lastCursorPos.x()-widget->width());
    }
    widget->move(lastCursorPos);
    widget->show();

    connect(button_filter, &QPushButton::clicked, widget, &QWidget::deleteLater);
    connect(this, &Mapas_Cercania::show_filter, widget, &QWidget::deleteLater);
    connect(this, &Mapas_Cercania::closing, widget, &QWidget::deleteLater);
    connect(this, &Mapas_Cercania::deleteLabelInformation, widget, &QWidget::deleteLater);
}

void Mapas_Cercania::addRemoveFilterList(QString value){
    if(filterColumnList.contains(value)){
        filterColumnList.removeOne(value);
    }else{
        filterColumnList << value;
    }
}

void Mapas_Cercania::on_pb_map_type_change_clicked()
{
    mapType++;
    if(mapType > 3){ mapType = 1;}

    if(marker_model){
        marker_model->setMapType(mapType);
    }
}
void Mapas_Cercania::show_loading(QString mess){
    emit hidingLoading();

    QWidget *widget_blur = new QWidget(this);
    widget_blur->setFixedSize(this->size()+QSize(0,0));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &Mapas_Cercania::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &Mapas_Cercania::hidingLoading, widget_blur, &QWidget::deleteLater);

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
    connect(this, &Mapas_Cercania::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &Mapas_Cercania::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &Mapas_Cercania::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &Mapas_Cercania::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &Mapas_Cercania::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}

void Mapas_Cercania::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void Mapas_Cercania::hide_loading(){
    emit hidingLoading();
}

