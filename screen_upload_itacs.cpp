#include "screen_upload_itacs.h"
#include "ui_screen_upload_itacs.h"
#include <QFile>
#include <QJsonArray>
#include "global_variables.h"
#include "rightclickmenu.h"
#include <QMessageBox>
#include <QScreen>
#include "operator_selection_screen.h"
#include "equipo_selection_screen.h"
#include "rightclickedsection.h"
#include "mycheckbox.h"
#include "mylineeditshine.h"
#include "fields_to_assing_itacs.h"
#include <QScrollArea>
#include <QCheckBox>
#include <QCompleter>
#include "mapas_cercania_itacs.h"
#include "globalfunctions.h"
#include <QGraphicsDropShadowEffect>

Screen_Upload_Itacs::Screen_Upload_Itacs(QWidget *parent, QString empresa) :
    QMainWindow(parent),
    ui(new Ui::Screen_Upload_Itacs)
{
    this->empresa = empresa;
    ui->setupUi(this);
}

Screen_Upload_Itacs::~Screen_Upload_Itacs()
{
    delete ui;
}

void Screen_Upload_Itacs::on_pb_subir_clicked()
{
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;
    QStringList codigos_itacs;
    bool uploadOk = true;

    oneITACScreen = new ITAC(nullptr, false, empresa);
    int total = jsonArray.size();
    for (int i=0; i < total; i++) {
        ui->statusbar->showMessage("Subiendo (" + QString::number(i+1) + "/" + QString::number(total) + ")...");
        o = jsonArray[i].toObject();
        oneITACScreen->setData(o, false);
        if(!oneITACScreen->uploadItac(o.value(codigo_itac_itacs).toString())){
            uploadOk = false;
        }
    }
    oneITACScreen->deleteLater();

    if(uploadOk){
        GlobalFunctions::showMessage(this,"Éxito","Itacs subidas correctamente");
        this->close();
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","Itacs no fueron subidas al servidor correctamente.");
    }
}

QJsonArray Screen_Upload_Itacs::getJsonArrayInFile(QString filename)
{
    QFile input(filename);
    QByteArray data_json;
    QJsonArray jsonArray;
    if(input.open(QIODevice::ReadOnly | QIODevice::Text)){
        data_json = input.readAll();
        jsonArray = database_comunication::getJsonArray(data_json);
        input.close();
    }
    return jsonArray;
}

void Screen_Upload_Itacs::closeEvent(QCloseEvent *event)
{
    emit closing();
    QWidget::closeEvent(event);
}
void Screen_Upload_Itacs::resizeEvent(QResizeEvent *event){

    QWidget::resizeEvent(event);
    if(timer.isActive()){
        timer.stop();
    }
    timer.setInterval(200);
    timer.start();
    connect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
}
void Screen_Upload_Itacs::getITACsFromFile(QString filename)
{
    jsonArrayAllITACs = getJsonArrayInFile(filename);
    filtering = false;

    populateTable(database_comunication::NONE_REQUEST);
}

void Screen_Upload_Itacs::update_itacs_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_ITAC_FIELDS,keys,values);
}
bool Screen_Upload_Itacs::updateITACs(QStringList lista_cod_emplazamientos, QJsonObject campos){

    QJsonObject cod_emplazamientos;

    for (int i=0; i < lista_cod_emplazamientos.size(); i++) {
        cod_emplazamientos.insert(QString::number(i), lista_cod_emplazamientos.at(i));
    }
    if(cod_emplazamientos.isEmpty()){
        return true;
    }
    campos.insert(date_time_modified_itacs, QDateTime::currentDateTime().toString(formato_fecha_hora));

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(cod_emplazamientos);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp_fields, temp_numins = QString::fromUtf8(ba);

    d.setObject(campos);
    ba = d.toJson(QJsonDocument::Compact);
    temp_fields = QString::fromUtf8(ba);

    keys << "json_cod_emplazamientos" << "json_fields" << "empresa";
    values << temp_numins << temp_fields << empresa.toLower();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Screen_Upload_Itacs::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Screen_Upload_Itacs::update_itacs_fields_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::update_itacs_fields_to_server_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}

void Screen_Upload_Itacs::populateTable(/*QByteArray ba, */database_comunication::serverRequestType tipo)
{
    Q_UNUSED(tipo);
    jsonArrayAllITACs = ordenarPor(jsonArrayAllITACs, codigo_itac_itacs, "");
}

