#include "seccion_llaves_de_puertas.h"
#include "ui_seccion_llaves_de_puertas.h"
#include "structure_itac.h"

Seccion_LLaves_de_Puertas::Seccion_LLaves_de_Puertas(QWidget *parent, QJsonObject jsonObject) :
    QWidget(parent),
    ui(new Ui::Seccion_LLaves_de_Puertas)
{
    ui->setupUi(this);
    this->setWindowTitle("Sección 2 Llaves de puertas");
    itac = jsonObject;

    ui->rb_llaves_de_exterior->setChecked(false);

    QStringList llaves_de_exterior, llaves_maestras,
            utilidad_de_llave_maestra;

    llaves_de_exterior <<"Allen"<<"Triángulo"<<"Cuadrado"<<"CABB"<<"Otros";
    llaves_maestras <<"IBERDROLA"<<"Gas"<<"Del Gestor"<<"Otros";
    utilidad_de_llave_maestra <<"Abre directamente el cuarto de contadores"
                             <<"Abre la puerta donde guardan llave de los contadores";
    QMap<QString, QStringList> mapa_spinners;

    mapa_spinners.insert("l_llaves_de_exterior", llaves_de_exterior);
    mapa_spinners.insert("l_llaves_maestras", llaves_maestras);
    mapa_spinners.insert("l_utilidad_de_llave_maestra", utilidad_de_llave_maestra);

    for (int i=0; i < mapa_spinners.keys().size(); i++) {
        QString object_name = mapa_spinners.keys().at(i);

        MyLabelSpinner *labelSpinner = ui->w_seccion_2->findChild<MyLabelSpinner*>(object_name);
        if(labelSpinner){
            labelSpinner->addItems(mapa_spinners.value(object_name));
        }
    }
    connect(ui->l_llaves_maestras, &MyLabelSpinner::itemSelected,
            ui->l_utilidad_de_llave_maestra, &MyLabelSpinner::show);

    hideAll();
    populateView();
}

Seccion_LLaves_de_Puertas::~Seccion_LLaves_de_Puertas()
{
    delete ui;
}

void Seccion_LLaves_de_Puertas::on_rb_llaves_de_exterior_toggled(bool checked)
{
    if(checked){
        ui->l_llaves_de_exterior->show();
        ui->lb_llaves_de_exterior->show();
    }else{
        ui->l_llaves_de_exterior->hide();
        ui->lb_llaves_de_exterior->hide();
    }
}

void Seccion_LLaves_de_Puertas::on_rb_llaves_maestras_toggled(bool checked)
{
    if(checked){
        ui->l_llaves_maestras->show();
        ui->lb_llaves_maestras->show();
        ui->l_utilidad_de_llave_maestra->show();
    }else{
        ui->l_llaves_maestras->hide();
        ui->lb_llaves_maestras->hide();
        ui->l_utilidad_de_llave_maestra->hide();
    }
}

void Seccion_LLaves_de_Puertas::on_rb_llave_especifica_toggled(bool checked)
{
    if(checked){
        ui->widget_llaves_especificas->show();
    }else{
        ui->widget_llaves_especificas->hide();
    }
}
void Seccion_LLaves_de_Puertas::on_rb_sin_comprobar_toggled(bool checked)
{

}

void Seccion_LLaves_de_Puertas::hideAll(){
    ui->l_llaves_maestras->hide();
    ui->lb_llaves_maestras->hide();
    ui->widget_llaves_especificas->hide();
    ui->l_llaves_de_exterior->hide();
    ui->lb_llaves_de_exterior->hide();
    ui->l_utilidad_de_llave_maestra->hide();
}


void Seccion_LLaves_de_Puertas::on_pb_cancelar_clicked()
{
    this->close();
    emit reject();
}

void Seccion_LLaves_de_Puertas::on_pb_aceptar_clicked()
{
    guardarDatos();
    emit itac_json(itac);
    emit accept();
    this->close();
}

