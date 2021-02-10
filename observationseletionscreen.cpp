#include "observationseletionscreen.h"
#include "ui_observationseletionscreen.h"
#include "observacion.h"
#include <QCompleter>

ObservationSeletionScreen::ObservationSeletionScreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObservationSeletionScreen)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowTitle("Selección de Observación");

    QStringList list;
    QJsonArray jsonArray = Observacion::readObservaciones();
    for (int i=0; i < jsonArray.size(); i++) {
        QString obs_v, origen_v, cod_v;
        origen_v = jsonArray.at(i).toObject().value(origen_observaciones).toString();
        obs_v = jsonArray.at(i).toObject().value(observacion_observaciones).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_observacion_observaciones).toString();

        list.append(cod_v+" - "+obs_v);
    }
    QCompleter *completer = new QCompleter(list, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_observacion->setCompleter(completer);
}

ObservationSeletionScreen::~ObservationSeletionScreen()
{
    delete ui;
}

void ObservationSeletionScreen::moveCenter(){
    QRect rect = parentWidget()->geometry();
    this->move(rect.width()/2 - this->width()/2,
               rect.height()/2 - this->height()/2 - 20);
}
void ObservationSeletionScreen::on_pb_close_clicked()
{
    emit reject();
    this->close();
}

void ObservationSeletionScreen::on_pb_aceptar_clicked()
{
    QString seleccion = ui->le_observacion->text();
    emit selected_observation(seleccion);
    emit accept();
    this->close();
}

void ObservationSeletionScreen::on_pb_cancelar_clicked()
{
    on_pb_close_clicked();
}
