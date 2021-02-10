#ifndef MAPAS_H
#define MAPAS_H

#include "MarkerModel.h"

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

namespace Ui {
class Mapas;
}

class Mapas : public QWidget
{
    Q_OBJECT

public:
    explicit Mapas(QWidget *parent = nullptr, QJsonObject object = QJsonObject());
    ~Mapas();
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
    void settedMarkerHand(const QString geocode);
    void settedMarker(const QString geocode);
    void erase_marker_hand();
    void erase_marker_home();

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

private:
    Ui::Mapas *ui;
    MarkerModel *marker_model;
    double xcoordenade = 1000;//coordenadas en España Bilbao
    double ycoordenade = 1000;

    double xcoordenade_hand = 1000; //coordenadas invalidas
    double ycoordenade_hand = 1000;

    bool hand_home = false;
    void initMarker(double xcoord, double ycoord);
    bool getCoordsFromString(QString string, double &xcoord, double &ycoord);
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
    QString m_prioridad = "MEDIA";
    QString m_orden = "";
    QString m_radio = "";

};

#endif // MAPAS_H
