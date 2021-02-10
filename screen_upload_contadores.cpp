#include "screen_upload_contadores.h"
#include "ui_screen_upload_contadores.h"
#include <QMap>
#include <QFileDialog>
#include <QFileDialog>
#include <QtXlsx>
#include <QtXlsx/xlsxformat.h>
#include <QMessageBox>
#include "operator_selection_screen.h"
#include "equipo_selection_screen.h"
#include "operario.h"
#include "global_variables.h"
#include "rightclickmenu.h"
#include "fields_to_assing_counters.h"
#include "rightclickedsection.h"
#include "mycheckbox.h"
#include "mylineeditshine.h"
#include <QScrollArea>
#include <QCheckBox>
#include "QProgressIndicator.h"
#include "clase.h"
#include "marca.h"
#include "mylabelshine.h"
#include <QScreen>
#include "globalfunctions.h"
#include <QGraphicsDropShadowEffect>

Screen_Upload_Contadores::Screen_Upload_Contadores(QWidget *parent, QString empresa) :
    QMainWindow(parent),
    ui(new Ui::Screen_Upload_Contadores)
{
    setWindowFlags(Qt::CustomizeWindowHint);

    this->empresa = empresa;
    ui->setupUi(this);
    model = new QStandardItemModel(this);

    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));

    mapa_contador.insert("número serie de contador",numero_serie_contadores); //Cuidado al aumentar la cantidad de keys, el tamaño del excel esta asiciado a este en funcion parse_to_QjsonArray
    mapa_contador.insert("calibre",calibre_contadores);
    mapa_contador.insert("longitud",longitud_contadores);
    mapa_contador.insert("ruedas",ruedas_contador_contadores);
    mapa_contador.insert("marca",marca_contadores);
    mapa_contador.insert("modelo",modelo_contadores);
    mapa_contador.insert("clase",clase_contadores);
    mapa_contador.insert("tipo de fluido",tipo_fluido_contadores);
    mapa_contador.insert("tipo de radio",tipo_radio_contadores);
    mapa_contador.insert("indice",lectura_inicial_contadores);
    mapa_contador.insert("lectura inicial",lectura_inicial_contadores);
    mapa_contador.insert("lectura",lectura_inicial_contadores);
    mapa_contador.insert("encargado",encargado_contadores);
    mapa_contador.insert("operario",encargado_contadores);
    mapa_contador.insert("equipo",equipo_encargado_contadores);

    QGraphicsDropShadowEffect* eff= new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    eff->setBlurRadius(20);
    eff->setOffset(2);
    ui->pb_upload_contadores_from_excel->setGraphicsEffect(eff);

    on_pb_load_contadores_from_excel_clicked();
}

Screen_Upload_Contadores::~Screen_Upload_Contadores()
{
    delete ui;
}


void Screen_Upload_Contadores::on_drag_screen(){

    start_moving_screen.stop();
    if(isFullScreen() || isMaximized()){
        return;
    }
    //ui->statusBar->showMessage("Moviendo");
    if(QApplication::mouseButtons()==Qt::LeftButton){

        start_moving_screen.start(10);
        init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
        init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
    }
    else if(QApplication::mouseButtons()==Qt::RightButton){
        on_drag_screen_released();
    }
}

void Screen_Upload_Contadores::on_start_moving_screen_timeout(){

    int x_pos = (int)this->pos().x()+((QWidget::mapFromGlobal(QCursor::pos())).x() - init_pos_x);
    int y_pos = (int)this->pos().y()+((QWidget::mapFromGlobal(QCursor::pos())).y() - init_pos_y);
    x_pos = (x_pos < 0)?0:x_pos;
    y_pos = (y_pos < 0)?0:y_pos;

    x_pos = (x_pos > QApplication::desktop()->width()-100)?QApplication::desktop()->width()-100:x_pos;
    y_pos = (y_pos > QApplication::desktop()->height()-180)?QApplication::desktop()->height()-180:y_pos;

    this->move(x_pos,y_pos);

    init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
    init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
}

void Screen_Upload_Contadores::on_drag_screen_released()
{
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
}

void Screen_Upload_Contadores::on_pb_create_new_contador_clicked()
{

}

