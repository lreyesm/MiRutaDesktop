#include "screen_table_clases.h"
#include "ui_screen_table_clases.h"

Screen_Table_Clases::Screen_Table_Clases(QWidget *parent, bool show) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Clases)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Clases");
    getClasesFromServer(show);
}

Screen_Table_Clases::~Screen_Table_Clases()
{
    delete ui;
}
void Screen_Table_Clases::getClasesFromServer(bool view)
{
    QStringList keys, values;
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    if(view){
        connect(this,SIGNAL(clasesReceived(database_comunication::serverRequestType)),
                this,SLOT(populateTable(database_comunication::serverRequestType)));
    }
    database_com.serverRequest(database_comunication::serverRequestType::GET_CLASES,keys,values);

}

void Screen_Table_Clases::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
     Q_UNUSED(tipo);
    jsonArrayAllClases = ordenarPor(jsonArrayAllClases, codigo_clase_clases, "");
}

void Screen_Table_Clases::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=NULL)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_clase_clases);
    mapa.insert("Clase",clase_clases);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Clase";

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

void Screen_Table_Clases::setTableView()
{
    if(model!=NULL){
        ui->tableView->setModel(model);

        //        ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->resizeColumnsToContents();
        float ancho = (float)(ui->tableView->width()-20)/4;
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

void Screen_Table_Clases::on_sectionClicked(int logicalIndex)
{
    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_clase_clases);
    mapa.insert("Clase",clase_clases);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Clase";

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);

    jsonArrayAllClases = ordenarPor(jsonArrayAllClases, ordenamiento, "");
}


void Screen_Table_Clases::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_CLASES)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_clases"))
        {
            result = database_comunication::script_result::get_clases_failed;
        }
        else
        {
            jsonArrayAllClases = database_comunication::getJsonArray(byte_array);
            Clase::writeClases(jsonArrayAllClases);
            serverAlredyAnswered = true;
            emit clasesReceived(tipo);
            //        export_done_tasks_to_excel();
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Clases::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Clases::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa
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

void Screen_Table_Clases::on_tableView_doubleClicked(const QModelIndex &index)
{
    oneClaseScreen = new Clase(nullptr, false);
    connect(oneClaseScreen, &Clase::update_tableClases,this,
            &Screen_Table_Clases::getClasesFromServer);
    QJsonObject o = jsonArrayAllClases.at(index.row()).toObject();
    oneClaseScreen->setData(o);
    oneClaseScreen->show();
}
void Screen_Table_Clases::on_pb_nueva_clicked()
{

    oneClaseScreen = new Clase(nullptr, true);
    connect(oneClaseScreen, &Clase::update_tableClases,this,
            &Screen_Table_Clases::getClasesFromServer);
    oneClaseScreen->show();
}
