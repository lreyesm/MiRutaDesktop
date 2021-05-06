#include "screen_tabla_tareas.h"
#include "ui_screen_tabla_tareas.h"

#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QUrlQuery>
#include <QtXlsx>
#include <QtXlsx/xlsxformat.h>
#include "selectionorder.h"
#include "database_comunication.h"
#include "global_variables.h"
#include "new_table_structure.h"
#include <QDesktopWidget>
#include <QDebug>
#include "causa.h"
#include "ruta.h"
#include "itac.h"
#include <QScreen>
#include <dbtareascontroller.h>
#include "idordenassign.h"
#include "QProgressIndicator.h"
#include "globalfunctions.h"
#include "mylabelshine.h"

using namespace QXlsx;

int screen_tabla_tareas::lastIDSAT =0;
int screen_tabla_tareas::lastNUMFICHERO_EXPORTACION =0;
int screen_tabla_tareas::lastIDExp =0;
int screen_tabla_tareas::emailPermission =0;
QString screen_tabla_tareas::lastSync ="";

screen_tabla_tareas::screen_tabla_tareas(QWidget *parent, QString empresa) :
    QDialog(parent),
    database_com(),
    ui(new Ui::screen_tabla_tareas)
{
    setWindowFlags(Qt::CustomizeWindowHint);

    this->empresa = empresa;

    ui->setupUi(this);

    QRect rect = QGuiApplication::screens().first()->geometry();
    this->setGeometry(-1,0, rect.width(),rect.height());

    qDebug()<<"Ancho"<<QString::number(rect.width());

    model = nullptr;
    serverAlredyAnswered = false;
    loaded_from_file = false;
    Script_excecution_result = -1;
    jsonArraySelected = 0;
    rutaToDATFile = "";
    operatorName = "";

    populate_map();

    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));
}

screen_tabla_tareas::~screen_tabla_tareas()
{
    delete ui;
    delete model;
}

void screen_tabla_tareas::showEvent(QShowEvent *event){
    QWidget::showEvent(event);
    QTimer::singleShot(500, this, &screen_tabla_tareas::on_pb_load_data_from_file_clicked);
}

void screen_tabla_tareas::resizeEvent(QResizeEvent *event){

    QRect rect = QGuiApplication::screens().first()->geometry();
    QSize s = event->size();
    if(s.width() > rect.width() || s.height() > rect.height()){
        this->showNormal();
        this->setFixedSize(rect.width(), rect.height() - rect.height()/20);
        if(timer.isActive()){
            timer.stop();
        }
        timer.setInterval(200);
        timer.start();
        connect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
    }
    QWidget::resizeEvent(event);
}


void screen_tabla_tareas::populate_map()
{
    map_days_week.insert("lu.","Lunes");
    map_days_week.insert("ma.","Martes");
    map_days_week.insert("mi.","Miércoles");
    map_days_week.insert("ju.","Jueves");
    map_days_week.insert("vi.","Viernes");
    map_days_week.insert("sá.","Sábado");
    map_days_week.insert("do.","Domingo");

    map_days_week.insert("mo.","Lunes");
    map_days_week.insert("tu.","Martes");
    map_days_week.insert("we.","Miércoles");
    map_days_week.insert("th.","Jueves");
    map_days_week.insert("fr.","Viernes");
    map_days_week.insert("sa.","Sábado");
    map_days_week.insert("su.","Domingo");

    map_months.insert(1,"Enero");
    map_months.insert(2,"Febrero");
    map_months.insert(3,"Marzo");
    map_months.insert(4,"Abril");
    map_months.insert(5,"Mayo");
    map_months.insert(6,"Junio");
    map_months.insert(7,"Julio");
    map_months.insert(8,"Agosto");
    map_months.insert(9,"Septiembre");
    map_months.insert(10,"Octubre");
    map_months.insert(11,"Noviembre");
    map_months.insert(12,"Diciembre");

    //campos excel de salida a partir de dat
    map.insert(poblacion,"Población");
    map.insert(calle,"CALLE");
    map.insert(numero,"Nº");
    map.insert(BIS,"BIS");
    map.insert(piso,"PISO");
    map.insert(mano,"MANO");
    map.insert(CONTADOR_Prefijo_anno,"AÑO  O PREFIJO");
    map.insert(numero_serie_contador,"Nº SERIE");
    map.insert(calibre_toma,"CALIBRE");
    map.insert(calibre_real,"CALIBRE. INST.");
    map.insert(operario,"OPERARIO");
    map.insert(ANOMALIA,"TAREA A REALIZAR");
    map.insert(emplazamiento,"EMPLAZAMIENTO");
    map.insert(observaciones,"OBSERVACIONES");
    map.insert(actividad,"ACTIVIDAD");
    map.insert(nombre_cliente,"TITULAR");
    map.insert(numero_abonado,"NºABONADO");
    map.insert(telefono1,"TELEFONO");
    map.insert(acceso,"ACCESO");
    map.insert(resultado,"RESULTADO");
    map.insert(nuevo_citas,"NUEVO");
    map.insert(fecha_instalacion,"FECHA");
    map.insert(zona,"ZONAS");
    map.insert(ruta,"RUTA");
    map.insert(marca_contador,"MARCA");
    map.insert(codigo_de_geolocalizacion,"CÓDIGO DE LOCALIZACIÓN");
    map.insert(url_geolocalizacion, "GEOLOCALIZACIÓN");
    map.insert(lectura_actual,"ÚLTIMA LECTURA");
    map.insert(GESTOR,"GESTOR");

    mapa_tipos_tareas.insert("NUEVO CONTADOR INSTALAR","NCI");
    mapa_tipos_tareas.insert("USADO CONTADOR INSTALAR","U");
    mapa_tipos_tareas.insert("LIMPIEZA DE FILTRO Y TOMA DE DATOS","LFTD");
    mapa_tipos_tareas.insert("TOMA DE DATOS","TD");
    mapa_tipos_tareas.insert("BAJA O CORTE DE SUMINISTRO","TBND");
    mapa_tipos_tareas.insert("SOLO INSTALAR","SI");
    mapa_tipos_tareas.insert("INSPECCIÓN","I");
    mapa_tipos_tareas.insert("COMPROBAR EMISOR","CF");
    mapa_tipos_tareas.insert("EMISOR LECTURA","EL");

}

void screen_tabla_tareas::fixModelForTable(QJsonArray jsonArray)
{
    disconnect(this,SIGNAL(TasksFilled(database_comunication::serverRequestType)),this,SLOT(populateTable(database_comunication::serverRequestType)));
    jsonArraySelected = 1;

    if(jsonArray.isEmpty()){
        return;
    }
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QMap <QString,QString> mapa, mapa_alt;

    QString fecha = "";

    fecha = "Fech.Import";
    mapa.insert(fecha,FechImportacion);

    mapa.insert("Id.Ord",idOrdenCABB);
    mapa.insert("Causa Origen",ANOMALIA);
    mapa.insert("Prefijo",CONTADOR_Prefijo_anno);
    mapa.insert("Marca",marca_contador);
    mapa.insert("Nº Serie",numero_serie_contador);
    mapa.insert("Calibre",calibre_toma);
    mapa.insert("Actividad",actividad);
    mapa.insert("Emplaza.",emplazamiento);
    mapa.insert("Acceso",acceso);
    mapa.insert("Calle",calle);
    mapa.insert("Portal",numero);
    mapa.insert("BIS",BIS);
    mapa.insert("Piso",piso);
    mapa.insert("Mano",mano);
    mapa.insert("Población",poblacion);
    mapa.insert("Nombre",nombre_cliente);
    mapa.insert("Nº Abonado",numero_abonado);
    mapa.insert("Ruta",ruta);
    mapa.insert("Lectura",lectura_ultima);
    mapa.insert("Observaciones",observaciones);

    mapa_alt.insert("Id.Ord",idOrdenCABB);
    mapa_alt.insert("Causa Origen",ANOMALIA);
    mapa_alt.insert("Prefijo",CONTADOR_Prefijo_anno);
    mapa_alt.insert("Marca",marca_contador);
    mapa_alt.insert("Nº Serie",numero_serie_contador);
    mapa_alt.insert("Calibre",calibre_toma);
    mapa_alt.insert("Actividad",actividad);
    mapa_alt.insert("Emplaza.",emplazamiento);
    mapa_alt.insert("Acceso",acceso);
    mapa_alt.insert("Calle",calle);
    mapa_alt.insert("Portal",numero);
    mapa_alt.insert("BIS",BIS);
    mapa_alt.insert("Piso",piso);
    mapa_alt.insert("Mano",mano);
    mapa_alt.insert("Población",poblacion);
    mapa_alt.insert("Nombre",nombre_cliente);
    mapa_alt.insert("Nº Abonado",numero_abonado);
    mapa_alt.insert("Ruta",ruta);
    mapa_alt.insert("C.Emplazamiento",codigo_de_geolocalizacion);
    mapa_alt.insert("Sector P",zona);
    mapa_alt.insert("Lectura",lectura_actual);
    mapa_alt.insert("Observaciones",observaciones);


    QStringList listHeaders;
    //    for(int i=0; i< mapa.keys().size(); i++){
    //        listHeaders<<mapa.keys().at(i);
    //    }
    listHeaders << "Id.Ord" << fecha << "Causa Origen" << "Prefijo"<<"Nº Serie"<< "Marca"
                <<"Calibre"<< "Actividad"<<"Emplaza." << "Acceso" << "Calle"<< "Portal" << "BIS"
               << "Piso" << "Mano" << "Población"<< "Nombre"  << "Nº Abonado" << "Ruta"
               << "C.Emplazamiento" << "Sector P" << "Lectura" << "Observaciones";

    model = new QStandardItemModel(rows, listHeaders.size());
    model->setHorizontalHeaderLabels(listHeaders);

    //insertando los datos
    QStandardItem *item;
    for(int i = 0; i < rows; i++)
    {
        for (int n = 0; n < listHeaders.size(); n++) {
            item = new QStandardItem();
            QString column_value = jsonArray[i].toObject().value(mapa.value(listHeaders.at(n))).toString();
            if(!checkIfFieldIsValid(column_value)){
                column_value = jsonArray[i].toObject().value(mapa_alt.value(listHeaders.at(n))).toString();
            }
            item->setData(column_value, Qt::EditRole);
            model->setItem(i, n, item);
        }
    }
}
void screen_tabla_tareas::populateTable(/*QByteArray ba,*/ database_comunication::serverRequestType tipo)
{
    //    serverAnswer(ba,tipo);
    Q_UNUSED(tipo);
    fixModelForTable(jsonArrayAllTask);
    setTableView();
}
void screen_tabla_tareas::setTableView(){
    timer.stop();
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    QList<double> sizes;
    sizes << 0.5/*ordCabb*/ << 1.2 /*Fech Import*/<< 1/*CausaOrigen*/<<  1/*AÑO o PREFIJO*/ <<  1/*SERIE*/ << 1.8/* MARCA*/ <<  0.5/* CALIBRE*/
          <<  1.2/*ACTIVI */ <<  1.5/*EMPLAZA */ <<  1.5/*ACCESO */ <<  2/*CALLE */ <<  0.5/*NUME */ <<  0.5/* BIS*/ <<  0.5/* PISO*/
           <<  0.5/*MANO */ <<  1.2/*MUNICIPIO */ <<  2.5/*NOMBRE */ << 1/*ABONADO */ <<  0.75/*CODLEC */ << 2 /*observaciones*/;

    QFont font = ui->tableView->font();
    int pointSize = font.pointSize();
    float ratio = static_cast<float>(pointSize)/10;
    QRect rect = QGuiApplication::screens().first()->geometry();
    int width = ui->tableView->size().width();
    if(width >= rect.width()){
        width = rect.width();
    }
    int width_table = width - 40;
    float medium_width_fileds = static_cast<float>(width_table)/16/*fields_count_in_table*/;

    for (int i=0; i < sizes.size(); i++) {
        ui->tableView->setColumnWidth(i, static_cast<int>(medium_width_fileds)*sizes.at(i)*ratio);
    }
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setFont(ui->tableView->font());

}

void screen_tabla_tareas::insertTareasInSQLite(QJsonArray jsonArray){
    QStringList principals_variables;
    DBtareasController db(empresa + db_tareas_path);
    for (int i=0; i < jsonArray.size(); i++) {
        QJsonObject jsonObject = jsonArray.at(i).toObject();
        QString principal_varJsonArray = jsonObject.value(principal_variable).toString().trimmed();
        principals_variables << principal_varJsonArray;
        if(db.checkIfTareaExists(principal_varJsonArray)){
            QJsonObject oldJson = db.getTareas(principal_variable, principal_varJsonArray).first().toObject();
            if(compareJsonObjectByDateModified(oldJson, jsonObject)){
                db.updateTarea(jsonObject);
            }
        }else{
            db.insertTarea(jsonObject);
        }
    }
    QStringList princ_vars_in_sqlite = db.getOneColumnValues(principal_variable);
    QString pv;
    foreach(pv, princ_vars_in_sqlite){
        if(!principals_variables.contains(pv)){
            db.removeTarea(pv);
        }
    }
}

