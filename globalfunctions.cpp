#include "globalfunctions.h"
#include "structure_itac.h"
#include "structure_contador.h"
#include "structure_ruta.h"
#include "new_table_structure.h"
#include "mylabelanimated.h"
#include "mylabelshine.h"
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QDesktopServices>
//#include <QScriptEngine>
//#include <QScriptContext>
//#include <QScriptValue>

QString GlobalFunctions::gestor_selected_file = "BD/gestor_selected.dat";
QString GlobalFunctions::empresa_selected_file = "BD/empresa_selected.dat";

GlobalFunctions::GlobalFunctions(QObject *parent, QString empresa) : QObject(parent)
{
    this->empresa = empresa;
}

//Code form "https://evileg.com/en/post/415/"
void GlobalFunctions::execJavaScriptCode(QString code) //param code: JavaScript code in string
{
    // Create a JavaScript engine object
//    QScriptEngine engine;
//    // Take from it the context in which we will work.
//    // In this case, a new context is created.
//    QScriptContext* context = engine.pushContext();

//    // Take the names of the variables with which to work
//    const QString var_1 = "";
//    const QString var_2 = "";

//    // Set the variables by trying to convert the text of the QLineEdit fields to double
//    bool ok = false;

//    double value = 1.0;
//    if (ok)
//    {
//        // Set variable with value as properties of the script activation object.
//        context->activationObject().setProperty(var_1, value);
//    }
//    else
//    {
//        // If the conversion fails, the value will be Undefined.
//        context->activationObject().setProperty(var_1, QScriptValue::UndefinedValue);
//    }

//    value = 1.0;
//    if (ok)
//    {
//         // Set variable with value as properties of the script activation object.
//        context->activationObject().setProperty(var_2, value);
//    }
//    else
//    {
//        // If the conversion fails, the value will be Undefined.
//        context->activationObject().setProperty(var_2, QScriptValue::UndefinedValue);
//    }

//    // Run the script
//    engine.evaluate(code);

//    // We clear the field of output of the result from the previous values

//    //Displaying the current state of variables
//    QString resVar_1 = " = " + context->activationObject().property(var_1).toString();
//    QString resVar_2 = " = " + context->activationObject().property(var_2).toString();

//    // Remove context
//    engine.popContext();

    // Since the QScriptEngine is created on the method stack, it will be deleted automatically when the method ends.
}

QString GlobalFunctions::getDirOfTask(QJsonObject jsonObject){
    QString dir = "";
    QString field = jsonObject.value(poblacion).toString();
    if(checkIfFieldIsValid(field)){
        dir += field +", ";
    }
    field = jsonObject.value(calle).toString();
    if(checkIfFieldIsValid(field)){
        dir += field +", ";
    }
    field = jsonObject.value(numero).toString();
    if(checkIfFieldIsValid(field)){
        dir += field +" ";
    }
    field = jsonObject.value(BIS).toString();
    if(checkIfFieldIsValid(field)){
        dir += field+" ";
    }
    field = jsonObject.value(piso).toString();
    if(checkIfFieldIsValid(field)){
        dir += field+" ";
    }
    field = jsonObject.value(mano).toString();
    if(checkIfFieldIsValid(field)){
        dir += field;
    }
    return dir.trimmed();
}
void GlobalFunctions::setDelay(int delay_ms){
    QEventLoop loop;
    QTimer::singleShot(delay_ms, &loop, &QEventLoop::quit);
    loop.exec();
}
QString GlobalFunctions::readEmpresaSelected(){
    QString empresa = "";
    QFile data_base_empresa_selected(empresa_selected_file); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base_empresa_selected.open(QIODevice::ReadOnly)){

        QDataStream in(&data_base_empresa_selected);
        in >> empresa;
        data_base_empresa_selected.close();
    }
    return empresa;
}
void GlobalFunctions::writeEmpresaSelected(QString empresa){
    QFile data_base_empresa_selected(empresa_selected_file); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base_empresa_selected.open(QIODevice::WriteOnly)){

        QDataStream out(&data_base_empresa_selected);
        out << empresa;
        data_base_empresa_selected.close();
    }
}
QString GlobalFunctions::readGestorSelected(){
    QString gestor = "Todos";
    QFile data_base_gestor_selected(gestor_selected_file); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base_gestor_selected.open(QIODevice::ReadOnly)){

        QDataStream in(&data_base_gestor_selected);
        in >> gestor;
        data_base_gestor_selected.close();
    }
    return gestor;
}
void GlobalFunctions::writeGestorSelected(QString gestor){
    QFile data_base_gestor_selected(gestor_selected_file); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base_gestor_selected.open(QIODevice::WriteOnly)){

        QDataStream out(&data_base_gestor_selected);
        out << gestor;
        data_base_gestor_selected.close();
    }
}
bool GlobalFunctions::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
void GlobalFunctions::clearWidgets(QLayout * layout) {
    if (! layout)
        return;
    while (auto item = layout->takeAt(0)) {
        delete item->widget();
        clearWidgets(item->layout());
    }
}

void GlobalFunctions::deleteAllChilds(QWidget *w){
    int c= w->children().size();
    for (int i=0; i < c; i++) {
        QWidget *child_widget = static_cast<QWidget*>((w->
                                                       children().at(i)));
        if(child_widget){
            if(child_widget->isWidgetType()){
                child_widget->deleteLater();
            }
        }
    }
}
bool GlobalFunctions::deleteChild(QWidget *w, QString childObjectName){
    int c= w->children().size();
    for (int i=0; i < c; i++) {
        QWidget *child_widget = static_cast<QWidget*>((w->
                                                       children().at(i)));
        if(child_widget){
            if(child_widget->isWidgetType()){
                if(child_widget->objectName()==childObjectName){
                    child_widget->deleteLater();
                    return true;
                }
            }
        }
    }
    return false;
}
QString GlobalFunctions::convertJsonObjectToString(QJsonObject jsonObject){
    QJsonDocument d;
    d.setObject(jsonObject);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp = QString::fromUtf8(ba);
    return temp;
}
QJsonObject GlobalFunctions::convertStringToJsonObject(QString jsonObject_string){
    QJsonObject jsonObject = QJsonObject();
    if(checkIfFieldIsValid(jsonObject_string)){
        QJsonDocument d = QJsonDocument::
                fromJson(jsonObject_string.toUtf8());
        if(d.isObject()){
            jsonObject = d.object();
        }
    }
    return jsonObject;
}
QJsonArray GlobalFunctions::convertStringToJsonArray(QString jsonArray_string){
    QJsonArray jsonArray = QJsonArray();
    QJsonDocument d = QJsonDocument::
            fromJson(jsonArray_string.toUtf8());
    if(d.isArray()){
        jsonArray = d.array();
    }
    return jsonArray;
}

