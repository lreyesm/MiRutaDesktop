#include "screen_table_itacs.h"
#include "ui_screen_table_itacs.h"

#include "global_variables.h"
#include "rightclickmenu.h"
#include <QMessageBox>
#include <QScreen>
#include <QDesktopWidget>
#include "operator_selection_screen.h"
#include "equipo_selection_screen.h"
#include "rightclickedsection.h"
#include "mycheckbox.h"
#include "mylineeditshine.h"
#include "fields_to_assing_itacs.h"
#include <QScrollArea>
#include <QCheckBox>
#include <QCompleter>
#include "mapas_cercania_itacs.h"
#include "QProgressIndicator.h"
#include "globalfunctions.h"
#include <QGraphicsDropShadowEffect>

Screen_Table_ITACs::Screen_Table_ITACs(QWidget *parent, bool show, QString empresa, QString gestor) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_ITACs)
{    
    Q_UNUSED(show);
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowTitle("Tabla de ITACs");
    this->empresa = empresa;
    this->gestor = gestor;

    ui->statusbar->hide();


    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));

    connect(ui->l_current_pagination, &MyLabelSpinner::itemSelected,
            this, &Screen_Table_ITACs::moveToPage);
    connect(this, &Screen_Table_ITACs::mouse_pressed,
            ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Screen_Table_ITACs::sectionPressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Screen_Table_ITACs::closing, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Screen_Table_ITACs::tablePressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
}

Screen_Table_ITACs::~Screen_Table_ITACs()
{
    delete ui;
}

void Screen_Table_ITACs::showEvent(QShowEvent *event){
    QWidget::showEvent(event);
    QTimer::singleShot(200, this, &Screen_Table_ITACs::getITACs);
}

void Screen_Table_ITACs::closeEvent(QCloseEvent *event)
{
    emit closing();
    QWidget::closeEvent(event);
}
void Screen_Table_ITACs::resizeEvent(QResizeEvent *event){

    QWidget::resizeEvent(event);
    if(timer.isActive()){
        timer.stop();
    }
    timer.setInterval(200);
    timer.start();
    connect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
}

QString Screen_Table_ITACs::crearFicheroTxtConTabla(QJsonArray jsonArray,QString ruta_y_nombre_file){
    QJsonDocument doc;
    doc.setArray(jsonArray);
    QString filename = ruta_y_nombre_file;
    if(filename.contains(".xlsx")){
        filename.replace(".xlsx", ".txt"); //cambiando formato si es necesario
    }
    if(filename.contains(".dat")){
        filename.replace(".dat", ".txt"); //cambiando formato si es necesario
    }
    if(!filename.contains(".txt")){
        filename+=".txt";
    }
    QByteArray byteArray = doc.toJson();
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        file.write(byteArray);
        file.close();
    }
    return ruta_y_nombre_file;
}
QString Screen_Table_ITACs::crearFicheroDATConTabla(QJsonArray jsonArray,QString ruta_y_nombre_file){
    QJsonDocument doc;
    doc.setArray(jsonArray);
    QString filename = ruta_y_nombre_file;
    if(filename.contains(".xlsx")){
        filename.replace(".xlsx", ".dat"); //cambiando formato si es necesario
    }
    if(filename.contains(".txt")){
        filename.replace(".txt", ".dat"); //cambiando formato si es necesario
    }
    if(!filename.contains(".dat")){
        filename+=".dat";
    }
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly)){
        file.seek(0);
        QDataStream out(&file);
        out<<jsonArray;
        file.close();
    }
    return filename;
}
QJsonArray Screen_Table_ITACs::fixJsonToLastModel(QJsonArray jsonArray){ //recibe el arreglo mostrado en la tabla entes de la descarga
    QStringList principal_var_all, principal_var_model;
    for (int i=0; i < jsonArray.size(); i++) { //obteniendo NUMIN de todos los json mostrados
        principal_var_model << jsonArray[i].toObject().value(codigo_itac_itacs).toString();
    }
    for (int i=0; i < jsonArrayAll.size(); i++) { //obteniendo NUMIN de todos los json descargados
        principal_var_all << jsonArrayAll[i].toObject().value(codigo_itac_itacs).toString();
    }
    for (int i=0; i < principal_var_model.size(); i++) { //si el jsonArray descargado no contiene algun json mostrado elimino el json mostrado
        if(!principal_var_all.contains(principal_var_model[i])){//para el caso que se haya eliminado
            jsonArray.removeAt(i);
            principal_var_model.removeAt(i);
            i--;
        }else{ //si el jsonArray descargado contiene el json mostrado actualizo la tabla (json mostrado) con jsonArray descargado
            int index = principal_var_all.indexOf(principal_var_model[i]);
            jsonArray[i] = jsonArrayAll[index];
        }
    }
    for (int i=0; i < principal_var_all.size(); i++) { //si se ha añadido tarea agregar del json descargado la tarea al json mostrado
        if(!principal_var_model.contains(principal_var_all[i])){//para el caso que se haya añadido tarea
            jsonArray.append(jsonArrayAll[i]);
        }
    }
    if(filtering){
        QJsonArray jsonArray_model; //para mantener la tabla identica a la anterior
        for (int i=0; i < jsonArray.size(); i++) {//si el json mostrado contiene el json descargado lo muestro para mantener ultimo filtro en la tabla (mostrado)
            if(principal_var_model.contains(jsonArray[i].toObject()
                                            .value(codigo_itac_itacs).toString())){
                jsonArray_model.append(jsonArray[i].toObject());
            }
        }
        return jsonArray_model;
    }else {
        return jsonArray;
    }
}

void Screen_Table_ITACs::updateItacsInTable(){
    if(this->isHidden()){
        return;
    }
    QString query = lastQuery;
    int id_start = last_id_start;
    getItacsCustomQuery(query, id_start);

    if(filtering){
        jsonArrayInTableFiltered = fixJsonToLastModel(jsonArrayInTableFiltered);
        fixModelForTable(jsonArrayInTableFiltered);
    }else{
        jsonArrayInTable = fixJsonToLastModel(jsonArrayInTable);
        fixModelForTable(jsonArrayInTable);
    }
    setTableView();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    if(lastSelectedRow != -1 && lastSelectedRow < jsonArray.size()
            && lastSelectedRow < model->rowCount()){
        QModelIndex index = model->index(lastSelectedRow, 0);
        ui->tableView->setCurrentIndex(index);
    }
}

QString Screen_Table_ITACs::getQueyStatus(){
    QString queryStatus = defaultQuery;
    QString query = " (" + queryStatus + ") ";
    return query;
}


void Screen_Table_ITACs::getITACs()
{
    show_loading("Cargando ITACs...");

    filtering = false;
    currentPage = 1;
    QString query = defaultQuery;
    getItacsCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;
    populateTable(database_comunication::GET_ITACS);

    hide_loading();
}
void Screen_Table_ITACs::get_itacs_amount_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ITACS_AMOUNT,keys,values);
}
void Screen_Table_ITACs::get_itacs_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ITACS_CUSTOM_QUERY,keys,values);
}
void Screen_Table_ITACs::get_all_column_values_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES,keys,values);
}
void Screen_Table_ITACs::get_all_column_values_custom_query_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES_CUSTOM_QUERY,keys,values);
}
void Screen_Table_ITACs::update_itacs_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_ITAC_FIELDS,keys,values);
}