bool screen_tabla_tareas::compareJsonObjectByDateModified(QJsonObject jsonObject_old, QJsonObject jsonObject_new){ //devuelve true si el segundo es mas actualizado
    QString oldMod = jsonObject_old.value(date_time_modified).toString();
    QDateTime date_oldMod =  QDateTime::fromString(oldMod, formato_fecha_hora);
    QString newMod = jsonObject_new.value(date_time_modified).toString();
    QDateTime date_newMod =  QDateTime::fromString(newMod, formato_fecha_hora);
    if(date_newMod > date_oldMod){
        return true;
    }else {
        return false;
    }
}

void screen_tabla_tareas::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    //    sender()->deleteLater();
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::DELETE_TAREA)
    {
        if(byte_array.contains("suceess delete_task"))
        {
            result = database_comunication::script_result::ok;
            emit task_delete_excecution_result(result);
        }
        else
        {
            if(byte_array.contains("ot suceess delete_task"))
            {
                result = database_comunication::script_result::delete_task_failed;
                emit task_delete_excecution_result(result);
            }
        }
    }
    emit script_excecution_result(result);
}

void screen_tabla_tareas::on_tableView_doubleClicked(const QModelIndex &index)
{
    QJsonObject o = jsonArrayAllTask[index.row()].toObject();

    other_task_screen *oneTareaScreen = new other_task_screen(nullptr, false, false, empresa);

    connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
    emit sendData(o);
    disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));

    oneTareaScreen->populateView();
    QRect rect = QGuiApplication::screens().first()->geometry();
    if(rect.width() <= 1366
            && rect.height() <= 768){
        oneTareaScreen->showMaximized();
    }else {
        oneTareaScreen->show();
    }
}

QString screen_tabla_tareas::setDirToExplorer(){

    QDate date = QDate::currentDate();
    QString day, month, year;
    day = QString::number(date.day());
    if(date.day()<10){
        day = "0"+day;
    }
    month = map_months.value(date.month());
    year = QString::number(date.year());
    QDir dir;
    QString dir_string;
    if(selection_Order == "DIARIAS"){
        dir.setPath("C:/GecontaInstala/Carga/"+selection_Order+"/"+year+"/"+day+"_"+month+"_"+year);
        if(!dir.exists()){
            dir.setPath("C:/GecontaInstala/Carga/"+selection_Order+"/"+year+"/");
        }
    }else{
        dir.setPath("C:/GecontaInstala/Carga/"+selection_Order+"/"+year+"/");
    }

    if(dir.exists()){
        dir_string = dir.path();
    }else{
        dir_string = QDir::current().path();
    }
    qDebug()<<dir.path();

    return dir_string;
}

QJsonObject screen_tabla_tareas::get_JObject_from_JArray(QJsonArray jsonArray, QString princ_var){
    for (int i=0; i < jsonArray.size(); i++) {
        if(jsonArray.at(i).toObject().value(principal_variable).toString().trimmed()==princ_var){
            return jsonArray.at(i).toObject();
        }
    }
    return QJsonObject();
}

bool screen_tabla_tareas::isUpdateNeeded(QJsonObject jsonObject_viejo, QJsonObject jsonObject_nuevo){
    QString date_modified_viejo_string;
    QString date_modified_nuevo_string;
    QDateTime fecha_viejo = QDateTime::fromString(jsonObject_viejo.value(date_time_modified).toString().trimmed(), formato_fecha_hora);
    QDateTime fecha_nuevo = QDateTime::fromString(jsonObject_nuevo.value(date_time_modified).toString().trimmed(), formato_fecha_hora);
    if(fecha_nuevo > fecha_viejo){
        return true;
    }else{
        return false;
    }
}
void screen_tabla_tareas::setRutaFile(QString dir){
    rutaToDATFile = dir;
}
bool screen_tabla_tareas::on_pb_load_data_from_file_clicked()
{
    show_loading("Cargando Información...");
    loaded_from_file = true;

    QString dir_string = setDirToExplorer();

    if(file_type == 1)
    {
        rutaToDATFile = QFileDialog::getOpenFileName(this,"Seleccione el archivo .DAT", dir_string, "Datos (*.dat)");
    }
    if(file_type == 2)
    {
        rutaToDATFile = QFileDialog::getOpenFileName(this,"Seleccione el archivo .XLS", dir_string, "Datos (*.xlsx *.xls)");
    }
    if(file_type == 3)
    {
        if(rutaToDATFile == "buscar_fichero_en_PC"){
            rutaToDATFile = QFileDialog::getOpenFileName(this,"Seleccione el archivo .txt", dir_string, "Datos (*.txt)");
        }
    }
    if(file_type == 4)
    {
        if(rutaToDATFile == "buscar_fichero_en_PC"){
            rutaToDATFile = QFileDialog::getOpenFileName(this,"Seleccione el archivo .DAT", dir_string , "Datos (*.dat)");
        }
    }
    if(!rutaToDATFile.isNull() && !rutaToDATFile.isEmpty()){
        jsonArrayAllTask = parse_to_QjsonArray(rutaToDATFile);

        fixModelForTable(jsonArrayAllTask);
        setTableView();
        hide_loading();
        return true;
    }
    else{
        hide_loading();
        return false;
    }
}

QString screen_tabla_tareas::obtenerPrefijoDeSerie(QString serie){ //ok correcta
    QString prefij = "";
    if(serie.contains(" ")){
        QStringList list = serie.split(" ");
        if(list.length()>=2){
            prefij = list.at(0).trimmed();
        }
    }
    return  prefij;
}

QMap<QString, QString> screen_tabla_tareas::mapExcelImport(QStringList listHeaders){
    QMap<QString, QString> map;
    //campos de excel de entrada
    map.insert("ID.ORD",idOrdenCABB);
    map.insert("ID. ORD",idOrdenCABB);
    map.insert("ID. ORDEN",idOrdenCABB);
    map.insert("ID ORDEN",idOrdenCABB);
    map.insert("ORDEN",idOrdenCABB);


    map.insert("NUMIN",numero_interno);
    map.insert("NUMERO INTERNO",numero_interno);
    map.insert("NÚMERO INTERNO",numero_interno);

    map.insert("DIRECCION","DIRECCION");

    map.insert("MUNICIPIO",poblacion);
    map.insert("POBLACION",poblacion);
    map.insert("POBLACIÓN",poblacion);

    map.insert("ESC",propiedad);
    map.insert("ORD_RUT",reparacion);
    map.insert("CONT PADRE",ruta);

    map.insert("CALLE",calle);
    map.insert("DES_VIA",calle);
    map.insert("Nº",numero);
    map.insert("NUM",numero);
    map.insert("NUM_EDIF",numero);
    map.insert("NÚMERO",numero);
    map.insert("NUMERO",numero);
    map.insert("PORTAL",numero);
    map.insert("NÚMERO PORTAL",numero);
    map.insert("NUMERO PORTAL",numero);
    map.insert("NÚMERO DE PORTAL",numero);
    map.insert("NUMERO DE PORTAL",numero);
    map.insert("BIS",BIS);
    map.insert("BLOQUE",BIS);
    map.insert("PISO",piso);
    map.insert("MANO",mano);
    map.insert("PUERTA",mano);
    map.insert("LETRA",mano);
    map.insert("AÑO  O PREFIJO",CONTADOR_Prefijo_anno);
    map.insert("AÑO O PREFIJO",CONTADOR_Prefijo_anno);
    map.insert("AÑO",CONTADOR_Prefijo_anno);
    map.insert("PREFIJO",CONTADOR_Prefijo_anno);

    map.insert("CONTADOR",numero_serie_contador);
    map.insert("CONTADOR",numero_serie_contador);
    map.insert("NUM_CONT",numero_serie_contador);
    map.insert("Nº SERIE",numero_serie_contador);
    map.insert("NºSERIE",numero_serie_contador);
    map.insert("N SERIE",numero_serie_contador);
    map.insert("SERIE",numero_serie_contador);
    map.insert("NUMERO SERIE",numero_serie_contador);
    map.insert("NÚMERO SERIE",numero_serie_contador);
    map.insert("NUMERO SERIE DEL CONTADOR",numero_serie_contador);
    map.insert("NÚMERO SERIE DEL CONTADOR",numero_serie_contador);
    map.insert("Nº SERIE CONT. RETIRADO",numero_serie_contador);

    map.insert("CALIBRE",calibre_toma);
    map.insert("CALIBRE DE CONTADOR",calibre_toma);
    map.insert("CALIBRE CONTADOR",calibre_toma);
    map.insert("CALIBRE CONT. RETIRADO",calibre_toma);

    map.insert("CALIBRE. INST.",calibre_real);
    map.insert("OPERARIO",operario);

    map.insert("ANOMALÍA (TAREA A REALIZAR)",ANOMALIA);
    map.insert("CAUSA ORIGEN",ANOMALIA);
    map.insert("TAREA A REALIZAR",ANOMALIA);
    map.insert("ANOMALÍA",ANOMALIA);
    map.insert("ANOMALIA",ANOMALIA);

    map.insert("EMPLAZAMIENTO",emplazamiento);

    map.insert("COMENTARIOS",observaciones);
    map.insert("OBSERVACIONES",observaciones);

    map.insert("MENSAJE LIBRE", MENSAJE_LIBRE);
    //        map.insert("OBSERVACIONES",observaciones_devueltas); //aqui son las devueltas porque es trabajo realizado
    map.insert("ACTIVIDAD",actividad);

    map.insert("NOMBRE",nombre_cliente);
    map.insert("TITULAR",nombre_cliente);
    map.insert("APELLIDO 1",nombre_cliente);
    map.insert("APELLIDO 2",nombre_cliente);

    map.insert("REF",numero_abonado);
    map.insert("CONTRATO",numero_abonado);
    map.insert("ABONADO",numero_abonado);
    map.insert("NºABONADO",numero_abonado);
    map.insert("Nº ABONADO",numero_abonado);
    map.insert("N ABONADO",numero_abonado);
    map.insert("NUMERO ABONADO",numero_abonado);
    map.insert("NÚMERO ABONADO",numero_abonado);

    map.insert("TELEFONO",telefono1);
    map.insert("TELÉFONO",telefono1);

    if(listHeaders.contains("Ref", Qt::CaseInsensitive)){
        map.insert("UBICACION",acceso);
        map.insert("UBICACIÓN",acceso);
    }else{
        map.insert("UBICACION",emplazamiento);
        map.insert("UBICACIÓN",emplazamiento);
    }
    map.insert("ACCESO",acceso);
    map.insert("KOKAPENA",acceso);

    map.insert("RESULTADO",resultado);

    map.insert("NUEVO",nuevo_citas);
    map.insert("NUEVO CITAS",nuevo_citas);
    map.insert("CITA",nuevo_citas);
    map.insert("CITAS",nuevo_citas);

    map.insert("FECHA",fecha_instalacion);
    map.insert("ZONA",zona);
    map.insert("ZONAS",zona);
    map.insert("SECTOR",zona);
    map.insert("SECTOR P",zona);
    map.insert("DES_RUT",zona);

    map.insert("RUTA",ruta);

    map.insert("MARCA",marca_contador);
    map.insert("MARCA ACT",marca_contador);
    map.insert("MARCA CONTADOR",marca_contador);
    map.insert("MARCA DE CONTADOR",marca_contador);

    map.insert("CÓDIGO DE EMPLAZAMIENTO",codigo_de_geolocalizacion);
    map.insert("CODIGO DE EMPLAZAMIENTO",codigo_de_geolocalizacion);
    map.insert("CÓDIGO EMPLAZAMIENTO",codigo_de_geolocalizacion);
    map.insert("CODIGO EMPLAZAMIENTO",codigo_de_geolocalizacion);
    map.insert("C.EMPLAZAMIENTO",codigo_de_geolocalizacion);
    map.insert("CÓDIGO DE LOCALIZACIÓN",codigo_de_geolocalizacion);
    map.insert("CÓDIGO DE LOCALIZACION",codigo_de_geolocalizacion);
    map.insert("CODIGO DE LOCALIZACIÓN",codigo_de_geolocalizacion);
    map.insert("CODIGO DE LOCALIZACION",codigo_de_geolocalizacion);

    map.insert("GEOLOCALIZACIÓN",geolocalizacion);
    map.insert("GEOLOCALIZACION",geolocalizacion);
    map.insert("ÚLTIMA LECTURA",lectura_actual);
    map.insert("ULTIMA LECTURA",lectura_actual);
    map.insert("GESTOR",GESTOR);
    map.insert("LINK GEOLOCALIZACIÓN",url_geolocalizacion);
    map.insert("LINK GEOLOCALIZACION",url_geolocalizacion);
    map.insert("LECTURA DE CONTADOR INSTALADO",lectura_contador_nuevo);
    map.insert("Nº SERIE CONTADOR INSTALADO",numero_serie_contador_devuelto);
    map.insert("Nº ANTENA CONTADOR INSTALADO",numero_serie_modulo);
    map.insert("CALIBRE CONTADOR INSTALADO", calibre_real);
    map.insert("LONGITUD CONTADOR INSTALADO",largo_devuelto);
    map.insert("CLASE CONTADOR INSTALADO",  TIPO_devuelto);
    map.insert("MARCA CONTADOR INSTALADO", marca_devuelta);
    map.insert("UBICACIÓN BATERÍA", ubicacion_en_bateria);
    map.insert("UBICACIÓN EN BATERÍA", ubicacion_en_bateria);
    map.insert("UBICACION BATERÍA", ubicacion_en_bateria);
    map.insert("UBICACIÓN BATERIA", ubicacion_en_bateria);

    map.insert("FECINST", fecha_instalacion);
    map.insert("FECEMISIO", FECEMISIO);
    map.insert("F_INST",  F_INST);
    map.insert("FECH_CIERRE",FECH_CIERRE);
    map.insert("FECHIMPORTACION", FechImportacion);
    map.insert("FECH_CIERRENEW", fech_cierrenew);
    map.insert("FECH_FACTURACION", fech_facturacion);
    map.insert("FECH_INFORMACIONNEW", fech_informacionnew);
    map.insert("F_INSTNEW", f_instnew);

    map.insert("PROPIEDAD",propiedad);

    return map;
}