void GlobalFunctions::sortStringList(QStringList &list, int orden){
    if(orden == MENOR_A_MAYOR){
        list.sort(Qt::CaseInsensitive);
    }else if(orden == MAYOR_A_MENOR){
        QStringList tempList = list;
        tempList.sort(Qt::CaseInsensitive);
        list.clear();
        QString item;
        foreach(item, tempList){
            list.prepend(item);
        }
    }
}
void GlobalFunctions::get_tareas_amount_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_TAREAS_AMOUNT,keys,values);
}
void GlobalFunctions::get_tareas_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_TAREAS_CUSTOM_QUERY,keys,values);
}
void GlobalFunctions::get_itacs_amount_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ITACS_AMOUNT,keys,values);
}
void GlobalFunctions::get_itacs_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ITACS_CUSTOM_QUERY,keys,values);
}
void GlobalFunctions::get_contadores_amount_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_CONTADORES_AMOUNT,keys,values);
}
void GlobalFunctions::get_contadores_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_CONTADORES_CUSTOM_QUERY,keys,values);
}
void GlobalFunctions::get_all_column_values_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES,keys,values);
}
void GlobalFunctions::get_all_column_values_custom_query_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES_CUSTOM_QUERY,keys,values);
}
void GlobalFunctions::get_multiple_values_fields_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_MULTIPLE_VALUES_FIELDS,keys,values);
}
void GlobalFunctions::get_multiple_values_fields_custom_query_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_MULTIPLE_VALUES_FIELDS_CUSTOM_QUERY,keys,values);
}
void GlobalFunctions::get_rutas_amount_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_RUTAS_AMOUNT,keys,values);
}
void GlobalFunctions::get_rutas_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_RUTAS_CUSTOM_QUERY,keys,values);
}

QJsonArray GlobalFunctions::getTareasFields(QStringList fields, QString field, QString value){
    QString  query = " (" + field + " LIKE '" + value +"')";
    QJsonObject jsonObjectFields;
    QString fieldx;
    foreach(fieldx, fields){
        jsonObjectFields.insert(fieldx, fieldx);
    }
    getMultipleValuesFieldsCustomQueryServer(empresa, "tareas", jsonObjectFields, query);
    return jsonArrayAll;
}
QJsonArray GlobalFunctions::getTareasFields(QStringList fields, QString query){
    QJsonObject jsonObjectFields;
    QString fieldx;
    foreach(fieldx, fields){
        jsonObjectFields.insert(fieldx, fieldx);
    }
    getMultipleValuesFieldsCustomQueryServer(empresa, "tareas", jsonObjectFields, query);
    return jsonArrayAll;
}
QJsonArray GlobalFunctions::getTareasFromServer(QString field, QString value){
    QString  query = " (" + field + " LIKE '" + value +"')";
    getTareasFromServer(empresa, query, limit_pagination, 0);
    return jsonArrayAll;
}
QJsonObject GlobalFunctions::getTareaFromServer(QString field, QString value){
    QString  query = " (" + field + " LIKE '" + value +"')";
    getTareasFromServer(empresa, query, 1, 0);
    if(jsonArrayAll.size() > 0){
        return jsonArrayAll.at(0).toObject();
    }
    return QJsonObject();
}
QJsonObject GlobalFunctions::getTareaFromServer(QJsonObject jsonObject){
    QString  query = "";
    QStringList keys = jsonObject.keys();
    QString key;
    foreach(key, keys){
        if(query.isEmpty()){
            query += " (" + key + " LIKE '" + jsonObject.value(key).toString() +"')";
        }else{
            query += " AND (" + key + " LIKE '" + jsonObject.value(key).toString() +"')";
        }
    }
    getTareasFromServer(empresa, query, 1, 0);
    if(jsonArrayAll.size() > 0){
        return jsonArrayAll.at(0).toObject();
    }
    return QJsonObject();
}

