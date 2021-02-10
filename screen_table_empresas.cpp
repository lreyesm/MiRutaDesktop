#include "screen_table_empresas.h"
#include "ui_screen_table_empresas.h"
#include "screen_table_empresas.h"
#include "ui_screen_table_empresas.h"

Screen_Table_Empresas::Screen_Table_Empresas(QWidget *parent, bool show) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Empresas)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Empresas");
    getEmpresasFromServer(show);
}

Screen_Table_Empresas::~Screen_Table_Empresas()
{
    delete ui;
}
void Screen_Table_Empresas::getEmpresasFromServer(bool view)
{
    QStringList keys, values;
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    if(view){
        connect(this,SIGNAL(empresasReceived(database_comunication::serverRequestType)),
                this,SLOT(populateTable(database_comunication::serverRequestType)));
    }
    database_com.serverRequest(database_comunication::serverRequestType::GET_EMPRESAS,keys,values);

}

void Screen_Table_Empresas::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
    Q_UNUSED(tipo);
    jsonArrayAllEmpresas = ordenarPor(jsonArrayAllEmpresas, codigo_empresa_empresas, "");
}

void Screen_Table_Empresas::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Código",codigo_empresa_empresas);
    mapa.insert("Empresa",empresa_empresas);
    mapa.insert("Descripción",descripcion_empresas);

    QStringList listHeaders;
    listHeaders <<"Código" << "Empresa" << "Descripción";

    model = new QStandardItemModel(rows, listHeaders.size());
    model->setHorizontalHeaderLabels(listHeaders);

    //insertando los datos
    QStandardItem *item;
    Q_UNUSED(item);
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

void Screen_Table_Empresas::setTableView()
{
    if(model!=nullptr){
        ui->tableView->setModel(model);

        //        ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->resizeColumnsToContents();
//        float ancho = (float)(ui->tableView->width()-20)/4;
//        ui->tableView->setColumnWidth(0, (int)(ancho * (float)2/4));
//        ui->tableView->setColumnWidth(1, (int)(ancho * (float)2/4));
        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->horizontalHeader()->setSectionsMovable(true);
        if(!connected_header_signal){
            connected_header_signal = true;
            connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
                    this, SLOT(on_sectionClicked(int)));
        }
    }
}

void Screen_Table_Empresas::on_sectionClicked(int logicalIndex)
{
    QMap <QString,QString> mapa;
    mapa.insert("Código",codigo_empresa_empresas);
    mapa.insert("Empresa",empresa_empresas);
    mapa.insert("Descripción",descripcion_empresas);

    QStringList listHeaders;
    listHeaders <<"Código" << "Empresa" << "Descripción";

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);

    jsonArrayAllEmpresas = ordenarPor(jsonArrayAllEmpresas, ordenamiento, "");
}


void Screen_Table_Empresas::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_EMPRESAS)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_empresas"))
        {
            result = database_comunication::script_result::get_empresas_failed;
        }
        else
        {
            jsonArrayAllEmpresas = database_comunication::getJsonArray(byte_array);
            Empresa::writeEmpresas(jsonArrayAllEmpresas);
            serverAlredyAnswered = true;
            emit empresasReceived(tipo);
            //        export_done_tasks_to_excel();
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Empresas::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Empresas::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa
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

void Screen_Table_Empresas::on_tableView_doubleClicked(const QModelIndex &index)
{
    oneEmpresaScreen = new Empresa(nullptr, false);
    connect(oneEmpresaScreen, &Empresa::update_tableEmpresas,this,
            &Screen_Table_Empresas::getEmpresasFromServer);
    QJsonObject o = jsonArrayAllEmpresas.at(index.row()).toObject();
    oneEmpresaScreen->setData(o);
    oneEmpresaScreen->show();
}
void Screen_Table_Empresas::on_pb_nueva_clicked()
{
    oneEmpresaScreen = new Empresa(nullptr, true);
    connect(oneEmpresaScreen, &Empresa::update_tableEmpresas,this,
            &Screen_Table_Empresas::getEmpresasFromServer);
    oneEmpresaScreen->show();
}