QJsonArray screen_tabla_tareas::parse_to_QjsonArray(QString path)
{
    QString current_datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    if(file_type == 1)
    {
        QStringList cods_emplazamiento, poblaciones;
        QFile file(path);
        file.open(QIODevice::ReadOnly);
        QStringList unparsed_rows, empty;
        QJsonArray jsonArray;
        char array [361];

        //reading rows
        while (!file.atEnd())
        {
            file.readLine(array, 361);
            QString line = QString::fromUtf8(array).trimmed();//QString(array);
            line.replace("�", "Ñ");
            if(!line.isEmpty()){
                unparsed_rows << line;
            }
        }
        //parsing rows
        for(int i=0; i<unparsed_rows.count(); i++)
        {
            QString Tipo_Tarea, calibre, anomalia;
            QJsonObject o;
            int pos=0;

            //A realizar------------------------------------------------------------------------------------------------------------
            o.insert(AREALIZAR, QJsonValue(unparsed_rows[i].mid(pos,12).trimmed()));
            pos+=12;
            //Intervención------------------------------------------------------------------------------------------------------------
            o.insert(INTERVENCION, QJsonValue(unparsed_rows[i].mid(pos,27).trimmed()));
            pos+=27;
            //Reparación (reparacion)------------------------------------------------------------------------------------------------------------
            o.insert(reparacion, QJsonValue(unparsed_rows[i].mid(pos,3).trimmed()));
            pos+=3;
            //Propiedad (propiedad)------------------------------------------------------------------------------------------------------------
            o.insert(propiedad, QJsonValue(unparsed_rows[i].mid(pos,1).trimmed()));
            pos+=1;
            //Contador (numero_serie_contador)------------------------------------------------------------------------------------------------------------
            QString serie_prefijo = QJsonValue(unparsed_rows[i].mid(pos,13).trimmed()).toString();
            QString prefijo = obtenerPrefijoDeSerie(serie_prefijo);
            QString serie_sin_prefijo = serie_prefijo;
            if(!prefijo.isEmpty() &&
                    (serie_sin_prefijo.mid(0, prefijo.size()) == prefijo)){
                serie_sin_prefijo = serie_sin_prefijo.remove(0, prefijo.size()).trimmed();
                bool allceros = true;
                for(int i=0; i < serie_sin_prefijo.size(); i ++){
                    if(serie_sin_prefijo.at(i) != "0"){//chequeando si es "0000..."
                        allceros = false;
                        break;
                    }
                }
                if(serie_sin_prefijo.at(0) == "0" && !allceros){ //si tiene un cero al principio lo elimino
                    serie_sin_prefijo.remove(0,1);
                }
                serie_prefijo = prefijo + serie_sin_prefijo;
            }

            o.insert(CONTADOR_Prefijo_anno, prefijo);
            o.insert(numero_serie_contador, serie_prefijo.trimmed()); //no remover prefijo y sin el cero al inicio
            pos+=13;

            //Marca (marca_contador)------------------------------------------------------------------------------------------------------------
            QString marca_l = QJsonValue(unparsed_rows[i].mid(pos,12).trimmed()).toString();
            o.insert(marca_contador, QJsonValue(unparsed_rows[i].mid(pos,12).trimmed()));
            pos+=12;
            //Calibre (calibre_toma)------------------------------------------------------------------------------------------------------------
            //        o.insert("calibre_toma", QJsonValue(unparsed_rows[i].mid(pos,4)));
            calibre = unparsed_rows[i].mid(pos,4).trimmed();
            pos+=4;
            //Ruedas (ruedas)------------------------------------------------------------------------------------------------------------
            o.insert(ruedas, QJsonValue(unparsed_rows[i].mid(pos,2).trimmed()));
            pos+=2;
            //Fecha (?)------------------------------------------------------------------------------------------------------------
            QString string = QJsonValue(unparsed_rows[i].mid(pos,10).trimmed()).toString();
            QDateTime date = QDateTime::fromString(string, "dd/MM/yyyy");
            if(date.isValid()){
                o.insert(fecha_instalacion, date.toString(formato_fecha_hora));
            }
            pos+=10;
            //Actividad (actividad)------------------------------------------------------------------------------------------------------------
            o.insert(actividad, QJsonValue(unparsed_rows[i].mid(pos,27).trimmed()));
            pos+=27;
            //Emplazamiento (emplazamiento)------------------------------------------------------------------------------------------------------------
            o.insert(emplazamiento, QJsonValue(unparsed_rows[i].mid(pos,22).trimmed()));
            pos+=22;
            //Acceso (acceso)------------------------------------------------------------------------------------------------------------
            QString access = QJsonValue(unparsed_rows[i].mid(pos,40).trimmed()).toString();
            o.insert(acceso, access);
            pos+=40;
            //Calle (calle)------------------------------------------------------------------------------------------------------------
            o.insert(calle, QJsonValue(unparsed_rows[i].mid(pos,20).trimmed()));
            pos+=20;
            //Número Portal (numero_portal)------------------------------------------------------------------------------------------------------------
            QString portal = unparsed_rows[i].mid(pos,3).trimmed();
            o.insert(numero, QJsonValue(portal));
            bool ok;
            int port_int = portal.toInt(&ok);
            if(ok){
                portal = QString::number(port_int);
            }
            pos+=3;
            //Bis (letra_edificio)------------------------------------------------------------------------------------------------------------
            o.insert(BIS, QJsonValue(unparsed_rows[i].mid(pos,3).trimmed()));
            pos+=3;
            //Piso (piso)------------------------------------------------------------------------------------------------------------
            o.insert(piso, QJsonValue(unparsed_rows[i].mid(pos,3).trimmed()));
            pos+=3;
            //Mano (mano)------------------------------------------------------------------------------------------------------------
            o.insert(mano, QJsonValue(unparsed_rows[i].mid(pos,3).trimmed()));
            pos+=3;
            //Municipio (poblacion)------------------------------------------------------------------------------------------------------------
            o.insert(poblacion, QJsonValue(unparsed_rows[i].mid(pos,12).trimmed()));
            pos+=12;
            //Nombre Abonado (nombre_cliente)------------------------------------------------------------------------------------------------------------
            o.insert(nombre_cliente, QJsonValue(unparsed_rows[i].mid(pos,60).trimmed()));
            pos+=60;
            //Número Abonado (numero_abonado)------------------------------------------------------------------------------------------------------------
            o.insert(numero_abonado, QJsonValue(unparsed_rows[i].mid(pos,9).trimmed()));
            pos+=9;
            //Código Ruta (ruta)------------------------------------------------------------------------------------------------------------
            QString ruta_l = unparsed_rows[i].mid(pos,8).trimmed();
            o.insert(ruta, QJsonValue(ruta_l));//(voy a cambiar CODLEC del CABB para la lectura, ellos lo utilizan para la ruta)
            pos+=8;
            //Fecha instalado (fecha importe)------------------------------------------------------------------------------------------------------------
            o.insert(FECEMISIO, QJsonValue(unparsed_rows[i].mid(pos,10).trimmed()));
            pos+=10;
            //Fecha generado parte (?)------------------------------------------------------------------------------------------------------------
            o.insert(FECULTREP, QJsonValue(unparsed_rows[i].mid(pos,10).trimmed()));
            pos+=10;
            //Anomalía (se completo tipo_tarea)------------------------------------------------------------------------------------------------------------
            anomalia = unparsed_rows[i].mid(pos,3).trimmed();
            pos+=3;
            if(!GlobalFunctions::checkIfFieldIsValid(anomalia)){
                anomalia = default_anomaly;
            }
            //Número Interno (numero_interno)------------------------------------------------------------------------------------------------------------
            o.insert(numero_interno, QJsonValue(unparsed_rows[i].mid(pos,12).trimmed()));
            pos+=12;
            //Observaciones (observaciones)------------------------------------------------------------------------------------------------------------
            QString obs = QJsonValue(unparsed_rows[i].mid(pos,30).trimmed()).toString();
            if(checkIfFieldIsValid(access)){
                obs += (" - " + access);
            }
            o.insert(observaciones, obs);//le sumo el acceso a las observaciones
            o.insert(OBSERVA, obs); //las que vienen del CABB
            pos+=30;

            //insertando Calibre (calibre_toma)
            QString temp = parse_calibre(anomalia,calibre);
            o.insert(calibre_toma, QJsonValue(temp));

            //insertando Calibre (calibre_toma)
            Tipo_Tarea = parse_tipo_tarea(anomalia, calibre, marca_l);
            o.insert(tipo_tarea, QJsonValue(Tipo_Tarea));

            //OJO QUITAR LUEGO SOLO PARA DEPURAR
            o.insert(ANOMALIA, QJsonValue(anomalia));
            QString intervencion = o.value(INTERVENCION).toString();
            o.insert(causa_origen, anomalia + " - " + intervencion);
            o.insert(accion_ordenada, Causa::getAccionOrdenadaFromCodeCausa(anomalia));

            o.insert(TIPORDEN, selection_Order);
            o.insert(FechImportacion, current_datetime);

            QString bis = o.value(BIS).toString().trimmed();
            if(!checkIfFieldIsValid(bis)){
                bis = "";
            }else{
                bis.prepend("-");
            }
            if(ok && checkIfFieldIsValid(ruta_l)){/////Revisar esto
                QString cod_emplazamiento = ruta_l + "." + portal + bis;
                cod_emplazamiento = ITAC::formatCodeEmplazamiento(cod_emplazamiento);
                o.insert(codigo_de_geolocalizacion, cod_emplazamiento);
                if(!cods_emplazamiento.contains(cod_emplazamiento)){
                    cods_emplazamiento << cod_emplazamiento;
                }
                QString pob = o.value(poblacion).toString().toUpper();
                if(!poblaciones.contains(pob)){
                    poblaciones << pob;
                }
            }
            o.insert(prioridad, "BAJA");
            QString gestor_l = GlobalFunctions::readGestorSelected();
            if(checkIfFieldIsValid(gestor_l)){
                o.insert(GESTOR, gestor_l);
            }else{

            }
            o = buscarTelefonosEnObservaciones(o);
            jsonArray.append(o);
        }
        file.close();

        cods_emplazamiento.append(poblaciones);
        GlobalFunctions gf(this, empresa);
        QStringList itacFields, rutasPoblacionesFields, rutasFields;
        itacFields<< codigo_itac_itacs << geolocalizacion_itacs;
        rutasFields << codigo_ruta_rutas << radio_portal_rutas << barrio_rutas << municipio_rutas;

        QJsonArray jsonArrayItacs = gf.getItacsFields(itacFields, codigo_itac_itacs, cods_emplazamiento);
        QJsonArray jsonArrayRutas = gf.getRutasFields(rutasFields, codigo_ruta_rutas, cods_emplazamiento);

        for (int i = 0; i < jsonArray.size(); i++) {
            QJsonObject jsonObjectTarea = jsonArray.at(i).toObject();
            QString cod_emplazamiento = jsonObjectTarea.value(codigo_de_geolocalizacion).toString();
            QString poblacion_l = jsonObjectTarea.value(poblacion).toString().trimmed().toUpper();

            for (int n = 0; n < jsonArrayItacs.size(); n++) {
                QJsonObject jsonObject = jsonArrayItacs.at(n).toObject();
                if(jsonObject.value(codigo_itac_itacs).toString()==cod_emplazamiento){
                    QString geoCode = jsonObject.value(geolocalizacion_itacs).toString();
                    if(checkIfFieldIsValid(geoCode)){
                        jsonObjectTarea.insert(geolocalizacion, geoCode);
                        jsonObjectTarea.insert(codigo_de_localizacion, geoCode);
                        jsonObjectTarea.insert(url_geolocalizacion, "https://maps.google.com/?q="+geoCode);

                        jsonArray.replace(i, jsonObjectTarea);
                        break;
                    }
                }
            }
            bool found = false;
            for (int n = 0; n < jsonArrayRutas.size(); n++) {
                QJsonObject jsonObjectRuta = jsonArrayRutas.at(n).toObject();
                if(cod_emplazamiento.contains("-")){
                    cod_emplazamiento=cod_emplazamiento.split("-").at(0).trimmed();
                }
                if(jsonObjectRuta.value(codigo_ruta_rutas).toString()==cod_emplazamiento){
                    QString radio_l = jsonObjectRuta.value(radio_portal_rutas).toString();
                    QString zona_l = jsonObjectRuta.value(barrio_rutas).toString();
                    if(checkIfFieldIsValid(zona_l)){
                        jsonObjectTarea.insert(zona, zona_l);
                    }
                    if(checkIfFieldIsValid(radio_l)){
                        jsonObjectTarea.insert(tipoRadio, radio_l);
                    }
                    jsonArray.replace(i, jsonObjectTarea);
                    found = true;
                    break;
                }
            }
            if(!found) {
                for (int n = 0; n < jsonArrayRutas.size(); n++) {
                    QJsonObject jsonObjectRuta = jsonArrayRutas.at(n).toObject();
                    if(jsonObjectRuta.value(codigo_ruta_rutas).toString().trimmed().toUpper()
                            == poblacion_l){
                        QString radio_l = jsonObjectRuta.value(radio_portal_rutas).toString();
                        QString zona_l = jsonObjectRuta.value(barrio_rutas).toString();
                        if(checkIfFieldIsValid(zona_l)){
                            jsonObjectTarea.insert(zona, zona_l);
                        }
                        if(checkIfFieldIsValid(radio_l)){
                            jsonObjectTarea.insert(tipoRadio, radio_l);
                        }
                        jsonArray.replace(i, jsonObjectTarea);
                        found = true;
                        break;
                    }
                }
            }
        }
        return jsonArray;
    }
    if(file_type == 2)
    {
        QStringList cods_emplazamiento, poblaciones;
        QXlsx::Document xlsx(path);
        QString sheetName="";
        QJsonArray jsonArray;
        foreach(sheetName, xlsx.sheetNames()){
            xlsx.selectSheet(sheetName);
            QStringList listHeaders, row_content;
            QList<QStringList> lista;
            int number_of_row = xlsx.dimension().lastRow();
            number_of_row = (number_of_row > 20000)? 20000: number_of_row; //Informaciona de cada tarea

            int number_of_column = xlsx.dimension().lastColumn();
            number_of_column = (number_of_column > 1000)? 1000: number_of_column; //Headers

            //read headers
            ///Limitar el numero de headers 26 es la cantidad de columnas actual del excel
            //        if(number_of_column > 36){
            //            number_of_column = 36;
            //        }
            for (int i=1; i<= number_of_column; i++)
            {
                if( xlsx.cellAt(1,i) != nullptr){
                    QVariant value = xlsx.cellAt(1,i)->value();
                    if(value!=0){
                        QString header = value.toString().trimmed();
                        if(checkIfFieldIsValid(header)){
                            listHeaders << header;
                        }
                    }
                }else{
                    break;
                }
            }
            number_of_column = listHeaders.size();
            QStringList fech_fields;
//            fech_fields << "FECINST" << "FECEMISIO" << "F_INST"
//                        << "FECH_CIERRE" << "FECHIMPORTACION"
//                        << "FECH_CIERRENEW" << "FECH_FACTURACION"
//                        << "FECH_INFORMACIONNEW" << "F_INSTNEW";

            QMap<QString, QString> map = mapExcelImport(listHeaders);
            //read data
            for(int i = 2; i <= number_of_row; i++)
            {
                row_content.clear();
                for(int j = 1; j <= number_of_column; j++)
                {
                    if(xlsx.cellAt(i,j) != nullptr)
                    {
                        row_content << xlsx.cellAt(i,j)->value().toString().trimmed();
                    }
                    else
                    {
                        row_content << "";
                    }
                }
                lista.insert(i-2, row_content);
            }
            int count_break = 0;
            //OJO debo hacer una comprobación de los encabezados
            //-2 por los encabezados y fila de filtros y -1 por la fila de resumen al final
            for(int i=0; i < number_of_row-2; i++)
            {
                QJsonObject o;
                QString latitud, longitud;
                row_content = lista.at(i);
                if(row_content.at(0).trimmed().isEmpty() && row_content.at(1).trimmed().isEmpty()){
                    count_break++;
                    if(count_break > 100){
                        break;
                    }
                    continue;
                }
                for(int j = 0; j < number_of_column; j++)
                {
                    QString header = listHeaders.at(j).toUpper().trimmed();
                    if(checkIfFieldIsValid(header) && map.keys().contains(header)){
                        if(header.contains("apellido", Qt::CaseInsensitive)){
                            QString value_header = map.value(header);
                            QString contenido_en_excel = row_content.at(j);
                            QString nom = o.value(nombre_cliente).toString();
                            if(!checkIfFieldIsValid(nom)){
                                nom = contenido_en_excel;
                            }else{
                                nom += " "+contenido_en_excel;
                            }
                            o.insert(nombre_cliente, nom);
                        }else{
                            QString value_header = map.value(header);
                            QString contenido_en_excel = row_content.at(j);
                            if(header.contains("NOMBRE", Qt::CaseInsensitive)){
                                QString nom = o.value(nombre_cliente).toString();
                                if(checkIfFieldIsValid(nom)){
                                    nom.prepend(contenido_en_excel + " ");
                                }else{
                                    nom = contenido_en_excel;
                                }
                                o.insert(value_header, nom);
                            }
                            else if(header.contains("direccion", Qt::CaseInsensitive)){
                                //Dirección/ Calle / número/ bis/ piso o local / mano
                                QStringList fields, split;
                                fields << calle << numero << BIS << piso << mano;
                                if(contenido_en_excel.contains(",")){
                                    split = contenido_en_excel.split(",");
                                    QString value;
                                    for(int i=0; i < split.size(); i++){
                                        if(fields.size() >= split.size()){
                                            o.insert(fields.at(i), split.at(i).trimmed());
                                        }
                                    }
                                }
                            }
//                            else if(fech_fields.contains(header)){
//                                QString value_header = map.value(header);
//                                QString contenido_en_excel = row_content.at(j).trimmed();
//                                QDateTime dt = QDateTime::fromString(contenido_en_excel, "d/m/yyyy");
//                                if(dt.isValid()){
//                                    contenido_en_excel = dt.toString(formato_fecha_hora);
//                                    o.insert(value_header, contenido_en_excel);
//                                }
//                            }
                            else if(header.contains("NUMIN", Qt::CaseInsensitive)){
                                QString value_header = map.value(header);
                                QString contenido_en_excel = row_content.at(j);
                                QString numin = contenido_en_excel.remove("NUMIN-").trimmed();
                                if(checkIfFieldIsValid(numin)){
                                    o.insert(numero_interno, numin);
                                }
                            }
                            else{
                                o.insert(value_header,QJsonValue(contenido_en_excel));
                            }
                        }
                    }
                    else if(checkIfFieldIsValid(header)){
                        if(header.contains("Latitud", Qt::CaseInsensitive)){
                            QString contenido_en_excel = row_content.at(j);
                            latitud = contenido_en_excel.trimmed().replace(",", ".");
                        }
                        if(header.contains("Longitud", Qt::CaseInsensitive)){
                            QString contenido_en_excel = row_content.at(j);
                            longitud = contenido_en_excel.trimmed().replace(",", ".");
                        }
                    }
                }
                o.insert(TIPORDEN, selection_Order);
                o.insert(FechImportacion, current_datetime);

                QString calibre = o.value(calibre_toma).toString().trimmed();
                QString anomalia = o.value(ANOMALIA).toString().trimmed();
                if(!GlobalFunctions::checkIfFieldIsValid(anomalia)){
                    anomalia = default_anomaly;
                    o.insert(ANOMALIA, anomalia);
                }
                QString marca_l = o.value(marca_contador).toString().trimmed();

                QString Tipo_Tarea = parse_tipo_tarea(anomalia, calibre, marca_l);
                o.insert(tipo_tarea, QJsonValue(Tipo_Tarea));

                QString intervencion = Causa::getIntervencionFromCodeCausa(anomalia);
                o.insert(causa_origen, anomalia + " - " + intervencion);
                o.insert(accion_ordenada, Causa::getAccionOrdenadaFromCodeCausa(anomalia));
                o.insert(AREALIZAR, Causa::getARealizarFromCodeCausa(anomalia));
                //Intervención------------------------------------------------------------------------------------------------------------
                o.insert(INTERVENCION, intervencion);

                QString cal = o.value(calibre_toma).toString().trimmed();
                QString longitud_desde_cal="";
                if(cal.contains("-") && cal.split("-").length()>=2){
                    longitud_desde_cal = cal.split("-").at(1).trimmed();
                    cal = cal.split("-").at(0).trimmed() + "-" + longitud_desde_cal.trimmed();
                }
                o.insert(calibre_toma, cal);
                o.insert(LARGO, longitud_desde_cal);

                QString tels = o.value(telefono1).toString().trimmed();
                QString tel2 = o.value(telefono2).toString().trimmed();
                if(tels.contains("-") && tels.split("-").length()>=2){
                    tel2 = tels.split("-").at(1).trimmed();
                    tels = tels.split("-").at(0).trimmed();
                }
                if(tels.contains(" ") && tels.split(" ").length()>=2){
                    tel2 = tels.split(" ").at(1).trimmed();
                    tels = tels.split(" ").at(0).trimmed();
                }
                o.insert(telefono1, tels);
                o.insert(telefono2, tel2);

                QString portal = o.value(numero).toString().trimmed();
                if(portal.size() < 3){
                    while(portal.size() < 3){
                        portal = "0" + portal;
                    }
                    o.insert(numero, portal);
                }

                o = buscarGeolocalizacion(o);
                o = buscarCitasEnObservaciones(o);
                o = buscarTelefonosEnObservaciones(o);

                QString cod_emplazamiento = o.value(codigo_de_geolocalizacion).toString();
                if(!checkIfFieldIsValid(cod_emplazamiento)){
                    QString bis = o.value(BIS).toString().trimmed();
                    if(!checkIfFieldIsValid(bis)){
                        bis = "";
                    }else{
                        bis.prepend("-");
                    }
                    QString portal = o.value(numero).toString().trimmed();
                    QString ruta_l = o.value(ruta).toString().trimmed();
                    bool ok;
                    int port_int = portal.toInt(&ok);
                    if(ok){
                        portal = QString::number(port_int);
                    }
                    if(ok && checkIfFieldIsValid(ruta_l)){
                        cod_emplazamiento = ruta_l + "." + portal + bis;
                        cod_emplazamiento = ITAC::formatCodeEmplazamiento(cod_emplazamiento);
                    }
                }
                if(checkIfFieldIsValid(cod_emplazamiento)){/////Revisar esto
                    cod_emplazamiento = ITAC::formatCodeEmplazamiento(cod_emplazamiento);
                    o.insert(codigo_de_geolocalizacion, cod_emplazamiento);
                    if(!poblaciones.contains(o.value(poblacion).toString().toUpper())){
                        poblaciones << o.value(poblacion).toString().toUpper();
                    }
                    if(!cods_emplazamiento.contains(cod_emplazamiento)){
                        cods_emplazamiento << cod_emplazamiento;
                    }
                }

                QString geocode = o.value(codigo_de_localizacion).toString();
                if(!checkIfFieldIsValid(geocode)){
                    if(checkIfFieldIsValid(latitud) && checkIfFieldIsValid(longitud)){
                        QString geocode = latitud + "," + longitud;
                        o.insert(codigo_de_localizacion, geocode);
                        o.insert(geolocalizacion, geocode);
                        o.insert(url_geolocalizacion, "https://maps.google.com/?q=" + geocode);
                    }
                }
                cod_emplazamiento = o.value(codigo_de_geolocalizacion).toString();
                if(!checkIfFieldIsValid(cod_emplazamiento)){
                    o.insert(codigo_de_geolocalizacion, o.value(numero_abonado).toString().trimmed());
                }

                o.remove("");
                if(!o.isEmpty() /*&& checkValidDirFields(o)*/){
                    jsonArray.append(o);
                }
            }
        }
        cods_emplazamiento.append(poblaciones);
        GlobalFunctions gf(this, empresa);
        QStringList itacFields, rutasPoblacionesFields, rutasFields;
        itacFields<< codigo_itac_itacs << geolocalizacion_itacs;
        rutasFields << codigo_ruta_rutas << radio_portal_rutas << barrio_rutas << municipio_rutas;

        QJsonArray jsonArrayItacs = gf.getItacsFields(itacFields, codigo_itac_itacs, cods_emplazamiento);
        QJsonArray jsonArrayRutas = gf.getRutasFields(rutasFields, codigo_ruta_rutas, cods_emplazamiento);

        for (int i = 0; i < jsonArray.size(); i++) {
            QJsonObject jsonObjectTarea = jsonArray.at(i).toObject();
            QString cod_emplazamiento = jsonObjectTarea.value(codigo_de_geolocalizacion).toString();
            QString poblacion_l = jsonObjectTarea.value(poblacion).toString().trimmed().toUpper();

            for (int n = 0; n < jsonArrayItacs.size(); n++) {
                QJsonObject jsonObject = jsonArrayItacs.at(n).toObject();
                if(jsonObject.value(codigo_itac_itacs).toString()==cod_emplazamiento){
                    QString geoCode = jsonObject.value(geolocalizacion_itacs).toString();
                    if(checkIfFieldIsValid(geoCode)){
                        jsonObjectTarea.insert(geolocalizacion, geoCode);
                        jsonObjectTarea.insert(codigo_de_localizacion, geoCode);
                        jsonObjectTarea.insert(url_geolocalizacion, "https://maps.google.com/?q="+geoCode);

                        jsonArray.replace(i, jsonObjectTarea);
                        break;
                    }
                }
            }
            bool found = false;
            for (int n = 0; n < jsonArrayRutas.size(); n++) {
                QJsonObject jsonObjectRuta = jsonArrayRutas.at(n).toObject();
                if(cod_emplazamiento.contains("-")){
                    cod_emplazamiento=cod_emplazamiento.split("-").at(0).trimmed();
                }
                if(jsonObjectRuta.value(codigo_ruta_rutas).toString()==cod_emplazamiento){
                    QString radio_l = jsonObjectRuta.value(radio_portal_rutas).toString();
                    QString zona_l = jsonObjectRuta.value(barrio_rutas).toString();
                    if(checkIfFieldIsValid(zona_l)){
                        jsonObjectTarea.insert(zona, zona_l);
                    }
                    if(checkIfFieldIsValid(radio_l)){
                        jsonObjectTarea.insert(tipoRadio, radio_l);
                    }
                    jsonArray.replace(i, jsonObjectTarea);
                    found = true;
                    break;
                }
            }
            if(!found) {
                for (int n = 0; n < jsonArrayRutas.size(); n++) {
                    QJsonObject jsonObjectRuta = jsonArrayRutas.at(n).toObject();
                    if(jsonObjectRuta.value(codigo_ruta_rutas).toString().trimmed().toUpper()
                            == poblacion_l){
                        QString radio_l = jsonObjectRuta.value(radio_portal_rutas).toString();
                        QString zona_l = jsonObjectRuta.value(barrio_rutas).toString();
                        if(checkIfFieldIsValid(zona_l)){
                            jsonObjectTarea.insert(zona, zona_l);
                        }
                        if(checkIfFieldIsValid(radio_l)){
                            jsonObjectTarea.insert(tipoRadio, radio_l);
                        }
                        jsonArray.replace(i, jsonObjectTarea);
                        found = true;
                        break;
                    }
                }
            }
        }
        return jsonArray;
    }
    if(file_type == 3){
        QFile input(path);
        QByteArray data_json;
        QJsonArray jsonArray;
        if(input.open(QIODevice::ReadOnly | QIODevice::Text)){
            data_json = input.readAll();
            jsonArray = database_comunication::getJsonArray(data_json);
            input.close();
            return jsonArray;
        }
    }
    if(file_type == 4){
        QFile input(path);
        QJsonArray jsonArray;
        if(input.open(QIODevice::ReadOnly)){
            QDataStream in(&input);
            in>>jsonArray;
            input.close();
            return jsonArray;
        }
    }
    return QJsonArray();
}