QJsonArray GlobalFunctions::getRutasFields(QStringList fields){
    QJsonObject jsonObjectFields;
    QString field;
    foreach(field, fields){
        jsonObjectFields.insert(field, field);
    }
    getMultipleValuesFieldsServer("", "rutas", jsonObjectFields);
    return jsonArrayAll;
}
QJsonArray GlobalFunctions::getRutasFields(QStringList fields, QString field, QString value){
    if(field == codigo_ruta_rutas){
        if(value.contains("-")){
            value=value.split("-").at(0).trimmed();
        }
    }
    QString  query = " (" + field + " LIKE '" + value +"')";
    QJsonObject jsonObjectFields;
    QString fieldx;
    foreach(fieldx, fields){
        jsonObjectFields.insert(fieldx, fieldx);
    }
    getMultipleValuesFieldsCustomQueryServer("", "rutas", jsonObjectFields, query);
    return jsonArrayAll;
}
QJsonArray GlobalFunctions::getRutasFields(QStringList fields, QString field, QStringList values){
    QString  query = "";
    for (int i=0; i < values.size(); i++) {
        QString value = values.at(i);
        if(field == codigo_ruta_rutas){
            if(value.contains("-")){
                value=value.split("-").at(0).trimmed();
            }
        }
        if(query.isEmpty()){
            query += " (" + field + " LIKE '" + value +"')";
        }else{
            query += " OR (" + field + " LIKE '" + value +"')";
        }
    }
    QJsonObject jsonObjectFields;
    QString fieldx;
    foreach(fieldx, fields){
        jsonObjectFields.insert(fieldx, fieldx);
    }
    getMultipleValuesFieldsCustomQueryServer("", "rutas", jsonObjectFields, query);
    return jsonArrayAll;
}
QJsonArray GlobalFunctions::getRutasFromServer(QString field, QString value){
    if(field == codigo_ruta_rutas){
        if(value.contains("-")){
            value=value.split("-").at(0).trimmed();
        }
    }
    QString  query = " (" + field + " LIKE '" + value +"')";
    getRutasFromServer(query, limit_pagination, 0);
    return jsonArrayAll;
}
QJsonObject GlobalFunctions::getRutaFromServer(QString field, QString value){
    if(field == codigo_ruta_rutas){
        if(value.contains("-")){
            value=value.split("-").at(0).trimmed();
        }
    }
    QString  query = " (" + field + " LIKE '" + value +"')";
    getRutasFromServer(query, 1, 0);
    if(jsonArrayAll.size() > 0){
        return jsonArrayAll.at(0).toObject();
    }
    return QJsonObject();
}
QJsonObject GlobalFunctions::getRutaFromServer(QString value){
    if(value.contains("-")){
        value=value.split("-").at(0).trimmed();
    }
    QString  query = " (" + codigo_ruta_rutas + " LIKE '" + value +"')";
    getRutasFromServer(query, 1, 0);
    if(jsonArrayAll.size() > 0){
        return jsonArrayAll.at(0).toObject();
    }
    return QJsonObject();
}

QJsonArray GlobalFunctions::getContadoresFromServer(QString field, QString value){
    QString  query = " (" + field + " LIKE '" + value +"')";
    getContadoresFromServer(empresa, query, limit_pagination, 0);
    return jsonArrayAll;
}
QJsonObject GlobalFunctions::getContadorFromServer(QString field, QString value){
    QString  query = " (" + field + " LIKE '" + value +"')";
    getContadoresFromServer(empresa, query, 1, 0);
    if(jsonArrayAll.size() > 0){
        return jsonArrayAll.at(0).toObject();
    }
    return QJsonObject();
}
QJsonObject GlobalFunctions::getContadorFromServer(QString value){
    QString  query = " (" + numero_serie_contadores + " LIKE '" + value +"')";
    getContadoresFromServer(empresa, query, 1, 0);
    if(jsonArrayAll.size() > 0){
        return jsonArrayAll.at(0).toObject();
    }
    return QJsonObject();
}

QJsonArray GlobalFunctions::getItacsFields(QStringList fields){
    QJsonObject jsonObjectFields;
    QString field;
    foreach(field, fields){
        jsonObjectFields.insert(field, field);
    }
    getMultipleValuesFieldsServer(empresa, "itacs", jsonObjectFields);
    return jsonArrayAll;
}
QJsonArray GlobalFunctions::getItacsFields(QStringList fields, QString field, QString value){
    QString  query = " (" + field + " LIKE '" + value +"')";
    QJsonObject jsonObjectFields;
    QString fieldx;
    foreach(fieldx, fields){
        jsonObjectFields.insert(fieldx, fieldx);
    }
    getMultipleValuesFieldsCustomQueryServer(empresa, "itacs", jsonObjectFields, query);
    return jsonArrayAll;
}
QJsonArray GlobalFunctions::getItacsFields(QStringList fields, QString field, QStringList values){
    QString  query = "";
    for (int i=0; i < values.size(); i++) {
        if(query.isEmpty()){
            query += " (" + field + " LIKE '" + values.at(i) +"')";
        }else{
            query += " OR (" + field + " LIKE '" + values.at(i) +"')";
        }
    }
    QJsonObject jsonObjectFields;
    QString fieldx;
    foreach(fieldx, fields){
        jsonObjectFields.insert(fieldx, fieldx);
    }
    getMultipleValuesFieldsCustomQueryServer(empresa, "itacs", jsonObjectFields, query);
    return jsonArrayAll;
}
QJsonArray GlobalFunctions::getItacsFromServer(QString field, QString value){
    QString  query = " (" + field + " LIKE '" + value +"')";
    getItacsFromServer(empresa, query, limit_pagination, 0);
    return jsonArrayAll;
}
QJsonArray GlobalFunctions::getItacsFromServer(QStringList fields, QStringList values){
    QString  query = "";
    for (int i=0; i < fields.size(); i++) {
        if(query.isEmpty()){
            query += " (" + fields.at(i) + " LIKE '" + values.at(i) +"')";
        }else{
            query += " AND (" + fields.at(i) + " LIKE '" + values.at(i) +"')";
        }
    }
    getItacsFromServer(empresa, query, limit_pagination, 0);
    return jsonArrayAll;
}
QJsonArray GlobalFunctions::getItacsFromServer(QString field, QStringList values){
    QString  query = "";
    for (int i=0; i < values.size(); i++) {
        if(query.isEmpty()){
            query += " (" + field + " LIKE '" + values.at(i) +"')";
        }else{
            query += " OR (" + field + " LIKE '" + values.at(i) +"')";
        }
    }
    getItacsFromServer(empresa, query, limit_pagination, 0);
    return jsonArrayAll;
}
QJsonObject GlobalFunctions::getItacFromServer(QString field, QString value){
    QString  query = " (" + field + " LIKE '" + value +"')";
    getItacsFromServer(empresa, query, 1, 0);
    if(jsonArrayAll.size() > 0){
        return jsonArrayAll.at(0).toObject();
    }
    return QJsonObject();
}
QJsonObject GlobalFunctions::getItacFromServer(QString value){
    QString  query = " (" + codigo_itac_itacs + " LIKE '" + value +"')";
    getItacsFromServer(empresa, query, 1, 0);
    if(jsonArrayAll.size() > 0){
        return jsonArrayAll.at(0).toObject();
    }
    return QJsonObject();
}
bool GlobalFunctions::checkIfTareaExist(QJsonObject jsonObject)
{
    QString  query = "";
    QStringList keys = jsonObject.keys();
    QString key;
    foreach(key, keys){
        if(query.isEmpty()){
            query += " (" + key + " LIKE '" + jsonObject.value(key).toString() +"')";
        }else{
            query += " AND (" + key + " LIKE '" + jsonObject.value(key).toString() +"')";
        }
    }
    getTareasAmountFromServer(empresa, query);
    if(count > 0){
        return true;
    }
    return false;
}
bool GlobalFunctions::checkIfTareaExist(QString value)
{
    QString  query = " (" + numero_interno + " LIKE '" + value +"')";
    getTareasAmountFromServer(empresa, query);
    if(count > 0){
        return true;
    }
    return false;
}
bool GlobalFunctions::checkIfTareaExist(QString field, QString value)
{
    QString  query = " (" + field + " LIKE '" + value +"')";
    getTareasAmountFromServer(empresa, query);
    if(count > 0){
        return true;
    }
    return false;
}