bool Screen_Table_ITACs::getItacsCustomQuery(QString query, int id_start){

    //    if(currentGestor != "Todos"){
    //        query += " AND (" + gestor_itacs + " LIKE '" + currentGestor +"')";
    //    }
    if(lastQuery != query){
        currentPage = 1;
    }
    lastQuery = query;
    last_id_start = id_start;

    getItacsAmountFromServer(empresa, query, limit_pagination);

    bool res = getItacsFromServer(empresa, query, limit_pagination, id_start);
    qDebug()<<"query send -> "<< query << " <- id start" << id_start;
    return res;
}
bool Screen_Table_ITACs::getItacsFromServer(QString empresa, QString query, int limit, int id_start)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT" << "id_start";
    values << empresa.toLower() << query << QString::number(limit) << QString::number(id_start);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_Table_ITACs::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_Table_ITACs::get_itacs_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::get_itacs_custom_query_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}
bool Screen_Table_ITACs::getItacsAmountFromServer(QString empresa, QString query, int limit)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT";
    values << empresa.toLower() << query << QString::number(limit);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_Table_ITACs::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_Table_ITACs::get_itacs_amount_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::get_itacs_amount_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}

bool Screen_Table_ITACs::getItacsValuesFieldServer(QString empresa, QString column)
{
    bool res = false;
    QStringList keys, values;

    keys << "empresa" << "columna" << "tabla";
    values << empresa << column << "itacs";

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &Screen_Table_ITACs::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, SLOT(get_all_column_values_request()));

    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;
    }
    delete q;
    return res;
}
bool Screen_Table_ITACs::getItacsValuesFieldCustomQueryServer(QString empresa, QString column, QString query)
{
    bool res = false;
    QStringList keys, values;

    keys << "empresa" << "columna" << "tabla" << "query";
    values << empresa << column << "itacs" << query;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &Screen_Table_ITACs::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, SLOT(get_all_column_values_custom_query_request()));

    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;
    }
    delete q;
    return res;
}

bool Screen_Table_ITACs::updateITACs(QStringList lista_cod_emplazamientos, QJsonObject campos){

    QJsonObject cod_emplazamientos;

    for (int i=0; i < lista_cod_emplazamientos.size(); i++) {
        cod_emplazamientos.insert(QString::number(i), lista_cod_emplazamientos.at(i));
    }
    if(cod_emplazamientos.isEmpty()){
        return true;
    }
    campos.insert(date_time_modified_itacs, QDateTime::currentDateTime().toString(formato_fecha_hora));

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(cod_emplazamientos);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp_fields, temp_numins = QString::fromUtf8(ba);

    d.setObject(campos);
    ba = d.toJson(QJsonDocument::Compact);
    temp_fields = QString::fromUtf8(ba);

    keys << "json_cod_emplazamientos" << "json_fields" << "empresa";
    values << temp_numins << temp_fields << empresa.toLower();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_Table_ITACs::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_Table_ITACs::update_itacs_fields_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::update_itacs_fields_to_server_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}

void Screen_Table_ITACs::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
    Q_UNUSED(tipo);
    jsonArrayInTable = ordenarPor(jsonArrayInTable, codigo_itac_itacs, "");
}

void Screen_Table_ITACs::setGestor(QString g)
{
    this->gestor = g;
}
QMap <QString,QStringList> Screen_Table_ITACs::fillMapForFixModel(QStringList &listHeaders){
    QMap <QString,QStringList> mapa;
    QStringList values;
    values << zona_itacs;
    mapa.insert("SECTOR P", values);
    values.clear();
    values << itac_itacs;
    mapa.insert("DIRECCIÓN", values);
    values.clear();
    values << codigo_itac_itacs;
    mapa.insert("C.EMPLAZAMIENTO", values);
    values.clear();
    values << puntuacion_itacs;
    mapa.insert("PUNTUACIÓN DE ITAC", values);
    values.clear();
    values << nombre_empresa_administracion_itacs << nombre_responsable_administracion_itacs
           << telefono_fijo_administracion_itacs << telefono_movil_administracion_itacs << direccion_oficina_administracion_itacs;
    mapa.insert("ADMINISTRACIÓN", values);//+telefono
    values.clear();
    values << nombre_presidente_itacs << vivienda_presidente_itacs << telefono_movil_presidente_itacs << telefono_fijo_presidente_itacs;
    mapa.insert("PRESIDENTE O\n COLABORADOR", values);//+vivienda y telefono
    values.clear();
    values << nombre_encargado_itacs << vivienda_encargado_itacs << telefono_fijo_encargado_itacs << telefono_movil_encargado_itacs;
    mapa.insert("ENCARGADO O\n CONSERJE", values);//+vivienda y telefono
    values.clear();
    values << acceso_itacs << descripcion_itacs;
    mapa.insert("NOTA GLOBAL", values); // + Descripcion DE INSTALACION

    values.clear();
    values << puntos_agua_total_itacs;
    mapa.insert("N PUNTOS DE AGUA\n TOTAL",values);
    values.clear();
    values << puntos_agua_con_contador_itacs;
    mapa.insert("N PUNTOS DE AGUA\n CON CONTADOR",values);
    values.clear();
    values << puntos_agua_con_contador_mas_tarea_itacs;
    mapa.insert("N PUNTOS DE AGUA CON\nCONTADOR MÁS TAREA",values);

    //SECCION 1
    values.clear();
    values << extra_acceso_ubicacion_itacs;
    mapa.insert("ACCESO",values);

    //SECCION 2
    values.clear();
    values << tipo_llave_itacs << extras_llaves_itacs;
    mapa.insert("LLAVES DE ACCESO", values); // + extras_llaves_itacs

    //SECCION 3
    values.clear();
    values << espacio_para_trabajar_itacs;
    mapa.insert("ESPACIO PARA\n TRABAJAR",values);
    values.clear();
    values << desague_itacs << extras_desague_itacs;
    mapa.insert("DESAGÜE",values); // + extras_desague_itacs
    values.clear();
    values << iluminacion_itacs << extras_iluminacion_itacs;
    mapa.insert("ILUMINACIÓN",values);

    //SECCION 4
    values.clear();
    values << tubo_de_alimentacion_itacs;
    mapa.insert("TUBO DE\nALIMENTACIÓN",values);
    values.clear();
    values << colector_itacs;
    mapa.insert("COLECTOR",values);
    values.clear();
    values << tuberias_de_entrada_contador_itacs;
    mapa.insert("TUBERÍAS DE\nENTRADA",values);
    values.clear();
    values << tuberias_de_salida_contador_itacs;
    mapa.insert("TUBERÍAS DE\nSALIDA",values);

    //SECCION 4
    values.clear();
    values << valvula_general_itacs << extras_valvula_general_itacs;
    mapa.insert("VÁLVULA GENERAL",values); // + extras_valvula_general_itacs
    values.clear();
    values << valvula_entrada_itacs << extras_valvula_entrada_itacs;
    mapa.insert("VÁLVULA DE ENTRADA",values); // +extras_valvula_entrada_itacs
    values.clear();
    values << valvula_salida_itacs << extras_valvula_salida_itacs;
    mapa.insert("VÁLVULA DE SALIDA",values); // +extras_valvula_salida_itacs
    values.clear();
    values << valvula_antiretorno_itacs << extras_valvula_antiretorno_itacs;
    mapa.insert("VÁLVULA DE\nANTIRETORNO",values); // +extras_valvula_antiretorno_itacs

    values.clear();
    values << operario_itacs;
    mapa.insert("EQUIPO",values);
    values.clear();
    values << equipo_itacs;
    mapa.insert("OPERARIO",values);
    values.clear();
    values << gestor_itacs;
    mapa.insert("GESTOR",values);

    listHeaders  << "SECTOR P" << "DIRECCIÓN" <<"C.EMPLAZAMIENTO" << "PUNTUACIÓN DE ITAC"
                 << "ADMINISTRACIÓN" << "PRESIDENTE O\n COLABORADOR" <<  "ENCARGADO O\n CONSERJE" << "NOTA GLOBAL"
                 << "N PUNTOS DE AGUA\n TOTAL" <<   "N PUNTOS DE AGUA\n CON CONTADOR" <<  "N PUNTOS DE AGUA CON\nCONTADOR MÁS TAREA"
                 << "ACCESO"  <<"LLAVES DE ACCESO"
                 << "ESPACIO PARA\n TRABAJAR"  <<  "DESAGÜE"  <<  "ILUMINACIÓN"
                 << "TUBO DE\nALIMENTACIÓN"  <<  "COLECTOR"  <<  "TUBERÍAS DE\nENTRADA"  << "TUBERÍAS DE\nSALIDA"
                 << "VÁLVULA GENERAL"  <<  "VÁLVULA DE ENTRADA"  <<  "VÁLVULA DE SALIDA"  <<"VÁLVULA DE\nANTIRETORNO"
                 << "EQUIPO"<< "OPERARIO"<< "GESTOR";

    return mapa;
}
void Screen_Table_ITACs::fixModelForTable(QJsonArray jsonArray)
{    
    int rows = jsonArray.count();

    addItemsToPaginationInfo(rows);

    //comprobando que no exista un modelo anterior
    if(model!=nullptr){
        delete model;
    }

    QStringList listHeaders;
    QMap <QString,QStringList> mapa = fillMapForFixModel(listHeaders);

    model = new QStandardItemModel(rows, listHeaders.size());
    model->setHorizontalHeaderLabels(listHeaders);

    //insertando los datos
    QString column_info;
    for(int i = 0; i < rows; i++)
    {
        for (int n = 0; n < listHeaders.size(); n++) {

            bool depreceated_color = false;
            QJsonObject jsonObject = jsonArray[i].toObject();

            QString prioridad_l = jsonObject.value(prioridad_itacs).toString().trimmed();
            QString ult_mod = jsonObject.value(ultima_modificacion_itacs).toString().trimmed();

            QStringList header_values = mapa.value(listHeaders.at(n));
            QString header_value;
            column_info = "";
            foreach(header_value, header_values){
                QString value = jsonObject.value(header_value).toString().trimmed();
                if(checkIfFieldIsValid(value)){
                    column_info += value + ", ";
                }
            }
            if(!column_info.trimmed().isEmpty()){
                column_info = column_info.trimmed();
                column_info.remove(column_info.size()-1, 1);
            }

            QModelIndex index = model->index(i, n, QModelIndex());
            bool nueva_info = false;
            if(ult_mod.contains("ANDROID", Qt::CaseInsensitive)){
                nueva_info = true;
                model->setData(index, QColor(0,100,0), Qt::BackgroundRole);//Tareas sin revisar //verde
                if(depreceated_color){
                    model->setData(index, QColor(0,100,0), Qt::ForegroundRole);
                    QFont font = ui->tableView->font();
                    font.setBold(true);
                    model->setData(index, font, Qt::FontRole); //Texto rojo en Negritas oscuro alta prioridad
                }else{
                    model->setData(index, QColor(255,255,255), Qt::ForegroundRole);
                }
            }
            if(prioridad_l == "ALTA"){
                if(nueva_info && !depreceated_color){
                    model->setData(index, QColor(255,100,100), Qt::ForegroundRole); //Texto en rojo claro alta prioridad para nueva informacion
                }else{
                    model->setData(index, QColor(255,50,50), Qt::ForegroundRole); //Texto en rojo oscuro alta prioridad
                }
                QFont font = ui->tableView->font();
                font.setBold(true);
                model->setData(index, font, Qt::FontRole); //Texto rojo en Negritas oscuro alta prioridad
            }
            model->setData(index, column_info);

        }
    }
}

