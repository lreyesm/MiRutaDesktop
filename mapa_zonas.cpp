#include "mapa_zonas.h"
#include "ui_mapa_zonas.h"

#include <QQmlContext>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>
#include "global_variables.h"
#include <QGraphicsDropShadowEffect>

Mapa_Zonas::Mapa_Zonas(QWidget *parent, QString geocode) :
    QWidget(parent),
    ui(new Ui::Mapa_Zonas)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(2);
    effect->setColor(color_blue_app);
    ui->label->setGraphicsEffect(effect);


    if(!geocode.isEmpty()){
        if(getCoordsFromString(geocode, xcoordenade, ycoordenade)){
            if(QGeoCoordinate(xcoordenade, ycoordenade).isValid()){
                marker_model = new MarkerModel(this, xcoordenade,  ycoordenade);
                initMarker(xcoordenade, ycoordenade);
            }
        }
    }else{
        marker_model = new MarkerModel(this, 43.263005,  -2.934991);
        marker_model->setMaxMarkers(1);
    }

    ui->quickWidget->rootContext()->setContextProperty("w",this);
    ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);
    ui->quickWidget->setSource(QUrl("qrc:/qml/mapview.qml"));

    connect(ui->lineEdit, &MyLineEditAutoComplete::textModified, this, &Mapa_Zonas::searchSuggestions);
    connect(ui->lineEdit, &MyLineEditAutoComplete::itemSelected, this, &Mapa_Zonas::searchItem);

    //    ui->lineEdit->setAutoCompleteList(list);


    ///Para busqueda en Mapas-----------------------------------------------------------------------------------------------------------------------------------------

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

    ui->l_max_min->hide();
}

Mapa_Zonas::~Mapa_Zonas()
{
    delete ui;
}
QString Mapa_Zonas::getMapPluginName(){
    return mapPluginName;
}
QString Mapa_Zonas::getMapPluginParameterName(){
    return mapPluginParameterName;
}
QString Mapa_Zonas::getMapPluginParameterValue(){
    return mapPluginParameterValue;
}
int Mapa_Zonas::getMapType(){
    return mapType;
}
void Mapa_Zonas::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    ui->quickWidget->setFixedSize(this->size());
    ui->quickWidget->move(0,0);

    ui->lineEdit->move(this->width()/2 - ui->lineEdit->width()/2, 10);

    ui->pb_close->move(this->width() - 50, 10);
    ui->pb_map_search->move(ui->lineEdit->pos().x() + ui->lineEdit->width() + 10,
                            ui->pb_map_search->pos().y());
    ui->l_max_min->move(this->width() - 100, 10);
}
void Mapa_Zonas::showLabelInformation(QGeoCoordinate coords){
    Q_UNUSED(coords);
    qDebug()<<"showLabelInformation";
}
void Mapa_Zonas::hideLabelInformation(){
    qDebug()<<"hideLabelInformation";
}
void Mapa_Zonas::printCoordenatesMarker(QGeoCoordinate coords)//coordenadas del marcador presionado
{
    qDebug()<<"On Marker "+ QString::number(coords.latitude()) + ", "+QString::number(coords.longitude());
}
void Mapa_Zonas::setCurrentZoomInQML(double zoom){
    if(marker_model){
        marker_model->setZoom(zoom);
    }
}
QString Mapa_Zonas::getTextWhitGeoCode(QGeoCoordinate coords){
    return coords.toString();
}

QString Mapa_Zonas::getCurrentPriorityMarker(QGeoCoordinate coords)
{
    Q_UNUSED(coords)
    return "qrc:///icons/marker.png";
}

bool Mapa_Zonas::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
QString Mapa_Zonas::getStringFromCoord(QGeoCoordinate coords){
    if(coords.isValid()){
        QString latitud = QString::number(coords.latitude(), 'f', 13);
        QString longitud = QString::number(coords.longitude(), 'f', 13);
        qDebug()<<"Coords: -> "<<coords;
        qDebug()<<"Conversion: -> " + latitud + "," + longitud;
        return latitud + "," + longitud;
    }
    return "";
}