void Screen_Upload_Itacs::setGestor(QString g)
{
    this->gestor = g;
}
QMap <QString,QStringList> Screen_Upload_Itacs::fillMapForFixModel(QStringList &listHeaders){
    QMap <QString,QStringList> mapa;
    QStringList values;
    values << zona_itacs;
    mapa.insert("SECTOR P", values);
    values.clear();
    values << itac_itacs;
    mapa.insert("DIRECCIÓN", values);
    values.clear();
    values << codigo_itac_itacs;
    mapa.insert("C.EMPLAZAMIENTO", values);
    values.clear();
    values << puntuacion_itacs;
    mapa.insert("PUNTUACIÓN DE ITAC", values);
    values.clear();
    values << nombre_empresa_administracion_itacs << nombre_responsable_administracion_itacs
           << telefono_fijo_administracion_itacs << telefono_movil_administracion_itacs << direccion_oficina_administracion_itacs;
    mapa.insert("ADMINISTRACIÓN", values);//+telefono
    values.clear();
    values << nombre_presidente_itacs << vivienda_presidente_itacs << telefono_movil_presidente_itacs << telefono_fijo_presidente_itacs;
    mapa.insert("PRESIDENTE O\n COLABORADOR", values);//+vivienda y telefono
    values.clear();
    values << nombre_encargado_itacs << vivienda_encargado_itacs << telefono_fijo_encargado_itacs << telefono_movil_encargado_itacs;
    mapa.insert("ENCARGADO O\n CONSERJE", values);//+vivienda y telefono
    values.clear();
    values << acceso_itacs << descripcion_itacs;
    mapa.insert("NOTA GLOBAL", values); // + Descripcion DE INSTALACION

    values.clear();
    values << puntos_agua_total_itacs;
    mapa.insert("N PUNTOS DE AGUA\n TOTAL",values);
    values.clear();
    values << puntos_agua_con_contador_itacs;
    mapa.insert("N PUNTOS DE AGUA\n CON CONTADOR",values);
    values.clear();
    values << puntos_agua_con_contador_mas_tarea_itacs;
    mapa.insert("N PUNTOS DE AGUA CON\nCONTADOR MÁS TAREA",values);

    //SECCION 1
    values.clear();
    values << extra_acceso_ubicacion_itacs;
    mapa.insert("ACCESO",values);

    //SECCION 2
    values.clear();
    values << tipo_llave_itacs << extras_llaves_itacs;
    mapa.insert("LLAVES DE ACCESO", values); // + extras_llaves_itacs

    //SECCION 3
    values.clear();
    values << espacio_para_trabajar_itacs;
    mapa.insert("ESPACIO PARA\n TRABAJAR",values);
    values.clear();
    values << desague_itacs << extras_desague_itacs;
    mapa.insert("DESAGÜE",values); // + extras_desague_itacs
    values.clear();
    values << iluminacion_itacs << extras_iluminacion_itacs;
    mapa.insert("ILUMINACIÓN",values);

    //SECCION 4
    values.clear();
    values << tubo_de_alimentacion_itacs;
    mapa.insert("TUBO DE\nALIMENTACIÓN",values);
    values.clear();
    values << colector_itacs;
    mapa.insert("COLECTOR",values);
    values.clear();
    values << tuberias_de_entrada_contador_itacs;
    mapa.insert("TUBERÍAS DE\nENTRADA",values);
    values.clear();
    values << tuberias_de_salida_contador_itacs;
    mapa.insert("TUBERÍAS DE\nSALIDA",values);

    //SECCION 4
    values.clear();
    values << valvula_general_itacs << extras_valvula_general_itacs;
    mapa.insert("VÁLVULA GENERAL",values); // + extras_valvula_general_itacs
    values.clear();
    values << valvula_entrada_itacs << extras_valvula_entrada_itacs;
    mapa.insert("VÁLVULA DE ENTRADA",values); // +extras_valvula_entrada_itacs
    values.clear();
    values << valvula_salida_itacs << extras_valvula_salida_itacs;
    mapa.insert("VÁLVULA DE SALIDA",values); // +extras_valvula_salida_itacs
    values.clear();
    values << valvula_antiretorno_itacs << extras_valvula_antiretorno_itacs;
    mapa.insert("VÁLVULA DE\nANTIRETORNO",values); // +extras_valvula_antiretorno_itacs

    values.clear();
    values << gestor_itacs;
    mapa.insert("GESTOR",values);

    listHeaders  << "SECTOR P" << "DIRECCIÓN" <<"C.EMPLAZAMIENTO" << "PUNTUACIÓN DE ITAC"
                 << "ADMINISTRACIÓN" << "PRESIDENTE O\n COLABORADOR" <<  "ENCARGADO O\n CONSERJE" << "NOTA GLOBAL"
                 << "N PUNTOS DE AGUA\n TOTAL" <<   "N PUNTOS DE AGUA\n CON CONTADOR" <<  "N PUNTOS DE AGUA CON\nCONTADOR MÁS TAREA"
                 << "ACCESO"  <<"LLAVES DE ACCESO"
                 << "ESPACIO PARA\n TRABAJAR"  <<  "DESAGÜE"  <<  "ILUMINACIÓN"
                 << "TUBO DE\nALIMENTACIÓN"  <<  "COLECTOR"  <<  "TUBERÍAS DE\nENTRADA"  << "TUBERÍAS DE\nSALIDA"
                 << "VÁLVULA GENERAL"  <<  "VÁLVULA DE ENTRADA"  <<  "VÁLVULA DE SALIDA"  <<"VÁLVULA DE\nANTIRETORNO"
                 << "GESTOR";

    return mapa;
}
void Screen_Upload_Itacs::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=nullptr){
        delete model;
    }

    QStringList listHeaders;
    QMap <QString,QStringList> mapa = fillMapForFixModel(listHeaders);

    model = new QStandardItemModel(rows, listHeaders.size());
    model->setHorizontalHeaderLabels(listHeaders);

    //insertando los datos
    QString column_info;
    for(int i = 0; i < rows; i++)
    {
        for (int n = 0; n < listHeaders.size(); n++) {

            QJsonObject jsonObject = jsonArray[i].toObject();
            QStringList header_values = mapa.value(listHeaders.at(n));
            QString header_value;
            column_info = "";
            foreach(header_value, header_values){
                QString value = jsonObject.value(header_value).toString().trimmed();
                if(checkIfFieldIsValid(value)){
                    column_info += value + ", ";
                }
            }
            if(!column_info.trimmed().isEmpty()){
                column_info = column_info.trimmed();
                column_info.remove(column_info.size()-1, 1);
            }
            QModelIndex index = model->index(i, n, QModelIndex());
            model->setData(index, column_info);
        }
    }
}

