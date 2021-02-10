#include "screen_table_rutas.h"
#include "ui_screen_table_rutas.h"
#include <QtXlsx>
#include <QtXlsx/xlsxformat.h>
#include <QFileDialog>
#include "global_variables.h"
#include "QProgressIndicator.h"

#include "globalfunctions.h"
#include "rightclickedsection.h"
#include <QCompleter>
#include <QScrollArea>
#include "mycheckbox.h"
#include "mylineeditshine.h"
#include <QDesktopWidget>
#include <QScreen>
#include <QGraphicsDropShadowEffect>
Screen_Table_Rutas::Screen_Table_Rutas(QWidget *parent, bool show) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Rutas)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Rutas");
    setWindowFlags(Qt::CustomizeWindowHint);

    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));

    connect(ui->l_current_pagination, &MyLabelSpinner::itemSelected,
            this, &Screen_Table_Rutas::moveToPage);
    connect(this, &Screen_Table_Rutas::mouse_pressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Screen_Table_Rutas::sectionPressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Screen_Table_Rutas::closing, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Screen_Table_Rutas::tablePressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);

    ui->statusbar->hide();
}

Screen_Table_Rutas::~Screen_Table_Rutas()
{
    delete ui;
}

void Screen_Table_Rutas::showEvent(QShowEvent *event){
    QWidget::showEvent(event);
    QTimer::singleShot(200, this, &Screen_Table_Rutas::getRutas);
}
void Screen_Table_Rutas::resizeEvent(QResizeEvent *event){

    QWidget::resizeEvent(event);
    if(timer.isActive()){
        timer.stop();
    }
    timer.setInterval(200);
    timer.start();
    connect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
}

void Screen_Table_Rutas::getRutas()
{
    show_loading("Descargando Rutas...");

    filtering = false;
    currentPage = 1;
    QString query = defaultQuery;
    getRutasCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;
    populateTable(database_comunication::GET_RUTAS);

    hide_loading();
}
void Screen_Table_Rutas::show_loading(QString mess){
    emit hidingLoading();
    QWidget *widget_blur = new QWidget(this);
    widget_blur->setFixedSize(this->size()+QSize(0,0));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &Screen_Table_Rutas::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &Screen_Table_Rutas::hidingLoading, widget_blur, &QWidget::deleteLater);

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
    connect(this, &Screen_Table_Rutas::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &Screen_Table_Rutas::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &Screen_Table_Rutas::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &Screen_Table_Rutas::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &Screen_Table_Rutas::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}

void Screen_Table_Rutas::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void Screen_Table_Rutas::hide_loading(){
    emit hidingLoading();
}