bool Mapa_Zonas::getCoordsFromString(QString string, double &xcoord, double &ycoord){
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

void Mapa_Zonas::initMarker(double xcoord, double ycoord){
    marker_model->setMaxMarkers(1);
    if(QGeoCoordinate(xcoord, ycoord).isValid()){
        marker_model->moveMarker(QGeoCoordinate(xcoord, ycoord));
    }
}



void Mapa_Zonas::printCoordenates(const QString &coordenada)
{
    QStringList pieces = coordenada.split(",");
    double latitude1 = pieces[0].toDouble();
    double longitude1 = pieces[1].toDouble();
    qDebug()<<latitude1;
    qDebug()<<longitude1;

    emit settedMarker(coordenada);
    xcoordenade = latitude1;
    ycoordenade = longitude1;

    marker_model->moveMarker(QGeoCoordinate(latitude1, longitude1));
}

double Mapa_Zonas::getYcoordenade()
{
    return marker_model->yposition();
}
double Mapa_Zonas::getXcoordenade()
{
    return marker_model->xposition();
}
double Mapa_Zonas::getZoomLevel()
{
    return 18;
}


void Mapa_Zonas::on_pb_erase_clicked()
{
    delete marker_model;

    marker_model = new MarkerModel(this, xcoordenade,  ycoordenade);
    xcoordenade = 1000;
    ycoordenade = 1000;
    emit erase_marker();

    marker_model->setMaxMarkers(1);

    ui->quickWidget->rootContext()->setContextProperty("w",this);
    ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);
}

void Mapa_Zonas::on_l_max_min_clicked()
{
    if(this->isMaximized()){
        this->showNormal();
        ui->lineEdit->setFixedSize(ui->lineEdit->size()/1.5);
        QFont font = ui->lineEdit->font();
        font.setPointSize(static_cast<int>(font.pointSize()/1.2));
        ui->lineEdit->setFont(font);
        ui->pb_map_search->setFixedSize(ui->lineEdit->height(), ui->lineEdit->height());
        ui->pb_map_search->move(ui->pb_map_search->pos().x(), ui->lineEdit->pos().y());
    }else{
        this->showMaximized();
        ui->lineEdit->setFixedSize(ui->lineEdit->size()*1.5);
        QFont font = ui->lineEdit->font();
        font.setPointSize(static_cast<int>(font.pointSize()*1.2));
        ui->lineEdit->setFont(font);
        ui->pb_map_search->setFixedSize(ui->lineEdit->height(), ui->lineEdit->height());
        ui->pb_map_search->move(ui->pb_map_search->pos().x(), ui->lineEdit->pos().y());
    }
}


void Mapa_Zonas::moveMarkerToOriginalPos(){
    delete marker_model;
    marker_model = new MarkerModel(this, xcoordenade,  ycoordenade);
    initMarker(xcoordenade, ycoordenade);
    ui->quickWidget->rootContext()->setContextProperty("w",this);
    ui->quickWidget->rootContext()->setContextProperty("marker_model", marker_model);
}


void Mapa_Zonas::setAndEmitCoords(QGeoCoordinate coords){

    QString coord = getStringFromCoord(coords);
    if(!coord.isEmpty()){
        emit settedMarker(coord);
    }
    xcoordenade = coords.latitude();
    ycoordenade = coords.longitude();
}

void Mapa_Zonas::searchSuggestions(QString suggestion){
    qDebug()<<"Buscando -> " + suggestion;
    QPlaceSearchRequest request;
    request.setSearchTerm(suggestion);
    request.setSearchArea(QGeoCircle(QGeoCoordinate(40.4636688, -3.7492199)));
    request.setLimit(10);
    suggestionReply = manager->searchSuggestions(request);
    connect(suggestionReply, &QPlaceSearchSuggestionReply::finished, this, &Mapa_Zonas::handleSuggestionReply);
}
void Mapa_Zonas::searchResult(QString item, int maxResults)
{
    //instantiate request and set parameters
    QPlaceSearchRequest searchRequest;
    searchRequest.setSearchTerm(item);
    searchRequest.setSearchArea(QGeoCircle(QGeoCoordinate(40.4636688, -3.7492199)));
    searchRequest.setLimit(maxResults); //specify how many results are to be retrieved.
    //send off a search request
    searchReply = manager->search(searchRequest);

    //connect a slot to handle the reply
    connect(searchReply, &QPlaceSearchReply::finished, this, &Mapa_Zonas::handleSearchReply);
}

void Mapa_Zonas::searchItem(QString item)
{
    lastSearchItem = "";
    searchResult(item);
}

void Mapa_Zonas::handleSearchReply()
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

void Mapa_Zonas::handleSuggestionReply()
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

void Mapa_Zonas::on_pb_map_search_clicked()
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

void Mapa_Zonas::on_pb_close_clicked()
{
    this->close();
}


