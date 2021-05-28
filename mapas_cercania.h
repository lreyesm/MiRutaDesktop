#ifndef MAPAS_CERCANIA_H
#define MAPAS_CERCANIA_H

#include "markermodelcercania.h"
#include "resumen_tareas.h"
#include <QGeoCoordinate>
#include <QWidget>
#include <QJsonObject>
#include <QGeoAddress>
#include <QGeoLocation>
#include <QtPositioning>
#include <QGeoServiceProvider>
#include <QPlaceManager>
#include <QPlaceSearchReply>
#include <QPlaceResult>
#include <QPlaceSearchSuggestionReply>
#include <QLabel>
#include <QCompleter>

namespace Ui {
class Mapas_Cercania;
}

class Mapas_Cercania : public QWidget
{
    Q_OBJECT

public:
    explicit Mapas_Cercania(QWidget *parent = nullptr, QJsonArray jsonArray = QJsonArray(), QString empresa = "");
    ~Mapas_Cercania();
    Q_INVOKABLE void setCenter(QGeoCoordinate coords);
    Q_INVOKABLE void setZoomLevel(double zoom);
    Q_INVOKABLE void printCoordenates(const QString &coordenada);
    Q_INVOKABLE void printCoordenatesMarker(QGeoCoordinate coords);
    Q_INVOKABLE void showLabelInformation(QGeoCoordinate coords);
    Q_INVOKABLE void hideLabelInformation();
    Q_INVOKABLE double getYcoordenade();
    Q_INVOKABLE double getXcoordenade();
    Q_INVOKABLE double getZoomLevel();
    Q_INVOKABLE void setCurrentZoomInQML(double zoom);
    Q_INVOKABLE QString getTextWhitGeoCode(QGeoCoordinate coords);
    Q_INVOKABLE QString getCurrentPriorityMarker(QGeoCoordinate coords);

    Q_INVOKABLE QString getMapPluginName();
    Q_INVOKABLE QString getMapPluginParameterName();
    Q_INVOKABLE QString getMapPluginParameterValue();
    Q_INVOKABLE int getMapType();

    int mapType = 1;
    QString mapPluginName = "googlemaps";
    QString mapPluginParameterName = "googlemaps.google_maps_key";
    QString mapPluginParameterValue = "AIzaSyB8Kny7pDcsLU9LbcKb9lMtEspHkWLqxKo";

//    int mapType = 10;
//    QString mapPluginName = "mapbox";
//    QString mapPluginParameterName = "mapbox.access_token";
//    QString mapPluginParameterValue = "pk.eyJ1IjoiZWxhdXRvbWF0aWNvbiIsImEiOiJja2c5dWcweDAwMHZoMnltdTgzNHoyOGQ3In0.wYXUSb63FWZWWiZPEEcpsA";

signals:
    void closing();
    void settedMarkerHand(const QString geocode);
    void settedMarker(const QString geocode);
    void erase_marker_hand();
    void erase_marker_home();
    void openTarea(QString);
    void deleteLabelInformation();
    void showJsonArrayInTable(QStringList);
    void perimeterChanged();
    void updateTareas(int);
    void updateOperarios();
    void script_excecution_result(int);
    void coords_selected(QJsonArray);

    void show_filter();
    void hidingLoading();
    void setLoadingTextSignal(QString);

protected slots:
    void resizeEvent(QResizeEvent *e);

private slots:
    void on_pb_home_casa_clicked();

    void on_pb_erase_clicked();
    void on_pb_close_clicked();
    void on_l_max_min_clicked();
    void on_pb_map_search_clicked();

    void handleSearchReply();
    void handleSuggestionReply();

    void searchSuggestions(QString suggestion);
    void searchResult(QString item, int maxResults = 1);

    void searchItem(QString item);

    void openTareaX();
    void openTareaXconTipo();
    void printCoordenatesMarkerAndType(QString tipo_t);
    void filtrarPerimetro();
    void setPerimeter(QString perimetro_string);
    void enableClearPerimeterButtons();
    void on_l_mostrar_en_tabla_clicked();

