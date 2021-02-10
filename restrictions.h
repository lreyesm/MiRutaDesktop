#ifndef RESTRICTIONS_H
#define RESTRICTIONS_H

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class Restrictions;
}

class Restrictions : public QWidget
{
    Q_OBJECT

public:
    explicit Restrictions(QWidget *parent = nullptr, QJsonObject cliente = QJsonObject(), QString empresa="");
    ~Restrictions();

    void fillWidgetRestricciones();

signals:
    void setRestrictions(QString restrictions);
private slots:
    void on_pb_agregar_clicked();

    void addRestriction(QString field, QString value);
    void deleteRestrinction(QString name);
    void deleteAllRestrinccions();
    void on_pb_eliminar_clicked();

private:
    Ui::Restrictions *ui;
    QJsonObject cliente;
    QJsonObject restricciones;
    QString empresa="";
    void addtoRestrictionWidget(QString field, QString value);
};

#endif // RESTRICTIONS_H