void Seccion_LLaves_de_Puertas::populateView(){
    QString siempre_abierto = itac.value(siempre_abierto_itacs).toString();
    QString llave = itac.value(tipo_llave_itacs).toString().toLower();
    QString extra_llaves = itac.value(extras_llaves_itacs).toString().trimmed();

    if(siempre_abierto == "TRUE"){
        ui->rb_siempre_abierto->setChecked(true);
    }else{
        ui->rb_siempre_abierto->setChecked(false);
    }

    if(llave.contains("exterior")){
        ui->rb_llaves_de_exterior->setChecked(true);
        if(checkIfFieldIsValid(extra_llaves)){
            ui->l_llaves_de_exterior->setText(extra_llaves);
        }
    }
    else if(llave.contains("maestras")){
        ui->rb_llaves_maestras->setChecked(true);
        if(checkIfFieldIsValid(extra_llaves)){
            QStringList extras = extra_llaves.split("\n");
            if(extras.size()>1){
                ui->l_llaves_maestras->setText(extras.at(0));
                ui->l_utilidad_de_llave_maestra->setText(extras.at(1));
            }
        }
    }
    else if(llave.contains("especifica") || llave.contains("específica")){
        ui->rb_llave_especifica->setChecked(true);
        if(checkIfFieldIsValid(extra_llaves)){
            QStringList extras = extra_llaves.split("\n");
            for (int i=0; i < extras.size(); i++) {
                QString extra = "cb_"+extras.at(i).trimmed();
                extra = extra.replace(" ", "_").trimmed();
                QCheckBox *cb = ui->w_seccion_2->findChild<QCheckBox*>(extra);
                if(cb && cb->isWidgetType()){
                    cb->setChecked(true);
                }
            }
        }
    }
    else if(llave.contains("comprobar")){
        ui->rb_sin_comprobar->setChecked(true);
    }

    QString nota = itac.value(llaves_nota_itacs).toString();
    if(checkIfFieldIsValid(nota)){
        ui->pt_nota->setPlainText(nota);
    }
}

void Seccion_LLaves_de_Puertas::guardarDatos(){
    float puntuacion_seccion2 = 0;
    QString siempre_abierto = "FALSE";
    QString llave = "";
    QString extra_llaves = "";

    if(ui->rb_siempre_abierto->isChecked()){
        siempre_abierto = "TRUE";
        puntuacion_seccion2+=1.5;
    }
    if(ui->rb_llaves_de_exterior->isChecked()){
        llave = "Llaves de exterior";
        extra_llaves += ui->l_llaves_de_exterior->currentText();
        if(extra_llaves == "Allen" || extra_llaves == "Triángulo"){
            puntuacion_seccion2+=1.5;
        }
        else if(extra_llaves == "Cuadrado" || extra_llaves == "CABB" || extra_llaves == "Otros"){
            puntuacion_seccion2+=0.75;
        }
    }
    else if(ui->rb_llaves_maestras->isChecked()){
        llave = "Llaves maestras";
        extra_llaves += ui->l_llaves_maestras->currentText() + "\n";
        if(extra_llaves.contains("IBERDROLA") || extra_llaves.contains("Gas")){
            puntuacion_seccion2+=1.5;
        }else if(extra_llaves.contains("Del Gestor")){
            puntuacion_seccion2+=1;
        }else if(extra_llaves.contains("Otros")){
            puntuacion_seccion2+=0.75;
        }
        extra_llaves += ui->l_utilidad_de_llave_maestra->currentText();
    }
    else if(ui->rb_llave_especifica->isChecked()){
        llave = "Llave especifica";
        if(siempre_abierto == "TRUE"){
            puntuacion_seccion2 += (-0.5);
        }else{
            puntuacion_seccion2 = 0;
        }
        int c= ui->widget_llaves_especificas->children().size();
        for (int i=0; i < c; i++) {
            if(static_cast<QCheckBox*>((ui->widget_llaves_especificas->
                                        children().at(i)))->isChecked()){
                QString extra = static_cast<QCheckBox*>((ui->widget_llaves_especificas->
                                                         children().at(i)))->objectName();
                extra = extra.replace("cb_", "").replace("_", " ").trimmed();
                extra_llaves += extra + "\n";
            }
        }
    }
    else if(ui->rb_sin_comprobar->isChecked()){
        llave = "Sin comprobar";
        puntuacion_seccion2 = 0;
        extra_llaves = "";
    }

    itac.insert(siempre_abierto_itacs, siempre_abierto);

    itac.insert(tipo_llave_itacs, llave);
    itac.insert(extras_llaves_itacs, extra_llaves);

    if(puntuacion_seccion2 > 1.5){
        puntuacion_seccion2 = 1.5;
    }
    QString puntuacion_seccion = QString::number(puntuacion_seccion2, 'f', 2);
    itac.insert(puntuacion_seccion2_itacs, puntuacion_seccion);

    QString nota = ui->pt_nota->toPlainText();
    if(!nota.isEmpty()){
        itac.insert(llaves_nota_itacs, nota);
    }
}

bool Seccion_LLaves_de_Puertas::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}