void Screen_Table_ITACs::setTableView()
{
    timer.stop();
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
    if(model!=nullptr){
        ui->tableView->setModel(model);

        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

        QList<double> sizes;
        sizes  << 1.2/*Sector P*/<< 1.5 /*ID ITAC (dir)*/<< 0.75/*Código*/ <<  0.8/*Empresa*/ <<  0.75/*GESTOR*/;

        int fields_count_in_table = ui->tableView->horizontalHeader()->count();
        ui->tableView->horizontalHeader()->setFixedHeight(50);
        QFont font = ui->tableView->font();
        int pointSize = font.pointSize();
        float ratio = static_cast<float>(pointSize)/fields_count_in_table;
        QRect rect = QGuiApplication::screens().first()->geometry();
        int width = ui->tableView->size().width();
        if(width >= rect.width()){
            width = rect.width();
        }
        int width_table = width - 15;
        float medium_width_fileds = static_cast<float>(width_table)/10/*fields_count_in_table*/;

        for (int i=0; i < fields_count_in_table; i++) {
            ui->tableView->setColumnWidth(i, static_cast<int>(medium_width_fileds)/**sizes.at(i)*ratio*/);
        }
        ui->tableView->horizontalHeader()->setSectionsMovable(true);
        ui->tableView->horizontalHeader()->setFont(ui->tableView->font());

        if(!connected_header_signal){
            connected_header_signal = true;
            connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
                    this, SLOT(on_sectionClicked(int)));
        }
    }
}

void Screen_Table_ITACs::on_sectionClicked(int logicalIndex)
{
    emit sectionPressed();
    filterColumnList.clear();
    lastCursorPos = this->mapFromGlobal(QCursor::pos());

    disconnect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));

    QStringList listHeaders;
    QMap <QString,QStringList> mapa = fillMapForFixModel(listHeaders);

    QString columna = listHeaders.at(logicalIndex);
    QStringList ordenamiento = mapa.value(columna);
    lastSectionCliked = columna;
    lastSectionFields = ordenamiento;

    RightClickedSection *rightClickedSection = new RightClickedSection(0, QCursor::pos());
    connect(rightClickedSection, SIGNAL(clickPressed(int)), this, SLOT(getMenuSectionClickedItem(int)));
    connect(this, &Screen_Table_ITACs::closing, rightClickedSection, &RightClickedSection::deleteLater);
    connect(this, &Screen_Table_ITACs::tablePressed, rightClickedSection, &RightClickedSection::deleteLater);
    connect(this, &Screen_Table_ITACs::mouse_pressed, rightClickedSection, &RightClickedSection::deleteLater);

    rightClickedSection->show();
}