void Screen_Upload_Itacs::setTableView()
{
    timer.stop();
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
    if(model!=nullptr){
        ui->tableView->setModel(model);

        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

        QList<double> sizes;
        sizes  << 1.2/*Sector P*/<< 1.5 /*ID ITAC (dir)*/<< 0.75/*Código*/ <<  0.8/*Empresa*/ <<  0.75/*GESTOR*/;

        int fields_count_in_table = ui->tableView->horizontalHeader()->count();
        ui->tableView->horizontalHeader()->setFixedHeight(50);
        QFont font = ui->tableView->font();
//        int pointSize = font.pointSize();
//        float ratio = static_cast<float>(pointSize)/fields_count_in_table;
        QRect rect = QGuiApplication::screens().first()->geometry();
        int width = ui->tableView->size().width();
        if(width >= rect.width()){
            width = rect.width();
        }
        int width_table = width - 15;
        float medium_width_fileds = static_cast<float>(width_table)/10/*fields_count_in_table*/;

        for (int i=0; i < fields_count_in_table; i++) {
            ui->tableView->setColumnWidth(i, static_cast<int>(medium_width_fileds)/**sizes.at(i)*ratio*/);
        }

        if(!connected_header_signal){
            connected_header_signal = true;
            connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
                    this, SLOT(on_sectionClicked(int)));
        }
    }
}