QJsonArray Screen_Table_Rutas::loadRutasExcel(){
    QString path = QFileDialog::getOpenFileName(this, "Seleccione excel con rutas", "", "Datos (*.xlsx *.xls)");

    if(path.isEmpty()){
        return QJsonArray();
    }
    QXlsx::Document xlsx(path);
    QString sheetName="";
    QJsonArray jsonArray;
    foreach(sheetName, xlsx.sheetNames()){
        xlsx.selectSheet(sheetName);
        QStringList listHeaders, row_content;
        QList<QStringList> lista;
        //        QFile file(path);
        //        file.open(QIODevice::ReadOnly);
        int number_of_row = xlsx.dimension().lastRow();
        int number_of_column = xlsx.dimension().lastColumn();

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
        QMap<QString, QString> map = mapExcelImport(listHeaders);
        //read data
        for(int i = 1; i <= number_of_row; i++)
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

        //OJO debo hacer una comprobación de los encabezados
        //-2 por los encabezados y fila de filtros y -1 por la fila de resumen al final
        for(int i=0; i < number_of_row - 1; i++)//**************revisar que no da bateo tenia un -2 antes
        {
            QJsonObject o;
            row_content = lista.at(i);
            if(row_content.at(0).trimmed().isEmpty() && row_content.at(1).trimmed().isEmpty()){
                ///Se salta las filas vacias
                continue;
            }
            for(int j = 0; j < number_of_column; j++)
            {
                QString header = listHeaders.at(j).toUpper().trimmed();
                if(checkIfFieldIsValid(header) && map.keys().contains(header)){
                    QString value_header = map.value(header);
                    QString contenido_en_excel = row_content.at(j);
                    o.insert(value_header,QJsonValue(contenido_en_excel));
                }
            }

            if(checkValidJsonObjectFields(o)){
                o.insert(date_time_modified_rutas, QDateTime::currentDateTime().toString(formato_fecha_hora));

                QString zona = o.value(barrio_rutas).toString().trimmed();
                QStringList split = zona.split(" ");
                if(split.size() > 1){
                    if(split.at(0).contains(".")){
                        QString cod = split.at(0);
                        split.removeFirst();
                        zona = (cod + " - " + split.join(" ")).trimmed();
                    }
                }
                o.insert(municipio_rutas, "BILBAO");
                o.insert(barrio_rutas, zona);

                QString ruta_l = o.value(ruta_rutas).toString().trimmed();
                QString portal_l = o.value(portal_rutas).toString().trimmed();
                portal_l = QString::number((portal_l.toInt()));

                o.insert(codigo_ruta_rutas, ruta_l + "." + portal_l);

                o.remove("");
                if(!o.isEmpty()){
                    jsonArray.append(o);
                }
            }
        }
    }
    return jsonArray;
}
QMap<QString, QString> Screen_Table_Rutas::mapExcelImport(QStringList listHeaders){
    Q_UNUSED(listHeaders);
    QMap<QString, QString> map;
    //campos de excel de entrada
    map.insert("HILOLECTCABB",ruta_rutas);
    map.insert("NOMBRECALLE",calle_rutas);
    map.insert("NUM",portal_rutas);
    map.insert("BARRIO",barrio_rutas);
    map.insert("DISTRITO",distrito_rutas);
    map.insert("PORTAL CON RADIO",radio_portal_rutas);

    return map;
}
bool Screen_Table_Rutas::checkValidJsonObjectFields(QJsonObject jsonObject){
    if(!checkIfFieldIsValid(jsonObject.value(barrio_rutas).toString())
            || !checkIfFieldIsValid(jsonObject.value(ruta_rutas).toString())
            || !checkIfFieldIsValid(jsonObject.value(portal_rutas).toString())){
        return false;
    }
    return true;
}
bool Screen_Table_Rutas::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
void Screen_Table_Rutas::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
    Q_UNUSED(tipo);
    fixModelForTable(jsonArrayAll);
    setTableView();
    hide_loading();
}

void Screen_Table_Rutas::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    addItemsToPaginationInfo(rows);

    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_ruta_rutas);
    mapa.insert("Calle",calle_rutas);
    mapa.insert("Sector P",barrio_rutas);
    mapa.insert("Radio",radio_portal_rutas);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Calle" << "Sector P" << "Radio";

    model = new QStandardItemModel(rows, listHeaders.size());
    model->setHorizontalHeaderLabels(listHeaders);

    //insertando los datos
    QString column_info;
    for(int i = 0; i < rows; i++)
    {
        for (int n = 0; n < listHeaders.size(); n++) {
            column_info = jsonArray[i].toObject().value(mapa.value(listHeaders.at(n))).toString();
            //            item->setData(column_info,Qt::EditRole);
            QModelIndex index = model->index(i, n, QModelIndex());
            model->setData(index, column_info);
        }
    }
}

void Screen_Table_Rutas::setTableView()
{
    timer.stop();
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
    if(model!=nullptr){
        ui->tableView->setModel(model);

        //        ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

        float ancho = (float)(ui->tableView->width()-100)/4;
        ui->tableView->setColumnWidth(0, (int)(ancho * (float)1));
        ui->tableView->setColumnWidth(1, (int)(ancho * (float)1.5));
        ui->tableView->setColumnWidth(2, (int)(ancho * (float)1));
        ui->tableView->setColumnWidth(3, (int)(ancho * (float)0.5));

        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->horizontalHeader()->setSectionsMovable(true);
        if(!connected_header_signal){
            connected_header_signal = true;
            connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
                    this, SLOT(on_sectionClicked(int)));
        }
    }
}
QMap <QString,QString> Screen_Table_Rutas::fillMapForFixModel(QStringList &listHeaders){
    QMap <QString,QString>  mapa;
    mapa.insert("Codigo",codigo_ruta_rutas);
    mapa.insert("Calle",calle_rutas);
    mapa.insert("Sector P",barrio_rutas);
    mapa.insert("Radio",radio_portal_rutas);

    listHeaders <<"Codigo" << "Calle" << "Sector P" << "Radio";
    return mapa;
}