void Screen_Upload_Contadores::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo){
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;
    if(tipo == database_comunication::GET_CONTADORES)
    {
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_administradores"))
        {
            result = database_comunication::script_result::get_contadores_failed;
        }
        else
        {
            filtering = false;
            jsonArrayInTable = database_comunication::getJsonArray(byte_array);
            Counter::writeCounters(jsonArrayInTable);
            serverAlredyAnswered = true;
            emit contadoresReceived(tipo);
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::GET_ALL_COLUMN_VALUES_CUSTOM_QUERY){
        jsonObjectValues = database_comunication::getJsonObject(byte_array);
        result = database_comunication::script_result::ok;
    }
    else if(tipo == database_comunication::GET_ALL_COLUMN_VALUES)
    {
        jsonObjectValues = database_comunication::getJsonObject(byte_array);
        result = database_comunication::script_result::ok;
    }
    else if(tipo == database_comunication::GET_CONTADORES_AMOUNT)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(byte_array.contains("ot success get_contadores_amount_custom_query"))
        {
            qDebug()<<byte_array;
            result = database_comunication::script_result::get_contadores_amount_failed;
        }
        else {
            jsonInfoContadoresAmount = database_comunication::getJsonObject(byte_array);
            qDebug()<<"query return -> "<<jsonInfoContadoresAmount.value("query").toString();
            QString count_contadores = jsonInfoContadoresAmount.value("count_contadores").toString();
            countContadores = count_contadores.toInt();
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::GET_CONTADORES_CUSTOM_QUERY)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(byte_array.contains("ot success get_contadores_with_limit_custom_query"))
        {
            qDebug()<<byte_array;
            result = database_comunication::script_result::get_contadores_custom_query_failed;
        }
        else {
            jsonArrayAll = database_comunication::getJsonArray(byte_array);
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::UPDATE_CONTADORES_FIELDS)
    {
        qDebug()<<byte_array;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(byte_array.contains("ot success update_contadores_fields"))
        {
            result = database_comunication::script_result::update_contadores_fields_to_server_failed;
        }
        else
        {
            if(byte_array.contains("success ok update_contadores_fields"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    else if(tipo == database_comunication::DELETE_CONTADORES){
        qDebug()<<byte_array;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(byte_array.contains("ot success delete_contadores"))
        {
            result = database_comunication::script_result::delete_contadores_fields_to_server_failed;
        }
        else
        {
            if(byte_array.contains("success ok delete_contadores"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    emit script_excecution_result(result);
}
void Screen_Upload_Contadores::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
    Q_UNUSED(tipo);
    jsonArrayInTable = ordenarPor(jsonArrayInTable, numero_serie_contadores, "");
}

QJsonArray Screen_Upload_Contadores::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Upload_Contadores::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa
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
QMap <QString,QString> Screen_Upload_Contadores::fillMapForFixModel(QStringList &listHeaders){
    QMap <QString,QString> mapa;
    mapa.insert("Numero serie", numero_serie_contadores);
    mapa.insert("Año o prefijo", anno_o_prefijo_contadores);
    mapa.insert("Calibre",calibre_contadores);
    mapa.insert("Longitud",longitud_contadores);
    mapa.insert("Ruedas",ruedas_contador_contadores);
    mapa.insert("C.Marca",codigo_marca_contadores);
    mapa.insert("Marca",marca_contadores);
    mapa.insert("Modelo",modelo_contadores);
    mapa.insert("C.Clase",codigo_clase_contadores);
    mapa.insert("Clase",clase_contadores);
    mapa.insert("T.Fluido",tipo_fluido_contadores);
    mapa.insert("T.Radio",tipo_radio_contadores);
    mapa.insert("Lectura",lectura_inicial_contadores);
    mapa.insert("Encargado",encargado_contadores);
    mapa.insert("Estado",status_contadores);

    listHeaders << "Numero serie" << "Año o prefijo" << "Calibre"
                << "Longitud" << "Ruedas" << "C.Marca"<< "Marca"
                << "Modelo" << "C.Clase"<< "Clase"  << "T.Fluido"
                << "T.Radio" << "Lectura" << "Encargado" << "Estado";

    return mapa;
}
void Screen_Upload_Contadores::fixModelForTable(QJsonArray jsonArray){

    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    model->clear();

    QStringList listHeaders;
    QMap <QString,QString> mapa = fillMapForFixModel(listHeaders);

    model = new QStandardItemModel(rows, listHeaders.size());
    model->setHorizontalHeaderLabels(listHeaders);

    //insertando los datos0
    QStandardItem *item;
    for(int i = 0; i < rows; i++)
    {
        for (int n = 0; n < listHeaders.size(); n++) {
            item = new QStandardItem();
            QString key = mapa.value(listHeaders.at(n));
            QString value = jsonArray.at(i).toObject().value(key).toString();
            //            if(key.contains(numero_serie_contadores)){ //Para quitar el prefijo del numero de serie
            //                QString prefijo = jsonArray.at(i).toObject().value(anno_o_prefijo_contadores).toString();
            //                if(value != prefijo){
            //                    if(!prefijo.isEmpty() &&
            //                        (value.mid(0, prefijo.size()) == prefijo)){
            //                         value = value.remove(0, prefijo.size()).trimmed();
            //                    }
            //                }
            //            }
            if(!checkIfFieldIsValid(value)){
                value ="";
            }
            item->setData(value ,Qt::EditRole);
            model->setItem(i, n, item);
        }
    }
}

void Screen_Upload_Contadores::setTableView()
{
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //    QItemSelectionModel * selectionModel = new QItemSelectionModel(QItemSelectionModel::Rows);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    int fields_count_in_table = ui->tableView->horizontalHeader()->count();
    int width_table = ui->tableView->size().width() - 120;
    float medium_width_fileds = (float)width_table/fields_count_in_table;

    for (int i=0; i< fields_count_in_table; i++) {
        ui->tableView->setColumnWidth(i, (int)(medium_width_fileds));
    }
    ui->tableView->setColumnWidth(0, medium_width_fileds*2);

    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->horizontalHeader()->setFont(ui->tableView->font());

    if(!connected_header_signal){
        connected_header_signal = true;
        connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),this, SLOT(on_sectionClicked(int)));
    }
}

void Screen_Upload_Contadores::on_sectionClicked(int logicalIndex)
{
    emit sectionPressed();
    filterColumnList.clear();
    lastCursorPos = this->mapFromGlobal(QCursor::pos());

    disconnect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));

    QStringList listHeaders;
    QMap <QString,QString> mapa = fillMapForFixModel(listHeaders);

    QString columna = listHeaders.at(logicalIndex);
    QString ordenamiento = mapa.value(columna);
    lastSectionCliked = columna;
    lastSectionField = ordenamiento;

    RightClickedSection *rightClickedSection = new RightClickedSection(0, QCursor::pos());
    connect(rightClickedSection, SIGNAL(clickPressed(int)), this, SLOT(getMenuSectionClickedItem(int)));
    connect(this, &Screen_Upload_Contadores::closing, rightClickedSection, &RightClickMenu::deleteLater);
    connect(this, &Screen_Upload_Contadores::tablePressed, rightClickedSection, &RightClickMenu::deleteLater);
    connect(this, &Screen_Upload_Contadores::mouse_pressed, rightClickedSection, &RightClickMenu::deleteLater);

    rightClickedSection->show();
}
QJsonArray Screen_Upload_Contadores::getCurrentJsonArrayInTable(){
    QJsonArray jsonArray;

    if(filtering){
        jsonArray = jsonArrayInTableFiltered;
    }else{
        jsonArray = jsonArrayInTable;
    }
    return jsonArray;
}
void Screen_Upload_Contadores::filtrarEnTabla(bool checked){
    solo_tabla_actual = checked;
    emit sectionPressed();

    showFilterWidgetOptions(false);
}
QStringList Screen_Upload_Contadores::getFieldValues(QString field){

    QStringList values;
    QJsonArray jsonArray;

    jsonArray = getCurrentJsonArrayInTable();
    QString value;
    for (int i=0; i < jsonArray.size(); i++) {
        value = jsonArray.at(i).toObject().value(field).toString();
        if(checkIfFieldIsValid(value)){
            if(!values.contains(value)){
                values << value;
            }
        }else if(field == status_contadores){
            if(!values.contains("DISPONIBLE")){
                values << "DISPONIBLE";
            }
        }
    }
    values.sort();
    return values;
}
void Screen_Upload_Contadores::filterColumnField(){

    if(filterColumnList.isEmpty()){
        return;
    }
    QJsonArray jsonArray;

    jsonArray = getCurrentJsonArrayInTable();
    jsonArrayInTableFiltered = QJsonArray();

    for(int i=0; i< jsonArray.size(); i++){
        QString value = jsonArray[i].toObject().value(lastSectionField).toString().trimmed();
        if(lastSectionField == status_contadores){
            if(!checkIfFieldIsValid(value) && filterColumnList.contains("DISPONIBLE")){
                jsonArrayInTableFiltered.append(jsonArray[i].toObject());
                continue;
            }
        }
        if(checkIfFieldIsValid(value) && filterColumnList.contains(value, Qt::CaseInsensitive)){
            jsonArrayInTableFiltered.append(jsonArray[i].toObject());
        }
    }

    filtering = true;
    filterColumnList.clear();
    fixModelForTable(jsonArrayInTableFiltered);
    setTableView();

    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
}
QString Screen_Upload_Contadores::getScrollBarStyle(){
    QString style =
            "QScrollBar:vertical{"
            "border: 2px #777777;"
            "background-color: solid #777777;"
            "border-radius: 5px;"
            "width: 10px;"
            "margin: 3px 0px 3px 0px;"
            "}"

            "QScrollBar::handle:vertical{"
            "background-color: #777777;"
            "border-radius: 5px;"
            "min-height: 20px;"
            "}"

            "QScrollBar::add-line:vertical{"
            "border: 2px solid white;"
            "background: solid white;"
            "border-radius: 5px;"
            "height 10px;"
            "subcontrol-position: bottom;"
            "subcontrol-origin: margin;"
            "}"

            "QScrollBar::sub-line:vertical{"
            "border: 2px solid white;"
            "background: solid white;"
            "border-radius: 5px;"
            "height 10px;"
            "subcontrol-position: top;"
            "subcontrol-origin: margin;"
            "}";

    return style;
}

void Screen_Upload_Contadores::showFilterWidgetOptions(bool offset){

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(5);
    vlayout->setSpacing(0);
    vlayout->setObjectName("v_layout");
    vlayout->setAlignment(Qt::AlignCenter);

    QWidget *widgetValues = new QWidget;
    widgetValues->setStyleSheet("background-color: rgb(77, 77, 77);"
                                "border-radius: 5px;");
    widgetValues->setLayout(vlayout);

    MyLineEditShine *lineEdit = new MyLineEditShine();
    QCheckBox *cb_todos = new QCheckBox();
    cb_todos->setStyleSheet("color: rgb(255, 255, 255);"
                            "background-color: rgba(77, 77, 77);");
    QFont f =  ui->tableView->font();
    f.setPointSize(9);
    cb_todos->setText("Todos");
    cb_todos->setFont(f);

    int itemHeight = 35;
    QStringList values = getFieldValues(lastSectionField);
    QString value;
    int width = 100;
    foreach(value, values){
        MyCheckBox *cb = new MyCheckBox();
        cb->setText(value);
        cb->setObjectName("cb_"+value);
        cb->setFixedHeight(itemHeight-5);
        cb->setStyleSheet("color: rgb(255, 255, 255);"
                          "background-color: rgba(77, 77, 77);");
        connect(cb, &MyCheckBox::toggleCheckBox, this, &Screen_Upload_Contadores::addRemoveFilterList);
        connect(lineEdit, &MyLineEditShine::textChanged, cb, &MyCheckBox::onTextSelectedChanged);
        connect(cb_todos, &QCheckBox::toggled, cb, &MyCheckBox::set_Checked);

        QFont font =  ui->tableView->font();
        font.setPointSize(9);

        cb->setFont(font);

        widgetValues->layout()->addWidget(cb);
        if((value.size() + 1) * 10  > width){
            if((value.size() + 1) * 10 < 500){
                width = (value.size() + 1) * 10;
            }
        }
    }
    QPushButton *button_filter = new QPushButton("FILTRAR  ");
    button_filter->setStyleSheet("color: rgb(255, 255, 255);"
                                 "background-color: rgba(77, 77, 77);");
    button_filter->setIcon(QIcon(":/icons/filter.png"));
    button_filter->setIconSize(QSize(20, 20));
    connect(button_filter, &QPushButton::clicked, this, &Screen_Upload_Contadores::filterColumnField);

    QFont font =  ui->tableView->font();
    font.setBold(true);
    font.setPointSize(10);

    button_filter->setFont(font);

    //    widgetValues->setFixedHeight(35 * (values.size() + 1));
    widgetValues->setFixedWidth(width);

    QScrollArea *scroll = new QScrollArea;
    QWidget *widget = new QWidget(this);
    QGraphicsDropShadowEffect* ef = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    ef->setBlurRadius(20);
    ef->setOffset(10);
    widget->setGraphicsEffect(ef);
    widget->setFixedSize((widgetValues->width() > 500)? 580 : widgetValues->width() + 80,
                         (values.size() > 10)? 515: (values.size() * itemHeight + 170));

    lineEdit->setStyleSheet("color: rgb(77, 77, 77);"
                            "background-color: rgb(255, 255, 255);");
    lineEdit->setFixedHeight(20);
    lineEdit->setFixedWidth(widgetValues->width());

    QCompleter *completer = new QCompleter(values, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    lineEdit->setCompleter(completer);

    widget->setStyleSheet("background-color: rgba(77, 77, 77);"
                          "border-radius: 5px;");
    QVBoxLayout *layout = new QVBoxLayout(widget);

    scroll->setWidget(widgetValues);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet(getScrollBarStyle());

    layout->setMargin(15);
    layout->setSpacing(15);

    layout->addWidget(lineEdit);
    layout->addWidget(cb_todos);
    layout->addWidget(scroll);
    layout->addWidget(button_filter);

    layout->setAlignment(Qt::AlignCenter);

    if(offset){
        QRect rect = QGuiApplication::screens().first()->geometry();
        if(lastCursorPos.x() > rect.width()/2){
            lastCursorPos.setX(lastCursorPos.x()-widget->width());
        }
    }
    widget->move(lastCursorPos);
    widget->show();

    connect(button_filter, &QPushButton::clicked, widget, &QWidget::deleteLater);
    connect(this, &Screen_Upload_Contadores::sectionPressed, widget, &QWidget::deleteLater);
    connect(this, &Screen_Upload_Contadores::closing, widget, &QWidget::deleteLater);
    connect(this, &Screen_Upload_Contadores::tablePressed, widget, &QWidget::deleteLater);
}

void Screen_Upload_Contadores::addRemoveFilterList(QString value){
    if(filterColumnList.contains(value)){
        filterColumnList.removeOne(value);
    }else{
        filterColumnList << value;
    }
}

void Screen_Upload_Contadores::getMenuSectionClickedItem(int selection){
    if(selection == RightClickedSection::FILTRAR){
        showFilterWidgetOptions();
    }
    else if(selection == RightClickedSection::ORDENAR_ASCENDENTE){
        if(lastSectionField != lectura_inicial_contadores){
            if(filtering){
                jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionField, "");
            }else{
                jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionField, "");
            }
        }else{
            if(filtering){
                jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionField, 0);
            }else{
                jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionField, 0);
            }
        }

    }
    else if(selection == RightClickedSection::ORDENAR_DESCENDENTE){
        if((lastSectionField != lectura_inicial_contadores)){
            if(filtering){
                jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionField, "MAYOR_MENOR");
            }else{
                jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionField, "MAYOR_MENOR");
            }
        }else{
            if(filtering){
                jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionField, 1);
            }else{
                jsonArrayInTable = ordenarPor(jsonArrayInTable, lastSectionField, 1);
            }
        }

    }
}