void Screen_Upload_Itacs::on_sectionClicked(int logicalIndex)
{
    emit sectionPressed();
    filterColumnList.clear();
    lastCursorPos = this->mapFromGlobal(QCursor::pos());

    disconnect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));

    QStringList listHeaders;
    QMap <QString,QStringList> mapa = fillMapForFixModel(listHeaders);

    QString columna = listHeaders.at(logicalIndex);
    QStringList ordenamiento = mapa.value(columna);
    lastSectionCliked = columna;
    lastSectionFields = ordenamiento;

    RightClickedSection *rightClickedSection = new RightClickedSection(0, QCursor::pos());
    connect(rightClickedSection, SIGNAL(clickPressed(int)), this, SLOT(getMenuSectionClickedItem(int)));
    connect(this, &Screen_Upload_Itacs::closing, rightClickedSection, &RightClickMenu::deleteLater);
    connect(this, &Screen_Upload_Itacs::tablePressed, rightClickedSection, &RightClickMenu::deleteLater);
    connect(this, &Screen_Upload_Itacs::mouse_pressed, rightClickedSection, &RightClickMenu::deleteLater);

    rightClickedSection->show();
}

QJsonArray Screen_Upload_Itacs::getCurrentJsonArrayInTable(){
    QJsonArray jsonArray;
    if(filtering){
        jsonArray = jsonArrayInTableFiltered;
    }else{
        jsonArray = jsonArrayAllITACs;
    }
    return jsonArray;
}
QStringList Screen_Upload_Itacs::getFieldValues(QStringList fields){
    QJsonArray jsonArray;

    if(solo_tabla_actual){
        jsonArray = getCurrentJsonArrayInTable();
    }else{
        jsonArray = jsonArrayAllITACs;
    }

    QStringList values;
    QString value;
    for (int i=0; i < jsonArray.size(); i++) {

        QJsonObject jsonObject = jsonArray[i].toObject();
        QStringList header_values = fields;
        QString header_value;
        value = "";
        foreach(header_value, header_values){
            QString data = jsonObject.value(header_value).toString().trimmed();
            if(checkIfFieldIsValid(data)){
                value += data + ", ";
            }
        }
        if(!value.trimmed().isEmpty()){
            value = value.trimmed();
            value.remove(value.size()-1, 1);
        }
        if(checkIfFieldIsValid(value)){
            if(!values.contains(value)){
                values << value;
            }
        }
    }
    values.sort();
    return values;
}
void Screen_Upload_Itacs::filtrarEnTabla(bool checked){
    solo_tabla_actual = checked;
    emit sectionPressed();

    showFilterWidgetOptions(false);
}
void Screen_Upload_Itacs::filterColumnField(){

    if(filterColumnList.isEmpty()){
        return;
    }
    QJsonArray jsonArray;

    if(solo_tabla_actual){
        jsonArray = getCurrentJsonArrayInTable();
    }else{
        jsonArray = jsonArrayAllITACs;
    }

    jsonArrayInTableFiltered = QJsonArray();

    for(int i=0; i< jsonArray.size(); i++){
        QJsonObject jsonObject = jsonArray[i].toObject();
        QStringList header_values = lastSectionFields;
        QString header_value;
        QString value = "";
        foreach(header_value, header_values){
            QString data = jsonObject.value(header_value).toString().trimmed();
            if(checkIfFieldIsValid(data)){
                value += data + ", ";
            }
        }
        if(!value.trimmed().isEmpty()){
            value = value.trimmed();
            value.remove(value.size()-1, 1);
        }
        if(checkIfFieldIsValid(value) && filterColumnList.contains(value, Qt::CaseInsensitive)){
            jsonArrayInTableFiltered.append(jsonArray[i].toObject());
        }
    }
    filtering = true;
    filterColumnList.clear();
    fixModelForTable(jsonArrayInTableFiltered);
    setTableView();
}
QString Screen_Upload_Itacs::getScrollBarStyle(){
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

void Screen_Upload_Itacs::showFilterWidgetOptions(bool offset){

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
    QFont f = ui->tableView->font();
    f.setPointSize(9);
    cb_todos->setText("Todos");
    cb_todos->setFont(f);

    QCheckBox *cb_tabla_actual = new QCheckBox();
    cb_tabla_actual->setStyleSheet("color: rgb(255, 255, 255);"
                                   "background-color: rgba(77, 77, 77);");
    cb_tabla_actual->setText("Tabla Actual");
    cb_tabla_actual->setFont(f);
    cb_tabla_actual->setChecked(true);
    connect(cb_tabla_actual, &QCheckBox::toggled, this, &Screen_Upload_Itacs::filtrarEnTabla);

    int itemHeight = 35;
    QStringList values = getFieldValues(lastSectionFields);
    QString value;
    int width = 100;
    foreach(value, values){
        MyCheckBox *cb = new MyCheckBox();
        cb->setText(value);
        cb->setObjectName("cb_"+value);
        cb->setFixedHeight(itemHeight-5);
        cb->setStyleSheet("color: rgb(255, 255, 255);"
                          "background-color: rgba(77, 77, 77);");
        connect(cb, &MyCheckBox::toggleCheckBox, this, &Screen_Upload_Itacs::addRemoveFilterList);
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
    connect(button_filter, &QPushButton::clicked, this, &Screen_Upload_Itacs::filterColumnField);

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
    layout->addWidget(cb_tabla_actual);
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
    connect(this, &Screen_Upload_Itacs::sectionPressed, widget, &QWidget::deleteLater);
    connect(this, &Screen_Upload_Itacs::closing, widget, &QWidget::deleteLater);
    connect(this, &Screen_Upload_Itacs::tablePressed, widget, &QWidget::deleteLater);
}

void Screen_Upload_Itacs::addRemoveFilterList(QString value){
    if(filterColumnList.contains(value)){
        filterColumnList.removeOne(value);
    }else{
        filterColumnList << value;
    }
}

void Screen_Upload_Itacs::getMenuSectionClickedItem(int selection){
    if(selection == RightClickedSection::FILTRAR){
        showFilterWidgetOptions();
    }
    else if(selection == RightClickedSection::ORDENAR_ASCENDENTE){
        if(filtering){
            jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionFields, "");
        }else{
            jsonArrayAllITACs = ordenarPor(jsonArrayAllITACs, lastSectionFields, "");
        }

    }
    else if(selection == RightClickedSection::ORDENAR_DESCENDENTE){
        if(filtering){
            jsonArrayInTableFiltered = ordenarPor(jsonArrayInTableFiltered, lastSectionFields, "MAYOR_MENOR");
        }else{
            jsonArrayAllITACs = ordenarPor(jsonArrayAllITACs, lastSectionFields, "MAYOR_MENOR");
        }
    }
}

bool Screen_Upload_Itacs::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

void Screen_Upload_Itacs::serverAnswer(QByteArray byte_array, database_comunication::serverRequestType tipo)
{
    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    int result = -1;

    if(tipo == database_comunication::GET_ITACS)
    {
        //        qDebug()<<"GET_ITACS"<<byte_array;
        byte_array.remove(0,2);
        byte_array.chop(2);

        if(byte_array.contains("not success get_itacs"))
        {
            result = database_comunication::script_result::get_itacs_failed;
        }
        else if(!byte_array.trimmed().isEmpty())
        {
            jsonArrayAllITACs = database_comunication::getJsonArray(byte_array);
            filtering = false;
            ITAC::writeITACs(jsonArrayAllITACs);
            serverAlredyAnswered = true;
            emit itacsReceived(tipo);
            //        export_done_tasks_to_excel();
            result = database_comunication::script_result::ok;
        }else{
            qDebug()<<"Tabla de itacas vacia";
        }
    }
    else if(tipo == database_comunication::UPDATE_ITAC_FIELDS)
    {
        qDebug()<<byte_array;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(byte_array.contains("ot success update_itac_fields"))
        {
            result = database_comunication::script_result::update_itacs_fields_to_server_failed;
        }
        else
        {
            if(byte_array.contains("success ok update_itac_fields"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    emit script_excecution_result(result);
}
QJsonArray Screen_Upload_Itacs::ordenarPor(QJsonArray jsonArray, QStringList fields, QString type){ //type  se usa
    QStringList array;
    QString temp;

    for (int j =0; j < jsonArray.size(); j++) {
        QJsonObject jsonObject = jsonArray[j].toObject();
        QStringList header_values = fields;
        QString header_value;
        QString value = "";
        foreach(header_value, header_values){
            QString data = jsonObject.value(header_value).toString().trimmed();
            if(checkIfFieldIsValid(data)){
                value += data + ", ";
            }
        }
        if(!value.trimmed().isEmpty()){
            value = value.trimmed();
            value.remove(value.size()-1, 1);
        }
        temp = value;
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
            QJsonObject jsonObject = jsonarraySaved[i].toObject();
            QStringList header_values = fields;
            QString header_value;
            QString value = "";
            foreach(header_value, header_values){
                QString data = jsonObject.value(header_value).toString().trimmed();
                if(checkIfFieldIsValid(data)){
                    value += data + ", ";
                }
            }
            if(!value.trimmed().isEmpty()){
                value = value.trimmed();
                value.remove(value.size()-1, 1);
            }
            if(array[j] ==  value){
                jsonArray.append(jsonarraySaved[i].toObject());
            }
        }
    }
    fixModelForTable(jsonArray);
    setTableView();
    return jsonArray;
}
QJsonArray Screen_Upload_Itacs::ordenarPor(QJsonArray jsonArray, QString field, QString type){ //type  se usa

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
QJsonArray Screen_Upload_Itacs::ordenarPor(QJsonArray jsonArray, QString field, int type){ //type  se usa

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

void Screen_Upload_Itacs::on_tableView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    //    abrirItacX(index.row());
}
void Screen_Upload_Itacs::on_pb_nueva_clicked()
{
    QJsonObject campos;
    campos.insert(gestor_itacs, gestor);
    oneITACScreen = new ITAC(nullptr, true, empresa, campos);
    connect(oneITACScreen, &ITAC::updateTableTareas, this, &Screen_Upload_Itacs::update_Table_Tareas);
    QRect rect = QGuiApplication::screens().first()->geometry();
    if(rect.width() <= 1366
            && rect.height() <= 768){
        oneITACScreen->showMaximized();
    }else {
        oneITACScreen->show();
    }
}
void Screen_Upload_Itacs::update_Table_Tareas(){
    emit updateTableTareas();
}
void Screen_Upload_Itacs::mousePressEvent(QMouseEvent *event)
{
    emit mouse_pressed();
    QWidget::mousePressEvent(event);
}
void Screen_Upload_Itacs::on_tableView_pressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    emit tablePressed();
    //    if(QApplication::mouseButtons()==Qt::RightButton){
    //        RightClickMenu *rightClickMenu = new RightClickMenu(0, QCursor::pos(), RightClickMenu::FROM_ITACS);
    //        connect(rightClickMenu, &RightClickMenu::clickPressed, this, &Screen_Upload_Itacs::getMenuClickedItem);
    //        connect(this, &Screen_Upload_Itacs::closing, rightClickMenu, &RightClickMenu::deleteLater);
    //        connect(this, &Screen_Upload_Itacs::tablePressed, rightClickMenu, &RightClickMenu::deleteLater);
    //        connect(this, &Screen_Upload_Itacs::mouse_pressed, rightClickMenu, &RightClickMenu::deleteLater);
    //        rightClickMenu->show();
    //    }
}
void Screen_Upload_Itacs::on_actionMostrarEnMapa(){
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una ITAC");
        return;
    }
    QJsonArray jsonArray, jsonArrayToShow;
    jsonArray = getCurrentJsonArrayInTable();

    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();

        jsonArrayToShow.append(jsonArray.at(posicion).toObject());
    }

    Mapas_Cercania_Itacs *mapa = new Mapas_Cercania_Itacs(nullptr, jsonArrayToShow, empresa);
    connect(mapa, &Mapas_Cercania_Itacs::openITAC, this, &Screen_Upload_Itacs::openItacX);
    connect(mapa, &Mapas_Cercania_Itacs::showJsonArrayInTable,
            this, &Screen_Upload_Itacs::setJsonArrayFilterbyPerimeter);
    mapa->show();
}
void Screen_Upload_Itacs::setJsonArrayFilterbyPerimeter(QJsonArray jsonArray){
    jsonArrayInTableFiltered = jsonArray;
    filtering = true;
    filterColumnList.clear();
    fixModelForTable(jsonArrayInTableFiltered);
    setTableView();
}

void Screen_Upload_Itacs::getMenuClickedItem(int selected)
{
    if(this->isHidden()){
        return;
    }
    if(selected == MOSTRAR_EN_MAPA){
        on_actionMostrarEnMapa();
    }
    else if(selected == DESCARGAR_FOTOS){
        on_actionDescargar_Fotos();
    }
    else if(selected == ASIGNAR_COMUNES){
        on_actionAsignar_campos_triggered();
    }
    else if(selected == ASIGNAR_A_EQUIPO){
        on_actionAsignar_a_un_equipo_triggered();
    }
    else if(selected == ASIGNAR_A_OPERARIO){
        on_actionAsignar_a_un_operario_triggered();
    }
    else if(selected == ELIMINAR){
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
        QString question = "";
        if(selection.isEmpty()){
            GlobalFunctions::showMessage(this,"Sin Selección","Seleccione al menos un ITAC");
            return;
        }else if(selection.size()== 1){
            question = "el ITAC seleccionado?";
        }else{
            question = "los ITACs seleccionados?";
        }
        GlobalFunctions gf(this);
    if(gf.showQuestion(this, "Confirmación", "¿Seguro desea"
                                 " eliminar " + question,
                                 QMessageBox::Ok, QMessageBox::No)
                == QMessageBox::Ok){
            on_pb_eliminar_clicked();
        }
    }
    else if(selected == ABRIR){
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
        if(!selection.isEmpty() && selection.size()==1){
            QModelIndex index = selection.at(0);
            int posicion = index.row();
            abrirItacX(posicion);
        }else{
            if(selection.size()>1){
                QMessageBox::information(this,"Seleccione solo una Itac","No puede abrir con mas de una Itac seleccionada");
            }else if(selection.isEmpty()){
                QMessageBox::information(this,"Seleccione una Itac","Seleccione al menos una Itac para abrir.");
            }
        }
    }
}
void Screen_Upload_Itacs::openItacX(QString cod_emplazamiento){
    GlobalFunctions gf(this, empresa);
    QJsonObject jsonObject = gf.getItacFromServer(cod_emplazamiento);
    if (!jsonObject.isEmpty()) {
        QJsonObject itac = jsonObject;

        ITAC *oneITACScreen = new ITAC(nullptr, false, empresa);

        connect(oneITACScreen, &ITAC::updateTableTareas, this,
                &Screen_Upload_Itacs::update_Table_Tareas);
        QRect rect = QGuiApplication::screens().first()->geometry();
        if(rect.width() <= 1366
                && rect.height() <= 768){
            oneITACScreen->showMaximized();
        }else {
            oneITACScreen->show();
        }
        oneITACScreen->setData(itac);
        return;
    }
}
void Screen_Upload_Itacs::on_actionDescargar_Fotos(){
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;
    int total =  selection.count();

    ui->statusbar->show();

    for(int i=0; i< total; i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();

        if(jsonArray.size() > posicion){
            o = jsonArray[posicion].toObject();
            oneITACScreen = new ITAC(nullptr, false, empresa);
            oneITACScreen->setData(o, true);
            ui->statusbar->showMessage("Descargando fotos... "
                                       "("+QString::number(i+1)+"/" + QString::number(total)+")", 5000);
            oneITACScreen->deleteLater();

        }
    }
    QMessageBox::information(this,"Descargadas","Las fotos fueron descargadas del servidor correctamente.");

    ui->statusbar->hide();
}
void Screen_Upload_Itacs::on_pb_eliminar_clicked(){

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;
    QStringList codigos_itacs;
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();

        if(jsonArray.size() > posicion){
            o = jsonArray[posicion].toObject();
            QString cod_v = o.value(codigo_itac_itacs).toString().trimmed();
            codigos_itacs << cod_v;
        }
    }
    jsonArrayInTableFiltered = deleteItacsFromJsonArray(jsonArrayInTableFiltered, codigos_itacs);
    jsonArrayAllITACs = deleteItacsFromJsonArray(jsonArrayAllITACs, codigos_itacs);

    jsonArray = getCurrentJsonArrayInTable();
    fixModelForTable(jsonArray);
    setTableView();
}

QJsonArray Screen_Upload_Itacs::deleteItacsFromJsonArray(QJsonArray jsonArray, QStringList codigos_itacs){
    for (int i=0; i < jsonArray.size(); i++) {
        QString cod_v;
        cod_v = jsonArray.at(i).toObject().value(codigo_itac_itacs).toString().trimmed();
        if(codigos_itacs.contains(cod_v)){
            jsonArray.removeAt(i);
            i--;
        }
    }
    return jsonArray;
}

void Screen_Upload_Itacs::on_actionAsignar_campos_triggered(){
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos un contador para asignar a un equipo");
        return;
    }

    Fields_to_Assing_ITACs *fields_screen = new Fields_to_Assing_ITACs(nullptr);

    connect(fields_screen,&Fields_to_Assing_ITACs::fields_selected,
            this,&Screen_Upload_Itacs::updateSelectedFields);

    QRect rect = QGuiApplication::screens().first()->geometry();
    if(rect.width() <= 1366
            && rect.height() <= 768){
        fields_screen->showMaximized();
    }else {
        fields_screen->show();
    }
}
void Screen_Upload_Itacs::updateSelectedFields(QJsonObject campos){

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;
    QStringList codigos_itacs;
    for(int i=0; i< selection.count(); i++)
    {
        QModelIndex index = selection.at(i);
        int posicion = index.row();

        if(jsonArray.size() > posicion){
            o = jsonArray[posicion].toObject();
            codigos_itacs << o.value(codigo_itac_itacs).toString();
        }
    }

    if(updateITACs(codigos_itacs, campos)){
        GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor satisfactoriamente.");
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
    }
}
void Screen_Upload_Itacs::abrirItacX(int index){
    QJsonObject campos;
    campos.insert(gestor_itacs, gestor);
    oneITACScreen = new ITAC(nullptr, false, empresa, campos);
    QJsonObject o =  getCurrentJsonArrayInTable().at(index).toObject();
    connect(oneITACScreen, &ITAC::updateTableTareas, this, &Screen_Upload_Itacs::update_Table_Tareas);
    QRect rect = QGuiApplication::screens().first()->geometry();
    if(rect.width() <= 1366
            && rect.height() <= 768){
        oneITACScreen->showMaximized();
    }else {
        oneITACScreen->show();
    }
    oneITACScreen->setData(o);
}