void Screen_Table_Rutas::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_RUTAS)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_rutas"))
        {
            result = database_comunication::script_result::get_rutas_failed;
        }
        else
        {
            jsonArrayAll = database_comunication::getJsonArray(byte_array);
            Ruta::writeRutas(jsonArrayAll);
            serverAlredyAnswered = true;
            emit rutasReceived(tipo);
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
            jsonInfoRutasAmount = database_comunication::getJsonObject(byte_array);
            qDebug()<<"query return -> "<<jsonInfoRutasAmount.value("query").toString();
            QString count_rutas = jsonInfoRutasAmount.value("count_rutas").toString();
            countRutas = count_rutas.toInt();
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
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Rutas::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Rutas::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa

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

void Screen_Table_Rutas::on_tableView_doubleClicked(const QModelIndex &index)
{
    oneRutaScreen = new Ruta(nullptr, false);
    connect(oneRutaScreen, &Ruta::update_tableRutas,this,
            &Screen_Table_Rutas::updateRutasInTable);
    QJsonObject o = jsonArrayAll.at(index.row()).toObject();
    oneRutaScreen->setData(o);
    oneRutaScreen->show();
}
void Screen_Table_Rutas::on_pb_nueva_clicked()
{
    oneRutaScreen = new Ruta(nullptr, true);
    connect(oneRutaScreen, &Ruta::update_tableRutas,this,
            &Screen_Table_Rutas::updateRutasInTable);
    oneRutaScreen->show();
}

void Screen_Table_Rutas::cargarDesdeExcel(){
    QJsonArray jsonArray = loadRutasExcel();
    oneRutaScreen = new Ruta(nullptr, true);
    int total = jsonArray.size();
    for (int i =0; i < total; i++) {
        QJsonObject o = jsonArray.at(i).toObject();
        oneRutaScreen->setData(o);
        oneRutaScreen->subirRuta(o.value(codigo_ruta_rutas).toString());
        ui->statusbar->showMessage("Subiendo -> "+  QString::number(i) +"/"+ QString::number(total));
    }
    ui->statusbar->showMessage("Subidas Correctamente", 5000);

    updateRutasInTable();
}
void Screen_Table_Rutas::fixRutasWrongCode(){

//    QJsonArray jsonArray = Ruta::readRutas();
//    oneRutaScreen = new Ruta(nullptr, false);
//    int total = jsonArray.size();
//    for (int i =0; i < total; i++) {
//        QJsonObject o = jsonArray.at(i).toObject();
//        QString ruta = o.value(ruta_rutas).toString();
//        if(ruta.size() < 6){
//            QString old_cod, cod = o.value(codigo_ruta_rutas).toString();
//            old_cod = cod;
//            cod.prepend("0");
//            ruta.prepend("0");
//            o.insert(codigo_ruta_rutas, cod);
//            o.insert(ruta_rutas, ruta);

//            oneRutaScreen->setData(o);
//            oneRutaScreen->subirRuta(cod);

//            o.insert(codigo_ruta_rutas, old_cod);
//            oneRutaScreen->setData(o);
//            oneRutaScreen->eliminarRuta(old_cod);
//        }
//        ui->statusbar->showMessage("Corrigiendo: " + QString::number(i+1)+ "/" + QString::number(total));
//    }
}

void Screen_Table_Rutas::moveToPage(QString page){
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
    if(jsonInfoRutasAmount.contains("id_" + key_id_string)){
        id_start = jsonInfoRutasAmount.value("id_" + key_id_string).toString().toInt();
    }

    QString query = lastQuery;
    getRutasCustomQuery(query, id_start);
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

void Screen_Table_Rutas::addItemsToPaginationInfo(int sizeShowing){
    QStringList keys = jsonInfoRutasAmount.keys();
    keys.sort();
    int paginas = 0;
    QStringList items;
    for (int i=0; i < keys.size(); i++) {
        QString key = keys.at(i);
        if(key.contains("id_")){
            paginas++;
            qDebug()<< key << " -> "<< jsonInfoRutasAmount.value(key).toString();
            items.append(QString::number(paginas));
        }
    }
    ui->l_current_pagination->addItems(items);
    currentPages = paginas;
    ui->l_current_pagination->setText(QString::number(currentPage) + " / "
                                      + ((currentPages==0)?"1":QString::number(currentPages)));
    ui->l_current_pagination->hideSpinnerList();

    ui->l_cantidad_de_rutas->setText("Mostrando "
                                     + (QString::number(((currentPage - 1) * limit_pagination) + 1) + "-"
                                        + (QString::number(sizeShowing + ((currentPage - 1) * limit_pagination)))
                                        + " de " + QString::number(countRutas))
                                     + " " + ((sizeShowing != 1)?"rutas":"contador"));
    checkPaginationButtons();
}

void Screen_Table_Rutas::checkPaginationButtons(){
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
void Screen_Table_Rutas::on_pb_next_pagination_clicked()
{
    currentPage++;
    moveToPage(QString::number(currentPage));
}

void Screen_Table_Rutas::on_pb_previous_pagination_clicked()
{
    currentPage--;
    moveToPage(QString::number(currentPage));
}


void Screen_Table_Rutas::get_rutas_amount_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_RUTAS_AMOUNT,keys,values);
}
void Screen_Table_Rutas::get_rutas_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_RUTAS_CUSTOM_QUERY,keys,values);
}
void Screen_Table_Rutas::get_all_column_values_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES,keys,values);
}
void Screen_Table_Rutas::get_all_column_values_custom_query_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES_CUSTOM_QUERY,keys,values);
}

