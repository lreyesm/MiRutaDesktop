#include "dbtareascontroller.h"
#include <QDebug>
#include "new_table_structure.h"
#include "tabla.h"

QString tableName = "tareas";

DBtareasController::DBtareasController(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    //        allTables = new tablesAndColumns();

    if (!m_db.open())
        qDebug() << "Error: connection with database fail";
    else
        qDebug() << "Database: connection ok";
}

DBtareasController::~DBtareasController()
{
    if (m_db.isOpen())
        m_db.close();
}
bool DBtareasController::isOpen() const
{
    return m_db.isOpen();
}

bool DBtareasController::createTableTareas(/*const QString &tableName*/)
{
    bool success = true;
    QSqlQuery query;
    QString tableName = "tareas";
    fillJsonType();
    QStringList keys = jsonTareaType.keys();
    QString queryString = "";
    QString key;
    foreach(key, keys){
        if(key != id){
            queryString += "," +key +  " TEXT ";
        }
    }
    qDebug() << queryString;
    query.prepare("CREATE TABLE " + tableName + " ("
                  + id + " INTEGER PRIMARY KEY AUTOINCREMENT "
                  + queryString + ");");
    if (!query.exec())
    {
        qDebug() << "Tareas table already created ";
        success = false;
    }
    return success;
}

bool DBtareasController::insertTarea(QJsonObject jsonObject)
{
    bool success = false;
    QString tableName = "tareas";
    QStringList keys = jsonTareaType.keys();
    QString queryStringKeys = "";
    QString queryStringValues = "";
    QString key;
    foreach(key, keys){
        if(key != id){
            queryStringKeys += key + ",";
        }
    }
    queryStringKeys.remove(queryStringKeys.size()-1, 1);

    foreach(key, keys){
        if(key != id){
            queryStringValues += ":"+key + ",";
        }
    }
    queryStringValues.remove(queryStringValues.size()-1, 1);

    QSqlQuery queryAdd;
    queryAdd.prepare("INSERT INTO " + tableName + " (" + queryStringKeys +
                     ") VALUES ("+ queryStringValues +")");
    foreach(key, keys){
        if(key != id){
            queryAdd.bindValue(":"+key, jsonObject.value(key).toString());
        }
    }

    if(queryAdd.exec())
        success = true;
    else
        qDebug() << "tarea could not add: " << queryAdd.lastError();

    return success;
}
bool DBtareasController::removeTarea(const QString &tareaPrincVar)
{
    bool success = false;

    QString tableName = "tareas";
    if (!tareaPrincVar.isEmpty())
    {
        QSqlQuery queryDelete;
        queryDelete.prepare("DELETE FROM " + tableName + " WHERE "+
                            principal_variable+"='" + tareaPrincVar+"'");
        success = queryDelete.exec();

        if(!success)
            qDebug() << "Tarea has been deleted: " << queryDelete.lastError();
    }
    else
        qDebug() << "Tarea has not been deleted";
    return success;
}

bool DBtareasController::updateTarea(QJsonObject jsonObject)
{
    bool success = false;

    QString queryString = "";
    QStringList keys = jsonObject.keys();
    QString key;
    foreach(key, keys){
        if(key != id || key != principal_variable){
            queryString += key + "='" + jsonObject.value(key).toString()+ "' ,";
        }
    }
    queryString.remove(queryString.size()-1, 1);
    QString query_str = "UPDATE " + tableName + " SET "+ queryString
            + " where "+ principal_variable + "='" + jsonObject.value(principal_variable).toString() +"'";

    QSqlQuery queryAdd;
    queryAdd.prepare(query_str);

    if(queryAdd.exec())
        success = true;
    else
        qDebug() << "Tarea could not be added: " << queryAdd.lastError();

    return success;
}

bool DBtareasController::checkIfTareaExists(const QString &tareaPrincVar){ //true si existe tarea

    bool found = false, ok;
    QSqlQuery q;
    QString queryString = "SELECT * FROM "+tableName+" WHERE "+principal_variable+"='"+tareaPrincVar+"';";
    ok = q.exec(queryString);
    if (ok && q.next()) {
        found = true;
    }
    return found;
}

int DBtareasController::getCountTableRows(){
    int rows = 0;
    QSqlQuery query("SELECT COUNT(*) FROM "+tableName);
    if (query.next()) {
        rows = query.value(0).toInt();
    }
    return rows;
}

