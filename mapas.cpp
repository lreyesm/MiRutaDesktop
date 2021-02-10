#include "mapas.h"
#include "mapas.h"
#include "ui_mapas.h"

#include <QQmlContext>
#include "new_table_structure.h"
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>
#include "global_variables.h"
#include <QGraphicsDropShadowEffect>

Mapas::Mapas(QWidget *parent, QJsonObject object) :
    QWidget(parent),
    ui(new Ui::Mapas)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(1);
    effect->setColor(color_blue_app);
    ui->label->setGraphicsEffect(effect);
    
    QString tipo_tarea_l = object.value(tipo_tarea).toString().trimmed();
    QString coords = object.value(geolocalizacion).toString().trimmed();
    QString coords_hand = object.value(codigo_de_localizacion).toString().trimmed();
    QString  prioridad_l = object.value(prioridad).toString().trimmed();
    QString  tipoOrden_l = object.value(TIPORDEN).toString().trimmed();
    QString  radio_l = object.value(tipoRadio).toString().trimmed();
    if(checkIfFieldIsValid(prioridad_l)){
        m_prioridad = prioridad_l;
    }
    if(checkIfFieldIsValid(radio_l)){
        m_radio = "RADIO_";
    }
    if(checkIfFieldIsValid(tipoOrden_l) && tipoOrden_l.contains("D", Qt::CaseInsensitive)){
        m_orden = "DIARIA_";
    }

    
    bool found = false;
    if(checkIfFieldIsValid(coords_hand)){
        if(getCoordsFromString(coords_hand, xcoordenade_hand, ycoordenade_hand)){
            found = true;
        }
    }
    if(checkIfFieldIsValid(coords)){
        if(getCoordsFromString(coords, xcoordenade, ycoordenade)){
            found = true;
        }
    }
    
    
    if(found){
        if(QGeoCoordinate(xcoordenade_hand, ycoordenade_hand).isValid()){
            hand_home = true;
            marker_model = new MarkerModel(this, xcoordenade_hand,  ycoordenade_hand);
            ui->pb_home_casa->setPixmap(QPixmap(":/icons/hand.png"));
            initMarker(xcoordenade_hand, ycoordenade_hand);
        }
        else if(QGeoCoordinate(xcoordenade, ycoordenade).isValid()){
            hand_home = false;
            marker_model = new MarkerModel(this, xcoordenade,  ycoordenade);
            ui->pb_home_casa->setPixmap(QPixmap(":/icons/home.png"));
            initMarker(xcoordenade, ycoordenade);
        }
        else{
            marker_model = new MarkerModel(this, 43.263005,  -2.934991);
            marker_model->setMaxMarkers(1);
        }
        
    }else{
        marker_model = new MarkerModel(this, 43.263005,  -2.934991);
        marker_model->setMaxMarkers(1);
    }
    
    ui->quickWidget->rootContext()->setContextProperty("w",this);
    ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);
    ui->quickWidget->setSource(QUrl("qrc:/qml/mapview.qml"));
    
    //    QStringList list;
    //    list << "Hola" << "Adios";
    
    connect(ui->lineEdit, &MyLineEditAutoComplete::textModified, this, &Mapas::searchSuggestions);
    connect(ui->lineEdit, &MyLineEditAutoComplete::itemSelected, this, &Mapas::searchItem);
    
    //    ui->lineEdit->setAutoCompleteList(list);
    
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
    
    
    QString poblacion_l = object.value(poblacion).toString().trimmed();
    QString calle_l = object.value(calle).toString().trimmed();
    QString portal = object.value(numero).toString().trimmed();
    bool ok = false;
    int port_int = 0;
    if(checkIfFieldIsValid(portal)){
        port_int = portal.toInt(&ok);
    }
    if(checkIfFieldIsValid(calle_l) && checkIfFieldIsValid(poblacion_l)){
        ui->lineEdit->setText(calle_l + ", " + poblacion_l);
        if(ok){
            ui->lineEdit->setText(calle_l + ", " + QString::number(port_int) + " " + poblacion_l);
            if(!found){
                searchSuggestions(ui->lineEdit->text());
            }
        }
    }
}