bool screen_tabla_tareas::checkValidDirFields(QJsonObject jsonObject){
    if(!checkIfFieldIsValid(jsonObject.value(poblacion).toString())
            && !checkIfFieldIsValid(jsonObject.value(calle).toString())
            && !checkIfFieldIsValid(jsonObject.value(numero).toString())){
        return false;
    }
    return true;
}
bool screen_tabla_tareas::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

QJsonObject screen_tabla_tareas::buscarGeolocalizacion(QJsonObject jsonObject){
    QString geo = jsonObject.value(geolocalizacion).toString().trimmed();
    if(checkIfFieldIsValid(geo)){
        geo.replace("(", "");
        geo.replace(")", "");
        jsonObject.insert(geolocalizacion, geo);
        QString url = "https://maps.google.com/?q=" + geo;
        jsonObject.insert(url_geolocalizacion, url);
    }
    return jsonObject;
}
QJsonObject screen_tabla_tareas::buscarTelefonosEnObservaciones(QJsonObject jsonObject){
    QString observaciones_string = nullptr;
    try {
        observaciones_string = jsonObject.value(observaciones).toString().trimmed();

        QString tels = observaciones_string;
        QString tel2 ="", tel1 = "";
        QString split1, split2;
        if(tels.contains("-") && tels.split("-").length()>=2){
            split2 = tels.split("-").at(1).trimmed();
            split1 = tels.split("-").at(0).trimmed();
            if(checkIfAllNumbers(split2)){
                tel2 = split2;
            }
            if(checkIfAllNumbers(split1)){
                tel1 = split1;
            }
        }
        else if(tels.contains(" ") && tels.split(" ").length()>=2){
            split2 = tels.split(" ").at(1).trimmed();
            split1 = tels.split(" ").at(0).trimmed();
            if(checkIfAllNumbers(split2)){
                tel2 = split2;
            }
            if(checkIfAllNumbers(split1)){
                tel1 = split1;
            }
        }
        jsonObject.insert(telefono1, tel1);
        jsonObject.insert(telefono2, tel2);
        if(checkIfFieldIsValid(observaciones_string)){
            QString telefono;
            bool first_digit = false;
            for (int i=0; i < observaciones_string.size(); i++) {
                if(observaciones_string.at(i).isNumber()){
                    first_digit= true;
                    telefono.append(observaciones_string.at(i));
                }
                else{
                    if(first_digit){
                        break;
                    }
                }
            }
            if(telefono.size() > 4){
                jsonObject.insert(telefono1, telefono);
            }
        }
    } catch (QException e) {
    }
    return jsonObject;
}
QJsonObject screen_tabla_tareas::buscarCitasEnObservaciones(QJsonObject jsonObject){
    QString observaciones_string = nullptr;
    try {
        observaciones_string = jsonObject.value(observaciones).toString().trimmed().toUpper();
        if(!observaciones_string.isEmpty() && observaciones_string.contains("CITA A LAS ")){
            QString hora_string = observaciones_string.replace("CITA A LAS ","");
            jsonObject = setHora(jsonObject, hora_string, "A las ", false, "");

        }else if(!observaciones_string.isEmpty() && observaciones_string.contains("CITA ENTRE LAS ")){
            QString horas_string = observaciones_string.replace("CITA ENTRE LAS ","");
            QString hora_string = horas_string.split(" Y LAS ")[0].trimmed();
            QString hora2_string = horas_string.split(" Y LAS ")[1].trimmed();
            jsonObject = setHora(jsonObject, hora_string, "Entre las ", true, hora2_string);
        }
        else{
            if(!observaciones_string.isEmpty() && observaciones_string.contains("CITA")
                    && observaciones_string.contains(" A LAS ")) {
                QString fecha_string = observaciones_string.split(" A LAS ")[0].replace("CITA ","").trimmed();
                QString hora_string = observaciones_string.split(" A LAS ")[1].trimmed();
                jsonObject = setFechaHora(jsonObject, fecha_string, hora_string, "A las ", false, "");

            }
            else if(!observaciones_string.isEmpty() && observaciones_string.contains("CITA")
                    && observaciones_string.contains(" ENTRE LAS ") && observaciones_string.contains(" Y LAS ")){
                QString fecha_string = observaciones_string.split(" ENTRE LAS ")[0].replace("CITA ","").trimmed();
                QString horas_string = observaciones_string.split(" ENTRE LAS ")[1].trimmed();
                QString hora_string = horas_string.split(" Y LAS ")[0].trimmed();
                QString hora2_string = horas_string.split(" Y LAS ")[1].trimmed();

                jsonObject = setFechaHora(jsonObject, fecha_string, hora_string, "Entre las ", true, hora2_string);
            }
        }
    } catch (QException e) {
    }
    return jsonObject;
}
QString screen_tabla_tareas::get_current_date_in_format(QDate date, QTime time, QTime time_end)
{
    QString day_week = date.toString("ddd");
    day_week = map_days_week.value(day_week);
    QString day = QString::number(date.day());
    QString month = map_months.value(date.month());
    QString year = QString::number(date.year());

    int h = time.hour();
    QString hour = QString::number(h);
    if(h < 10){
        hour = "0"+hour;
    }
    int t = time.minute();
    QString minutes = QString::number(t);
    if(t < 10){
        minutes = "0"+minutes;
    }
    QString time_string = hour + ":"+minutes;


    h = time_end.hour();
    QString hour_end = QString::number(h);
    if(hour_end.toInt() < 10){
        hour_end = "0"+hour_end;
    }
    t = time_end.minute();
    QString minutes_end = QString::number(t);
    if( t < 10){
        minutes_end = "0"+minutes_end;
    }
    QString time_string_end = hour_end + ":"+minutes_end;

    return day_week + ", " + day + " de "+ month +" de "+ year +"\n"+"Entre las "+ time_string + " y las "+ time_string_end;
}
QJsonObject screen_tabla_tareas::setHora(QJsonObject jsonObject , QString hora_string, QString string_before_time,
                                         bool second_hour, QString hora_second){
    Q_UNUSED(string_before_time);
    QTime time = QTime::currentTime();
    QTime time_temp;
    try {
        time_temp = QTime::fromString(hora_string,"hh:mm");
        if(!time_temp.isValid()){
            time_temp = QTime::fromString(hora_string, "h:mm");
        }
        time.setHMS(time_temp.hour(), time_temp.minute(), 0);
        if(second_hour){
            time_temp = QTime::fromString(hora_second,"hh:mm");
            if(!time_temp.isValid()){
                time_temp = QTime::fromString(hora_second, "h:mm");
            }
            time_temp.setHMS(time_temp.hour(), time_temp.minute(), 0);
        }
        QDate currentDate = QDate::currentDate();
        QString lnuevo_citas;
        if(second_hour){
            lnuevo_citas = get_current_date_in_format(currentDate, time, time_temp);
        }else{
            lnuevo_citas = get_current_date_in_format(currentDate, time, time);
        }
        jsonObject.insert(status_tarea, "IDLE CITA");
        jsonObject.insert(nuevo_citas, lnuevo_citas);
        QString date_formated_string = QDateTime(QDate::currentDate(), time).toString(formato_fecha_hora);
        jsonObject.insert(fecha_hora_cita, date_formated_string);
    }catch (QException e) {

    }
    return  jsonObject;
}
QJsonObject screen_tabla_tareas::setFechaHora(QJsonObject jsonObject, QString fecha_string, QString hora_string,
                                              QString string_before_time, bool second_hour, QString hora_second){

    Q_UNUSED(string_before_time);
    QString s = fecha_string+"/"+ QString::number(QDate::currentDate().year());  s.replace("/","-");
    QDate d = QDate::fromString(s, "d-M-yyyy");
    if(!d.isValid()){
        d = QDate::fromString(s, "dd-MM-yyyy");
    }
    QTime t = QTime::fromString(hora_string, "hh:mm");
    if(!t.isValid()){
        t = QTime::fromString(hora_string, "h:mm");
    }
    QDateTime date = QDateTime(d, t);
    QTime time_2;
    if(second_hour){
        time_2 = QTime::fromString(hora_second, "hh:mm");
        if(!time_2.isValid()){
            time_2 = QTime::fromString(hora_second, "h:mm");
        }
    }
    try {
        QString lnuevo_citas;
        if(second_hour){
            lnuevo_citas = get_current_date_in_format(date.date(), date.time(), time_2);
        }else{
            lnuevo_citas = get_current_date_in_format(date.date(), date.time(), date.time());
        }
        jsonObject.insert(status_tarea, "IDLE CITA");
        jsonObject.insert(nuevo_citas,lnuevo_citas);
        QString date_formated_string ="";
        date_formated_string = date.toString(formato_fecha_hora);
        jsonObject.insert(fecha_hora_cita, date_formated_string);

    } catch (QException e) {
    }
    return  jsonObject;
}