QStringList DBtareasController::getOneColumnValues(const QString &column){//retorna los resultados de las columnas validos y sin repeticion
    QStringList values;
    QSqlQuery query;
    query.prepare("SELECT "+column+" FROM "+tableName);
    query.exec();

    while (query.next()) {
        QString value = query.value(0).toString();
//        qDebug() << "found" << value;
        if(Tabla::checkIfFieldIsValid(value) && !values.contains(value)){
            values << value;
        }
//        QSqlRecord rec = query.record();
//        qDebug() << "Number of columns: " << rec.count();

//        int idIndex = rec.indexOf("id");
//        int keywordIndex = rec.indexOf("keyword");
//        qDebug() << query.value(idIndex).toString() << query.value(keywordIndex).toString();

    }
    return values;
}
QJsonArray DBtareasController::getTareas(){
    QString tableName = "tareas";
    QString queryString = "SELECT * FROM "+tableName;
    return SQLQueryGetArray(queryString);
}
QJsonArray DBtareasController::getTareas(QString key, QString value){
    QString tableName = "tareas";
    QString where_clause=" where "+key+"='"+value+"'";
    QString queryString = "SELECT * FROM "+tableName + where_clause;
    return SQLQueryGetArray(queryString);
}
QJsonArray DBtareasController::getTareas(QStringList keys_where, QString values_where){
    QString tableName = "tareas";
    QString where_clause="";
    if(Tabla::checkIfFieldIsValid(keys_where.at(0))
            && Tabla::checkIfFieldIsValid(values_where.at(0))){
        where_clause += " where "+keys_where.at(0)+"='"+values_where.at(0)+"'";

        if(where_clause.size() > 1 && values_where.size() > 1){
            for(int i = 1; i < keys_where.size(); i++){
                if(Tabla::checkIfFieldIsValid(keys_where.at(i))
                        && Tabla::checkIfFieldIsValid(values_where.at(i))){
                    where_clause += " AND "+keys_where.at(i)+"='"+values_where.at(i)+"'";
                }
            }
        }
    }
    QString queryString = "SELECT * FROM "+tableName + where_clause;
    return SQLQueryGetArray(queryString);
}

QString DBtareasController::SQLQueryGetString(const QString & sqlquery) {
    QSqlQuery query;

    query.setForwardOnly(true);
    if (!query.exec(sqlquery))
        return QString();

    QJsonDocument  json;
    QJsonArray recordsArray;

    while(query.next()) {
        QJsonObject recordObject;
        for(int x=0; x < query.record().count(); x++) {
            recordObject.insert( query.record().fieldName(x),
                                 QJsonValue::fromVariant(query.value(x)));
        }
        recordsArray.push_back(recordObject);
    }
    json.setArray(recordsArray);

    return json.toJson();
}
QJsonArray DBtareasController::SQLQueryGetArray(const QString & sqlquery) {
    QSqlQuery query;

    query.setForwardOnly(true);
    if (!query.exec(sqlquery))
        return QJsonArray();

    QJsonArray recordsArray;

    while(query.next()) {
        QJsonObject recordObject;
        for(int x=0; x < query.record().count(); x++) {
            recordObject.insert( query.record().fieldName(x),
                                 QJsonValue::fromVariant(query.value(x)));
        }
        recordsArray.push_back(recordObject);
    }
    return recordsArray;
}