bool Screen_Upload_Contadores::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

void Screen_Upload_Contadores::on_pb_cruz_clicked()
{
    model->clear();
    emit closing();
    this->close();
}

void Screen_Upload_Contadores::show_loading(QString mess){
    emit hidingLoading();

    QWidget *widget_blur = new QWidget(this);
    widget_blur->setFixedSize(this->size()+QSize(0,30));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &Screen_Upload_Contadores::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &Screen_Upload_Contadores::hidingLoading, widget_blur, &QWidget::deleteLater);

    MyLabelShine *label_loading_text = new MyLabelShine(widget_blur);
    label_loading_text->setStyleSheet("background-color: rgb(255, 255, 255);"
                                      "color: rgb(54, 141, 206);"
                                      "border-radius: 10px;"
                                      "font: italic 14pt \"Segoe UI Semilight\";");

    QRect rect = widget_blur->geometry();
    label_loading_text->setText(mess);
    label_loading_text->setFixedSize(400, 150);
    label_loading_text->move(rect.width()/2
                             - label_loading_text->size().width()/2,
                             rect.height()/2
                             -  label_loading_text->size().height()/2);
    label_loading_text->setMargin(20);
    label_loading_text->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    label_loading_text->show();
    connect(this, &Screen_Upload_Contadores::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &Screen_Upload_Contadores::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &Screen_Upload_Contadores::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &Screen_Upload_Contadores::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &Screen_Upload_Contadores::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}