QString screen_tabla_tareas::fill_with_spaces(QString str,int size)
{
    QString temp = str;
    if(str.size() >= size)
        temp.truncate(size);
    else
    {
        while(temp.size() < size)
            temp += " ";
    }
    return temp;
}

QString screen_tabla_tareas::parse_calibre(QString anomalia, QString calibre)
{
    //anomalias codfigo TD, CF y Baja corte suministro devolver solo calibre
    if(anomalia == "027" || anomalia == "A32" || anomalia == "C32" ||
            anomalia == "X12" || anomalia == "X13" || anomalia == "X14" || anomalia == "X23" ||
            anomalia == "035" || anomalia == "036" || anomalia == "037" || anomalia == "038" || anomalia == "039")
        return calibre;
    else
    {
        //comprobar resto de codigos
        if(anomalia == "001" || anomalia == "002" || anomalia == "004" || anomalia == "005" ||
                anomalia == "008" || anomalia == "009" || anomalia == "021" || anomalia == "024" ||
                anomalia == "025" || anomalia == "026" || anomalia == "034" || anomalia == "A32" ||
                anomalia == "A33" || anomalia == "C33" || anomalia == "CVF" || anomalia == "APC" ||
                anomalia == "NZ2" /*|| anomalia desconocida codigo R ||*/   || anomalia == "023" ||
                anomalia == "A31" || anomalia == "010" || anomalia == "023" || anomalia == "A30" ||
                anomalia == "S01" || anomalia == "Z23"  || anomalia == "Z21")
            return calibre;
        else
            //return "NULL";
            return calibre;
    }
}

