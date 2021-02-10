#include "markermodelcercania.h"
#include <QDebug>
MarkerModelCercania::MarkerModelCercania(QObject *parent, double xpos, double ypos)
    : QAbstractListModel(parent),
      m_maxMarkers(0)
{
    m_xposition = xpos;
    m_yposition = ypos;
}
void MarkerModelCercania::setCenter(double xpos, double ypos)
{
    m_xposition = xpos;
    m_yposition = ypos;
    Q_EMIT xpositionChanged();
    Q_EMIT ypositionChanged();
}
QGeoCoordinate MarkerModelCercania::getCurrentCenter(){
    QGeoCoordinate center(m_xposition, m_yposition);
    return center;
}
void MarkerModelCercania::setText(QString text)
{
    m_currentText = text;
    Q_EMIT currentTextChanged();
}
void MarkerModelCercania::setZoom(double zoom)
{
    m_zoomLevel = zoom;
}
void MarkerModelCercania::setZoomLevel(double zoom)
{
    m_zoomLevel = zoom;
    Q_EMIT zoomLevelChanged();
}
void MarkerModelCercania::setMapType(double type)
{
    m_mapType = type;
    Q_EMIT mapTypeChanged();
}
void MarkerModelCercania::moveMarker(const QGeoCoordinate &coordinate)
{
    QGeoCoordinate last = m_current;
    m_current = coordinate;
    Q_EMIT currentChanged();
    if(!last.isValid())
        return;
    if(m_maxMarkers == 0){
        insert(0, last);
        return;
    }
    if(rowCount() >= m_maxMarkers - 1){
        while (rowCount() >= m_maxMarkers)
            removeLastMarker();
        removeLastMarker();
    }
    insert(0, last);
}
int MarkerModelCercania::maxMarkers() const
{
    return m_maxMarkers;
}

void MarkerModelCercania::setMaxMarkers(int maxMarkers)
{
    m_maxMarkers = maxMarkers >= 1 ? maxMarkers: 0;
}

QGeoCoordinate MarkerModelCercania::current() const
{
    return m_current;
}

double MarkerModelCercania::yposition() const
{
    return m_yposition;
}

double MarkerModelCercania::xposition() const
{
    return m_xposition;
}

QString MarkerModelCercania::currentText() const
{
    return m_currentText;
}
double MarkerModelCercania::zoomLevel() const
{
    return m_zoomLevel;
}
double MarkerModelCercania::mapType() const
{
    return m_mapType;
}
int MarkerModelCercania::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_markers.count();
}

QVariant MarkerModelCercania::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role == PositionRole)
        return QVariant::fromValue(m_markers[index.row()]);
    return QVariant();
}

QHash<int, QByteArray> MarkerModelCercania::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PositionRole] = "position";
    return roles;
}

void MarkerModelCercania::insert(int row, const QGeoCoordinate & coordinate)
{
    beginInsertRows(QModelIndex(), row, row);
    m_markers.insert(row, coordinate);
    endInsertRows();
}

void MarkerModelCercania::removeLastMarker()
{
    int rc = rowCount();
    beginRemoveRows(QModelIndex(), rowCount()-1, rowCount()-1);
    m_markers.clear();
    endRemoveRows();
    qDebug()<<"removeLastMarker";
}
