#include "seccion_estado_de_las_valvulas.h"
#include "ui_seccion_estado_de_las_valvulas.h"
#include "structure_itac.h"

Seccion_Estado_de_las_Valvulas::Seccion_Estado_de_las_Valvulas(QWidget *parent, QJsonObject jsonObject) :
    QWidget(parent),
    ui(new Ui::Seccion_Estado_de_las_Valvulas)
{
    ui->setupUi(this);
    this->setWindowTitle("Sección 5 Estado de conservacion y diseño de zona de trabajo");
    itac = jsonObject;

    QStringList  valvula_de_salida_al_contador, estado_general,
            valvula_de_entrada_al_contador, estado_valvula_antiretorno;
    valvula_de_salida_al_contador <<"SIN COMPROBAR"<<"EN BUEN ESTADO"<<"ESTADO REGULAR"<<"EN MAL ESTADO";
    estado_general <<"SIN COMPROBAR"<<"EN BUEN ESTADO"<<"ESTADO REGULAR"<<"EN MAL ESTADO"<<"SIN COMPROBAR";
    valvula_de_entrada_al_contador <<"SIN COMPROBAR"<<"EN BUEN ESTADO"<<"ESTADO REGULAR"<<"EN MAL ESTADO";
    estado_valvula_antiretorno <<"SIN COMPROBAR"<<"EN BUEN ESTADO"<<"ESTADO REGULAR"<<"EN MAL ESTADO";

    QMap<QString, QStringList> mapa_spinners;

    mapa_spinners.insert("l_valvula_de_salida_al_contador", valvula_de_salida_al_contador);
    mapa_spinners.insert("l_estado_general", estado_general);
    mapa_spinners.insert("l_valvula_de_entrada_al_contador", valvula_de_entrada_al_contador);
    mapa_spinners.insert("l_estado_valvula_antiretorno", estado_valvula_antiretorno);

    for (int i=0; i < mapa_spinners.keys().size(); i++) {
        QString object_name = mapa_spinners.keys().at(i);

        MyLabelSpinner *labelSpinner = ui->w_seccion_5->findChild<MyLabelSpinner*>(object_name);
        if(labelSpinner){
            labelSpinner->addItems(mapa_spinners.value(object_name));
        }
    }
    populateView();
}

Seccion_Estado_de_las_Valvulas::~Seccion_Estado_de_las_Valvulas()
{
    delete ui;
}
QJsonObject Seccion_Estado_de_las_Valvulas::setExcelenteEnGeneral(QJsonObject itac){

    itac.insert(valvula_general_itacs, "Sin comprobar");
    itac.insert(extras_valvula_general_itacs,"EN BUEN ESTADO");

    itac.insert(valvula_antiretorno_itacs, "Sin comprobar");
    itac.insert(extras_valvula_antiretorno_itacs,"EN BUEN ESTADO");

    itac.insert(valvula_entrada_itacs, "Sin comprobar");
    itac.insert(extras_valvula_entrada_itacs,"EN BUEN ESTADO");

    itac.insert(valvula_salida_itacs, "Sin comprobar");
    itac.insert(extras_valvula_salida_itacs,"EN BUEN ESTADO");

    itac.insert(puntuacion_seccion5_itacs,"5");

    return itac;
}

