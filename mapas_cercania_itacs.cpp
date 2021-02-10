#include "mapas_cercania_itacs.h"
#include "ui_mapas_cercania_itacs.h"

#include <QQmlContext>
#include "itac.h"
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
#include <QAbstractItemView>
#include <QScrollArea>
#include "mylineeditshine.h"
#include "mycheckbox.h"
#include "globalfunctions.h"

Mapas_Cercania_Itacs::Mapas_Cercania_Itacs(QWidget *parent, QJsonArray jsonArray, QString empresa) :
    QWidget(parent),
    ui(new Ui::Mapas_Cercania_Itacs)
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

    jsonArrayITACs = jsonArray;
    QString geoCode = "";
    for (int i=0; i < jsonArrayITACs.size(); i++) {
        geoCode = jsonArrayITACs.at(i).toObject().
                value(geolocalizacion_itacs).toString().trimmed();
        if(checkIfFieldIsValid(geoCode)){
            first_coords = getCoordsFromString(geoCode, first_latitud, first_longitud);
            if(first_coords.isValid()){
                break;
            }
        }
    }
    if(first_coords.isValid()){
        zoom_level = fillMapWithGeolocalizations(jsonArrayITACs, first_coords);
    }

    ui->quickWidget->setSource(QUrl("qrc:/qml/mapview.qml"));

    connect(ui->lineEdit, &MyLineEditAutoComplete::textModified, this, &Mapas_Cercania_Itacs::searchSuggestions);
    connect(ui->lineEdit, &MyLineEditAutoComplete::itemSelected, this, &Mapas_Cercania_Itacs::searchItem);

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

    //    timer_hibernacion.setInterval(60000);
    //    connect(&timer_hibernacion, &QTimer::timeout, this, &Mapas_Cercania_Itacs::checkHibernation);
    //    timer_hibernacion.start();

    ui->l_max_min->hide();
}

Mapas_Cercania_Itacs::~Mapas_Cercania_Itacs()
{
    delete ui;
}
QString Mapas_Cercania_Itacs::getMapPluginName(){
    return mapPluginName;
}
QString Mapas_Cercania_Itacs::getMapPluginParameterName(){
    return mapPluginParameterName;
}
QString Mapas_Cercania_Itacs::getMapPluginParameterValue(){
    return mapPluginParameterValue;
}
int Mapas_Cercania_Itacs::getMapType(){
    return mapType;
}
void Mapas_Cercania_Itacs::setCenter(QGeoCoordinate coords)
{
    if(marker_model){
        marker_model->setCenter(coords.latitude(), coords.longitude());
    }
}

void Mapas_Cercania_Itacs::setZoomLevel(double zoom)
{
    if(marker_model){
        marker_model->setZoomLevel(zoom);
    }
}

void Mapas_Cercania_Itacs::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    ui->quickWidget->setFixedSize(this->size());
    ui->quickWidget->move(0,0);
    //    ui->pb_home_casa->move(this->width() - 50, this->height()/2 -50);
    //    ui->pb_erase->move(this->width() - 50, this->height()/2);

    ui->lineEdit->move(this->width()/2 - ui->lineEdit->width()/2, 10);

    ui->pb_close->move(this->width() - 50, 10);
    ui->pb_map_search->move(ui->lineEdit->pos().x() + ui->lineEdit->width() + 10,
                            ui->pb_map_search->pos().y());
    //    ui->l_max_min->move(this->width() - 100, 10);
    ui->l_mostrar_en_tabla->move(this->width() - 100, 10);
    ui->pb_show_filter_zona->move(this->width() - 150, 10);
    ui->widget_zona->move(this->width() - 310, 60);
    ui->l_cantidad_de_tareas->move(10, this->height() - 40);

    ui->pb_map_type_change->move(10, this->height() - 90);

}

//bool Mapas_Cercania_Itacs::checkHibernation(){
//    QJsonObject numeros_internos, campos;
//    QGeoCoordinate center;
//    QString priority = "MEDIA";
//    for (int i=0; i < tareas_hibernadas.keys().size(); i++) {
//        QString numInterno = tareas_hibernadas.keys().at(i);
//        QDateTime dt = tareas_hibernadas.value(numInterno);

//        if(dt <= QDateTime::currentDateTime()){
//            numeros_internos.insert(QString::number(i), numInterno);
//            tareas_hibernadas.remove(numInterno);