void Screen_Upload_Itacs::on_actionAsignar_a_un_equipo_triggered(){

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una itac para asignar a un equipo");
        return;
    }

    Equipo_Selection_Screen *seleccionEquipoScreen = new Equipo_Selection_Screen(this, empresa);

    seleccionEquipoScreen->getEquipo_OperariosFromServer();
    connect(seleccionEquipoScreen,&Equipo_Selection_Screen::equipoSelected,
            this,&Screen_Upload_Itacs::get_equipo_selected);

    seleccionEquipoScreen->moveCenter();

    ui->statusbar->show();

    if(!selection.empty())
    {
        if(seleccionEquipoScreen->exec())
        {
            QJsonArray jsonArray = getCurrentJsonArrayInTable();
            QJsonObject o, campos;
            QStringList codigos_itacs;
            for(int i=0; i< selection.count(); i++)
            {
                QModelIndex index = selection.at(i);
                int posicion = index.row();

                if(jsonArray.size() > posicion){
                    o = jsonArray[posicion].toObject();
                    codigos_itacs << o.value(codigo_itac_itacs).toString();
                }
            }
            campos.insert(equipo_itacs,equipoName);

            if(updateITACs(codigos_itacs, campos)){
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
                ui->statusbar->showMessage("Asignado correctamente");
            }else{
                GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
                ui->statusbar->showMessage("Fallo Asignando");
            }
            disconnect(seleccionEquipoScreen,&Equipo_Selection_Screen::equipoSelected,
                       this,&Screen_Upload_Itacs::get_equipo_selected);
        }
    }
    ui->statusbar->hide();
}
void Screen_Upload_Itacs::get_equipo_selected(QString u)
{
    equipoName = u;
}