bool Screen_Table_Rutas::getRutasCustomQuery(QString query, int id_start){

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
bool Screen_Table_Rutas::getRutasFromServer(QString query, int limit, int id_start)
{
    QStringList keys, values;

    keys << "query" << "LIMIT" << "id_start";
    values << query << QString::number(limit) << QString::number(id_start);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_Table_Rutas::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_Table_Rutas::get_rutas_request);

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

    case database_comunication::script_result::get_rutas_custom_query_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}
bool Screen_Table_Rutas::getRutasAmountFromServer( QString query, int limit)
{
    QStringList keys, values;

    keys << "query" << "LIMIT";
    values << query << QString::number(limit);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_Table_Rutas::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_Table_Rutas::get_rutas_amount_request);

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

    case database_comunication::script_result::get_rutas_amount_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}

bool Screen_Table_Rutas::getRutasValuesFieldServer(QString column)
{
    bool res = false;
    QStringList keys, values;

    keys << "columna" << "tabla";
    values << column << "rutas";

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &Screen_Table_Rutas::script_excecution_result,q,&QEventLoop::exit);

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
bool Screen_Table_Rutas::getRutasValuesFieldCustomQueryServer(QString column, QString query)
{
    bool res = false;
    QStringList keys, values;

    keys << "columna" << "tabla" << "query";
    values << column << "rutas" << query;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &Screen_Table_Rutas::script_excecution_result,q,&QEventLoop::exit);

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

QJsonArray Screen_Table_Rutas::getCurrentJsonArrayInTable(){
    QJsonArray jsonArray;

    if(filtering){
        jsonArray = jsonArrayInTableFiltered;
    }else{
        jsonArray = jsonArrayInTable;
    }
    return jsonArray;
}

QJsonArray Screen_Table_Rutas::fixJsonToLastModel(QJsonArray jsonArray){ //recibe el arreglo mostrado en la tabla entes de la descarga
    QStringList principal_var_all, principal_var_model;
    for (int i=0; i < jsonArray.size(); i++) { //obteniendo NUMIN de todos los json mostrados
        principal_var_model << jsonArray[i].toObject().value(codigo_ruta_rutas).toString();
    }
    for (int i=0; i < jsonArrayAll.size(); i++) { //obteniendo NUMIN de todos los json descargados
        principal_var_all << jsonArrayAll[i].toObject().value(codigo_ruta_rutas).toString();
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
                                            .value(codigo_ruta_rutas).toString())){
                jsonArray_model.append(jsonArray[i].toObject());
            }
        }
        return jsonArray_model;
    }else {
        return jsonArray;
    }
}