//            for (int i= 0; i < jsonArrayITACs.size(); i++) {
//                QJsonObject jsonObject = jsonArrayITACs.at(i).toObject();
//                QString numin = jsonObject.value(principal_variable).toString().trimmed(); //mano
//                QString geocode = jsonObject.value(codigo_de_localizacion).toString().trimmed(); //mano
//                if(!checkIfFieldIsValid(geocode)){
//                    geocode = jsonObject.value(geolocalizacion).toString().trimmed();//casa
//                }
//                if(numin == numInterno){
//                    double x, y;
//                    center = getCoordsFromString(geocode, x, y);
//                    QString field = jsonObject.value(hibernacion).toString().trimmed();
//                    if(checkIfFieldIsValid(field)){
//                        QStringList split = field.split("::");
//                        if(split.size()>1){
//                            priority = split.at(1).trimmed();
//                        }
//                    }
//                    jsonObject.insert(prioridad, priority);
//                    jsonArrayITACs[i] = jsonObject;
//                    break;
//                }
//            }
//        }
//    }

//    if(numeros_internos.isEmpty()){
//        return false;
//    }
//    campos.insert(prioridad, priority);

//    QStringList keys, values;
//    QJsonDocument d;
//    d.setObject(numeros_internos);
//    QByteArray ba = d.toJson(QJsonDocument::Compact);
//    QString temp_fields, temp_numins = QString::fromUtf8(ba);

//    d.setObject(campos);
//    ba = d.toJson(QJsonDocument::Compact);
//    temp_fields = QString::fromUtf8(ba);

//    keys << "json_numin" << "json_fields" << "empresa";
//    values << temp_numins << temp_fields << empresa.toLower();

//    this->keys = keys;
//    this->values = values;

//    QEventLoop *q = new QEventLoop();

//    connect(this, &Mapas_Cercania_Itacs::script_excecution_result,q,&QEventLoop::exit);
//    QTimer::singleShot(DELAY, this, &Mapas_Cercania_Itacs::update_tareas_fields_request);

//    double previous_zoom = marker_model->zoomLevel();
//    fillMapWithGeolocalizations(jsonArrayITACs, center);
//    marker_model->setZoomLevel(previous_zoom);

//    bool res = false;
//    switch(q->exec())
//    {
//    case database_comunication::script_result::timeout:
//        res = false;
//        break;

//    case database_comunication::script_result::ok:
//        emit updateTareas(1);
//        res = true;
//        break;

//    case database_comunication::script_result::update_tareas_fields_to_server_failed:
//        res = false;
//        break;
//    }
//    delete q;

//    return res;
//}
//void Mapas_Cercania_Itacs::update_tareas_fields_request(){
//    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
//            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
//    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA_FIELDS,keys,values);
//}
//void Mapas_Cercania_Itacs::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
//{
//    QString respuesta = QString::fromUtf8(ba);
//    int result = -1;
//    if(tipo == database_comunication::UPDATE_TAREA_FIELDS)
//    {
//        qDebug()<<respuesta;
//        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

//        if(ba.contains("ot success update_tarea_fields"))
//        {
//            result = database_comunication::script_result::update_tareas_fields_to_server_failed;
//        }
//        else
//        {
//            if(ba.contains("success ok update_tarea_fields"))
//            {
//                result = database_comunication::script_result::ok;

//            }
//        }
//    }
//    emit script_excecution_result(result);
//}

