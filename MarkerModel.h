#ifndef MARKERMODEL_H
#define MARKERMODEL_H

#include <QAbstractListModel>
#include <QGeoCoordinate>

class MarkerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate current READ current NOTIFY currentChanged)
    Q_PROPERTY(double xposition READ xposition NOTIFY xpositionChanged)
    Q_PROPERTY(double yposition READ yposition NOTIFY ypositionChanged)
    Q_PROPERTY(QString currentText READ currentText NOTIFY currentTextChanged)
    Q_PROPERTY(double zoomLevel READ zoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(double mapType READ mapType NOTIFY mapTypeChanged)

public:
    enum MarkerRoles{
        PositionRole = Qt::UserRole + 1000,
    };
    explicit MarkerModel(QObject *parent = nullptr, double xpos =0, double ypos=0);
    void moveMarker(const QGeoCoordinate & coordinate);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int maxMarkers() const;
    void setMaxMarkers(int maxMarkers=0);
    QGeoCoordinate current() const;
    double yposition() const;
    double xposition() const;
    QString currentText() const;
    double zoomLevel() const;
    double mapType() const;
    void removeLastMarker();

    void mapToPosition(QGeoCoordinate gc);

    void setText(QString text);
    void setZoomLevel(double zoom);
    void setMapType(double type);
    void setZoom(double zoom);
signals:
    void currentChanged();
    void xpositionChanged();
    void ypositionChanged();
    void currentTextChanged();
    void zoomLevelChanged();
    void mapTypeChanged();
private:
    void insert(int row, const QGeoCoordinate & coordinate);

    QList<QGeoCoordinate> m_markers;
    QGeoCoordinate m_current;
    QString m_currentText = "TEXTO";
    double m_zoomLevel = 18;
    double m_mapType = 1;
    double m_yposition;
    double m_xposition;
    int m_maxMarkers;
};

#endif // MARKERMODEL_H