void Screen_Table_Rutas::updateRutasInTable(bool b){
    Q_UNUSED(b);
    if(this->isHidden()){
        return;
    }
    QString query = lastQuery;
    int id_start = last_id_start;
    getRutasCustomQuery(query, id_start);

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

QString Screen_Table_Rutas::getQueyStatus(){
    QString queryStatus = defaultQuery;
    QString query = " (" + queryStatus + ") ";
    return query;
}
void Screen_Table_Rutas::on_sectionClicked(int logicalIndex)
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
    connect(this, &Screen_Table_Rutas::closing, rightClickedSection, &RightClickedSection::deleteLater);
    connect(this, &Screen_Table_Rutas::tablePressed, rightClickedSection, &RightClickedSection::deleteLater);
    connect(this, &Screen_Table_Rutas::mouse_pressed, rightClickedSection, &RightClickedSection::deleteLater);

    rightClickedSection->show();
}
void Screen_Table_Rutas::filtrarEnTabla(bool checked){
    solo_tabla_actual = checked;
    emit sectionPressed();

    showFilterWidgetOptions(false);
}
QStringList Screen_Table_Rutas::getFieldValues(QString field){

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
            }
        }
    }else{
        show_loading("Cargando valores...");
        QString queryStatus = getQueyStatus();
        QString query = " (" + queryStatus + ") ";
        bool res = getRutasValuesFieldCustomQueryServer(
                    field, query);
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
void Screen_Table_Rutas::filterColumnField(){

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
            QString value = jsonObject.value(lastSectionField).toString().trimmed();
            if(checkIfFieldIsValid(value) && filterColumnList.contains(value, Qt::CaseInsensitive)){
                jsonArrayInTableFiltered.append(jsonObject);
            }
        }
        jsonInfoRutasAmount = QJsonObject();
        countRutas = jsonArrayInTableFiltered.size();
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
        getRutasCustomQuery(query);
        jsonArrayInTableFiltered = jsonArrayAll;
    }

    filtering = true;
    filterColumnList.clear();
    fixModelForTable(jsonArrayInTableFiltered);
    setTableView();
    hide_loading();
    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
}
QString Screen_Table_Rutas::getScrollBarStyle(){
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

void Screen_Table_Rutas::showFilterWidgetOptions(bool offset){

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
    connect(cb_tabla_actual, &QCheckBox::toggled, this, &Screen_Table_Rutas::filtrarEnTabla);

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
        connect(cb, &MyCheckBox::toggleCheckBox, this, &Screen_Table_Rutas::addRemoveFilterList);
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
    connect(button_filter, &QPushButton::clicked, this, &Screen_Table_Rutas::filterColumnField);

    QFont font =  ui->tableView->font();
    font.setBold(true);
    font.setPointSize(10);

    button_filter->setFont(font);

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
        QRect rect = this->geometry();
        if(lastCursorPos.x() > rect.width()/2){
            lastCursorPos.setX(lastCursorPos.x()-widget->width());
        }
    }
    widget->move(lastCursorPos);
    widget->show();

    connect(button_filter, &QPushButton::clicked, widget, &QWidget::deleteLater);
    connect(this, &Screen_Table_Rutas::sectionPressed, widget, &QWidget::deleteLater);
    connect(this, &Screen_Table_Rutas::closing, widget, &QWidget::deleteLater);
    connect(this, &Screen_Table_Rutas::tablePressed, widget, &QWidget::deleteLater);
}

void Screen_Table_Rutas::addRemoveFilterList(QString value){
    if(filterColumnList.contains(value)){
        filterColumnList.removeOne(value);
    }else{
        filterColumnList << value;
    }
}

void Screen_Table_Rutas::getMenuSectionClickedItem(int selection){
    if(selection == RightClickedSection::FILTRAR){
        showFilterWidgetOptions();
    }
    else if(selection == RightClickedSection::ORDENAR_ASCENDENTE){
        if(filtering){
            jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionField, "");
        }else{
            jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionField, "");
        }
    }
    else if(selection == RightClickedSection::ORDENAR_DESCENDENTE){
        if(filtering){
            jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionField, "MAYOR_MENOR");
        }else{
            jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionField, "MAYOR_MENOR");
        }
    }
}

void Screen_Table_Rutas::on_drag_screen(){

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

void Screen_Table_Rutas::on_start_moving_screen_timeout(){

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

void Screen_Table_Rutas::on_drag_screen_released()
{
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
}

void Screen_Table_Rutas::on_tableView_pressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    emit tablePressed();
}
void Screen_Table_Rutas::on_pb_cruz_clicked()
{
    emit closing();
    model->clear();
    this->close();
}

void Screen_Table_Rutas::on_pb_maximize_clicked()
{
    if(isMaximized()){
        this->showNormal();
    }else {
        this->showMaximized();
    }
}

void Screen_Table_Rutas::on_pb_minimize_clicked()
{
    this->showMinimized();
}

void Screen_Table_Rutas::on_pb_inicio_clicked()
{
    getRutas();
}