void Screen_Upload_Contadores::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void Screen_Upload_Contadores::hide_loading(){
    emit hidingLoading();
}

void Screen_Upload_Contadores::on_pb_load_contadores_from_excel_clicked()
{
    QString rutaToDATFile = QFileDialog::getOpenFileName(this,"Seleccione el archivo .XLS", QDir::current().path(), "Datos (*.xlsx *.xls)");
    if(!rutaToDATFile.isNull() && !rutaToDATFile.isEmpty()){
        jsonArrayInTableExcel = parse_to_QjsonArray(rutaToDATFile);
        jsonArrayInTableExcel = ordenarPor(jsonArrayInTableExcel, numero_serie_contadores, "");
    }
    else{
        QTimer::singleShot(100, this, &Screen_Upload_Contadores::on_pb_cruz_clicked);
    }
}

QJsonArray Screen_Upload_Contadores::parse_to_QjsonArray(QString rutaToDATFile){
    QXlsx::Document xlsx(rutaToDATFile);
    QJsonArray jsonArray, jsonArrayToReturn;
    QStringList listHeaders, row_content;
    QList<QStringList> lista;
    //        QFile file(path);
    //        file.open(QIODevice::ReadOnly);
    int number_of_row = xlsx.dimension().lastRow();
    int number_of_column = xlsx.dimension().lastColumn();

    //read headers
    ///Limitar el numero de headers 26 es la cantidad de columnas actual del excel
    if(number_of_column > mapa_contador.size()-2){
        number_of_column = mapa_contador.size()-2;
    }
    for (int i=1; i<=number_of_column; i++)
    {
        listHeaders << xlsx.cellAt(1,i)->value().toString().trimmed().toLower();
    }

    //read data
    for(int i = 3; i <= number_of_row; i++)
    {
        row_content.clear();
        for(int j = 1; j <= number_of_column; j++)
        {
            if(xlsx.cellAt(i,j) != 0)
            {
                row_content << xlsx.cellAt(i,j)->value().toString().trimmed();
            }
            else
            {
                row_content << "";

            }
        }
        lista.insert(i-3, row_content);
    }

    //OJO debo hacer una comprobación de los encabezados
    //-2 por los encabezados y fila de filtros y -1 por la fila de resumen al final
    for(int i=0; i < number_of_row-2; i++)
    {
        QJsonObject o;
        row_content = lista.at(i);
        if(row_content.at(0).trimmed().isEmpty() && row_content.at(1).trimmed().isEmpty()){
            continue;
        }
        for(int j = 0; j < number_of_column; j++)
        {
            QString campo_en_excel = listHeaders.at(j);
            QString campo_en_json = mapa_contador.value(listHeaders.at(j));
            QString valor = row_content.at(j);
            o.insert(campo_en_json, valor);
        }
        o.remove("");

        QMap<QString, QString> clases;
        jsonArray = Clase::readClases();
        for (int i=0; i < jsonArray.size(); i++) {
            QString clase_v, cod_v;
            clase_v = jsonArray.at(i).toObject().value(clase_clases).toString();
            cod_v = jsonArray.at(i).toObject().value(codigo_clase_clases).toString();
            clases.insert(cod_v, clase_v);
        }

        QMap<QString, QString> marcas;
        jsonArray = Marca::readMarcas();
        for (int i=0; i < jsonArray.size(); i++) {
            QString marca_v, modelo_v, cod_v;
            marca_v = jsonArray.at(i).toObject().value(marca_marcas).toString();
            modelo_v = jsonArray.at(i).toObject().value(modelo_marcas).toString();
            cod_v = jsonArray.at(i).toObject().value(codigo_marca_marcas).toString();

            marcas.insert(cod_v, marca_v + " - " + modelo_v);
        }


        for (int i=0; i < marcas.size(); i++) {
            QString marcas_x = marcas.values().at(i).toLower();
            QString marca_o = o.value(marca_contadores).toString().toLower();
            QString modelo_o = o.value(modelo_contadores).toString().toLower();
            if(marcas_x.contains(marca_o)
                    && marcas_x.contains(modelo_o)){
                QString key = marcas.keys().at(i);
                o.insert(codigo_marca_contadores, key);
                break;
            }
        }
        QString clase_o = o.value(clase_contadores).toString().toLower().replace(" ", "");
        for (int i=0; i < clases.size(); i++) {
            QString clases_i = clases.values().at(i).toLower().replace(" ", "");
            if(clases_i.contains(clase_o)){
                QString key = clases.keys().at(i);
                o.insert(codigo_clase_contadores,key);
                break;
            }
        }
        QString prefijo = o.value(numero_serie_contadores).toString();
        while(prefijo.contains("  ")){
            prefijo = prefijo.replace("  ", " ");
        }
        if(prefijo.split(" ").size() >= 2){
            prefijo = prefijo.split(" ").at(0);
        }
        o.insert(anno_o_prefijo_contadores, prefijo);
        jsonArrayToReturn.append(o);
    }

    return jsonArrayToReturn;
}

