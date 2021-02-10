#include "screen_table_equipo_operarios.h"
#include "ui_screen_table_equipo_operarios.h"

Screen_Table_Equipo_Operarios::Screen_Table_Equipo_Operarios(QWidget *parent, bool show, QString empresa) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Equipo_Operarios)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Equipo de Fontaneros");
    this->empresa = empresa;
    getEquipo_OperariosFromServer(show);
}

Screen_Table_Equipo_Operarios::~Screen_Table_Equipo_Operarios()
{
    delete ui;
}
void Screen_Table_Equipo_Operarios::getEquipo_OperariosFromServer(bool view)
{
    QStringList keys, values;
    keys << "empresa";
    values << empresa.toLower();
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    if(view){
        connect(this,SIGNAL(equipo_operariosReceived(database_comunication::serverRequestType)),
                this,SLOT(populateTable(database_comunication::serverRequestType)));
    }
    database_com.serverRequest(database_comunication::serverRequestType::GET_EQUIPO_OPERARIOS,keys,values);

}

void Screen_Table_Equipo_Operarios::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
     Q_UNUSED(tipo);
    jsonArrayAllEquipo_Operarios = ordenarPor(jsonArrayAllEquipo_Operarios, codigo_equipo_operario_equipo_operarios, "");
}

void Screen_Table_Equipo_Operarios::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=NULL)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_equipo_operario_equipo_operarios);
    mapa.insert("Equipo de Fontaneros",equipo_operario_equipo_operarios);
    mapa.insert("Encargado",nombre_encargado_equipo_operarios);
    mapa.insert("Teléfono",telefono_equipo_operarios);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Equipo de Fontaneros" << "Encargado" << "Teléfono";

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

void Screen_Table_Equipo_Operarios::setTableView()
{
    if(model!=NULL){
        ui->tableView->setModel(model);
        //        ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
//        ui->tableView->resizeColumnsToContents();

        QList<double> sizes;
        sizes << 0.5/*Codigo*/ << 1.2 /*Equipo*/<< 1/*Encargado*/<< 1/*Telefono*/;

        int width_table = ui->tableView->size().width() - 20;
        float medium_width_fileds = static_cast<float>(width_table)/4/*fields_count_in_table*/;

        for (int i=0; i < 4; i++) {
            //if( ui->tableView->columnWidth(i) > static_cast<int>(medium_width_fileds)){
            ui->tableView->setColumnWidth(i, static_cast<int>(medium_width_fileds)*sizes.at(i));
            //}
        }

        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->horizontalHeader()->setSectionsMovable(true);
        if(!connected_header_signal){
            connected_header_signal = true;
            connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
                    this, SLOT(on_sectionClicked(int)));
        }
    }
}

void Screen_Table_Equipo_Operarios::on_sectionClicked(int logicalIndex)
{
    QMap <QString,QString> mapa;
    mapa.insert("Codigo",codigo_equipo_operario_equipo_operarios);
    mapa.insert("Equipo de Fontaneros",equipo_operario_equipo_operarios);
    mapa.insert("Encargado",nombre_encargado_equipo_operarios);
    mapa.insert("Teléfono",telefono_equipo_operarios);

    QStringList listHeaders;
    listHeaders <<"Codigo" << "Equipo de Fontaneros" << "Encargado" << "Teléfono";

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);

    jsonArrayAllEquipo_Operarios = ordenarPor(jsonArrayAllEquipo_Operarios, ordenamiento, "");
}


void Screen_Table_Equipo_Operarios::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_EQUIPO_OPERARIOS)
    {
        qDebug()<<byte_array;
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_equipo_operarios"))
        {
            result = database_comunication::script_result::get_equipo_operarios_failed;
        }
        else
        {
            jsonArrayAllEquipo_Operarios = database_comunication::getJsonArray(byte_array);
            Equipo_Operario::writeEquipo_Operarios(jsonArrayAllEquipo_Operarios);
            emit equipo_operariosReceived(tipo);
            //        export_done_tasks_to_excel();
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Equipo_Operarios::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Equipo_Operarios::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa
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

void Screen_Table_Equipo_Operarios::on_tableView_doubleClicked(const QModelIndex &index)
{
    oneEquipo_OperarioScreen = new Equipo_Operario(nullptr, false, empresa);
    connect(oneEquipo_OperarioScreen, &Equipo_Operario::update_tableEquipo_Operarios,this,
            &Screen_Table_Equipo_Operarios::getEquipo_OperariosFromServer);
    connect(oneEquipo_OperarioScreen, &Equipo_Operario::updateTareas,this,
            &Screen_Table_Equipo_Operarios::on_updateTareas);
    QJsonObject o = jsonArrayAllEquipo_Operarios.at(index.row()).toObject();
    oneEquipo_OperarioScreen->setData(o);
    oneEquipo_OperarioScreen->show();
}
void Screen_Table_Equipo_Operarios::on_pb_nueva_clicked()
{

    oneEquipo_OperarioScreen = new Equipo_Operario(nullptr, true, empresa);
    connect(oneEquipo_OperarioScreen, &Equipo_Operario::update_tableEquipo_Operarios,this,
            &Screen_Table_Equipo_Operarios::getEquipo_OperariosFromServer);
    oneEquipo_OperarioScreen->show();
}
void Screen_Table_Equipo_Operarios::on_updateTareas()
{
    emit updateTareas();
}