QJsonArray Screen_Table_ITACs::getCurrentJsonArrayInTable(){
    QJsonArray jsonArray;
    if(filtering){
        jsonArray = jsonArrayInTableFiltered;
    }else{
        jsonArray = jsonArrayInTable;
    }
    return jsonArray;
}
QStringList Screen_Table_ITACs::getFieldValues(QStringList fields){
    QJsonArray jsonArray;
    QStringList values;

    if(solo_tabla_actual){
        jsonArray = getCurrentJsonArrayInTable();

    }else{
        show_loading("Cargando valores...");
        GlobalFunctions gf(this, empresa);
        jsonArray = gf.getItacsFields(fields);
        hide_loading();
    }

    QString value;
    for (int i=0; i < jsonArray.size(); i++) {
        QJsonObject jsonObject = jsonArray[i].toObject();
        QStringList header_values = fields;
        QString header_value;
        value = "";
        foreach(header_value, header_values){
            QString data = jsonObject.value(header_value).toString().trimmed();
            if(checkIfFieldIsValid(data)){
                value += data + ", ";
            }
        }
        if(!value.trimmed().isEmpty()){
            value = value.trimmed();
            value.remove(value.size()-1, 1);
        }
        if(checkIfFieldIsValid(value)){
            if(!values.contains(value)){
                values << value;
            }
        }
    }
    values.sort();
    return values;
}
void Screen_Table_ITACs::filtrarEnTabla(bool checked){
    solo_tabla_actual = checked;
    emit sectionPressed();

    showFilterWidgetOptions(false);
}
void Screen_Table_ITACs::filterColumnField(){
    if(filterColumnList.isEmpty()){
        return;
    }
    show_loading("Buscando Resultados...");
    QJsonArray jsonArray;
    if(solo_tabla_actual){
        jsonArray = getCurrentJsonArrayInTable();
        jsonArrayInTableFiltered = QJsonArray();
        for(int i=0; i< jsonArray.size(); i++){
            QJsonObject jsonObject = jsonArray[i].toObject();
            QStringList header_values = lastSectionFields;
            QString header_value;
            QString value = "";
            foreach(header_value, header_values){
                QString data = jsonObject.value(header_value).toString().trimmed();
                if(checkIfFieldIsValid(data)){
                    value += data + ", ";
                }
            }
            if(!value.trimmed().isEmpty()){
                value = value.trimmed();
                value.remove(value.size()-1, 1);
            }
            if(checkIfFieldIsValid(value) && filterColumnList.contains(value, Qt::CaseInsensitive)){
                jsonArrayInTableFiltered.append(jsonArray[i].toObject());
            }
        }
        jsonInfoItacsAmount = QJsonObject();
        countItacs = jsonArrayInTableFiltered.size();
        currentPage = 1;
    }
    else{
        if(lastSectionFields.size() > 1){
            GlobalFunctions gf(this, empresa);
            QStringList ids, tempList = lastSectionFields;
            tempList.append(id_itac);
            jsonArray = gf.getItacsFields(tempList);
            for(int i=0; i< jsonArray.size(); i++){
                QJsonObject jsonObject = jsonArray[i].toObject();
                QStringList header_values = lastSectionFields;
                QString header_value;
                QString value = "";
                foreach(header_value, header_values){
                    QString data = jsonObject.value(header_value).toString().trimmed();
                    if(checkIfFieldIsValid(data)){
                        value += data + ", ";
                    }
                }
                if(!value.trimmed().isEmpty()){
                    value = value.trimmed();
                    value.remove(value.size()-1, 1);
                }
                if(checkIfFieldIsValid(value) && filterColumnList.contains(value, Qt::CaseInsensitive)){
                    ids << jsonObject.value(id_itac).toString();
                }
            }
            QString queryStatus = getQueyStatus();
            QString query = "(";
            QString id;
            foreach(id, ids){
                if(query == "("){
                    query += " ( (`" + id_itac + "` LIKE " + id + ") ";
                }else{
                    query += " OR (`" + id_itac + "` LIKE " + id + ")";
                }
            }
            if(query != "("){
                query += ") AND ";
            }
            query +=  " (" + queryStatus + ") )";
            getItacsCustomQuery(query);
            jsonArrayInTableFiltered = jsonArrayAll;

        }else{
            QString queryStatus = getQueyStatus();
            QString query = "(";
            QString value;
            foreach(value, filterColumnList){
                if(query == "("){
                    query += " ( (`" + lastSectionFields.first() + "` LIKE '" + value + "') ";
                }else{
                    query += " OR (`" + lastSectionFields.first() + "` LIKE '" + value + "')";
                }
            }
            if(query != "("){
                query += ") AND ";
            }
            query +=  " (" + queryStatus + ") )";
            getItacsCustomQuery(query);
            jsonArrayInTableFiltered = jsonArrayAll;
        }
    }
    filtering = true;
    filterColumnList.clear();
    fixModelForTable(jsonArrayInTableFiltered);
    setTableView();
    hide_loading();
    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
}
QString Screen_Table_ITACs::getScrollBarStyle(){
    QString style =
            "QScrollBar:vertical{"
            "border: 2px #777777;"
            "background-color: solid #777777;"
            "border-radius: 5px;"
            "width: 10px;"
            "margin: 3px 0px 3px 0px;"
            "}"

            "QScrollBar::handle:vertical{"
            "background-color: #777777;"
            "border-radius: 5px;"
            "min-height: 20px;"
            "}"

            "QScrollBar::add-line:vertical{"
            "border: 2px solid white;"
            "background: solid white;"
            "border-radius: 5px;"
            "height 10px;"
            "subcontrol-position: bottom;"
            "subcontrol-origin: margin;"
            "}"

            "QScrollBar::sub-line:vertical{"
            "border: 2px solid white;"
            "background: solid white;"
            "border-radius: 5px;"
            "height 10px;"
            "subcontrol-position: top;"
            "subcontrol-origin: margin;"
            "}";

    return style;
}