void Screen_Upload_Contadores::on_pb_upload_contadores_from_excel_clicked()
{
    show_loading("Espere, subiendo al servidor...");

    QStringList alredyOnDatabase;

    ui->pb_upload_contadores_from_excel->hide();
    int total = jsonArrayInTableExcel.size();

    Counter *contador = new Counter(nullptr, empresa);

    for(int i=0, reintentos = 0;i<jsonArrayInTableExcel.size();i++)
    {
        show_loading("Espere, subiendo contadores... ("
                     +QString::number(i)+"/"+QString::number(total)+")");
        QJsonObject o = jsonArrayInTableExcel[i].toObject();

        //        screen_get_one_tarea *one_tarea_screen = new screen_get_one_tarea();
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        o.insert(date_time_modified_contadores, timestamp);

        QStringList keys, values;
        QJsonDocument d;
        d.setObject(o);
        QByteArray ba = d.toJson(QJsonDocument::Compact);
        keys << "json" << "empresa";
        QString temp = QString::fromUtf8(ba);
        values << temp << empresa.toLower();

        contador->populateFixView(o);

        QEventLoop *q = new QEventLoop();

        connect(contador, &Counter::script_excecution_result,q,&QEventLoop::exit);

        QString serie = o.value(numero_serie_contadores).toString().trimmed();
        GlobalFunctions gf(this, empresa);
        if(gf.checkIfCounterExist(numero_serie_contadores, serie)){
            contador->update_contador_request(keys, values);
            alredyOnDatabase << serie;
        }
        else{
            contador->create_contador_request(keys, values);
        }

        switch(q->exec())
        {
        case database_comunication::script_result::timeout:
            i--;
            reintentos++;
            if(reintentos == RETRIES)
            {
                GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                i = jsonArrayInTableExcel.size();
            }
            break;

        case database_comunication::script_result::contador_to_server_ok:
            reintentos = 0;
            break;

        case database_comunication::script_result::update_contador_to_server_failed:
            i--;
            reintentos++;
            if(reintentos == RETRIES)
            {
                GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                i = jsonArrayInTableExcel.size();
            }
            break;

        case database_comunication::script_result::create_contador_to_server_failed:
            i--;
            reintentos++;
            if(reintentos == RETRIES)
            {
                GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                i = jsonArrayInTableExcel.size();
            }
            break;
        }

        delete q;

    }
    hide_loading();

    if(!alredyOnDatabase.isEmpty())
    {
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Números internos replicados","Los siguientes contadores se actualizaron porque ya se encontraban en el servidor.\n" + alredyOnDatabase.join(" , "));
    }
    GlobalFunctions::showMessage(this,"Éxito","Información actualizada correctamente");
    QTimer::singleShot(100, this, &Screen_Upload_Contadores::on_pb_cruz_clicked);
    emit updateContadores();
}


