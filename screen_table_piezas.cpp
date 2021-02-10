#include "screen_table_piezas.h"
#include "ui_screen_table_piezas.h"
#include "cantidad_piezas.h"
#include <QMessageBox>
#include "globalfunctions.h"

Screen_Table_Piezas::Screen_Table_Piezas(QWidget *parent, bool show, bool adding_l) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Piezas)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Piezas");
    getPiezasFromServer(show);
    adding = adding_l;
    if(adding){
        ui->widget_adding->show();
        ui->pb_nueva_pieza->hide();
    }else{
        ui->widget_adding->hide();
        ui->pb_nueva_pieza->show();
    }
}

Screen_Table_Piezas::~Screen_Table_Piezas()
{
    delete ui;
}

void Screen_Table_Piezas::resizeEvent(QResizeEvent *event){

    QWidget::resizeEvent(event);
    if(timer.isActive()){
        timer.stop();
    }
    timer.setInterval(200);
    timer.start();
    connect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
}

void Screen_Table_Piezas::getPiezasFromServer(bool view)
{
    QStringList keys, values;
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    if(view){
        connect(this,SIGNAL(piezasReceived(database_comunication::serverRequestType)),
                this,SLOT(populateTable(database_comunication::serverRequestType)));
    }
    database_com.serverRequest(database_comunication::serverRequestType::GET_PIEZAS,keys,values);

}

void Screen_Table_Piezas::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
     Q_UNUSED(tipo);
    jsonArrayAllPiezas = ordenarPor(jsonArrayAllPiezas, codigo_pieza_piezas, "");
}

void Screen_Table_Piezas::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_pieza_piezas);
    mapa.insert("Pieza",pieza_piezas);
    mapa.insert("Estado",state_pieza);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Pieza" << "Estado";

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

void Screen_Table_Piezas::setTableView()
{
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
    timer.stop();
    if(model!=nullptr){
        ui->tableView->setModel(model);

        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->resizeColumnsToContents();
        float ancho = (float)(ui->tableView->width()-20)/3;
        ui->tableView->setColumnWidth(0, (int)(ancho * (float)1/3));
        ui->tableView->setColumnWidth(1, (int)(ancho * (float)5/3));
        ui->tableView->setColumnWidth(2, (int)(ancho * (float)2/3));
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->horizontalHeader()->setSectionsMovable(true);
        if(!connected_header_signal){
            connected_header_signal = true;
            connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
                    this, SLOT(on_sectionClicked(int)));
        }
    }
}

void Screen_Table_Piezas::on_sectionClicked(int logicalIndex)
{
    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_pieza_piezas);
    mapa.insert("Pieza",pieza_piezas);
    mapa.insert("Estado",state_pieza);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Pieza" << "Estado";

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);

    jsonArrayAllPiezas = ordenarPor(jsonArrayAllPiezas, ordenamiento, "");
}


void Screen_Table_Piezas::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_PIEZAS)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_piezas"))
        {
            result = database_comunication::script_result::get_piezas_failed;
        }
        else
        {
            jsonArrayAllPiezas = database_comunication::getJsonArray(byte_array);
            Pieza::writePiezas(jsonArrayAllPiezas);
            serverAlredyAnswered = true;
            emit piezasReceived(tipo);
            //        export_done_tasks_to_excel();
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Piezas::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Piezas::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa

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

void Screen_Table_Piezas::getPiezaAdded(const QString pieza_added)
{
    QStringList piezas_en_listWidget;
    if(ui->listWidget->count() != 0){
        for (int i =0 ; i < ui->listWidget->count(); i++) {
            QString pieza_l = ui->listWidget->item(i)->text();
            piezas_en_listWidget << pieza_l;
        }
        bool exist = false;
        for (int i =0 ; i < piezas_en_listWidget.size(); i++) {
            QString pieza_i = piezas_en_listWidget.at(i);
            QStringList pieza_i_splited = pieza_i.split("-");
            pieza_i_splited.removeFirst();
            QString pieza_i_sin_cant = pieza_i_splited.join("-").trimmed();
            if(pieza_added.contains(pieza_i_sin_cant)){ ///ya existe la pieza en la lista
                exist = true;
                int cant_ya_exitente = pieza_i.split("-").first().trimmed().toInt();
                int cant_agregada = pieza_added.split("-").first().trimmed().toInt();
                int total = cant_agregada + cant_ya_exitente;
                QString sustitucion = QString::number(total) + " - " + pieza_i_sin_cant;
                piezas_en_listWidget.replace(i, sustitucion);
                break;
            }
        }
        if(!exist){
            piezas_en_listWidget << pieza_added;
        }
    }
    else{
        piezas_en_listWidget << pieza_added;
    }
    ui->listWidget->clear();
    ui->listWidget->addItems(piezas_en_listWidget);
}

void Screen_Table_Piezas::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(adding){
        QJsonObject o = jsonArrayAllPiezas.at(index.row()).toObject();

        Cantidad_Piezas *cantidad = new Cantidad_Piezas(this, o.value(pieza_piezas).toString().trimmed());
        connect(cantidad, &Cantidad_Piezas::add_pieza,this,
                &Screen_Table_Piezas::getPiezaAdded);

        cantidad->exec();

    }
    else {
        onePiezaScreen = new Pieza(nullptr, false);
        connect(onePiezaScreen, &Pieza::update_tablePiezas,this,
                &Screen_Table_Piezas::getPiezasFromServer);
        QJsonObject o = jsonArrayAllPiezas.at(index.row()).toObject();
        onePiezaScreen->setData(o);
        onePiezaScreen->show();
    }
}

void Screen_Table_Piezas::addAlreadyAssingPiezas(QString piezas)
{
    ui->listWidget->addItems(piezas.split("\n"));
}

void Screen_Table_Piezas::on_pb_nueva_pieza_clicked()
{
    onePiezaScreen = new Pieza(nullptr, true);
    connect(onePiezaScreen, &Pieza::update_tablePiezas,this,
            &Screen_Table_Piezas::getPiezasFromServer);
    onePiezaScreen->show();
}

void Screen_Table_Piezas::on_pb_add_pieza_clicked()
{
    QStringList piezas_en_listWidget;

    for (int i =0 ; i < ui->listWidget->count(); i++) {
        QString pieza_l = ui->listWidget->item(i)->text();
        if(!pieza_l.trimmed().isEmpty()){
            piezas_en_listWidget << pieza_l;
        }
    }
    add_piezas(piezas_en_listWidget);
    this->close();
}

void Screen_Table_Piezas::on_pb_eliminar_todas_clicked()
{
    GlobalFunctions gf(this);
    if(gf.showQuestion(this,"Confirmación","Seguro desea eliminar todas las piezas?"
                          , QMessageBox::Ok, QMessageBox::No)== QMessageBox::Ok){
        ui->listWidget->clear();
    }
}

void Screen_Table_Piezas::on_pb_eliminar_una_clicked()
{
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        delete ui->listWidget->takeItem(ui->listWidget->row(item));
    }
}