void Screen_Upload_Itacs::get_user_selected(QString u)
{
    operatorName = u;
}

void Screen_Upload_Itacs::on_actionAsignar_a_un_operario_triggered()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una itac para asignar a un operario");
        return;
    }

    Operator_Selection_Screen *seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);

    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,&Operator_Selection_Screen::user,
            this, &Screen_Upload_Itacs::get_user_selected);

    seleccionOperarioScreen->moveCenter();

    ui->statusbar->show();

    if(!selection.empty())
    {
        if(seleccionOperarioScreen->exec())
        {
            QJsonArray jsonArray = getCurrentJsonArrayInTable();
            QJsonObject o, campos;
            QStringList codigos_itacs;
            for(int i=0; i< selection.count(); i++)
            {
                QModelIndex index = selection.at(i);
                int posicion = index.row();

                if(jsonArray.size() > posicion){
                    o = jsonArray[posicion].toObject();
                    codigos_itacs << o.value(codigo_itac_itacs).toString();
                }
            }
            campos.insert(operario_itacs,operatorName);

            if(updateITACs(codigos_itacs, campos)){
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor satisfactoriamente.");
                ui->statusbar->showMessage("Asignado correctamente");
            }else{
                GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
                ui->statusbar->showMessage("Fallo Asignando");
            }
            disconnect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));
        }
    }
    ui->statusbar->hide();
}
