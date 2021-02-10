#include "restriction.h"
#include "ui_restriction.h"
#include "new_table_structure.h"
#include <QCompleter>
#include "globalfunctions.h"
#include "QProgressIndicator.h"

Restriction::Restriction(QWidget *parent, QString empresa) :
    QWidget(parent),
    ui(new Ui::Restriction)
{
    ui->setupUi(this);

    this->empresa = empresa;

    fillSpinnerColumn();
}

Restriction::~Restriction()
{
    delete ui;
}

void Restriction::fillSpinnerColumn(){
    QMap <QString,QString> mapa = fillMap();
    QStringList campos = mapa.keys();
    QString campo;
    GlobalFunctions::sortStringList(campos, GlobalFunctions::MAYOR_A_MENOR);
    foreach (campo, campos) {
        ui->l_spinner_campos->addItem(campo);
    }
    ui->l_spinner_campos->hideSpinnerList();
    connect(ui->l_spinner_campos, &MyLabelSpinner::itemSelected, this, &Restriction::fillValuesWithColumn);
}

void Restriction::fillValuesWithColumn(QString column){
    show_loading(ui->widget, QPoint(ui->widget->width()/2, ui->widget->height()/2));
    QMap <QString,QString> mapa = fillMap();
    if(mapa.contains(column)){
        column = mapa.value(column);
    }
    QStringList keys, values;

    keys << "empresa" << "columna" << "tabla";
    values << empresa << column << "tareas";

    this->keys = keys;
    this->values = values;

    QTimer::singleShot(500, this, SLOT(getColumnValues()));

}
void Restriction::getColumnValues(){
    QEventLoop *q = new QEventLoop();
    connect(this, &Restriction::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, SLOT(get_all_column_values_request()));

    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        break;

    case database_comunication::script_result::ok:
        QTimer::singleShot(100, this, SLOT(fillValues()));
        break;
    }
    delete q;
    emit hidingLoading();
}
void Restriction::show_loading(QWidget *parent, QPoint pos, QColor color, int w, int h){
    emit hidingLoading();

    QLabel *label_back = new QLabel(parent);

    connect(this, &Restriction::hidingLoading, label_back, &QLabel::hide);
    connect(this, &Restriction::hidingLoading, label_back, &QLabel::deleteLater);
    int border = 12;
    label_back->setFixedSize(w + border, h + border);
    pos.setX(pos.x()-w/2);
    pos.setY(pos.y()-h/2);
    label_back->move(pos);
    QString circle_radius_string = QString::number(static_cast<int>((w+border)/2));
    //    QString colorRBG = getColorString(color);
    label_back->setStyleSheet("background-color: #FFFFFF;"
                              "border-radius: "+circle_radius_string+"px;"
                              /*"border: 1px solid #"+colorRBG+";"*/);
    label_back->show();

    QProgressIndicator *pi = new QProgressIndicator(parent);
    connect(this, &Restriction::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &Restriction::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color);
    pi->setFixedSize(w, h);
    pi->startAnimation();
    pos.setX(pos.x()+border/2 + 1);
    pos.setY(pos.y()+border/2 + 1);
    pi->move(pos);
    pi->show();
}
void Restriction::fillValues(){
    QStringList values;
    QStringList keys = jsonObjectAnswer.keys();
    for (int i= 0; i < keys.size(); i++) {
        QString key = keys.at(i);
        if(!key.contains("query") && !key.contains("count_values")){
            QString value = jsonObjectAnswer.value(key).toString();
            if(!values.contains(value, Qt::CaseInsensitive) && GlobalFunctions::checkIfFieldIsValid(value)){
                values << value;
            }
        }
    }
    QCompleter *completer = new QCompleter(values, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_value->setCompleter(completer);
}
void Restriction::on_pb_aceptar_clicked()
{
    emit selectedRestriction(ui->l_spinner_campos->currentText(), ui->le_value->text());
    this->close();
}

void Restriction::on_pb_cancelar_clicked()
{
    this->close();
}
void Restriction::get_all_column_values_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES,keys,values);
}

void Restriction::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this,
               SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_ALL_COLUMN_VALUES)
    {
        jsonObjectAnswer = database_comunication::getJsonObject(byte_array);
        result = database_comunication::script_result::ok;
    }
    emit script_excecution_result(result);
}

