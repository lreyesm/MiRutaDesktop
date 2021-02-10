#ifndef JORNADA_OPERARIO_H
#define JORNADA_OPERARIO_H

#include <QMainWindow>
#include <QJsonObject>
#include <QStandardItemModel>
#include <QTimer>

namespace Ui {
class Jornada_Operario;
}

class Jornada_Operario : public QMainWindow
{
    Q_OBJECT

public:
    explicit Jornada_Operario(QWidget *parent = nullptr, QJsonObject json = QJsonObject());
    ~Jornada_Operario();

    QString getHourInSeconds(qint64 seconds);
protected slots:
    void resizeEvent(QResizeEvent *event);
private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);
    void setTableView();
    void on_l_up_month_clicked();
    void on_l_down_month_clicked();
    void on_l_up_year_clicked();
    void on_l_down_year_clicked();
private:
    Ui::Jornada_Operario *ui;
    QJsonObject operario_json;
    QJsonObject jornada_json;
    QStandardItemModel* model  = nullptr;
    void fixModelForTable(QJsonObject jsonObject, int month, int year);
    bool connected_header_signal= false;
     QMap<QString, int> mapMonth;

     void fillJsonDate();
     void setCurrentMonth(int month);
     QTimer timer;
};

#endif // JORNADA_OPERARIO_H