QString screen_tabla_tareas::parse_tipo_tarea(QString anomalia, QString calibre, QString marca)
{
    if(!checkIfFieldIsValid(calibre)){
        calibre = "?";
    }
    if(!checkIfFieldIsValid(anomalia)){
        return "NCI " + calibre + "mm";
    }
    QString tipo_t = Causa::getTipoTareaFromCodeCausa(anomalia).trimmed();
    calibre = calibre.trimmed();
    //    bool cal_ok;
    //    int calibre_int = calibre.trimmed().toInt(&cal_ok);

    if(tipo_t == "NCI")
    {
        return "NCI " + calibre + "mm";
    }
    else if(tipo_t == "NCI + RC")
    {
        return "NCI " + calibre + "mm " + "+ RC";
    }
    else if(tipo_t == "ITAC")
    {
        return "ITAC";
    }
    else if(tipo_t == "LFTD")
    {
        return "LFTD " + calibre + "mm";
    }
    else if(tipo_t == "S.I.")
    {
        return "S.I. " + calibre + "mm";
    }
    else if(tipo_t == "U")
    {
        return "U " + calibre + "mm";
    }
    else if(tipo_t == "TD")
    {
        if(marca.contains("Sappel", Qt::CaseInsensitive)){
            return "TD";
        }
        return "TD/NCI " + calibre + "mm";
    }
    else if(tipo_t == "I+")
    {
        return "I+";
    }
    else if(tipo_t == "I+/NCI")
    {
        if(marca.contains("Sappel", Qt::CaseInsensitive)){
            return "I+";
        }
        return "I+/NCI " + calibre + "mm";
    }
    else if(tipo_t == "LECT.R o RC")
    {
        return "LECT.R o RC";
    }
    else if(tipo_t == "RC")
    {
        return "RC";
    }
    else if(tipo_t == "RC NCI")
    {
        return "RC NCI " + calibre + "mm";
    }
    else if(tipo_t == "RCN")
    {
        return "RCN";
    }
    else if(tipo_t == "C. H. W4")
    {
        return "C. H. W4";
    }
    else if(tipo_t == "T"){
        QString pre;
        if(calibre == "15")
            pre = "T3/4\" ";

        else if(calibre == "13")
            pre =  "T7/8\" ";

        else if(calibre == "20")
            pre =  "T1\" ";

        else if(calibre == "25")
            pre =  "T11/4\" ";

        else if(calibre == "30")
            pre =  "T11/2\" ";

        else if(calibre == "40")
            pre =  "T2\" ";

        else{
            pre =  "TBDN ";
        }
        return pre + calibre + "mm";
    }
    else if(tipo_t == "T + NCI"){
        QString pre;
        if(calibre == "15")
            pre = "T3/4\" ";

        else if(calibre == "13")
            pre =  "T7/8\" ";

        else if(calibre == "20")
            pre =  "T1\" ";

        else if(calibre == "25")
            pre =  "T11/4\" ";

        else if(calibre == "30")
            pre =  "T11/2\" ";

        else if(calibre == "40")
            pre =  "T2\" ";

        else{
            pre =  "TBDN ";
        }
        return pre + "+ NCI " + calibre + "mm";
    }
    return tipo_t + " " + calibre + "mm";
}

QJsonObject screen_tabla_tareas::buscarTelefonosEnObservaciones_excel(QJsonObject jsonObject){
    QString observaciones_string = nullptr;
    //        Log.e("Ejecutando", "buscarTelefonosEnObservaciones");
    try {
        observaciones_string = jsonObject.value(observaciones).toString().trimmed().replace(" ","");
        if(!observaciones_string.isEmpty() && observaciones_string.contains("-") && observaciones_string.split("-").size() >= 2){
            QString mtelefono1 = observaciones_string.split("-")[0].replace("T", "");
            QString mtelefono2 = observaciones_string.split("-")[1].replace("T", "");

            if(mtelefono1.size() > 3 && checkIfAllNumbers(mtelefono1)) {
                jsonObject.insert(telefono1, mtelefono1);
            }
            if(mtelefono2.size() > 3 && checkIfAllNumbers(mtelefono2)) {
                jsonObject.insert(telefono2, mtelefono2);
            }
        }
        else{
            if(!observaciones_string.isEmpty()) {
                QString mtelefono1 = observaciones_string.replace("T", "");
                if (checkIfAllNumbers(mtelefono1) && mtelefono1.size() > 3) {
                    jsonObject.insert(telefono1, mtelefono1);
                }
            }
        }
    } catch (QException e) {

    }
    return jsonObject;
}
bool screen_tabla_tareas::checkIfAllNumbers(QString string){
    for (int i=0;i<string.length();i++) {
        if(!string.at(i).isDigit()){
            return false;
        }
    }
    return true;
}
QJsonObject screen_tabla_tareas::checkIfDuplicateByInformation(QJsonObject o){
    QJsonObject jsonToSearch;
    jsonToSearch.insert(poblacion, o.value(poblacion).toString().trimmed());
    jsonToSearch.insert(calle, o.value(calle).toString().trimmed());
    jsonToSearch.insert(numero, o.value(numero).toString().trimmed());
    jsonToSearch.insert(BIS, o.value(BIS).toString().trimmed());
    jsonToSearch.insert(piso, o.value(piso).toString().trimmed());
    jsonToSearch.insert(mano, o.value(mano).toString().trimmed());
    jsonToSearch.insert(numero_abonado, o.value(numero_abonado).toString().trimmed());
    jsonToSearch.insert(tipo_tarea, o.value(tipo_tarea).toString().trimmed());
    jsonToSearch.insert(calibre_toma, o.value(calibre_toma).toString().trimmed());
    jsonToSearch.insert(numero_serie_contador, o.value(numero_serie_contador).toString().trimmed());
    GlobalFunctions gf(this, empresa);
    QJsonObject jsonObjectFound = gf.getTareaFromServer(jsonToSearch);
    if(!jsonObjectFound.isEmpty()){
        return jsonObjectFound;
    }
    else{
        return QJsonObject();
    }
}
void screen_tabla_tareas::on_pb_load_all_task_to_server_clicked()
{
    ui->pb_load_all_task_to_server->setEnabled(false);

    show_loading("Espere, subiendo tareas...");
    QStringList alredyOnDatabase, lista_principal_variables;
    QJsonObject o;

    //    /*getAllInternalNumbers*/();


    other_task_screen *oneTareaScreen = new other_task_screen(nullptr, false, false, empresa);

    connect(oneTareaScreen, SIGNAL(script_excecution_result(int)), this, SLOT(update_script_execution_result(int)));
    bool duplicated = false;
    int total = jsonArrayAllTask.size();

    GlobalFunctions gf(this, empresa);
    lista_principal_variables = gf.getTareasList();

    QStringList lista_no_subidas;
    for(int i=0, reintentos = 0;i<jsonArrayAllTask.size();i++)
    {
        o = jsonArrayAllTask[i].toObject();
        QJsonObject jsonObjectFound = checkIfDuplicateByInformation(o);
        if(!jsonObjectFound.isEmpty()){
            duplicated = true;
            if(file_type!= 3 && file_type!= 4){
                continue;//Si no es de excel o dat no subo ni actualizo tarea
            }else{
                o.insert(principal_variable, jsonObjectFound.value(principal_variable).toString());
            }
        }
        setLoadingText("Espere, subiendo tareas... ("
                       +QString::number(i)+"/"+QString::number(total)+")");

        QString princ_var = o.value(principal_variable).toString();
        if(file_type == 3 || !lista_principal_variables.contains(princ_var))
        {
            QJsonObject jsonObject_viejo = get_JObject_from_JArray(jsonArrayAllTaskinServer, princ_var);
            if(!jsonObject_viejo.isEmpty()){
                if(!ui->cb_overwrite_tasks->isChecked()){//si esta checkeada sobrescritura actualizo
                    if(!isUpdateNeeded(jsonObject_viejo, o)){ //si no necesita actualizar ignoro la tarea
                        continue;
                    }
                }
            }
            oneTareaScreen->setShowMesageBox(false);
            connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
            emit sendData(o);
            disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
            oneTareaScreen->populateView(false);
            QEventLoop *q = new QEventLoop();
            connect(oneTareaScreen, &other_task_screen::task_upload_excecution_result,q,&QEventLoop::exit);
            oneTareaScreen->setShowMesageBox(false);

            QTimer::singleShot(DELAY,oneTareaScreen,&other_task_screen::on_pb_update_server_info_clicked);

            switch(q->exec())
            {
            case database_comunication::script_result::timeout:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    i++;
                    QString princ_var = jsonArrayAllTask.at(i).toObject().value(principal_variable).toString();
                    lista_no_subidas << princ_var;
                    GlobalFunctions::showWarning(this,"Error subiendo tarea","No se pudo completar la solucitud de la subida"
                                                                             " de la tarea: NUMERO INTERNO -> "+princ_var);
                    GlobalFunctions gf(this);
                    if(gf.showQuestion(this, "Confirmación", "¿Desea continuar la subida de tareas?")
                            == QMessageBox::Ok){
                    }else{
                        return;
                    }
                }
                break;
            case database_comunication::script_result::task_to_server_ok:
                reintentos = 0;
                break;
            case database_comunication::script_result::update_task_to_server_failed:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    i++;
                    QString princ_var = jsonArrayAllTask.at(i).toObject().value(principal_variable).toString();
                    lista_no_subidas << princ_var;
                    GlobalFunctions::showWarning(this,"Error subiendo tarea","No se pudo completar la solucitud de la subida"
                                                                             " de la tarea: NUMERO INTERNO -> "+princ_var);
                    GlobalFunctions gf(this);
                    if(gf.showQuestion(this, "Confirmación", "¿Desea continuar la subida de tareas?")
                            == QMessageBox::Ok){
                    }else{
                        return;
                    }
                }
                break;
            case database_comunication::script_result::create_task_to_server_failed:
                i--;
                reintentos++;
                if(reintentos == RETRIES)
                {
                    i++;
                    QString princ_var = jsonArrayAllTask.at(i).toObject().value(principal_variable).toString();
                    lista_no_subidas << princ_var;
                    GlobalFunctions::showWarning(this,"Error subiendo tarea","No se pudo completar la solucitud de la subida"
                                                                             " de la tarea: NUMERO INTERNO -> "+princ_var);
                    GlobalFunctions gf(this);
                    if(gf.showQuestion(this, "Confirmación", "¿Desea continuar la subida de tareas?")
                            == QMessageBox::Ok){
                    }else{
                        return;
                    }
                }
                break;
            }
            delete q;
            oneTareaScreen->clear_all_pictures();

        }
        else
        {
            alredyOnDatabase << o.value(principal_variable).toString();
        }
    }
    if(!lista_no_subidas.isEmpty())
    {
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Tareas no subidas","Los siguientes NÚMEROS INTERNOS no se subieron al servidor.\n" + lista_no_subidas.join(" , "));
    }
    if(!alredyOnDatabase.isEmpty())
    {
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Variable principal replicados","Los siguientes variables principales no se cargaron porque ya se encontraban en el servidor.\n" + alredyOnDatabase.join(" , "));
    }
    if(duplicated && !ui->cb_overwrite_tasks->isChecked())
    {
        QMessageBox::information(this,"Información replicada","Alguna tareas no fueron subidas porque ya se encontraban en el servidor.");
    }
    //    getTareasFromServer();
    hide_loading();
    ui->pb_load_all_task_to_server->setEnabled(true);
    if(other_task_screen::conexion_activa){
        GlobalFunctions::showMessage(this,"Éxito","Se han subido "+
                                     QString::number(total)+" tareas en el servidor satisfactoriamente.");
    }else {
        GlobalFunctions::showMessage(this,"Éxito","Información actualizada en respaldo local satisfactoriamente.");
    }
    //    getAllInternalNumbers();
    disconnect(oneTareaScreen, SIGNAL(script_excecution_result(int)), this, SLOT(update_script_execution_result(int)));
    if(!rutaToDATFile.isEmpty()){
        QString path = other_task_screen::buscarCarpetaDeTrabajoPendiente();
        QString filename = rutaToDATFile;
        filename.remove(path);
        QStringList trabajo_cargado = other_task_screen::readTrabajosCargados();
        if(!trabajo_cargado.contains(filename)){
            trabajo_cargado.append(filename);
            other_task_screen::writeTrabajosCargados(trabajo_cargado);
        }
        rutaToDATFile = "";
    }
    oneTareaScreen->deleteLater();
    emit updateOtherTable();
    on_pb_close_clicked();
}

