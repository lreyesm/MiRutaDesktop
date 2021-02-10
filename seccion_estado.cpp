#include "seccion_estado.h"
#include "ui_seccion_estado.h"
#include "structure_itac.h"

Seccion_Estado::Seccion_Estado(QWidget *parent, QJsonObject jsonObject) :
    QWidget(parent),
    ui(new Ui::Seccion_Estado)
{
    ui->setupUi(this);
    this->setWindowTitle("Sección 3 Estado de conservacion y diseño de zona de trabajo");
    itac = jsonObject;

    QStringList  espacio_para_trabajar, desague_si, poca_nula;
    espacio_para_trabajar <<"Cómodo"<<"Justo"<<"Incómodo"<<"Nulo";
    desague_si <<"Funciona Bien"<<"No Operativo"<<"Sin Comprobar";
    poca_nula << "Cambiar Bombilla"<< "Llevar Luz Propia"<<"Otros";

    QMap<QString, QStringList> mapa_spinners;

    mapa_spinners.insert("l_espacio_para_trabajar", espacio_para_trabajar);
    mapa_spinners.insert("l_desague_si", desague_si);
    mapa_spinners.insert("l_poca_nula", poca_nula);

    for (int i=0; i < mapa_spinners.keys().size(); i++) {
        QString object_name = mapa_spinners.keys().at(i);

        MyLabelSpinner *labelSpinner = ui->w_seccion_3->findChild<MyLabelSpinner*>(object_name);
        if(labelSpinner){
            labelSpinner->addItems(mapa_spinners.value(object_name));
        }

    }
    hideAll();
    populateView();
}

Seccion_Estado::~Seccion_Estado()
{
    delete ui;
}

void Seccion_Estado::on_rb_desague_si_toggled(bool checked)
{
    if(checked){
        ui->l_desague_si->show();
    }else{
        ui->l_desague_si->hide();
    }
}
void Seccion_Estado::on_rb_sin_comprobar_toggled(bool checked)
{

}
void Seccion_Estado::hideAll(){
    ui->l_poca_nula->hide();
    ui->l_desague_si->hide();
}


void Seccion_Estado::on_pb_cancelar_clicked()
{
    this->close();
    emit reject();
}

void Seccion_Estado::on_pb_aceptar_clicked()
{
    guardarDatos();
    emit itac_json(itac);
    emit accept();
    this->close();
}
void Seccion_Estado::populateView(){
    QString espacio = itac.value(espacio_para_trabajar_itacs).toString();;
    QString desague = itac.value(desague_itacs).toString();;
    QString extra_desague = itac.value(extras_desague_itacs).toString().trimmed();
    QString iluminacion = itac.value(iluminacion_itacs).toString().trimmed();
    QString iluminacion_extra = itac.value(extras_iluminacion_itacs).toString().trimmed();

    if(checkIfFieldIsValid(espacio)){
        ui->l_espacio_para_trabajar->setText(espacio);
    }

    if(desague == "TRUE" || desague == "Si"){
        ui->rb_desague_si->setChecked(true);
        if(checkIfFieldIsValid(extra_desague)){
            ui->l_desague_si->setText(extra_desague);
        }
    }
    else if(desague == "FALSE" || desague == "No"){
        ui->rb_desague_no->setChecked(true);
    }
    else if( desague == "Sin comprobar"){
        ui->rb_sin_comprobar->setChecked(true);
    }

    if(checkIfFieldIsValid(iluminacion)){
        iluminacion = "rb_" + iluminacion;
        QRadioButton *rb = ui->groupBox_iluminacion->findChild<QRadioButton*>(iluminacion);
        if(rb){
            rb->setChecked(true);
        }
        if(checkIfFieldIsValid(iluminacion_extra)){
            ui->l_poca_nula->setText(iluminacion_extra);
        }
    }

    QString nota = itac.value(estado_de_conservacion_nota_itacs).toString();
    if(checkIfFieldIsValid(nota)){
        ui->pt_nota->setPlainText(nota);
    }
}

QJsonObject Seccion_Estado::setExcelenteEnGeneral(QJsonObject itac){
    itac.insert(puntuacion_seccion3_itacs, "1.5");
    return itac;
}

QJsonObject Seccion_Estado::setMalEstadoEnGeneral(QJsonObject itac){
    itac.insert(puntuacion_seccion3_itacs,"0");
    return itac;
}

void Seccion_Estado::guardarDatos(){
    float puntuacion_seccion3 = 0;
    QString espacio = "";
    QString desague = "FALSE";
    QString extra_desague = "";
    QString iluminacion = "";
    QString iluminacion_extra = "";

    espacio = ui->l_espacio_para_trabajar->currentText();
    if(espacio == "Cómodo"){
        puntuacion_seccion3+=0.5;
    }else if(espacio == "Justo"){
        puntuacion_seccion3+=0.3;
    }else if(espacio == "Incómodo"){
        puntuacion_seccion3+=(-1);
    }else if(espacio == "Nulo"){
        puntuacion_seccion3+=(-6);
    }

    if(ui->rb_desague_si->isChecked()){
        desague = "Si";
        extra_desague = ui->l_desague_si->currentText();
        if(extra_desague == "Funciona Bien"){
            puntuacion_seccion3+=0.5;
        }
    }
    else if(ui->rb_desague_no->isChecked()){
        desague = "No";
        extra_desague = "";
    }
    else if(ui->rb_sin_comprobar->isChecked()){
        desague = "Sin comprobar";
        extra_desague = "";
    }

    if(ui->rb_Natural->isChecked()){
        iluminacion = "Natural";
        puntuacion_seccion3+=0.5;
    }
    else if(ui->rb_Suficiente->isChecked()){
        iluminacion = "Suficiente";
        puntuacion_seccion3+=0.5;
    }
    else if(ui->rb_Poca->isChecked()){
        iluminacion = "Poca";
        iluminacion_extra += ui->l_poca_nula->currentText();
        if(iluminacion_extra == "Cambiar Bombilla"){
            puntuacion_seccion3+=0.2;
        }
    }
    else if(ui->rb_Nula->isChecked()){
        iluminacion = "Nula";
        iluminacion_extra += ui->l_poca_nula->currentText();
        if(iluminacion_extra == "Cambiar Bombilla"){
            puntuacion_seccion3+=0.2;
        }
    }

    itac.insert(espacio_para_trabajar_itacs, espacio);

    itac.insert(desague_itacs, desague);
    itac.insert(extras_desague_itacs, extra_desague);

    itac.insert(iluminacion_itacs, iluminacion);
    itac.insert(extras_iluminacion_itacs, iluminacion_extra);

    QString puntuacion_seccion = QString::number(puntuacion_seccion3, 'f', 2);
    itac.insert(puntuacion_seccion3_itacs, puntuacion_seccion);

    QString nota = ui->pt_nota->toPlainText();
    if(!nota.isEmpty()){
        itac.insert(estado_de_conservacion_nota_itacs, nota);
    }
}
bool Seccion_Estado::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
void Seccion_Estado::on_rb_Poca_toggled(bool checked)
{
    if(checked){
        ui->l_poca_nula->show();
    }
}

void Seccion_Estado::on_rb_Nula_toggled(bool checked)
{
    if(checked){
        ui->l_poca_nula->show();
    }
}

void Seccion_Estado::on_rb_Natural_toggled(bool checked)
{
    if(checked){
        ui->l_poca_nula->hide();
    }
}

void Seccion_Estado::on_rb_Suficiente_toggled(bool checked)
{
    if(checked){
        ui->l_poca_nula->hide();
    }
}



