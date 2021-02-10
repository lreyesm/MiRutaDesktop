#include "screen_table_administradores.h"
#include "ui_screen_table_administradores.h"

Screen_Table_Administradores::Screen_Table_Administradores(QWidget *parent, bool show, QString empresa) :
    QMainWindow(parent),
    ui(new Ui::Screen_Table_Administradores)
{
    ui->setupUi(this);
    this->setWindowTitle("Tabla de Administradores");
    this->empresa = empresa;
    getAdministradoresFromServer(show);
}

Screen_Table_Administradores::~Screen_Table_Administradores()
{
    delete ui;
}
void Screen_Table_Administradores::setEmpresa(QString emp)
{
    this->empresa = emp;
}
void Screen_Table_Administradores::getAdministradoresFromServer(bool view)
{
    QStringList keys, values;
    keys << "empresa";
    values << empresa.toLower();
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    if(view){
        connect(this,SIGNAL(administradoresReceived(database_comunication::serverRequestType)),
                this,SLOT(populateTable(database_comunication::serverRequestType)));
    }
    database_com.serverRequest(database_comunication::serverRequestType::GET_ADMINISTRADORES,keys,values);

}

void Screen_Table_Administradores::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
    Q_UNUSED(tipo);
    jsonArrayAllAdministradores = ordenarPor(jsonArrayAllAdministradores, codigo_administrador_administradores, "");
}

void Screen_Table_Administradores::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Código",codigo_administrador_administradores);
    mapa.insert("Usuario",administrador_administradores);
    mapa.insert("Nombre",nombre_administradores);
    mapa.insert("Teléfono",telefono_administradores);

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
            //            item->setData(column_info,Qt::EditRole);
            QModelIndex index = model->index(i, n, QModelIndex());
            model->setData(index, column_info);
        }
    }
}

void Screen_Table_Administradores::setTableView()
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

void Screen_Table_Administradores::on_sectionClicked(int logicalIndex)
{
    QMap <QString,QString> mapa;
    mapa.insert("Código",codigo_administrador_administradores);
    mapa.insert("Usuario",administrador_administradores);
    mapa.insert("Nombre",nombre_administradores);
    mapa.insert("Teléfono",telefono_administradores);

    QStringList listHeaders;
    listHeaders <<"Código" << "Usuario" << "Nombre" << "Teléfono";

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);

    jsonArrayAllAdministradores = ordenarPor(jsonArrayAllAdministradores, ordenamiento, "");
}


void Screen_Table_Administradores::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_ADMINISTRADORES)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_administradores"))
        {
            result = database_comunication::script_result::get_administradores_failed;
        }
        else
        {
            jsonArrayAllAdministradores = database_comunication::getJsonArray(byte_array);
            Administrador::writeAdministradores(jsonArrayAllAdministradores);
            serverAlredyAnswered = true;
            emit administradoresReceived(tipo);
            //        export_done_tasks_to_excel();
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Table_Administradores::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Table_Administradores::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa
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

void Screen_Table_Administradores::on_tableView_doubleClicked(const QModelIndex &index)
{
    oneAdministradorScreen = new Administrador(nullptr, false, empresa);
    connect(oneAdministradorScreen, &Administrador::update_tableAdministradores,this,
            &Screen_Table_Administradores::getAdministradoresFromServer);
    QJsonObject o = jsonArrayAllAdministradores.at(index.row()).toObject();
    oneAdministradorScreen->setData(o);
    oneAdministradorScreen->show();
}
void Screen_Table_Administradores::on_pb_nueva_clicked()
{
    oneAdministradorScreen = new Administrador(nullptr, true, empresa);
    connect(oneAdministradorScreen, &Administrador::update_tableAdministradores,this,
            &Screen_Table_Administradores::getAdministradoresFromServer);
    oneAdministradorScreen->show();
}