void Screen_Table_ITACs::showFilterWidgetOptions(bool offset){

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(5);
    vlayout->setSpacing(0);
    vlayout->setObjectName("v_layout");
    vlayout->setAlignment(Qt::AlignCenter);

    QWidget *widgetValues = new QWidget;
    widgetValues->setStyleSheet("background-color: rgb(77, 77, 77);"
                                "border-radius: 5px;");
    widgetValues->setLayout(vlayout);

    MyLineEditShine *lineEdit = new MyLineEditShine();
    QCheckBox *cb_todos = new QCheckBox();
    cb_todos->setStyleSheet("color: rgb(255, 255, 255);"
                            "background-color: rgba(77, 77, 77);");
    QFont f =  ui->tableView->font();
    f.setPointSize(9);
    cb_todos->setText("Todos");
    cb_todos->setFont(f);

    QCheckBox *cb_tabla_actual = new QCheckBox();
    cb_tabla_actual->setStyleSheet("color: rgb(255, 255, 255);"
                                   "background-color: rgba(77, 77, 77);");
    cb_tabla_actual->setText("Tabla Actual");
    cb_tabla_actual->setFont(f);
    cb_tabla_actual->setChecked(solo_tabla_actual);
    connect(cb_tabla_actual, &QCheckBox::toggled, this, &Screen_Table_ITACs::filtrarEnTabla);

    int itemHeight = 35;
    QStringList values = getFieldValues(lastSectionFields);
    QString value;
    int width = 100;
    foreach(value, values){
        MyCheckBox *cb = new MyCheckBox();
        cb->setText(value);
        cb->setObjectName("cb_"+value);
        cb->setFixedHeight(itemHeight-5);
        cb->setStyleSheet("color: rgb(255, 255, 255);"
                          "background-color: rgba(77, 77, 77);");
        connect(cb, &MyCheckBox::toggleCheckBox, this, &Screen_Table_ITACs::addRemoveFilterList);
        connect(lineEdit, &MyLineEditShine::textChanged, cb, &MyCheckBox::onTextSelectedChanged);
        connect(cb_todos, &QCheckBox::toggled, cb, &MyCheckBox::set_Checked);

        QFont font =  ui->tableView->font();
        font.setPointSize(9);

        cb->setFont(font);

        widgetValues->layout()->addWidget(cb);
        if((value.size() + 1) * 10  > width){
            if((value.size() + 1) * 10 < 500){
                width = (value.size() + 1) * 10;
            }
        }
    }
    QPushButton *button_filter = new QPushButton("FILTRAR  ");
    button_filter->setStyleSheet("color: rgb(255, 255, 255);"
                                 "background-color: rgba(77, 77, 77);");
    button_filter->setIcon(QIcon(":/icons/filter.png"));
    button_filter->setIconSize(QSize(20, 20));
    connect(button_filter, &QPushButton::clicked, this, &Screen_Table_ITACs::filterColumnField);

    QFont font =  ui->tableView->font();
    font.setBold(true);
    font.setPointSize(10);

    button_filter->setFont(font);

    //    widgetValues->setFixedHeight(35 * (values.size() + 1));
    widgetValues->setFixedWidth(width);

    QScrollArea *scroll = new QScrollArea;
    QWidget *widget = new QWidget(this);
    QGraphicsDropShadowEffect* ef = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    ef->setBlurRadius(20);
    ef->setOffset(10);
    widget->setGraphicsEffect(ef);
    widget->setFixedSize((widgetValues->width() > 500)? 580 : widgetValues->width() + 80,
                         (values.size() > 10)? 515: (values.size() * itemHeight + 170));

    lineEdit->setStyleSheet("color: rgb(77, 77, 77);"
                            "background-color: rgb(255, 255, 255);");
    lineEdit->setFixedHeight(20);
    lineEdit->setFixedWidth(widgetValues->width());

    QCompleter *completer = new QCompleter(values, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    lineEdit->setCompleter(completer);

    widget->setStyleSheet("background-color: rgba(77, 77, 77);"
                          "border-radius: 5px;");
    QVBoxLayout *layout = new QVBoxLayout(widget);

    scroll->setWidget(widgetValues);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet(getScrollBarStyle());

    layout->setMargin(15);
    layout->setSpacing(15);

    layout->addWidget(lineEdit);
    layout->addWidget(cb_tabla_actual);
    layout->addWidget(cb_todos);
    layout->addWidget(scroll);
    layout->addWidget(button_filter);

    layout->setAlignment(Qt::AlignCenter);

    if(offset){
        QRect rect = QGuiApplication::screens().first()->geometry();
        if(lastCursorPos.x() > rect.width()/2){
            lastCursorPos.setX(lastCursorPos.x()-widget->width());
        }
    }
    widget->move(lastCursorPos);
    widget->show();

    connect(button_filter, &QPushButton::clicked, widget, &QWidget::hide);
    connect(button_filter, &QPushButton::clicked, widget, &QWidget::deleteLater);
    connect(this, &Screen_Table_ITACs::sectionPressed, widget, &QWidget::deleteLater);
    connect(this, &Screen_Table_ITACs::closing, widget, &QWidget::deleteLater);
    connect(this, &Screen_Table_ITACs::tablePressed, widget, &QWidget::deleteLater);
}

void Screen_Table_ITACs::addRemoveFilterList(QString value){
    if(filterColumnList.contains(value)){
        filterColumnList.removeOne(value);
    }else{
        filterColumnList << value;
    }
}

void Screen_Table_ITACs::getMenuSectionClickedItem(int selection){
    if(selection == RightClickedSection::FILTRAR){
        showFilterWidgetOptions();
    }
    else if(selection == RightClickedSection::ORDENAR_ASCENDENTE){
        if(filtering){
            jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionFields, "");
        }else{
            jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionFields, "");
        }

    }
    else if(selection == RightClickedSection::ORDENAR_DESCENDENTE){
        if(filtering){
            jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionFields, "MAYOR_MENOR");
        }else{
            jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionFields, "MAYOR_MENOR");
        }
    }
}

bool Screen_Table_ITACs::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