    void showResumen();
    void moveMapCenter(QString geoCode);
    void openTareaWithNumin(QString numin);
    void emitUpdateTareas(QJsonObject data);
    bool checkHibernation();
    void update_tareas_fields_request();
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void filterZona(QString zona_selected);

    void on_pb_close_filter_zona_clicked();

    void on_pb_show_filter_zona_clicked();

    void getOperariosFromServer();
    void on_pb_add_zonas_clicked();

    QStringList getFieldValues(QString field);
    void showFilterWidgetOptions(QString field);
    void filterColumnField();
    void addRemoveFilterList(QString value);
    bool tareaConCitaHoy(QJsonObject jsonObject);
    void on_pb_map_type_change_clicked();

    void fillMap();
    void setLoadingText(QString mess);
    void show_loading(QString mess);
    void hide_loading();

    void on_pb_add_blocks_clicked();

    void filterField(QStringList zonas_selected, QString field);
private:
    Ui::Mapas_Cercania *ui;
    QString lastSectionField;
    QJsonArray jsonArrayTareas, jsonArrayTareasForTable;
    double zoom_level = 18;
    MarkerModelCercania *marker_model = nullptr;
    double xcoordenade = 1000;//coordenadas en España Bilbao
    double ycoordenade = 1000;

    double xcoordenade_hand = 1000; //coordenadas invalidas
    double ycoordenade_hand = 1000;

    bool hand_home = false;
    void initMarker(double xcoord, double ycoord);
    QGeoCoordinate getCoordsFromString(QString string, double &xcoord, double &ycoord);
    bool checkIfFieldIsValid(QString var);

    QGeoServiceProvider *provider;
    QPlaceSearchReply *searchReply;
    QPlaceManager *manager;
    QPlaceSearchSuggestionReply *suggestionReply;

    QList<QPlaceSearchResult> lastListResults;
    int posSearch = 0;
    QString lastSearchItem;
    QString getStringFromCoord(QGeoCoordinate coords);
    void setAndEmitCoords(QGeoCoordinate coords);
    double fillMapWithGeolocalizations(QJsonArray jsonArray = QJsonArray(), QGeoCoordinate center = QGeoCoordinate(), double perimetro = -1); //retorna el zoom para ver todas
    double degreesToRadians(double degrees);
    double distanceInKmBetweenEarthCoordinates(double lat1, double lon1, double lat2, double lon2);
    double getZoomLevelWithDistance(double distance);
    double first_longitud, first_latitud;
    QGeoCoordinate first_coords, lastEnterCoords;
    QString lastTipoTareaSelected = "";
    QGeoCoordinate last_marker_clicked_coords;
    bool tooltip = true;//habilita mostrado el tipo de tarea al entrar en un marcador
    bool tooltip_enable = true;//habilita mostrado el tipo de tarea al entrar en un marcador (esta es para bloquear el tooltip cuando esta el filtrado de perimetro)
    bool clearPerimeterButtons_enable = true;
    double perimetro = 0;
    QGeoCoordinate centro_perimetro = QGeoCoordinate();
    void setPerimeterButtons(QPoint pos_center = QPoint(-1,-1));
    QPoint last_center_pos;
    double getDistanceWithZoomLevel(double zoom_l);
    double get_Meter_per_Pixel(double zoom);
    QMap<QString, QDateTime> tareas_hibernadas;
    QMap<QString, QString> mapa_coords_strings;
    QMap<QString, QString> mapa_coords_priority, mapa_coords_ordenes, mapa_coords_radios, mapa_coords_citas;
    QMap<QString, QJsonArray> mapa_coords_operarios;
    Resumen_Tareas *resumen = nullptr;

    QString getViewOfTarea(QJsonObject jsonObject);
    QString empresa="";
    bool comparePriority(QString firstPriority, QString secondPriority);
    QTimer timer_hibernacion;
    database_comunication database_com;
    QStringList keys, values;
    void fillTypesAndToolTips(QMap<QString, int> &tipos_t, QMap<QString, QString> &tipos_t_toolTips, QGeoCoordinate coords);
    QCompleter *completer_zonas = nullptr;


    QString getViewOfOperario(QJsonObject jsonObject);
    QString getScrollBarStyle();

    QStringList filterColumnList;
};

#endif // MAPAS_CERCANIA_H
