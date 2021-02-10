#include "screen_tabla_contadores.h"
#include "ui_screen_tabla_contadores.h"
#include <QMap>
#include <QFileDialog>
#include <QFileDialog>
#include <QtXlsx>
#include <QtXlsx/xlsxformat.h>
#include <QMessageBox>
#include "operator_selection_screen.h"
#include "equipo_selection_screen.h"
#include "operario.h"
#include "global_variables.h"
#include "rightclickmenu.h"
#include "fields_to_assing_counters.h"
#include "rightclickedsection.h"
#include "mycheckbox.h"
#include "mylineeditshine.h"
#include <QScrollArea>
#include <QCheckBox>
#include "QProgressIndicator.h"
#include "clase.h"
#include "marca.h"
#include "screen_upload_contadores.h"
#include <QGraphicsDropShadowEffect>
#include "globalfunctions.h"

Screen_tabla_contadores::Screen_tabla_contadores(QWidget *parent, QString empresa) :
    QWidget(parent),
    ui(new Ui::Screen_tabla_contadores)
{
    setWindowFlags(Qt::CustomizeWindowHint);

    this->empresa = empresa;
    ui->setupUi(this);
    model = new QStandardItemModel(this);

    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));

    mapa_contador.insert("número serie de contador",numero_serie_contadores); //Cuidado al aumentar la cantidad de keys, el tamaño del excel esta asiciado a este en funcion parse_to_QjsonArray
    mapa_contador.insert("calibre",calibre_contadores);
    mapa_contador.insert("longitud",longitud_contadores);
    mapa_contador.insert("ruedas",ruedas_contador_contadores);
    mapa_contador.insert("marca",marca_contadores);
    mapa_contador.insert("modelo",modelo_contadores);
    mapa_contador.insert("clase",clase_contadores);
    mapa_contador.insert("tipo de fluido",tipo_fluido_contadores);
    mapa_contador.insert("tipo de radio",tipo_radio_contadores);
    mapa_contador.insert("indice",lectura_inicial_contadores);
    mapa_contador.insert("lectura inicial",lectura_inicial_contadores);
    mapa_contador.insert("lectura",lectura_inicial_contadores);
    mapa_contador.insert("encargado",encargado_contadores);
    mapa_contador.insert("operario",encargado_contadores);
    mapa_contador.insert("equipo",equipo_encargado_contadores);


    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(2);
    ui->pb_load_contadores_from_excel->setGraphicsEffect(effect);

    QGraphicsDropShadowEffect* eff= new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    eff->setBlurRadius(20);
    eff->setOffset(2);
    ui->pb_create_new_contador->setGraphicsEffect(eff);

    connect(ui->l_current_pagination, &MyLabelSpinner::itemSelected,
            this, &Screen_tabla_contadores::moveToPage);
    connect(this, &Screen_tabla_contadores::mouse_pressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Screen_tabla_contadores::sectionPressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Screen_tabla_contadores::closing, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Screen_tabla_contadores::tablePressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
}

Screen_tabla_contadores::~Screen_tabla_contadores()
{
    delete ui;
}

void Screen_tabla_contadores::showEvent(QShowEvent *event){
    QWidget::showEvent(event);
    QTimer::singleShot(200, this, &Screen_tabla_contadores::getContadores);
}

QJsonArray Screen_tabla_contadores::fixJsonToLastModel(QJsonArray jsonArray){ //recibe el arreglo mostrado en la tabla entes de la descarga
    QStringList principal_var_all, principal_var_model;
    for (int i=0; i < jsonArray.size(); i++) { //obteniendo NUMIN de todos los json mostrados
        principal_var_model << jsonArray[i].toObject().value(numero_serie_contadores).toString();
    }
    for (int i=0; i < jsonArrayAll.size(); i++) { //obteniendo NUMIN de todos los json descargados
        principal_var_all << jsonArrayAll[i].toObject().value(numero_serie_contadores).toString();
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
                                            .value(numero_serie_contadores).toString())){
                jsonArray_model.append(jsonArray[i].toObject());
            }
        }
        return jsonArray_model;
    }else {
        return jsonArray;
    }
}

void Screen_tabla_contadores::updateContadoresInTable(){
    if(this->isHidden()){
        return;
    }
    QString query = lastQuery;
    int id_start = last_id_start;
    getContadoresCustomQuery(query, id_start);

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

QString Screen_tabla_contadores::getQueyStatus(){
    QString queryStatus = defaultQuery;
    QString query = " (" + queryStatus + ") ";
    return query;
}


QJsonObject Screen_tabla_contadores::getContadorFrom(QJsonArray jsonArray, QString field, QString value)
{
    for (int j = 0; j < jsonArray.size(); j++) {
        if(jsonArray[j].toObject().value(field).toString().trimmed().contains(value, Qt::CaseInsensitive)){
            return jsonArray[j].toObject();
        }
    }
    return QJsonObject();
}

void Screen_tabla_contadores::get_contadores_amount_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_CONTADORES_AMOUNT,keys,values);
}
void Screen_tabla_contadores::get_contadores_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_CONTADORES_CUSTOM_QUERY,keys,values);
}
void Screen_tabla_contadores::get_all_column_values_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES,keys,values);
}
void Screen_tabla_contadores::get_all_column_values_custom_query_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES_CUSTOM_QUERY,keys,values);
}