Mapas::~Mapas()
{
    delete ui;
}
QString Mapas::getMapPluginName(){
    return mapPluginName;
}
QString Mapas::getMapPluginParameterName(){
    return mapPluginParameterName;
}
QString Mapas::getMapPluginParameterValue(){
    return mapPluginParameterValue;
}
int Mapas::getMapType(){
    return mapType;
}
void Mapas::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    ui->quickWidget->setFixedSize(this->size());
    ui->quickWidget->move(0,0);
    ui->pb_home_casa->move(this->width() - 50, this->height()/2 -50);
    ui->pb_erase->move(this->width() - 50, this->height()/2);
    
    ui->lineEdit->move(this->width()/2 - ui->lineEdit->width()/2, 10);
    
    ui->pb_close->move(this->width() - 50, 10);
    ui->pb_map_search->move(ui->lineEdit->pos().x() + ui->lineEdit->width() + 10,
                            ui->pb_map_search->pos().y());
    ui->l_max_min->move(this->width() - 100, 10);
}

void Mapas::setCurrentZoomInQML(double zoom){
    if(marker_model){
        marker_model->setZoom(zoom);
    }
}
QString Mapas::getTextWhitGeoCode(QGeoCoordinate coords){
    return coords.toString();
}

QString Mapas::getCurrentPriorityMarker(QGeoCoordinate coords)
{
    Q_UNUSED(coords);
    if(!m_prioridad.contains(m_radio)){
        m_prioridad.prepend(m_radio);
    }
    if(!m_prioridad.contains(m_orden)){
        m_prioridad.prepend(m_orden);
    }
    return "qrc:///icons/"+m_prioridad+"_priority_marker.png";
}

bool Mapas::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
QString Mapas::getStringFromCoord(QGeoCoordinate coords){
    if(coords.isValid()){
        QString latitud = QString::number(coords.latitude(), 'f', 13);
        QString longitud = QString::number(coords.longitude(), 'f', 13);
        qDebug()<<"Coords: -> "<<coords;
        qDebug()<<"Conversion: -> " + latitud + "," + longitud;
        return latitud + "," + longitud;
    }
    return "";
}

bool Mapas::getCoordsFromString(QString string, double &xcoord, double &ycoord){
    QStringList split;
    split = string.split(",");
    if(split.length() > 1){
        QString x_string = split.at(0).trimmed();
        QString y_string = split.at(1).trimmed();
        bool x_ok, y_ok;
        xcoord = x_string.toDouble(&x_ok);
        ycoord = y_string.toDouble(&y_ok);
        //            qDebug()<<"xcoord: -> "+QString::number(xcoord, 'g', 13);
        qDebug() << "xcoord: " << QString("%1").arg(xcoord, 0, 'e', 13);
        qDebug()<<"ycoord: -> "+QString::number(ycoord, 'g', 13);
        qDebug()<<"x_string: -> "+x_string;
        qDebug()<<"y_string: -> "+y_string;
        if(x_ok && y_ok){
            return true;
        }
    }
    return false;
}

void Mapas::initMarker(double xcoord, double ycoord){
    marker_model->setMaxMarkers(1);
    if(QGeoCoordinate(xcoord, ycoord).isValid()){
        marker_model->moveMarker(QGeoCoordinate(xcoord, ycoord));
    }
}

void Mapas::showLabelInformation(QGeoCoordinate coords){
    Q_UNUSED(coords);
    qDebug()<<"showLabelInformation";
}
void Mapas::hideLabelInformation(){
    qDebug()<<"hideLabelInformation";
}
void Mapas::printCoordenatesMarker(QGeoCoordinate coords)//coordenadas del marcador presionado
{
    qDebug()<<"On Marker "+ QString::number(coords.latitude()) + ", "+QString::number(coords.longitude());
}
void Mapas::printCoordenates(const QString &coordenada)
{
    QStringList pieces = coordenada.split(",");
    double latitude1 = pieces[0].toDouble();
    double longitude1 = pieces[1].toDouble();
    qDebug()<<latitude1;
    qDebug()<<longitude1;
    
    if(hand_home){
        emit settedMarkerHand(coordenada);
        xcoordenade_hand = latitude1;
        ycoordenade_hand = longitude1;
    }else{
        emit settedMarker(coordenada);
        xcoordenade = latitude1;
        ycoordenade = longitude1;
    }
    marker_model->moveMarker(QGeoCoordinate(latitude1, longitude1));
}

double Mapas::getYcoordenade()
{
    return marker_model->yposition();
}
double Mapas::getXcoordenade()
{
    return marker_model->xposition();
}
double Mapas::getZoomLevel()
{
    return 18;
}