void Mapas_Cercania_Itacs::emitUpdateITACs(QJsonObject data){
    emit updateITACs();
    QString prin_var = data.value(codigo_itac_itacs).toString();
    for (int i= 0; i < jsonArrayITACs.size(); i++) {
        QJsonObject jsonObject = jsonArrayITACs.at(i).toObject();
        QString cod_emplazamiento = jsonObject.value(codigo_itac_itacs).toString().trimmed(); //mano
        if(cod_emplazamiento == prin_var){
            jsonObject.insert(date_time_modified_itacs, data.value(date_time_modified_itacs));
            jsonArrayITACs[i] = jsonObject;
            break;
        }
    }
    double previous_zoom = marker_model->zoomLevel();
    fillMapWithGeolocalizations(jsonArrayITACs, marker_model->getCurrentCenter());
    marker_model->setZoomLevel(previous_zoom);
}
bool Mapas_Cercania_Itacs::comparePriority(QString firstPriority, QString secondPriority){ //true si la seguda prioridad es mayor a la primera
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
double Mapas_Cercania_Itacs::fillMapWithGeolocalizations(QJsonArray jsonArray, QGeoCoordinate center, double perimetro){

    qDebug()<<"center: "<< center;
    double distance, max_distance = 0;
    double longitud, latitud;

    jsonArrayITACsForTable = QJsonArray();

    delete marker_model;
    marker_model = new MarkerModelCercania(this, center.latitude(), center.longitude());
    marker_model->setMaxMarkers(jsonArray.size() + 1);

    int cant_mostradas =0;
    QStringList zonas;
    QList<QGeoCoordinate> lista_coordenadas;
    for (int i= 0; i < jsonArray.size(); i++) {
        QString pric_var = jsonArray.at(i).toObject().value(codigo_itac_itacs).toString().trimmed();
        QString direccion = jsonArray.at(i).toObject().value(itac_itacs).toString().trimmed();
        QString prioridad_l = jsonArray.at(i).toObject().value(prioridad_itacs).toString().trimmed();
        QString geocode = jsonArray.at(i).toObject().value(geolocalizacion_itacs).toString().trimmed(); //mano
        if(completer_zonas == nullptr){
            QString zona_l = jsonArray.at(i).toObject().value(zona_itacs).toString().trimmed(); //mano
            if(checkIfFieldIsValid(zona_l) && !zonas.contains(zona_l)){
                zonas << zona_l;
            }
        }
        if(!checkIfFieldIsValid(prioridad_l)){
            prioridad_l = "MEDIA";//naranja
        }

        if(checkIfFieldIsValid(geocode)){
            QGeoCoordinate coords = getCoordsFromString(geocode, latitud, longitud);
            if(coords.isValid() && marker_model != nullptr)
            {
                if(!lista_coordenadas.contains(coords)){
                    distance = distanceInKmBetweenEarthCoordinates(center.latitude(), center.longitude(), latitud, longitud);
                    if((perimetro > 0 && distance*1000 <= perimetro) || (perimetro <= 0)){
                        //El perimetro esta en metros y la distancia en Km

                        mapa_coords_strings.insert(coords.toString(),direccion);
                        mapa_coords_priority.insert(coords.toString(),prioridad_l);

                        if(distance > max_distance){
                            max_distance = distance;
                        }
                        marker_model->setText(QString::number(i));
                        marker_model->moveMarker(coords);
                        lista_coordenadas.append(coords);
                        cant_mostradas++;
                        jsonArrayITACsForTable.append(jsonArray.at(i).toObject());
                    }
                }else{
                    cant_mostradas++;
                    jsonArrayITACsForTable.append(jsonArray.at(i).toObject());
                    if(comparePriority(mapa_coords_priority.value(coords.toString()), prioridad_l)){
                        mapa_coords_priority.remove(coords.toString());
                        mapa_coords_priority.insert(coords.toString(),prioridad_l);
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
        connect(ui->cb_zonas, &MyComboBoxShine::currentTextChanged, this, &Mapas_Cercania_Itacs::filterZona);
    }

    ui->l_cantidad_de_tareas->setText("Resultado: "+QString::number(cant_mostradas)
                                      + ((cant_mostradas != 1)?" ITACs":" ITAC"));

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

    return zoom;
}
void Mapas_Cercania_Itacs::filterZonas(QStringList zonas_selected){
    QJsonArray jsonArrayFiltered;
    QGeoCoordinate first_coords;
    for (int i= 0; i < jsonArrayITACs.size(); i++) {
        QString zona_tarea = jsonArrayITACs.at(i).toObject().value(zona_itacs).toString().trimmed();
        if(zonas_selected.contains(zona_tarea)){
            jsonArrayFiltered.append(jsonArrayITACs.at(i).toObject());
            if(!first_coords.isValid()){
                QString geoCode = jsonArrayITACs.at(i).toObject().
                        value(geolocalizacion_itacs).toString().trimmed();
                if(checkIfFieldIsValid(geoCode)){
                    first_coords = getCoordsFromString(geoCode, first_latitud, first_longitud);
                }
            }
        }
    }
    zoom_level = fillMapWithGeolocalizations(jsonArrayFiltered, first_coords);
}

void Mapas_Cercania_Itacs::filterZona(QString zona_selected){
    QJsonArray jsonArrayFiltered;
    QGeoCoordinate first_coords;
    for (int i= 0; i < jsonArrayITACs.size(); i++) {
        QString zona_itac = jsonArrayITACs.at(i).toObject().value(zona_itacs).toString().trimmed(); //mano
        if(zona_itac == zona_selected){
            jsonArrayFiltered.append(jsonArrayITACs.at(i).toObject());
            if(!first_coords.isValid()){
                QString geoCode = jsonArrayITACs.at(i).toObject().
                        value(geolocalizacion_itacs).toString().trimmed();
                if(checkIfFieldIsValid(geoCode)){
                    first_coords = getCoordsFromString(geoCode, first_latitud, first_longitud);
                }
            }
        }
    }
    zoom_level = fillMapWithGeolocalizations(jsonArrayFiltered, first_coords);
}
//void Mapas_Cercania_Itacs::showResumen(){
//    QPoint thisPos = this->mapToGlobal(QPoint(0,0));
//    if(resumen == nullptr){
//        resumen = new Resumen_Tareas(nullptr, jsonArrayITACsForTable, empresa);
//        resumen->move(thisPos.x() + this->width() + 10, thisPos.y()-7);

//        connect(this, &Mapas_Cercania_Itacs::closing, resumen, &Resumen_Tareas::close);
//        connect(resumen, &Resumen_Tareas::moveMapCenter,
//                this, &Mapas_Cercania_Itacs::moveMapCenter);
//        connect(resumen, &Resumen_Tareas::openTarea,
//                this, &Mapas_Cercania_Itacs::openTareaWithNumin);
//        connect(resumen, &Resumen_Tareas::updateTareas,
//                this, &Mapas_Cercania_Itacs::emitUpdateITACs);
//        connect(this, &Mapas_Cercania_Itacs::coords_selected, resumen, &Resumen_Tareas::focusTareaCoords);

//        resumen->show();

//    }else{
//        resumen->setTareas(jsonArrayITACsForTable);
//        if(resumen->isHidden()){
//            resumen->move(thisPos.x() + this->width() + 10, thisPos.y()-7);
//            resumen->show();
//        }
//    }
//}
void Mapas_Cercania_Itacs::openITACwithCod_Emplazamiento(QString cod){
    emit openITAC(cod);
}

void Mapas_Cercania_Itacs::moveMapCenter(QString geoCode){
    double xcoord, ycoord;
    getCoordsFromString(geoCode, xcoord, ycoord);
    if(marker_model){
        marker_model->setCenter(xcoord, ycoord);
        marker_model->setZoomLevel(18);
        zoom_level = 18;
        hideLabelInformation();
    }
}
void Mapas_Cercania_Itacs::setCurrentZoomInQML(double zoom){
    if(marker_model){
        marker_model->setZoom(zoom);
    }
}
QString Mapas_Cercania_Itacs::getTextWhitGeoCode(QGeoCoordinate coords){
    QString text = mapa_coords_strings.value(coords.toString());
    return text;
}

QString Mapas_Cercania_Itacs::getCurrentPriorityMarker(QGeoCoordinate coords)
{
    //    "qrc:///icons/marker.png"
    QString priority = mapa_coords_priority.value(coords.toString());
    return "qrc:///icons/"+priority+"_priority_marker.png";
}
double Mapas_Cercania_Itacs::get_Meter_per_Pixel(double zoom){
    double dist = getDistanceWithZoomLevel(zoom);
    double m_center_top = dist/(sqrt(2)); //raiz de dos si es del centro a la diagonal superior izqquierda //sin raiz de dos desde el centro al tope del widget

    double pixels_center_top = static_cast<double>(this->height())/2.0;
    double meter_per_pixel = m_center_top / pixels_center_top;
    return meter_per_pixel / 1.5; //1.6 prueba y error //1.5 distancia exacta bien
}
void Mapas_Cercania_Itacs::printCoordenates(const QString &coordenada)
{
    double latitude1;
    double longitude1;
    centro_perimetro = getCoordsFromString(coordenada, latitude1, longitude1);
    qDebug()<<"centro_perimetro: "<< centro_perimetro;
    setPerimeterButtons();
}
void Mapas_Cercania_Itacs::setPerimeterButtons(QPoint pos_center){
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

    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, label_centro, &QLabel::deleteLater);
    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, eff, &QGraphicsDropShadowEffect::deleteLater);
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

    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, label, &QLabel::deleteLater);
    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, effect, &QGraphicsDropShadowEffect::deleteLater);
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

    connect(lineEdit, &QLineEdit::textChanged, this, &Mapas_Cercania_Itacs::setPerimeter);
    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, lineEdit, &QLineEdit::deleteLater);
    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, ef, &QGraphicsDropShadowEffect::deleteLater);
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
    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, label_meter, &QLabel::deleteLater);
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

    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, pb_filtrar, &QLabel::deleteLater);
    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, e, &QGraphicsDropShadowEffect::deleteLater);
    connect(pb_filtrar, &QPushButton::clicked, this, &Mapas_Cercania_Itacs::filtrarPerimetro);
    pb_filtrar->setFixedSize(label_width, label_height);
    pos = pos_center;
    pos.setX(pos.x() + circle_radius + 5);
    pos.setY(pos.y() + label_height/2 + 5);
    pb_filtrar->move(pos);
    pb_filtrar->show();

    QTimer::singleShot(1000, this, &Mapas_Cercania_Itacs::enableClearPerimeterButtons);
}
void Mapas_Cercania_Itacs::enableClearPerimeterButtons(){
    clearPerimeterButtons_enable = true;
}