void Screen_Upload_Contadores::on_pb_asignar_operario_clicked()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos un contador");
        return;
    }
    Operator_Selection_Screen *seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);
    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));

    QJsonArray jsonArray = getCurrentJsonArrayInTable();

    int total = selection.count();
    if(seleccionOperarioScreen->exec())
    {

        Counter *contador = new Counter(nullptr, empresa);

        for(int i=0, reintentos = 0; i< selection.count(); i++)
        {
            show_loading("Espere, actualizando contadores... ("
                         +QString::number(i)+"/"+QString::number(total)+")");

            QModelIndex index = selection.at(i);
            int posicion = index.row();

            if(jsonArray.size() > posicion){

                QJsonObject o = jsonArray[posicion].toObject();

                //        screen_get_one_tarea *one_tarea_screen = new screen_get_one_tarea();
                QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
                o.insert(date_time_modified_contadores, timestamp);
                o.insert(encargado_contadores, operatorName);

                QStringList keys, values;
                QJsonDocument d;
                d.setObject(o);
                QByteArray ba = d.toJson(QJsonDocument::Compact);
                keys << "json" << "empresa";
                QString temp = QString::fromUtf8(ba);
                values << temp << empresa.toLower();

                contador->populateFixView(o);

                QEventLoop *q = new QEventLoop();

                connect(contador, &Counter::script_excecution_result,q,&QEventLoop::exit);

                contador->update_contador_request(keys, values);

                switch(q->exec())
                {
                case database_comunication::script_result::timeout:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = jsonArray.size();
                    }
                    break;

                case database_comunication::script_result::contador_to_server_ok:
                    reintentos = 0;
                    break;

                case database_comunication::script_result::update_contador_to_server_failed:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = jsonArray.size();
                    }
                    break;

                case database_comunication::script_result::create_contador_to_server_failed:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = jsonArray.size();
                    }
                    break;
                }

                delete q;
            }
        }
        hide_loading();
    }
}
void Screen_Upload_Contadores::delete_contadores_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_CONTADORES,keys,values);
}
void Screen_Upload_Contadores::update_contadores_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_CONTADORES_FIELDS,keys,values);
}

