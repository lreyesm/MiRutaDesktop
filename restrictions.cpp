#include "restrictions.h"
#include "ui_restrictions.h"
#include "cliente.h"
#include "restriction.h"
#include "globalfunctions.h"
#include <QHBoxLayout>
#include <QLabel>
#include "clickablelabel.h"
#include <QMessageBox>

Restrictions::Restrictions(QWidget *parent, QJsonObject cliente, QString empresa) :
    QWidget(parent),
    ui(new Ui::Restrictions)
{
    ui->setupUi(this);

    this->empresa = empresa;
    this->cliente = cliente;
    QString permisos = cliente.value(permisos_clientes).toString();
    if(GlobalFunctions::checkIfFieldIsValid(permisos)){
        QJsonDocument d = QJsonDocument::
                fromJson(permisos.toUtf8());
        if(d.isObject()){
            restricciones = d.object();
            fillWidgetRestricciones();
        }
    }
}

Restrictions::~Restrictions()
{
    delete ui;
}

void Restrictions::fillWidgetRestricciones(){
    for(int i=0; i < restricciones.keys().size(); i++){
        QString key = restricciones.keys().at(i);
        QString value = restricciones.value(key).toString();
        addtoRestrictionWidget(key, value);
    }
}
void Restrictions::on_pb_agregar_clicked()
{
    Restriction *res = new Restriction(this, empresa);
    connect(res, &Restriction::selectedRestriction, this, &Restrictions::addRestriction);
    res->show();
}
void Restrictions::addtoRestrictionWidget(QString field, QString value){
    if(GlobalFunctions::checkIfFieldIsValid(field) &&
            GlobalFunctions::checkIfFieldIsValid(value)){
        QWidget *widget = new QWidget();
        widget->setObjectName("w_"+field);
        QHBoxLayout *layout= new QHBoxLayout;
        layout->setAlignment(Qt::AlignCenter);
        layout->setSpacing(10);
        widget->setLayout(layout);

        QFont f = this->font();
        f.setBold(true);
        QLabel *key_label = new QLabel(field +" :  ");
        key_label->setAlignment(Qt::AlignCenter);
        key_label->setFont(f);
        key_label->setFixedWidth(250);

        f.setBold(false);
        QLabel *value_label = new QLabel(value);
        value_label->setAlignment(Qt::AlignCenter);
        value_label->setFont(f);
        ClickableLabel *button = new ClickableLabel();
        button->setBackgroundImage(("icons/delete_blue.png"));
        button->setObjectName(field);
        button->setFixedSize(25, 25);
        button->setScaledContents(true);
        connect(button, &ClickableLabel::objectClicked, this, &Restrictions::deleteRestrinction);

        widget->layout()->addWidget(key_label);
        widget->layout()->addWidget(value_label);
        widget->layout()->addWidget(button);

        ui->widget_restricciones->layout()->addWidget(widget);
    }
}
void Restrictions::deleteRestrinction(QString name){
    qDebug()<<name<<" -----------------------------------------------------------------------------";
    if(restricciones.contains(name)){
        restricciones.remove(name);
        GlobalFunctions::deleteAllChilds(ui->widget_restricciones);
        fillWidgetRestricciones();
    }
    emit setRestrictions(GlobalFunctions::convertJsonObjectToString(restricciones));
}
void Restrictions::deleteAllRestrinccions(){
    restricciones = QJsonObject();
    GlobalFunctions::deleteAllChilds(ui->widget_restricciones);
    emit setRestrictions(GlobalFunctions::convertJsonObjectToString(restricciones));
}

void Restrictions::addRestriction(QString field, QString value){
    if(restricciones.contains(field)){
        QString prevValue = restricciones.value(field).toString().trimmed();
        if(GlobalFunctions::checkIfFieldIsValid(field) && !prevValue.contains(value)){
            value = prevValue+"; "+value;
        }
        GlobalFunctions::deleteAllChilds(ui->widget_restricciones);

        restricciones.insert(field, value);
        fillWidgetRestricciones();
    }
    else{
        restricciones.insert(field, value);
        addtoRestrictionWidget(field, value);
    }
    emit setRestrictions(GlobalFunctions::convertJsonObjectToString(restricciones));
}


void Restrictions::on_pb_eliminar_clicked()
{
    GlobalFunctions gf(this);
    if(gf.showQuestion(this, "Confirmación", "Desea eliminar todas las restricciones",
                             QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok){
        deleteAllRestrinccions();
    }
}
