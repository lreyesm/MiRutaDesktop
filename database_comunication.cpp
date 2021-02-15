#include "database_comunication.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QUrlQuery>
#include <QFile>
#include <QEventLoop>
#include <QtNetwork>
#include "global_variables.h"

database_comunication::database_comunication():QObject()
{
    tipoPeticion = serverRequestType::NONE_REQUEST;
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(downloadFinished(QNetworkReply*)));
}

database_comunication::~database_comunication()
{

}
void database_comunication::cancelRequests()
{
    emit cancelDownload();
}

void database_comunication::serverRequest(serverRequestType type, QStringList keys, QStringList values)
{
    QUrl serviceUrl;
    QByteArray postData;
    QUrlQuery query;
    QString url, filename;

//        url = "http://localhost/";
    //    url = "https://server26194.webcindario.com/";
    //    url = "https://server26194.000webhostapp.com/"; //servidor de pruebas

    url = "https://mywateroute.com/Mi_Ruta/"; //servidor actual de michel de pago*******************************

    tipoPeticion = type;

    bool is_file_upload = false;
    switch (type)
    {
    //Nuevo--------------------------------------------------------------------------------------------------------------------------------------------------
    case serverRequestType::GET_FILES_TO_UPDATE:
        serviceUrl = QUrl(url + "get_filenames_to_update.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
        //------------------------------------------------------------------------------------------------------------------------------------------------------
    case serverRequestType::TEST_DB_CONNECTION:
        serviceUrl = QUrl(url + "db_connection.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::LOGIN:
        serviceUrl = QUrl(url + "login_administrador.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        query.addQueryItem(keys[2],values[2]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::SAVE_LOGIN:
        if(!url.contains("localhost")){
            serviceUrl = QUrl(url_server + "save_login.php");
            query.addQueryItem(keys[0],values[0]);//json_info
            query.addQueryItem(keys[1], url); //server
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        break;

    case serverRequestType::GET_ADMINISTRADORES:
        serviceUrl = QUrl(url + "get_administradores.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPDATE_ADMINISTRADOR:
        serviceUrl = QUrl(url + "update_administrador.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::DELETE_ADMINISTRADOR:
        serviceUrl = QUrl(url + "delete_administrador.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::CREATE_ADMINISTRADOR:
        serviceUrl = QUrl(url + "create_administrador.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPLOAD_ADMINISTRADOR_IMAGE:
        serviceUrl = QUrl(url + "upload_administrador_image.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        query.addQueryItem(keys[2],values[2]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::DOWNLOAD_ADMINISTRADOR_IMAGE:
        serviceUrl = QUrl(url + "download_administrador_image.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_EQUIPO_OPERARIOS:
        serviceUrl = QUrl(url + "get_equipo_operarios.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPDATE_EQUIPO_OPERARIO:
        serviceUrl = QUrl(url + "update_equipo_operario.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::DELETE_EQUIPO_OPERARIO:
        serviceUrl = QUrl(url + "delete_equipo_operario.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::CREATE_EQUIPO_OPERARIO:
        serviceUrl = QUrl(url + "create_equipo_operario.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_OPERARIOS:
        serviceUrl = QUrl(url + "get_operarios.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPDATE_OPERARIO:
        serviceUrl = QUrl(url + "update_operario.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::DELETE_OPERARIO:
        serviceUrl = QUrl(url + "delete_operario.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::CREATE_OPERARIO:
        serviceUrl = QUrl(url + "create_operario.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPLOAD_OPERARIO_IMAGE:
        serviceUrl = QUrl(url + "upload_operario_image.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        query.addQueryItem(keys[2],values[2]);
        query.addQueryItem(keys[3],values[3]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::DOWNLOAD_OPERARIO_IMAGE:
        serviceUrl = QUrl(url + "download_operario_image.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_ONE_OPERARIO:
        serviceUrl = QUrl(url + "get_one_operario.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_ONE_OPERARIO_TASKS:
        serviceUrl = QUrl(url + "get_one_operator_tasks.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;


    case serverRequestType::LOGIN_CLIENTE:
        serviceUrl = QUrl(url + "login_cliente.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        query.addQueryItem(keys[2],values[2]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_CLIENTES:
        serviceUrl = QUrl(url + "get_clientes.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPDATE_CLIENTE:
        serviceUrl = QUrl(url + "update_cliente.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::DELETE_CLIENTE:
        serviceUrl = QUrl(url + "delete_cliente.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::CREATE_CLIENTE:
        serviceUrl = QUrl(url + "create_cliente.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPLOAD_CLIENTE_IMAGE:
        serviceUrl = QUrl(url + "upload_cliente_image.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        query.addQueryItem(keys[2],values[2]);
        query.addQueryItem(keys[3],values[3]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::DOWNLOAD_CLIENTE_IMAGE:
        serviceUrl = QUrl(url + "download_cliente_image.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_ALL_INTERNAL_NUMBERS:
        serviceUrl = QUrl(url + "get_all_numeros_internos.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_ALL_SERIAL_NUMBERS:
        serviceUrl = QUrl(url + "get_all_numeros_series.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_CAUSAS:
        serviceUrl = QUrl(url + "get_causas.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;


    case serverRequestType::GET_RESULTADOS:
        serviceUrl = QUrl(url + "get_resultados.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_EMPLAZAMIENTOS:
        serviceUrl = QUrl(url + "get_emplazamientos.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_CLASES:
        serviceUrl = QUrl(url + "get_clases.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_TIPOS:
        serviceUrl = QUrl(url + "get_tipos.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_OBSERVACIONES:
        serviceUrl = QUrl(url + "get_observaciones.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_PIEZAS:
        serviceUrl = QUrl(url + "get_piezas.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_INFOS:
        serviceUrl = QUrl(url + "get_infos.php");
        if(keys.size() > 0 && values.size() > 0){
            query.addQueryItem(keys[0],values[0]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }else{
            qDebug()<<"GET_INFOS crash la cantidad de parametros no es correcta";
        }
        break;
    case serverRequestType::DELETE_INFO:
        serviceUrl = QUrl(url + "delete_info.php");
        if(keys.size() > 1 && values.size() > 1){
            query.addQueryItem(keys[0],values[0]);
            query.addQueryItem(keys[1],values[1]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }else{
            qDebug()<<"DELETE_INFO crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::CREATE_INFO:
        serviceUrl = QUrl(url + "create_info.php");
        if(keys.size() > 1 && values.size() > 1){
            query.addQueryItem(keys[0],values[0]);
            query.addQueryItem(keys[1],values[1]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }else{
            qDebug()<<"CREATE_INFO crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::UPDATE_INFO:
        serviceUrl = QUrl(url + "update_info.php");
        if(keys.size() > 1 && values.size() > 1){
            query.addQueryItem(keys[0],values[0]);
            query.addQueryItem(keys[1],values[1]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }else{
            qDebug()<<"UPDATE_INFO crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_RUTAS_AMOUNT:
        serviceUrl = QUrl(url + "get_rutas_amount_custom_query.php");
        if(keys.size() > 1 && values.size() > 1){
            query.addQueryItem(keys[0],values[0]); //query
            query.addQueryItem(keys[1],values[1]); //LIMIT
            //            query.addQueryItem(keys[2],values[2]); //empresa
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_RUTAS_AMOUNT crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_RUTAS_CUSTOM_QUERY:
        serviceUrl = QUrl(url + "get_rutas_with_limit_custom_query.php");
        if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //query
            query.addQueryItem(keys[1],values[1]); //LIMIT
            query.addQueryItem(keys[2],values[2]); //id_start
            //            query.addQueryItem(keys[3],values[3]); //empresa
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_RUTAS_CUSTOM_QUERY crash la cantidad de parametros no es correcta";
        }
        break;
    case serverRequestType::GET_RUTAS:
        serviceUrl = QUrl(url + "get_rutas.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::UPDATE_RUTA:
        serviceUrl = QUrl(url + "update_ruta.php");
        postData = values[0].toUtf8();
        break;
    case serverRequestType::DELETE_RUTA:
        serviceUrl = QUrl(url + "delete_ruta.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_RUTA:
        serviceUrl = QUrl(url + "create_ruta.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::GET_RUEDAS:
        serviceUrl = QUrl(url + "get_ruedas.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::UPDATE_RUEDA:
        serviceUrl = QUrl(url + "update_rueda.php");
        postData = values[0].toUtf8();
        break;
    case serverRequestType::DELETE_RUEDA:
        serviceUrl = QUrl(url + "delete_rueda.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_RUEDA:
        serviceUrl = QUrl(url + "create_rueda.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::GET_ONE_TASK:
        serviceUrl = QUrl(url + "get_one_tarea.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_CONTADORES_AMOUNT:
        serviceUrl = QUrl(url + "get_contadores_amount_custom_query.php");
        if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //empresa
            query.addQueryItem(keys[1],values[1]); //query
            query.addQueryItem(keys[2],values[2]); //LIMIT
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_CONTADORES_AMOUNT crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_CONTADORES_CUSTOM_QUERY:
        serviceUrl = QUrl(url + "get_contadores_with_limit_custom_query.php");
        if(keys.size() > 3 && values.size() > 3){
            query.addQueryItem(keys[0],values[0]); //empresa
            query.addQueryItem(keys[1],values[1]); //query
            query.addQueryItem(keys[2],values[2]); //LIMIT
            query.addQueryItem(keys[3],values[3]); //id_start
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_CONTADORES_CUSTOM_QUERY crash la cantidad de parametros no es correcta";
        }
        break;
    case serverRequestType::GET_CONTADORES:
        serviceUrl = QUrl(url + "get_contadores.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::UPDATE_CONTADOR:
        serviceUrl = QUrl(url + "update_contador.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::CREATE_CONTADOR:
        serviceUrl = QUrl(url + "create_contador.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::DELETE_CONTADORES:
        serviceUrl = QUrl(url + "delete_contadores.php");
        if(keys.size() > 1 && values.size() > 1){
            query.addQueryItem(keys[0],values[0]); //json numeros serie
            query.addQueryItem(keys[1],values[1]); //empresa
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"DELETE_CONTADORES crash la cantidad de parametros no es correcta";
        }
        break;


    case serverRequestType::GET_MULTIPLE_VALUES_FIELDS:
        serviceUrl = QUrl(url + "get_all_column_multiple_values.php");
        if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //empresa
            query.addQueryItem(keys[1],values[1]); //json_fields
            query.addQueryItem(keys[2],values[2]); //tabla
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else if(keys.size() > 1 && values.size() > 1){
            query.addQueryItem(keys[0],values[0]); //json_fields
            query.addQueryItem(keys[1],values[1]); //tabla
            //            query.addQueryItem(keys[2],values[2]); //empresa
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_MULTIPLE_VALUES_FIELDS crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_MULTIPLE_VALUES_FIELDS_CUSTOM_QUERY:
        serviceUrl = QUrl(url + "get_all_column_multiple_values_custom_query.php");
        if(keys.size() > 3 && values.size() > 3){
            query.addQueryItem(keys[0],values[0]); //empresa
            query.addQueryItem(keys[1],values[1]); //json_fields
            query.addQueryItem(keys[2],values[2]); //tabla
            query.addQueryItem(keys[3],values[3]); //query
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //json_fields
            query.addQueryItem(keys[1],values[1]); //tabla
            query.addQueryItem(keys[2],values[2]); //query
            //            query.addQueryItem(keys[3],values[3]); //empresa
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_MULTIPLE_VALUES_FIELDS_CUSTOM_QUERY crash la cantidad de parametros no es correcta";
        }
        break;
    case serverRequestType::GET_ALL_COLUMN_VALUES:
        serviceUrl = QUrl(url + "get_all_column_values.php");
        if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //columna
            query.addQueryItem(keys[1],values[1]); //tabla
            query.addQueryItem(keys[2],values[2]); //empresa
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else if(keys.size() > 1 && values.size() > 1){
            query.addQueryItem(keys[0],values[0]); //columna
            query.addQueryItem(keys[1],values[1]); //tabla
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_ALL_COLUMN_VALUES crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_ALL_COLUMN_VALUES_CUSTOM_QUERY:
        serviceUrl = QUrl(url + "get_all_column_values_custom_query.php");
        if(keys.size() > 3 && values.size() > 3){
            query.addQueryItem(keys[0],values[0]); //columna
            query.addQueryItem(keys[1],values[1]); //tabla
            query.addQueryItem(keys[2],values[2]); //empresa
            query.addQueryItem(keys[3],values[3]); //query
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //columna
            query.addQueryItem(keys[1],values[1]); //tabla
            query.addQueryItem(keys[2],values[2]); //query
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_ALL_COLUMN_VALUES crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::UPDATE_CONTADORES_FIELDS:
        serviceUrl = QUrl(url + "update_contadores_fields.php");
        if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //json numeros serie
            query.addQueryItem(keys[1],values[1]); //json campos y valores a actualizar
            query.addQueryItem(keys[2],values[2]); //empresa
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"UPDATE_ITAC_FIELDS crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_ITACS_AMOUNT:
        serviceUrl = QUrl(url + "get_itacs_amount_custom_query.php");
        if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //empresa
            query.addQueryItem(keys[1],values[1]); //query
            query.addQueryItem(keys[2],values[2]); //LIMIT
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_ITACS_AMOUNT crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_ITACS_CUSTOM_QUERY:
        serviceUrl = QUrl(url + "get_itacs_with_limit_custom_query.php");
        if(keys.size() > 3 && values.size() > 3){
            query.addQueryItem(keys[0],values[0]); //empresa
            query.addQueryItem(keys[1],values[1]); //query
            query.addQueryItem(keys[2],values[2]); //LIMIT
            query.addQueryItem(keys[3],values[3]); //id_start
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_ITACS_CUSTOM_QUERY crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_ITACS:
        serviceUrl = QUrl(url + "get_itacs.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPDATE_ITAC_WITH_ID:
        serviceUrl = QUrl(url + "update_itac_with_id.php");
        if(keys.size() > 1 && values.size() > 1){
            query.addQueryItem(keys[0],values[0]);//json itac
            query.addQueryItem(keys[1],values[1]);//empresa
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"UPDATE_ITAC_WITH_ID crash la cantidad de parametros no es correcta";
        }
        break;
    case serverRequestType::UPDATE_ITAC:
        serviceUrl = QUrl(url + "update_itac.php");
        query.addQueryItem(keys[0],values[0]);//json itac
        query.addQueryItem(keys[1],values[1]);//empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::UPDATE_ITAC_FIELDS:
        serviceUrl = QUrl(url + "update_itac_fields.php");
        if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //json cod emplazamientos
            query.addQueryItem(keys[1],values[1]); //json campos y valores a actualizar
            query.addQueryItem(keys[2],values[2]); //empresa
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"UPDATE_ITAC_FIELDS crash la cantidad de parametros no es correcta";
        }
        break;
    case serverRequestType::DELETE_ITAC:
        serviceUrl = QUrl(url + "delete_itac.php");
        query.addQueryItem(keys[0],values[0]);//json itac
        query.addQueryItem(keys[1],values[1]);//empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::CREATE_ITAC:
        serviceUrl = QUrl(url + "create_itac.php");
        query.addQueryItem(keys[0],values[0]);//json itac
        query.addQueryItem(keys[1],values[1]);//empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPLOAD_ITAC_IMAGE:
        serviceUrl = QUrl(url + "upload_itac_image.php");
        if(keys.size() > 5 && values.size() > 5){
            query.addQueryItem(keys[0],values[0]);
            query.addQueryItem(keys[1],values[1]);
            query.addQueryItem(keys[2],values[2]);
            query.addQueryItem(keys[3],values[3]);
            query.addQueryItem(keys[4],values[4]);
            query.addQueryItem(keys[5],values[5]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }else{
            qDebug()<<"UPLOAD_ITAC_IMAGE crash la cantidad de parametros no es correcta";
        }
        break;
    case serverRequestType::DOWNLOAD_ITAC_IMAGE:
        serviceUrl = QUrl(url + "download_itac_image.php");
        if(keys.size() > 3 && values.size() > 3){
            query.addQueryItem(keys[0],values[0]);
            query.addQueryItem(keys[1],values[1]);
            query.addQueryItem(keys[2],values[2]);
            query.addQueryItem(keys[3],values[3]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }else{
            qDebug()<<"DOWNLOAD_ITAC_IMAGE crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_ZONAS:
        serviceUrl = QUrl(url + "get_zonas.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPDATE_ZONA:
        serviceUrl = QUrl(url + "update_zona.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::DELETE_ZONA:
        serviceUrl = QUrl(url + "delete_zona.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::CREATE_ZONA:
        serviceUrl = QUrl(url + "create_zona.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;


    case serverRequestType::UPDATE_CAUSA:
        serviceUrl = QUrl(url + "update_causa.php");
        postData = values[0].toUtf8();
        break;


    case serverRequestType::UPDATE_RESULTADO:
        serviceUrl = QUrl(url + "update_resultado.php");
        postData = values[0].toUtf8();
        break;
    case serverRequestType::UPDATE_EMPLAZAMIENTO:
        serviceUrl = QUrl(url + "update_emplazamiento.php");
        postData = values[0].toUtf8();
        break;
    case serverRequestType::UPDATE_CLASE:
        serviceUrl = QUrl(url + "update_clase.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::UPDATE_TIPO:
        serviceUrl = QUrl(url + "update_tipo.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::UPDATE_OBSERVACION:
        serviceUrl = QUrl(url + "update_observacion.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::UPDATE_PIEZA:
        serviceUrl = QUrl(url + "update_pieza.php");
        postData = values[0].toUtf8();
        break;


    case serverRequestType::UPLOAD_USER_IMAGE:
        //        serviceUrl = QUrl(url + "upload_user_image.php");
        serviceUrl = QUrl(url + "upload_user_image_qt.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::DOWNLOAD_USER_IMAGE:
        //        serviceUrl = QUrl(url + "download_user_image.php");
        serviceUrl = QUrl(url + "download_user_image_qt.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_TASKS:
        serviceUrl = QUrl(url + "get_tareas.php");
        query.addQueryItem(keys[0],values[0]);//empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_TAREAS_AMOUNT:
        serviceUrl = QUrl(url + "get_tareas_amount_custom_query.php");
        if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //empresa
            query.addQueryItem(keys[1],values[1]); //query
            query.addQueryItem(keys[2],values[2]); //LIMIT
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_TAREAS_AMOUNT crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_TAREAS_CUSTOM_QUERY:
        serviceUrl = QUrl(url + "get_tareas_with_limit_custom_query.php");
        if(keys.size() > 3 && values.size() > 3){
            query.addQueryItem(keys[0],values[0]); //empresa
            query.addQueryItem(keys[1],values[1]); //query
            query.addQueryItem(keys[2],values[2]); //LIMIT
            query.addQueryItem(keys[3],values[3]); //id_start
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"GET_TAREAS_CUSTOM_QUERY crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::GET_TAREAS_INFORMADAS:
        serviceUrl = QUrl(url + "get_tareas_informadas.php");
        query.addQueryItem(keys[0],values[0]);//empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_TASKS_GESTOR:
        serviceUrl = QUrl(url + "get_tareas_gestor.php");
        query.addQueryItem(keys[0],values[0]); //gestor
        query.addQueryItem(keys[1],values[1]); //empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPDATE_TAREA:
        serviceUrl = QUrl(url + "update_tarea.php");
        if(keys.size() > 1 && values.size() > 1){
            query.addQueryItem(keys[0],values[0]);
            query.addQueryItem(keys[1],values[1]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"UPDATE_TAREA crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::UPDATE_TAREA_FIELDS:
        serviceUrl = QUrl(url + "update_tarea_fields.php");
        if(keys.size() > 2 && values.size() > 2){
            query.addQueryItem(keys[0],values[0]); //json numeros internos
            query.addQueryItem(keys[1],values[1]); //json campos y valores a actualizar
            query.addQueryItem(keys[2],values[2]); //empresa
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"UPDATE_TAREA_FIELDS crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::CREATE_TAREA:
        serviceUrl = QUrl(url + "create_task.php");
        if(keys.size() > 1 && values.size() > 1){
            query.addQueryItem(keys[0],values[0]);
            query.addQueryItem(keys[1],values[1]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }else{
            qDebug()<<"CREATE_TAREA crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::DELETE_TAREA:
        serviceUrl = QUrl(url + "delete_tarea.php");
        if(keys.size() > 5 && values.size() > 5){
            query.addQueryItem(keys[0],values[0]);
            query.addQueryItem(keys[1],values[1]);
            query.addQueryItem(keys[2],values[2]);
            query.addQueryItem(keys[3],values[3]);
            query.addQueryItem(keys[4],values[4]);
            query.addQueryItem(keys[5],values[5]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }else{
            qDebug()<<"DELETE_TAREA crash la cantidad de parametros no es correcta";
        }
        break;
    case serverRequestType::UPLOAD_TASK_IMAGE:
        serviceUrl = QUrl(url + "upload_task_image.php");
        if(keys.size() > 5 && values.size() > 5){
            query.addQueryItem(keys[0],values[0]);
            query.addQueryItem(keys[1],values[1]);
            query.addQueryItem(keys[2],values[2]);
            query.addQueryItem(keys[3],values[3]);
            query.addQueryItem(keys[4],values[4]);
            query.addQueryItem(keys[5],values[5]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"UPLOAD_TASK_IMAGE crash la cantidad de parametros no es correcta";
        }
        break;

    case serverRequestType::DOWNLOAD_TASK_IMAGE:
        serviceUrl = QUrl(url + "download_task_image.php");
        if(keys.size() > 5 && values.size() > 5){
            query.addQueryItem(keys[0],values[0]);
            query.addQueryItem(keys[1],values[1]);
            query.addQueryItem(keys[2],values[2]);
            query.addQueryItem(keys[3],values[3]);
            query.addQueryItem(keys[4],values[4]);
            query.addQueryItem(keys[5],values[5]);
            postData = query.toString(QUrl::FullyDecoded).toUtf8();
        }
        else{
            qDebug()<<"DOWNLOAD_TASK_IMAGE crash la cantidad de parametros no es correcta";
        }
        break;


    case serverRequestType::GET_MARCAS:
        serviceUrl = QUrl(url + "get_marcas.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::UPDATE_MARCA:
        serviceUrl = QUrl(url + "update_marca.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::DELETE_MARCA:
        serviceUrl = QUrl(url + "delete_marca.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_MARCA:
        serviceUrl = QUrl(url + "create_marca.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::GET_EMPRESAS:
        serviceUrl = QUrl(url + "get_empresas.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::UPDATE_EMPRESA:
        serviceUrl = QUrl(url + "update_empresa.php");
        postData = values[0].toUtf8();
        break;
    case serverRequestType::DELETE_EMPRESA:
        serviceUrl = QUrl(url + "delete_empresa.php");
        postData = values[0].toUtf8();
        break;
    case serverRequestType::CREATE_EMPRESA:
        serviceUrl = QUrl(url + "create_empresa.php");
        postData = values[0].toUtf8();
        break;
    case serverRequestType::UPLOAD_EMPRESA_IMAGE:
        serviceUrl = QUrl(url + "upload_empresa_image.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::DOWNLOAD_EMPRESA_IMAGE:
        serviceUrl = QUrl(url + "download_empresa_image.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;



    case serverRequestType::GET_CALIBRES:
        serviceUrl = QUrl(url + "get_calibres.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPDATE_CALIBRE:
        serviceUrl = QUrl(url + "update_calibre.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::DELETE_CALIBRE:
        serviceUrl = QUrl(url + "delete_calibre.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_CALIBRE:
        serviceUrl = QUrl(url + "create_calibre.php");
        postData = values[0].toUtf8();
        break;


    case serverRequestType::DELETE_LONGITUD:
        serviceUrl = QUrl(url + "delete_longitud.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_LONGITUD:
        serviceUrl = QUrl(url + "create_longitud.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::GET_LONGITUDES:
        serviceUrl = QUrl(url + "get_longitudes.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::UPDATE_LONGITUD:
        serviceUrl = QUrl(url + "update_longitud.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::DELETE_CAUSA:
        serviceUrl = QUrl(url + "delete_causa.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_CAUSA:
        serviceUrl = QUrl(url + "create_causa.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::GET_GESTORES:
        serviceUrl = QUrl(url + "get_gestores.php");
        query.addQueryItem(keys[0],values[0]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPDATE_GESTOR:
        serviceUrl = QUrl(url + "update_gestor.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::DELETE_GESTOR:
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        serviceUrl = QUrl(url + "delete_gestor.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::CREATE_GESTOR:
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        serviceUrl = QUrl(url + "create_gestor.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::UPLOAD_GESTOR_IMAGE:
        serviceUrl = QUrl(url + "upload_gestor_image.php");
        query.addQueryItem(keys[0],values[0]);
        query.addQueryItem(keys[1],values[1]);
        query.addQueryItem(keys[2],values[2]);
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;
    case serverRequestType::DOWNLOAD_GESTOR_IMAGE:
        serviceUrl = QUrl(url + "download_gestor_image.php");
        query.addQueryItem(keys[0],values[0]);//empresa
        query.addQueryItem(keys[1],values[1]);//gestor
        query.addQueryItem(keys[2],values[2]); //image name
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::DELETE_RESULTADO:
        serviceUrl = QUrl(url + "delete_resultado.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_RESULTADO:
        serviceUrl = QUrl(url + "create_resultado.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::DELETE_EMPLAZAMIENTO:
        serviceUrl = QUrl(url + "delete_emplazamiento.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_EMPLAZAMIENTO:
        serviceUrl = QUrl(url + "create_emplazamiento.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::DELETE_CLASE:
        serviceUrl = QUrl(url + "delete_clase.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_CLASE:
        serviceUrl = QUrl(url + "create_clase.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::DELETE_TIPO:
        serviceUrl = QUrl(url + "delete_tipo.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_TIPO:
        serviceUrl = QUrl(url + "create_tipo.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::DELETE_OBSERVACION:
        serviceUrl = QUrl(url + "delete_observacion.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_OBSERVACION:
        serviceUrl = QUrl(url + "create_observacion.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::DELETE_PIEZA:
        serviceUrl = QUrl(url + "delete_pieza.php");
        postData = values[0].toUtf8();
        break;

    case serverRequestType::CREATE_PIEZA:
        serviceUrl = QUrl(url + "create_pieza.php");
        postData = values[0].toUtf8();
        break;



    case serverRequestType::GET_DONE_TASKS:
        serviceUrl = QUrl(url + "get_done_tasks.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::DELETE_FILE:
        serviceUrl = QUrl(url + "delete_file.php");
        query.addQueryItem(keys[0],values[0]);//nombre
        query.addQueryItem(keys[1],values[1]);//gestor
        query.addQueryItem(keys[2],values[2]); //empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_CLIENT_WORK:
        serviceUrl = QUrl(url + "get_client_work.php");
        query.addQueryItem(keys[0],values[0]);//nombre
        query.addQueryItem(keys[1],values[1]);//gestor
        query.addQueryItem(keys[2],values[2]); //empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::LOAD_WORK:
        serviceUrl = QUrl(url + "load_work.php");
        query.addQueryItem(keys[0],values[0]);//nombre
        query.addQueryItem(keys[1],values[1]); //empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::SEND_CLIENT_WORK:
        serviceUrl = QUrl(url + "send_client_work.php");
        postData = builUploadString(values[0],
                values[1]/*Empresa*/, values[2]/*Gestor*/);
        is_file_upload = true;
        break;

    case serverRequestType::SAVE_WORK:
        serviceUrl = QUrl(url + "save_work.php");
        postData = builUploadString(values[0],
                values[1]/*Empresa*/);
        is_file_upload = true;
        break;

    case serverRequestType::GET_FILES_WORK:
        serviceUrl = QUrl(url + "get_filenames_to_load_work.php");
        query.addQueryItem(keys[0],values[0]);//empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::GET_FILES_CLIENT_WORK:
        serviceUrl = QUrl(url + "get_filenames_to_load_client_work.php");
        query.addQueryItem(keys[0],values[0]);//gestor
        query.addQueryItem(keys[1],values[1]);//empresa
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::TEST_CONECTION:
        serviceUrl = QUrl(url + "test_database.php");
        postData = query.toString(QUrl::FullyDecoded).toUtf8();
        break;

    case serverRequestType::DOWNLOAD_FILE:
        if(values.size() > 1){
            donwloadDirLocal= values[1];
            filename = url + values[0];
            serviceUrl = QUrl::fromEncoded(filename.toLocal8Bit());
            doDownload(serviceUrl);
        }else{
            qDebug()<<"DOWNLOAD_FILE crash la cantidad de parametros no es correcta";
        }
        return;

    default:

        break;
    }
    //    postData = query.toString(QUrl::FullyEncoded).toUtf8();

    if(!is_file_upload){
        // Call the webservice
        networkManager = new QNetworkAccessManager(this);
        connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                SLOT(onPostAnswer(QNetworkReply*)));

        QNetworkRequest networkRequest;

        if(serviceUrl.toString().contains("https://"))
        {
            QSslConfiguration config = QSslConfiguration::defaultConfiguration();
            config.setProtocol(QSsl::TlsV1_2);
            networkRequest.setSslConfiguration(config);
        }

        networkRequest.setUrl(serviceUrl);
        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

        /*QNetworkReply *nr =*/ networkManager->post(networkRequest,postData);
//        QObject::connect(this, &database_comunication::cancelDownload
//                         , nr, &QNetworkReply::abort);
    }
    else {
        networkManager = new QNetworkAccessManager(this);
        connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                SLOT(onPostAnswer(QNetworkReply*)));

        QNetworkRequest networkRequest;

        if(serviceUrl.toString().contains("https://"))
        {
            QSslConfiguration config = QSslConfiguration::defaultConfiguration();
            config.setProtocol(QSsl::TlsV1_2);
            networkRequest.setSslConfiguration(config);
        }

        networkRequest.setUrl(serviceUrl);
        QString bound = "*****";
        networkRequest.setRawHeader(QString("Content-Type").toLatin1(),
                                    QString("multipart/form-data; boundary="+bound).toLatin1());
        networkRequest.setRawHeader(QString("Content-Length").toLatin1(),
                                    QString::number(postData.length()).toLatin1());

        /*QNetworkReply *nr =*/ networkManager->post(networkRequest,postData);
//        QObject::connect(this, &database_comunication::cancelDownload
//                         , nr, &QNetworkReply::abort);
    }

}

void database_comunication::doDownload(const QUrl &url)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    // List of reply
    currentDownloads.append(reply);
}

QString database_comunication::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    if (QFile::exists(basename)) {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i)))
            ++i;

        basename += QString::number(i);
    }

    return basename;
}

void database_comunication::downloadFinished(QNetworkReply *reply)
{
    QUrl url = reply->url();
    QString filename;
    if (reply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n",
                url.toEncoded().constData(),
                qPrintable(reply->errorString()));
        QByteArray bytes;
        bytes  = reply->errorString().toLatin1();
        emit alredyAnswered(bytes, tipoPeticion);
    } else {
        filename = saveFileName(url);
        if (saveToDisk(filename, reply))
            printf("Download of %s succeeded (saved to %s)\n",
                   url.toEncoded().constData(), qPrintable(filename));
    }

    currentDownloads.removeAll(reply);
    reply->deleteLater();

    if (currentDownloads.isEmpty()){
        // all downloads finished
        //        QCoreApplication::instance()->quit();
        QByteArray ba;
        ba  = QString("archivo_descargado - " + filename).toLatin1();
        emit alredyAnswered(ba, tipoPeticion);
    }
}

bool database_comunication::saveToDisk(const QString &filename, QIODevice *reply)
{
    QDir dir;
    dir.setPath(donwloadDirLocal);
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    QString path = dir.path() + "/" + filename;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }
    qint64 bytes_to_read = reply->bytesAvailable();
    for (int i = 0; i < bytes_to_read; i+=10000) {
        QByteArray bytes = reply->read(10000);
        file.write(bytes);
    }
    bytes_to_read = reply->bytesAvailable();
    for (int i = 0; i < bytes_to_read; i++) {
        QByteArray bytes = reply->read(1);
        file.write(bytes);
    }
    file.close();

    return true;
}

void database_comunication::sslErrors(const QList<QSslError> &sslErrors)
{
#ifndef QT_NO_SSL
    foreach (const QSslError &error, sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}


bool database_comunication::checkInternetConnection(){//devuelve true si hay conexion a internet
    return true; //importante quitar esto-----------------------------------------------------------------
    QNetworkAccessManager nam;
    QNetworkRequest req(QUrl("http://www.google.com"));
    QNetworkReply* reply = nam.get(req);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (reply->bytesAvailable()){
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        return true;
        //        QMessageBox::information(this, "Info", "You are connected to the internet :)");
    }
    else{
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        return false;
        //        QMessageBox::critical(this, "Info", "You are not connected to the internet :(");
    }
}

bool database_comunication::checkConnection(){//devuelve true si hay conexion a internet
    return true; //importante quitar esto-----------------------------------------------------------------
    QNetworkAccessManager nam;
    QNetworkRequest req(QUrl("http://www.google.com"));
    QNetworkReply* reply = nam.get(req);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if (reply->bytesAvailable()){
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        QObject::connect(this, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                         this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        connect(this, &database_comunication::processedAnswer,&loop,&QEventLoop::exit);
        serverRequest(TEST_CONECTION, QStringList(), QStringList());
        bool retorno = false;
        switch(loop.exec()){
        case database_comunication::connection_failed:
            retorno = false;
            break;
        case database_comunication::connection_success:
            retorno = true;
            break;
        default:
            retorno = false;
            break;
        }
        QObject::disconnect(this, &database_comunication::processedAnswer,&loop,&QEventLoop::exit);
        QObject::disconnect(this, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        return retorno;
        //        QMessageBox::information(this, "Info", "You are connected to the internet :)");
    }
    else{
        QObject::disconnect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        return false;
        //        QMessageBox::critical(this, "Info", "You are not connected to the internet :(");
    }
}

QByteArray database_comunication::builUploadString(QString filename_and_dir, QString empresa, QString gestor){
    QString split_character = "**--__";
    QString bound = "*****";
    QString file_dir_server = empresa + split_character + gestor + split_character + filename_and_dir.split("/").last();
    QString script = "send_client_work.php";
    if(gestor.isEmpty()){
        file_dir_server = empresa + split_character + filename_and_dir.split("/").last();
        QString script = "save_work.php";
    }

    QByteArray data(QString("--" + bound + "\r\n").toLatin1());
    data.append("Content-Disposition: form-data; name=\"action\"\r\n\r\n");
    data.append(script + "\r\n");
    data.append(QString("--" + bound + "\r\n").toLatin1());
    data.append("Content-Disposition: form-data; name=\"bill\"; filename=\"");
    data.append(file_dir_server);
    data.append("\"\r\n");
    data.append("Content-Type: text/xml\r\n\r\n");

    QFile file(filename_and_dir);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "File Error: File not found!";
        return data;
    }
    else {
        qDebug() << "File found";
    }
    data.append(file.readAll());
    data.append("\r\n");
    data.append("--" + bound + "--\r\n");

    file.close();

    return data;
}

QByteArray database_comunication::getFileData(QString filename_and_dir)
{
    QByteArray data;
    QFile file(filename_and_dir);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "File Error: File not found!";
        return data;
    }
    else {
        qDebug() << "File found";
        data.append(file.readAll());
        file.close();
    }
    return data;
}

void database_comunication::serverAnswer(QByteArray result_data,database_comunication::serverRequestType type){
    Q_UNUSED(type);
    int process_result = database_comunication::connection_failed;
    if(result_data.contains("connection not success")){
        process_result = database_comunication::connection_failed;
    }else if(result_data.contains("connection success")){
        process_result = database_comunication::connection_success;
    }
    emit processedAnswer(process_result);
}

void database_comunication::onPostAnswer(QNetworkReply *reply)
{
    //    qDebug() << reply->url().toString();
    //    const QList<QByteArray>& rawHeaderList(reply->rawHeaderList());
    //    foreach (QByteArray rawHeader, rawHeaderList) {
    //        qDebug() << reply->rawHeader(rawHeader);
    //    }
    QByteArray answer = reply->readAll();
    emit alredyAnswered(answer, tipoPeticion);
    networkManager->deleteLater();
}

QJsonObject database_comunication::getJsonObject(QByteArray byte_array)
{
    QJsonDocument d = QJsonDocument::fromJson(byte_array);
    if(d.isObject())
        return d.object();
    else
        return QJsonObject();
}

QJsonArray database_comunication::getJsonArray(QByteArray byte_array)
{
    QJsonDocument d = QJsonDocument::fromJson(byte_array);
    if(d.isArray())
        return d.array();
    else
        return QJsonArray();
}

QImage database_comunication::getImageFromString(QString str)
{
    QByteArray imageData = QByteArray::fromBase64(str.toUtf8());
    QImage img;
    if(img.loadFromData(imageData))
        return img;
    else
        return QImage();

}
