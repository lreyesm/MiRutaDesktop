#include "screen_table_infos.h"
#include "ui_screen_table_infos.h"

Screen_Table_Infos::Screen_Table_Infos(QWidget *parent, bool show, QString empresa) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Infos)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Infos");
    this->empresa = empresa;
    getInfosFromServer(show);
}

Screen_Table_Infos::~Screen_Table_Infos()
{
    delete ui;
}
void Screen_Table_Infos::getInfosFromServer(bool view)
{
    QStringList keys, values;
    keys << "empresa";
    values << empresa.toLower();
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    if(view){
        connect(this,SIGNAL(infosReceived(database_comunication::serverRequestType)),
                this,SLOT(populateTable(database_comunication::serverRequestType)));
    }
    database_com.serverRequest(database_comunication::serverRequestType::GET_INFOS,keys,values);

}

void Screen_Table_Infos::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
     Q_UNUSED(tipo);
    jsonArrayAllInfos = ordenarPor(jsonArrayAllInfos, codigo_info_infos, "");
}

void Screen_Table_Infos::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Código",codigo_info_infos);
    mapa.insert("Info",info_infos);

    QStringList listHeaders;
    listHeaders <<"Código" << "Info";

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

void Screen_Table_Infos::setTableView()
{
    if(model!=nullptr){
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

void Screen_Table_Infos::on_sectionClicked(int logicalIndex)
{
    QMap <QString,QString> mapa;
    mapa.insert("Código",codigo_info_infos);
    mapa.insert("Info",info_infos);

    QStringList listHeaders;
    listHeaders <<"Código" << "Info";

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);

    jsonArrayAllInfos = ordenarPor(jsonArrayAllInfos, ordenamiento, "");
}


void Screen_Table_Infos::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_INFOS)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_infos"))
        {
            result = database_comunication::script_result::get_infos_failed;
        }
        else
        {
            jsonArrayAllInfos = database_comunication::getJsonArray(byte_array);
            Info::writeInfos(jsonArrayAllInfos);
            serverAlredyAnswered = true;
            emit infosReceived(tipo);
            //        export_done_tasks_to_excel();
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Infos::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Infos::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa

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

void Screen_Table_Infos::on_tableView_doubleClicked(const QModelIndex &index)
{
    oneInfoScreen = new Info(nullptr, false, empresa);
    connect(oneInfoScreen, &Info::update_tableInfos,this,
            &Screen_Table_Infos::getInfosFromServer);
    QJsonObject o = jsonArrayAllInfos.at(index.row()).toObject();
    oneInfoScreen->setData(o);
    oneInfoScreen->show();
}
void Screen_Table_Infos::on_pb_nueva_clicked()
{

    oneInfoScreen = new Info(nullptr, true, empresa);
    connect(oneInfoScreen, &Info::update_tableInfos,this,
            &Screen_Table_Infos::getInfosFromServer);
    oneInfoScreen->show();
}
