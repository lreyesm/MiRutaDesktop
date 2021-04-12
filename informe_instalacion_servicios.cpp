#include "informe_instalacion_servicios.h"
#include "ui_informe_instalacion_servicios.h"
#include <QDialog>
#include "new_table_structure.h"
#include "global_variables.h"
#include <QDateTime>
#include "info.h"

Informe_Instalacion_Servicios::Informe_Instalacion_Servicios(QWidget *parent, QString empresa) :
    QMainWindow(parent),
    ui(new Ui::Informe_Instalacion_Servicios)
{
    ui->setupUi(this);
    this->setWindowTitle("Informe de Instalaciones y Servicios");
    this->empresa = empresa;
    //    ui->statusbar->hide();
}

Informe_Instalacion_Servicios::~Informe_Instalacion_Servicios()
{
    delete ui;
}

void Informe_Instalacion_Servicios::showWidgetData(bool show){
    if(show){
        ui->widget_task_data->show();
    }else{
        ui->widget_task_data->hide();
    }
}

void Informe_Instalacion_Servicios::setData(QJsonObject o)
{
    jsonObject = o;

    QString idSat = jsonObject.value(ID_SAT).toString();
    QString idOrden = jsonObject.value(idOrdenCABB).toString();
    if(!checkIfFieldIsValid(idSat) || !checkIfFieldIsValid(idOrden)){
        Info *info = new Info(nullptr, false, empresa);
        info->getInfoInServer();
        QJsonArray jsonArrayInfo = Info::readInfos();
        QJsonObject jsonObjectInfo = jsonArrayInfo.at(0).toObject();

        if(!checkIfFieldIsValid(idSat)){
            int ultimoIDSAT = jsonObjectInfo.value(lastIDSAT_infos).toString().trimmed().toInt();
            ultimoIDSAT++;
            jsonObject.insert(ID_SAT, QString::number(ultimoIDSAT));
            QString id_sat_string = QString::number(ultimoIDSAT);
            jsonObjectInfo.insert(lastIDSAT_infos, id_sat_string);
        }
        if(!checkIfFieldIsValid(idOrden)){
            int ultimoIDOrden = jsonObjectInfo.value(lastIDOrden_infos).toString().trimmed().toInt();
            ultimoIDOrden++;
            jsonObject.insert(idOrdenCABB, QString::number(ultimoIDOrden));
            QString id_orden_string = QString::number(ultimoIDOrden);
            jsonObjectInfo.insert(lastIDOrden_infos, id_orden_string);
        }

        info->actualizarInfoInServer(jsonObjectInfo);
    }

    ui->le_id_sat->setText(nullity_check(jsonObject.value(ID_SAT).toString()));
    ui->le_numero_abonado->setText(nullity_check(jsonObject.value(numero_abonado).toString()));
    ui->le_calibre_real->setText(nullity_check(jsonObject.value(calibre_real).toString()));
    ui->le_fecha_informe->setText(QDateTime::currentDateTime().toString(formato_fecha_hora));
    ui->le_fecha_realizacion->setText(nullity_check(jsonObject.value(F_INST).toString()));
    QString observaciones_dev = jsonObject.value(OBSERVA).toString();
    if(!checkIfFieldIsValid(observaciones_dev)){
        observaciones_dev = jsonObject.value(observaciones_devueltas).toString();
    }
    ui->pt_observaciones->setPlainText(nullity_check(observaciones_dev));
    QString estado_tarea = jsonObject.value(Estado).toString();
    if(checkIfFieldIsValid(estado_tarea)){
        if(estado_tarea == "INCIDENCIA"){
            ui->cb_incidencia->setChecked(true);
        }
    }
    QString servicios_value = jsonObject.value(servicios).toString();
    QString suministros_value = jsonObject.value(suministros).toString();
    if(checkIfFieldIsValid(servicios_value)){
        setCheckBoxesServicios(servicios_value.split("\n"));
    }
    if(checkIfFieldIsValid(suministros_value)){
        setCheckBoxesSuministros(suministros_value.split("\n"));
    }
}
void Informe_Instalacion_Servicios::setCheckBoxesServicios(QStringList list){
    if(list.contains("Instalación")){
        ui->cb_instalacion->setChecked(true);
    }
    if(list.contains("Baja")){
        ui->cb_baja->setChecked(true);
    }
    if(list.contains("Toma Dato")){
        ui->cb_toma_dato->setChecked(true);
    }
    if(list.contains("Reparación")){
        ui->cb_reparacion->setChecked(true);
    }
    if(list.contains("Incidencia")){
        ui->cb_incidencia->setChecked(true);
    }
    if(list.contains("Rcfg Propio")){
        ui->cb_refugio_propio->setChecked(true);
    }
    if(list.contains("Rcfg Baja")){
        ui->cb_refugio_baja->setChecked(true);
    }
    if(list.contains("Rcfg Nuevo")){
        ui->cb_refugio_nuevo->setChecked(true);
    }
    if(list.contains("Toma Dato Radio")){
        ui->cb_toma_dato_radio->setChecked(true);
    }
    if(list.contains("Precintados")){
        ui->cb_precintados->setChecked(true);
    }
    if(list.contains("Desprecintados")){
        ui->cb_desprecintados->setChecked(true);
    }
}
QStringList Informe_Instalacion_Servicios::getCheckBoxeServicios(){
    QStringList list;
    if(ui->cb_instalacion->isChecked()){
        list << "Instalación";
    }
    if(ui->cb_baja->isChecked()){
        list << "Baja";
    }
    if(ui->cb_toma_dato->isChecked()){
        list << "Toma Dato";
    }
    if(ui->cb_reparacion->isChecked()){
        list << "Reparación";
    }
    if(ui->cb_incidencia->isChecked()){
        list << "Incidencia";
    }
    if(ui->cb_refugio_propio->isChecked()){
        list << "Rcfg Propio";
    }
    if(ui->cb_refugio_baja->isChecked()){
        list << "Rcfg Baja";
    }
    if(ui->cb_refugio_nuevo->isChecked()){
        list << "Rcfg Nuevo";
    }
    if(ui->cb_toma_dato_radio->isChecked()){
        list << "Toma Dato Radio";
    }
    if(ui->cb_precintados->isChecked()){
        list << "Precintados";
    }
    if(ui->cb_desprecintados->isChecked()){
        list << "Desprecintados";
    }
    return list;
}

