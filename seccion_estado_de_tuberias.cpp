#include "seccion_estado_de_tuberias.h"
#include "ui_seccion_estado_de_tuberias.h"
#include "structure_itac.h"

Seccion_Estado_de_Tuberias::Seccion_Estado_de_Tuberias(QWidget *parent, QJsonObject jsonObject) :
    QWidget(parent),
    ui(new Ui::Seccion_Estado_de_Tuberias)
{
    ui->setupUi(this);
    this->setWindowTitle("Sección 4 Estado de las Tuberías");
    itac = jsonObject;

    QStringList  tubo_de_alimentacion, colector, tuberias_de_entrada, tuberias_de_salida;
    tubo_de_alimentacion << "Sin comprobar"<<"Excelente"<<"Regular"<<"Poco fiable"<< "Mal estado"<<"Otros";
    colector << "Sin comprobar"<<"Excelente"<<"Regular"<<"Poco fiable"<< "Mal estado"<<"Otros";
    tuberias_de_salida << "Sin comprobar"<<"Excelente"<<"Regular"<<"Poco fiable"<< "Mal estado"<<"Otros";
    tuberias_de_entrada << "Sin comprobar"<<"Excelente"<<"Regular"<<"Poco fiable"<< "Mal estado"<<"Otros";

    QMap<QString, QStringList> mapa_spinners;

    mapa_spinners.insert("l_tubo_de_alimentacion", tubo_de_alimentacion);
    mapa_spinners.insert("l_colector", colector);
    mapa_spinners.insert("l_tuberias_de_salida", tuberias_de_salida);
    mapa_spinners.insert("l_tuberias_de_entrada", tuberias_de_entrada);

    for (int i=0; i < mapa_spinners.keys().size(); i++) {
        QString object_name = mapa_spinners.keys().at(i);

        MyLabelSpinner *labelSpinner = ui->w_seccion_4->findChild<MyLabelSpinner*>(object_name);
        if(labelSpinner){
            labelSpinner->addItems(mapa_spinners.value(object_name));
        }
    }
    populateView();
}

Seccion_Estado_de_Tuberias::~Seccion_Estado_de_Tuberias()
{
    delete ui;
}

QJsonObject Seccion_Estado_de_Tuberias::setMalEstadoEnGeneral(QJsonObject itac)
{
    QString tubo_de_alimentacion = "Mal estado";
    QString colector = "Mal estado";
    QString tuberias_de_salida = "Mal estado";
    QString tuberias_de_entrada = "Mal estado";

    if(!tubo_de_alimentacion.isEmpty()){
        itac.insert(tubo_de_alimentacion_itacs, tubo_de_alimentacion);
    }
    if(!colector.isEmpty()){
        itac.insert(colector_itacs, colector);
    }
    if(!tuberias_de_entrada.isEmpty()){
        itac.insert(tuberias_de_entrada_contador_itacs, tuberias_de_entrada);
    }
    if(!tuberias_de_salida.isEmpty()){
        itac.insert(tuberias_de_salida_contador_itacs, tuberias_de_salida);
    }
    itac.insert(puntuacion_seccion4_itacs,"0");
    return itac;
}
QJsonObject Seccion_Estado_de_Tuberias::setExcelenteEnGeneral(QJsonObject itac)
{
    QString tubo_de_alimentacion = "Excelente";
    QString colector = "Excelente";
    QString tuberias_de_salida = "Excelente";
    QString tuberias_de_entrada = "Excelente";

    if(!tubo_de_alimentacion.isEmpty()){
        itac.insert(tubo_de_alimentacion_itacs, tubo_de_alimentacion);
    }
    if(!colector.isEmpty()){
        itac.insert(colector_itacs, colector);
    }
    if(!tuberias_de_entrada.isEmpty()){
        itac.insert(tuberias_de_entrada_contador_itacs, tuberias_de_entrada);
    }
    if(!tuberias_de_salida.isEmpty()){
        itac.insert(tuberias_de_salida_contador_itacs, tuberias_de_salida);
    }
    itac.insert(puntuacion_seccion4_itacs,"2");
    return itac;
}

