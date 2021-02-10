#include "screen_table_observaciones.h"
#include "ui_screen_table_observaciones.h"

Screen_Table_Observaciones::Screen_Table_Observaciones(QWidget *parent, bool show) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Observaciones)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Observaciones");
    getObservacionesFromServer(show);
}

Screen_Table_Observaciones::~Screen_Table_Observaciones()
{
    delete ui;
}

void Screen_Table_Observaciones::resizeEvent(QResizeEvent *event){

    QWidget::resizeEvent(event);
    if(timer.isActive()){
        timer.stop();
    }
    timer.setInterval(200);
    timer.start();
    connect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
}

void Screen_Table_Observaciones::getObservacionesFromServer(bool view)
{
    QStringList keys, values;
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    if(view){
        connect(this,SIGNAL(observacionesReceived(database_comunication::serverRequestType)),
                this,SLOT(populateTable(database_comunication::serverRequestType)));
    }
    database_com.serverRequest(database_comunication::serverRequestType::GET_OBSERVACIONES,keys,values);

}

void Screen_Table_Observaciones::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
     Q_UNUSED(tipo);
    jsonArrayAllObservaciones = ordenarPor(jsonArrayAllObservaciones, codigo_observacion_observaciones, "");
}

void Screen_Table_Observaciones::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=NULL)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_observacion_observaciones);
    mapa.insert("Observacion",observacion_observaciones);
    mapa.insert("Origen",origen_observaciones);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Observacion" << "Origen";

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

void Screen_Table_Observaciones::setTableView()
{
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
    timer.stop();
    if(model!=NULL){
        ui->tableView->setModel(model);

        //        ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
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

void Screen_Table_Observaciones::on_sectionClicked(int logicalIndex)
{
    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_observacion_observaciones);
    mapa.insert("Observacion",observacion_observaciones);
    mapa.insert("Origen",origen_observaciones);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Observacion"<< "Origen";

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);

    jsonArrayAllObservaciones = ordenarPor(jsonArrayAllObservaciones, ordenamiento, "");
}


void Screen_Table_Observaciones::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_OBSERVACIONES)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_observaciones"))
        {
            result = database_comunication::script_result::get_observaciones_failed;
        }
        else
        {
            jsonArrayAllObservaciones = database_comunication::getJsonArray(byte_array);
            Observacion::writeObservaciones(jsonArrayAllObservaciones);
            serverAlredyAnswered = true;
            emit observacionesReceived(tipo);
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Observaciones::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Observaciones::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa
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

void Screen_Table_Observaciones::on_tableView_doubleClicked(const QModelIndex &index)
{
    oneObservacionScreen = new Observacion(nullptr, false);
    connect(oneObservacionScreen, &Observacion::update_tableObservaciones,this,
            &Screen_Table_Observaciones::getObservacionesFromServer);
    QJsonObject o = jsonArrayAllObservaciones.at(index.row()).toObject();
    oneObservacionScreen->setData(o);
    oneObservacionScreen->show();
}
void Screen_Table_Observaciones::on_pb_nueva_clicked()
{

    oneObservacionScreen = new Observacion(nullptr, true);
    connect(oneObservacionScreen, &Observacion::update_tableObservaciones,this,
            &Screen_Table_Observaciones::getObservacionesFromServer);
    oneObservacionScreen->show();
}