bool GlobalFunctions::checkIfRutaExist(QString field, QString value)
{
    QString  query = " (" + field + " LIKE '" + value +"')";
    getRutasAmountFromServer(query);
    if(count > 0){
        return true;
    }
    return false;
}
bool GlobalFunctions::checkIfRutaExist(QString value)
{
    QString  query = " (" + codigo_ruta_rutas + " LIKE '" + value +"')";
    getRutasAmountFromServer(query);
    if(count > 0){
        return true;
    }
    return false;
}

bool GlobalFunctions::checkIfCounterExist(QString field, QString value)
{
    QString  query = " (" + field + " LIKE '" + value +"')";
    getContadoresAmountFromServer(empresa, query);
    if(count > 0){
        return true;
    }
    return false;
}
bool GlobalFunctions::checkIfCounterExist(QString value)
{
    QString  query = " (" + numero_serie_contadores + " LIKE '" + value +"')";
    getContadoresAmountFromServer(empresa, query);
    if(count > 0){
        return true;
    }
    return false;
}

bool GlobalFunctions::checkIfItacExist(QString field, QString value)
{
    QString  query = " (" + field + " LIKE '" + value +"')";
    getItacsAmountFromServer(empresa, query);
    if(count > 0){
        return true;
    }
    return false;
}
bool GlobalFunctions::checkIfItacExist(QString value)
{
    QString  query = " (" + codigo_itac_itacs + " LIKE '" + value +"')";
    getItacsAmountFromServer(empresa, query);
    if(count > 0){
        return true;
    }
    return false;
}

bool GlobalFunctions::getTareasCustomQuery(QString query, int id_start){

    //    if(currentGestor != "Todos"){
    //        query += " AND (" + GESTOR + " LIKE '" + currentGestor +"')";
    //    }
    if(lastQuery != query){
        currentPage = 1;
    }
    lastQuery = query;
    last_id_start = id_start;

    getTareasAmountFromServer(empresa, query, limit_pagination);

    bool res = getTareasFromServer(empresa, query, limit_pagination, id_start);
    qDebug()<<"query send -> "<< query << " <- id start" << id_start;
    return res;
}
bool GlobalFunctions::getTareasFromServer(QString empresa, QString query, int limit, int id_start)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT" << "id_start";
    values << empresa.toLower() << query << QString::number(limit) << QString::number(id_start);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &GlobalFunctions::get_tareas_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::get_contadores_custom_query_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}
bool GlobalFunctions::getTareasAmountFromServer(QString empresa, QString query, int limit)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT";
    values << empresa.toLower() << query << QString::number(limit);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &GlobalFunctions::get_tareas_amount_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::get_contadores_amount_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}

bool GlobalFunctions::getRutasCustomQuery(QString query, int id_start){

    //    if(currentGestor != "Todos"){
    //        query += " AND (" + gestor_rutas + " LIKE '" + currentGestor +"')";
    //    }
    if(lastQuery != query){
        currentPage = 1;
    }
    lastQuery = query;
    last_id_start = id_start;

    getRutasAmountFromServer(query, limit_pagination);

    bool res = getRutasFromServer(query, limit_pagination, id_start);
    qDebug()<<"query send -> "<< query << " <- id start" << id_start;
    return res;
}
bool GlobalFunctions::getRutasFromServer(QString query, int limit, int id_start)
{
    QStringList keys, values;

    keys << "query" << "LIMIT" << "id_start";
    values << query << QString::number(limit) << QString::number(id_start);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &GlobalFunctions::get_rutas_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::get_rutas_custom_query_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}
bool GlobalFunctions::getRutasAmountFromServer( QString query, int limit)
{
    QStringList keys, values;

    keys << "query" << "LIMIT";
    values << query << QString::number(limit);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &GlobalFunctions::get_rutas_amount_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::get_rutas_amount_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}

bool GlobalFunctions::getContadoresCustomQuery(QString query, int id_start){

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
bool GlobalFunctions::getContadoresFromServer(QString empresa, QString query, int limit, int id_start)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT" << "id_start";
    values << empresa.toLower() << query << QString::number(limit) << QString::number(id_start);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &GlobalFunctions::get_contadores_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::get_contadores_custom_query_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}
bool GlobalFunctions::getContadoresAmountFromServer(QString empresa, QString query, int limit)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT";
    values << empresa.toLower() << query << QString::number(limit);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &GlobalFunctions::get_contadores_amount_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::get_contadores_amount_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}

bool GlobalFunctions::getItacsCustomQuery(QString query, int id_start){

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
bool GlobalFunctions::getItacsFromServer(QString empresa, QString query, int limit, int id_start)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT" << "id_start";
    values << empresa.toLower() << query << QString::number(limit) << QString::number(id_start);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &GlobalFunctions::get_itacs_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::get_itacs_custom_query_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}
bool GlobalFunctions::getItacsAmountFromServer(QString empresa, QString query, int limit)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT";
    values << empresa.toLower() << query << QString::number(limit);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &GlobalFunctions::get_itacs_amount_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::get_itacs_amount_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}