QMap <QString,QString> Restriction::fillMap(){

    QMap <QString,QString> mapa;
    //    mapa.insert("Id.Ord",idOrdenCABB);
    //    mapa.insert("NUMIN", numero_interno);
    mapa.insert("CAUSA ORIGEN", ANOMALIA);
    mapa.insert("C.ACCIÓN ORD.", tipo_tarea);
    //    mapa.insert("AREALIZAR",AREALIZAR);
    //    mapa.insert("INTERVENCI",INTERVENCION);
    //    mapa.insert("REPARACION", reparacion);
    //    mapa.insert("PROP.",propiedad);
    //    mapa.insert("AÑO o PREFIJO", CONTADOR_Prefijo_anno);
    mapa.insert("SERIE",numero_serie_contador);
    mapa.insert("MARCA",marca_contador);
    //    mapa.insert("CALIBRE",calibre_toma);
    //    mapa.insert("RUEDAS", ruedas);
    //    mapa.insert("FECINST",fecha_instalacion);
    //    mapa.insert("ACTIVI",actividad);
    //    mapa.insert("EMPLAZA", emplazamiento);
    //    mapa.insert("ACCESO",acceso);
    mapa.insert("CALLE",calle);
    mapa.insert("NUME",numero);
    mapa.insert("BIS",BIS);
    //    mapa.insert("PISO",piso);
    //    mapa.insert("MANO",mano);
    mapa.insert("MUNICIPIO",poblacion);
    mapa.insert("NOMBRE",nombre_cliente);
    mapa.insert("ABONADO",numero_abonado);
    mapa.insert("CODLEC",ruta);
    //    mapa.insert("FECEMISIO",FECEMISIO);
    //    mapa.insert("FECULTREP",FECULTREP);
    //    mapa.insert("DATOS ESPECIFICOS",observaciones);
    mapa.insert("SECTOR P",zona);
    mapa.insert("PRIORIDAD",prioridad);
    mapa.insert("RS",resultado);
    //    mapa.insert("F_EJEC",F_INST);
    //    mapa.insert("LECT_INS",lectura_contador_nuevo);
    //    mapa.insert("EMPLAZADV",emplazamiento_devuelto);
    //    mapa.insert("RESTO_EM",RESTO_EM);
    //    mapa.insert("LECT_LEV",lectura_actual);
    //    mapa.insert("OBSERVADV",observaciones_devueltas);
    mapa.insert("Estado",status_tarea);
    mapa.insert("MARCADV",marca_devuelta);
    //    mapa.insert("CALIBREDV",calibre_real);
    //    mapa.insert("RUEDASDV",RUEDASDV);
    //    mapa.insert("LONG",LARGO);
    //    mapa.insert("LONGDV",largo_devuelto);
    mapa.insert("seriedv", numero_serie_contador_devuelto);
    //    mapa.insert("PREFIJO DV",CONTADOR_Prefijo_anno_devuelto);
    mapa.insert("CAUSA DESTINO",AREALIZAR_devuelta);
    //    mapa.insert("intervencidv",intervencion_devuelta);
    //    mapa.insert("RESTEMPLAZA",RESTEMPLAZA);
    //    mapa.insert("FECH_CIERRE",FECH_CIERRE);
    mapa.insert("TIPORDEN",TIPORDEN);
    mapa.insert("EQUIPO",equipo);
    mapa.insert("OPERARIO",operario);
    //    mapa.insert("observaciones",MENSAJE_LIBRE);
    //    mapa.insert("TIPOFLUIDO",TIPOFLUIDO_devuelto);
    //    mapa.insert("idexport",idexport);
    //    mapa.insert("fech_facturacion",fech_facturacion);
    //    mapa.insert("fech_cierrenew",fecha_de_cambio);
    //    mapa.insert("fech_informacionnew",fech_informacionnew);
    mapa.insert("tipoRadio",tipoRadio_devuelto);
    //    mapa.insert("REQUERIDA",marcaR);
    //    mapa.insert("Módulo",numero_serie_modulo);
    mapa.insert("C.EMPLAZAMIENTO",codigo_de_geolocalizacion);
    //    mapa.insert("Geolocalización",url_geolocalizacion);

    return mapa;
}