void Seccion_Estado_de_Tuberias::guardarDatos()
{
    float puntuacion_seccion4 = 0;
    QString tubo_de_alimentacion = ui->l_tubo_de_alimentacion->currentText();
    QString colector = ui->l_colector->currentText();
    QString tuberias_de_entrada = ui->l_tuberias_de_entrada->currentText();

    QString puntos_totales = ui->le_puntos_agua_total->text();
    QString puntos_contador = ui->le_puntos_agua_con_contador->text();
    QString puntos_contador_mas_tarea = ui->le_puntos_agua_con_contador_mas_tarea_abierta->text();

    QString tuberias_de_salida = ui->l_tuberias_de_salida->currentText();

    if(!tubo_de_alimentacion.isEmpty()){
        itac.insert(tubo_de_alimentacion_itacs, tubo_de_alimentacion);
        if(tubo_de_alimentacion == "Excelente" || tubo_de_alimentacion == "Regular"){
            puntuacion_seccion4+=0.5;
        }
        if(tubo_de_alimentacion == "Poco fiable"){
            puntuacion_seccion4+=0.2;
        }
        if(tubo_de_alimentacion == "Mal estado"){ ///Otros!!!!?????
            puntuacion_seccion4+=(-2);
        }
    }
    if(!colector.isEmpty()){
        itac.insert(colector_itacs, colector);
        if(colector == "Excelente" || colector == "Regular"){
            puntuacion_seccion4+=0.5;
        }
        if(colector == "Poco fiable"){
            puntuacion_seccion4+=0.2;
        }
        if(colector == "Mal estado"){ ///Otros!!!!?????
            puntuacion_seccion4+=(-2);
        }
    }
    if(!tuberias_de_entrada.isEmpty()){
        itac.insert(tuberias_de_entrada_contador_itacs, tuberias_de_entrada);
        if(tuberias_de_entrada == "Excelente" || tuberias_de_entrada == "Regular"){
            puntuacion_seccion4+=0.5;
        }
        if(tuberias_de_entrada == "Poco fiable"){
            puntuacion_seccion4+=0.2;
        }
        if(tuberias_de_entrada == "Mal estado"){ ///Otros!!!!?????
            puntuacion_seccion4+=(-6);
        }
    }
    if(!tuberias_de_salida.isEmpty()){
        itac.insert(tuberias_de_salida_contador_itacs, tuberias_de_salida);
        if(tuberias_de_salida == "Excelente" || tuberias_de_salida == "Regular"){
            puntuacion_seccion4+=0.5;
        }
        if(tuberias_de_salida == "Poco fiable"){
            puntuacion_seccion4+=0.2;
        }
        if(tuberias_de_salida == "Mal estado"){ ///Otros!!!!?????
            puntuacion_seccion4+=(-3);
        }
    }

    if(!puntos_totales.isEmpty()){
        itac.insert(puntos_agua_total_itacs, puntos_totales);
    }
    if(!puntos_contador.isEmpty()){
        itac.insert(puntos_agua_con_contador_itacs, puntos_contador);
    }
    if(!puntos_contador_mas_tarea.isEmpty()){
        itac.insert(puntos_agua_con_contador_mas_tarea_itacs, puntos_contador_mas_tarea);
    }

    QString puntuacion_seccion = QString::number(puntuacion_seccion4, 'f', 2);
    itac.insert(puntuacion_seccion4_itacs, puntuacion_seccion);

    QString nota = ui->pt_nota->toPlainText();
    if(!nota.isEmpty()){
        itac.insert(estado_de_tuberias_nota_itacs, nota);
    }
}

void Seccion_Estado_de_Tuberias::populateView()
{
    QString tubo_de_alimentacion = itac.value(tubo_de_alimentacion_itacs).toString().trimmed();
    QString tuberias_de_entrada = itac.value(tuberias_de_entrada_contador_itacs).toString();
    QString tuberias_de_salida = itac.value(tuberias_de_salida_contador_itacs).toString();
    QString colector = itac.value(colector_itacs).toString();

    QString puntos_totales = itac.value(puntos_agua_total_itacs).toString().trimmed();
    QString puntos_contador = itac.value(puntos_agua_con_contador_itacs).toString().trimmed();
    QString puntos_contador_mas_tarea = itac.value(puntos_agua_con_contador_mas_tarea_itacs).toString().trimmed();

    if(ui->l_tubo_de_alimentacion->getSpinnerList().contains(tubo_de_alimentacion)){
        ui->l_tubo_de_alimentacion->setText(tubo_de_alimentacion);
    }
    if(ui->l_colector->getSpinnerList().contains(colector)){
        ui->l_colector->setText(colector);
    }
    if(ui->l_tuberias_de_entrada->getSpinnerList().contains(tuberias_de_entrada)){
        ui->l_tuberias_de_entrada->setText(tuberias_de_entrada);
    }
    if(ui->l_tuberias_de_salida->getSpinnerList().contains(tuberias_de_salida)){
        ui->l_tuberias_de_salida->setText(tuberias_de_salida);
    }

    if(checkIfFieldIsValid(puntos_totales)){
        ui->le_puntos_agua_total->setText(puntos_totales);
    }
    if(checkIfFieldIsValid(puntos_contador)){
        ui->le_puntos_agua_con_contador->setText(puntos_contador);
    }
    if(checkIfFieldIsValid(puntos_contador_mas_tarea)){
        ui->le_puntos_agua_con_contador_mas_tarea_abierta->setText(puntos_contador_mas_tarea);
    }

    QString nota = itac.value(estado_de_tuberias_nota_itacs).toString();
    if(checkIfFieldIsValid(nota)){
        ui->pt_nota->setPlainText(nota);
    }
}

void Seccion_Estado_de_Tuberias::on_pb_cancelar_clicked()
{
    this->close();
    emit reject();
}

void Seccion_Estado_de_Tuberias::on_pb_aceptar_clicked()
{
    guardarDatos();
    emit itac_json(itac);
    emit accept();
    this->close();
}
bool Seccion_Estado_de_Tuberias::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
