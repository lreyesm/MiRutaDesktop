#include "screen_table_info.h"
#include "ui_screen_table_info.h"

#include "screen_table_info.h"
#include "ui_screen_table_info.h"

Screen_Table_Info::Screen_Table_Info(QWidget *parent, bool show) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Info)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Info");
    getInfoFromServer(show);
}

Screen_Table_Info::~Screen_Table_Info()
{
    delete ui;
}
void Screen_Table_Info::getInfoFromServer(bool view)
{
    QStringList keys, values;
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    if(view){
        connect(this,SIGNAL(infoReceived(database_comunication::serverRequestType)),
                this,SLOT(populateTable(database_comunication::serverRequestType)));
    }
    database_com.serverRequest(database_comunication::serverRequestType::GET_INFOS,keys,values);

}

void Screen_Table_Info::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
    jsonArrayAllInfo = ordenarPor(jsonArrayAllInfo, codigo_info_info, "");
}

void Screen_Table_Info::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=NULL)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_info_info);
    mapa.insert("Info",info_info);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Info";

    model = new QStandardItemModel(rows, listHeaders.size());
    model->setHorizontalHeaderLabels(listHeaders);

    //insertando los datos
    QStandardItem *item;
    QString column_info;
    for(int i = 0; i < rows; i++)
    {
        for (int n = 0; n < listHeaders.size(); n++) {
            item = new QStandardItem();
            column_info = jsonArray[i].toObject().value(mapa.value(listHeaders.at(n))).toString();
            //            item->setData(column_info,Qt::EditRole);
            QModelIndex index = model->index(i, n, QModelIndex());
            model->setData(index, column_info);
        }
    }
}

void Screen_Table_Info::setTableView()
{
    if(model!=NULL){
        ui->tableView->setModel(model);

        //        ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->resizeColumnsToContents();
        float ancho = (float)(ui->tableView->width()-20)/3;
        ui->tableView->setColumnWidth(0, (int)(ancho * (float)2/4));
        ui->tableView->setColumnWidth(1, (int)(ancho * (float)2/4));
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->horizontalHeader()->setSectionsMovable(true);
        if(!connected_header_signal){
            connected_header_signal = true;
            connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
                    this, SLOT(on_sectionClicked(int)));
        }
    }
}

void Screen_Table_Info::on_sectionClicked(int logicalIndex)
{
    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_info_info);
    mapa.insert("Info",info_info);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Info";

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);

    ordenarPor(jsonArrayAllInfo, ordenamiento, "");
}


void Screen_Table_Info::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_INFOS)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_info"))
        {
            result = database_comunication::script_result::get_info_failed;
        }
        else
        {
            jsonArrayAllInfo = database_comunication::getJsonArray(byte_array);
            Info::writeInfo(jsonArrayAllInfo);
            serverAlredyAnswered = true;
            emit infoReceived(tipo);
            //        export_done_tasks_to_excel();
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Info::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Info::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa

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

void Screen_Table_Info::on_tableView_doubleClicked(const QModelIndex &index)
{
    oneInfoScreen = new Info(nullptr, false);
    connect(oneInfoScreen, &Info::update_tableInfo,this,
            &Screen_Table_Info::getInfoFromServer);
    QJsonObject o = jsonArrayAllInfo.at(index.row()).toObject();
    oneInfoScreen->setData(o);
    oneInfoScreen->show();
}
void Screen_Table_Info::on_pb_nueva_clicked()
{

    oneInfoScreen = new Info(nullptr, true);
    connect(oneInfoScreen, &Info::update_tableInfo,this,
            &Screen_Table_Info::getInfoFromServer);
    oneInfoScreen->show();
}