bool Screen_tabla_contadores::getContadoresCustomQuery(QString query, int id_start){

    //    if(currentGestor != "Todos"){
    //        query += " AND (" + gestor_contadores + " LIKE '" + currentGestor +"')";
    //    }
    if(lastQuery != query){
        currentPage = 1;
    }
    lastQuery = query;
    last_id_start = id_start;

    getContadoresAmountFromServer(empresa, query, limit_pagination);

    bool res = getContadoresFromServer(empresa, query, limit_pagination, id_start);
    qDebug()<<"query send -> "<< query << " <- id start" << id_start;
    return res;
}
bool Screen_tabla_contadores::getContadoresFromServer(QString empresa, QString query, int limit, int id_start)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT" << "id_start";
    values << empresa.toLower() << query << QString::number(limit) << QString::number(id_start);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_tabla_contadores::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_tabla_contadores::get_contadores_request);

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

    case database_comunication::script_result::get_contadores_custom_query_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}
bool Screen_tabla_contadores::getContadoresAmountFromServer(QString empresa, QString query, int limit)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT";
    values << empresa.toLower() << query << QString::number(limit);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_tabla_contadores::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_tabla_contadores::get_contadores_amount_request);

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

    case database_comunication::script_result::get_contadores_amount_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}

bool Screen_tabla_contadores::getContadoresValuesFieldServer(QString empresa, QString column)
{
    bool res = false;
    QStringList keys, values;

    keys << "empresa" << "columna" << "tabla";
    values << empresa << column << "contadores";

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &Screen_tabla_contadores::script_excecution_result,q,&QEventLoop::exit);

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
bool Screen_tabla_contadores::getContadoresValuesFieldCustomQueryServer(QString empresa, QString column, QString query)
{
    bool res = false;
    QStringList keys, values;

    keys << "empresa" << "columna" << "tabla" << "query";
    values << empresa << column << "contadores" << query;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &Screen_tabla_contadores::script_excecution_result,q,&QEventLoop::exit);

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