QStringList GlobalFunctions::getTareasList(){
    QStringList values;
    bool res = getValuesFieldServer(
                empresa, "tareas", numero_interno);
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
    return values;
}
QStringList GlobalFunctions::getTareasList(QString field, QString value){
    QStringList values;
    QString  query = " (" + field + " LIKE '" + value +"')";
    bool res = getValuesFieldCustomQueryServer(
                empresa, "tareas", numero_interno, query);
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
    return values;
}
QStringList GlobalFunctions::getContadoresList(bool disponibles, QString serie){
    QString query = "";
    if(disponibles){
        query = " (" + status_contadores + " NOT LIKE 'INSTALLED%')";
    }else{
        query = " (" + status_contadores + " LIKE 'INSTALLED%')";
    }
    if(!serie.trimmed().isEmpty()){
        query = "(" +query+" AND (" + numero_serie_contadores + " LIKE '"+serie+"%') )";
    }
    QStringList values;
    bool res = getValuesFieldCustomQueryServer(
                empresa, "contadores",numero_serie_contadores, query);
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
    return values;
}
QStringList GlobalFunctions::getContadoresList(){
    QStringList values;
    bool res = getValuesFieldServer(
                empresa, "contadores",numero_serie_contadores);
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
    return values;
}
QStringList GlobalFunctions::getItacsList(){
    QStringList values;
    bool res = getValuesFieldServer(
                empresa, "itacs",codigo_itac_itacs);
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
    return values;
}
QStringList GlobalFunctions::getRutasList(){
    QStringList values;
    bool res = getRutasValuesFieldServer(codigo_ruta_rutas);
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
    return values;
}

bool GlobalFunctions::getMultipleValuesFieldsServer(QString empresa, QString tabla, QJsonObject json_fields)
{
    bool res = false;
    QStringList keys, values;

    QJsonDocument d;
    d.setObject(json_fields);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp_fields = QString::fromUtf8(ba);

    if(!empresa.isEmpty()){
        keys << "empresa";
        values << empresa;
    }
    keys << "json_fields" << "tabla";
    values << temp_fields << tabla;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &GlobalFunctions::get_multiple_values_fields_request);

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
bool GlobalFunctions::getMultipleValuesFieldsCustomQueryServer(QString empresa, QString tabla, QJsonObject json_fields, QString query)
{
    bool res = false;
    QStringList keys, values;

    QJsonDocument d;
    d.setObject(json_fields);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp_fields = QString::fromUtf8(ba);

    if(!empresa.isEmpty()){
        keys << "empresa";
        values << empresa;
    }
    keys << "json_fields" << "tabla" << "query";
    values << temp_fields << tabla << query;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &GlobalFunctions::get_multiple_values_fields_custom_query_request);

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