void Informe_Instalacion_Servicios::setCheckBoxesSuministros(QStringList list){
    if(list.contains("Contador")){
        ui->cb_contador->setChecked(true);
    }
    if(list.contains("Módulo R3")){
        ui->cb_modulor3->setChecked(true);
    }
    if(list.contains("Módulo R4")){
        ui->cb_modulor4->setChecked(true);
    }
    if(list.contains("Módulo W4")){
        ui->cb_modulow4->setChecked(true);
    }
    if(list.contains("Módulo LRW")){
        ui->cb_modulolrw->setChecked(true);
    }
    if(list.contains("Reed")){
        ui->cb_reed->setChecked(true);
    }
    if(list.contains("Dp")){
        ui->cb_dp->setChecked(true);
    }
    if(list.contains("Pulsar")){
        ui->cb_pulsar->setChecked(true);
    }
}
QStringList Informe_Instalacion_Servicios::getCheckBoxeSuministros(){
    QStringList list;
    if(ui->cb_contador->isChecked()){
        list << "Contador";
    }
    if(ui->cb_modulor3->isChecked()){
        list << "Módulo R3";
    }
    if(ui->cb_modulor4->isChecked()){
        list << "Módulo R4";
    }
    if(ui->cb_modulow4->isChecked()){
        list << "Módulo W4";
    }
    if(ui->cb_modulolrw->isChecked()){
        list << "Módulo LRW";
    }
    if(ui->cb_reed->isChecked()){
        list << "Reed";
    }
    if(ui->cb_dp->isChecked()){
        list << "Dp";
    }
    if(ui->cb_pulsar->isChecked()){
        list << "Pulsar";
    }
    return list;
}

void Informe_Instalacion_Servicios::guardarDatos()
{
    QStringList serviciosList = getCheckBoxeServicios();
    QStringList suministrosList = getCheckBoxeSuministros();
    jsonObject.insert(servicios, serviciosList.join("\n"));
    jsonObject.insert(suministros, suministrosList.join("\n"));

    jsonObject.insert(ID_SAT, ui->le_id_sat->text());
    jsonObject.insert(numero_abonado, ui->le_numero_abonado->text());
    jsonObject.insert(calibre_real, ui->le_calibre_real->text());
    jsonObject.insert(fecha_informe_servicios, ui->le_fecha_informe->text());
    jsonObject.insert(fecha_realizacion, ui->le_fecha_realizacion->text());
    jsonObject.insert(OBSERVA, ui->pt_observaciones->toPlainText());
}

void Informe_Instalacion_Servicios::on_pb_aceptar_clicked()
{
    guardarDatos();
    emit sendData(jsonObject);
    emit finalizado_informe(QDialog::Accepted);
    this->close();
}

void Informe_Instalacion_Servicios::on_pb_cancelar_clicked()
{
    emit finalizado_informe(QDialog::Rejected);
    this->close();
}

QString Informe_Instalacion_Servicios::nullity_check(QString q)
{
    if(q.trimmed() == "null"  || q.trimmed() == "NULL" || q.trimmed() == "Null")
        return "";
    else
        return q;
}
bool Informe_Instalacion_Servicios::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=NULL && !var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