void Screen_tabla_contadores::on_drag_screen(){

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

void Screen_tabla_contadores::on_start_moving_screen_timeout(){

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

void Screen_tabla_contadores::on_drag_screen_released()
{
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
}

void Screen_tabla_contadores::on_pb_create_new_contador_clicked()
{
    Counter *contador = new Counter(nullptr, empresa);
    connect(contador, &Counter::updateTablecontadores,
            this,&Screen_tabla_contadores::updateContadoresInTable);
    connect(contador, &Counter::closing,contador, &Counter::deleteLater);
    contador->show();
}

void Screen_tabla_contadores::getContadores()
{
    show_loading("Descargando Contadores...");

    filtering = false;
    currentPage = 1;
    QString query = defaultQuery;
    getContadoresCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;
    populateTable(database_comunication::GET_CONTADORES);

    hide_loading();
}

void Screen_tabla_contadores::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo){
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_CONTADORES)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_administradores"))
        {
            result = database_comunication::script_result::get_contadores_failed;
        }
        else
        {
            filtering = false;
            jsonArrayInTable = database_comunication::getJsonArray(byte_array);
            Counter::writeCounters(jsonArrayInTable);
            serverAlredyAnswered = true;
            emit contadoresReceived(tipo);
            result = database_comunication::script_result::ok;
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
    else if(tipo == database_comunication::GET_CONTADORES_AMOUNT)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(byte_array.contains("ot success get_contadores_amount_custom_query"))
        {
            qDebug()<<byte_array;
            result = database_comunication::script_result::get_contadores_amount_failed;
        }
        else {
            jsonInfoContadoresAmount = database_comunication::getJsonObject(byte_array);
            qDebug()<<"query return -> "<<jsonInfoContadoresAmount.value("query").toString();
            QString count_contadores = jsonInfoContadoresAmount.value("count_contadores").toString();
            countContadores = count_contadores.toInt();
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::GET_CONTADORES_CUSTOM_QUERY)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(byte_array.contains("ot success get_contadores_with_limit_custom_query"))
        {
            qDebug()<<byte_array;
            result = database_comunication::script_result::get_contadores_custom_query_failed;
        }
        else {
            jsonArrayAll = database_comunication::getJsonArray(byte_array);
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::UPDATE_CONTADORES_FIELDS)
    {
        qDebug()<<byte_array;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(byte_array.contains("ot success update_contadores_fields"))
        {
            result = database_comunication::script_result::update_contadores_fields_to_server_failed;
        }
        else
        {
            if(byte_array.contains("success ok update_contadores_fields"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_CONTADORES){
        qDebug()<<byte_array;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(byte_array.contains("ot success delete_contadores"))
        {
            result = database_comunication::script_result::delete_contadores_fields_to_server_failed;
        }
        else
        {
            if(byte_array.contains("success ok delete_contadores"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    emit script_excecution_result(result);
}
void Screen_tabla_contadores::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
    Q_UNUSED(tipo);
    jsonArrayInTable = ordenarPor(jsonArrayInTable, numero_serie_contadores, "");
    emit filledContadores();
}

QJsonArray Screen_tabla_contadores::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_tabla_contadores::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa
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
QMap <QString,QString> Screen_tabla_contadores::fillMapForFixModel(QStringList &listHeaders){
    QMap <QString,QString> mapa;
    mapa.insert("Numero serie", numero_serie_contadores);
    mapa.insert("Año o prefijo", anno_o_prefijo_contadores);
    mapa.insert("Calibre",calibre_contadores);
    mapa.insert("Longitud",longitud_contadores);
    mapa.insert("Ruedas",ruedas_contador_contadores);
    mapa.insert("C.Marca",codigo_marca_contadores);
    mapa.insert("Marca",marca_contadores);
    mapa.insert("Modelo",modelo_contadores);
    mapa.insert("C.Clase",codigo_clase_contadores);
    mapa.insert("Clase",clase_contadores);
    mapa.insert("T.Fluido",tipo_fluido_contadores);
    mapa.insert("T.Radio",tipo_radio_contadores);
    mapa.insert("Lectura",lectura_inicial_contadores);
    mapa.insert("Encargado",encargado_contadores);
    mapa.insert("Estado",status_contadores);

    listHeaders << "Numero serie" << "Año o prefijo" << "Calibre"
                << "Longitud" << "Ruedas" << "C.Marca"<< "Marca"
                << "Modelo" << "C.Clase"<< "Clase"  << "T.Fluido"
                << "T.Radio" << "Lectura" << "Encargado" << "Estado";

    return mapa;
}
void Screen_tabla_contadores::fixModelForTable(QJsonArray jsonArray){
    int rows = jsonArray.count();
    addItemsToPaginationInfo(rows);
    //comprobando que no exista un modelo anterior
    model->clear();

    QStringList listHeaders;
    QMap <QString,QString> mapa = fillMapForFixModel(listHeaders);

    model = new QStandardItemModel(rows, listHeaders.size());
    model->setHorizontalHeaderLabels(listHeaders);

    //insertando los datos0
    QStandardItem *item;
    for(int i = 0; i < rows; i++)
    {
        for (int n = 0; n < listHeaders.size(); n++) {
            item = new QStandardItem();
            QString key = mapa.value(listHeaders.at(n));
            QString value = jsonArray.at(i).toObject().value(key).toString();
            //            if(key.contains(numero_serie_contadores)){ //Para quitar el prefijo del numero de serie
            //                QString prefijo = jsonArray.at(i).toObject().value(anno_o_prefijo_contadores).toString();
            //                if(value != prefijo){
            //                    if(!prefijo.isEmpty() &&
            //                        (value.mid(0, prefijo.size()) == prefijo)){
            //                         value = value.remove(0, prefijo.size()).trimmed();
            //                    }
            //                }
            //            }
            if(!checkIfFieldIsValid(value)){
                value ="";
            }
            item->setData(value ,Qt::EditRole);
            model->setItem(i, n, item);
        }
    }
}

void Screen_tabla_contadores::setTableView()
{
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //    QItemSelectionModel * selectionModel = new QItemSelectionModel(QItemSelectionModel::Rows);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    int fields_count_in_table = ui->tableView->horizontalHeader()->count();
    int width_table = ui->tableView->size().width() - 150;
    float medium_width_fileds = (float)width_table/fields_count_in_table;

    for (int i=0; i< fields_count_in_table; i++) {
        ui->tableView->setColumnWidth(i, (int)(medium_width_fileds));
    }
    ui->tableView->setColumnWidth(0, medium_width_fileds*2);
    //    ui->tableView->setColumnWidth(2, (int)(medium_width_fileds * 0.66));//Ancho de
    //    ui->tableView->setColumnWidth(3, (int)(medium_width_fileds * 0.60));//Ancho de Prefijo
    //    ui->tableView->setColumnWidth(4, (int)(medium_width_fileds * 1.32));//Ancho de marca
    //    ui->tableView->setColumnWidth(6, (int)(medium_width_fileds * 0.49));
    //    ui->tableView->setColumnWidth(7, (int)(medium_width_fileds * 0.99));
    //    ui->tableView->setColumnWidth(8, (int)(medium_width_fileds * 1.33));
    //    ui->tableView->setColumnWidth(9, (int)(medium_width_fileds * 0.82));
    //    ui->tableView->setColumnWidth(10, (int)(medium_width_fileds *1.66));

    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setFont(ui->tableView->font());

    if(!connected_header_signal){
        connected_header_signal = true;
        connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),this, SLOT(on_sectionClicked(int)));
    }
}

void Screen_tabla_contadores::on_sectionClicked(int logicalIndex)
{
    emit sectionPressed();
    filterColumnList.clear();
    lastCursorPos = this->mapFromGlobal(QCursor::pos());

    disconnect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));

    QStringList listHeaders;
    QMap <QString,QString> mapa = fillMapForFixModel(listHeaders);

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);
    lastSectionCliked = columna;
    lastSectionField = ordenamiento;

    RightClickedSection *rightClickedSection = new RightClickedSection(0, QCursor::pos());
    connect(rightClickedSection, SIGNAL(clickPressed(int)), this, SLOT(getMenuSectionClickedItem(int)));
    connect(this, &Screen_tabla_contadores::closing, rightClickedSection, &RightClickedSection::deleteLater);
    connect(this, &Screen_tabla_contadores::tablePressed, rightClickedSection, &RightClickedSection::deleteLater);
    connect(this, &Screen_tabla_contadores::mouse_pressed, rightClickedSection, &RightClickedSection::deleteLater);

    rightClickedSection->show();
}
QJsonArray Screen_tabla_contadores::getCurrentJsonArrayInTable(){
    QJsonArray jsonArray;

    if(filtering){
        jsonArray = jsonArrayInTableFiltered;
    }else{
        jsonArray = jsonArrayInTable;
    }
    return jsonArray;
}
void Screen_tabla_contadores::filtrarEnTabla(bool checked){
    solo_tabla_actual = checked;
    emit sectionPressed();

    showFilterWidgetOptions(false);
}
QStringList Screen_tabla_contadores::getFieldValues(QString field){

    QStringList values;
    QJsonArray jsonArray;
    if(solo_tabla_actual){
        jsonArray = getCurrentJsonArrayInTable();
        QString value;
        for (int i=0; i < jsonArray.size(); i++) {
            value = jsonArray.at(i).toObject().value(field).toString();
            if(checkIfFieldIsValid(value)){
                if(!values.contains(value)){
                    values << value;
                }
            }else if(field == status_contadores){
                if(!values.contains("DISPONIBLE")){
                    values << "DISPONIBLE";
                }
            }
        }
    }else{
        show_loading("Cargando valores...");
        QString queryStatus = getQueyStatus();
        QString query = " (" + queryStatus + ") ";
        bool res = getContadoresValuesFieldCustomQueryServer(
                    empresa, field, query);
        if(res){
            QStringList keys = jsonObjectValues.keys();
            for (int i= 0; i < keys.size(); i++) {
                QString key = keys.at(i);
                if(!key.contains("query") && !key.contains("count_values")){
                    QString value = jsonObjectValues.value(key).toString();
                    if(!values.contains(value, Qt::CaseInsensitive) && checkIfFieldIsValid(value)){
                        values << value;
                    }
                }
            }
        }
        hide_loading();
    }
    values.sort();
    return values;
}
void Screen_tabla_contadores::filterColumnField(){

    if(filterColumnList.isEmpty()){
        return;
    }
    show_loading("Buscando Resultados...");
    QJsonArray jsonArray;
    if(solo_tabla_actual){
        jsonArray = getCurrentJsonArrayInTable();
        jsonArrayInTableFiltered = QJsonArray();

        for(int i=0; i< jsonArray.size(); i++){
            QString value = jsonArray[i].toObject().value(lastSectionField).toString().trimmed();
            if(lastSectionField == status_contadores){
                if(!checkIfFieldIsValid(value) && filterColumnList.contains("DISPONIBLE")){
                    jsonArrayInTableFiltered.append(jsonArray[i].toObject());
                    continue;
                }
            }
            if(checkIfFieldIsValid(value) && filterColumnList.contains(value, Qt::CaseInsensitive)){
                jsonArrayInTableFiltered.append(jsonArray[i].toObject());
            }
        }
        jsonInfoContadoresAmount = QJsonObject();
        countContadores = jsonArrayInTableFiltered.size();
        currentPage = 1;
    }else{
        QString queryStatus = getQueyStatus();
        QString query = "(";
        QString value;
        foreach(value, filterColumnList){
            if(query == "("){
                query += " ( (`" + lastSectionField + "` LIKE '" + value + "%') ";
            }else{
                query += " OR (`" + lastSectionField + "` LIKE '" + value + "%')";
            }
        }
        if(query != "("){
            query += ") AND ";
        }
        query +=  " (" + queryStatus + ") )";
        getContadoresCustomQuery(query);
        jsonArrayInTableFiltered = jsonArrayAll;
    }

    filtering = true;
    filterColumnList.clear();
    fixModelForTable(jsonArrayInTableFiltered);
    setTableView();
    hide_loading();
    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
}
QString Screen_tabla_contadores::getScrollBarStyle(){
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

void Screen_tabla_contadores::showFilterWidgetOptions(bool offset){

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
    connect(cb_tabla_actual, &QCheckBox::toggled, this, &Screen_tabla_contadores::filtrarEnTabla);

    int itemHeight = 35;
    QStringList values = getFieldValues(lastSectionField);
    QString value;
    int width = 100;
    foreach(value, values){
        MyCheckBox *cb = new MyCheckBox();
        cb->setText(value);
        cb->setObjectName("cb_"+value);
        cb->setFixedHeight(itemHeight-5);
        cb->setStyleSheet("color: rgb(255, 255, 255);"
                          "background-color: rgba(77, 77, 77);");
        connect(cb, &MyCheckBox::toggleCheckBox, this, &Screen_tabla_contadores::addRemoveFilterList);
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
    connect(button_filter, &QPushButton::clicked, this, &Screen_tabla_contadores::filterColumnField);

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

    connect(button_filter, &QPushButton::clicked, widget, &QWidget::deleteLater);
    connect(this, &Screen_tabla_contadores::sectionPressed, widget, &QWidget::deleteLater);
    connect(this, &Screen_tabla_contadores::closing, widget, &QWidget::deleteLater);
    connect(this, &Screen_tabla_contadores::tablePressed, widget, &QWidget::deleteLater);
}

void Screen_tabla_contadores::addRemoveFilterList(QString value){
    if(filterColumnList.contains(value)){
        filterColumnList.removeOne(value);
    }else{
        filterColumnList << value;
    }
}

void Screen_tabla_contadores::getMenuSectionClickedItem(int selection){
    if(selection == RightClickedSection::FILTRAR){
        showFilterWidgetOptions();
    }
    else if(selection == RightClickedSection::ORDENAR_ASCENDENTE){
        if(lastSectionField != lectura_inicial_contadores){
            if(filtering){
                jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionField, "");
            }else{
                jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionField, "");
            }
        }else{
            if(filtering){
                jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionField, 0);
            }else{
                jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionField, 0);
            }
        }

    }
    else if(selection == RightClickedSection::ORDENAR_DESCENDENTE){
        if((lastSectionField != lectura_inicial_contadores)){
            if(filtering){
                jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionField, "MAYOR_MENOR");
            }else{
                jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionField, "MAYOR_MENOR");
            }
        }else{
            if(filtering){
                jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionField, 1);
            }else{
                jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionField, 1);
            }
        }
    }
}


