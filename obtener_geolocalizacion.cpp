#include "obtener_geolocalizacion.h"
#include "ui_obtener_geolocalizacion.h"
#include <QMessageBox>
#include "processesclass.h"
#include "globalfunctions.h"

Obtener_Geolocalizacion::Obtener_Geolocalizacion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Obtener_Geolocalizacion)
{
    ui->setupUi(this);
    this->setWindowTitle("Ubicacion en Mapa");
}

Obtener_Geolocalizacion::~Obtener_Geolocalizacion()
{
    delete ui;
}

void Obtener_Geolocalizacion::on_buttonBox_accepted()
{
    if(coords == "..."){
        GlobalFunctions::showWarning(this, "Error", "No hay coordenadas validas para insertar");
        return;
    }
    emit coordenadas_obtenidas(coords);
}

void Obtener_Geolocalizacion::on_le_enlace_con_coordenadas_textChanged(const QString &arg1)
{
    coords = get_coordinades_From_Link_GoogleMaps(arg1);
    ui->l_coordenadas_obtenidas->setText(coords);
}

QString Obtener_Geolocalizacion::get_coordinades_From_Link_GoogleMaps(const QString &link_google_maps)
{
    //    QString link_google_maps = "https://www.google.com/maps/dir//23.0504409,-82.5170422/@23.0382777,-82.5225353,13z";
    if(link_google_maps.contains("dir//")){
        if(link_google_maps.split("dir//").size() >= 2){
            QString posicion_string = link_google_maps.split("dir//").at(1);
            if(posicion_string.contains("/@")){
                QString latLang = posicion_string.split("/@").at(0);
                return latLang;
            }
        }
    }
    return "...";
}

void Obtener_Geolocalizacion::on_pb_tutorial_clicked()
{
    QString file_name = "googleMapsUbicacion.mp4";
    QString pr = QDir::currentPath()+"/videos/"+file_name;
    ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 500, true);
}