void Mapas_Cercania_Itacs::setPerimeter(QString perimetro_string){
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
    connect(label_centro, &ClickableLabel::clicked, this, &Mapas_Cercania_Itacs::hideLabelInformation);

    connect(this, &Mapas_Cercania_Itacs::perimeterChanged, label_centro_inner, &ClickableLabel::deleteLater);
    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, label_centro_inner, &ClickableLabel::deleteLater);

    connect(this, &Mapas_Cercania_Itacs::perimeterChanged, label_centro, &ClickableLabel::deleteLater);
    connect(this, &Mapas_Cercania_Itacs::perimeterChanged, eff, &QGraphicsDropShadowEffect::deleteLater);
    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, label_centro, &ClickableLabel::deleteLater);
    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, eff, &QGraphicsDropShadowEffect::deleteLater);

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
void Mapas_Cercania_Itacs::filtrarPerimetro(){
    clearPerimeterButtons_enable=true;
    hideLabelInformation();
    if(centro_perimetro.isValid()){
        zoom_level = fillMapWithGeolocalizations(jsonArrayITACs, centro_perimetro, perimetro);
    }
}
QString Mapas_Cercania_Itacs::getViewOfITAC(QJsonObject jsonObject){
    QString view = "";
    QString field = jsonObject.value(itac_itacs).toString(); //direccion
    if(checkIfFieldIsValid(field)){
        view += field;
    }
    field = jsonObject.value(nombre_empresa_administracion_itacs).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nEmpresa: "+field;
    }
    field = jsonObject.value(zona_itacs).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nSector P: "+field;
    }
    field = jsonObject.value(acceso_itacs).toString();
    if(checkIfFieldIsValid(field)){
        view += field  +"\nAcceso: "+field;
    }
    field = jsonObject.value(descripcion_itacs).toString();
    if(checkIfFieldIsValid(field)){
        view += field  +"\nDescripcion: "+field;
    }
    return view;
}
void Mapas_Cercania_Itacs::fillDirsAndToolTips(QMap<QString, int> &dirs, QMap<QString, QString> &dirs_toolTips, QGeoCoordinate coords){
    double xcoord, ycoord;
    QString geoCode, numin;
    for (int i =0; i< jsonArrayITACs.size(); i++) {
        geoCode = jsonArrayITACs.at(i).toObject().value(geolocalizacion_itacs).toString();
        if(checkIfFieldIsValid(geoCode)){
            getCoordsFromString(geoCode, xcoord, ycoord);
            QGeoCoordinate coords_itac(xcoord, ycoord);
            qDebug()<<"coords_itac: "<<coords_itac;
            qDebug()<<"coords: "<<coords;
            if(coords_itac.isValid() && coords_itac == coords){
                QString direccion = jsonArrayITACs.at(i).toObject().value(itac_itacs).toString().trimmed();
                QString toolTip = getViewOfITAC(jsonArrayITACs.at(i).toObject());
                if(!dirs.keys().contains(direccion)){
                    dirs.insert(direccion, 1);
                    dirs_toolTips.insert(direccion, toolTip);
                }
            }
        }
    }
}
void Mapas_Cercania_Itacs::showLabelInformation(QGeoCoordinate coords){
    QPoint pos = this->mapFromGlobal(QCursor::pos());
    if(tooltip_enable && coords.isValid()){
        if(lastEnterCoords!=coords){
            hideLabelInformation();
            lastEnterCoords = coords;
        }
        qDebug()<<"lastEnterCoords: "<<lastEnterCoords;
        QMap<QString, int> dirs;
        QMap<QString, QString> dirs_toolTips;
        fillDirsAndToolTips(dirs, dirs_toolTips, coords);

        if(!dirs.isEmpty()){
            if(tooltip){
                tooltip = false;
                QWidget *widget = new QWidget(this);
                QVBoxLayout *layout= new QVBoxLayout;
                layout->setAlignment(Qt::AlignCenter);
                widget->setLayout(layout);
                QString dir = "";
                QString dir_toolTip = "";
                for (int i = 0; i < dirs.keys().size(); i++) {
                    dir = dirs.keys().at(i);
                    dir_toolTip = dirs_toolTips.value(dirs_toolTips.keys().at(i));

                    ClickableLabel *label = new ClickableLabel();
                    label->setText(dir);
                    label->setToolTip(dir_toolTip);
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
                    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, label, &QLabel::deleteLater);
                    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, effect, &QGraphicsDropShadowEffect::deleteLater);
                    connect(label, &ClickableLabel::textClicked, this, &Mapas_Cercania_Itacs::printCoordenatesMarkerAndType);
                    label->setAlignment(Qt::AlignCenter);
                    label->setFixedWidth(dir.size()*8);
                    label->setFixedHeight(20);
                    widget->layout()->addWidget(label);

                    qDebug()<<(this->mapFromGlobal(QCursor::pos()));
                }
                widget->setFixedHeight(dirs.keys().size() * 50);
                pos.setY(pos.y()-widget->height()/2);
                widget->move(pos);
                connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, widget, &QWidget::deleteLater);
                widget->show();
                //                QTimer::singleShot(4000, this, &Mapas_Cercania_Itacs::hideLabelInformation);
            }
        }
    }
}
void Mapas_Cercania_Itacs::hideLabelInformation(){
    if(clearPerimeterButtons_enable){
        tooltip_enable = true;
        emit deleteLabelInformation();
        tooltip = true;
    }
    //    qDebug()<<"hideLabelInformation";
}
void Mapas_Cercania_Itacs::printCoordenatesMarker(QGeoCoordinate coords)//coordenadas del marcador presionado
{
    //    QGeoCoordinate coords;
    qDebug()<<"On Marker "+ QString::number(coords.latitude()) + ", "+QString::number(coords.longitude());
    last_marker_clicked_coords = coords;
    //    QTimer::singleShot(150, this, &Mapas_Cercania_Itacs::openTareaX);
    double x, y;
    QJsonArray jsonArray;
    QString geoCode;
    for (int i=0; i < jsonArrayITACs.size(); i++) {
        geoCode = jsonArrayITACs.at(i).toObject().value(geolocalizacion_itacs).toString();
        if(checkIfFieldIsValid(geoCode)){
            qDebug()<< geoCode;
            if(getCoordsFromString(geoCode, x, y)==coords){
                jsonArray.append(jsonArrayITACs.at(i).toObject());
            }
        }
    }
    //    emit coords_selected(jsonArray);
    moveMapCenter(getStringFromCoord(coords));
}
void Mapas_Cercania_Itacs::openItacX(){ //Ahora mismo no la estoy usando
    if(last_marker_clicked_coords.isValid()){
        double xcoord, ycoord;
        QString geoCode, cod;
        for (int i =0; i< jsonArrayITACs.size(); i++) {
            geoCode = jsonArrayITACs.at(i).toObject().value(geolocalizacion_itacs).toString();
            if(checkIfFieldIsValid(geoCode)){
                qDebug()<< geoCode;
                getCoordsFromString(geoCode, xcoord, ycoord);
                QGeoCoordinate coords_itac(xcoord, ycoord);
                if(coords_itac.isValid() && coords_itac == last_marker_clicked_coords){
                    cod = jsonArrayITACs.at(i).toObject().value(codigo_itac_itacs).toString().trimmed();
                    last_marker_clicked_coords = QGeoCoordinate();
                    emit openITAC(cod);
                    break;
                }
            }
        }
    }
}
void Mapas_Cercania_Itacs::printCoordenatesMarkerAndType(QString dir)//coordenadas del marcador presionado
{
    //    QGeoCoordinate coords;
    qDebug()<<"On Marker type "+ dir;
    lastDirSelected = dir;
    QTimer::singleShot(150, this, &Mapas_Cercania_Itacs::openItacXconDir);
}
void Mapas_Cercania_Itacs::openItacXconDir(){
    if(lastEnterCoords.isValid()){
        double xcoord, ycoord;
        QString geoCode, cod, dir;
        for (int i =0; i< jsonArrayITACs.size(); i++) {
            geoCode = jsonArrayITACs.at(i).toObject().value(geolocalizacion_itacs).toString();
            if(checkIfFieldIsValid(geoCode)){
                dir = jsonArrayITACs.at(i).toObject().value(itac_itacs).toString().trimmed();
                qDebug()<< geoCode;
                getCoordsFromString(geoCode, xcoord, ycoord);
                QGeoCoordinate coords_itac(xcoord, ycoord);
                if(coords_itac.isValid() && coords_itac == lastEnterCoords && lastDirSelected == dir){
                    cod = jsonArrayITACs.at(i).toObject().value(codigo_itac_itacs).toString().trimmed();
                    //                lastEnterCoords = QGeoCoordinate();
                    emit openITAC(cod);
                    break;
                }
            }
        }
    }
}