bool Screen_tabla_contadores::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

void Screen_tabla_contadores::on_tableView_doubleClicked(const QModelIndex &index)
{
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o = jsonArray[index.row()].toObject();

    openContadorX(o);
}

void Screen_tabla_contadores::openContadorX(QJsonObject o){

    Counter *contador = new Counter(nullptr, empresa);

    contador->setOperariosDisponibles(Operario::getListaUsuarios());
    contador->populateView(o);
    contador->show();
    connect(contador, &Counter::updateTablecontadores,
            this,&Screen_tabla_contadores::updateContadoresInTable);
}

void Screen_tabla_contadores::show_loading(QString mess){
    emit hidingLoading();

    QWidget *widget_blur = new QWidget(this);
    widget_blur->setFixedSize(this->size()+QSize(0,0));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &Screen_tabla_contadores::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &Screen_tabla_contadores::hidingLoading, widget_blur, &QWidget::deleteLater);

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
    connect(this, &Screen_tabla_contadores::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &Screen_tabla_contadores::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &Screen_tabla_contadores::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &Screen_tabla_contadores::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &Screen_tabla_contadores::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}

void Screen_tabla_contadores::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void Screen_tabla_contadores::hide_loading(){
    emit hidingLoading();
}

void Screen_tabla_contadores::on_pb_load_contadores_from_excel_clicked()
{
    Screen_Upload_Contadores *uploadScreen = new Screen_Upload_Contadores(nullptr, empresa);
    connect(uploadScreen, &Screen_Upload_Contadores::updateContadores,
            this, &Screen_tabla_contadores::updateContadoresInTable);
    uploadScreen->show();
}

