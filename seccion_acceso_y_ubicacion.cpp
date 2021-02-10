#include "seccion_acceso_y_ubicacion.h"
#include "ui_seccion_acceso_y_ubicacion.h"
#include "structure_itac.h"

Seccion_Acceso_y_Ubicacion::Seccion_Acceso_y_Ubicacion(QWidget *parent, QJsonObject jsonObject) :
    QWidget(parent),
    ui(new Ui::Seccion_Acceso_y_Ubicacion)
{
    ui->setupUi(this);
    this->setWindowTitle("Sección 1 Acceso y Ubicación");
    itac = jsonObject;

    QStringList  dentro_del_edificio, exterior;
    dentro_del_edificio <<"Portal"<<"Garaje"<<"Sótano"<<"Planta"<<"Descansillo"
                       <<"Entreplanta"<<"Entresuelo"<<"Otros";
    exterior <<"Armario o Nicho"<<"Caseta"<<"Arqueta"<<"Aire Libre"<< "Cuarto"<<"Otros";

    QMap<QString, QStringList> mapa_spinners;

    mapa_spinners.insert("l_dentro_del_edificio", dentro_del_edificio);
    mapa_spinners.insert("l_exterior", exterior);

    for (int i=0; i < mapa_spinners.keys().size(); i++) {
        QString object_name = mapa_spinners.keys().at(i);

        MyLabelSpinner *labelSpinner = ui->w_seccion_2->findChild<MyLabelSpinner*>(object_name);
        if(labelSpinner){
            labelSpinner->addItems(mapa_spinners.value(object_name));
        }
    }
    connect(ui->l_dentro_del_edificio, &MyLabelSpinner::itemSelected,
            ui->l_exterior, &MyLabelSpinner::clear);
    connect(ui->l_exterior, &MyLabelSpinner::itemSelected,
            ui->l_dentro_del_edificio, &MyLabelSpinner::clear);

    ui->l_dentro_del_edificio->clear();
    ui->l_exterior->clear();

    hideAll();

    populateView();
}

Seccion_Acceso_y_Ubicacion::~Seccion_Acceso_y_Ubicacion()
{
    delete ui;
}
void Seccion_Acceso_y_Ubicacion::on_pb_cancelar_clicked()
{
    this->close();
    emit reject();
}

void Seccion_Acceso_y_Ubicacion::on_pb_aceptar_clicked()
{
    guardarDatos();
    emit itac_json(itac);
    emit accept();
    this->close();
}

void Seccion_Acceso_y_Ubicacion::populateView(){
    QString ubicacion = itac.value(acceso_ubicacion_ubicacion_itacs).toString();
    QString acceso = itac.value(acceso_ubicacion_acceso_itacs).toString();
    QString extra = itac.value(extra_acceso_ubicacion_itacs).toString();

    ubicacion = "rb_"+ubicacion.replace(" ", "_").trimmed();
    QRadioButton *rb = ui->groupBox_ubicacion->findChild<QRadioButton*>(ubicacion);
    if(rb){
        rb->setChecked(true);
    }
    acceso = "rb_"+acceso.replace(" ", "_").trimmed();
    QRadioButton *rb2 = ui->groupBox_acceso->findChild<QRadioButton*>(acceso);
    if(rb2){
        rb2->setChecked(true);
    }

    if(ui->l_dentro_del_edificio->getSpinnerList().contains(extra)){
        ui->l_dentro_del_edificio->setText(extra);
    }
    if(ui->l_exterior->getSpinnerList().contains(extra)){
        ui->l_exterior->setText(extra);
    }
    QString nota = itac.value(acceso_ubicacion_nota_itacs).toString();
    if(checkIfFieldIsValid(nota)){
        ui->pt_nota->setPlainText(nota);
    }
}

void Seccion_Acceso_y_Ubicacion::guardarDatos(){
    int puntuacion_seccion1 = 0;
    QString dentro = ui->l_dentro_del_edificio->currentText();
    QString exterior = ui->l_exterior->currentText();

    for (int i=0; i < ui->groupBox_ubicacion->children().size(); i++) {
        QRadioButton *rb = static_cast<QRadioButton*>((ui->groupBox_ubicacion->
                                                       children().at(i)));
        if(rb && rb->isWidgetType()){
            if(rb->isChecked()){
                QString name = rb->objectName();
                name = name.replace("rb_", "").replace("_", " ").trimmed();
                itac.insert(acceso_ubicacion_ubicacion_itacs, name);
            }
        }
    }
    for (int i=0; i < ui->groupBox_acceso->children().size(); i++) {
        QRadioButton *rb = static_cast<QRadioButton*>((ui->groupBox_acceso->
                                                       children().at(i)));
        if(rb && rb->isWidgetType()){
            if(rb->isChecked()){
                QString name = rb->objectName();
                name = name.replace("rb_", "").replace("_", " ").trimmed();
                itac.insert(acceso_ubicacion_acceso_itacs, name);
            }
        }
    }
    if(!dentro.isEmpty()){
        itac.insert(extra_acceso_ubicacion_itacs, dentro);
    }else if(!exterior.isEmpty()){
        itac.insert(extra_acceso_ubicacion_itacs, exterior);
    }

    QString puntuacion_seccion = QString::number(puntuacion_seccion1);
    itac.insert(puntuacion_seccion1_itacs, puntuacion_seccion);

    QString nota = ui->pt_nota->toPlainText();
    if(!nota.isEmpty()){
        itac.insert(acceso_ubicacion_nota_itacs, nota);
    }
}
bool Seccion_Acceso_y_Ubicacion::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

void Seccion_Acceso_y_Ubicacion::on_rb_Exterior_toggled(bool checked)
{
    if(checked){
        ui->lb_exterior->show();
        ui->l_exterior->show();
        ui->groupBox_acceso->show();
    }else{
        ui->lb_exterior->hide();
        ui->l_exterior->hide();
        ui->groupBox_acceso->hide();
    }
}
void Seccion_Acceso_y_Ubicacion::hideAll(){
    ui->lb_exterior->hide();
    ui->l_exterior->hide();
    ui->groupBox_acceso->hide();
    ui->lb_dentro_del_edificio->hide();
    ui->l_dentro_del_edificio->hide();
}

void Seccion_Acceso_y_Ubicacion::on_rb_Dentro_de_Edificio_toggled(bool checked)
{
    if(checked){
        ui->lb_dentro_del_edificio->show();
        ui->l_dentro_del_edificio->show();
    }else{
        ui->lb_dentro_del_edificio->hide();
        ui->l_dentro_del_edificio->hide();
    }
}