bool Mapas_Cercania_Itacs::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
QString Mapas_Cercania_Itacs::getStringFromCoord(QGeoCoordinate coords){
    if(coords.isValid()){
        QString latitud = QString::number(coords.latitude(), 'f', 13);
        QString longitud = QString::number(coords.longitude(), 'f', 13);
        qDebug()<<"Coords: -> "<<coords;
        qDebug()<<"Conversion: -> " + latitud + "," + longitud;
        return latitud + "," + longitud;
    }
    return "";
}

QGeoCoordinate Mapas_Cercania_Itacs::getCoordsFromString(QString string, double &xcoord, double &ycoord){
    QStringList split;
    split = string.split(",");
    if(split.length() > 1){
        QString x_string = split.at(0).trimmed();
        QString y_string = split.at(1).trimmed();
        bool x_ok, y_ok;
        xcoord = x_string.toDouble(&x_ok);
        ycoord = y_string.toDouble(&y_ok);
        if(x_ok && y_ok){
            return QGeoCoordinate(xcoord, ycoord);
        }
    }
    return QGeoCoordinate();
}

void Mapas_Cercania_Itacs::initMarker(double xcoord, double ycoord){
    marker_model->setMaxMarkers(1);
    if(QGeoCoordinate(xcoord, ycoord).isValid()){
        marker_model->moveMarker(QGeoCoordinate(xcoord, ycoord));
    }
}

