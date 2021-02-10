#ifndef FIELDS_TO_ASSING_COUNTERS_H
#define FIELDS_TO_ASSING_COUNTERS_H

#include <QWidget>
#include <QtCore>

namespace Ui {
class Fields_to_Assing_Counters;
}

class Fields_to_Assing_Counters : public QWidget
{
    Q_OBJECT

public:
    explicit Fields_to_Assing_Counters(QWidget *parent = nullptr);
    ~Fields_to_Assing_Counters();

signals:
    void fields_selected(QJsonObject);

private slots:
    void on_pb_actualizar_clicked();

private:
    Ui::Fields_to_Assing_Counters *ui;
    void initializeMaps();
};

#endif // FIELDS_TO_ASSING_COUNTERS_H