QJsonArray Screen_tabla_contadores::parse_to_QjsonArray(QString rutaToDATFile){
    QXlsx::Document xlsx(rutaToDATFile);
    QJsonArray jsonArray, jsonArrayToReturn;
    QStringList listHeaders, row_content;
    QList<QStringList> lista;
    //        QFile file(path);
    //        file.open(QIODevice::ReadOnly);
    int number_of_row = xlsx.dimension().lastRow();
    int number_of_column = xlsx.dimension().lastColumn();

    //read headers
    ///Limitar el numero de headers 26 es la cantidad de columnas actual del excel
    if(number_of_column > mapa_contador.size()-2){
        number_of_column = mapa_contador.size()-2;
    }
    for (int i=1; i<=number_of_column; i++)
    {
        listHeaders << xlsx.cellAt(1,i)->value().toString().trimmed().toLower();
    }

    //read data
    for(int i = 3; i <= number_of_row; i++)
    {
        row_content.clear();
        for(int j = 1; j <= number_of_column; j++)
        {
            if(xlsx.cellAt(i,j) != 0)
            {
                row_content << xlsx.cellAt(i,j)->value().toString().trimmed();
            }
            else
            {
                row_content << "";

            }
        }
        lista.insert(i-3, row_content);
    }

    //OJO debo hacer una comprobación de los encabezados
    //-2 por los encabezados y fila de filtros y -1 por la fila de resumen al final
    for(int i=0; i < number_of_row-2; i++)
    {
        QJsonObject o;
        row_content = lista.at(i);
        if(row_content.at(0).trimmed().isEmpty() && row_content.at(1).trimmed().isEmpty()){
            continue;
        }
        for(int j = 0; j < number_of_column; j++)
        {
            QString campo_en_excel = listHeaders.at(j);
            QString campo_en_json = mapa_contador.value(listHeaders.at(j));
            QString valor = row_content.at(j);
            o.insert(campo_en_json, valor);
        }
        o.remove("");

        QMap<QString, QString> clases;
        jsonArray = Clase::readClases();
        for (int i=0; i < jsonArray.size(); i++) {
            QString clase_v, cod_v;
            clase_v = jsonArray.at(i).toObject().value(clase_clases).toString();
            cod_v = jsonArray.at(i).toObject().value(codigo_clase_clases).toString();
            clases.insert(cod_v, clase_v);
        }

        QMap<QString, QString> marcas;
        jsonArray = Marca::readMarcas();
        for (int i=0; i < jsonArray.size(); i++) {
            QString marca_v, modelo_v, cod_v;
            marca_v = jsonArray.at(i).toObject().value(marca_marcas).toString();
            modelo_v = jsonArray.at(i).toObject().value(modelo_marcas).toString();
            cod_v = jsonArray.at(i).toObject().value(codigo_marca_marcas).toString();

            marcas.insert(cod_v, marca_v + " - " + modelo_v);
        }


        for (int i=0; i < marcas.size(); i++) {
            QString marcas_x = marcas.values().at(i).toLower();
            QString marca_o = o.value(marca_contadores).toString().toLower();
            QString modelo_o = o.value(modelo_contadores).toString().toLower();
            if(marcas_x.contains(marca_o)
                    && marcas_x.contains(modelo_o)){
                QString key = marcas.keys().at(i);
                o.insert(codigo_marca_contadores, key);
                break;
            }
        }
        QString clase_o = o.value(clase_contadores).toString().toLower().replace(" ", "");
        for (int i=0; i < clases.size(); i++) {
            QString clases_i = clases.values().at(i).toLower().replace(" ", "");
            if(clases_i.contains(clase_o)){
                QString key = clases.keys().at(i);
                o.insert(codigo_clase_contadores,key);
                break;
            }
        }
        QString prefijo = o.value(numero_serie_contadores).toString();
        while(prefijo.contains("  ")){
            prefijo = prefijo.replace("  ", " ");
        }
        if(prefijo.split(" ").size() >= 2){
            prefijo = prefijo.split(" ").at(0);
        }
        o.insert(anno_o_prefijo_contadores, prefijo);
        jsonArrayToReturn.insert(i,o);
    }

    return jsonArrayToReturn;
}