QJsonObject Seccion_Estado_de_las_Valvulas:: setMalEstadoEnGeneral(QJsonObject itac){

    itac.insert(valvula_general_itacs, "Sin comprobar");
    itac.insert(extras_valvula_general_itacs,"EN MAL ESTADO");

    itac.insert(valvula_antiretorno_itacs, "Sin comprobar");
    itac.insert(extras_valvula_antiretorno_itacs,"EN MAL ESTADO");

    itac.insert(valvula_entrada_itacs, "De Batería M.M.V");
    itac.insert(extras_valvula_entrada_itacs,"EN MAL ESTADO");

    itac.insert(valvula_salida_itacs, "De Batería M.M.V");
    itac.insert(extras_valvula_salida_itacs,"EN MAL ESTADO");

    itac.insert(puntuacion_seccion5_itacs,"0");

    return itac;
}
void Seccion_Estado_de_las_Valvulas::guardarDatos()
{
    float puntuacion_seccion5 = 0;

    for (int i=0; i < ui->groupBox_general->children().size(); i++) {
        QRadioButton *rb = static_cast<QRadioButton*>((ui->groupBox_general->
                                                       children().at(i)));
        if(rb && rb->isWidgetType()){
            if(rb->isChecked()){
                QString name = rb->text();
                itac.insert(valvula_general_itacs, name);
                QString estado = ui->l_estado_general->currentText();
                itac.insert(extras_valvula_general_itacs, estado);
                if(name.contains("No Tiene",Qt::CaseInsensitive)){
                    puntuacion_seccion5+=(-0.8);
                }
                else if(name.contains("Bola",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.8;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=1;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-0.8);
                   }
                }
                else if(name.contains("Compuerta",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.3;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=0.8;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-0.8);
                   }
                }
                else if(name.contains("Mariposa",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.8;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=1;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-0.8);
                   }
                }
            }
        }
    }
    for (int i=0; i < ui->groupBox_entrada->children().size(); i++) {
        QRadioButton *rb = static_cast<QRadioButton*>((ui->groupBox_entrada->
                                                       children().at(i)));
        if(rb && rb->isWidgetType()){
            if(rb->isChecked()){
                QString name = rb->text();
                itac.insert(valvula_entrada_itacs, name);
                QString estado = ui->l_valvula_de_entrada_al_contador->currentText();
                itac.insert(extras_valvula_entrada_itacs, estado);
                if(name.contains("No Tiene",Qt::CaseInsensitive)){
                    puntuacion_seccion5+=(-3);
                }
                else if(name.contains("De Bola",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=2.8;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=3;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-3);
                   }
                }
                else if(name.contains("Compuerta",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=2;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=2.8;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-3);
                   }
                }
                else if(name.contains("Batería Bola",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=2;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=2.8;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-3);
                   }
                }
                else if(name.contains("M.T.C",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=2.3;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=2.5;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-3);
                   }
                }
                else if(name.contains("M.P.F",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=2;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=2.3;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-3);
                   }
                }
                else if(name.contains("M.M.V",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=1.5;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-3);
                   }
                }
            }
        }
    }
    for (int i=0; i < ui->groupBox_salida->children().size(); i++) {
        QRadioButton *rb = static_cast<QRadioButton*>((ui->groupBox_salida->
                                                       children().at(i)));
        if(rb && rb->isWidgetType()){
            if(rb->isChecked()){
                QString name = rb->text();
                itac.insert(valvula_salida_itacs, name);
                QString estado = ui->l_valvula_de_salida_al_contador->currentText();
                itac.insert(extras_valvula_salida_itacs, estado);
                if(name.contains("No Tiene",Qt::CaseInsensitive)){
                    puntuacion_seccion5+=(-1);
                }
                else if(name.contains("De Bola",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.3;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=0.5;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-1);
                   }
                }
                else if(name.contains("Compuerta",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.15;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=0.3;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-1);
                   }
                }
                else if(name.contains("Batería Bola",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.3;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=0.5;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-1);
                   }
                }
                else if(name.contains("M.T.C",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.2;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=0.25;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-1);
                   }
                }
                else if(name.contains("M.P.F",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.15;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=0.3;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-1);
                   }
                }
                else if(name.contains("M.M.V",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.1;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=0.2;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-1);
                   }
                }
            }
        }
    }
    for (int i=0; i < ui->groupBox_antiretorno->children().size(); i++) {
        QRadioButton *rb = static_cast<QRadioButton*>((ui->groupBox_antiretorno->
                                                       children().at(i)));
        if(rb && rb->isWidgetType()){
            if(rb->isChecked()){
                QString name = rb->text();
                itac.insert(valvula_antiretorno_itacs, name);
                QString estado = ui->l_estado_valvula_antiretorno->currentText();
                itac.insert(extras_valvula_antiretorno_itacs, estado);
                if(name.contains("No Tiene",Qt::CaseInsensitive)){
                    puntuacion_seccion5+=(-1);
                }
                else if(name.contains("de Salida",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.25;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=0.5;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(0);
                   }
                }
                else if(name.contains("Cilindro",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.25;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=0.5;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-1);
                   }
                }
                else if(name.contains("Escuadra",Qt::CaseInsensitive)){
                    if(estado == "SIN COMPROBAR" || estado == "ESTADO REGULAR"){
                         puntuacion_seccion5+=0.2;
                    }else if(estado == "EN BUEN ESTADO"){
                        puntuacion_seccion5+=0.3;
                   }else if(estado == "EN MAL ESTADO"){
                        puntuacion_seccion5+=(-1);
                   }
                }

            }
        }
    }

    QString puntuacion_seccion = QString::number(puntuacion_seccion5, 'f', 2);
    itac.insert(puntuacion_seccion5_itacs, puntuacion_seccion);

    QString nota = ui->pt_nota->toPlainText();
    if(!nota.isEmpty()){
        itac.insert(estado_de_valvulas_nota_itacs, nota);
    }
}