void screen_tabla_tareas::show_loading(QString mess){
    emit hidingLoading();

    QWidget *widget_blur = new QWidget(this);
    widget_blur->setFixedSize(this->size()+QSize(0,30));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &screen_tabla_tareas::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &screen_tabla_tareas::hidingLoading, widget_blur, &QWidget::deleteLater);

    MyLabelShine *label_loading_text = new MyLabelShine(widget_blur);
    label_loading_text->setStyleSheet("background-color: rgb(255, 255, 255);"
                                      "color: rgb(54, 141, 206);"
                                      "border-radius: 10px;"
                                      "font: italic 14pt \"Segoe UI Semilight\";");

    QRect rect = widget_blur->geometry();
    label_loading_text->setText(mess);
    label_loading_text->setFixedSize(400, 150);
    label_loading_text->move(rect.width()/2
                             - label_loading_text->size().width()/2,
                             rect.height()/2
                             -  label_loading_text->size().height()/2);
    label_loading_text->setMargin(20);
    label_loading_text->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    label_loading_text->show();
    connect(this, &screen_tabla_tareas::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &screen_tabla_tareas::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &screen_tabla_tareas::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &screen_tabla_tareas::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &screen_tabla_tareas::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}
void screen_tabla_tareas::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void screen_tabla_tareas::hide_loading(){
    emit hidingLoading();
}

void screen_tabla_tareas::on_pb_insert_task_to_specific_operator_clicked()
{
    if(seleccionOperarioScreen ==nullptr){
        seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);
    }
    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));

    QItemSelectionModel *select = ui->tableView->selectionModel();
    QModelIndexList list = select->selectedRows();

    if(!list.empty())
    {
        if(seleccionOperarioScreen->exec())
        {
            other_task_screen *oneTareaScreen = new other_task_screen(nullptr, false, true, empresa);

            QJsonObject o;

            o = jsonArrayAllTask[list[0].row()].toObject();

            o.insert(operario,operatorName);

            connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
            emit sendData(o);
            disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));

            oneTareaScreen->populateView();
            QRect rect = QGuiApplication::screens().first()->geometry();
            if(rect.height() <= 768
                    && rect.width() <= 1366){
                oneTareaScreen->showMaximized();
            }else {
                oneTareaScreen->show();
            }
        }
        disconnect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));
    }
}

void screen_tabla_tareas::get_user_selected(QString u)
{
    operatorName = u;
}

void screen_tabla_tareas::on_pb_insert_task_to_one_operator_clicked()
{
    if(seleccionOperarioScreen==nullptr){
        seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);
    }
    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(!selection.empty())
    {
        if(seleccionOperarioScreen->exec())
        {
            show_loading("Asignando...");
            QJsonObject o;
            QList<int> lista_posiciones_tareas_a_subir;
            int total = selection.count();

            other_task_screen *oneTareaScreen = new other_task_screen(nullptr, false, true, empresa);
            for(int i=0, reintentos = 0; i< selection.count(); i++)
            {
                setLoadingText("Espere, asignando tareas... ("
                               +QString::number(i)+"/"+QString::number(total)+")");

                QModelIndex index = selection.at(i);
                int posicion = index.row();

                if(jsonArrayAllTask.size() > posicion){
                    o = jsonArrayAllTask[posicion].toObject();

                    o.insert(operario,operatorName);

                    oneTareaScreen->clearTask();

                    oneTareaScreen->setShowMesageBox(false);

                    connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
                    emit sendData(o);
                    disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));

                    oneTareaScreen->populateView(false);
                    //            QEventLoop q;

                    QEventLoop *q = new QEventLoop();

#if (USE_TIMEOUT == 1)
                    try{
                        QTimer::singleShot(TIMEOUT, this, SLOT(conection_timeout()));
                    }catch(QException e){
                        e.raise();
                        qDebug()<<"Excepcion del timer lanzada";
                    }

#endif
                    connect(oneTareaScreen, &other_task_screen::task_upload_excecution_result,q,&QEventLoop::exit);

                    oneTareaScreen->setShowMesageBox(false);

                    QTimer *timer;
                    timer = new QTimer();

                    timer->setSingleShot(true);
                    connect(timer,SIGNAL(timeout()),oneTareaScreen,SLOT(on_pb_update_server_info_clicked()));
                    timer->start(DELAY);

                    switch(q->exec())
                    {
                    case database_comunication::script_result::timeout:
                        i--;
                        reintentos++;
                        if(reintentos == RETRIES)
                        {
                            GlobalFunctions gf(this);
                            GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                            i = jsonArrayAllTask.size();
                        }
                        break;
                    case database_comunication::script_result::task_to_server_ok:
                        reintentos = 0;
                        break;
                    case database_comunication::script_result::update_task_to_server_failed:
                        i--;
                        reintentos++;
                        if(reintentos == RETRIES)
                        {
                            GlobalFunctions gf(this);
                            GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                            i = jsonArrayAllTask.size();
                        }
                        break;
                    case database_comunication::script_result::create_task_to_server_failed:
                        i--;
                        reintentos++;
                        if(reintentos == RETRIES)
                        {
                            GlobalFunctions gf(this);
                            GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                            i = jsonArrayAllTask.size();
                        }
                        break;
                    }
                    delete timer;
                    delete q;
                }

            }
            oneTareaScreen->deleteLater();
            hide_loading();
        }
        if(other_task_screen::conexion_activa){
            GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
        }else {
            GlobalFunctions::showMessage(this,"Éxito","Información actualizada en respaldo local satisfactoriamente.");
        }
        disconnect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));
        emit updateOtherTable();
        on_pb_close_clicked();
    }else{
        GlobalFunctions::showMessage(this,"Sin selección","Debe seleccionar al menos una tarea");
    }
}

void screen_tabla_tareas::get_fields_selected(QMap<QString, QString> map_received){
    if(!map_received.isEmpty()){
        fields_selected = map_received;
    }
}

void screen_tabla_tareas::on_pb_assign_fields_to_multiple_tasks_clicked()
{
    //ui->pb_insert_all_tasks_to_one_operator->setEnabled(false);

    Fields_to_Assign_Dialog = new Fields_to_Assign(this, empresa);

    connect(Fields_to_Assign_Dialog,SIGNAL(fields_selected(QMap<QString, QString>)),this,SLOT(get_fields_selected(QMap<QString, QString>)));

    QJsonObject jsonObject;
    QList<QString> keys;
    if(Fields_to_Assign_Dialog->exec())
    {
        if(!fields_selected.isEmpty()){
            show_loading("Espere, asignando campos a tareas...");
            keys = fields_selected.keys();

            for(int i = 0; i< fields_selected.keys().size(); i++){
                qDebug()<<"Campo: "<<fields_selected.keys().at(i)<<"   Valor: "<<fields_selected.values().at(i);
            }
            QList<int> lista_posiciones_tareas_a_subir;

            other_task_screen *oneTareaScreen = new other_task_screen(nullptr, false, true, empresa);
            QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
            int total = selection.count();
            for(int i=0, reintentos = 0; i< selection.count(); i++)
            {
                setLoadingText("Espere, subiendo tareas... ("
                               +QString::number(i)+"/"+QString::number(total)+")");

                QModelIndex index = selection.at(i);
                int posicion = index.row();
                //                qDebug() << "lista_numeros_internos";
                //                qDebug() << lista_numeros_internos.size();

                if(jsonArrayAllTask.size() > posicion){
                    jsonObject = jsonArrayAllTask[posicion].toObject();

                    //                    qDebug() << jsonObject.value(numero_interno);
                    for(int n=0; n < fields_selected.size(); n++){
                        QString key = keys.at(n);
                        if(key!="null" && key != nullptr && !key.isEmpty()){
                            QString value = fields_selected.value(key, "null");
                            if(value != "null"){
                                jsonObject.insert(key, value);
                            }
                        }
                    }

                    qDebug() << jsonObject.value(codigo_de_geolocalizacion);

                    oneTareaScreen->setShowMesageBox(false);

                    connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
                    emit sendData(jsonObject);
                    disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));

                    oneTareaScreen->populateView(false);

                    QEventLoop *q = new QEventLoop();
                    connect(oneTareaScreen, &other_task_screen::task_upload_excecution_result,q,&QEventLoop::exit);

                    oneTareaScreen->setShowMesageBox(false);

                    QTimer::singleShot(DELAY,oneTareaScreen,&other_task_screen::on_pb_update_server_info_clicked);

                    switch(q->exec())
                    {
                    case database_comunication::script_result::timeout:
                        i--;
                        reintentos++;
                        if(reintentos == RETRIES)
                        {
                            GlobalFunctions gf(this);
                            GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                            i = jsonArrayAllTask.size();
                        }
                        break;
                    case database_comunication::script_result::task_to_server_ok:
                        reintentos = 0;
                        break;
                    case database_comunication::script_result::update_task_to_server_failed:
                        i--;
                        reintentos++;
                        if(reintentos == RETRIES)
                        {
                            GlobalFunctions gf(this);
                            GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                            i = jsonArrayAllTask.size();
                        }
                        break;
                    case database_comunication::script_result::create_task_to_server_failed:
                        i--;
                        reintentos++;
                        if(reintentos == RETRIES)
                        {
                            GlobalFunctions gf(this);
                            GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                            i = jsonArrayAllTask.size();
                        }
                        break;
                    }
                    delete q;
                }
            }
            oneTareaScreen->deleteLater();
            hide_loading();
            if(other_task_screen::conexion_activa){
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
            }else {
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada en respaldo local");
            }
            //            getAllInternalNumbers();
            disconnect(oneTareaScreen, SIGNAL(script_excecution_result(int)), this, SLOT(update_script_execution_result(int)));
        }
    }
}

void screen_tabla_tareas::on_pb_insert_all_tasks_to_one_operator_clicked()
{
    ui->pb_insert_all_tasks_to_one_operator->setEnabled(false);

    if(seleccionOperarioScreen==nullptr){
        seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);
    }
    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));

    if(seleccionOperarioScreen->exec())
    {
        show_loading("Espere, asignando tareas...");
        QJsonObject o;
        QStringList alredyOnDatabase, lista_principal_variables;
        //        getAllInternalNumbers();
        bool duplicated = false;
        int total = jsonArrayAllTask.count();

        other_task_screen *oneTareaScreen = new other_task_screen(nullptr, false, true, empresa);
        GlobalFunctions gf(this, empresa);
        lista_principal_variables = gf.getTareasList();

        for(int i = 0, reintentos = 0; i < jsonArrayAllTask.count();i++)
        {

            setLoadingText("Espere, subiendo tareas... ("
                           +QString::number(i)+"/"+QString::number(total)+")");
            o = jsonArrayAllTask[i].toObject();
            QJsonObject jsonObjectFound = checkIfDuplicateByInformation(o);
            if(!jsonObjectFound.isEmpty()){
                duplicated = true;
                if(file_type!= 3 || file_type!= 4){
                    continue;
                }else{
                    o.insert(principal_variable, jsonObjectFound.value(principal_variable).toString());
                }
            }
            if(!lista_principal_variables.contains(o.value(principal_variable).toString()))
            {
                o = jsonArrayAllTask[i].toObject();
                o.insert(operario, operatorName);

                oneTareaScreen->setShowMesageBox(false);

                connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
                emit sendData(o);
                disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));

                oneTareaScreen->populateView(false);

                QEventLoop *q = new QEventLoop();

                connect(oneTareaScreen, &other_task_screen::task_upload_excecution_result,q,&QEventLoop::exit);

                oneTareaScreen->setShowMesageBox(false);

                QTimer::singleShot(DELAY,oneTareaScreen,&other_task_screen::on_pb_update_server_info_clicked);

                switch(q->exec())
                {
                case database_comunication::script_result::timeout:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        GlobalFunctions gf(this);
                        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = jsonArrayAllTask.size();
                    }
                    break;
                case database_comunication::script_result::task_to_server_ok:
                    reintentos = 0;
                    break;
                case database_comunication::script_result::update_task_to_server_failed:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        GlobalFunctions gf(this);
                        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = jsonArrayAllTask.size();
                    }
                    break;
                case database_comunication::script_result::create_task_to_server_failed:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        GlobalFunctions gf(this);
                        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = jsonArrayAllTask.size();
                    }
                    break;
                }
                delete q;
                oneTareaScreen->clear_all_pictures();
            }
            else
            {
                alredyOnDatabase << o.value(principal_variable).toString();
            }
        }
        if(!alredyOnDatabase.isEmpty())
        {
            GlobalFunctions gf(this);
            GlobalFunctions::showWarning(this,"Números internos replicados","Los siguientes numeros internos no se cargaron porque ya se encontraban en el servidor.\n" + alredyOnDatabase.join(" , "));
        }
        if(duplicated)
        {
            GlobalFunctions gf(this);
            GlobalFunctions::showWarning(this,"Informacion replicada","Alguna tareas no fueron subidas porque ya se encontraban en el servidor.");
        }
        hide_loading();

        if(other_task_screen::conexion_activa){
            GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
        }else {
            GlobalFunctions::showMessage(this,"Éxito","Información actualizada en respaldo local satisfactoriamente.");
        }
        oneTareaScreen->deleteLater();
        disconnect(oneTareaScreen, SIGNAL(script_excecution_result(int)), this, SLOT(update_script_execution_result(int)));
        emit updateOtherTable();
        on_pb_close_clicked();
    }
    ui->pb_insert_all_tasks_to_one_operator->setEnabled(true);
}
void screen_tabla_tareas::get_order_selected(QString o)
{
    selection_Order = o;
}
void screen_tabla_tareas::get_date_selected(QDate d)
{
    selection_date = d;
}