void Screen_tabla_contadores::on_pb_asignar_operario_clicked()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos un contador");
        return;
    }
    Operator_Selection_Screen *seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);
    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));

    QJsonArray jsonArray = getCurrentJsonArrayInTable();

    int total = selection.count();
    if(seleccionOperarioScreen->exec())
    {

        Counter *contador = new Counter(nullptr, empresa);

        for(int i=0, reintentos = 0; i< selection.count(); i++)
        {
            show_loading("Espere, actualizando contadores... ("
                         +QString::number(i)+"/"+QString::number(total)+")");

            QModelIndex index = selection.at(i);
            int posicion = index.row();

            if(jsonArray.size() > posicion){

                QJsonObject o = jsonArray[posicion].toObject();

                //        screen_get_one_tarea *one_tarea_screen = new screen_get_one_tarea();
                QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
                o.insert(date_time_modified_contadores, timestamp);
                o.insert(encargado_contadores, operatorName);

                QStringList keys, values;
                QJsonDocument d;
                d.setObject(o);
                QByteArray ba = d.toJson(QJsonDocument::Compact);
                keys << "json" << "empresa";
                QString temp = QString::fromUtf8(ba);
                values << temp << empresa.toLower();

                contador->populateFixView(o);

                QEventLoop *q = new QEventLoop();

                connect(contador, &Counter::script_excecution_result,q,&QEventLoop::exit);

                contador->update_contador_request(keys, values);

                switch(q->exec())
                {
                case database_comunication::script_result::timeout:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = jsonArray.size();
                    }
                    break;

                case database_comunication::script_result::contador_to_server_ok:
                    reintentos = 0;
                    break;

                case database_comunication::script_result::update_contador_to_server_failed:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = jsonArray.size();
                    }
                    break;

                case database_comunication::script_result::create_contador_to_server_failed:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = jsonArray.size();
                    }
                    break;
                }

                delete q;
            }
        }
        hide_loading();
        updateContadoresInTable();
    }

}
void Screen_tabla_contadores::delete_contadores_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_CONTADORES,keys,values);
}
void Screen_tabla_contadores::update_contadores_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_CONTADORES_FIELDS,keys,values);
}
bool Screen_tabla_contadores::updateContadores(QStringList lista_numeros_serie, QJsonObject campos){

    QJsonObject numeros_serie;

    for (int i=0; i < lista_numeros_serie.size(); i++) {
        numeros_serie.insert(QString::number(i), lista_numeros_serie.at(i));
    }
    if(numeros_serie.isEmpty()){
        return true;
    }
    campos.insert(date_time_modified_contadores, QDateTime::currentDateTime().toString(formato_fecha_hora));

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(numeros_serie);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp_fields, temp_numeros_serie = QString::fromUtf8(ba);

    d.setObject(campos);
    ba = d.toJson(QJsonDocument::Compact);
    temp_fields = QString::fromUtf8(ba);

    keys << "json_numeros_serie" << "json_fields" << "empresa";
    values << temp_numeros_serie << temp_fields << empresa.toLower();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_tabla_contadores::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_tabla_contadores::update_contadores_fields_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::update_contadores_fields_to_server_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}

