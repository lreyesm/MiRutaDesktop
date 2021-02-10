#include "fields_to_assing_counters.h"
#include "ui_fields_to_assing_counters.h"
#include "structure_contador.h"
#include "marca.h"
#include "clase.h"
#include "tipo.h"
#include "longitud.h"
#include "calibre.h"
#include "operario.h"
#include "global_variables.h"
#include <QCompleter>

Fields_to_Assing_Counters::Fields_to_Assing_Counters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Fields_to_Assing_Counters)
{
    ui->setupUi(this);
    this->setWindowTitle("Asignar a Contadores");

    initializeMaps();
}

Fields_to_Assing_Counters::~Fields_to_Assing_Counters()
{
    delete ui;
}

void Fields_to_Assing_Counters::on_pb_actualizar_clicked()
{
    QJsonObject fields;

    if(!ui->le_encargado->text().isEmpty()){
        fields.insert(encargado_contadores,ui->le_encargado->text());
    }
    if(!ui->le_calibre->text().isEmpty()){
        fields.insert(calibre_contadores,ui->le_calibre->text());
    }
    if(!ui->le_longitud->text().isEmpty()){
        fields.insert(longitud_contadores,ui->le_longitud->text());
    }
    if(!ui->le_ruedas->text().isEmpty()){
        fields.insert(ruedas_contador_contadores,ui->le_ruedas->text());
    }
    if(!ui->le_lectura_inicial->text().isEmpty()){
        fields.insert(lectura_inicial_contadores,ui->le_lectura_inicial->text());
    }
    if(!ui->le_marca->text().isEmpty()){
        QString marca_contador;
        QString modelo_contador;
        QString codigo_marca_contador;

        QString marca_modelo = ui->le_marca->text();
        if(marca_modelo.contains(" - ")){
            QStringList string = marca_modelo.split(" - ");
            if(string.length()>= 3){
                marca_contador = string.at(1);
                modelo_contador = string.at(2);
                codigo_marca_contador = string.at(0);
            }
        }
        fields.insert(marca_contadores, marca_contador);
        fields.insert(modelo_contadores, modelo_contador);
        fields.insert(codigo_marca_contadores, codigo_marca_contador);
    }
    if(!ui->le_clase->text().isEmpty()){
        QString clase_y_codigo;
        QString clase_contador;
        QString codigo_clase_contador;

        clase_y_codigo = ui->le_clase->text();
        if(clase_y_codigo.contains(" - ")){
            QStringList string = clase_y_codigo.split(" - ");
            if(string.length()>= 2){
                clase_contador = string.at(1);
                codigo_clase_contador = string.at(0);
            }
        }
        fields.insert(clase_contadores, clase_contador);
        fields.insert(codigo_clase_contadores, codigo_clase_contador);
    }
    if(!ui->le_fluido->text().isEmpty()){
        QString fluido= ui->le_fluido->text();
        fields.insert(tipo_fluido_contadores, fluido);
    }
    if(!ui->le_radio->text().isEmpty()){
        QString radio= ui->le_radio->text();
        fields.insert(tipo_radio_contadores, radio);
    }
    if(!ui->l_status->text().isEmpty() && ui->l_status->text() != "POR DEFECTO"){
        QString status= ui->l_status->text();
        if(status == "DISPONIBLE"){
            status = "";
        }
        fields.insert(status_contadores, status);
    }
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    fields.insert(date_time_modified_contadores, timestamp);

    emit fields_selected(fields);
    this->close();
}

void Fields_to_Assing_Counters::initializeMaps(){
    QJsonArray jsonArray;
    QStringList lista_calibres, lista_longitudes, lista_tipo_radio, lista_tipo_fluido, lista_operarios;
    QMap<QString, QString> mapaTiposDeClase, mapaTiposDeMarca;

    QStringList status;
    status << "POR DEFECTO" << "INSTALLED" << "DISPONIBLE";
    ui->l_status->addItems(status);

    lista_tipo_radio.append("R3");
    lista_tipo_radio.append("R4");
    lista_tipo_radio.append("W4");
    lista_tipo_radio.append("LRW");

    jsonArray = Operario::readOperarios();
    for (int i=0; i < jsonArray.size(); i++) {
        QString tipo_v;
        tipo_v = jsonArray.at(i).toObject().value(operario_operarios).toString();
        lista_operarios.append(tipo_v);
    }

    jsonArray = Tipo::readTipos();
    for (int i=0; i < jsonArray.size(); i++) {
        QString tipo_v;
        tipo_v = jsonArray.at(i).toObject().value(tipo_tipos).toString();
        lista_tipo_fluido.append(tipo_v);
    }
    jsonArray = Longitud::readLongitudes();
    for (int i=0; i < jsonArray.size(); i++) {
        QString longitud_v;
        longitud_v = jsonArray.at(i).toObject().value(longitud_longitudes).toString();
        lista_longitudes.append(longitud_v);
    }

    jsonArray = Calibre::readCalibres();
    for (int i=0; i < jsonArray.size(); i++) {
        QString calibre_v;
        calibre_v = jsonArray.at(i).toObject().value(calibre_calibres).toString();
        lista_calibres.append(calibre_v);
    }

    jsonArray = Clase::readClases();
    for (int i=0; i < jsonArray.size(); i++) {
        QString clase_v, cod_v;
        clase_v = jsonArray.at(i).toObject().value(clase_clases).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_clase_clases).toString();
        mapaTiposDeClase.insert(cod_v, clase_v);
    }

    jsonArray = Marca::readMarcas();
    for (int i=0; i < jsonArray.size(); i++) {
        QString marca_v, modelo_v, cod_v;
        marca_v = jsonArray.at(i).toObject().value(marca_marcas).toString();
        modelo_v = jsonArray.at(i).toObject().value(modelo_marcas).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_marca_marcas).toString();

        mapaTiposDeMarca.insert(cod_v, marca_v + " - " + modelo_v);
    }


    QStringList marcas;
    for (int i=0;i<mapaTiposDeMarca.size();i++) {

        marcas<< mapaTiposDeMarca.keys().at(i) + " - " + mapaTiposDeMarca.values().at(i);
    }
    QStringList clases;
    for (int i=0;i<mapaTiposDeClase.size();i++) {

        clases<< mapaTiposDeClase.keys().at(i) + " - " + mapaTiposDeClase.values().at(i);
    }
    QCompleter *completer = new QCompleter(marcas, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_marca->setCompleter(completer);

    completer = new QCompleter(lista_operarios, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_encargado->setCompleter(completer);

    completer = new QCompleter(lista_calibres, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_calibre->setCompleter(completer);

    completer = new QCompleter(lista_longitudes, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_longitud->setCompleter(completer);

    completer = new QCompleter(lista_tipo_fluido, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_fluido->setCompleter(completer);

    completer = new QCompleter(clases, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_clase->setCompleter(completer);

    completer = new QCompleter(lista_tipo_radio, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_radio->setCompleter(completer);

}