double Mapas_Cercania_Itacs::getYcoordenade()
{
    return marker_model->yposition();
}
double Mapas_Cercania_Itacs::getXcoordenade()
{
    return marker_model->xposition();
}
double Mapas_Cercania_Itacs::getZoomLevel()
{
    return zoom_level;
}

void Mapas_Cercania_Itacs::searchSuggestions(QString suggestion){
    qDebug()<<"Buscando -> " + suggestion;
    QPlaceSearchRequest request;
    request.setSearchTerm(suggestion);
    request.setSearchArea(QGeoCircle(QGeoCoordinate(40.4636688, -3.7492199)));
    request.setLimit(10);
    suggestionReply = manager->searchSuggestions(request);
    connect(suggestionReply, &QPlaceSearchSuggestionReply::finished, this, &Mapas_Cercania_Itacs::handleSuggestionReply);
}
void Mapas_Cercania_Itacs::searchResult(QString item, int maxResults)
{
    //instantiate request and set parameters
    QPlaceSearchRequest searchRequest;
    searchRequest.setSearchTerm(item);
    searchRequest.setSearchArea(QGeoCircle(QGeoCoordinate(40.4636688, -3.7492199)));
    searchRequest.setLimit(maxResults); //specify how many results are to be retrieved.
    //send off a search request
    searchReply = manager->search(searchRequest);

    //connect a slot to handle the reply
    connect(searchReply, &QPlaceSearchReply::finished, this, &Mapas_Cercania_Itacs::handleSearchReply);
}