void Mapas::on_pb_home_casa_clicked()
{
    if(hand_home){
        hand_home = false;
        delete marker_model;
        marker_model = new MarkerModel(this, xcoordenade,  ycoordenade);
        ui->pb_home_casa->setPixmap(QPixmap(":/icons/home.png"));
        if(QGeoCoordinate(xcoordenade, ycoordenade).isValid()){
            marker_model->moveMarker(QGeoCoordinate(xcoordenade, ycoordenade));
        }
        
    }else{
        hand_home = true;
        ui->pb_home_casa->setPixmap(QPixmap(":/icons/hand.png"));
        delete marker_model;
        marker_model = new MarkerModel(this, xcoordenade_hand,  ycoordenade_hand);
        if(QGeoCoordinate(xcoordenade_hand, ycoordenade_hand).isValid()){
            marker_model->moveMarker(QGeoCoordinate(xcoordenade_hand, ycoordenade_hand));
        }
    }
    ui->quickWidget->rootContext()->setContextProperty("w",this);
    ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);
}

void Mapas::on_pb_erase_clicked()
{
    delete marker_model;
    
    if(hand_home){
        marker_model = new MarkerModel(this, xcoordenade_hand,  ycoordenade_hand);
        xcoordenade_hand = 1000;
        ycoordenade_hand = 1000;
        emit erase_marker_hand();
    }else{
        marker_model = new MarkerModel(this, xcoordenade,  ycoordenade);
        xcoordenade = 1000;
        ycoordenade = 1000;
        emit erase_marker_home();
    }
    
    marker_model->setMaxMarkers(1);
    
    ui->quickWidget->rootContext()->setContextProperty("w",this);
    ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);
}

void Mapas::on_l_max_min_clicked()
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



void Mapas::setAndEmitCoords(QGeoCoordinate coords){
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

void Mapas::searchSuggestions(QString suggestion){
    qDebug()<<"Buscando -> " + suggestion;
    QPlaceSearchRequest request;
    request.setSearchTerm(suggestion);
    request.setSearchArea(QGeoCircle(QGeoCoordinate(40.4636688, -3.7492199)));
    request.setLimit(10);
    suggestionReply = manager->searchSuggestions(request);
    connect(suggestionReply, &QPlaceSearchSuggestionReply::finished, this, &Mapas::handleSuggestionReply);
}
void Mapas::searchResult(QString item, int maxResults)
{
    //instantiate request and set parameters
    QPlaceSearchRequest searchRequest;
    searchRequest.setSearchTerm(item);
    searchRequest.setSearchArea(QGeoCircle(QGeoCoordinate(40.4636688, -3.7492199)));
    searchRequest.setLimit(maxResults); //specify how many results are to be retrieved.
    //send off a search request
    searchReply = manager->search(searchRequest);
    
    //connect a slot to handle the reply
    connect(searchReply, &QPlaceSearchReply::finished, this, &Mapas::handleSearchReply);
}

void Mapas::searchItem(QString item)
{  
    lastSearchItem = "";
    searchResult(item);
}

void Mapas::handleSearchReply()
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
                
                delete marker_model;
                marker_model = new MarkerModel(this, xcoord,  ycoord);
                marker_model->setMaxMarkers(1);
                if(QGeoCoordinate(xcoord,  ycoord).isValid()){
                    qDebug()<<"Valido";
                    marker_model->moveMarker(QGeoCoordinate(xcoord,  ycoord));
                    setAndEmitCoords(QGeoCoordinate(xcoord,  ycoord));
                }
                else{
                    qDebug()<<"No valido";
                }
                
                ui->quickWidget->rootContext()->setContextProperty("w",this);
                ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);
            }
        }
    }
    searchReply->deleteLater();  //discard reply
    searchReply = 0;
}

void Mapas::handleSuggestionReply()
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

void Mapas::on_pb_map_search_clicked()
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
                
                delete marker_model;
                marker_model = new MarkerModel(this, xcoord,  ycoord);
                marker_model->setMaxMarkers(1);
                if(QGeoCoordinate(xcoord,  ycoord).isValid()){
                    qDebug()<<"Valido iteracion";
                    marker_model->moveMarker(QGeoCoordinate(xcoord,  ycoord));
                    setAndEmitCoords(QGeoCoordinate(xcoord,  ycoord));
                }
                else{
                    qDebug()<<"No valido";
                }
                ui->quickWidget->rootContext()->setContextProperty("w",this);
                ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);
            }
        }
    }else{
        lastSearchItem = ui->lineEdit->text();
        if(!lastSearchItem.isEmpty()){
            searchResult(lastSearchItem, 10);
        }
    }
}

void Mapas::on_pb_close_clicked()
{
    this->close();
}