void screen_tabla_tareas::on_pb_export_to_excel_clicked()
{
    QString rutaToXLSXFile = QFileDialog::getSaveFileName(this,"Seleccione la ruta y nombre del fichero xlsx.",
                                                          QDir::current().path()+"/"+ QDateTime::currentDateTime().toString(formato_fecha_hora).
                                                          replace(":","_").replace("-","_").replace(" ","__")
                                                          +"__Trabajo_Exportado", "Excel (*.xlsx)");
    if(!rutaToDATFile.isNull() && !rutaToDATFile.isEmpty() && !rutaToXLSXFile.isNull() && !rutaToXLSXFile.isEmpty())
    {
        QJsonArray ja;
        ja = jsonArrayAllTask;///parse_to_QjsonArray(rutaToDATFile);

        //        QStringList listHeaders = ja[0].toObject().keys();
        QMap<QString,QString> mapa_exportacion;
        mapa_exportacion.insert("Población",poblacion);
        mapa_exportacion.insert("CALLE",calle);
        mapa_exportacion.insert("Nº",numero);
        mapa_exportacion.insert("BIS",BIS);
        mapa_exportacion.insert("PISO",piso);
        mapa_exportacion.insert("MANO",mano);
        mapa_exportacion.insert("AÑO O PREFIJO CONT. RETIRADO",CONTADOR_Prefijo_anno);
        mapa_exportacion.insert("Nº SERIE CONT. RETIRADO",numero_serie_contador);
        mapa_exportacion.insert("CALIBRE CONT. RETIRADO",calibre_toma);
        mapa_exportacion.insert("CALIBRE. INST.",calibre_real);
        mapa_exportacion.insert("OPERARIO",operario);
        mapa_exportacion.insert("Anomalía (TAREA A REALIZAR)",ANOMALIA);
        mapa_exportacion.insert("EMPLAZAMIENTO",emplazamiento);
        mapa_exportacion.insert("OBSERVACIONES",observaciones);
        mapa_exportacion.insert("ACTIVIDAD",actividad);
        mapa_exportacion.insert("TITULAR",nombre_cliente);
        mapa_exportacion.insert("NºABONADO",numero_abonado);
        mapa_exportacion.insert("TELEFONO",telefono1);
        mapa_exportacion.insert("ACCESO",acceso);
        mapa_exportacion.insert("RESULTADO",resultado);
        mapa_exportacion.insert("NUEVO",nuevo_citas);
        mapa_exportacion.insert("FECHA",F_INST);
        mapa_exportacion.insert("ZONAS",zona);
        mapa_exportacion.insert("RUTA",ruta);
        mapa_exportacion.insert("MARCA",marca_contador);
        mapa_exportacion.insert("ÚLTIMA LECTURA",lectura_actual);
        mapa_exportacion.insert("GESTOR",GESTOR);
        mapa_exportacion.insert("LECTURA DE CONTADOR INSTALADO",lectura_contador_nuevo);
        mapa_exportacion.insert("Nº SERIE CONTADOR INSTALADO",numero_serie_contador_devuelto);
        mapa_exportacion.insert("Nº ANTENA CONTADOR INSTALADO",numero_serie_modulo);
        mapa_exportacion.insert("CALIBRE CONTADOR INSTALADO",calibre_real);
        mapa_exportacion.insert("MARCA CONTADOR INSTALADO",marca_devuelta);
        mapa_exportacion.insert("CLASE CONTADOR INSTALADO",TIPO_devuelto);
        mapa_exportacion.insert("LONGITUD CONTADOR INSTALADO",largo_devuelto);
        mapa_exportacion.insert("CÓDIGO DE LOCALIZACIÓN",codigo_de_geolocalizacion);
        mapa_exportacion.insert("LINK GEOLOCALIZACIÓN",url_geolocalizacion);

        QStringList listHeaders;
        listHeaders << "Población" << "CALLE" << "Nº" << "BIS" << "PISO"
                    << "MANO" << "AÑO O PREFIJO CONT. RETIRADO" << "Nº SERIE CONT. RETIRADO"
                    << "CALIBRE CONT. RETIRADO"
                    << "CALIBRE. INST." << "OPERARIO" <<"Anomalía (TAREA A REALIZAR)"
                    << "EMPLAZAMIENTO" << "OBSERVACIONES" << "ACTIVIDAD"
                    << "TITULAR" << "NºABONADO" << "TELEFONO" << "ACCESO"
                    << "RESULTADO" << "NUEVO" << "FECHA" << "ZONAS"
                    << "RUTA" << "MARCA"<< "ÚLTIMA LECTURA"<<"GESTOR"
                    <<"LECTURA DE CONTADOR INSTALADO"<< "Nº SERIE CONTADOR INSTALADO"
                   <<"Nº ANTENA CONTADOR INSTALADO" << "CALIBRE CONTADOR INSTALADO"
                  <<"MARCA CONTADOR INSTALADO" << "CLASE CONTADOR INSTALADO"<< "LONGITUD CONTADOR INSTALADO"
                 << "CÓDIGO DE LOCALIZACIÓN" << "LINK GEOLOCALIZACIÓN";
        int rows = ja.count();

        QXlsx::Document xlsx;
        QXlsx::Format header_format;
        header_format.setTextWarp(true);
        //        header_format.setHorizontalAlignment(Format::AlignHDistributed);
        //        header_format.setVerticalAlignment(Format::AlignVDistributed);
        //        header_format.setShrinkToFit(true);
        //        header_format.setDiagonalBorderType(Format::DiagonalBorderUp);
        //        header_format.setDiagonalBorderStyle(Format::BorderThin);

        //write headers
        for (int i=0; i<listHeaders.count(); i++)
        {
            xlsx.write(1, i+1, listHeaders[i]);
            //            xlsx.setColumnWidth(i+1, i+1, listHeaders[i].count());
        }


        //        xlsx.setRowFormat(1,1,header_format);
        //        xlsx.setColumnFormat(1,listHeaders.count(),header_format);

        //write data
        for(int i = 0; i < rows; i++)
        {
            for(int j = 0; j < listHeaders.size(); j++)
            {
                //            QString temp = ja[i].toObject().value(listHeaders[j]).toString();
                xlsx.write(i+2, j+1, ja[i].toObject().value((mapa_exportacion.value(listHeaders[j]))).toString());
            }
        }

        //        for (int i=0; i<listHeaders.count(); i++)
        //        {
        //            int test = listHeaders[i].count();
        //            bool t = xlsx.setColumnWidth(i+1, i+1, listHeaders[i].count());
        //        }

        xlsx.saveAs(rutaToXLSXFile);

        GlobalFunctions::showMessage(this,"Éxito","Fichero XLSX generado correctamente");
    }
}

void screen_tabla_tareas::on_pb_close_clicked()
{
    emit closing();
    ui->cb_overwrite_tasks->setChecked(false);
    if(model){
        model->clear();
    }
    this->close();
}

void screen_tabla_tareas::on_drag_screen()
{
    start_moving_screen.stop();
    if(isFullScreen() || isMaximized()){
        return;
    }
    if(QApplication::mouseButtons()==Qt::LeftButton){

        start_moving_screen.start(10);
        init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
        init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
    }
    else if(QApplication::mouseButtons()==Qt::RightButton){

    }
}

void screen_tabla_tareas::on_drag_screen_released()
{
    if(isFullScreen()){

        return;
    }
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
    //current_win_Pos = QPoint(this->pos().x()-200,this->pos().y()-200);
}

void screen_tabla_tareas::on_start_moving_screen_timeout()
{
    int x_pos = static_cast<int>(this->pos().x()+((QWidget::mapFromGlobal(QCursor::pos())).x() - init_pos_x));
    int y_pos = static_cast<int>(this->pos().y()+((QWidget::mapFromGlobal(QCursor::pos())).y() - init_pos_y));
    x_pos = (x_pos < 0)?0:x_pos;
    y_pos = (y_pos < 0)?0:y_pos;

    x_pos = (x_pos > QApplication::desktop()->width()-100)?QApplication::desktop()->width()-100:x_pos;
    y_pos = (y_pos > QApplication::desktop()->height()-180)?QApplication::desktop()->height()-180:y_pos;

    this->move(x_pos,y_pos);

    init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
    init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
}

QString screen_tabla_tareas::get_date_from_status(QJsonObject object, QString status)
{
    if(status=="IDLE"){
        return object.value(FechImportacion).toString();
    }
    if(status=="DONE" || status=="CITA"){
        return object.value(F_INST).toString();
    }
    if(status=="CLOSED"){
        return object.value(FECH_CIERRE).toString();
    }
    if(status=="INFORMADA"){
        return object.value(fech_informacionnew).toString();
    }
    if(status=="REQUERIDA"){
        return object.value(F_INST).toString();
    }
    return "";
}

QJsonObject screen_tabla_tareas::set_date_from_status(QJsonObject object, QString status, QString date)
{
    if(status=="IDLE"){
        object.insert(FechImportacion, date);
    }
    if(status=="DONE" || status=="CITA"){
        object.insert(F_INST, date);
    }
    if(status=="CLOSED"){
        object.insert(FECH_CIERRE, date);
    }
    if(status=="INFORMADA"){
        object.insert(fech_informacionnew, date);
    }
    if(status=="REQUERIDA"){
        object.insert(F_INST, date);
    }
    return object;
}

void screen_tabla_tareas::readVariablesInBD()
{
    QFile *data_base = new QFile(datos); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::ReadOnly))
        {
            QDataStream in(data_base);
            in>>emailPermission;
            in>>lastSync;
            data_base->close();
        }
    }
}

void screen_tabla_tareas::writeVariablesInBD()
{
    QFile *data_base = new QFile(datos); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::ReadWrite))
        {
            data_base->seek(0);
            QDataStream out(data_base);
            out<<emailPermission;
            out<<lastSync;
            data_base->close();
        }
    }
}

QString screen_tabla_tareas::composeNumExp(int num)
{
    QString numreturn = QString::number(num);

    while(numreturn.length() < 5){
        numreturn = "0"+numreturn;
    }
    return numreturn;
}

void screen_tabla_tareas::on_cb_overwrite_tasks_clicked(bool checked)
{
    if(checked){
        GlobalFunctions gf(this);
        if(gf.showQuestion(this, "Confirmación", "¿Desea sobreescribir información de tarea(s) existente(s) en el servidor?",
                           QMessageBox::Ok, QMessageBox::No)
                == QMessageBox::Ok){
            ui->cb_overwrite_tasks->setChecked(true);
        }
        else{
            ui->cb_overwrite_tasks->setChecked(false);
        }
    }
}
void screen_tabla_tareas::updateIDOrdenes(QString id_orden){
    QJsonObject jsonObject;
    bool ok;
    int id_orden_inicial = id_orden.toInt(&ok);
    if(ok){
        for(int i=0; i < jsonArrayAllTask.size(); i++){
            jsonObject = jsonArrayAllTask.at(i).toObject();
            jsonObject.insert(idOrdenCABB, QString::number(id_orden_inicial));
            jsonObject.insert(ID_SAT, QString::number(id_orden_inicial));
            id_orden_inicial++;
            jsonArrayAllTask.replace(i, jsonObject);
        }
        populateTable(database_comunication::serverRequestType::GET_TASKS);
    }
}
void screen_tabla_tareas::on_pb_id_orden_inicial_clicked()
{
    IDOrdenAssign *id_orden_screen = new IDOrdenAssign(this);
    connect(id_orden_screen, &IDOrdenAssign::id_selected, this, &screen_tabla_tareas::updateIDOrdenes);
    id_orden_screen->moveCenter();
    id_orden_screen->show();
}

void screen_tabla_tareas::on_pb_cruz_clicked()
{
    on_pb_close_clicked();
}

void screen_tabla_tareas::on_pb_maximize_clicked()
{
    if(this->isMaximized()){
        this->showNormal();
    }else {
        this->showMaximized();
    }
    QTimer::singleShot(200, this, SLOT(setTableView()));
}

void screen_tabla_tareas::on_pb_minimize_clicked()
{
    this->showMinimized();
}