void Screen_tabla_contadores::on_tableView_pressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    emit tablePressed();
    if(QApplication::mouseButtons()==Qt::RightButton){
        RightClickMenu *rightClickMenu = new RightClickMenu(0, QCursor::pos(), RightClickMenu::FROM_CONTADORES);
        connect(rightClickMenu, &RightClickMenu::clickPressed, this, &Screen_tabla_contadores::getMenuClickedItem);
        connect(this, &Screen_tabla_contadores::closing, rightClickMenu, &RightClickMenu::deleteLater);
        connect(this, &Screen_tabla_contadores::tablePressed, rightClickMenu, &RightClickMenu::deleteLater);
        connect(this, &Screen_tabla_contadores::mouse_pressed, rightClickMenu, &RightClickMenu::deleteLater);
        rightClickMenu->show();
    }
}
void Screen_tabla_contadores::getMenuClickedItem(int selected)
{
    if(this->isHidden()){
        return;
    }
    else if(selected == ASIGNAR_COMUNES){
        show_loading("Asignando campos...");
        on_actionAsignar_campos_triggered();
        hide_loading();
    }
    else if(selected == ASIGNAR_A_EQUIPO){
        show_loading("Asignando equipo...");
        on_actionAsignar_a_un_equipo_triggered();
        hide_loading();
    }
    else if(selected == ASIGNAR_A_OPERARIO){
        show_loading("Asignando fontanero...");
        on_actionAsignar_a_un_operario_triggered();
        hide_loading();
    }
    else if(selected == ELIMINAR){
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
        QString question = "";
        if(selection.isEmpty()){
            GlobalFunctions::showMessage(this,"Sin selección","Seleccione al menos un contador");
            return;
        }else if(selection.size()== 1){
            question = "el contador seleccionado?";
        }else{
            question = "los contadores seleccionados?";
        }
        GlobalFunctions gf(this);
        if(gf.showQuestion(this, "Confirmación", "¿Seguro desea"
                           " eliminar "+question,
                           QMessageBox::Ok, QMessageBox::No)
                == QMessageBox::Ok){
            on_pb_eliminar_clicked();
        }
    }
    else if(selected == ABRIR){
        QJsonArray jsonArray = getCurrentJsonArrayInTable();
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
        if(!selection.isEmpty() && selection.size()==1){
            QModelIndex index = selection.at(0);
            int posicion = index.row();
            QJsonObject o = jsonArray[posicion].toObject();
            openContadorX(o);
        }else{
            if(selection.size()>1){
                GlobalFunctions::showMessage(this,"Selección múltiple","Seleccione solo un contador para abrir");
            }else if(selection.isEmpty()){
                GlobalFunctions::showMessage(this,"Sin selección","Seleccione un contador para abrir");
            }
        }
    }
}
void Screen_tabla_contadores::on_pb_eliminar_clicked(){
    show_loading("Eliminando contadores...");
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;
    QStringList numeros_serie;
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();

        if(jsonArray.size() > posicion){
            o = jsonArray[posicion].toObject();
            numeros_serie << o.value(numero_serie_contadores).toString();
        }
    }
    if(deleteContadores(numeros_serie)){
        GlobalFunctions::showMessage(this,"Éxito","Los contadores fueron eliminados satisfactoriamente.");
        setLoadingText("Actualizando contadores...");
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","Los contadores no fueron eliminados en el servidor.");
        setLoadingText("Actualizando contadores...");
    }
    updateContadoresInTable();
    hide_loading();
}
bool Screen_tabla_contadores::deleteContadores(QStringList lista_numeros_serie){

    QJsonObject numeros_serie;

    for (int i=0; i < lista_numeros_serie.size(); i++) {
        numeros_serie.insert(QString::number(i), lista_numeros_serie.at(i));
    }
    if(numeros_serie.isEmpty()){
        return true;
    }
    QStringList keys, values;
    QJsonDocument d;
    d.setObject(numeros_serie);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp_numeros_serie = QString::fromUtf8(ba);

    keys << "json_numeros_serie" << "empresa";
    values << temp_numeros_serie << empresa.toLower();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_tabla_contadores::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_tabla_contadores::delete_contadores_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::update_contadores_fields_to_server_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}
void Screen_tabla_contadores::on_actionAsignar_campos_triggered(){
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos un contador");
        return;
    }

    Fields_to_Assing_Counters *fields_screen = new Fields_to_Assing_Counters(nullptr);

    connect(fields_screen,&Fields_to_Assing_Counters::fields_selected,
            this,&Screen_tabla_contadores::updateSelectedFields);

    fields_screen->show();
}

void Screen_tabla_contadores::updateSelectedFields(QJsonObject campos){

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;
    QStringList numeros_serie;
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();

        if(jsonArray.size() > posicion){
            o = jsonArray[posicion].toObject();
            numeros_serie << o.value(numero_serie_contadores).toString();
        }
    }

    if(updateContadores(numeros_serie, campos)){
        GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
    }
    updateContadoresInTable();
}

