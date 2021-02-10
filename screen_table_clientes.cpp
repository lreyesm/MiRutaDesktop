#include "screen_table_clientes.h"
#include "ui_screen_table_clientes.h"

#include "screen_table_clientes.h"
#include "ui_screen_table_clientes.h"

Screen_Table_Clientes::Screen_Table_Clientes(QWidget *parent, bool show, QString empresa) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Clientes)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Clientes");
    this->empresa = empresa;
    getClientesFromServer(show);
}

Screen_Table_Clientes::~Screen_Table_Clientes()
{
    delete ui;
}
void Screen_Table_Clientes::setEmpresa(QString emp)
{
    this->empresa = emp;
}
void Screen_Table_Clientes::getClientesFromServer(bool view)
{
    QStringList keys, values;
    keys << "empresa";
    values << empresa.toLower();
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    if(view){
        connect(this,SIGNAL(clientesReceived(database_comunication::serverRequestType)),
                this,SLOT(populateTable(database_comunication::serverRequestType)));
    }
    database_com.serverRequest(database_comunication::serverRequestType::GET_CLIENTES,keys,values);

}

void Screen_Table_Clientes::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
    Q_UNUSED(tipo);
    jsonArrayAllClientes = ordenarPor(jsonArrayAllClientes, codigo_cliente_clientes, "");
}

void Screen_Table_Clientes::closeEvent(QCloseEvent *event)
{
    emit closing();
    QWidget::closeEvent(event);
}

void Screen_Table_Clientes::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Código",codigo_cliente_clientes);
    mapa.insert("Usuario",cliente_clientes);
    mapa.insert("Nombre",nombre_clientes);
    mapa.insert("Teléfono",telefono_clientes);

    QStringList listHeaders;
    listHeaders <<"Código" << "Usuario" << "Nombre" << "Teléfono";

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
            QModelIndex index = model->index(i, n, QModelIndex());
            model->setData(index, column_info);
        }
    }
}

void Screen_Table_Clientes::setTableView()
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

void Screen_Table_Clientes::on_sectionClicked(int logicalIndex)
{
    QMap <QString,QString> mapa;
    mapa.insert("Código",codigo_cliente_clientes);
    mapa.insert("Usuario",cliente_clientes);
    mapa.insert("Nombre",nombre_clientes);
    mapa.insert("Teléfono",telefono_clientes);

    QStringList listHeaders;
    listHeaders <<"Código" << "Usuario" << "Nombre" << "Teléfono";

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);

    jsonArrayAllClientes = ordenarPor(jsonArrayAllClientes, ordenamiento, "");
}


void Screen_Table_Clientes::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_CLIENTES)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_clientes"))
        {
            result = database_comunication::script_result::get_clientes_failed;
        }
        else
        {
            jsonArrayAllClientes = database_comunication::getJsonArray(byte_array);
            Cliente::writeClientes(jsonArrayAllClientes);
            serverAlredyAnswered = true;
            emit clientesReceived(tipo);
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Clientes::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Clientes::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa
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

void Screen_Table_Clientes::on_tableView_doubleClicked(const QModelIndex &index)
{
    oneClienteScreen = new Cliente(nullptr, false, empresa);
    connect(oneClienteScreen, &Cliente::update_tableClientes,this,
            &Screen_Table_Clientes::getClientesFromServer);
    connect(oneClienteScreen, &Cliente::updateTareas,this,
            &Screen_Table_Clientes::on_updateTareas);
    QJsonObject o = jsonArrayAllClientes.at(index.row()).toObject();
    oneClienteScreen->setData(o);
    oneClienteScreen->show();
}
void Screen_Table_Clientes::on_pb_nueva_clicked()
{
    oneClienteScreen = new Cliente(nullptr, true, empresa);
    connect(oneClienteScreen, &Cliente::update_tableClientes,this,
            &Screen_Table_Clientes::getClientesFromServer);
    oneClienteScreen->show();
}
void Screen_Table_Clientes::on_updateTareas()
{
    emit updateTareas();
}

