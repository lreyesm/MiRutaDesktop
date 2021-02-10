#ifndef Mapas_Cercania_Itacs_ITACS_H
#define Mapas_Cercania_Itacs_ITACS_H

#include "markermodelcercania.h"

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
class Mapas_Cercania_Itacs;
}

class Mapas_Cercania_Itacs : public QWidget
{
    Q_OBJECT

public:
    explicit Mapas_Cercania_Itacs(QWidget *parent = nullptr, QJsonArray jsonArray = QJsonArray(), QString empresa = "");
    ~Mapas_Cercania_Itacs();
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

signals:
    void closing();
    void settedMarkerHand(const QString geocode);
    void settedMarker(const QString geocode);
    void erase_marker_hand();
    void erase_marker_home();
    void openITAC(QString);
    void deleteLabelInformation();
    void showJsonArrayInTable(QJsonArray);
    void perimeterChanged();
    void updateITACs();
    void script_excecution_result(int);
    void coords_selected(QJsonArray);
    void show_filter();

protected slots:
    void resizeEvent(QResizeEvent *e);

private slots:
    void on_pb_close_clicked();
    void on_pb_map_search_clicked();

    void handleSearchReply();
    void handleSuggestionReply();

    void searchSuggestions(QString suggestion);
    void searchResult(QString item, int maxResults = 1);

    void searchItem(QString item);

    void openItacX();
    void openItacXconDir();
    void printCoordenatesMarkerAndType(QString dir);
    void filtrarPerimetro();
    void setPerimeter(QString perimetro_string);
    void enableClearPerimeterButtons();
    void on_l_mostrar_en_tabla_clicked();

    void moveMapCenter(QString geoCode);
    void openITACwithCod_Emplazamiento(QString cod);
    void emitUpdateITACs(QJsonObject data);

    void filterZona(QString zona_l);

    void on_pb_close_filter_zona_clicked();

    void on_pb_show_filter_zona_clicked();

    void on_pb_map_type_change_clicked();

    QStringList getFieldValues(QString field);
    void showFilterWidgetOptions();
    void filterColumnField();
    void addRemoveFilterList(QString value);
    void filterZonas(QStringList zonas_selected);

    void on_pb_add_zonas_clicked();
private:
    Ui::Mapas_Cercania_Itacs *ui;
    QJsonArray jsonArrayITACs, jsonArrayITACsForTable;
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

    double fillMapWithGeolocalizations(QJsonArray jsonArray = QJsonArray(), QGeoCoordinate center = QGeoCoordinate(), double perimetro = -1); //retorna el zoom para ver todas
    double degreesToRadians(double degrees);
    double distanceInKmBetweenEarthCoordinates(double lat1, double lon1, double lat2, double lon2);
    double getZoomLevelWithDistance(double distance);
    double first_longitud, first_latitud;
    QGeoCoordinate first_coords, lastEnterCoords;
    QString lastDirSelected = "";
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
    QMap<QString, QString> mapa_coords_priority;

    QString getViewOfITAC(QJsonObject jsonObject);
    QString empresa="";
    bool comparePriority(QString firstPriority, QString secondPriority);
    QTimer timer_hibernacion;
//    database_comunication database_com;
    QStringList keys, values;
    void fillDirsAndToolTips(QMap<QString, int> &tipos_t, QMap<QString, QString> &tipos_t_toolTips, QGeoCoordinate coords);
    QCompleter *completer_zonas = nullptr;

    QStringList filterColumnList;

    QString getScrollBarStyle();
};

#endif // Mapas_Cercania_Itacs_ITACS_H