bool GlobalFunctions::getValuesFieldServer(QString empresa, QString tabla, QString column)
{
    bool res = false;
    QStringList keys, values;

    keys << "empresa" << "columna" << "tabla";
    values << empresa << column << tabla;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

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
bool GlobalFunctions::getValuesFieldCustomQueryServer(QString empresa, QString tabla
                                                      , QString column, QString query)
{
    bool res = false;
    QStringList keys, values;

    keys << "empresa" << "columna" << "tabla" << "query";
    values << empresa << column << tabla<< query;

    this->keys = keys;
    this->values = values;

    QTimer::singleShot(500, this, SLOT(getColumnValues()));

    QEventLoop *q = new QEventLoop();
    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

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
bool GlobalFunctions::getRutasValuesFieldServer(QString column)
{
    bool res = false;
    QStringList keys, values;

    keys << "columna" << "tabla";
    values << column << "rutas";

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

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
bool GlobalFunctions::getRutasValuesFieldCustomQueryServer(QString column, QString query)
{
    bool res = false;
    QStringList keys, values;

    keys << "columna" << "tabla" << "query";
    values << column << "rutas" << query;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &GlobalFunctions::script_excecution_result,q,&QEventLoop::exit);

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

void GlobalFunctions::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo){
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_ALL_COLUMN_VALUES_CUSTOM_QUERY){
        jsonObjectValues = database_comunication::getJsonObject(byte_array);
        result = database_comunication::script_result::ok;
    }
    else if(tipo == database_comunication::GET_ALL_COLUMN_VALUES)
    {
        jsonObjectValues = database_comunication::getJsonObject(byte_array);
        result = database_comunication::script_result::ok;
    }
    else if(tipo == database_comunication::GET_MULTIPLE_VALUES_FIELDS){
        jsonArrayAll = database_comunication::getJsonArray(byte_array);
        result = database_comunication::script_result::ok;
    }
    else if(tipo == database_comunication::GET_MULTIPLE_VALUES_FIELDS_CUSTOM_QUERY){
        jsonArrayAll = database_comunication::getJsonArray(byte_array);
//        QFile file("BD/query.dat");
//        if(file.open(QIODevice::WriteOnly)){
//            file.write(byte_array);
//            file.close();
//        };
//        qDebug()<<byte_array;
        result = database_comunication::script_result::ok;
    }
    else if(tipo == database_comunication::GET_RUTAS_AMOUNT)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(byte_array.contains("ot success get_rutas_amount_custom_query"))
        {
            qDebug()<<byte_array;
            result = database_comunication::script_result::get_rutas_amount_failed;
        }
        else {
            jsonInfoAmount = database_comunication::getJsonObject(byte_array);
            qDebug()<<"query return -> "<<jsonInfoAmount.value("query").toString();
            QString count_rutas = jsonInfoAmount.value("count_rutas").toString();
            count = count_rutas.toInt();
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::GET_RUTAS_CUSTOM_QUERY)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(byte_array.contains("ot success get_rutas_with_limit_custom_query"))
        {
            qDebug()<<byte_array;
            result = database_comunication::script_result::get_rutas_custom_query_failed;
        }
        else {
            jsonArrayAll = database_comunication::getJsonArray(byte_array);
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::GET_TAREAS_AMOUNT)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(byte_array.contains("ot success get_tareas_amount_custom_query"))
        {
            qDebug()<<byte_array;
            result = database_comunication::script_result::get_tareas_amount_failed;
        }
        else {
            jsonInfoAmount = database_comunication::getJsonObject(byte_array);
            qDebug()<<"query return -> "<<jsonInfoAmount.value("query").toString();
            QString count_tareas = jsonInfoAmount.value("count_tareas").toString();
            count = count_tareas.toInt();
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::GET_TAREAS_CUSTOM_QUERY)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(byte_array.contains("ot success get_tareas_with_limit_custom_query"))
        {
            qDebug()<<byte_array;
            result = database_comunication::script_result::get_tareas_custom_query_failed;
        }
        else {
            jsonArrayAll = database_comunication::getJsonArray(byte_array);
            result = database_comunication::script_result::ok;
        }
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
            jsonInfoAmount = database_comunication::getJsonObject(byte_array);
            qDebug()<<"query return -> "<<jsonInfoAmount.value("query").toString();
            QString count_contadores = jsonInfoAmount.value("count_contadores").toString();
            count = count_contadores.toInt();
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
            jsonInfoAmount = database_comunication::getJsonObject(byte_array);
            qDebug()<<"query return -> "<<jsonInfoAmount.value("query").toString();
            QString count_itacs = jsonInfoAmount.value("count_itacs").toString();
            count = count_itacs.toInt();
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

    emit script_excecution_result(result);
}

void GlobalFunctions::showWarning(QWidget *parent, const QString title, const QString mess){
    int length = mess.length();
    int res = length / 45;
    int sizeInc = 0;
    for(int i = 0; i < res; i++){
        sizeInc += 40;
    }
    QWidget *widget_blur = new QWidget(parent);
    widget_blur->setFixedSize(parent->size()+QSize(0,30));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();


    MyLabelShine *label_loading_text = new MyLabelShine(widget_blur);
    label_loading_text->setStyleSheet("background-color: rgb(255, 255, 255);"
                                      "color: rgb(54, 141, 206);"
                                      "border-radius: 10px;"
                                      "font: italic 14pt \"Segoe UI Semilight\";");
    QRect rect = widget_blur->geometry();
    int w = 600, h= 180+sizeInc;
    w = (parent->size().width() <= w)? parent->size().width()-10: w;
    h = (parent->size().height() <= h)? parent->size().height()-10: h;
    label_loading_text->setFixedSize(w, h);
    label_loading_text->move(rect.width()/2
                             - label_loading_text->size().width()/2,
                             rect.height()/2
                             -  label_loading_text->size().height()/2);
    label_loading_text->setMaximumSize(parent->size());
    label_loading_text->show();

    QLabel *message = new QLabel(widget_blur);
    message->setText(mess);
    message->setFixedSize(580, 80+sizeInc);
    message->move(rect.width()/2
                  - message->size().width()/2,
                  rect.height()/2
                  -  message->size().height()/2 - 15);
    message->setStyleSheet("background-color: rgb(255, 255, 255);"
                           "color: rgb(54, 141, 206);"
                           "font: italic 14pt \"Segoe UI Semilight\";");
    message->setAlignment(Qt::AlignCenter);
    message->show();
    message->raise();
    QLabel *l_title = new QLabel(widget_blur);
    l_title->setText(title);
    l_title->setFixedSize(580, 25);
    l_title->move(rect.width()/2
                  - l_title->size().width()/2,
                  label_loading_text->pos().y() + 25);
    l_title->setStyleSheet("background-color: rgb(255, 255, 255);"
                           "color: rgb(54, 141, 206);"
                           "font: italic 14pt \"Segoe UI\";");
    l_title->setAlignment(Qt::AlignCenter);
    l_title->show();
    l_title->raise();

    MyLabelAnimated *close = new MyLabelAnimated(widget_blur);
    close->setStyleSheet(
                "QLabel{"
                "border-radius: 5px;"
                "background-color: rgb(255, 255, 255);"
                "}"
                "QLabel:hover:!pressed{"
                " background-color: rgba(255, 200, 200);"
                "}");
    close->setScaledContents(true);
    close->setPixmap(QPixmap(":/icons/close.png"));
    close->setFixedSize(30, 30);
    close->move(label_loading_text->pos().x() +
                label_loading_text->width() -
                close->width() - 10,
                label_loading_text->pos().y() + 10);
    close->show();
    close->raise();
    QLabel *icon = new QLabel(widget_blur);
    icon->setStyleSheet("background-color: rgb(255, 255, 255);");
    icon->setScaledContents(true);
    icon->setPixmap(QPixmap(":/icons/warning_yellow.png"));
    icon->setFixedSize(35, 35);
    icon->move(label_loading_text->pos().x() + 10,
               label_loading_text->pos().y() + 10);
    icon->show();
    icon->raise();

    MyLabelAnimated *pb_aceptar = new MyLabelAnimated(widget_blur);
    pb_aceptar->setShadowEffect();
    pb_aceptar->setStyleSheet("QLabel{"
                              "background-color: rgb(54, 141, 206);"
                              "color: rgb(255, 255, 255);"
                              "border-radius: 5px;"
                              "font: italic 12pt \"Segoe UI\";"
                              "}"
                              "QLabel:hover:!pressed{"
                              "background-color: #3080C0; "
                              "}");
    pb_aceptar->setText("ACEPTAR");
    pb_aceptar->setFixedSize(100, 35);
    pb_aceptar->move(rect.width()/2
                     - pb_aceptar->size().width()/2,
                     label_loading_text->pos().y() +
                     label_loading_text->height() -
                     pb_aceptar->height() - 20);
    pb_aceptar->setAlignment(Qt::AlignCenter);
    pb_aceptar->show();
    pb_aceptar->raise();

    connect(pb_aceptar, &MyLabelAnimated::clicked, widget_blur, &QWidget::hide);
    connect(pb_aceptar, &MyLabelAnimated::clicked, widget_blur, &QWidget::deleteLater);

    connect(close, &MyLabelAnimated::clicked, widget_blur, &QWidget::hide);
    connect(close, &MyLabelAnimated::clicked, widget_blur, &QWidget::deleteLater);

    QEventLoop q;
    connect(pb_aceptar, &MyLabelAnimated::clicked,&q,&QEventLoop::quit);
    connect(close, &MyLabelAnimated::clicked,&q,&QEventLoop::quit);
    q.exec();
}

void GlobalFunctions::showMessage(QWidget *parent, const QString title, const QString mess){
    int length = mess.length();
    int res = length / 45;
    int sizeInc = 0;
    for(int i = 0; i < res; i++){
        sizeInc += 40;
    }
    QWidget *widget_blur = new QWidget(parent);
    widget_blur->setFixedSize(parent->size()+QSize(0,30));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();


    MyLabelShine *label_loading_text = new MyLabelShine(widget_blur);
    label_loading_text->setStyleSheet("background-color: rgb(255, 255, 255);"
                                      "color: rgb(54, 141, 206);"
                                      "border-radius: 10px;"
                                      "font: italic 14pt \"Segoe UI Semilight\";");
    QRect rect = widget_blur->geometry();
    int w = 600, h= 180+sizeInc;
    w = (parent->size().width() <= w)? parent->size().width()-10: w;
    h = (parent->size().height() <= h)? parent->size().height()-10: h;
    label_loading_text->setFixedSize(w, h);
    label_loading_text->move(rect.width()/2
                             - label_loading_text->size().width()/2,
                             rect.height()/2
                             -  label_loading_text->size().height()/2);
    label_loading_text->setMaximumSize(parent->size());
    label_loading_text->show();

    QLabel *message = new QLabel(widget_blur);
    message->setText(mess);
    message->setFixedSize(580, 80+sizeInc);
    message->move(rect.width()/2
                  - message->size().width()/2,
                  rect.height()/2
                  -  message->size().height()/2 - 15);
    message->setStyleSheet("background-color: rgb(255, 255, 255);"
                           "color: rgb(54, 141, 206);"
                           "font: italic 14pt \"Segoe UI Semilight\";");
    message->setAlignment(Qt::AlignCenter);
    message->show();
    message->raise();
    QLabel *l_title = new QLabel(widget_blur);
    l_title->setText(title);
    l_title->setFixedSize(580, 25);
    l_title->move(rect.width()/2
                  - l_title->size().width()/2,
                  label_loading_text->pos().y() + 25);
    l_title->setStyleSheet("background-color: rgb(255, 255, 255);"
                           "color: rgb(54, 141, 206);"
                           "font: italic 14pt \"Segoe UI\";");
    l_title->setAlignment(Qt::AlignCenter);
    l_title->show();
    l_title->raise();

    MyLabelAnimated *close = new MyLabelAnimated(widget_blur);
    close->setStyleSheet(
                "QLabel{"
                "border-radius: 5px;"
                "background-color: rgb(255, 255, 255);"
                "}"
                "QLabel:hover:!pressed{"
                " background-color: rgba(255, 200, 200);"
                "}");
    close->setScaledContents(true);
    close->setPixmap(QPixmap(":/icons/close.png"));
    close->setFixedSize(30, 30);
    close->move(label_loading_text->pos().x() +
                label_loading_text->width() -
                close->width() - 10,
                label_loading_text->pos().y() + 10);
    close->show();
    close->raise();
    QLabel *icon = new QLabel(widget_blur);
    icon->setStyleSheet("background-color: rgb(255, 255, 255);");
    icon->setScaledContents(true);
    icon->setPixmap(QPixmap(":/icons/information.png"));
    icon->setFixedSize(35, 35);
    icon->move(label_loading_text->pos().x() + 10,
               label_loading_text->pos().y() + 10);
    icon->show();
    icon->raise();

    MyLabelAnimated *pb_aceptar = new MyLabelAnimated(widget_blur);
    pb_aceptar->setShadowEffect();
    pb_aceptar->setStyleSheet("QLabel{"
                              "background-color: rgb(54, 141, 206);"
                              "color: rgb(255, 255, 255);"
                              "border-radius: 5px;"
                              "font: italic 12pt \"Segoe UI\";"
                              "}"
                              "QLabel:hover:!pressed{"
                              "background-color: #3080C0; "
                              "}");
    pb_aceptar->setText("ACEPTAR");
    pb_aceptar->setFixedSize(100, 35);
    pb_aceptar->move(rect.width()/2
                     - pb_aceptar->size().width()/2,
                     label_loading_text->pos().y() +
                     label_loading_text->height() -
                     pb_aceptar->height() - 20);
    pb_aceptar->setAlignment(Qt::AlignCenter);
    pb_aceptar->show();
    pb_aceptar->raise();

    connect(pb_aceptar, &MyLabelAnimated::clicked, widget_blur, &QWidget::hide);
    connect(pb_aceptar, &MyLabelAnimated::clicked, widget_blur, &QWidget::deleteLater);

    connect(close, &MyLabelAnimated::clicked, widget_blur, &QWidget::hide);
    connect(close, &MyLabelAnimated::clicked, widget_blur, &QWidget::deleteLater);

    QEventLoop q;
    connect(pb_aceptar, &MyLabelAnimated::clicked,&q,&QEventLoop::quit);
    connect(close, &MyLabelAnimated::clicked,&q,&QEventLoop::quit);
    q.exec();
}

int GlobalFunctions::showQuestion(QWidget *parent, const QString title, const QString mess,
                                  int acceptOption, int cancelOption){
    return showQuestion(parent, title, mess, acceptOption, cancelOption, "ACEPTAR", "CANCELAR");

}

int GlobalFunctions::showQuestion(QWidget *parent, const QString title, const QString mess, int acceptOption,
                                  int cancelOption, QString acceptText, QString cancelText){
    int result = -1;
    int length = mess.length();
    int res = length / 45;
    int sizeInc = 0;
    for(int i = 0; i < res; i++){
        sizeInc += 40;
    }
    QWidget *widget_blur = new QWidget(parent);
    widget_blur->setFixedSize(parent->size()+QSize(0,30));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();


    MyLabelShine *label_loading_text = new MyLabelShine(widget_blur);
    label_loading_text->setStyleSheet("background-color: rgb(255, 255, 255);"
                                      "color: rgb(54, 141, 206);"
                                      "border-radius: 10px;"
                                      "font: italic 14pt \"Segoe UI Semilight\";");
    QRect rect = widget_blur->geometry();
    int w = 600, h= 180+sizeInc;
    w = (parent->size().width() <= w)? parent->size().width()-10: w;
    h = (parent->size().height() <= h)? parent->size().height()-10: h;
    label_loading_text->setFixedSize(w, h);
    label_loading_text->move(rect.width()/2
                             - label_loading_text->size().width()/2,
                             rect.height()/2
                             -  label_loading_text->size().height()/2);
    label_loading_text->show();

    QLabel *message = new QLabel(widget_blur);
    message->setText(mess);
    message->setFixedSize(580, 80+sizeInc);
    message->move(rect.width()/2
                  - message->size().width()/2,
                  rect.height()/2
                  -  message->size().height()/2 - 15);
    message->setStyleSheet("background-color: rgb(255, 255, 255);"
                           "color: rgb(54, 141, 206);"
                           "font: italic 14pt \"Segoe UI Semilight\";");
    message->setAlignment(Qt::AlignCenter);
    message->show();
    message->raise();
    QLabel *l_title = new QLabel(widget_blur);
    l_title->setText(title);
    l_title->setFixedSize(580, 25);
    l_title->move(rect.width()/2
                  - l_title->size().width()/2,
                  label_loading_text->pos().y() + 25);
    l_title->setStyleSheet("background-color: rgb(255, 255, 255);"
                           "color: rgb(54, 141, 206);"
                           "font: italic 14pt \"Segoe UI\";");
    l_title->setAlignment(Qt::AlignCenter);
    l_title->show();
    l_title->raise();

    MyLabelAnimated *close = new MyLabelAnimated(widget_blur);
    close->setClickedOption(cancelOption);
    close->setStyleSheet(
                "QLabel{"
                "border-radius: 5px;"
                "background-color: rgb(255, 255, 255);"
                "}"
                "QLabel:hover:!pressed{"
                " background-color: rgba(255, 200, 200);"
                "}");
    close->setScaledContents(true);
    close->setPixmap(QPixmap(":/icons/close.png"));
    close->setFixedSize(30, 30);
    close->move(label_loading_text->pos().x() +
                label_loading_text->width() -
                close->width() - 10,
                label_loading_text->pos().y() + 10);
    close->show();
    close->raise();
    QLabel *icon = new QLabel(widget_blur);
    icon->setStyleSheet("background-color: rgb(255, 255, 255);");
    icon->setScaledContents(true);
    icon->setPixmap(QPixmap(":/icons/question.png"));
    icon->setFixedSize(35, 35);
    icon->move(label_loading_text->pos().x() + 10,
               label_loading_text->pos().y() + 10);
    icon->show();
    icon->raise();

    MyLabelAnimated *pb_aceptar = new MyLabelAnimated(widget_blur);
    pb_aceptar->setClickedOption(acceptOption);
    pb_aceptar->setShadowEffect();
    pb_aceptar->setStyleSheet("QLabel{"
                              "background-color: rgb(54, 141, 206);"
                              "color: rgb(255, 255, 255);"
                              "border-radius: 5px;"
                              "font: italic 12pt \"Segoe UI\";"
                              "}"
                              "QLabel:hover:!pressed{"
                              "background-color: #3080C0; "
                              "}");
    pb_aceptar->setText(acceptText);
    pb_aceptar->setFixedSize(100, 35);
    pb_aceptar->move(label_loading_text->width()/3
                     + label_loading_text->pos().x()
                     - pb_aceptar->size().width()/2,
                     label_loading_text->pos().y() +
                     label_loading_text->height() -
                     pb_aceptar->height() - 20);
    pb_aceptar->setAlignment(Qt::AlignCenter);
    pb_aceptar->show();
    pb_aceptar->raise();

    MyLabelAnimated *pb_cancelar = new MyLabelAnimated(widget_blur);
    pb_cancelar->setClickedOption(cancelOption);
    pb_cancelar->setShadowEffect();
    pb_cancelar->setStyleSheet("QLabel{"
                              "background-color: rgb(54, 141, 206);"
                              "color: rgb(255, 255, 255);"
                              "border-radius: 5px;"
                              "font: italic 12pt \"Segoe UI\";"
                              "}"
                              "QLabel:hover:!pressed{"
                              "background-color: #3080C0; "
                              "}");
    pb_cancelar->setText(cancelText);
    pb_cancelar->setFixedSize(100, 35);
    pb_cancelar->move(label_loading_text->width()*2/3
                     + label_loading_text->pos().x()
                     - pb_cancelar->size().width()/2,
                     label_loading_text->pos().y() +
                     label_loading_text->height() -
                     pb_cancelar->height() - 20);
    pb_cancelar->setAlignment(Qt::AlignCenter);
    pb_cancelar->show();
    pb_cancelar->raise();

    connect(pb_aceptar, &MyLabelAnimated::clicked, widget_blur, &QWidget::hide);
    connect(pb_aceptar, &MyLabelAnimated::clicked, widget_blur, &QWidget::deleteLater);
    connect(pb_cancelar, &MyLabelAnimated::clicked, widget_blur, &QWidget::hide);
    connect(pb_cancelar, &MyLabelAnimated::clicked, widget_blur, &QWidget::deleteLater);

    connect(close, &MyLabelAnimated::clicked, widget_blur, &QWidget::hide);
    connect(close, &MyLabelAnimated::clicked, widget_blur, &QWidget::deleteLater);

    QEventLoop q;
    connect(pb_aceptar, &MyLabelAnimated::clickedOption,&q,&QEventLoop::exit);
    connect(pb_cancelar, &MyLabelAnimated::clickedOption,&q,&QEventLoop::exit);
    connect(close, &MyLabelAnimated::clickedOption,&q,&QEventLoop::exit);
    result = q.exec();

    return result;
}

void GlobalFunctions::showInExplorer(const QString &path)
{
    QDesktopServices::openUrl( QUrl::fromLocalFile(path) );
}