void Screen_Upload_Contadores::on_tableView_pressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    emit tablePressed();
    //    if(QApplication::mouseButtons()==Qt::RightButton){
    //        RightClickMenu *rightClickMenu = new RightClickMenu(0, QCursor::pos(), RightClickMenu::FROM_CONTADORES);
    //        connect(rightClickMenu, &RightClickMenu::clickPressed, this, &Screen_Upload_Contadores::getMenuClickedItem);
    //        connect(this, &Screen_Upload_Contadores::closing, rightClickMenu, &RightClickMenu::deleteLater);
    //        connect(this, &Screen_Upload_Contadores::tablePressed, rightClickMenu, &RightClickMenu::deleteLater);
    //        connect(this, &Screen_Upload_Contadores::mouse_pressed, rightClickMenu, &RightClickMenu::deleteLater);
    //        rightClickMenu->show();
    //    }
}
void Screen_Upload_Contadores::getMenuClickedItem(int selected)
{
    if(this->isHidden()){
        return;
    }
    else if(selected == ELIMINAR){
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
        QString question = "";
        if(selection.isEmpty()){
            GlobalFunctions::showMessage(this,"Sin Selección","Seleccione al menos un Contador para eliminar.");
            return;
        }else if(selection.size()== 1){
            question = "el contador seleccionado?";
        }else{
            question = "los contadores seleccionados?";
        }
        GlobalFunctions gf(this);
    if(gf.showQuestion(this, "Confirmación", "¿Seguro desea"
                                 " eliminar "+question,
                                 QMessageBox::Ok, QMessageBox::No)
                == QMessageBox::Ok){
            on_pb_eliminar_clicked();
        }
    }
    else if(selected == ABRIR){
        QJsonArray jsonArray = getCurrentJsonArrayInTable();
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
        if(!selection.isEmpty() && selection.size()==1){
            QModelIndex index = selection.at(0);
            int posicion = index.row();
            QJsonObject o = jsonArray[posicion].toObject();
//            openContadorX(o);
        }else{
            if(selection.size()>1){
                QMessageBox::information(this,"Seleccione solo un Contador","No puede abrir con mas de un Contador seleccionado");
            }else if(selection.isEmpty()){
                QMessageBox::information(this,"Seleccione una Contador","Seleccione al menos un Contador para abrir.");
            }
        }
    }
}