void DBtareasController::fillJsonType(){
    jsonTareaType.insert(id, -1);
    jsonTareaType.insert(idOrdenCABB, "");
    jsonTareaType.insert(FechImportacion, "");
    jsonTareaType.insert(numero_interno, "");
    jsonTareaType.insert(GESTOR, "");
    jsonTareaType.insert(ANOMALIA, "");
    jsonTareaType.insert(AREALIZAR, "");//numero de portal
    jsonTareaType.insert(INTERVENCION, "");
    jsonTareaType.insert(reparacion, "");
    jsonTareaType.insert(propiedad, "");
    jsonTareaType.insert(CONTADOR_Prefijo_anno, "");
    jsonTareaType.insert(numero_serie_contador, "");
    jsonTareaType.insert(marca_contador, "");
    jsonTareaType.insert(calibre_toma, "");
    jsonTareaType.insert(ruedas, "");
    jsonTareaType.insert(fecha_instalacion, "");
    jsonTareaType.insert(actividad, "");
    jsonTareaType.insert(emplazamiento, "");
    jsonTareaType.insert(acceso, "");
    jsonTareaType.insert(calle, "");
    jsonTareaType.insert(numero, "");//numero de portal
    jsonTareaType.insert(BIS, "");
    jsonTareaType.insert(piso, "");
    jsonTareaType.insert(mano, "");
    jsonTareaType.insert(poblacion, "");
    jsonTareaType.insert(nombre_cliente, "");
    jsonTareaType.insert(numero_abonado, "");
    jsonTareaType.insert(nombre_firmante, "");
    jsonTareaType.insert(numero_carnet_firmante, "");
    jsonTareaType.insert(lectura_ultima, "");
    jsonTareaType.insert(FECEMISIO, "");
    jsonTareaType.insert(FECULTREP, "");
    jsonTareaType.insert(OBSERVA, "");
    jsonTareaType.insert(RS, "");
    jsonTareaType.insert(F_INST, "");
    jsonTareaType.insert(INDICE, "");
    jsonTareaType.insert(emplazamiento_devuelto, "");
    jsonTareaType.insert(RESTO_EM, "");
    jsonTareaType.insert(lectura_actual, "");
    jsonTareaType.insert(lectura_contador_nuevo, "");
    jsonTareaType.insert(observaciones_devueltas, "");
    jsonTareaType.insert(TIPO, "");
    jsonTareaType.insert(TIPO_devuelto, "");
    jsonTareaType.insert(Estado, "");
    jsonTareaType.insert(marca_devuelta, "");
    jsonTareaType.insert(calibre_real, "");
    jsonTareaType.insert(RUEDASDV, "");
    jsonTareaType.insert(LARGO, "");
    jsonTareaType.insert(largo_devuelto, "");//numero de portal
    jsonTareaType.insert(numero_serie_contador_devuelto, "");
    jsonTareaType.insert(CONTADOR_Prefijo_anno_devuelto, "");
    jsonTareaType.insert(AREALIZAR_devuelta, "");
    jsonTareaType.insert(intervencion_devuelta, "");
    jsonTareaType.insert(RESTEMPLAZA, "");
    jsonTareaType.insert(FECH_CIERRE, "");
    jsonTareaType.insert(TIPORDEN, "");
    jsonTareaType.insert(operario, "");
    jsonTareaType.insert(observaciones, "");
    jsonTareaType.insert(TIPOFLUIDO, "");
    jsonTareaType.insert(TIPOFLUIDO_devuelto, "");
    jsonTareaType.insert(idexport, "");
    jsonTareaType.insert(fech_facturacion, "");
    jsonTareaType.insert(fech_cierrenew, "");
    jsonTareaType.insert(fech_informacionnew, "");
    jsonTareaType.insert(f_instnew, "");
    jsonTareaType.insert(tipoRadio, "");
    jsonTareaType.insert(tipoRadio_devuelto, "");
    jsonTareaType.insert(marcaR, "");
    jsonTareaType.insert(codigo_de_localizacion, "");
    jsonTareaType.insert(codigo_de_geolocalizacion, "");
    jsonTareaType.insert(geolocalizacion, "");
    jsonTareaType.insert(url_geolocalizacion, "");
    jsonTareaType.insert(foto_antes_instalacion, "");
    jsonTareaType.insert(foto_numero_serie, "");
    jsonTareaType.insert(foto_lectura, "");
    jsonTareaType.insert(foto_despues_instalacion, "");
    jsonTareaType.insert(foto_incidencia_1, "");
    jsonTareaType.insert(foto_incidencia_2, "");
    jsonTareaType.insert(foto_incidencia_3, "");
    jsonTareaType.insert(firma_cliente, "");
    jsonTareaType.insert(tipo_tarea, "");
    jsonTareaType.insert(telefonos_cliente, "");
    jsonTareaType.insert(telefono1, "");
    jsonTareaType.insert(telefono2, "");
    jsonTareaType.insert(fechas_tocado_puerta, "");
    jsonTareaType.insert(fechas_nota_aviso, "");
    jsonTareaType.insert(resultado, "");
    jsonTareaType.insert(nuevo_citas, "");
    jsonTareaType.insert(fecha_hora_cita, "");
    jsonTareaType.insert(fecha_de_cambio, "");
    jsonTareaType.insert(zona, "");
    jsonTareaType.insert(ruta, "");
    jsonTareaType.insert(numero_serie_modulo, "");
    jsonTareaType.insert(ubicacion_en_bateria, "");
    jsonTareaType.insert(incidencia, "");
    jsonTareaType.insert(ID_FINCA, "");
    jsonTareaType.insert(COMENTARIOS, "");
    jsonTareaType.insert(DNI_CIF_COMUNIDAD, "");
    jsonTareaType.insert(TARIFA, "");
    jsonTareaType.insert(TOTAL_CONTADORES, "");
    jsonTareaType.insert(C_CANAL, "");
    jsonTareaType.insert(C_LYC, "");
    jsonTareaType.insert(C_AGRUPA, "");
    jsonTareaType.insert(DNI_CIF_ABONADO, "");
    jsonTareaType.insert(C_COMUNERO, "");
    jsonTareaType.insert(MENSAJE_LIBRE, "");

    jsonTareaType.insert(ID_SAT, "");
    jsonTareaType.insert(fecha_realizacion, "");
    jsonTareaType.insert(suministros, "");
    jsonTareaType.insert(servicios, "");
    jsonTareaType.insert(equipo, "");
    jsonTareaType.insert(fecha_informe_servicios, "");
    jsonTareaType.insert(piezas, "");
    jsonTareaType.insert(prioridad, "");

    jsonTareaType.insert(causa_origen, "");
    jsonTareaType.insert(accion_ordenada, "");
    jsonTareaType.insert(hibernacion, "");

    jsonTareaType.insert(date_time_modified, "");
    jsonTareaType.insert(status_tarea, "");
}