void Screen_Table_ITACs::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;

    if(tipo == database_comunication::GET_ITACS)
    {
        //        qDebug()<<"GET_ITACS"<<byte_array;
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_itacs"))
        {
            result = database_comunication::script_result::get_itacs_failed;
        }
        else if(!byte_array.trimmed().isEmpty())
        {
            jsonArrayInTable = database_comunication::getJsonArray(byte_array);
            filtering = false;
            ITAC::writeITACs(jsonArrayInTable);
            serverAlredyAnswered = true;
            emit itacsReceived(tipo);
            //        export_done_tasks_to_excel();
            result = database_comunication::script_result::ok;
        }else{
            qDebug()<<"Tabla de itacas vacia";
        }
    }
    else if(tipo == database_comunication::GET_ALL_COLUMN_VALUES_CUSTOM_QUERY){
        jsonObjectValues = database_comunication::getJsonObject(byte_array);
        result = database_comunication::script_result::ok;
    }
    else if(tipo == database_comunication::GET_ALL_COLUMN_VALUES)
    {
        jsonObjectValues = database_comunication::getJsonObject(byte_array);
        result = database_comunication::script_result::ok;
    }
    else if(tipo == database_comunication::GET_ITACS_AMOUNT)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(byte_array.contains("ot success get_itacs_amount_custom_query"))
        {
            qDebug()<<byte_array;
            result = database_comunication::script_result::get_itacs_amount_failed;
        }
        else {
            jsonInfoItacsAmount = database_comunication::getJsonObject(byte_array);
            qDebug()<<"query return -> "<<jsonInfoItacsAmount.value("query").toString();
            QString count_itacs = jsonInfoItacsAmount.value("count_itacs").toString();
            countItacs = count_itacs.toInt();
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::GET_ITACS_CUSTOM_QUERY)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(byte_array.contains("ot success get_itacs_with_limit_custom_query"))
        {
            qDebug()<<byte_array;
            result = database_comunication::script_result::get_itacs_custom_query_failed;
        }
        else {
            jsonArrayAll = database_comunication::getJsonArray(byte_array);
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::UPDATE_ITAC_FIELDS)
    {
        qDebug()<<byte_array;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(byte_array.contains("ot success update_itac_fields"))
        {
            result = database_comunication::script_result::update_itacs_fields_to_server_failed;
        }
        else
        {
            if(byte_array.contains("success ok update_itac_fields"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_ITACs::ordenarPor(QJsonArray jsonArray, QStringList fields, QString type){ //type  se usa
    QStringList array;
    QString temp;

    for (int j =0; j < jsonArray.size(); j++) {
        QJsonObject jsonObject = jsonArray[j].toObject();
        QStringList header_values = fields;
        QString header_value;
        QString value = "";
        foreach(header_value, header_values){
            QString data = jsonObject.value(header_value).toString().trimmed();
            if(checkIfFieldIsValid(data)){
                value += data + ", ";
            }
        }
        if(!value.trimmed().isEmpty()){
            value = value.trimmed();
            value.remove(value.size()-1, 1);
        }
        temp = value;
        if(!array.contains(temp)){
            array << temp;
        }
    }
    for (int i =0; i < array.size(); i++) {
        for (int j =0; j < array.size(); j++) {
            if(array[j].isEmpty()){
                array[j] = "zzzzzzzzz";
            }
            if(type == "MAYOR_MENOR"){
                if((array[i] > array[j])){
                    temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
            }else{
                if((array[i] < array[j])){
                    temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
            }
        }
    }
    for (int j = 0; j < array.size(); j++) {
        if(array[j] == "zzzzzzzzz"){
            array[j] = "";
        }
    }
    QJsonArray jsonarraySaved = jsonArray;
    jsonArray = QJsonArray();
    for (int j = 0; j < array.size(); j++) {
        for (int i = 0; i < jsonarraySaved.size(); i++) {
            QJsonObject jsonObject = jsonarraySaved[i].toObject();
            QStringList header_values = fields;
            QString header_value;
            QString value = "";
            foreach(header_value, header_values){
                QString data = jsonObject.value(header_value).toString().trimmed();
                if(checkIfFieldIsValid(data)){
                    value += data + ", ";
                }
            }
            if(!value.trimmed().isEmpty()){
                value = value.trimmed();
                value.remove(value.size()-1, 1);
            }
            if(array[j] ==  value){
                jsonArray.append(jsonarraySaved[i].toObject());
            }
        }
    }
    fixModelForTable(jsonArray);
    setTableView();
    return jsonArray;
}
QJsonArray Screen_Table_ITACs::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

    QStringList array;
    QString temp;

    for (int j =0; j < jsonArray.size(); j++) {
        temp = jsonArray[j].toObject().value(field).toString().trimmed().toLower().replace(" ", "");
        if(!array.contains(temp)){
            array << temp;
        }
    }
    for (int i =0; i < array.size(); i++) {
        for (int j =0; j < array.size(); j++) {
            if(array[j].isEmpty()){
                array[j] = "zzzzzzzzz";
            }
            if(type == "MAYOR_MENOR"){
                if((array[i] > array[j])){
                    temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
            }else{
                if((array[i] < array[j])){
                    temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
            }
        }
    }
    for (int j = 0; j < array.size(); j++) {
        if(array[j] == "zzzzzzzzz"){
            array[j] = "";
        }
    }
    QJsonArray jsonarraySaved = jsonArray;
    jsonArray = QJsonArray();
    for (int j = 0; j < array.size(); j++) {
        for (int i = 0; i < jsonarraySaved.size(); i++) {
            if(array[j] ==  jsonarraySaved[i].toObject().value(field).toString().trimmed().toLower().replace(" ", "")){
                jsonArray.append(jsonarraySaved[i].toObject());
            }
        }
    }
    fixModelForTable(jsonArray);
    setTableView();
    return jsonArray;
}
QJsonArray Screen_Table_ITACs::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa

    Q_UNUSED(type);
    QList<int> array;
    int temp;
    for (int j =0; j < jsonArray.size(); j++) {
        temp = jsonArray[j].toObject().value(field).toString().trimmed().toInt();
        if(!array.contains(temp)){
            array.append(temp);
        }
    }
    for (int i =0; i < jsonArray.size(); i++) {
        for (int j =0; j < jsonArray.size(); j++) {
            if(array[i] < array[j]){
                temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }
    QJsonArray jsonarraySaved = jsonArray;
    jsonArray = QJsonArray();
    for (int j = 0; j < array.size(); j++) {
        for (int i = 0; i < jsonarraySaved.size(); i++) {
            if(array[j] ==  jsonarraySaved[i].toObject().value(field).toString().trimmed().toInt()){
                jsonArray.append(jsonarraySaved[i].toObject());
            }
        }
    }
    fixModelForTable(jsonArray);
    setTableView();
    return jsonArray;
}

void Screen_Table_ITACs::on_tableView_doubleClicked(const QModelIndex &index)
{
    abrirItacX(index.row());
}
void Screen_Table_ITACs::on_pb_nueva_clicked()
{
    QJsonObject campos;
    campos.insert(gestor_itacs, gestor);
    oneITACScreen = new ITAC(nullptr, true, empresa, campos);
    connect(oneITACScreen, &ITAC::update_tableITACs,this,
            &Screen_Table_ITACs::updateItacsInTable);
    connect(oneITACScreen, &ITAC::updateTableTareas, this, &Screen_Table_ITACs::update_Table_Tareas);
    connect(oneITACScreen, &ITAC::closing,oneITACScreen, &ITAC::deleteLater);
    QRect rect = QGuiApplication::screens().first()->geometry();
    if(rect.width() <= 1366
            && rect.height() <= 768){
        oneITACScreen->showMaximized();
    }else {
        oneITACScreen->show();
    }
}
void Screen_Table_ITACs::update_Table_Tareas(){
    emit updateTableTareas();
}
void  Screen_Table_ITACs::mousePressEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
{
    Q_UNUSED(e);
    emit mouse_pressed();
}
void Screen_Table_ITACs::mouseReleaseEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
{
    Q_UNUSED(e);
    emit mouse_Release();
}

void Screen_Table_ITACs::on_tableView_pressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    emit tablePressed();
    if(QApplication::mouseButtons()==Qt::RightButton){
        RightClickMenu *rightClickMenu = new RightClickMenu(0, QCursor::pos(), RightClickMenu::FROM_ITACS);
        connect(rightClickMenu, &RightClickMenu::clickPressed, this, &Screen_Table_ITACs::getMenuClickedItem);
        connect(this, &Screen_Table_ITACs::closing, rightClickMenu, &RightClickMenu::deleteLater);
        connect(this, &Screen_Table_ITACs::tablePressed, rightClickMenu, &RightClickMenu::deleteLater);
        connect(this, &Screen_Table_ITACs::mouse_pressed, rightClickMenu, &RightClickMenu::deleteLater);
        rightClickMenu->show();
    }
}
void Screen_Table_ITACs::on_actionMostrarEnMapa(){
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos un ITAC");
        return;
    }
    QJsonArray jsonArray, jsonArrayToShow;
    jsonArray = getCurrentJsonArrayInTable();

    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();

        jsonArrayToShow.append(jsonArray.at(posicion).toObject());
    }

    Mapas_Cercania_Itacs *mapa = new Mapas_Cercania_Itacs(nullptr, jsonArrayToShow, empresa);
    connect(mapa, &Mapas_Cercania_Itacs::openITAC, this, &Screen_Table_ITACs::openItacX);
    connect(mapa, &Mapas_Cercania_Itacs::showJsonArrayInTable,
            this, &Screen_Table_ITACs::setJsonArrayFilterbyPerimeter);
    connect(mapa, &Mapas_Cercania_Itacs::updateITACs,
            this, &Screen_Table_ITACs::update_Table_ITACs);
    mapa->show();
}
void Screen_Table_ITACs::setJsonArrayFilterbyPerimeter(QJsonArray jsonArray){
    jsonArrayInTableFiltered = jsonArray;
    filtering = true;
    filterColumnList.clear();
    fixModelForTable(jsonArrayInTableFiltered);
    setTableView();
}
void Screen_Table_ITACs::update_Table_ITACs(){
    updateItacsInTable();
}
void Screen_Table_ITACs::getMenuClickedItem(int selected)
{
    if(this->isHidden()){
        return;
    }
    if(selected == MOSTRAR_EN_MAPA){
        on_actionMostrarEnMapa();
    }
    else if(selected == DESCARGAR_FOTOS){
        on_actionDescargar_Fotos();
    }
    else if(selected == ASIGNAR_COMUNES){
        on_actionAsignar_campos_triggered();
    }
    else if(selected == ASIGNAR_A_EQUIPO){
        on_actionAsignar_a_un_equipo_triggered();
    }
    else if(selected == ASIGNAR_A_OPERARIO){
        on_actionAsignar_a_un_operario_triggered();
    }
    else if(selected == ELIMINAR){
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
        QString question = "";
        if(selection.isEmpty()){
            GlobalFunctions::showMessage(this,"Sin Selección","Seleccione al menos un ITAC");
            return;
        }else if(selection.size()== 1){
            question = "el ITAC seleccionado?";
        }else{
            question = "los ITACs seleccionados?";
        }
        GlobalFunctions gf(this);
        if(gf.showQuestion(this, "Confirmación", "¿Seguro desea"
                           " eliminar " + question,
                           QMessageBox::Ok, QMessageBox::No)
                == QMessageBox::Ok){
            on_pb_eliminar_clicked();
        }
    }
    else if(selected == ABRIR){
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
        if(!selection.isEmpty() && selection.size()==1){
            QModelIndex index = selection.at(0);
            int posicion = index.row();
            abrirItacX(posicion);
        }else{
            if(selection.size()>1){
                GlobalFunctions::showMessage(this,"Selección múltiple","No puede abrir más de una Itac");
            }else if(selection.isEmpty()){
                GlobalFunctions::showMessage(this,"Sin Selección","Seleccione al menos una Itac");
            }
        }
    }
}
void Screen_Table_ITACs::openItacX(QString cod_emplazamiento){
    GlobalFunctions gf(this, empresa);
    QJsonObject jsonObject = gf.getItacFromServer(cod_emplazamiento);
    if (!jsonObject.isEmpty()) {
        QJsonObject itac = jsonObject;

        ITAC *oneITACScreen = new ITAC(nullptr, false, empresa);
        connect(oneITACScreen, &ITAC::update_tableITACs,this,
                &Screen_Table_ITACs::updateItacsInTable);
        connect(oneITACScreen, &ITAC::updateTableTareas, this,
                &Screen_Table_ITACs::update_Table_Tareas);
        connect(oneITACScreen, &ITAC::closing,oneITACScreen, &ITAC::deleteLater);
        QRect rect = QGuiApplication::screens().first()->geometry();
        if(rect.width() <= 1366
                && rect.height() <= 768){
            oneITACScreen->showMaximized();
        }else {
            oneITACScreen->show();
        }
        oneITACScreen->setData(itac);
        return;
    }
}
void Screen_Table_ITACs::on_actionDescargar_Fotos(){
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;
    int total =  selection.count();

    oneITACScreen = new ITAC(nullptr, false, empresa);
    show_loading("Descargando fotos...");
    for(int i=0; i< total; i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();

        if(jsonArray.size() > posicion){
            o = jsonArray[posicion].toObject();
            oneITACScreen->setData(o, true);
            setLoadingText("Descargando fotos... "
                           "("+QString::number(i+1)+"/" + QString::number(total)+")");
        }
    }
    hide_loading();
    oneITACScreen->deleteLater();
    GlobalFunctions::showMessage(this,"Descargadas","Las fotos fueron descargadas correctamente.");
}

void Screen_Table_ITACs::on_pb_eliminar_clicked(){

    show_loading("Eliminando ITAC(s)...");
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;
    QStringList codigos_itacs;
    bool deleteOk = true;
    oneITACScreen = new ITAC(nullptr, false, empresa);
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();
        if(jsonArray.size() > posicion){
            o = jsonArray[posicion].toObject();
            oneITACScreen->setData(o, false);
            if(!oneITACScreen->eliminarITAC(o.value(codigo_itac_itacs).toString())){
                deleteOk = false;
            }
        }
    }
    oneITACScreen->deleteLater();
    if(deleteOk){
        GlobalFunctions::showMessage(this,"Eliminadas","Itacs eliminadas correctamente.");
        setLoadingText("Actualizando Itacs...");
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","Itacs no fueron eliminadas correctamente");
        setLoadingText("Actualizando Itacs...");
    }
    updateItacsInTable();
    hide_loading();
}
void Screen_Table_ITACs::on_actionAsignar_campos_triggered(){
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una ITAC");
        return;
    }

    Fields_to_Assing_ITACs *fields_screen = new Fields_to_Assing_ITACs(nullptr);

    connect(fields_screen,&Fields_to_Assing_ITACs::fields_selected,
            this,&Screen_Table_ITACs::updateSelectedFields);

    QRect rect = QGuiApplication::screens().first()->geometry();
    if(rect.width() <= 1366
            && rect.height() <= 768){
        fields_screen->showMaximized();
    }else {
        fields_screen->show();
    }
}
void Screen_Table_ITACs::updateSelectedFields(QJsonObject campos){

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;
    QStringList codigos_itacs;
    show_loading("Asignando campos...");
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();

        if(jsonArray.size() > posicion){
            o = jsonArray[posicion].toObject();
            codigos_itacs << o.value(codigo_itac_itacs).toString();
        }
    }

    if(updateITACs(codigos_itacs, campos)){
        hide_loading();
        GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
    }else{
        hide_loading();
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
    }
    updateItacsInTable();
}
void Screen_Table_ITACs::abrirItacX(int index){
    QJsonObject campos;
    campos.insert(gestor_itacs, gestor);
    oneITACScreen = new ITAC(nullptr, false, empresa, campos);
    connect(oneITACScreen, &ITAC::update_tableITACs,this,
            &Screen_Table_ITACs::updateItacsInTable);
    QJsonObject o =  getCurrentJsonArrayInTable().at(index).toObject();
    connect(oneITACScreen, &ITAC::updateTableTareas, this, &Screen_Table_ITACs::update_Table_Tareas);
    connect(oneITACScreen, &ITAC::closing,oneITACScreen, &ITAC::deleteLater);
    QRect rect = QGuiApplication::screens().first()->geometry();
    if(rect.width() <= 1366
            && rect.height() <= 768){
        oneITACScreen->showMaximized();
    }else {
        oneITACScreen->show();
    }
    oneITACScreen->setData(o);
}

void Screen_Table_ITACs::on_actionAsignar_a_un_equipo_triggered(){

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una ITAC");
        return;
    }

    Equipo_Selection_Screen *seleccionEquipoScreen = new Equipo_Selection_Screen(this, empresa);

    seleccionEquipoScreen->getEquipo_OperariosFromServer();
    connect(seleccionEquipoScreen,&Equipo_Selection_Screen::equipoSelected,
            this,&Screen_Table_ITACs::get_equipo_selected);

    seleccionEquipoScreen->moveCenter();

    ui->statusbar->show();

    if(!selection.empty())
    {
        if(seleccionEquipoScreen->exec())
        {
            QJsonArray jsonArray = getCurrentJsonArrayInTable();
            QJsonObject o, campos;
            QStringList codigos_itacs;
            for(int i=0; i< selection.count(); i++)
            {
                QModelIndex index = selection.at(i);
                int posicion = index.row();

                if(jsonArray.size() > posicion){
                    o = jsonArray[posicion].toObject();
                    codigos_itacs << o.value(codigo_itac_itacs).toString();
                }
            }
            campos.insert(equipo_itacs,equipoName);

            if(updateITACs(codigos_itacs, campos)){
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
                ui->statusbar->showMessage("Asignado correctamente");
            }else{
                GlobalFunctions gf(this);
                GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
                ui->statusbar->showMessage("Fallo Asignando");
            }
            disconnect(seleccionEquipoScreen,&Equipo_Selection_Screen::equipoSelected,
                       this,&Screen_Table_ITACs::get_equipo_selected);

            updateItacsInTable();
        }
    }
    ui->statusbar->hide();
}
void Screen_Table_ITACs::get_equipo_selected(QString u)
{
    equipoName = u;
}

void Screen_Table_ITACs::get_user_selected(QString u)
{
    operatorName = u;
}

void Screen_Table_ITACs::on_actionAsignar_a_un_operario_triggered()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una ITAC");
        return;
    }

    Operator_Selection_Screen *seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);

    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,&Operator_Selection_Screen::user,
            this, &Screen_Table_ITACs::get_user_selected);

    seleccionOperarioScreen->moveCenter();

    ui->statusbar->show();

    if(!selection.empty())
    {
        if(seleccionOperarioScreen->exec())
        {
            QJsonArray jsonArray = getCurrentJsonArrayInTable();
            QJsonObject o, campos;
            QStringList codigos_itacs;
            for(int i=0; i< selection.count(); i++)
            {
                QModelIndex index = selection.at(i);
                int posicion = index.row();

                if(jsonArray.size() > posicion){
                    o = jsonArray[posicion].toObject();
                    codigos_itacs << o.value(codigo_itac_itacs).toString();
                }
            }
            campos.insert(operario_itacs,operatorName);

            if(updateITACs(codigos_itacs, campos)){
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
                ui->statusbar->showMessage("Asignado correctamente");
            }else{
                GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
                ui->statusbar->showMessage("Fallo Asignando");
            }
            disconnect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));
            updateItacsInTable();
        }
    }
    ui->statusbar->hide();
}