void Screen_Upload_Contadores::on_pb_eliminar_clicked(){
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;
    QStringList numeros_serie;
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();

        if(jsonArray.size() > posicion){
            o = jsonArray[posicion].toObject();
            numeros_serie << o.value(numero_serie_contadores).toString();
        }
    }

    if(deleteContadores(numeros_serie)){
        QMessageBox::information(this,"Eliminados","Los contadores eliminados en el servidor satisfactoriamente.");
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","Los contadores no fueron eliminados en el servidor.");
    }
}
bool Screen_Upload_Contadores::deleteContadores(QStringList lista_numeros_serie){

    QJsonObject numeros_serie;

    for (int i=0; i < lista_numeros_serie.size(); i++) {
        numeros_serie.insert(QString::number(i), lista_numeros_serie.at(i));
    }
    if(numeros_serie.isEmpty()){
        return true;
    }
    QStringList keys, values;
    QJsonDocument d;
    d.setObject(numeros_serie);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp_numeros_serie = QString::fromUtf8(ba);

    keys << "json_numeros_serie" << "empresa";
    values << temp_numeros_serie << empresa.toLower();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_Upload_Contadores::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_Upload_Contadores::delete_contadores_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::update_contadores_fields_to_server_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}

void Screen_Upload_Contadores::get_equipo_selected(QString u)
{
    equipoName = u;
}

void Screen_Upload_Contadores::get_user_selected(QString u)
{
    operatorName = u;
}

void Screen_Upload_Contadores::on_pb_maximize_clicked()
{
    if(isMaximized()){
        this->showNormal();
    }else {
        this->showMaximized();
    }
    QTimer::singleShot(200, this, SLOT(setTableView()));
}

void Screen_Upload_Contadores::on_pb_minimize_clicked()
{
    this->showMinimized();
}