void Seccion_Estado_de_las_Valvulas::populateView()
{
    ui->rb_salida_no_tiene->setChecked(true);
    ui->rb_entrada_no_tiene->setChecked(true);
    ui->rb_general_no_tiene->setChecked(true);
    ui->rb_antiretorno_no_tiene->setChecked(true);

    QString general = itac.value(valvula_general_itacs).toString().trimmed();
    QString entrada = itac.value(valvula_entrada_itacs).toString().trimmed();;
    QString salida = itac.value(valvula_salida_itacs).toString().trimmed();
    QString antiretorno = itac.value(valvula_antiretorno_itacs).toString().trimmed();

    QString extras_general = itac.value(extras_valvula_general_itacs).toString().trimmed();
    QString extras_entrada = itac.value(extras_valvula_entrada_itacs).toString().trimmed();
    QString extras_salida = itac.value(extras_valvula_salida_itacs).toString().trimmed();
    QString extras_antiretorno = itac.value(extras_valvula_antiretorno_itacs).toString().trimmed();

    int c = ui->groupBox_general->children().size();
    for (int i=0; i < c; i++) {
        QRadioButton *rb = static_cast<QRadioButton*>((ui->groupBox_general->
                                                       children().at(i)));
        if(rb && rb->isWidgetType()){
            if(rb->text() == general){
                rb->setChecked(true);
                if(ui->l_estado_general->getSpinnerList().contains(extras_general)){
                    ui->l_estado_general->setText(extras_general);
                }
                break;
            }
        }
    }
    c = ui->groupBox_entrada->children().size();
    for (int i=0; i < c; i++) {
        QRadioButton *rb = static_cast<QRadioButton*>((ui->groupBox_entrada->
                                                       children().at(i)));
        if(rb && rb->isWidgetType()){
            if(rb->text() == entrada){
                rb->setChecked(true);
                if(ui->l_valvula_de_entrada_al_contador->getSpinnerList().contains(extras_entrada)){
                    ui->l_valvula_de_entrada_al_contador->setText(extras_entrada);
                }
                break;
            }
        }
    }
    c = ui->groupBox_salida->children().size();
    for (int i=0; i < c; i++) {
        QRadioButton *rb = static_cast<QRadioButton*>((ui->groupBox_salida->
                                                       children().at(i)));
        if(rb && rb->isWidgetType()){
            if(rb->text() == salida){
                rb->setChecked(true);
                if(ui->l_valvula_de_salida_al_contador->getSpinnerList().contains(extras_salida)){
                    ui->l_valvula_de_salida_al_contador->setText(extras_salida);
                }
                break;
            }
        }
    }
    c = ui->groupBox_antiretorno->children().size();
    for (int i=0; i < c; i++) {
        QRadioButton *rb = static_cast<QRadioButton*>((ui->groupBox_antiretorno->
                                                       children().at(i)));
        if(rb && rb->isWidgetType()){
            if(rb->text() == antiretorno){
                rb->setChecked(true);
                if(ui->l_estado_valvula_antiretorno->getSpinnerList().contains(extras_antiretorno)){
                    ui->l_estado_valvula_antiretorno->setText(extras_antiretorno);
                }
                break;
            }
        }
    }

    QString nota = itac.value(estado_de_valvulas_nota_itacs).toString();
    if(checkIfFieldIsValid(nota)){
        ui->pt_nota->setPlainText(nota);
    }
}

void Seccion_Estado_de_las_Valvulas::on_pb_cancelar_clicked()
{
    this->close();
    emit reject();
}

void Seccion_Estado_de_las_Valvulas::on_pb_aceptar_clicked()
{
    guardarDatos();
    emit itac_json(itac);
    emit accept();
    this->close();
}
bool Seccion_Estado_de_las_Valvulas::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

void Seccion_Estado_de_las_Valvulas::on_rb_general_no_tiene_toggled(bool checked)
{
    if(checked){
        ui->l_estado_general->hide();
        ui->lb_estado_general->hide();
    }else{
        ui->l_estado_general->show();
        ui->lb_estado_general->show();
    }
}

void Seccion_Estado_de_las_Valvulas::on_rb_entrada_no_tiene_toggled(bool checked)
{
    if(checked){
        ui->l_valvula_de_entrada_al_contador->hide();
        ui->lb_valvula_de_entrada_al_contador->hide();
    }else{
        ui->l_valvula_de_entrada_al_contador->show();
        ui->lb_valvula_de_entrada_al_contador->show();
    }
}

void Seccion_Estado_de_las_Valvulas::on_rb_salida_no_tiene_toggled(bool checked)
{
    if(checked){
        ui->l_valvula_de_salida_al_contador->hide();
        ui->lb_valvula_de_salida_al_contador->hide();
    }else{
        ui->l_valvula_de_salida_al_contador->show();
        ui->lb_valvula_de_salida_al_contador->show();
    }
}

void Seccion_Estado_de_las_Valvulas::on_rb_antiretorno_no_tiene_toggled(bool checked)
{
    if(checked){
        ui->l_estado_valvula_antiretorno->hide();
        ui->lb_estado_valvula_antiretorno->hide();
    }else{
        ui->l_estado_valvula_antiretorno->show();
        ui->lb_estado_valvula_antiretorno->show();
    }
}