void Screen_Table_ITACs::moveToPage(QString page){
    show_loading("Cargando  (" + page + " / " + QString::number(currentPages) + ")  ...");
    qDebug()<<"page -> " + page << " -------------------------------------------------";
    currentPage = page.toInt();
    ui->l_current_pagination->setText(page + " / " + QString::number(currentPages));

    int key_id = (page.toInt() - 1)*limit_pagination;
    QString key_id_string = QString::number(key_id);
    if(key_id == 0){
        key_id_string = QString::number(1);
    }
    int id_start = 0;
    if(jsonInfoItacsAmount.contains("id_" + key_id_string)){
        id_start = jsonInfoItacsAmount.value("id_" + key_id_string).toString().toInt();
    }

    QString query = lastQuery;
    getItacsCustomQuery(query, id_start);
    if(filtering){
        jsonArrayInTableFiltered = jsonArrayAll;
    }else{
        jsonArrayInTable = jsonArrayAll;
    }
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    fixModelForTable(jsonArray);
    setTableView();

    checkPaginationButtons();
    hide_loading();
}

void Screen_Table_ITACs::addItemsToPaginationInfo(int sizeShowing){
    QStringList keys = jsonInfoItacsAmount.keys();
    keys.sort();
    int paginas = 0;
    QStringList items;
    for (int i=0; i < keys.size(); i++) {
        QString key = keys.at(i);
        if(key.contains("id_")){
            paginas++;
            qDebug()<< key << " -> "<< jsonInfoItacsAmount.value(key).toString();
            items.append(QString::number(paginas));
        }
    }
    ui->l_current_pagination->addItems(items);
    currentPages = paginas;
    ui->l_current_pagination->setText(QString::number(currentPage) + " / "
                                      + ((currentPages==0)?"1":QString::number(currentPages)));
    ui->l_current_pagination->hideSpinnerList();

    ui->l_cantidad_de_itacs->setText("Mostrando "
                                     + (QString::number(((currentPage - 1) * limit_pagination) + 1) + "-"
                                        + (QString::number(sizeShowing + ((currentPage - 1) * limit_pagination)))
                                        + " de " + QString::number(countItacs))
                                     + " " + ((sizeShowing != 1)?"itacs":"itac"));
    checkPaginationButtons();
}