void Mapas_Cercania_Itacs::searchItem(QString item)
{
    lastSearchItem = "";
    searchResult(item);
}

void Mapas_Cercania_Itacs::handleSearchReply()
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
                    zoom_level = fillMapWithGeolocalizations(jsonArrayITACs, QGeoCoordinate(xcoord,  ycoord));
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

void Mapas_Cercania_Itacs::handleSuggestionReply()
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

void Mapas_Cercania_Itacs::on_pb_map_search_clicked()
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
                    zoom_level = fillMapWithGeolocalizations(jsonArrayITACs, QGeoCoordinate(xcoord,  ycoord));
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
double Mapas_Cercania_Itacs::getZoomLevelWithDistance(double distance){ //zoom para ver esta distancia
    double zoom_d = log2((int)(40000 / (distance / 2)));
    int zoom_l = (int)(floor(zoom_d));
    return zoom_l;
}
double Mapas_Cercania_Itacs::getDistanceWithZoomLevel(double zoom_l){ //zoom para ver esta distancia
    double distance = 80000/(pow(2, zoom_l));
    return distance * 1000; //*1000 metros
}
double Mapas_Cercania_Itacs::degreesToRadians(double degrees) {
    return degrees * 3.141592 / 180;
}

double Mapas_Cercania_Itacs::distanceInKmBetweenEarthCoordinates(double lat1, double lon1, double lat2, double lon2) {
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
void Mapas_Cercania_Itacs::on_pb_close_clicked()
{
    emit closing();
    this->close();
}



void Mapas_Cercania_Itacs::on_l_mostrar_en_tabla_clicked()
{
    if(jsonArrayITACsForTable.isEmpty()){
        GlobalFunctions::showMessage(this, "Sin Resultados", "No hay itacs para mostrar en tabla");
    }else{
        emit showJsonArrayInTable(jsonArrayITACsForTable);
        on_pb_close_clicked();
    }
}


void Mapas_Cercania_Itacs::on_pb_close_filter_zona_clicked()
{
    ui->widget_zona->close();
}

void Mapas_Cercania_Itacs::on_pb_show_filter_zona_clicked()
{
    ui->widget_zona->show();
}
void Mapas_Cercania_Itacs::on_pb_add_zonas_clicked()
{
    emit show_filter();
    filterColumnList.clear();
    showFilterWidgetOptions();
}
QStringList Mapas_Cercania_Itacs::getFieldValues(QString field){
    QJsonArray jsonArray = jsonArrayITACs;
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
void Mapas_Cercania_Itacs::filterColumnField(){

    if(filterColumnList.isEmpty()){
        return;
    }
    filterZonas(filterColumnList);
    disconnect(ui->cb_zonas, &MyComboBoxShine::currentTextChanged, this, &Mapas_Cercania_Itacs::filterZona);
    ui->cb_zonas->clear();
    ui->cb_zonas->addItems(filterColumnList);
    connect(ui->cb_zonas, &MyComboBoxShine::currentTextChanged, this, &Mapas_Cercania_Itacs::filterZona);
}
QString Mapas_Cercania_Itacs::getScrollBarStyle(){
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
void Mapas_Cercania_Itacs::showFilterWidgetOptions(){

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
    QStringList values = getFieldValues(zona_itacs);
    QString value;
    int width = 100;
    foreach(value, values){
        MyCheckBox *cb = new MyCheckBox();
        cb->setText(value);
        cb->setObjectName("cb_"+value);
        cb->setFixedHeight(itemHeight-5);
        cb->setStyleSheet("color: rgb(255, 255, 255);"
                          "background-color: rgba(77, 77, 77);");
        connect(cb, &MyCheckBox::toggleCheckBox, this, &Mapas_Cercania_Itacs::addRemoveFilterList);
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
    connect(button_filter, &QPushButton::clicked, this, &Mapas_Cercania_Itacs::filterColumnField);

    QFont font = button_filter->font();
    font.setFamily("Segoe UI");
    font.setBold(true);
    font.setPointSize(10);

    button_filter->setFont(font);

    //    widgetValues->setFixedHeight(35 * (values.size() + 1));
    widgetValues->setFixedWidth(width);

    QScrollArea *scroll = new QScrollArea;
    QWidget *widget = new QWidget(this);
    QGraphicsDropShadowEffect* ef = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    ef->setBlurRadius(20);
    ef->setOffset(10);
    widget->setGraphicsEffect(ef);

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
    connect(this, &Mapas_Cercania_Itacs::show_filter, widget, &QWidget::deleteLater);
    connect(this, &Mapas_Cercania_Itacs::closing, widget, &QWidget::deleteLater);
    connect(this, &Mapas_Cercania_Itacs::deleteLabelInformation, widget, &QWidget::deleteLater);
}

void Mapas_Cercania_Itacs::addRemoveFilterList(QString value){
    if(filterColumnList.contains(value)){
        filterColumnList.removeOne(value);
    }else{
        filterColumnList << value;
    }
}
void Mapas_Cercania_Itacs::on_pb_map_type_change_clicked()
{
    mapType++;
    if(mapType > 3){ mapType = 1;}

    if(marker_model){
        marker_model->setMapType(mapType);
    }
}
