#include "screen_table_calibres.h"
#include "ui_screen_table_calibres.h"

Screen_Table_Calibres::Screen_Table_Calibres(QWidget *parent, bool show) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Calibres)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Calibres");
    getCalibresFromServer(show);
}

Screen_Table_Calibres::~Screen_Table_Calibres()
{
    delete ui;
}
void Screen_Table_Calibres::getCalibresFromServer(bool view)
{
    QStringList keys, values;
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    if(view){
        connect(this,SIGNAL(calibresReceived(database_comunication::serverRequestType)),
                this,SLOT(populateTable(database_comunication::serverRequestType)));
    }
    database_com.serverRequest(database_comunication::serverRequestType::GET_CALIBRES,keys,values);

}

void Screen_Table_Calibres::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
     Q_UNUSED(tipo);
    jsonArrayAllCalibres = ordenarPor(jsonArrayAllCalibres, codigo_calibre_calibres, "");
}

void Screen_Table_Calibres::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_calibre_calibres);
    mapa.insert("Calibre",calibre_calibres);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Calibre";

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

void Screen_Table_Calibres::setTableView()
{
    if(model!=nullptr){
        ui->tableView->setModel(model);

        //        ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->resizeColumnsToContents();
        float ancho = (float)(ui->tableView->width()-20)/2;
        ui->tableView->setColumnWidth(0, (int)(ancho * (float)1/2));
        ui->tableView->setColumnWidth(1, (int)(ancho * (float)1/2));
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->horizontalHeader()->setSectionsMovable(true);
        if(!connected_header_signal){
            connected_header_signal = true;
            connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
                    this, SLOT(on_sectionClicked(int)));
        }
    }
}

void Screen_Table_Calibres::on_sectionClicked(int logicalIndex)
{
    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_calibre_calibres);
    mapa.insert("Calibre",calibre_calibres);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Calibre";

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);

    jsonArrayAllCalibres = ordenarPor(jsonArrayAllCalibres, ordenamiento, "");
}


void Screen_Table_Calibres::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_CALIBRES)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_calibres"))
        {
            result = database_comunication::script_result::get_calibres_failed;
        }
        else
        {
            jsonArrayAllCalibres = database_comunication::getJsonArray(byte_array);
            Calibre::writeCalibres(jsonArrayAllCalibres);
            serverAlredyAnswered = true;
            emit calibresReceived(tipo);
            //        export_done_tasks_to_excel();
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Calibres::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Calibres::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa

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

void Screen_Table_Calibres::on_tableView_doubleClicked(const QModelIndex &index)
{
    oneCalibreScreen = new Calibre(nullptr, false);
    connect(oneCalibreScreen, &Calibre::update_tableCalibres,this,
            &Screen_Table_Calibres::getCalibresFromServer);
    QJsonObject o = jsonArrayAllCalibres.at(index.row()).toObject();
    oneCalibreScreen->setData(o);
    oneCalibreScreen->show();
}
void Screen_Table_Calibres::on_pb_nueva_clicked()
{

    oneCalibreScreen = new Calibre(nullptr, true);
    connect(oneCalibreScreen, &Calibre::update_tableCalibres,this,
            &Screen_Table_Calibres::getCalibresFromServer);
    oneCalibreScreen->show();
}