void Screen_Table_ITACs::checkPaginationButtons(){
    if(currentPage >= currentPages){
        ui->pb_next_pagination->setEnabled(false);
    }else{
        ui->pb_next_pagination->setEnabled(true);
    }
    if(currentPage <= 1){
        ui->pb_previous_pagination->setEnabled(false);
    }else{
        ui->pb_previous_pagination->setEnabled(true);

    }
    if(currentPages <= 1){
        ui->l_current_pagination->set_Enabled(false);
    }
    else{
        ui->l_current_pagination->set_Enabled(true);
    }
}
void Screen_Table_ITACs::on_pb_next_pagination_clicked()
{
    currentPage++;
    moveToPage(QString::number(currentPage));
}

void Screen_Table_ITACs::on_pb_previous_pagination_clicked()
{
    currentPage--;
    moveToPage(QString::number(currentPage));
}


void Screen_Table_ITACs::show_loading(QString mess){
    emit hidingLoading();

    QWidget *widget_blur = new QWidget(this);
    QSize size = this->size();
    size += QSize(0,30);
    widget_blur->move(0,0);
    widget_blur->setFixedSize(size);
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &Screen_Table_ITACs::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &Screen_Table_ITACs::hidingLoading, widget_blur, &QWidget::deleteLater);

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
    connect(this, &Screen_Table_ITACs::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &Screen_Table_ITACs::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &Screen_Table_ITACs::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &Screen_Table_ITACs::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &Screen_Table_ITACs::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}

void Screen_Table_ITACs::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void Screen_Table_ITACs::hide_loading(){
    emit hidingLoading();
}


void Screen_Table_ITACs::on_pb_cruz_clicked()
{
    emit closing();
    model->clear();
    this->close();
}

void Screen_Table_ITACs::on_pb_maximize_clicked()
{
    if(isMaximized()){
        this->showNormal();
    }else {
        this->showMaximized();
    }
}

void Screen_Table_ITACs::on_pb_minimize_clicked()
{
    this->showMinimized();
}

void Screen_Table_ITACs::on_drag_screen(){

    start_moving_screen.stop();
    if(isFullScreen() || isMaximized()){
        return;
    }
    //ui->statusBar->showMessage("Moviendo");
    if(QApplication::mouseButtons()==Qt::LeftButton){

        start_moving_screen.start(10);
        init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
        init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
    }
    else if(QApplication::mouseButtons()==Qt::RightButton){
        on_drag_screen_released();
    }
}

void Screen_Table_ITACs::on_start_moving_screen_timeout(){

    int x_pos = (int)this->pos().x()+((QWidget::mapFromGlobal(QCursor::pos())).x() - init_pos_x);
    int y_pos = (int)this->pos().y()+((QWidget::mapFromGlobal(QCursor::pos())).y() - init_pos_y);
    x_pos = (x_pos < 0)?0:x_pos;
    y_pos = (y_pos < 0)?0:y_pos;

    x_pos = (x_pos > QApplication::desktop()->width()-100)?QApplication::desktop()->width()-100:x_pos;
    y_pos = (y_pos > QApplication::desktop()->height()-180)?QApplication::desktop()->height()-180:y_pos;

    this->move(x_pos,y_pos);

    init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
    init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
}

void Screen_Table_ITACs::on_drag_screen_released()
{
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
}

void Screen_Table_ITACs::on_pb_inicio_clicked()
{
    getITACs();
}

void Screen_Table_ITACs::on_pb_export_data_clicked()
{
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QDir dir;
    dir.setPath(QDir::currentPath() + "/Trabajo Exportado");
    if(!dir.exists()){
        dir.mkpath(QDir::currentPath() + "/Trabajo Exportado");
    }
    QString name_file = "ITACs_Dia.dat" + QDateTime::currentDateTime().toString("yyyy_MM_dd__HH_mm_ss");
    QString filename = dir.path() + "/" + name_file;

    crearFicheroDATConTabla(jsonArray, filename);
    crearFicheroTxtConTabla(jsonArray, filename);

    GlobalFunctions::showMessage(this,"Éxito","Ficheros exportados correctamente");
}