void Screen_tabla_contadores::on_actionAsignar_a_un_equipo_triggered(){

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos un contador");
        return;
    }

    Equipo_Selection_Screen *seleccionEquipoScreen = new Equipo_Selection_Screen(this, empresa);

    seleccionEquipoScreen->getEquipo_OperariosFromServer();
    connect(seleccionEquipoScreen,&Equipo_Selection_Screen::equipoSelected,
            this,&Screen_tabla_contadores::get_equipo_selected);

    seleccionEquipoScreen->moveCenter();

    if(!selection.empty())
    {
        if(seleccionEquipoScreen->exec())
        {
            QJsonArray jsonArray = getCurrentJsonArrayInTable();
            QJsonObject o, campos;
            QStringList numeros_serie;
            for(int i=0; i< selection.count(); i++)
            {
                QModelIndex index = selection.at(i);
                int posicion = index.row();

                if(jsonArray.size() > posicion){
                    o = jsonArray[posicion].toObject();
                    numeros_serie << o.value(numero_serie_contadores).toString();
                }
            }
            campos.insert(equipo_encargado_contadores,equipoName);

            if(updateContadores(numeros_serie, campos)){
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
            }else{
                GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
            }
            disconnect(seleccionEquipoScreen,&Equipo_Selection_Screen::equipoSelected,
                       this,&Screen_tabla_contadores::get_equipo_selected);

            updateContadoresInTable();
        }
    }
}
void Screen_tabla_contadores::get_equipo_selected(QString u)
{
    equipoName = u;
}

void Screen_tabla_contadores::get_user_selected(QString u)
{
    operatorName = u;
}

void Screen_tabla_contadores::on_actionAsignar_a_un_operario_triggered()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos un contador");
        return;
    }

    Operator_Selection_Screen *seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);

    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,&Operator_Selection_Screen::user,
            this, &Screen_tabla_contadores::get_user_selected);

    seleccionOperarioScreen->moveCenter();

    if(!selection.empty())
    {
        if(seleccionOperarioScreen->exec())
        {
            QJsonArray jsonArray = getCurrentJsonArrayInTable();
            QJsonObject o, campos;
            QStringList numeros_serie;
            for(int i=0; i< selection.count(); i++)
            {
                QModelIndex index = selection.at(i);
                int posicion = index.row();

                if(jsonArray.size() > posicion){
                    o = jsonArray[posicion].toObject();
                    numeros_serie << o.value(numero_serie_contadores).toString();
                }
            }
            campos.insert(encargado_contadores,operatorName);

            if(updateContadores(numeros_serie, campos)){
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
            }else{
                GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
            }
            disconnect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));
            updateContadoresInTable();
        }
    }
}

void Screen_tabla_contadores::on_pb_cruz_clicked()
{
    emit closing();
    model->clear();
    this->close();
}

void Screen_tabla_contadores::on_pb_maximize_clicked()
{
    if(isMaximized()){
        this->showNormal();
    }else {
        this->showMaximized();
    }
    QTimer::singleShot(200, this, SLOT(setTableView()));
}

void Screen_tabla_contadores::on_pb_minimize_clicked()
{
    this->showMinimized();
}


void Screen_tabla_contadores::moveToPage(QString page){
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
    if(jsonInfoContadoresAmount.contains("id_" + key_id_string)){
        id_start = jsonInfoContadoresAmount.value("id_" + key_id_string).toString().toInt();
    }

    QString query = lastQuery;
    getContadoresCustomQuery(query, id_start);
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

void Screen_tabla_contadores::addItemsToPaginationInfo(int sizeShowing){
    QStringList keys = jsonInfoContadoresAmount.keys();
    keys.sort();
    int paginas = 0;
    QStringList items;
    for (int i=0; i < keys.size(); i++) {
        QString key = keys.at(i);
        if(key.contains("id_")){
            paginas++;
            items.append(QString::number(paginas));
        }
    }
    ui->l_current_pagination->addItems(items);
    currentPages = paginas;
    ui->l_current_pagination->setText(QString::number(currentPage) + " / "
                                      + ((currentPages==0)?"1":QString::number(currentPages)));
    ui->l_current_pagination->hideSpinnerList();

    ui->l_cantidad_de_contadores->setText("Mostrando "
                                          + (QString::number(((currentPage - 1) * limit_pagination) + 1) + "-"
                                             + (QString::number(sizeShowing + ((currentPage - 1) * limit_pagination)))
                                             + " de " + QString::number(countContadores))
                                          + " " + ((sizeShowing != 1)?"contadores":"contador"));
    checkPaginationButtons();
}

void Screen_tabla_contadores::checkPaginationButtons(){
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
void Screen_tabla_contadores::on_pb_next_pagination_clicked()
{
    currentPage++;
    moveToPage(QString::number(currentPage));
}

void Screen_tabla_contadores::on_pb_previous_pagination_clicked()
{
    currentPage--;
    moveToPage(QString::number(currentPage));
}

void Screen_tabla_contadores::on_pb_inicio_clicked()
{
    getContadores();
}
