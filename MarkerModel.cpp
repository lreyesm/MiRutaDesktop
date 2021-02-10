#include "MarkerModel.h"
#include <QDebug>
MarkerModel::MarkerModel(QObject *parent, double xpos, double ypos)
    : QAbstractListModel(parent),
      m_maxMarkers(0)
{
    m_xposition = xpos;
    m_yposition = ypos;
}
void MarkerModel::setZoom(double zoom)
{
    m_zoomLevel = zoom;
}
void MarkerModel::setText(QString text)
{
    m_currentText = text;
    Q_EMIT currentTextChanged();
}
void MarkerModel::setZoomLevel(double zoom)
{
    m_zoomLevel = zoom;
    Q_EMIT zoomLevelChanged();
}
void MarkerModel::setMapType(double type)
{
    m_mapType = type;
    Q_EMIT mapTypeChanged();
}
void MarkerModel::moveMarker(const QGeoCoordinate &coordinate)
{
    QGeoCoordinate last = m_current;
    m_current = coordinate;
    Q_EMIT currentChanged();

    qDebug()<<"marker count"<< this->rowCount();
    if(!last.isValid()){
        insert(0, coordinate);
        return;
    }

    removeLastMarker();
    insert(0, coordinate);
}
int MarkerModel::maxMarkers() const
{
    return m_maxMarkers;
}

void MarkerModel::setMaxMarkers(int maxMarkers)
{
    m_maxMarkers = maxMarkers >= 1 ? maxMarkers: 0;
}

void MarkerModel::mapToPosition(QGeoCoordinate gc){
    if(gc.isValid() && m_current != gc){
        m_current = gc;
        m_xposition = gc.longitude();
        m_yposition = gc.latitude();

        Q_EMIT currentChanged();
    }
}
QGeoCoordinate MarkerModel::current() const
{
    return m_current;
}

double MarkerModel::yposition() const
{
    return m_yposition;
}

double MarkerModel::xposition() const
{
    return m_xposition;
}

QString MarkerModel::currentText() const
{
    return m_currentText;
}

double MarkerModel::zoomLevel() const
{
    return m_zoomLevel;
}
double MarkerModel::mapType() const
{
    return m_mapType;
}
int MarkerModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_markers.count();
}

QVariant MarkerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role == PositionRole)
        return QVariant::fromValue(m_markers[index.row()]);
    return QVariant();
}

QHash<int, QByteArray> MarkerModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PositionRole] = "position";
    return roles;
}

void MarkerModel::insert(int row, const QGeoCoordinate & coordinate)
{
    beginInsertRows(QModelIndex(), row, row);
    m_markers.insert(row, coordinate);
    endInsertRows();
}

void MarkerModel::removeLastMarker()
{
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    if(!m_markers.isEmpty())
        m_markers.removeLast();
    endRemoveRows();
    qDebug()<<"removeLastMarker";
}
