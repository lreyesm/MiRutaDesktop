#include "tabla.h"
#include "ui_tabla.h"
#include "new_table_structure.h"
#include "calendardialog.h"
#include <QException>
#include <QFileDialog>
#include "screen_tabla_tareas.h"
#include <QDebug>
#include "fields_to_assign.h"
#include <QDesktopWidget>
#include "global_variables.h"
#include "select_file_to_download.h"
#include <QHeaderView>
#include <QTableView>
#include "tutoriales.h"
#include "videoplayback.h"
#include <QMessageBox>
#include <JlCompress.h>
#include "seleccion_gestor.h"
#include "select_file_to_download.h"
#include <QLayout>
#include "info.h"
#include "gestor.h"
#include "screen_table_infos.h"
#include "facturacion.h"
#include "processesclass.h"
#include "causa.h"
#include "selectionorder.h"
#include "equipo_selection_screen.h"
#include "mapas_cercania.h"
#include "resumen_tareas.h"
#include "selection_priority.h"
#include <QGraphicsDropShadowEffect>
#include "rightclickmenu.h"
#include "rightclickedsection.h"
#include "mycheckbox.h"
#include "mylineeditshine.h"
#include "daterangeselection.h"
#include <QScrollArea>
#include <QCheckBox>
#include <dbtareascontroller.h>
#include "QProgressIndicator.h"
#include "screen_table_clientes.h"
#include "structure_itac.h"
#include "globalfunctions.h"
#include "databaseoptions.h"

using namespace QXlsx;

Tabla::Tabla(QWidget *parent, QString empresa) :
    QMainWindow(parent),
    ui(new Ui::Tabla)
{
    ui->setupUi(this);
    setWindowTitle("Mi Ruta "+versionMiRuta);
    setWindowFlags(Qt::CustomizeWindowHint);

    this->empresa = empresa;

    qDebug()<<"Cambio--------------------------------------";

    QGraphicsDropShadowEffect* ef = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    ef->setBlurRadius(20);
    ef->setOffset(1);
    ui->pb_gestor_inicial_seleccionado->setGraphicsEffect(ef);

    //    QGraphicsDropShadowEffect* eff = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    //    eff->setBlurRadius(20);
    //    eff->setOffset(20);
    //    ui->widget_menu->setGraphicsEffect(eff);

    ui->rb_todas->setChecked(true);
    fillMapaTiposTareas();
    hideAllFilters();

    ui->statusbar->showMessage("Cargando tareas del servidor...");
    hide_loading();

    //Nuevo -----------------------------------------------------------------------------------------
    ui->statusbar->hide();
    this->setMouseTracking(true);
    emit hideMenuFast("");

    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));

    connect(this,SIGNAL(hideMenuFast(QString)),this,SLOT(hideMenu(QString)));

    connect(ui->l_archivo,SIGNAL(mouseLeftClicked(QString)),this,SLOT(hideMenu(QString)));
    connect(ui->l_ordenar,SIGNAL(mouseLeftClicked(QString)),this,SLOT(hideMenu(QString)));
    connect(ui->l_tareas,SIGNAL(mouseLeftClicked(QString)),this,SLOT(hideMenu(QString)));
    connect(ui->l_ayuda,SIGNAL(mouseLeftClicked(QString)),this,SLOT(hideMenu(QString)));
    connect(ui->l_tablas,SIGNAL(mouseLeftClicked(QString)),this,SLOT(hideMenu(QString)));

    connect(ui->l_archivo,SIGNAL(actionPress(QString)),this,SLOT(onActionPress(QString)));
    connect(ui->l_ordenar,SIGNAL(actionPress(QString)),this,SLOT(onActionPress(QString)));
    connect(ui->l_tareas,SIGNAL(actionPress(QString)),this,SLOT(onActionPress(QString)));
    connect(ui->l_ayuda,SIGNAL(actionPress(QString)),this,SLOT(onActionPress(QString)));
    connect(ui->l_tablas,SIGNAL(actionPress(QString)),this,SLOT(onActionPress(QString)));

    connect(ui->pb_buscar,SIGNAL(actionPress(QString)),this,SLOT(onActionPress(QString)));

    connect(ui->tableView,SIGNAL(rightCliked(QMouseEvent*)),this,SLOT(rightClikedTable(QMouseEvent*)));
    connect(ui->tableView,SIGNAL(leftCliked(QMouseEvent*)),this,SLOT(leftClikedTable(QMouseEvent*)));

    uncheckAllRadioButtons(NINGUNO);

    QRect rect = QGuiApplication::screens().first()->geometry();
    int height = rect.height();
    int des = 160;
    if(height >= 1080){
        ui->l_archivo->move(ui->l_archivo->pos().x(), ui->l_archivo->pos().y()+des);
        ui->l_ordenar->move(ui->l_ordenar->pos().x(), ui->l_ordenar->pos().y()+des);
        ui->l_tareas->move(ui->l_tareas->pos().x(), ui->l_tareas->pos().y()+des);
        ui->l_ayuda->move(ui->l_ayuda->pos().x(), ui->l_ayuda->pos().y()+des);
        ui->l_tablas->move(ui->l_tablas->pos().x(), ui->l_tablas->pos().y()+des);
    }
    //End Nuevo -----------------------------------------------------------------------------------------
    campos_de_fechas << FechImportacion << fecha_instalacion << F_INST << fecha_hora_cita
                     << FECH_CIERRE << fech_informacionnew;

    connect(ui->l_current_pagination, &MyLabelSpinner::itemSelected,this, &Tabla::moveToPage);
    connect(this, &Tabla::hideMenuFast, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Tabla::sectionPressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Tabla::closing, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Tabla::tablePressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);
    connect(this, &Tabla::mouse_pressed, ui->l_current_pagination, &MyLabelSpinner::hideSpinnerList);

    connect(this, &Tabla::updateTableInfo,this, &Tabla::updateTareasInTable);

    QFile file(info_pagination); // ficheros .dat se puede utilizar formato txt tambien
    if(file.exists()){
        if(file.open(QIODevice::ReadOnly))
        {
            QDataStream in(&file);
            int cant = 0;
            in>>cant;
            if(cant > 0){
                limit_pagination = cant;
            }
            file.close();
        }
    }

    this->setAttribute(Qt::WA_DeleteOnClose);
}

void Tabla::showEvent( QShowEvent * event )
{
    QWidget::showEvent(event);
}
void Tabla::closeEvent(QCloseEvent *event)
{   
    emit closing();
    QWidget::closeEvent(event);
}
void Tabla::resizeEvent(QResizeEvent *event){

    QRect rect = QGuiApplication::screens().first()->geometry();
    QSize s = event->size();
    if(s.width() > rect.width() || s.height() > rect.height()){
        this->showNormal();
        this->setFixedSize(rect.width(), rect.height() - rect.height()/20);

        if(timer.isActive()){
            timer.stop();
        }
        timer.setInterval(200);
        timer.start();
        connect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
    }
    QWidget::resizeEvent(event);
}

Tabla::~Tabla()
{
    delete ui;
}
QJsonArray Tabla::loadExportedDatFile(QString filename){

    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QStringList unparsed_rows, empty;
    QJsonArray jsonArray;
    QJsonObject o;
    QString Tipo_Tarea, calibre, anomalia;

    char array [361];

    //reading rows
    while (file.read(array, 361))
    {
        unparsed_rows << QString::fromLatin1(array);//QString(array);
    }

    //parsing rows
    for(int i=0; i<unparsed_rows.count(); i++)
    {
        QJsonObject o;
        int pos=0;

        //"Prefijo devuelto"-------------------------------------------------------------------------------------
        o.insert(CONTADOR_Prefijo_anno_devuelto, QJsonValue(unparsed_rows[i].mid(pos,6).trimmed()));
        pos+=6;
        //numero_serie_contador_devuelto-------------------------------------------------------------------------------------
        o.insert(numero_serie_contador_devuelto, QJsonValue(unparsed_rows[i].mid(pos,7).trimmed()));
        pos+=7;
        //marca_contador-------------------------------------------------------------------------------------
        o.insert(marca_devuelta, QJsonValue(unparsed_rows[i].mid(pos,3).trimmed()));
        pos+=3;
        //aqui añadir otros datos de la marca
        //CALIBRE-------------------------------------------------------------------------------------
        o.insert(calibre_real, QJsonValue(unparsed_rows[i].mid(pos,3).trimmed()));
        pos+=3;
        //RUEDAS devueltas-------------------------------------------------------------------------------------
        o.insert(RUEDASDV, QJsonValue(unparsed_rows[i].mid(pos,1).trimmed()));
        pos+=1;
        //numero_abonado------------------------------------------------------------------------------------
        o.insert(numero_abonado, QJsonValue(unparsed_rows[i].mid(pos,8).trimmed()));
        pos+=8;
        //RESULTADO-------------------------------------------------------------------------------------
        o.insert(resultado, QJsonValue(unparsed_rows[i].mid(pos,3).trimmed()));
        pos+=3;
        //FECHA instalacion nuevo OJO lo asocié con 'fecha_de_cambio'--------------------------------------------------------------------------------------
        QString date = QJsonValue(unparsed_rows[i].mid(pos,10).trimmed()).toString();
        QDate f = QDate::fromString(date, "ddMMyyyy");
        o.insert(F_INST, f.toString("yyyy-MM-dd"));
        pos+=10;
        //LECTURA INSTALADO----------------------------------------------------------------------------------
        o.insert(lectura_contador_nuevo, QJsonValue(unparsed_rows[i].mid(pos,8).trimmed()));
        pos+=8;
        //EMPLAZAMIENTO------------------------------------------------------------------------------------
        o.insert(emplazamiento_devuelto, QJsonValue(unparsed_rows[i].mid(pos,2).trimmed()));
        pos+=2;
        //RESTO DEVUELTO --------------------------------------------------------------------------------
        o.insert(RESTO_EM, QJsonValue(unparsed_rows[i].mid(pos,8).trimmed()));
        pos+=8;
        //Nº INTERNO--------------------------------------------------------------------------------------
        o.insert(numero_interno, QJsonValue(unparsed_rows[i].mid(pos,12).trimmed()));
        pos+=12;
        //LECTURA LEVANTADO  OJO lo asocié con 'lectura_ultima'--------------------------------------------------------------------------------------
        o.insert(lectura_actual, QJsonValue(unparsed_rows[i].mid(pos,8).trimmed()));
        pos+=8;
        //OBSERVACIONES OJO solo tiene 3 caracteres y el .dat de entrada utiliza 30 caracteres para este campo--------------------------------------------------------------------------------------
        o.insert(observaciones_devueltas, QJsonValue(unparsed_rows[i].mid(pos,3).trimmed()));
        pos+=3;
        //CLASE DE CONTADOR--------------------------------------------------------------------------------------
        o.insert(TIPO_devuelto, QJsonValue(unparsed_rows[i].mid(pos,1).trimmed()));
        pos+=1;

        jsonArray.append(o);
    }
    file.close();
    return jsonArray;

}

void Tabla::hideAllFilters(){
    ui->widget_filtro_lineEdit->hide();
    ui->widget_filtro_direccion->hide();
    ui->widget_filtro_tipoTarea->hide();
    ui->widget_filtros->hide();
    ui->widget_filtro_gestores->hide();
    ui->pb_buscar_trabajo->show();
}

void Tabla::uncheckAllRadioButtons(){
    ui->rb_abierta->setChecked(false);
    ui->rb_ejecutada->setChecked(false);
    ui->rb_cerrada->setChecked(false);
    ui->rb_informada->setChecked(false);
    ui->rb_requerida->setChecked(false);
    ui->rb_todas->setChecked(false);
}

void Tabla::fillMapaTiposTareas(){
    
    mapaEstados.insert("Abierta", "IDLE");
    mapaEstados.insert("Abierta (Cita)", "IDLE CITA");
    mapaEstados.insert("Abierta (En Batería)", "IDLE TO_BAT");
    mapaEstados.insert("Ejecutada", "DONE");
    mapaEstados.insert("Cerrada", "CLOSED");
    mapaEstados.insert("Informada", "INFORMADA");
    mapaEstados.insert("Requerida", "REQUERIDA");

    map_days_week.insert("lu.","Lunes");
    map_days_week.insert("ma.","Martes");
    map_days_week.insert("mi.","Miércoles");
    map_days_week.insert("ju.","Jueves");
    map_days_week.insert("vi.","Viernes");
    map_days_week.insert("sa.","Sábado");
    map_days_week.insert("do.","Domingo");
    
    map_months.insert(1,"Enero");
    map_months.insert(2,"Febrero");
    map_months.insert(3,"Marzo");
    map_months.insert(4,"Abril");
    map_months.insert(5,"Mayo");
    map_months.insert(6,"Junio");
    map_months.insert(7,"Julio");
    map_months.insert(8,"Agosto");
    map_months.insert(9,"Septiembre");
    map_months.insert(10,"Octubre");
    map_months.insert(11,"Noviembre");
    map_months.insert(12,"Diciembre");
}

QJsonArray Tabla::fixJsonToLastModel(QJsonArray jsonArray){ //recibe el arreglo mostrado en la tabla entes de la descarga
    QStringList principal_var_all, principal_var_model;
    for (int i=0; i < jsonArray.size(); i++) { //obteniendo NUMIN de todos los json mostrados
        principal_var_model << jsonArray[i].toObject().value(principal_variable).toString();
    }
    for (int i=0; i < jsonArrayAll.size(); i++) { //obteniendo NUMIN de todos los json descargados
        principal_var_all << jsonArrayAll[i].toObject().value(principal_variable).toString();
    }
    for (int i=0; i < principal_var_model.size(); i++) { //si el jsonArray descargado no contiene algun json mostrado elimino el json mostrado
        if(!principal_var_all.contains(principal_var_model[i])){//para el caso que se haya eliminado
            jsonArray.removeAt(i);
            principal_var_model.removeAt(i);
            i--;
        }else{ //si el jsonArray descargado contiene el json mostrado actualizo la tabla (json mostrado) con jsonArray descargado
            int index = principal_var_all.indexOf(principal_var_model[i]);
            jsonArray[i] = jsonArrayAll[index];
        }
    }
    for (int i=0; i < principal_var_all.size(); i++) { //si se ha añadido tarea agregar del json descargado la tarea al json mostrado
        if(!principal_var_model.contains(principal_var_all[i])){//para el caso que se haya añadido tarea
            jsonArray.append(jsonArrayAll[i]);
        }
    }
    jsonArray = getCurrentRadioButton(jsonArray); //filtranado (en caso de ausentes o cita esto es necesario)
    if(filtering){
        QJsonArray jsonArray_model; //para mantener la tabla identica a la anterior
        for (int i=0; i < jsonArray.size(); i++) {//si el json mostrado contiene el json descargado lo muestro para mantener ultimo filtro en la tabla (mostrado)
            if(principal_var_model.contains(jsonArray[i].toObject().value(principal_variable).toString())){
                jsonArray_model.append(jsonArray[i].toObject());
            }
        }
        return jsonArray_model;
    }else {
        return jsonArray;
    }
}

bool Tabla::matchMultipleFields(QMap<QString, QString> fields, QJsonObject jsonObject){
    for (int i =0; i < fields.keys().size(); i++) {
        QString key = fields.keys().at(i);
        QString value = fields.value(key);
        QString jsonObjectFieldvalue = jsonObject.value(key).toString();
        if(value != jsonObjectFieldvalue){
            return false;
        }
    }
    return true;
}

QJsonArray Tabla::getCurrentRadioButton(QJsonArray jsonArray){
    if(ui->rb_todas->isChecked()){
        return jsonArray;
        
    }else if(ui->rb_abierta->isChecked()){
        return fillFilterOrdenABIERTAS(jsonArray);
        
    }else if(ui->rb_ausente->isChecked()){
        return fillFilterOrdenAUSENTES(jsonArray);
        
    }else if(ui->rb_citas->isChecked()){
        return fillFilterOrdenCITAS(jsonArray);
        
    }else if(ui->rb_ejecutada->isChecked()){
        return fillFilterOrdenEJECUTADAS(jsonArray);
        
    }else if(ui->rb_cerrada->isChecked()){
        return fillFilterOrdenCERRADAS(jsonArray);
        
    }else if(ui->rb_informada->isChecked()){
        return fillFilterOrdenINFORMADAS(jsonArray);
        
    }else if(ui->rb_requerida->isChecked()){
        return fillFilterOrdenREQUERIDAS(jsonArray);
    }
    return QJsonArray();
}

QJsonArray Tabla::filtroInicialDeGestores(QJsonArray jsonArray, bool abrir){
    QString gestor_seleccionado = "";
    if(abrir){
        gestor_seleccionado = abrirSeleccionGestores(jsonArray, true);
    }
    if(gestor_seleccionado.isEmpty()){
        if(GlobalFunctions::readGestorSelected().isEmpty()){
            gestor_seleccionado = "Todos";
        }else{
            gestor_seleccionado = GlobalFunctions::readGestorSelected();
        }
    }
    ui->pb_gestor_inicial_seleccionado->setText(gestor_seleccionado);
    if(gestor_seleccionado != "Todos"){
        for(int i =0; i < jsonArray.size(); i++){
            QString gestor_en_array = jsonArray.at(i).toObject().value(GESTOR).toString();
            if(!checkIfFieldIsValid(gestor_en_array)){
                gestor_en_array = "Sin_Gestor";
            }
            if(gestor_en_array != gestor_seleccionado){
                jsonArray.removeAt(i);
                i--;
            }
        }
    }
    return jsonArray;
}

void Tabla::setJsonArrayAll(QJsonArray jsonArray){

    bool mover_a_todas = true;

    if(other_task_screen::conexion_activa){
        //if(!jsonArray.isEmpty()){ //aqui modificar esto para el caso de que se actualice una tare y se debe volver al estado de filtro previo
        if(!jsonArrayAll.isEmpty()){
            jsonArrayAll = jsonArray;
            if(filtering){
                jsonArrayInTableFiltered = fixJsonToLastModel(jsonArrayInTableFiltered);
                fixModelforTable(jsonArrayInTableFiltered);
                setTableView();
            }else{
                jsonArrayInTable = fixJsonToLastModel(jsonArrayInTable);
                fixModelforTable(jsonArrayInTable);
                setTableView();
            }
        }else {//Aqui es cuando entra por primera vez a la tabla
            jsonArrayAll = jsonArray;
            mover_a_todas = true;
        }
        screen_tabla_tareas::lastSync = QDateTime::currentDateTime().toString(formato_fecha_hora);
        screen_tabla_tareas::writeVariablesInBD();

    }else{
        ui->pb_new_info->setEnabled(false);
        ui->pb_eliminar->setEnabled(false);
        if(!jsonArrayAll.isEmpty()){
            jsonArrayAll = other_task_screen::readJsonArrayTasks();
            if(filtering){
                jsonArrayInTableFiltered = fixJsonToLastModel(jsonArrayInTableFiltered);
                fixModelforTable(jsonArrayInTableFiltered);
                setTableView();
            }else{
                jsonArrayInTable = fixJsonToLastModel(jsonArrayInTable);
                fixModelforTable(jsonArrayInTable);
                setTableView();
            }
        }else {
            jsonArrayAll = other_task_screen::readJsonArrayTasks();
            filtering = false;
            jsonArrayInTable = jsonArrayAll;
            fixModelforTable(jsonArrayInTable);
            setTableView();
        }

    }

    if(lastSelectedRow != -1 && lastSelectedRow < jsonArrayInTable.size() && lastSelectedRow < model->rowCount()){
        QModelIndex index = model->index(lastSelectedRow, 0);
        ui->tableView->setCurrentIndex(index);
    }

    if(mover_a_todas){
        //        on_rb_todas_clicked();
    }
    ui->statusbar->showMessage("Tareas cargadas correctamente", 3000);
    habilitarBotonAtras(false);
}

QJsonArray Tabla::cargarJsonArrayDeTxt(QString ruta){
    if(ruta.isEmpty()){
        ruta = QFileDialog::getOpenFileName(this, "Seleccione fichero para cargar el trabajo", QDir::currentPath(), tr("Texto (*.txt)"));
    }
    QFile input(ruta);
    QByteArray data_json;
    QJsonArray jsonArray;
    if(input.open(QIODevice::ReadOnly | QIODevice::Text)){
        data_json = input.readAll();
        jsonArray = database_comunication::getJsonArray(data_json);
        input.close();
        if(jsonArray.isEmpty()){
            GlobalFunctions gf(this);
            GlobalFunctions::showWarning(this,"Error cargando archivo","No se pudo cargar el trabajo, el archivo esta vacio.");
        }
        return jsonArray;
    }
    GlobalFunctions gf(this);
    GlobalFunctions::showWarning(this,"Error cargando archivo","No se pudo cargar el trabajo, el archivo esta corrupto.");
    return QJsonArray();
}

QJsonObject Tabla::get_JObject_from_JArray(QJsonArray jsonArray, QString principal_var){
    for (int i=0; i < jsonArray.size(); i++) {
        if(jsonArray.at(i).toObject().value(principal_variable).toString().trimmed()==principal_var){
            return jsonArray.at(i).toObject();
        }
    }
    return QJsonObject();
}
bool Tabla::checkIfNewInfoSync()
{
    show_loading("Sincronizando...");
    ui->pb_new_info->setPixmap(QPixmap(":/icons/sync_button.png"));
    if(ui->pb_new_info->isEnabled()){
        ui->pb_new_info->setToolTip("No hay modificaciones desde la última sincronización");
    }
    QString queryStatus = defaultQuery;
    QString query = "( (`" + ultima_modificacion + "` LIKE '%" + "ANDROID" + "%') AND (" + queryStatus + ") )";
    if(currentGestor != "Todos"){
        query += " AND (" + GESTOR + " LIKE '" + currentGestor +"')";
    }
    getTareasAmountFromServer(empresa, query, limit_pagination);

    if(countTareas > 0){
        ui->pb_new_info->setPixmap(QPixmap(":/icons/sync_button_warning.png"));
        QString toolTip = "Hay tareas con cambios, puedes filtrar por cambios sin revisar";
        if(ui->pb_new_info->isEnabled()){
            ui->pb_new_info->setToolTip(toolTip);
        }
        hide_loading();
        return true;
    }
    hide_loading();
    return false;
}
QMap <QString,QString> Tabla::fillMapForFixModel(QStringList &listHeaders){
    QMap <QString,QString> mapa;
    QString fecha = "", calibre_ = "";
    if(ui->rb_ausente->isChecked()){
        fecha = "Fech.Ausencia";
        mapa.insert(fecha, fechas_tocado_puerta);
    }
    else if(ui->rb_citas->isChecked()){
        fecha = "Fech.Cita";
        mapa.insert(fecha,fecha_hora_cita);
    }else{
        fecha = "Fech.Import";
        mapa.insert(fecha,FechImportacion);
    }
    mapa.insert("Id.Ord",idOrdenCABB);
    mapa.insert("NUMIN", numero_interno);
    mapa.insert("CAUSA ORIGEN", ANOMALIA);
    mapa.insert("C.ACCIÓN ORD.", tipo_tarea);
    mapa.insert("AREALIZAR",AREALIZAR);
    mapa.insert("INTERVENCI",INTERVENCION);
    mapa.insert("REPARACION", reparacion);
    mapa.insert("PROP.",propiedad);
    mapa.insert("AÑO o PREFIJO", CONTADOR_Prefijo_anno);
    mapa.insert("SERIE",numero_serie_contador);
    mapa.insert("MARCA",marca_contador);
    mapa.insert("CALIBRE",calibre_toma);
    mapa.insert("RUEDAS", ruedas);
    mapa.insert("FECINST",fecha_instalacion);
    mapa.insert("ACTIVI",actividad);
    mapa.insert("EMPLAZA", emplazamiento);
    mapa.insert("ACCESO",acceso);
    mapa.insert("CALLE",calle);
    mapa.insert("NUME",numero);
    mapa.insert("BIS",BIS);
    mapa.insert("PISO",piso);
    mapa.insert("MANO",mano);
    mapa.insert("MUNICIPIO",poblacion);
    mapa.insert("NOMBRE",nombre_cliente);
    mapa.insert("ABONADO",numero_abonado);
    mapa.insert("CODLEC",ruta);
    mapa.insert("FECEMISIO",FECEMISIO);
    mapa.insert("FECULTREP",FECULTREP);
    mapa.insert("DATOS ESPECIFICOS",observaciones);
    mapa.insert("SECTOR P",zona);
    mapa.insert("PRIORIDAD",prioridad);
    mapa.insert("RS",resultado);
    mapa.insert("F_EJEC",F_INST);
    mapa.insert("LECT_INS",lectura_contador_nuevo);
    mapa.insert("EMPLAZADV",emplazamiento_devuelto);
    mapa.insert("RESTO_EM",RESTO_EM);
    mapa.insert("LECT_LEV",lectura_actual);
    mapa.insert("OBSERVADV",observaciones_devueltas);
    mapa.insert("Estado",status_tarea);
    mapa.insert("MARCADV",marca_devuelta);
    mapa.insert("CALIBREDV",calibre_real);
    mapa.insert("RUEDASDV",RUEDASDV);
    mapa.insert("LONG",LARGO);
    mapa.insert("LONGDV",largo_devuelto);
    mapa.insert("seriedv", numero_serie_contador_devuelto);
    mapa.insert("PREFIJO DV",CONTADOR_Prefijo_anno_devuelto);
    mapa.insert("CAUSA DESTINO",AREALIZAR_devuelta);
    mapa.insert("intervencidv",intervencion_devuelta);
    mapa.insert("RESTEMPLAZA",RESTEMPLAZA);
    mapa.insert("FECH_CIERRE",FECH_CIERRE);
    mapa.insert("TIPORDEN",TIPORDEN);
    mapa.insert("EQUIPO",equipo);
    mapa.insert("OPERARIO",operario);
    mapa.insert("observaciones",MENSAJE_LIBRE);
    mapa.insert("TIPOFLUIDO",TIPOFLUIDO_devuelto);
    mapa.insert("idexport",idexport);
    mapa.insert("fech_facturacion",fech_facturacion);
    mapa.insert("fech_cierrenew",fecha_de_cambio);
    mapa.insert("fech_informacionnew",fech_informacionnew);
    mapa.insert("tipoRadio",tipoRadio_devuelto);
    mapa.insert("REQUERIDA",marcaR);
    mapa.insert("Módulo",numero_serie_modulo);
    mapa.insert("C.EMPLAZAMIENTO",codigo_de_geolocalizacion);
    mapa.insert("Geolocalización",url_geolocalizacion);


    listHeaders <<"Id.Ord" << fecha << "CAUSA ORIGEN" << "C.ACCIÓN ORD." << "AREALIZAR"<< "INTERVENCI" <<"PROP."
               << "AÑO o PREFIJO"<<"SERIE" << "MARCA" << "CALIBRE"<< "RUEDAS" << "FECINST"
               << "ACTIVI" << "EMPLAZA" << "ACCESO"<< "CALLE"  << "NUME" << "BIS"
               << "PISO"<<"MANO" << "MUNICIPIO" << "NOMBRE" << "ABONADO" << "CODLEC"
               << "FECEMISIO" << "DATOS ESPECIFICOS"<< "SECTOR P"<< "PRIORIDAD"<< "RS"  << "F_EJEC" << "LECT_INS" << "EMPLAZADV"
               << "RESTO_EM"  << "LECT_LEV" << "OBSERVADV"<< "Estado"  << "MARCADV" << "CALIBREDV"
               << "RUEDASDV"<<"LONGDV" << "seriedv" << "PREFIJO DV"<< "CAUSA DESTINO" << "intervencidv"
               << "FECH_CIERRE"<<"TIPORDEN" << "EQUIPO" << "OPERARIO" << "observaciones"<< "TIPOFLUIDO"<< "tipoRadio"
               << "REQUERIDA" << "idexport"<<"fech_cierrenew" << "fech_informacionnew"
               << "Módulo" << "C.EMPLAZAMIENTO" << "Geolocalización";

    return mapa;
}
void Tabla::fixModelforTable(QJsonArray jsonArray, bool save_history)
{
    int rows = jsonArray.count();

    if(save_history){
        addItemsToPaginationInfo(rows);
    }

    //para historia de navegacion-----------------------------------------------------
    if(save_history){
        if(jsonArrayHistoryList.size() < MAX_JSONARRAY_HISTORY_SIZE){
            jsonArrayHistoryList.append(jsonArray);
            radioButtonsHistory.append(getCurrentRadioButton());
            countTareasHistory.append(countTareas);
            showingTextHistory.append(ui->l_cantidad_de_tareas->text());
            paginationHistory.append(ui->l_current_pagination->currentText());
            paginationItemsHistory.append(ui->l_current_pagination->getSpinnerList());
            queryHistory.append(lastQuery);
            id_startHistory.append(last_id_start);
        }else{
            jsonArrayHistoryList.removeAt(0);
            jsonArrayHistoryList.append(jsonArray);
            radioButtonsHistory.removeAt(0);
            radioButtonsHistory.append(getCurrentRadioButton());
            countTareasHistory.removeAt(0);
            countTareasHistory.append(countTareas);
            showingTextHistory.removeAt(0);
            showingTextHistory.append(ui->l_cantidad_de_tareas->text());
            paginationHistory.removeAt(0);
            paginationHistory.append(ui->l_current_pagination->currentText());
            paginationItemsHistory.removeAt(0);
            paginationItemsHistory.append(ui->l_current_pagination->getSpinnerList());
            queryHistory.removeAt(0);
            queryHistory.append(lastQuery);
            id_startHistory.removeAt(0);
            id_startHistory.append(last_id_start);
        }
    }
    if(jsonArrayHistoryList.size() >= 2){
        habilitarBotonAtras(true);
    }else{
        habilitarBotonAtras(false);
    }
    //fin para historia de navegacion--------------------------------------------------
    
    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QStringList listHeaders;
    QMap <QString,QString> mapa = fillMapForFixModel(listHeaders);

    fields_count_in_table = listHeaders.size();

    model = new QStandardItemModel(rows, listHeaders.size(), this);
    model->setHorizontalHeaderLabels(listHeaders);
    
    //insertando los datos
    QString column_info;
    rows_red.clear();
    for(int i = 0; i < rows; i++)
    {
        bool depreceated_color = false;
        QJsonObject jsonObject = jsonArray.at(i).toObject();

        QString prioridad_l = jsonObject.value(prioridad).toString().trimmed();

        QString msg = getViewOfTarea(jsonObject), obs = jsonObject.value(observaciones).toString().trimmed();
        if(checkIfFieldIsValid(obs)){
            msg += "\nOBS ->  "+ obs +"\n";
        }
        obs = jsonObject.value(observaciones_devueltas).toString().trimmed();
        if(checkIfFieldIsValid(obs)){
            msg += "OBS DV ->  "+ obs + "\n";
        }
        obs = jsonObject.value(MENSAJE_LIBRE).toString().trimmed();
        if(checkIfFieldIsValid(obs)){
            msg += "MSG LIBRE ->  "+ obs + "\n";
        }

        QString ult_mod = jsonObject.value(ultima_modificacion).toString().trimmed();

        if(checkIfFieldIsValid(ult_mod)){
            QString f_mod = jsonObject.value(date_time_modified).toString().trimmed();
            QDateTime date = QDateTime::fromString(f_mod, formato_fecha_hora);
            if(date.isValid()){
                ult_mod += " -> " + date.toString(formato_fecha_hora_new_view);
            }
            msg += "ULT MODIFICACIÓN -> "+ ult_mod;
        }


        QString fecha_cita = jsonObject.value(fecha_hora_cita).toString().trimmed();
        if(other_task_screen::checkIfFieldIsValid(fecha_cita)){
            QDateTime date_cita = QDateTime::fromString(fecha_cita, formato_fecha_hora);
            if(date_cita <= QDateTime::currentDateTime()){
                QString status = jsonObject.value(status_tarea).toString().trimmed();
                if(status.contains("IDLE")){
                    msg = "Cita Vencida -> " + date_cita.toString(formato_fecha_hora_new_view) + "\n" + msg;
                }
                if(getCurrentRadioButton() == CITA){
                    depreceated_color = true;
                }
            }
        }

        for (int n = 0; n < listHeaders.size(); n++) {
            QString header = (listHeaders.at(n)) ;
            QString value_header = mapa.value(listHeaders.at(n));
            if(n!=1){
                QString value = jsonObject.value(mapa.value(listHeaders.at(n))).toString();
                if(value_header != marca_contador && value_header != zona){
                    if(value.contains(" - ")){
                        value = value.split(" - ").at(0).trimmed();
                    }
                }
                column_info = value;
            }else{
                column_info = changeFechaFormat(jsonObject.value(mapa.value(listHeaders.at(n))).toString());
            }

            column_info = returnIfSerieOrPrefijo(jsonArray, i, value_header, column_info);

            if(header.contains("Fec", Qt::CaseInsensitive) || header == "F_INST"){
                QString value = jsonObject.value(mapa.value(listHeaders.at(n))).toString();
                QDateTime date = QDateTime::fromString(value, formato_fecha_hora);
                column_info = date.toString(formato_fecha_hora_new_view);
            }
            if(value_header == status_tarea) {
                QString status = jsonObject.value(status_tarea).toString().trimmed();
                column_info = mapaEstados.key(status).left(1);
            }
            if(value_header == TIPORDEN) {
                QString tipoorden = jsonObject.value(TIPORDEN).toString().trimmed();
                column_info = tipoorden.left(1);
            }
            if(value_header == url_geolocalizacion) {
                if(!checkIfFieldIsValid(column_info)){
                    QStringList fields;
                    fields << codigo_de_localizacion << geolocalizacion;
                    QString geoCode = getValidField(jsonObject, fields);
                    if(checkIfFieldIsValid(geoCode)){
                        column_info = "https://maps.google.com/?q=" +geoCode;
                    }
                }
            }
            if(value_header == fechas_tocado_puerta) {
                if(!checkIfFieldIsValid(jsonObject.value(fechas_tocado_puerta).toString()) &&
                        !checkIfFieldIsValid(jsonObject.value(fechas_nota_aviso).toString()) &&
                        !checkIfFieldIsValid(jsonObject.value(telefonos_cliente).toString())){
                    QString cita = jsonObject.value(fecha_hora_cita).toString().trimmed();
                    if(checkIfFieldIsValid(cita)){
                        column_info = "Cita Vencida:\n";
                        column_info += changeFechaFormat(cita);
                        depreceated_color = true;
                    }
                }else{
                    QStringList l, list_f;
                    column_info="";
                    QString tocado = jsonObject.value(fechas_tocado_puerta).toString().trimmed();
                    if(checkIfFieldIsValid(tocado)){
                        column_info = "Tocado en Puerta:\n";
                        list_f = tocado.split("\n");
                        for (int c =0; c < list_f.size(); c++) {
                            l.append(changeFechaFormat(list_f.at(c)));
                        }
                        column_info += l.join("\n") + "\n\n";
                        l.clear();
                    }
                    
                    QString notas = jsonObject.value(fechas_nota_aviso).toString().trimmed();
                    if(checkIfFieldIsValid(notas)){
                        column_info += "Nota de Aviso:\n";
                        list_f = notas.split("\n");
                        for (int c =0; c < list_f.size(); c++) {
                            l.append(changeFechaFormat(list_f.at(c)));
                        }
                        column_info += l.join("\n")+ "\n\n";
                        l.clear();
                    }
                    
                    QString tels = jsonObject.value(telefonos_cliente).toString().trimmed();
                    if(checkIfFieldIsValid(tels)){
                        column_info += "Info Teléfonos:\n" + tels;
                    }
                    
                    QString cita = jsonObject.value(fecha_hora_cita).toString().trimmed();
                    if(checkIfFieldIsValid(cita)){
                        column_info += "\n\nCita Vencida:\n" +changeFechaFormat(cita);
                        rows_red << i;
                        depreceated_color = true;
                    }
                }
            }

            //            item->setData(column_info,Qt::EditRole);

            if(!checkIfFieldIsValid(column_info)){ column_info = "";}

            QModelIndex index = model->index(i, n, QModelIndex());
            if(depreceated_color){ //Pone en amarillo citas vencidas
                if(n - 1 >= 0){
                    index = model->index(i, n - 1, QModelIndex());
                    model->setData(index, QColor(229,190,50), Qt::BackgroundRole);//citas vencidas
                }
                index = model->index(i, n, QModelIndex());
                model->setData(index, QColor(229,190,50), Qt::BackgroundRole);//citas vencidas
            }
            bool nueva_info = false;
            if(ult_mod.contains("ANDROID", Qt::CaseInsensitive)){
                nueva_info = true;
                if(depreceated_color){
                    model->setData(index, QColor(0,100,0), Qt::ForegroundRole);
                    QFont font = ui->tableView->font();
                    font.setBold(true);
                    model->setData(index, font, Qt::FontRole); //Texto rojo en Negritas oscuro alta prioridad
                }else{
                    model->setData(index, QColor(255,255,255), Qt::ForegroundRole);
                    model->setData(index, QColor(0,100,0), Qt::BackgroundRole);//Tareas sin revisar //verde
                }
            }
            if(prioridad_l == "ALTA"){
                if(nueva_info && !depreceated_color){
                    model->setData(index, QColor(255,100,100), Qt::ForegroundRole); //Texto en rojo claro alta prioridad para nueva informacion
                }else{
                    model->setData(index, QColor(255,50,50), Qt::ForegroundRole); //Texto en rojo oscuro alta prioridad
                }
                QFont font = ui->tableView->font();
                font.setBold(true);
                model->setData(index, font, Qt::FontRole); //Texto rojo en Negritas oscuro alta prioridad
            }
            if(checkIfFieldIsValid(msg)){
                model->setData(index, msg, Qt::ToolTipRole);
            }else{
                model->setData(index, "Esta tarea no tiene mensaje libre", Qt::ToolTipRole);
            }
            model->setData(index, column_info);
        }
    }
    
    if(jsonArray.isEmpty()){
        ui->statusbar->showMessage("No se encontraron resultados. Presione atras para "
                                   "ver ultima tabla guardada.", 4000);
    }
}
QString Tabla::changeFechaFormat(QString f){
    if(checkIfFieldIsValid(f)){
        QDateTime dt = QDateTime::fromString(f, "yyyy-MM-dd hh:mm:ss");
        return dt.toString("dd/MM/yy hh:mm");
    }else{
        return "";
    }
    
}
bool Tabla::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

void Tabla::setTableView(bool delegate)
{
    Q_UNUSED(delegate);
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));
    timer.stop();
    if(model!=nullptr){
        ui->tableView->setModel(model);

        ui->tableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

        QList<double> sizes;
        sizes << 0.5/*ordCabb*/ << 1.2 /*Fech Import*/<< 1/*CausaOrigen*/<< 1/*c.AccionOrdenada*/<< 1.5/*ARealizar*/<< 2/*Intervenc*/
              <<  0.5/*PROPIEDAD*/ <<  1/*AÑO o PREFIJO*/ <<  1/*SERIE*/ << 1.8/* MARCA*/ <<  0.5/* CALIBRE*/ << 0.5/*ruedas*/ << 1.2/* FECINST*/
               <<  1.2/*ACTIVI */ <<  1.5/*EMPLAZA */ <<  1.5/*ACCESO */ <<  2/*CALLE */ <<  0.5/*NUME */ <<  0.5/* BIS*/ <<  0.5/* PISO*/
                <<  0.5/*MANO */ <<  1.2/*MUNICIPIO */ <<  2.5/*NOMBRE */ << 1/*ABONADO */ <<  0.75/*CODLEC */ <<  1.2/*FECEMISIO */
                 <<  3/*DATOS ESPECIFICOS */ <<  2/*SECTOR P */ <<  1/*prioridad */ <<  0.5/*RS */ <<  1.5/* F_EJEC*/ <<  0.75/* LECT_INS*/<<  2/*EMPLAZADV*/
                  <<  1/*RESTO_EM*/ <<  1/*LECT_LEV*/ <<  2/*OBSERVADV*/<<  0.5/*Estado*/ << 2/*MARCADV*/<<  0.75/*CALIBREDV*/ <<  0.75/*RUEDASDV*/
                   <<  0.5/*LONGDV*/<<  1.2/*seriedv*/ <<  0.75/*PREFIJO DV*/<<  1/*CAUSA DESTINO*/ <<  1/*intervencidv*/ << 1.2 /*FECH_CIERRE*/
                    <<  0.75/*TIPORDEN*/<<  1/*EQUIPO*/<<1/*OPERARIO*/<< 2 /*observaciones*/<< 1 /*TIPOFLUIDO*/<< 0.5 /*TIPORadio*/<< 0.75 /*Requerida*/<< 0.5/*idexport*/
                     <<  1.2/*fech_cierrenew*/ <<  1.2/*fech_informacionnew*/<< 2.2/*Módulo*/<<  1.2/*C.EMPLAZAMIENTO*/<<  4/*Geolocalización*/<<1<<1;

        QFont font = ui->tableView->font();
        int pointSize = font.pointSize();
        float ratio = static_cast<float>(pointSize)/10;
        QRect rect = QGuiApplication::screens().first()->geometry();
        int width = ui->tableView->size().width();
        if(width >= rect.width()){
            width = rect.width();
        }
        int width_table = width - 40;
        float medium_width_fileds = static_cast<float>(width_table)/16/*fields_count_in_table*/;

        for (int i=0; i < fields_count_in_table; i++) {
            //if( ui->tableView->columnWidth(i) > static_cast<int>(medium_width_fileds)){
            ui->tableView->setColumnWidth(i, static_cast<int>(medium_width_fileds)*sizes.at(i)*ratio);
            //}
        }

        ComboBoxDelegate *cb_delegate = new ComboBoxDelegate(this);
        connect(cb_delegate, SIGNAL(doubleClickedComboBox(QModelIndex)), this, SLOT(on_tableView_doubleClicked(QModelIndex)));
        ui->tableView->setItemDelegate(cb_delegate);

        //        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->horizontalHeader()->setSectionsMovable(true);
        ui->tableView->horizontalHeader()->setFont(ui->tableView->font());

        for (int i=0; i < model->columnCount(); i++) {
            model->horizontalHeaderItem(i)->setToolTip(model->horizontalHeaderItem(i)->text());
        }
        if(!connected_header_signal){
            connected_header_signal = true;
            connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),this, SLOT(on_sectionClicked(int)));
        }
    }
}

QString Tabla::getViewOfTarea(QJsonObject jsonObject){
    QString view = "";
    QString field = jsonObject.value(poblacion).toString();
    if(checkIfFieldIsValid(field)){
        view += field +", ";
    }
    field = jsonObject.value(calle).toString();
    if(checkIfFieldIsValid(field)){
        view += field +", ";
    }
    field = jsonObject.value(numero).toString();
    if(checkIfFieldIsValid(field)){
        view += field +" ";
    }
    field = jsonObject.value(BIS).toString();
    if(checkIfFieldIsValid(field)){
        view += field;
    }
    field = jsonObject.value(numero_abonado).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nABONADO " + field +", ";
    }
    field = jsonObject.value(nombre_cliente).toString();
    if(checkIfFieldIsValid(field)){
        view += field;
    }
    field = jsonObject.value(tipo_tarea).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nTAREA " + field +", ";
    }
    field = jsonObject.value(numero_serie_contador).toString();
    if(checkIfFieldIsValid(field)){
        view += " CONT "+ field;
    }
    field = jsonObject.value(telefono1).toString();
    if(checkIfFieldIsValid(field)){
        view += "\nTELF1 "+ field + "   ";
    }
    field = jsonObject.value(telefono2).toString();
    if(checkIfFieldIsValid(field)){
        view += "TELF2 "+ field;
    }
    field = jsonObject.value(nuevo_citas).toString();
    if(checkIfFieldIsValid(field)){
        view += "\n\nCITA "+ field +"\n";
    }
    field = jsonObject.value(prioridad).toString().trimmed();
    if(field == "HIBERNAR"){
        field = jsonObject.value(hibernacion).toString().trimmed();
        if(checkIfFieldIsValid(field)){
            QStringList split = field.split("::");
            if(split.size()>1){
                field = split.at(0).trimmed();
            }
            QDateTime dt = QDateTime::fromString(field, formato_fecha_hora);
            view += "\nHIBERNADA HASTA ->  "+ dt.toString(formato_fecha_hora_new_view);
        }
    }
    return view;
}

void Tabla::on_sectionClicked(int logicalIndex)
{
    //    ui->statusbar->showMessage("Field C:" + listHeaders.at(logicalIndex) + "Var C:" + mapa.value(listHeaders.at(logicalIndex)) + "  C:" + QString::number(logicalIndex) );
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
    QPoint pos = QCursor::pos();
    QRect rect = QGuiApplication::screens().first()->geometry();
    if(pos.x() > rect.width()/2){
        pos.setX(pos.x()-172); //172 ancho del widgetMenuSection
    }
    RightClickedSection *rightClickedSection = new RightClickedSection(0, pos);
    connect(rightClickedSection, SIGNAL(clickPressed(int)), this, SLOT(getMenuSectionClickedItem(int)));
    connect(this, &Tabla::closing, rightClickedSection, &RightClickedSection::deleteLater);
    connect(this, &Tabla::tablePressed, rightClickedSection, &RightClickedSection::deleteLater);
    connect(this, &Tabla::mouse_pressed, rightClickedSection, &RightClickedSection::deleteLater);

    rightClickedSection->show();
}

QStringList Tabla::getFieldValues(QString field){

    QStringList values;
    if(solo_tabla_actual){
        QJsonArray jsonArray;
        jsonArray = getCurrentJsonArrayInTable();
        QString value;
        for (int i=0; i < jsonArray.size(); i++) {
            value = jsonArray.at(i).toObject().value(field).toString();
            if(checkIfFieldIsValid(value)){
                if(campos_de_fechas.contains(field)){
                    value = value.trimmed().split(" ").at(0).trimmed();
                }
                if(!values.contains(value)){
                    values << value;
                }
            }
        }
    }else{
        show_loading("Cargando valores...");
        QString queryStatus = getQueyStatus();
        QString query = " (" + queryStatus + ") ";
        bool res = getTareasValuesFieldCustomQueryServer(
                    empresa, field, query);
        if(res){
            QStringList keys = jsonObjectValues.keys();
            for (int i= 0; i < keys.size(); i++) {
                QString key = keys.at(i);
                if(!key.contains("query") && !key.contains("count_values")){
                    QString value = jsonObjectValues.value(key).toString();
                    if(!values.contains(value, Qt::CaseInsensitive) && checkIfFieldIsValid(value)){
                        values << value;
                    }
                }
            }
        }
        hide_loading();
    }
    values.sort();
    return values;
}
void Tabla::filterColumnField(){

    if(filterColumnList.isEmpty()){
        return;
    }
    show_loading("Buscando Resultados...");
    emit hideMenuFast("");

    QJsonArray jsonArray;

    if(solo_tabla_actual){
        jsonArray = getCurrentJsonArrayInTable();
        jsonArrayInTableFiltered = QJsonArray();
        for(int i=0; i< jsonArray.size(); i++){
            QJsonObject jsonObject = jsonArray[i].toObject();
            QString value = jsonObject.value(lastSectionField).toString().trimmed();
            if(checkIfFieldIsValid(value)){
                if(campos_de_fechas.contains(lastSectionField)){
                    value = value.trimmed().split(" ").at(0).trimmed();
                }
                if(filterColumnList.contains(value, Qt::CaseInsensitive)){
                    jsonArrayInTableFiltered.append(jsonObject);
                }
            }
        }
        jsonInfoTareasAmount = QJsonObject();
        countTareas = jsonArrayInTableFiltered.size();
        currentPage = 1;
    }else{

        QString queryStatus = getQueyStatus();
        QString query = "(";
        QString value;
        foreach(value, filterColumnList){
            if(query == "("){
                query += " ( (`" + lastSectionField + "` LIKE '" + value + "%') ";
            }else{
                query += " OR (`" + lastSectionField + "` LIKE '" + value + "%')";
            }
        }
        if(query != "("){
            query += ") AND ";
        }
        query +=  " (" + queryStatus + ") )";
        getTareasCustomQuery(query);
        jsonArrayInTableFiltered = jsonArrayAll;
    }

    filtering = true;
    filterColumnList.clear();
    fixModelforTable(jsonArrayInTableFiltered);
    setTableView();
    hide_loading();
    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
}
void Tabla::filtrarEnTabla(bool checked){
    solo_tabla_actual = checked;
    emit sectionPressed();

    showFilterWidgetOptions(false);
}
QString Tabla::getScrollBarStyle(){
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
void Tabla::showFilterWidgetOptions(bool offset){

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
    cb_todos->setText("Marcar Todos");
    cb_todos->setFont(f);

    QCheckBox *cb_tabla_actual = new QCheckBox();
    cb_tabla_actual->setStyleSheet("color: rgb(255, 255, 255);"
                                   "background-color: rgba(77, 77, 77);");
    cb_tabla_actual->setText("Tabla Actual");
    cb_tabla_actual->setFont(f);
    cb_tabla_actual->setChecked(solo_tabla_actual);
    connect(cb_tabla_actual, &QCheckBox::toggled, this, &Tabla::filtrarEnTabla);

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
        connect(cb, &MyCheckBox::toggleCheckBox, this, &Tabla::addRemoveFilterList);
        connect(lineEdit, &MyLineEditShine::textChanged, cb, &MyCheckBox::onTextSelectedChanged);
        connect(cb_todos, &QCheckBox::toggled, cb, &MyCheckBox::set_Checked);

        QFont font = ui->tableView->font();
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
    connect(button_filter, &QPushButton::clicked, this, &Tabla::filterColumnField);

    QFont font = ui->tableView->font();
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
    connect(this, &Tabla::sectionPressed, widget, &QWidget::deleteLater);
    connect(this, &Tabla::closing, widget, &QWidget::deleteLater);
    connect(this, &Tabla::tablePressed, widget, &QWidget::deleteLater);
}

void Tabla::addRemoveFilterList(QString value){
    if(filterColumnList.contains(value)){
        filterColumnList.removeOne(value);
    }else{
        filterColumnList << value;
    }
}

void Tabla::getMenuSectionClickedItem(int selection){
    if(selection == RightClickedSection::FILTRAR){
        showFilterWidgetOptions();
    }
    else if(selection == RightClickedSection::ORDENAR_ASCENDENTE){
        if((lastSectionField != lectura_actual) && (lastSectionField != ruta) &&
                (lastSectionField != numero_abonado) && (lastSectionField != idOrdenCABB)){
            ordenarPor(lastSectionField, "", false);
        }else{
            ordenarPor(lastSectionField, 0, false);
        }
    }
    else if(selection == RightClickedSection::ORDENAR_DESCENDENTE){
        if((lastSectionField != lectura_actual) && (lastSectionField != ruta) &&
                (lastSectionField != numero_abonado) && (lastSectionField != idOrdenCABB)){
            ordenarPor(lastSectionField, "MAYOR_MENOR", false);
        }else{
            ordenarPor(lastSectionField, 1, false);
        }
    }
}
void Tabla::showTodas(){

}
void Tabla::updateTareasInTable(){
    if(this->isHidden()){
        return;
    }
    show_loading("Actualizando tabla...");
    QString query = lastQuery;
    int id_start = last_id_start;
    getTareasCustomQuery(query, id_start);

    if(filtering){
        jsonArrayInTableFiltered = fixJsonToLastModel(jsonArrayInTableFiltered);
        fixModelforTable(jsonArrayInTableFiltered);
    }else{
        jsonArrayInTable = fixJsonToLastModel(jsonArrayInTable);
        fixModelforTable(jsonArrayInTable);
    }
    setTableView();
    screen_tabla_tareas::lastSync = QDateTime::currentDateTime().toString(formato_fecha_hora);
    screen_tabla_tareas::writeVariablesInBD();

    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    if(lastSelectedRow != -1 && lastSelectedRow < jsonArray.size() && lastSelectedRow < model->rowCount()){
        QModelIndex index = model->index(lastSelectedRow, 0);
        ui->tableView->setCurrentIndex(index);
    }
    hide_loading();
}
void Tabla::getTareas()
{
    currentGestor = selectGestor();
    if(currentGestor.trimmed().isEmpty()){
        this->on_pb_close_clicked();
    }
    checkIfNewInfoSync();
    on_rb_todas_clicked();
    habilitarBotonAtras(false);
}
bool Tabla::getTareasCustomQuery(QString query, int id_start){

    if(currentGestor != "Todos"){
        QString gestorQuery = " AND (" + GESTOR + " LIKE '" + currentGestor +"')";
        if(!query.contains(gestorQuery)){
            query += gestorQuery;
        }
    }
    if(lastQuery != query){
        currentPage = 1;
    }
    lastQuery = query;
    last_id_start = id_start;

    getTareasAmountFromServer(empresa, query, limit_pagination);

    bool res = getTareasFromServer(empresa, query, limit_pagination, id_start);
    qDebug()<<"query send -> "<< query << " <- id start" << id_start;
    return res;
}
void Tabla::on_rb_todas_clicked()
{
    show_loading("Cargando todas...");
    hideAllFilters();
    uncheckAllRadioButtons(TODAS);
    filtering = false;
    selected_status = "TODAS";

    QString query = defaultQuery;
    getTareasCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;

    fixModelforTable(jsonArrayInTable);
    setTableView();

    setRadiobutton(TODAS);
    hide_loading();
}

QString Tabla::getQueyStatus(){
    QString queryStatus = defaultQuery;
    if(selected_status != "TODAS"){
        if(selected_status != "AUSENTE" && selected_status != "CITA"){
            queryStatus = status_tarea + " LIKE '%" + selected_status + "%'";
        }
        else{
            if(selected_status == "AUSENTE"){
                queryStatus = "(" + status_tarea + " LIKE '%CITA%') AND "
                        + "( (" + fecha_hora_cita + " IS NULL)"
                        + " OR (" + fecha_hora_cita + " LIKE 'null')"
                        + " OR (" + fecha_hora_cita + " LIKE '')"
                        + ")";
            }
            else if(selected_status == "CITA"){
                queryStatus = "(" + status_tarea + " LIKE '%CITA%') AND "
                        +"( (" + fecha_hora_cita + " IS NOT NULL)"
                        +" AND (" + fecha_hora_cita + " <> 'null')"
                        +" AND (" + fecha_hora_cita + " <> '')"
                        +")";
            }
        }
    }
    QString query = " (" + queryStatus + ") ";
    return query;
}
void Tabla::on_rb_abierta_clicked()
{
    show_loading("Cargando abiertas...");
    hideAllFilters();
    uncheckAllRadioButtons(ABIERTAS);
    filtering = false;
    ui->statusbar->showMessage("Mostrando Abiertas");

    selected_status = "IDLE";
    QString query = getQueyStatus();
    getTareasCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;


    ordenarPor(FechImportacion, "MAYOR_MENOR");
    hide_loading();
}
void Tabla::on_rb_ausente_clicked()
{
    show_loading("Cargando ausentes...");
    hideAllFilters();
    uncheckAllRadioButtons(AUSENTE);
    filtering = false;
    ui->statusbar->showMessage("Mostrando Tareas con Ausentes");

    selected_status = "AUSENTE";
    QString query = getQueyStatus();
    getTareasCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;

    ordenarPor(date_time_modified, "MAYOR_MENOR");
    hide_loading();
}
void Tabla::on_rb_citas_clicked()
{
    show_loading("Cargando citas...");
    hideAllFilters();
    uncheckAllRadioButtons(CITA);
    filtering = false;
    ui->statusbar->showMessage("Mostrando Tareas con Citas");
    selected_status = "CITA";
    QString query = getQueyStatus();
    getTareasCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;


    ordenarPor(fecha_hora_cita, "");
    hide_loading();
}
void Tabla::on_rb_ejecutada_clicked()
{
    show_loading("Cargando ejecutadas...");
    hideAllFilters();
    uncheckAllRadioButtons(EJECUTADA);
    filtering = false;
    ui->statusbar->showMessage("Mostrando Ejecutadas");
    selected_status = "DONE";

    QString query = getQueyStatus();
    getTareasCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;


    ordenarPor(F_INST, "MAYOR_MENOR");
    hide_loading();
}
void Tabla::on_rb_cerrada_clicked()
{
    show_loading("Cargando cerradas...");
    hideAllFilters();
    uncheckAllRadioButtons(CERRADA);
    filtering = false;
    ui->statusbar->showMessage("Mostrando Cerradas");

    selected_status = "CLOSED";

    QString query = getQueyStatus();
    getTareasCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;


    ordenarPor(FECH_CIERRE, "MAYOR_MENOR");
    hide_loading();
}
void Tabla::on_rb_informada_clicked()
{
    show_loading("Cargando informadas...");
    hideAllFilters();
    uncheckAllRadioButtons(INFORMADA);
    filtering = false;
    ui->statusbar->showMessage("Mostrando Informadas");

    selected_status = "INFORMADA";

    QString query = getQueyStatus();
    getTareasCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;


    ordenarPor(fech_informacionnew, "MAYOR_MENOR");// esto para ordenar
    hide_loading();
}
void Tabla::on_rb_requerida_clicked()
{
    show_loading("Cargando requeridas...");
    hideAllFilters();
    uncheckAllRadioButtons(REQUERIDA);
    filtering = false;
    ui->statusbar->showMessage("Mostrando Requeridas");

    selected_status = "REQUERIDA";

    QString query = getQueyStatus();
    getTareasCustomQuery(query);
    jsonArrayInTable = jsonArrayAll;


    fixModelforTable(jsonArrayInTable);
    setTableView();
    hide_loading();
}

QJsonArray Tabla::fillFilterOrdenABIERTAS(QJsonArray jsonArray){
    QJsonArray jsonArrayforTable = QJsonArray();
    for(int i=0; i< jsonArray.size(); i++){
        QString status = jsonArray[i].toObject().value(status_tarea).toString().trimmed();
        if(status.contains("IDLE")){
            jsonArrayforTable.append(jsonArray[i]);
        }
    }
    return jsonArrayforTable;
}
QJsonArray Tabla::fillFilterOrdenAUSENTES(QJsonArray jsonArray){
    QJsonArray jsonArrayforTable = QJsonArray();
    for(int i=0; i< jsonArray.size(); i++){
        QString status = jsonArray[i].toObject().value(status_tarea).toString().trimmed();
        if(status.contains("CITA")){
            QString fecha_cita = jsonArray[i].toObject().value(fecha_hora_cita).toString().trimmed();
            if(!other_task_screen::checkIfFieldIsValid(fecha_cita)){
                jsonArrayforTable.append(jsonArray[i]);
            }
        }
    }
    return jsonArrayforTable;
}
QJsonArray Tabla::fillFilterOrdenCITAS(QJsonArray jsonArray){
    QJsonArray jsonArrayforTable = QJsonArray();
    for(int i=0; i< jsonArray.size(); i++){
        QString status = jsonArray[i].toObject().value(status_tarea).toString().trimmed();
        if(status.contains("CITA")){
            QString fecha_cita = jsonArray[i].toObject().value(fecha_hora_cita).toString().trimmed();
            if(other_task_screen::checkIfFieldIsValid(fecha_cita)){
                ////Este codigo comentado capta las citas pendientes y las pone en citas
                //QDateTime date_cita = QDateTime::fromString(fecha_cita, formato_fecha_hora);
                //if(date_cita > QDateTime::currentDateTime()){
                jsonArrayforTable.append(jsonArray[i]);
                //}
            }
        }
    }
    return jsonArrayforTable;
}
QJsonArray Tabla::fillFilterOrdenEJECUTADAS(QJsonArray jsonArray){
    QJsonArray jsonArrayforTable = QJsonArray();
    for(int i=0; i< jsonArray.size(); i++){
        QString status = jsonArray[i].toObject().value(status_tarea).toString().trimmed();
        if(status.contains("DONE")){
            jsonArrayforTable.append(jsonArray[i]);
        }
    }
    return jsonArrayforTable;
}
QJsonArray Tabla::fillFilterOrdenCERRADAS(QJsonArray jsonArray){
    QJsonArray jsonArrayforTable = QJsonArray();
    for(int i=0; i< jsonArray.size(); i++){
        QString status = jsonArray[i].toObject().value(status_tarea).toString().trimmed();
        if(status.contains("CLOSED")){
            jsonArrayforTable.append(jsonArray[i]);
        }
    }
    return jsonArrayforTable;
}
QJsonArray Tabla::fillFilterOrdenINFORMADAS(QJsonArray jsonArray){
    QJsonArray jsonArrayforTable = QJsonArray();
    for(int i=0; i< jsonArray.size(); i++){
        QString status = jsonArray[i].toObject().value(status_tarea).toString().trimmed();
        if(status.contains("INFORMADA")){
            jsonArrayforTable.append(jsonArray[i]);
        }
    }
    return jsonArrayforTable;
}
QJsonArray Tabla::fillFilterOrdenREQUERIDAS(QJsonArray jsonArray){
    QJsonArray jsonArrayforTable = QJsonArray();
    for(int i=0; i< jsonArray.size(); i++){
        QString status = jsonArray[i].toObject().value(status_tarea).toString().trimmed();
        if(status.contains("REQUERIDA")){
            jsonArrayforTable.append(jsonArray[i]);
        }
    }
    return jsonArrayforTable;
}




void clearWidgets(QLayout * layout) {
    if (! layout)
        return;
    while (auto item = layout->takeAt(0)) {
        delete item->widget();
        clearWidgets(item->layout());
    }
}

void Tabla::on_actionTipo_de_Tarea_triggered()
{
    add_causas_to_select();
}
void Tabla::add_causas_to_select()
{
    show_loading("Cargando causas...");
    hideAllFilters();
    filter_type |= F_CAUSA_ORIGEN;
    QStringList anomalias;  //ANOMALIA

    filter_column_field_selected = ANOMALIA;
    QString queryStatus = getQueyStatus();
    QString query = " (" + queryStatus +  ") ";
    bool res = getTareasValuesFieldCustomQueryServer(
                empresa, filter_column_field_selected, query);
    if(res){
        QStringList values;
        QStringList keys = jsonObjectValues.keys();
        for (int i= 0; i < keys.size(); i++) {
            QString key = keys.at(i);
            if(!key.contains("query") && !key.contains("count_values")){
                QString value = jsonObjectValues.value(key).toString();
                if(!values.contains(value, Qt::CaseInsensitive) && checkIfFieldIsValid(value)){
                    values << value;
                }
            }
        }
        anomalias = values;
    }
    anomalias.sort();
    ui->cb_tipoTarea->clear();
    clearWidgets(ui->widget_filtro_tiposTarea_checkboxes->layout());
    ui->cb_tipoTarea->addItems(anomalias);

    ui->widget_filtro_tipoTarea->show();
    ui->widget_filtros->show();

    hide_loading();
}

void Tabla::add_calibres_to_select(const QString &anomalia)
{
    QStringList calibres;
    for(int i=0; i< ui->cb_calibre->count(); i++){
        calibres.append(ui->cb_calibre->itemText(i));
    }
    QString queryStatus = getQueyStatus();
    filter_column_field_selected = calibre_toma;
    QString query = " (" + ANOMALIA + " LIKE '" + anomalia +  "') "
            + " AND (" + queryStatus + ")";
    bool res = getTareasValuesFieldCustomQueryServer(
                empresa, filter_column_field_selected, query);
    if(res){
        QStringList keys = jsonObjectValues.keys();
        for (int i= 0; i < keys.size(); i++) {
            QString key = keys.at(i);
            if(!key.contains("query") && !key.contains("count_values")){
                QString value = jsonObjectValues.value(key).toString();
                if(!calibres.contains(value, Qt::CaseInsensitive) && checkIfFieldIsValid(value)){
                    calibres << value;
                }
            }
        }
    }
    calibres.sort();
    if(!calibres.contains("Todos")){
        calibres.prepend("Todos");
    }else{
        calibres.removeOne("Todos");
        calibres.prepend("Todos");
    }
    ui->cb_calibre->clear();
    ui->cb_calibre->addItems(calibres);
}

void Tabla::on_pb_filtrar_clicked()
{
    filter_enabled = false;
    emit hideMenuFast("");
    show_loading("Buscando resultados...");
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    hideAllFilters();
    jsonArrayInTableFiltered = QJsonArray();

    QString valueToFilter = ui->le_a_filtrar->text().trimmed();
    filtering = true;

    if(filter_type == F_DIRECCION){
        QString municipio_selected = ui->le_poblacion->text();
        QString street_selected = ui->le_calle->text();
        QString portal_selected = ui->cb_portal->currentText();
        QString bis_selected = ui->cb_BIS->currentText();

        ui->le_poblacion->setText("");
        ui->le_calle->setText("");

        QString queryStatus = getQueyStatus();
        QString query = "(";
        if(!municipio_selected.isEmpty()){
            query += "(" + poblacion + " LIKE '" + municipio_selected + "') AND ";
        }
        if(!street_selected.isEmpty()){
            query += "(" + calle + " LIKE '" + street_selected + "') AND ";
        }
        if(portal_selected != "Todos"){
            query += "(" + numero + " LIKE '" + portal_selected + "') AND ";
        }
        if(bis_selected != "Todos"){
            query += "(" + BIS + " LIKE '" + bis_selected + "') AND ";
        }

        query +=  " (" + queryStatus + ") )";
        getTareasCustomQuery(query);
        jsonArrayInTableFiltered = jsonArrayAll;


        ordenarPor(numero, 0);
        filter_type = 0;
        hide_loading();
        return;
    }
    else if(filter_type == F_CAUSA_ORIGEN){
        QStringList anomalias_seleccionadas;
        int c= ui->widget_filtro_tiposTarea_checkboxes->children().size();
        for (int i=0; i < c; i++) {
            QString name = ui->widget_filtro_tiposTarea_checkboxes->
                    children().at(i)->objectName();
            if(name.contains("cb_")){
                if((static_cast<QCheckBox*>(ui->widget_filtro_tiposTarea_checkboxes->
                                            children().at(i)))->isChecked()){
                    anomalias_seleccionadas.append(name.remove("cb_"));
                    static_cast<QCheckBox*>((ui->widget_filtro_tiposTarea_checkboxes->
                                             children().at(i)))->deleteLater();
                }else {
                    static_cast<QCheckBox*>((ui->widget_filtro_tiposTarea_checkboxes->
                                             children().at(i)))->deleteLater();
                }
            }
        }
        QString queryStatus = getQueyStatus();
        QString query = "(";
        QString anomalia;
        foreach(anomalia, anomalias_seleccionadas){
            if(query == "("){
                query += " ( (" + ANOMALIA + " LIKE '" + anomalia + "') ";
            }else{
                query += " OR (" + ANOMALIA + " LIKE '" + anomalia + "')";
            }
        }
        if(query != "("){
            query += ") AND ";
        }
        QString calibre_selected = ui->cb_calibre->currentText().trimmed();
        if(calibre_selected != "Todos"){
            query += "(" + calibre_toma + " LIKE '" + calibre_selected + "') AND ";
        }
        query +=  " (" + queryStatus + ") )";
        getTareasCustomQuery(query);
        jsonArrayInTableFiltered = jsonArrayAll;
    }
    else if(filter_type == F_SECTION){
        QString queryStatus = getQueyStatus();
        QString query = "( (`" + lastSectionField + "` LIKE '" + valueToFilter + "%') AND (" + queryStatus + ") )";
        getTareasCustomQuery(query);
        jsonArrayInTableFiltered = jsonArrayAll;

        ui->le_a_filtrar->setText("");
    }
    else{
        QString queryStatus = getQueyStatus();
        QString query = "( (`" + filter_column_field_selected + "` LIKE '" + valueToFilter + "%') AND (" + queryStatus + ") )";
        getTareasCustomQuery(query);
        jsonArrayInTableFiltered = jsonArrayAll;
    }

    filter_type = 0;

    fixModelforTable(jsonArrayInTableFiltered);
    setTableView();
    hide_loading();
}

void Tabla::on_actionContadores_Unitarios_triggered()
{
    show_loading("Buscando Resultados...");
    hideAllFilters();
    filter_type |= F_NO_EN_BATERIA;
    filtering = true;

    QString queryStatus = getQueyStatus(); //'value%' buscar q empiece con values
    QString query = "(((" + acceso + " NOT LIKE 'BAT%')"
            + " AND (`" + emplazamiento + "` NOT LIKE '" + "BATERIA" + "')"
            + " AND (`" + emplazamiento + "` NOT LIKE '" + "BATERÍA" + "')"
            + " AND (`" + emplazamiento + "` NOT LIKE '" + "BAT" + "')"
            + " AND (`" + emplazamiento + "` NOT LIKE '" + "BA-" + "%')"
            + " AND (`" + emplazamiento + "` NOT LIKE '" + "BT-" + "%')"
            + ") AND (" + queryStatus + "))";
    getTareasCustomQuery(query);
    jsonArrayInTableFiltered = jsonArrayAll;

    filter_type = 0;

    fixModelforTable(jsonArrayInTableFiltered);
    setTableView();
    hide_loading();

}

void Tabla::on_actionContadores_en_Bater_a_triggered()
{
    show_loading("Buscando Resultados...");
    hideAllFilters();
    filter_type |= F_EN_BATERIA;

    filtering = true;
    QString queryStatus = getQueyStatus();
    QString query = "(((" + acceso + " LIKE 'BAT%')"
            + " OR (`" + emplazamiento + "` LIKE '" + "BATERIA" + "')"
            + " OR (`" + emplazamiento + "` LIKE '" + "BATERÍA" + "')"
            + " OR (`" + emplazamiento + "` LIKE '" + "BAT" + "')"
            + " OR (`" + emplazamiento + "` LIKE '" + "BA-" + "%')"
            + " OR (`" + emplazamiento + "` LIKE '" + "BT-" + "%')"
            + ") AND (" + queryStatus + "))";
    getTareasCustomQuery(query);
    jsonArrayInTableFiltered = jsonArrayAll;

    filter_type = 0;

    jsonArrayInTableFiltered = ordenarPorBateria(jsonArrayInTableFiltered);
    fixModelforTable(jsonArrayInTableFiltered);
    setTableView();
    hide_loading();
}

bool Tabla::checkIfBatteryTask(QJsonObject jsonObject){
    QString access = jsonObject.value(acceso).toString().trimmed();
    QString ubicacion = jsonObject.value(emplazamiento).toString().trimmed().toUpper();
    if(access.contains("BAT") || ubicacion == "BAT" || ubicacion.contains("BA-") || ubicacion.contains("BT-")
            || ubicacion.contains("BATERIA") || ubicacion.contains("BATERÍA")){
        return true;
    }
    return false;
}
void Tabla::on_actionC_Geolocalizaci_n_triggered()
{
    show_loading("Cargando C.Emplazamientos...");
    hideAllFilters();

    filter_type |= F_GEOLOCALIZACION;
    ui->l_tipo_filtro->setText("C.Emplazamiento:");

    filter_column_field_selected = codigo_de_geolocalizacion;

    QString queryStatus = getQueyStatus();
    QString query = " (" + queryStatus +  ") ";
    bool res = getTareasValuesFieldCustomQueryServer(
                empresa, filter_column_field_selected, query);
    if(res){
        fillValuesInLineEditToFilter();
    }

    ui->widget_filtro_lineEdit->show();
    ui->widget_filtros->show();
    hide_loading();
}
void Tabla::on_actionTitular_triggered()
{
    show_loading("Cargando Nombres...");
    hideAllFilters();

    filter_type |= F_TITULAR;
    ui->l_tipo_filtro->setText("Nombre:");

    filter_column_field_selected = nombre_cliente;

    QString queryStatus = getQueyStatus();
    QString query = " (" + queryStatus +  ") ";
    bool res = getTareasValuesFieldCustomQueryServer(
                empresa, filter_column_field_selected, query);
    if(res){
        fillValuesInLineEditToFilter();
    }

    ui->widget_filtro_lineEdit->show();
    ui->widget_filtros->show();

    hide_loading();
}
void Tabla::on_actionN_Abonado_triggered()
{
    show_loading("Cargando Abonados...");
    hideAllFilters();

    filter_type |= F_N_ABONADO;
    ui->l_tipo_filtro->setText("Nº Abonado:");
    filter_column_field_selected = numero_abonado;

    QString queryStatus = getQueyStatus();
    QString query = " (" + queryStatus +  ") ";
    bool res = getTareasValuesFieldCustomQueryServer(
                empresa, filter_column_field_selected, query);
    if(res){
        fillValuesInLineEditToFilter();
    }

    ui->widget_filtro_lineEdit->show();
    ui->widget_filtros->show();
    hide_loading();
}
void Tabla::on_actionZona_triggered()
{
    show_loading("Cargando Sectors P...");

    hideAllFilters();
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    filter_type |= F_ZONA;
    ui->l_tipo_filtro->setText("Sector P:");

    filter_column_field_selected = zona;
    QString queryStatus = getQueyStatus();
    QString query = " (" + queryStatus +  ") ";
    bool res = getTareasValuesFieldCustomQueryServer(
                empresa, filter_column_field_selected, query);
    if(res){
        fillValuesInLineEditToFilter();
    }

    ui->widget_filtro_lineEdit->show();
    ui->widget_filtros->show();

    hide_loading();
}

void Tabla::on_actionN_Serie_triggered()
{
    show_loading("Cargando Series...");
    hideAllFilters();
    filter_type |= F_SERIE;
    ui->l_tipo_filtro->setText("Nº Serie:");

    filter_column_field_selected = numero_serie_contador;
    QString queryStatus = getQueyStatus();
    QString query = " (" + queryStatus +  ") ";
    bool res = getTareasValuesFieldCustomQueryServer(
                empresa, filter_column_field_selected, query);
    if(res){
        fillValuesInLineEditToFilter();
    }

    ui->widget_filtro_lineEdit->show();
    ui->widget_filtros->show();

    hide_loading();
}

void Tabla::on_actionPor_Resultado_triggered(){

    show_loading("Cargando Resultados...");
    hideAllFilters();
    filter_type |= F_RESULTADO;
    ui->l_tipo_filtro->setText("Resultado:");

    filter_column_field_selected = resultado;
    QString queryStatus = getQueyStatus();
    QString query = " (" + queryStatus +  ") ";
    bool res = getTareasValuesFieldCustomQueryServer(
                empresa, filter_column_field_selected, query);
    if(res){
        fillValuesInLineEditToFilter();
    }

    ui->widget_filtro_lineEdit->show();
    ui->widget_filtros->show();
    hide_loading();
}

void Tabla::on_actionContadores_triggered()
{
    if(other_task_screen::conexion_activa){
        emit abrirTablaContadores(true);
    }else{
        QMessageBox::information(this,"No hay conexión con la BD","No se puede abrir la tabla de contadores sin conexión con la BD del servidor.");
    }
}

void Tabla::on_actionOperarios_triggered()
{
    if(other_task_screen::conexion_activa){
        emit abrirTablaOperarios();
    }else{
        QMessageBox::information(this,"No hay conexión con la BD","No se puede abrir la tabla de operarios sin conexión con la BD del servidor.");
    }
}

void Tabla::on_actionNueva_Tarea_triggered()
{
    //    emit crearNuevaTarea();
    QJsonObject o;
    o.insert(status_tarea, state_abierta);
    other_task_screen *oneTareaScreen = new other_task_screen(nullptr, false, true, empresa);
    connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
    emit sendData(o);
    disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
    oneTareaScreen->populateView(false);
    connect(oneTareaScreen, SIGNAL(task_upload_excecution_result(int)),this, SLOT(updateTableWithServerInfo(int)));
    connect(oneTareaScreen, SIGNAL(tarea_revisada(QString)),this, SLOT(on_tarea_revisada(QString)));
    connect(oneTareaScreen, &other_task_screen::updateITACs,this, &Tabla::updateITACsServerInfo);
    oneTareaScreen->toogleEdit(true, true);
    oneTareaScreen->setAttribute(Qt::WA_DeleteOnClose);
    //    QRect rect = QGuiApplication::screens().first()->geometry();
    //    if(rect.width() <= 1366
    //            && rect.height() <= 768){
    oneTareaScreen->showMaximized();
    //    }else {
    //        oneTareaScreen->show();
    //    }
}
void Tabla::updateITACsServerInfo(){
    ui->statusbar->showMessage("Actualizando ITACs",3000);
    emit updateITACsInfo();
}
void Tabla::updateTableWithServerInfo(int result){
    Q_UNUSED(result);
    ui->statusbar->showMessage("Actualizando informacion",3000);
    emit updateTableInfo();
}
void Tabla::on_tarea_revisada(QString princ_var){
    Q_UNUSED(princ_var);
    if(filtering && filter_type == F_SIN_REVISAR){
        filtrarPorCambiosSinRevisar(last_id_start);
    }else{
        emit updateTableInfo();
    }
}
void Tabla::on_tableView_doubleClicked(const QModelIndex &index)
{
    lastSelectedRow = index.row();
    abrirTareaX(lastSelectedRow);
}

void Tabla::openTareaX(QString numin){
    QJsonObject o;

    GlobalFunctions gf(this, empresa);
    o = gf.getTareaFromServer(numero_interno, numin);

    bool sin_revisar = false;
    QString ultima_mod = o.value(ultima_modificacion).toString().trimmed();
    if(ultima_mod.contains("ANDROID")){
        sin_revisar = true;
    }
    other_task_screen *oneTareaScreen = new other_task_screen(nullptr, sin_revisar, true, empresa);

    connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
    emit sendData(o);
    disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));

    if(ui->pb_logotipo->pixmap()){
        oneTareaScreen->setLogoType(*ui->pb_logotipo->pixmap());
    }else{
        oneTareaScreen->getLogoType();
    }

    connect(oneTareaScreen, SIGNAL(task_upload_excecution_result(int)),this, SLOT(updateTableWithServerInfo(int)));
    connect(oneTareaScreen, SIGNAL(tarea_revisada(QString)),this, SLOT(on_tarea_revisada(QString)));
    connect(oneTareaScreen, &other_task_screen::updateITACs,this, &Tabla::updateITACsServerInfo);
    //    connect(oneTareaScreen, &other_task_screen::closing,oneTareaScreen, &other_task_screen::deleteLater);

    oneTareaScreen->setAttribute(Qt::WA_DeleteOnClose);
    //    QRect rect = QGuiApplication::screens().first()->geometry();
    //    if(rect.width() <= 1366
    //            && rect.height() <= 768){
    oneTareaScreen->showMaximized();
    //    }else {
    //        oneTareaScreen->show();
    //    }
    if(oneTareaScreen->populateView()){
        oneTareaScreen->createAutoPDF(false);
    }
}
void Tabla::abrirTareaX(int index){
    QJsonObject o;
    if(filtering){
        o = jsonArrayInTableFiltered.at(index).toObject();
    }else{
        o = jsonArrayInTable.at(index).toObject();
    }
    bool sin_revisar = false;
    QString ultima_mod = o.value(ultima_modificacion).toString().trimmed();
    if(ultima_mod.contains("ANDROID")){
        sin_revisar = true;
    }

    other_task_screen *oneTareaScreen = new other_task_screen(nullptr, sin_revisar, true, empresa);

    connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
    emit sendData(o);
    disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));

    if(ui->pb_logotipo->pixmap()){
        oneTareaScreen->setLogoType(*ui->pb_logotipo->pixmap());
    }else{
        oneTareaScreen->getLogoType();
    }
    connect(oneTareaScreen, SIGNAL(task_upload_excecution_result(int)),this, SLOT(updateTableWithServerInfo(int)));
    connect(oneTareaScreen, SIGNAL(tarea_revisada(QString)),this, SLOT(on_tarea_revisada(QString)));
    connect(oneTareaScreen, &other_task_screen::updateITACs,this, &Tabla::updateITACsServerInfo);
    //    connect(oneTareaScreen, &other_task_screen::closing, oneTareaScreen, &other_task_screen::deleteLater);

    oneTareaScreen->setAttribute(Qt::WA_DeleteOnClose);
    //    QRect rect = QGuiApplication::screens().first()->geometry();
    //    if(rect.width() <= 1366
    //            && rect.height() <= 768){
    oneTareaScreen->showMaximized();
    //    }else {
    //        oneTareaScreen->show();
    //    }
    if(oneTareaScreen->populateView()){
        oneTareaScreen->createAutoPDF(false);
    }
}
void Tabla::getContadoresToCompleter(){
    emit abrirTablaContadores(false);
}

void Tabla::on_actionExcel_4_triggered()
{
    emit importarExcel("DIARIAS");
}
void Tabla::on_actionFichero_DAT_triggered()
{
    emit importarDAT("DIARIAS");
}

void Tabla::on_actionExcel_3_triggered()
{
    emit importarExcel("MASIVAS");
}
void Tabla::on_actionFichero_DAT_2_triggered()
{
    emit importarDAT("MASIVAS");
}

void Tabla::on_actionExcel_5_triggered()
{
    emit importarExcel("ESPECIALES");
}
void Tabla::on_actionFichero_DAT_3_triggered()
{
    emit importarDAT("ESPECIALES");
}
void Tabla::getFileDownloadDir(QString dir)
{
    file_download_dir_string  = dir;
}
void Tabla::getFileDirSelected(QString dir)
{
    file_download_dir_selected  = dir;
}

void Tabla::on_actionDesdde_Fichero_TXT_triggered()
{
    emit importarTXT("buscar_fichero_en_PC");
}

QString Tabla::crearFicheroTxtConTabla(QJsonArray jsonArray,QString ruta_y_nombre_file){
    QJsonDocument doc;
    doc.setArray(jsonArray);
    QString filename = ruta_y_nombre_file;
    if(filename.contains(".xlsx")){
        filename.replace(".xlsx", ".txt"); //cambiando formato si es necesario
    }
    QByteArray byteArray = doc.toJson();
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        file.write(byteArray);
        file.close();
    }
    return ruta_y_nombre_file;
}
QString Tabla::crearFicheroDATConTabla(QJsonArray jsonArray,QString ruta_y_nombre_file){
    QJsonDocument doc;
    doc.setArray(jsonArray);
    QString filename = ruta_y_nombre_file;
    if(filename.contains(".xlsx")){
        filename.replace(".xlsx", ".dat"); //cambiando formato si es necesario
    }
    if(filename.contains(".txt")){
        filename.replace(".txt", ".dat"); //cambiando formato si es necesario
    }
    if(!filename.contains(".dat")){
        filename+=".dat";
    }
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly)){
        file.seek(0);
        QDataStream out(&file);
        out<<jsonArray;
        file.close();
    }
    return filename;
}

QJsonArray Tabla::getCurrentJsonArrayInTable(){
    QJsonArray jsonArray;

    if(filtering){
        jsonArray = jsonArrayInTableFiltered;
    }else{
        jsonArray = jsonArrayInTable;
    }
    return jsonArray;
}

void Tabla::show_loading(QString mess){
    emit hidingLoading();

    QWidget *widget_blur = new QWidget(this);
    widget_blur->setFixedSize(this->size()+QSize(0,30));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &Tabla::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &Tabla::hidingLoading, widget_blur, &QWidget::deleteLater);

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
    connect(this, &Tabla::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &Tabla::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &Tabla::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &Tabla::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &Tabla::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}
void Tabla::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void Tabla::hide_loading(){
    emit hidingLoading();
}

void Tabla::upload_save_work_file_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::SAVE_WORK,keys,values);
}
void Tabla::send_client_work_file_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::SEND_CLIENT_WORK,keys,values);
}
bool Tabla::sendClientWorkFileToServer(QString filename){

    QStringList keys, values;

    keys << "filename"  << "empresa" << "gestor";
    values << filename << empresa << GlobalFunctions::readGestorSelected();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, SLOT(send_client_work_file_request()));

    bool retorno = false;
    int res = q->exec();
    switch(res)
    {
    case database_comunication::script_result::timeout:
        break;
    case database_comunication::script_result::upload_file_ok:
        retorno = true;
        break;
    case database_comunication::script_result::upload_file_failed:
        break;
    }
    //    delete timer;
    delete q;
    return retorno;
}
bool Tabla::uploadFileToServer(QString filename){

    QStringList keys, values;

    keys << "filename"  << "empresa";
    values << filename << empresa;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, SLOT(upload_save_work_file_request()));

    bool retorno = false;
    int res = q->exec();
    switch(res)
    {
    case database_comunication::script_result::timeout:
        break;
    case database_comunication::script_result::upload_file_ok:
        retorno = true;
        break;
    case database_comunication::script_result::upload_file_failed:
        break;
    }
    //    delete timer;
    delete q;
    return retorno;
}

void Tabla::salvarTrabajoEnServidor(){
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QDir dir;
    dir.setPath(QDir::currentPath() + "/Trabajo Exportado");
    if(!dir.exists()){
        dir.mkpath(QDir::currentPath() + "/Trabajo Exportado");
    }
    QString filename = dir.path()+"/Trabajo.txt";
    crearFicheroTxtConTabla(jsonArray, filename);
    crearFicheroDATConTabla(jsonArray, filename);

    uploadFileToServer(filename);
    QString saveFilename = dir.path()+"/Trabajo_Respaldado_"+ QDateTime::currentDateTime().toString(formato_fecha_hora_for_filename)+".txt";
    QFile::copy(filename, saveFilename);
    if(uploadFileToServer(saveFilename)){
        GlobalFunctions::showMessage(this,"Éxito","El trabajo fue salvado y subido");
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
    }
}

void Tabla::on_actionSubir_Trabajo_a_Servidor_triggered()
{
    salvarTrabajoEnServidor();
}
void Tabla::getFileDirSelectedAndDownload(QString file_download_dir_selected){

    if(file_download_dir_selected.toLower() =="ninguno"){
        return;
    }

    this->file_download_dir_selected = file_download_dir_selected;

    QEventLoop *q = new QEventLoop();

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QStringList keys, values;
    keys << "nombre" << "empresa";
    values << file_download_dir_selected << empresa;

    this->keys = keys;
    this->values = values;

    QTimer::singleShot(DELAY, this, SLOT(download_save_work_file_request()));

    int res = q->exec();
    switch(res)
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        break;
    case database_comunication::script_result::download_file_ok:
        break;
    case database_comunication::script_result::download_file_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de tranferencia con el servidor.");
        break;
    case database_comunication::script_result::download_file_doesnt_exists:
        GlobalFunctions::showWarning(this,"Error darchivo no encontrado","No se pudo completar la solucitud porque el archivo no existe el servidor.");
        break;
    }
    //    delete timer;
    delete q;
    emit importarTXT(file_download_dir_string);
}
void Tabla::on_actionCargar_Trabajo_Salvado_triggered()
{
    Select_File_to_Download *selection_file_window = new Select_File_to_Download(this, "", empresa);
    selection_file_window->getFilesWorkFromServer();
    connect(selection_file_window,SIGNAL(work_Selected_file(QString)),this,SLOT(getFileDirSelectedAndDownload(QString)));
    selection_file_window->exec();
}

void Tabla::on_actionEspeciales_triggered()
{
    selection_Order = "ESPECIALES";
    informarTareas("ESPECIALES");
}

void Tabla::on_actionMasivas_triggered()
{
    selection_Order = "MASIVAS";
    informarTareas("MASIVAS");
}

void Tabla::on_actionDiarias_triggered()
{
    selection_Order = "DIARIAS";
    informarTareas("DIARIAS");
}

QString Tabla::get_date_from_status(QJsonObject object, QString status)
{
    if(status=="IDLE"){
        return object.value(FechImportacion).toString();
    }
    if(status=="AUSENTE"){
        QString fecha = object.value(fechas_tocado_puerta).toString();
        if(!other_task_screen::checkIfFieldIsValid(fecha)){
            fecha = object.value(fechas_nota_aviso).toString();
            if(!other_task_screen::checkIfFieldIsValid(fecha)){
                fecha = object.value(date_time_modified).toString();
            }
        }
        return fecha;
    }
    if(status=="CITA"){
        return object.value(fecha_hora_cita).toString();
    }
    if(status=="DONE"){
        return object.value(F_INST).toString();
    }
    if(status=="CLOSED"){
        return object.value(FECH_CIERRE).toString();
    }
    if(status=="INFORMADA"){
        return object.value(fech_informacionnew).toString();
    }
    if(status=="REQUERIDA"){
        return object.value(F_INST).toString();
    }
    if(status=="MODIFICACION"){
        return object.value(date_time_modified).toString();
    }
    return "";
}

QString Tabla::get_current_date_in_format(QDate date, QTime time, QTime time_end)
{
    QString day_week = date.toString("ddd");
    day_week = map_days_week.value(day_week);
    QString day = QString::number(date.day());
    QString month = map_months.value(date.month());
    QString year = QString::number(date.year());

    int h = time.hour();
    QString hour = QString::number(h);
    if(h < 10){
        hour = "0"+hour;
    }
    int t = time.minute();
    QString minutes = QString::number(t);
    if(t < 10){
        minutes = "0"+minutes;
    }
    QString time_string = hour + ":"+minutes;


    h = time_end.hour();
    QString hour_end = QString::number(h);
    if(hour_end.toInt() < 10){
        hour_end = "0"+hour_end;
    }
    t = time_end.minute();
    QString minutes_end = QString::number(t);
    if( t < 10){
        minutes_end = "0"+minutes_end;
    }
    QString time_string_end = hour_end + ":"+minutes_end;

    return day_week + ", " + day + " de "+ month +" de "+ year +"\n"+"Entre las "+ time_string + " y las "+ time_string_end;
}
QJsonObject Tabla::set_date_from_status(QJsonObject object, QString status, QString date, QTime time)
{
    if(status.contains("IDLE") && !status.contains("CITA")){
        object.insert(FechImportacion, date);
    }
    if(status=="CITA"){
        object.insert(nuevo_citas, get_current_date_in_format(QDate::fromString(date, formato_fecha_hora), time, time));
        object.insert(fecha_hora_cita, date);
    }
    if(status=="DONE"){
        object.insert(F_INST, date);
    }
    if(status=="CLOSED"){
        object.insert(FECH_CIERRE, date);
    }
    if(status=="INFORMADA"){
        object.insert(fech_informacionnew, date);
    }
    if(status=="REQUERIDA"){
        object.insert(F_INST, date);
    }
    return object;
}

void Tabla::get_date_selected(QDate d)
{
    selection_date = d;
}

void Tabla::informarTareas(QString order)
{
    if(other_task_screen::conexion_activa){
        QJsonArray  jsonArray_to_Inform;
        QJsonArray jsonArray;
        jsonArray = getCurrentJsonArrayInTable();

        CalendarDialog *calendarDialog = new CalendarDialog(nullptr, false, empresa);
        connect(calendarDialog,SIGNAL(date_selected(QDate)),this,SLOT(get_date_selected(QDate)));

        if(calendarDialog->exec()){

            Info *info = new Info(nullptr, false, empresa);
            info->getInfoInServer();
            QJsonArray jsonArrayInfo = Info::readInfos();
            QJsonObject jsonObjectInfo = jsonArrayInfo.at(0).toObject();///info Json
            ultimoIDExportacion = jsonObjectInfo.value(lastIDExportacion_infos).toString().trimmed().toInt();
            ultimoIDSAT = jsonObjectInfo.value(lastIDSAT_infos).toString().trimmed().toInt();
            ultimoIDOrden = jsonObjectInfo.value(lastIDOrden_infos).toString().trimmed().toInt();

            other_task_screen *oneTareaScreen = new other_task_screen(nullptr, false, true, empresa);

            for(int i=0, reintentos = 0; i< jsonArray.size() /*max_iteration*/; i++)
            {
                if(jsonArray.size() > i /* posicion*/){
                    QJsonObject jsonObject = jsonArray[i].toObject();
                    QString tiporden = jsonObject.value(TIPORDEN).toString();
                    if( tiporden == order){

                        QString date_from_status = get_date_from_status(jsonObject, selected_status).left(10);

                        if(date_from_status == selection_date.toString("yyyy-MM-dd")){
                            QString idSat = jsonObject.value(ID_SAT).toString().trimmed();
                            if(!checkIfFieldIsValid(idSat)){
                                ultimoIDSAT++;
                                jsonObject.insert(ID_SAT, QString::number(ultimoIDSAT));
                            }
                            QString idOrden = jsonObject.value(idOrdenCABB).toString().trimmed();
                            if(!checkIfFieldIsValid(idOrden)){
                                ultimoIDOrden++;
                                jsonObject.insert(idOrdenCABB, QString::number(ultimoIDOrden));
                            }
                            jsonObject.insert(status_tarea, "INFORMADA");
                            int idexp = ultimoIDExportacion;
                            jsonObject = set_date_from_status(
                                        jsonObject, "INFORMADA",
                                        QDateTime::currentDateTime().toString(formato_fecha_hora));
                            jsonObject.insert(idexport, QString::number(idexp));

                            jsonArray_to_Inform.append(jsonObject);
                        }else{
                            continue;
                        }
                    }else{
                        continue;
                    }
                    qDebug() << jsonObject.value(TIPORDEN).toString();

                    if(oneTareaScreen != nullptr){
                        oneTareaScreen->clearTask();
                    }
                    oneTareaScreen->setShowMesageBox(false);

                    connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
                    emit sendData(jsonObject);
                    disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));

                    oneTareaScreen->populateView(false);

                    QEventLoop *q = new QEventLoop();

#if (USE_TIMEOUT == 1)
                    try{
                        QTimer::singleShot(TIMEOUT, this, SLOT(conection_timeout()));
                    }catch(QException e){
                        e.raise();
                        qDebug()<<"Excepcion del timer lanzada";
                    }

#endif
                    connect(oneTareaScreen, &other_task_screen::task_upload_excecution_result,q,&QEventLoop::exit);

                    oneTareaScreen->setShowMesageBox(false);

                    QTimer::singleShot(DELAY,oneTareaScreen,SLOT(on_pb_update_server_info_clicked()));

                    switch(q->exec())
                    {
                    case database_comunication::script_result::timeout:
                        i--;
                        reintentos++;
                        if(reintentos == RETRIES)
                        {
                            GlobalFunctions gf(this);
                            GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                            i = jsonArray.size();
                        }
                        break;
                    case database_comunication::script_result::task_to_server_ok:
                        reintentos = 0;
                        break;
                    case database_comunication::script_result::update_task_to_server_failed:
                        i--;
                        reintentos++;
                        if(reintentos == RETRIES)
                        {
                            GlobalFunctions gf(this);
                            GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                            i = jsonArray.size();
                        }
                        break;
                    case database_comunication::script_result::create_task_to_server_failed:
                        i--;
                        reintentos++;
                        if(reintentos == RETRIES)
                        {
                            GlobalFunctions gf(this);
                            GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                            i = jsonArray.size();
                        }
                        break;
                    }
                    disconnect(oneTareaScreen, &other_task_screen::task_upload_excecution_result,q,&QEventLoop::exit);
                    delete q;
                }
            }
            //        hide_loading();
            GlobalFunctions::showMessage(this,"Éxito","Las tareas han sido informadas");
            //        getAllInternalNumbers();

            exportExcelAndDat(jsonArray_to_Inform);

            ultimoIDExportacion++;
            jsonObjectInfo.insert(lastIDOrden_infos, QString::number(ultimoIDOrden));
            jsonObjectInfo.insert(lastIDSAT_infos, QString::number(ultimoIDSAT));
            jsonObjectInfo.insert(lastIDExportacion_infos, QString::number(ultimoIDExportacion));

            info->actualizarInfoInServer(jsonObjectInfo);

            emit updateTableInfo();

            oneTareaScreen->deleteLater();
        }
        disconnect(calendarDialog,SIGNAL(date_selected(QDate)),this,SLOT(get_date_selected(QDate)));
    }
    else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de conexión","No puede informar tareas sin conexión al servidor de la tabla");
    }
}
void Tabla::exportExcelAndDat(QJsonArray jsonArray){

    export_to_dat_IDExp(jsonArray);
    QString dir = export_to_excel_IDExp(jsonArray);
    QString numExp = composeNumExp(ultimoIDExportacion);
    QString name = crearFicheroDATConTabla(jsonArray, dir +"/GCT"+ numExp + "_EXTRA.dat");
    //    disconnect(this,SIGNAL(exportarExcelyDAT()),this,SLOT(exportExcelAndDat()));

    QFileDialog::getOpenFileName(this, "Aqui se ha guardado el informe", dir, tr("Images (*.dat *.xls *.xlsx)"));
}

QString Tabla::fill_with_spaces(QString str,int size, bool blank)
{
    QString temp = str;
    if(str.size() >= size)
        temp.truncate(size);
    else
    {
        if(str.trimmed().isEmpty()){
            blank = true;
        }
        while(temp.size() < size){
            if(blank){
                temp.append(" ");
            }else{
                temp.prepend("0");
            }
        }
    }
    return temp;
}
QString Tabla::composeNumExp(int num)
{
    QString numreturn = QString::number(num);

    while(numreturn.length() < 5){
        numreturn = "0"+numreturn;
    }
    return numreturn;
}
QString Tabla::setDirExpToExplorer(){

    //    QDate date = QDate::currentDate(); ////Antigua direccion de exportacion
    //    QString day, month, year;
    //    day = QString::number(date.day());
    //    if(date.day()<10){
    //        day = "0"+day;
    //    }
    //    month = map_months.value(date.month());
    //    year = QString::number(date.year());
    //    QDir dir;
    //    QString dir_string;
    //    if(selection_Order == "DIARIAS"){
    //        dir.setPath("C:/GecontaInstala/Informe/"+selection_Order+"/"+year+"/"+day+"_"+month+"_"+year);
    //        if(!dir.exists()){
    //            dir.setPath("C:/GecontaInstala/Informe/"+selection_Order+"/"+year+"/"+day+"_"+month+"_"+year);
    //        }
    //    }else{
    //        dir.setPath("C:/GecontaInstala/Informe/"+selection_Order+"/"+year+"/");
    //    }

    QDir dir = QDir::current();
    dir.setPath(dir.path() + "/Ficheros Informados");
    QString dir_string;

    if(dir.exists()){
        dir_string = dir.path();
    }else{
        if(dir.mkpath(dir.path())){
            dir_string = dir.path();
        }else{
            dir_string = QDir::current().path();
        }
    }
    qDebug()<<dir.path();

    return dir_string;
}
QString Tabla::setDirToExplorer(){

    QDate date = QDate::currentDate();
    QString day, month, year;
    day = QString::number(date.day());
    if(date.day()<10){
        day = "0"+day;
    }
    month = map_months.value(date.month());
    year = QString::number(date.year());
    QDir dir;
    QString dir_string;
    if(selection_Order == "DIARIAS"){
        dir.setPath("C:/GecontaInstala/Carga/"+selection_Order+"/"+year+"/"+day+"_"+month+"_"+year); //cambiar esta direccion más adelante
        if(!dir.exists()){
            dir.setPath("C:/GecontaInstala/Carga/"+selection_Order+"/"+year+"/");
        }
    }else{
        dir.setPath("C:/GecontaInstala/Carga/"+selection_Order+"/"+year+"/");
    }

    if(dir.exists()){
        dir_string = dir.path();
    }else{
        dir_string = QDir::current().path();
    }
    qDebug()<<dir.path();

    return dir_string;
}

void Tabla::export_to_dat_IDExp(QJsonArray jsonArray)
{
    QString numExp = composeNumExp(ultimoIDExportacion);

    QString rutaToDATFile = setDirExpToExplorer()+"/GCT"+ numExp + ".dat";
    if(!rutaToDATFile.isNull() && !rutaToDATFile.isEmpty())
    {
        //    QString output_filename = "GTC0....dat";
        QFile file(rutaToDATFile);
        if(file.open(QIODevice::WriteOnly))
        {
            //writing data
            QString stream;
            QString temp;
            for(int i = 0; i < jsonArray.size(); i++)
            {
                QString numSerie = jsonArray[i].toObject().value(numero_serie_contador_devuelto).toString();
                QString prefijo = jsonArray[i].toObject().value(CONTADOR_Prefijo_anno_devuelto).toString();
                if(!checkIfFieldIsValid(prefijo)){
                    prefijo = eliminarNumerosAlFinal(numSerie).trimmed();
                }
                if(!prefijo.isEmpty() &&
                        (numSerie.mid(0, prefijo.size()) == prefijo)){
                    numSerie = numSerie.remove(0, prefijo.size()).trimmed();
                }
                QString numSerieSinPrefijo = numSerie;
                //"Prefijo devuelto";--------------------------------------------------------------------------------------
                temp = prefijo;
                //            temp.truncate(6);
                temp = fill_with_spaces(temp,6);
                stream.append(temp);

                //"Nº CONTADOR  ";--------------------------------------------------------------------------------------
                temp = numSerieSinPrefijo.trimmed();
                //            temp.truncate(7);
                temp = fill_with_spaces(temp,7, false); //false es rellenar con 0, true con espacios
                stream.append(temp);

                //marca_contador--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(marca_devuelta).toString().trimmed(); //comprobar si solo pone codigo marca
                //            temp.truncate(3);
                temp = fill_with_spaces(temp,3);
                stream.append(temp);

                //CALIBRE--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(calibre_real).toString().trimmed();
                //            temp.truncate(3);
                temp = fill_with_spaces(temp,3, false);
                stream.append(temp);

                //Nº RUEDAS--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(RUEDASDV).toString();
                //            temp.truncate(1);
                temp = fill_with_spaces(temp,1);
                stream.append(temp);

                //CODIGO DE ABONADO--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(numero_abonado).toString().trimmed();
                //            temp.truncate(8);
                temp = fill_with_spaces(temp,8, false);
                stream.append(temp);

                //RESULTADO--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(resultado).toString();
                //            temp.truncate(3);
                temp = fill_with_spaces(temp,3);
                stream.append(temp);

                //FECHA instalacion nuevo OJO lo asocié con 'fecha_de_cambio'--------------------------------------------------------------------------------------
                QString f = jsonArray[i].toObject().value(F_INST).toString();
                if(f.isEmpty()){
                    f = QDate::currentDate().toString("yyyy-MM-dd");
                }
                //                f= "2020-01-07";
                f= f.left(10);
                temp = (f.left(10)).right(2) + ""+(f.left(7)).right(2) + ""+ f.left(4); //ddMMyyyy
                //            temp.truncate(8);
                temp = fill_with_spaces(temp,8);
                stream.append(temp);

                //LECTURA INSTALADO OJO lo asocié con 'lectura_actual'--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(lectura_contador_nuevo).toString().trimmed();
                //            temp.truncate(8);
                temp = fill_with_spaces(temp,8, false);
                stream.append(temp);

                //EMPLAZAMIENTO--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(emplazamiento_devuelto).toString();
                //            temp.truncate(2);
                temp = fill_with_spaces(temp,2);
                stream.append(temp);

                //RESTO DEVUELTO (RESTO_EM)--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(RESTO_EM).toString();
                //            temp.truncate(8);
                temp = fill_with_spaces(temp,8);
                stream.append(temp);

                //Nº INTERNO--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(numero_interno).toString();
                //            temp.truncate(12);
                temp = fill_with_spaces(temp,12);
                stream.append(temp);

                //LECTURA LEVANTADO  OJO lo asocié con 'lectura_ultima'--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(lectura_actual).toString().trimmed();
                //            temp.truncate(8);
                temp = fill_with_spaces(temp,8, false);
                stream.append(temp);

                //OBSERVACIONES OJO solo tiene 3 caracteres y el .dat de entrada utiliza 30 caracteres para este campo--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(observaciones_devueltas).toString();
                //            temp.truncate(3);
                temp = fill_with_spaces(temp,3);
                stream.append(temp);

                //CLASE DE CONTADOR--------------------------------------------------------------------------------------
                temp = jsonArray[i].toObject().value(TIPO_devuelto).toString();
                //            temp.truncate(1);
                temp = fill_with_spaces(temp,1);
                stream.append(temp);

                stream.append("\n");
            }
            file.write(stream.toUtf8());
            file.close();

            GlobalFunctions::showMessage(this,"Éxito","Fichero DAT de tareas generado");
        }
    }
}
QString Tabla::eliminarNumerosAlFinal(QString string){
    for(int n = string.size()-1; n >= 0; n--) {
        if(string.at(n).isDigit()){
            string.remove(n, 1);
        }else{
            break;
        }
    }
    return string;
}

QMap<QString,QString> Tabla::fillMapaExpCABB(){
    QMap<QString,QString> mapa_exp;

    mapa_exp.insert("IDCENTRO","3");
    mapa_exp.insert("IDEXPORT","1");
    mapa_exp.insert("IDSAT", ID_SAT);
    mapa_exp.insert("IDMODULO2",numero_serie_contador_devuelto);
    mapa_exp.insert("CODIGOCAUDAL",TIPO_devuelto);
    mapa_exp.insert("CALIBRE2",calibre_real);
    mapa_exp.insert("DIGITOS2",RUEDASDV);
    mapa_exp.insert("REFEXPORTABONADO",numero_abonado);
    mapa_exp.insert("CODIGORESULTADO",resultado);
    mapa_exp.insert("CODIGOMOTIVO",observaciones_devueltas);
    mapa_exp.insert("FECHASERVICIO",F_INST);
    mapa_exp.insert("LECTURA2",lectura_contador_nuevo);
    mapa_exp.insert("LECTURAVIEJO",lectura_actual);
    mapa_exp.insert("ADICIONALLECTORCONTADOR",emplazamiento_devuelto);
    mapa_exp.insert("CODIGOAGUAS",numero_interno);
    mapa_exp.insert("CODIGOMARCA",marca_devuelta);
    mapa_exp.insert("FECHABAJA",F_INST);
    mapa_exp.insert("CODIGOAVERIA",ANOMALIA);
    mapa_exp.insert("OBSINTERNAS",MENSAJE_LIBRE);
    mapa_exp.insert("CODIGOAVERIAFINAL",AREALIZAR_devuelta);
    mapa_exp.insert("EMPLAZAMIENTONUEVO",emplazamiento_devuelto);
    mapa_exp.insert("TIPO FLUIDO",TIPOFLUIDO_devuelto);
    mapa_exp.insert("TIPO DE RADIO",tipoRadio_devuelto);
    mapa_exp.insert("GEOLOCALIZACION",geolocalizacion);

    return mapa_exp;
}

QMap<QString,QString> Tabla::fillMapaExp(){
    QMap<QString,QString> mapa_exportacion;
    mapa_exportacion.insert("Población",poblacion);
    mapa_exportacion.insert("CALLE",calle);
    mapa_exportacion.insert("Nº",numero);
    mapa_exportacion.insert("BIS",BIS);
    mapa_exportacion.insert("PISO",piso);
    mapa_exportacion.insert("MANO",mano);
    mapa_exportacion.insert("AÑO O PREFIJO CONT. RETIRADO",CONTADOR_Prefijo_anno);
    mapa_exportacion.insert("Nº SERIE CONT. RETIRADO",numero_serie_contador);
    mapa_exportacion.insert("CALIBRE CONT. RETIRADO",calibre_toma);
    //    mapa_exportacion.insert("CALIBRE. INST.",calibre_real);
    mapa_exportacion.insert("OPERARIO",operario);
    mapa_exportacion.insert("Anomalía (TAREA A REALIZAR)",AREALIZAR_devuelta);
    mapa_exportacion.insert("EMPLAZAMIENTO",emplazamiento_devuelto);
    mapa_exportacion.insert("UBICACIÓN BATERÍA",ubicacion_en_bateria);
    mapa_exportacion.insert("OBSERVACIONES",MENSAJE_LIBRE);
    mapa_exportacion.insert("ACTIVIDAD",actividad);
    mapa_exportacion.insert("TITULAR",nombre_cliente);
    mapa_exportacion.insert("NºABONADO",numero_abonado);
    mapa_exportacion.insert("TELEFONO",telefono1);
    mapa_exportacion.insert("ACCESO",acceso);
    mapa_exportacion.insert("RESULTADO",resultado);
    mapa_exportacion.insert("NUEVO",nuevo_citas);
    mapa_exportacion.insert("FECHA",F_INST);
    mapa_exportacion.insert("ZONAS",zona);
    mapa_exportacion.insert("RUTA",ruta);
    mapa_exportacion.insert("MARCA",marca_contador);
    mapa_exportacion.insert("ÚLTIMA LECTURA",lectura_actual);
    mapa_exportacion.insert("GESTOR",GESTOR);
    mapa_exportacion.insert("LECTURA DE CONTADOR INSTALADO",lectura_contador_nuevo);
    mapa_exportacion.insert("Nº SERIE CONTADOR INSTALADO",numero_serie_contador_devuelto);
    mapa_exportacion.insert("Nº ANTENA CONTADOR INSTALADO",numero_serie_modulo);
    mapa_exportacion.insert("CALIBRE CONTADOR INSTALADO",calibre_real);
    mapa_exportacion.insert("MARCA CONTADOR INSTALADO",marca_devuelta);
    mapa_exportacion.insert("CLASE CONTADOR INSTALADO",TIPO_devuelto);
    mapa_exportacion.insert("LONGITUD CONTADOR INSTALADO",largo_devuelto);
    mapa_exportacion.insert("CÓDIGO DE LOCALIZACIÓN",codigo_de_geolocalizacion);
    mapa_exportacion.insert("LINK GEOLOCALIZACIÓN",url_geolocalizacion);
    mapa_exportacion.insert("GEOLOCALIZACION",geolocalizacion);
    return mapa_exportacion;
}

QMap<QString,QString> Tabla::fillMapaExpAlternativo(){
    QMap<QString,QString> mapa_exportacion_alternativo;
    mapa_exportacion_alternativo.insert("Población",poblacion);
    mapa_exportacion_alternativo.insert("CALLE",calle);
    mapa_exportacion_alternativo.insert("Nº",numero);
    mapa_exportacion_alternativo.insert("BIS",BIS);
    mapa_exportacion_alternativo.insert("PISO",piso);
    mapa_exportacion_alternativo.insert("MANO",mano);
    mapa_exportacion_alternativo.insert("AÑO O PREFIJO CONT. RETIRADO",CONTADOR_Prefijo_anno);
    mapa_exportacion_alternativo.insert("Nº SERIE CONT. RETIRADO",numero_serie_contador);
    mapa_exportacion_alternativo.insert("CALIBRE CONT. RETIRADO",calibre_toma);
    //    mapa_exportacion_alternativo.insert("CALIBRE. INST.",calibre_real);
    mapa_exportacion_alternativo.insert("OPERARIO",operario);
    mapa_exportacion_alternativo.insert("Anomalía (TAREA A REALIZAR)",ANOMALIA);
    mapa_exportacion_alternativo.insert("EMPLAZAMIENTO",emplazamiento);
    mapa_exportacion_alternativo.insert("UBICACIÓN BATERÍA",ubicacion_en_bateria);
    mapa_exportacion_alternativo.insert("OBSERVACIONES",observaciones);
    mapa_exportacion_alternativo.insert("ACTIVIDAD",actividad);
    mapa_exportacion_alternativo.insert("TITULAR",nombre_cliente);
    mapa_exportacion_alternativo.insert("NºABONADO",numero_abonado);
    mapa_exportacion_alternativo.insert("TELEFONO",telefono1);
    mapa_exportacion_alternativo.insert("ACCESO",acceso);
    mapa_exportacion_alternativo.insert("RESULTADO",resultado);
    mapa_exportacion_alternativo.insert("NUEVO",nuevo_citas);
    mapa_exportacion_alternativo.insert("FECHA",F_INST);
    mapa_exportacion_alternativo.insert("ZONAS",zona);
    mapa_exportacion_alternativo.insert("RUTA",ruta);
    mapa_exportacion_alternativo.insert("MARCA",marca_contador);
    mapa_exportacion_alternativo.insert("ÚLTIMA LECTURA",lectura_ultima);
    mapa_exportacion_alternativo.insert("GESTOR",GESTOR);
    mapa_exportacion_alternativo.insert("LECTURA DE CONTADOR INSTALADO",lectura_contador_nuevo);
    mapa_exportacion_alternativo.insert("Nº SERIE CONTADOR INSTALADO",numero_serie_contador_devuelto);
    mapa_exportacion_alternativo.insert("Nº ANTENA CONTADOR INSTALADO",numero_serie_modulo);
    mapa_exportacion_alternativo.insert("CALIBRE CONTADOR INSTALADO",calibre_real);
    mapa_exportacion_alternativo.insert("MARCA CONTADOR INSTALADO",marca_devuelta);
    mapa_exportacion_alternativo.insert("CLASE CONTADOR INSTALADO",TIPO_devuelto);
    mapa_exportacion_alternativo.insert("LONGITUD CONTADOR INSTALADO",largo_devuelto);
    mapa_exportacion_alternativo.insert("CÓDIGO DE LOCALIZACIÓN",codigo_de_geolocalizacion);
    mapa_exportacion_alternativo.insert("LINK GEOLOCALIZACIÓN",url_geolocalizacion);
    mapa_exportacion_alternativo.insert("GEOLOCALIZACION",geolocalizacion);
    return mapa_exportacion_alternativo;
}

QString Tabla::fillSheetExcepciones(QJsonArray jsonArray, QXlsx::Document &xlsx)
{
    QMap<QString,QString> mapa_exp = fillMapaExpCABB();
    QStringList listHeaders;
    listHeaders << "IDCENTRO"  << "IDEXPORT"  << "IDSAT"  << "IDMODULO2"  << "CODIGOCAUDAL"
                << "CALIBRE2"  << "DIGITOS2"  << "REFEXPORTABONADO"  << "CODIGORESULTADO"
                << "CODIGOMOTIVO"  << "FECHASERVICIO"  << "LECTURA2"  << "LECTURAVIEJO"
                << "ADICIONALLECTORCONTADOR"  << "CODIGOAGUAS"  << "CODIGOMARCA"<< "FECHABAJA"
                << "CODIGOAVERIA"  << "OBSINTERNAS"<< "CODIGOAVERIAFINAL"<<" GEOLOCALIZACION";

    int rows = jsonArray.count();

    xlsx.selectSheet("EXCEPCIONES");
    //write headers
    for (int i=0; i<listHeaders.count(); i++)
    {
        Format f;
        xlsx.write(1, i+1, listHeaders[i]/*.toUpper()*/);

    }
    //PAra gestor Geconta
    //write data
    QString temp;

    for(int i = 0; i < rows; i++)
    {
        QString anomalia = jsonArray[i].toObject().value(ANOMALIA).toString().trimmed();
        QString arealizar_dev = jsonArray[i].toObject().value(AREALIZAR_devuelta).toString().trimmed();

        if(arealizar_dev.contains("-")){
            arealizar_dev = arealizar_dev.split("-").at(0).trimmed();
        }
        if(anomalia != arealizar_dev){
            for(int n=0; n < listHeaders.count(); n++){
                if(n>2){
                    QString value_header = mapa_exp.value(listHeaders.at(n));
                    QString value = jsonArray[i].toObject().value(mapa_exp.value(listHeaders.at(n))).toString();
                    if(value.contains(" - ")){
                        value = value.split(" - ").at(0).trimmed();
                    }
                    temp = value;

                    if(value_header == observaciones_devueltas){
                        if(checkIfFieldIsValid(temp)){
                            QStringList list;
                            list = temp.split("\n");
                            temp = "";
                            for (int c=0; c < list.size(); c++) {
                                temp+= list.at(c).split("-").at(0).trimmed()+".";
                            }
                            temp = temp.trimmed();
                            temp.remove(temp.size()-1,1);
                        }
                    }
                    if(listHeaders.at(n) == "FECHABAJA"){
                        if(!arealizar_dev.contains("036") && !arealizar_dev.contains("037") && !arealizar_dev.contains("039")){
                            temp = "";
                        }
                    }
                }
                else if(n+1==2){
                    temp = QString::number(ultimoIDExportacion);
                }
                else {
                    temp = mapa_exp.value(listHeaders.at(n));
                }
                //                temp.chop(7);
                if(n+1==3){
                    temp = jsonArray[i].toObject().value(ID_SAT).toString().trimmed();

                }
                xlsx.write(i+2,n+1,temp);
            }
        }

    }
    return "";
}

QString Tabla::fillSheetCausas(QJsonArray jsonArray, QXlsx::Document &xlsx)
{
    Q_UNUSED(jsonArray);

    QStringList listHeaders;
    listHeaders << "CAUSA"  << "DESCRIPCIONCAUSA"  << "TOTALCAUSAS";

    xlsx.selectSheet("CAUSAS");
    //write headers
    for (int i=0; i<listHeaders.count(); i++)
    {
        Format f;
        xlsx.write(1, i+1, listHeaders[i]/*.toUpper()*/);

    }
    return "";
}

QString Tabla::fillSheetCalibres(QJsonArray jsonArray, QXlsx::Document &xlsx)
{
    QMap<QString,QString> mapa_exp;
    mapa_exp.insert("ABONADO",numero_abonado);
    mapa_exp.insert("NINTER",numero_interno);
    mapa_exp.insert("POBLACION",poblacion);
    mapa_exp.insert("CAL_VIEJO",calibre_toma);
    mapa_exp.insert("CAL_NUEVO",calibre_real);

    QStringList listHeaders;
    listHeaders << "IDEXPORT"  << "FECHASERVICIO"  << "ABONADO"  << "NINTER"
                << "DIRECCION"  << "POBLACION"  << "CAL_VIEJO" << "CAL_NUEVO";

    xlsx.selectSheet("CALIBRES");
    //write headers
    for (int i=0; i<listHeaders.count(); i++)
    {
        Format f;
        xlsx.write(1, i+1, listHeaders[i]/*.toUpper()*/);

    }
    //PAra gestor Geconta
    //write data
    QString temp = "";

    for(int i = 0; i < jsonArray.size(); i++)
    {
        QString cal_t = jsonArray[i].toObject().value(calibre_toma).toString().trimmed();
        QString cal_r = jsonArray[i].toObject().value(calibre_real).toString().trimmed();

        if(cal_t != cal_r){
            for(int n=0; n < listHeaders.count(); n++){
                if(n>1){
                    QString header = listHeaders.at(n);
                    if(header == "DIRECCION"){
                        QString calle_l, numero_l, bis_l, piso_l, mano_l;
                        calle_l = jsonArray[i].toObject().value(calle).toString().trimmed();
                        numero_l = jsonArray[i].toObject().value(numero).toString().trimmed();
                        bis_l = jsonArray[i].toObject().value(BIS).toString().trimmed();
                        piso_l = jsonArray[i].toObject().value(piso).toString().trimmed();
                        mano_l = jsonArray[i].toObject().value(mano).toString().trimmed();

                        temp = calle_l + "  " + numero_l + "  " + bis_l + "  " + piso_l + " " + mano_l;

                    }else {
                        temp = jsonArray[i].toObject().value(mapa_exp.value(listHeaders.at(n))).toString();
                    }

                }
                else if(n==0){
                    temp = QString::number(ultimoIDExportacion);
                }
                else if(n==1){
                    QString fecha_servicio = jsonArray[i].toObject().value(F_INST).toString().trimmed();
                    QDateTime date = QDateTime::fromString(fecha_servicio, formato_fecha_hora);
                    temp = date.toString(formato_fecha_hora_new_view);
                }
                xlsx.write(i + 2,n+1,temp);
                temp = "";
            }
        }
    }
    return "";
}

QString Tabla::fillSheetResumenServicios(QJsonArray jsonArray, QXlsx::Document &xlsx)
{
    QStringList listHeaders;
    listHeaders << "IDEXPORT"  << "FECHA"
                << "MONTAJE13_20"  << "MONTAJE25_40" << "MONTAJE50_100"  << "MONTAJE125_200"
                << "DESMONTAJE13_20"  << "DESMONTAJE25_40" << "DESMONTAJE50_100" << "DESMONTAJE125_200"
                << "ALMACEN13_20 " << "ALMACEN25_40" << "ALMACEN50_100" << "ALMACEN125_200" << "ALMACEN_MODULO"
                << "TOMADATOS" << "TOMADATOSRADIO"
                << "EMISOR PROPIO REINSTALADO/RECONFIG"
                << "EMISORA BAJA REINSTALADO/RECONFIG"
                << "EMISORA NUEVA CONFIG"
                << "DESPRECINTADO 13-20"<< "DESPRECINTADO 25-40" << "DESPRECINTADO 50-100" << "DESPRECINTADO 125-200"
                << "PRECINTADO 13-20"<< "PRECINTADO 25-40"<< "PRECINTADO 50-100" << "PRECINTADO 125-200";

    xlsx.selectSheet("RESUMEN SERVICIOS");
    //write headers
    for (int i=0; i<listHeaders.count(); i++)
    {
        Format f;
        xlsx.write(1, i+1, listHeaders[i]/*.toUpper()*/);

    }
    //PAra gestor Geconta
    //write data
    QString temp = "";

    for(int n=0; n < listHeaders.count(); n++){
        if(n>1){
            QString header = listHeaders.at(n);
            temp = cantidadCamposResumenServicios(header, jsonArray);
        }
        else if(n==0){
            temp = QString::number(ultimoIDExportacion);
        }
        else if(n==1){
            temp = QDate::currentDate().toString("dd/MM/yyyy");
        }
        xlsx.write(2,n+1,temp);
        temp = "";
    }
    return "";
}
QString Tabla::cantidadCamposResumenServicios(QString cals, QJsonArray jsonArray){
    int cant=0;
    int cal1 =0, cal2=0;
    bool ok, ok1, ok2;
    if(cals.contains("DESPRECINTADO")){
        cals.remove("DESPRECINTADO");
        cals = cals.trimmed();
        QStringList split = cals.split("-");
        if(cals.contains("-") && split.size() >= 2){
            cal1 = split.at(0).toInt(&ok1);
            cal2 = split.at(1).toInt(&ok2);
        }
        for (int i=0; i < jsonArray.size(); i++) {
            int cal_json = jsonArray.at(i).toObject().value(calibre_real).toString().trimmed().toInt(&ok);
            QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
            if(ok){
                if(cal1 <= cal_json && cal2 >= cal_json && servicios_l.contains("Desprecintados", Qt::CaseInsensitive)){
                    cant++;
                }
            }

        }
    }
    else if(cals.contains("PRECINTADO")){
        cals.remove("PRECINTADO");
        cals = cals.trimmed();
        QStringList split = cals.split("-");
        if(cals.contains("-") && split.size() >= 2){
            cal1 = split.at(0).toInt(&ok1);
            cal2 = split.at(1).toInt(&ok2);
        }
        for (int i=0; i < jsonArray.size(); i++) {
            int cal_json = jsonArray.at(i).toObject().value(calibre_real).toString().trimmed().toInt(&ok);
            QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
            if(ok){
                if(cal1 <= cal_json && cal2 >= cal_json && servicios_l.contains("Precintados", Qt::CaseInsensitive)){
                    cant++;
                }
            }

        }
    }
    if(cals.contains("DESMONTAJE")){
        cals.remove("DESMONTAJE");
        cals = cals.trimmed();
        QStringList split = cals.split("_");
        if(cals.contains("_") && split.size() >= 2){
            cal1 = split.at(0).toInt(&ok1);
            cal2 = split.at(1).toInt(&ok2);
        }
        for (int i=0; i < jsonArray.size(); i++) {
            int cal_json = jsonArray.at(i).toObject().value(calibre_real).toString().trimmed().toInt(&ok);
            QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
            if(ok){
                if(cal1 <= cal_json && cal2 >= cal_json && servicios_l.contains("Baja", Qt::CaseInsensitive) && !servicios_l.contains("Rcfg Baja", Qt::CaseInsensitive)){
                    cant++;
                }
            }

        }
    }
    else if(cals.contains("MONTAJE")){
        cals.remove("MONTAJE");
        cals = cals.trimmed();
        QStringList split = cals.split("_");
        if(cals.contains("_") && split.size() >= 2){
            cal1 = split.at(0).toInt(&ok1);
            cal2 = split.at(1).toInt(&ok2);
        }
        for (int i=0; i < jsonArray.size(); i++) {
            int cal_json = jsonArray.at(i).toObject().value(calibre_real).toString().trimmed().toInt(&ok);
            QString res = jsonArray.at(i).toObject().value(resultado).toString().trimmed();
            QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
            if(ok){
                if(cal1 <= cal_json && cal_json <= cal2 && servicios_l.contains("Instalación", Qt::CaseInsensitive)
                        /*&& (resultado.contains("011")|| resultado.contains("012"))*/){ //011 012 - baja realizada
                    cant++;
                }
            }

        }
    }
    if(cals.contains("ALMACEN") && !cals.contains("MODULO")){
        cals.remove("ALMACEN");
        cals = cals.trimmed();
        QStringList split = cals.split("_");
        if(cals.contains("_") && split.size() >= 2){
            cal1 = split.at(0).toInt(&ok1);
            cal2 = split.at(1).toInt(&ok2);

            for (int i=0; i < jsonArray.size(); i++) {
                int cal_json = jsonArray.at(i).toObject().value(calibre_real).toString().trimmed().toInt(&ok);
                QString res = jsonArray.at(i).toObject().value(resultado).toString().trimmed();
                QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
                if(ok){
                    if(cal1 <= cal_json && cal2 >= cal_json && (resultado.contains("012"))
                            && servicios_l.contains("Baja", Qt::CaseInsensitive)
                            && !servicios_l.contains("Rcfg Baja", Qt::CaseInsensitive)){ //012 - baja y retirado al almacen
                        cant++;
                    }
                }

            }
        }
    }
    if(cals.contains("ALMACEN") && cals.contains("MODULO")){
        for (int i=0; i < jsonArray.size(); i++) {
            QString radio = jsonArray.at(i).toObject().value(tipoRadio_devuelto).toString().trimmed();
            QString res = jsonArray.at(i).toObject().value(resultado).toString().trimmed();
            QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
            if((radio.contains("R3",Qt::CaseInsensitive) || radio.contains("R4",Qt::CaseInsensitive)
                || radio.contains("W4",Qt::CaseInsensitive) || radio.contains("LRW",Qt::CaseInsensitive))
                    && (resultado.contains("012"))
                    && servicios_l.contains("Baja", Qt::CaseInsensitive)
                    && !servicios_l.contains("Rcfg Baja", Qt::CaseInsensitive)){
                cant++;
            }
        }
    }
    if(cals == "TOMADATOS"){
        for (int i=0; i < jsonArray.size(); i++) {
            QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
            if(servicios_l.contains("Toma Dato", Qt::CaseInsensitive) && !servicios_l.contains("Toma Dato Radio", Qt::CaseInsensitive)){
                cant++;
            }
        }
    }
    if(cals == "TOMADATOSRADIO"){
        for (int i=0; i < jsonArray.size(); i++) {
            QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
            if(servicios_l.contains("Toma Dato Radio", Qt::CaseInsensitive)){
                cant++;
            }
        }
    }
    if(cals == "EMISOR PROPIO REINSTALADO/RECONFIG"){
        for (int i=0; i < jsonArray.size(); i++) {
            QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
            if(servicios_l.contains("Rcfg Propio", Qt::CaseInsensitive)){
                cant++;
            }
        }
    }
    if(cals == "EMISORA BAJA REINSTALADO/RECONFIG"){
        for (int i=0; i < jsonArray.size(); i++) {
            QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
            if(servicios_l.contains("Rcfg Baja", Qt::CaseInsensitive)){
                cant++;
            }
        }
    }
    if(cals == "EMISORA NUEVA CONFIG"){
        for (int i=0; i < jsonArray.size(); i++) {
            QString servicios_l = jsonArray.at(i).toObject().value(servicios).toString().trimmed();
            if(servicios_l.contains("Rcfg Nuevo", Qt::CaseInsensitive)){
                cant++;
            }
        }
    }

    if(cant >0){
        return QString::number(cant);
    }
    return "";
}

QString Tabla::fillSheetResumenMaterial(QJsonArray jsonArray, QXlsx::Document &xlsx)
{
    QStringList listHeaders;
    listHeaders << "IDEXPORT"  << "FECHA"  << "MM13"  << "MM15"
                << "MM20"  << "MM25"  << "MM30" << "MM40"  << "MM50"
                << "MM65"  << "MM80" << "MM100" << "MM15B" << "MM20B"
                << "MM25B" << "MM30B" << "WOLTMAN 50" << "WOLTMAN 65"
                << "WOLTMAN 80" << "WOLTMAN 100" << "WOLTMAN 125"
                << "WOLTMAN 150"<< "WOLTMAN 200" << "MODULOR3"
                << "MODULOR4"<< "MODULOW4" << "MODULOLRW" << "REED"<< "DP"<< "PULSAR";

    xlsx.selectSheet("RESUMEN MATERIAL");
    //write headers
    for (int i=0; i<listHeaders.count(); i++)
    {
        Format f;
        xlsx.write(1, i+1, listHeaders[i]/*.toUpper()*/);

    }
    //PAra gestor Geconta
    //write data
    QString temp = "";

    for(int n=0; n < listHeaders.count(); n++){
        if(n>1){
            QString header = listHeaders.at(n);
            if(header.contains("MM")){
                temp = (cantidadDeCalibreMM(header, jsonArray));
            }
            else if(header.contains("WOLTMAN")){
                temp = (cantidadDeCalibreWOLTMAN(header, jsonArray));
            }
            else if(header.contains("MODULO")){
                temp = (cantidadDeCalibreTipoRadio(header, jsonArray));
            }
            else{
                if(header =="REED"){
                    temp = (cantidadDeREED(jsonArray));
                }else if(header =="DP"){
                    temp = (cantidadDeDP(jsonArray));
                }else if(header == "PULSAR"){
                    temp = (cantidadDePulsar(jsonArray));
                }
            }
        }
        else if(n==0){
            temp = QString::number(ultimoIDExportacion);
        }
        else if(n==1){
            temp = QDate::currentDate().toString("dd/MM/yyyy");
        }
        xlsx.write(2,n+1,temp);
        temp = "";
    }
    return "";
}
QString Tabla::cantidadDePulsar(QJsonArray jsonArray){
    int cant=0;
    for (int i=0; i < jsonArray.size(); i++) {
        QString suministros_l = jsonArray.at(i).toObject().value(suministros).toString().trimmed();
        if(suministros_l.contains("Pulsar", Qt::CaseInsensitive)){
            cant++;
        }
    }
    if(cant >0){
        return QString::number(cant);
    }
    return "";
}
QString Tabla::cantidadDeDP(QJsonArray jsonArray){
    int cant=0;
    for (int i=0; i < jsonArray.size(); i++) {
        QString suministros_l = jsonArray.at(i).toObject().value(suministros).toString().trimmed();
        if(suministros_l.contains("DP", Qt::CaseInsensitive)){
            cant++;
        }
    }
    if(cant >0){
        return QString::number(cant);
    }
    return "";
}
QString Tabla::cantidadDeREED(QJsonArray jsonArray){
    int cant=0;
    for (int i=0; i < jsonArray.size(); i++) {
        QString suministros_l = jsonArray.at(i).toObject().value(suministros).toString().trimmed();
        if(suministros_l.contains("REED", Qt::CaseInsensitive)){
            cant++;
        }
    }
    if(cant >0){
        return QString::number(cant);
    }
    return "";
}
QString Tabla::cantidadDeCalibreTipoRadio(QString radio, QJsonArray jsonArray){
    int cant=0;
    if(radio.contains("MODULO")){
        radio.remove("MODULO");
        radio = radio.trimmed();
        for (int i=0; i < jsonArray.size(); i++) {
            QString suministro_l = jsonArray.at(i).toObject().value(suministros).toString().trimmed();
            if(suministro_l.contains(radio, Qt::CaseInsensitive)){
                cant++;
            }
        }
    }
    if(cant >0){
        return QString::number(cant);
    }
    return "";
}
QString Tabla::cantidadDeCalibreWOLTMAN(QString cal, QJsonArray jsonArray){
    int cant=0;
    if(cal.contains("WOLTMAN")){
        cal.remove("WOLTMAN");
        cal = cal.trimmed();
        for (int i=0; i < jsonArray.size(); i++) {
            QString cal_json = jsonArray.at(i).toObject().value(calibre_real).toString().trimmed();
            QString marca_json = jsonArray.at(i).toObject().value(marca_devuelta).toString().trimmed();

            if(cal == cal_json && (marca_json.contains("052") || marca_json.contains("059"))){
                cant++;
            }
        }
    }
    if(cant >0){
        return QString::number(cant);
    }
    return "";
}
QString Tabla::cantidadDeCalibreMM(QString cal, QJsonArray jsonArray){
    int cant=0;
    if(cal.contains("MM")){
        cal.remove("MM");
        cal = cal.trimmed();
        for (int i=0; i < jsonArray.size(); i++) {
            QString cal_json = jsonArray.at(i).toObject().value(calibre_real).toString().trimmed();
            QString marca_json = jsonArray.at(i).toObject().value(marca_devuelta).toString().trimmed();
            if(cal.contains("B")){
                if(cal.contains(cal_json) && marca_json.contains("036")){
                    cant++;
                }
            }
            else{
                if(cal == cal_json && (marca_json.contains("008") || marca_json.contains("007") || marca_json.contains("054"))){
                    cant++;
                }
            }
        }
    }
    if(cant >0){
        return QString::number(cant);
    }
    return "";
}
QString Tabla::fillSheetPartes(QJsonArray jsonArray, QXlsx::Document &xlsx)
{
    QMap<QString,QString> mapa_exp = fillMapaExpCABB();
    QStringList listHeaders;
    listHeaders << "IDCENTRO"  << "IDEXPORT"  << "IDSAT"  << "IDMODULO2"  << "CODIGOCAUDAL"
                << "CALIBRE2"  << "DIGITOS2"  << "REFEXPORTABONADO"  << "CODIGORESULTADO"
                << "CODIGOMOTIVO"  << "FECHASERVICIO"  << "LECTURA2"  << "LECTURAVIEJO"
                << "ADICIONALLECTORCONTADOR"  << "CODIGOAGUAS"  << "CODIGOMARCA"<< "FECHABAJA"
                << "CODIGOAVERIA"  << "OBSINTERNAS"<< "CODIGOAVERIAFINAL"
                << "TIPO FLUIDO"  << "TIPO DE RADIO" << "GEOLOCALIZACION";

    int rows = jsonArray.count();

    xlsx.selectSheet("PARTES");
    //write headers
    for (int i=0; i<listHeaders.count(); i++)
    {
        Format f;
        xlsx.write(1, i+1, listHeaders[i]/*.toUpper()*/);

    }
    //PAra gestor Geconta
    //write data
    QString temp;

    for(int i = 0, row=2; i < rows; i++)
    {
        for(int n=0; n < listHeaders.count(); n++){
            if(n>2){
                QString header = listHeaders.at(n);
                QString value = jsonArray[i].toObject().value(mapa_exp.value(header)).toString();
                if(value.contains(" - ")){
                    value = value.split(" - ").at(0).trimmed();
                }
                temp = value;

                if(header == "ADICIONALLECTORCONTADOR"){
                    temp += jsonArray[i].toObject().value(RESTO_EM).toString();
                }
                if(header == "FECHABAJA"){
                    QString arealizar = jsonArray[i].toObject().value(AREALIZAR_devuelta).toString().trimmed();
                    if(arealizar != "036" && arealizar != "037" && arealizar != "039"){
                        temp = "";
                    }

                }
            }
            else if(n+1==2){
                temp = QString::number(ultimoIDExportacion);
            }
            else {
                temp = mapa_exp.value(listHeaders.at(n));
            }
            //                temp.chop(7);
            if(n+1==3){
                temp = jsonArray[i].toObject().value(ID_SAT).toString().trimmed();
            }
            xlsx.write(row,n+1,temp);
        }
        row++;

    }
    return "";
}

QString Tabla::export_to_excel_IDExp(QJsonArray jsonArray)
{
    QString numExp = composeNumExp(ultimoIDExportacion);
    //    QString rutaToXLSXFile = QFileDialog::getSaveFileName(this,"Seleccione la ruta y nombre del fichero xlsx del CABB.", QDir::current().path(), "Excel (*.xlsx)");

    QString prevdir = setDirExpToExplorer();
    QString rutaToXLSXFile = prevdir +"/GCT"+ numExp + ".xlsx";
    QString rutaToXLSXFile_gestor_independiente = prevdir +"/GCT"+ numExp + "_Gestor_Independiente.xlsx";

    if(!rutaToXLSXFile.isNull() && !rutaToXLSXFile.isEmpty())
    {
        QXlsx::Document xlsx;

        xlsx.addSheet("PARTES");
        xlsx.addSheet("RESUMEN MATERIAL");
        xlsx.addSheet("RESUMEN SERVICIOS");
        xlsx.addSheet("CAUSAS");
        xlsx.addSheet("CALIBRES");
        xlsx.addSheet("EXCEPCIONES");

        fillSheetPartes(jsonArray, xlsx);
        fillSheetResumenMaterial(jsonArray, xlsx);
        fillSheetResumenServicios(jsonArray, xlsx);
        fillSheetCausas(jsonArray, xlsx);
        fillSheetCalibres(jsonArray, xlsx);
        fillSheetExcepciones(jsonArray, xlsx);
        xlsx.saveAs(rutaToXLSXFile);
        GlobalFunctions::showMessage(this,"Éxito","Fichero XLSX de tareas generado");
    }
    return prevdir;
}

void Tabla::on_actionImportacion_triggered()
{
    filtrarPorFecha(FechImportacion);
}
void Tabla::on_actionEjecucion_triggered()
{
    filtrarPorFecha(F_INST);
}
void Tabla::on_actionCierre_triggered()
{
    filtrarPorFecha(FECH_CIERRE);
}
void Tabla::on_actionInformada_triggered()
{
    filtrarPorFecha(fech_informacionnew);
}
void Tabla::on_actionDe_Modificacion_2_triggered()
{
    filtrarPorFecha(date_time_modified);
}
void Tabla::on_actionDe_Cita_triggered()
{
    filtrarPorFecha(fecha_hora_cita);
}
void Tabla::get_dates_selected(QStringList dates)
{
    show_loading("Buscando Resultados...");
    filtering = true;
    selected_dates = dates;

    QString queryStatus = getQueyStatus();
    QString query = "(";
    QString date;
    foreach(date, dates){
        if(query == "("){
            query += " ( (`" + fecha_to_filter + "` LIKE '" + date + "%') ";
        }else{
            query += " OR (`" + fecha_to_filter + "` LIKE '" + date + "%')";
        }
    }
    if(query != "("){
        query += ") AND ";
    }
    query +=  " (" + queryStatus + ") )";
    getTareasCustomQuery(query);
    jsonArrayInTableFiltered = jsonArrayAll;

    fixModelforTable(jsonArrayInTableFiltered);
    setTableView();
    hide_loading();
}

void Tabla::filtrarPorFecha(QString tipoFecha){

    DateRangeSelection *dateRange = new DateRangeSelection(this);
    connect(dateRange, &DateRangeSelection::dates_range, this, &Tabla::get_dates_selected);
    fecha_to_filter = tipoFecha;
    dateRange->show();
}

QString Tabla::getDirection(QJsonObject jsonObject){
    QString dir = "";
    QString poblacion_l = jsonObject.value(poblacion).toString();
    if(checkIfFieldIsValid(poblacion_l)){
        dir += poblacion_l;
    }
    QString calle_l = jsonObject.value(calle).toString();
    if(checkIfFieldIsValid(calle_l)){
        dir += "  " +calle_l;
    }
    QString portal = jsonObject.value(numero).toString();
    if(checkIfFieldIsValid(portal)){
        dir += "  " +portal;
    }
    return dir.trimmed();
}
QJsonArray Tabla::ordenarPorBateria(QJsonArray jsonArray){
    QJsonArray orderJsonArray;
    QJsonArray tempJsonArray;
    QJsonArray inBatteryJsonArray;
    QJsonArray notInBatteryJsonArray;
    QStringList direcciones;
    for(int i=0; i< jsonArray.size(); i++){
        QJsonObject jsonObject = jsonArray.at(i).toObject();
        if(checkIfBatteryTask(jsonObject)){
            inBatteryJsonArray.append(jsonObject);
            QString dir = getDirection(jsonObject);
            if(!direcciones.contains(dir)){
                direcciones.append(dir);
            }
        }else{
            qDebug()<<"NOT BATTERY "<< jsonObject.value(acceso).toString()
                   <<" "<< jsonObject.value(emplazamiento).toString();
            notInBatteryJsonArray.append(jsonObject);
        }
    }
    QString dir;
    foreach(dir, direcciones){
        tempJsonArray = QJsonArray();
        for(int i=0; i< inBatteryJsonArray.size(); i++){
            QJsonObject jsonObject = inBatteryJsonArray.at(i).toObject();
            QString tempDir = getDirection(jsonObject);
            if(tempDir == dir){
                tempJsonArray.append(jsonObject);
            }
        }
        QStringList fields;
        fields << ubicacion_en_bateria << emplazamiento;
        tempJsonArray = ordenarPor(tempJsonArray, fields);
        for(int i=0; i< tempJsonArray.size(); i++){
            orderJsonArray.append(tempJsonArray.at(i).toObject());
        }
    }
    for(int i=0; i< notInBatteryJsonArray.size(); i++){
        orderJsonArray.append(notInBatteryJsonArray.at(i).toObject());
    }
    return orderJsonArray;
}
QJsonObject Tabla::fillCausaData(QJsonObject jsonObjectTarea, QString anomaly){
    QJsonObject jsonCausa = Causa::getCausaObject(anomaly);

    qDebug()<<jsonCausa;
    jsonObjectTarea.insert(ANOMALIA, anomaly);
    jsonObjectTarea.insert(AREALIZAR, jsonCausa.value(arealizar_causas).toString());
    jsonObjectTarea.insert(INTERVENCION, jsonCausa.value(causa_causas).toString());//intervencion
    jsonObjectTarea.insert(accion_ordenada, jsonCausa.value(accion_ordenada_causas).toString());
    QString caliber = jsonObjectTarea.value(calibre_toma).toString();
    QString mark = jsonObjectTarea.value(marca_contador).toString();
    QString tipo_tarea_l = screen_tabla_tareas::parse_tipo_tarea(anomaly, caliber, mark);
    jsonObjectTarea.insert(tipo_tarea, tipo_tarea_l);
    QString causa_origen_l = anomaly + " - " + jsonCausa.value(causa_causas).toString();
    jsonObjectTarea.insert(causa_origen, causa_origen_l);

    return jsonObjectTarea;
}
void Tabla::get_fields_selected(QMap<QString, QString> map_received){
    if(!map_received.isEmpty()){
        fields_selected = map_received;
    }
}

void Tabla::on_actionAsignar_campos_comunes_triggered()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una tarea");
        return;
    }
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    Fields_to_Assign  *fields_to_Assign_Dialog = new Fields_to_Assign(this, empresa);
    fields_to_Assign_Dialog->setJsonArrayContadores(jsonArrayContadores);
    connect(fields_to_Assign_Dialog,SIGNAL(fields_selected(QMap<QString, QString>)),
            this,SLOT(get_fields_selected(QMap<QString, QString>)));

    QJsonObject jsonObject;
    QList<QString> keys;
    if(fields_to_Assign_Dialog->exec())
    {
        other_task_screen *oneTareaScreen = new other_task_screen(nullptr, false, true, empresa);
        if(!fields_selected.isEmpty()){
            show_loading("Espere, asignando campos a tareas...");
            ui->statusbar->showMessage("Espere, asignando campos a tareas...");
            keys = fields_selected.keys();
            for(int i = 0; i< fields_selected.keys().size(); i++){
                qDebug()<<"Campo: "<<fields_selected.keys().at(i)<<"   Valor: "<<fields_selected.values().at(i);
            }

            if(fields_selected.keys().contains(ANOMALIA)){
                int total = selection.count();
                for(int i=0, reintentos = 0; i< selection.count(); i++)
                {
                    setLoadingText("Espere, subiendo tareas... ("
                                   +QString::number(i+1)+"/"+QString::number(total)+")");
                    QModelIndex index = selection.at(i);
                    int posicion = index.row();

                    if(jsonArray.size() > posicion){
                        jsonObject = jsonArray[posicion].toObject();

                        for(int n=0; n < fields_selected.size(); n++){
                            QString key = keys.at(n);
                            if(key!="null" && key != nullptr && !key.isEmpty()){
                                QString value = fields_selected.value(key, "null");
                                if(value != "null"){
                                    jsonObject.insert(key, value);
                                    if(key == ANOMALIA){
                                        jsonObject = fillCausaData(jsonObject, value);
                                    }
                                }
                            }
                        }

                        if(oneTareaScreen != nullptr){
                            oneTareaScreen->clearTask();
                        }
                        oneTareaScreen->setShowMesageBox(false);

                        connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
                        emit sendData(jsonObject);
                        disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));

                        oneTareaScreen->populateView(false);

                        //            QEventLoop q;

                        QEventLoop *q = new QEventLoop();

#if (USE_TIMEOUT == 1)
                        try{
                            QTimer::singleShot(TIMEOUT, this, SLOT(conection_timeout()));
                        }catch(QException e){
                            e.raise();
                            qDebug()<<"Excepcion del timer lanzada";
                        }

#endif
                        connect(oneTareaScreen, &other_task_screen::task_upload_excecution_result,q,&QEventLoop::exit);

                        oneTareaScreen->setShowMesageBox(false);

                        QTimer *timer;
                        timer = new QTimer();

                        timer->setSingleShot(true);
                        connect(timer,SIGNAL(timeout()),oneTareaScreen,SLOT(on_pb_update_server_info_clicked()));
                        timer->start(DELAY);

                        switch(q->exec())
                        {
                        case database_comunication::script_result::timeout:
                            i--;
                            reintentos++;
                            if(reintentos == RETRIES)
                            {
                                GlobalFunctions gf(this);
                                GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                                i = jsonArray.size();
                            }
                            break;
                        case database_comunication::script_result::task_to_server_ok:
                            reintentos = 0;
                            break;
                        case database_comunication::script_result::update_task_to_server_failed:
                            i--;
                            reintentos++;
                            if(reintentos == RETRIES)
                            {
                                GlobalFunctions gf(this);
                                GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                                i = jsonArray.size();
                            }
                            break;
                        case database_comunication::script_result::create_task_to_server_failed:
                            i--;
                            reintentos++;
                            if(reintentos == RETRIES)
                            {
                                GlobalFunctions gf(this);
                                GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                                i = jsonArray.size();
                            }
                            break;
                        }
                        disconnect(oneTareaScreen, &other_task_screen::task_upload_excecution_result,q,&QEventLoop::exit);
                        delete q;
                    }
                }
            }else{
                QJsonObject o, campos;
                QStringList numeros_internos_list, emplazament_codes;
                for(int i=0; i< selection.count(); i++)
                {
                    QModelIndex index = selection.at(i);
                    int posicion = index.row();

                    if(jsonArray.size() > posicion){
                        o = jsonArray[posicion].toObject();
                        numeros_internos_list << o.value(numero_interno).toString();
                    }
                }
                for(int n=0; n < fields_selected.size(); n++){
                    QString key = fields_selected.keys().at(n);
                    if(checkIfFieldIsValid(key)){
                        QString value = fields_selected.value(key, "null");
                        if(checkIfFieldIsValid(value)){
                            campos.insert(key, value);
                        }
                    }
                }
                if(fields_selected.keys().contains(status_tarea)){
                    QString status = campos.value(status_tarea).toString();
                    if(status.contains(state_informada)){
                        campos = set_date_from_status(campos, status, QDateTime::currentDateTime().toString(formato_fecha_hora));
                    }
                }
                if(fields_selected.keys().contains(equipo)){
                    QJsonObject campos_itacs;
                    QString team_value = fields_selected.value(equipo, "");
                    campos_itacs.insert(equipo, team_value);
                    if(updateITACs(emplazament_codes, campos_itacs)){
                        ui->statusbar->showMessage("Asignados ITACs correctamente");
                    }else{
                        ui->statusbar->showMessage("Fallo Asignando ITACs");
                    }
                }
                if(fields_selected.keys().contains(operario)){
                    QJsonObject campos_itacs;
                    QString operator_value = fields_selected.value(operario, "");
                    campos_itacs.insert(operario, operator_value);
                    if(updateITACs(emplazament_codes, campos_itacs)){
                        ui->statusbar->showMessage("Asignados ITACs correctamente");
                    }else{
                        ui->statusbar->showMessage("Fallo Asignando ITACs");
                    }
                }
                campos.insert(date_time_modified,QDateTime::currentDateTime().toString(formato_fecha_hora));
                if(update_fields(numeros_internos_list, campos)){
                    ui->statusbar->showMessage("Asignado correctamente");
                }else{
                    ui->statusbar->showMessage("Fallo Asignando");
                }
            }
            hide_loading();
            ui->statusbar->showMessage("Campos asignados correctamente");
            GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");

            emit updateTableInfo();
        }
        oneTareaScreen->deleteLater();
    }
}

void Tabla::get_gestor_selected(QString g)
{
    emit gestorSelected(g);
    GlobalFunctions::writeGestorSelected(g);
}

void Tabla::get_equipo_selected(QString u)
{
    equipoName = u;
}

void Tabla::get_user_selected(QString u)
{
    operatorName = u;
}

void Tabla::on_actionDescargar_Fotos()
{
    if(!other_task_screen::conexion_activa){
        GlobalFunctions::showMessage(this,"Sin conexión","No se puede descargar fotos");
        return;
    }

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una tarea");
        return;
    }

    show_loading("Espere, descargando fotos...");
    ui->statusbar->showMessage("Espere, descargando fotos...");
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QJsonObject o;

    int total = selection.count();

    other_task_screen *oneTareaScreen = new other_task_screen(nullptr, false, true, empresa);

    for(int i=0; i< selection.count(); i++)
    {
        setLoadingText("Espere, descargando fotos... ("
                       +QString::number(i+1)+"/"+QString::number(total)+")");

        QModelIndex index = selection.at(i);
        int posicion = index.row();

        if(jsonArray.size() > posicion){
            QJsonObject o = jsonArray.at(posicion).toObject();

            connect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));
            emit sendData(o);
            disconnect(this, SIGNAL(sendData(QJsonObject)), oneTareaScreen, SLOT(getData(QJsonObject)));

            oneTareaScreen->populateView(true);
            oneTareaScreen->createAutoPDF(false, true);
            oneTareaScreen->clearTask();
        }
    }
    oneTareaScreen->deleteLater();
    hide_loading();
    ui->statusbar->showMessage("Descargadas correctamente");
    GlobalFunctions::showMessage(this,"Éxito","Las fotos han sido descargadas");

}
void Tabla::on_actionResumen_Tareas_triggered(){
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una tarea");
        return;
    }
    if(!selection.empty())
    {
        QJsonArray jsonArray = getCurrentJsonArrayInTable();
        QJsonArray jsonArrayTareasForResumen;
        for(int i=0; i< selection.count(); i++)
        {
            QModelIndex index = selection.at(i);
            int posicion = index.row();

            if(jsonArray.size() > posicion){
                jsonArrayTareasForResumen.append(
                            jsonArray[posicion].toObject());
            }
        }
        QPoint thisPos = this->mapToGlobal(QPoint(0,0));
        Resumen_Tareas *resumen = new Resumen_Tareas(nullptr, jsonArrayTareasForResumen);
        connect(this, &Tabla::closing,
                resumen, &Resumen_Tareas::close);
        connect(resumen, &Resumen_Tareas::openTarea,
                this, &Tabla::openTareaX);
        resumen->move(thisPos.x() + this->width()/2 -resumen->width()/2,
                      thisPos.y() + this->height()/2 -resumen->height()/2);
        resumen->show();
    }
}
void Tabla::on_actionAsignar_a_un_equipo_triggered(){
    if(!other_task_screen::conexion_activa){
        GlobalFunctions::showMessage(this,"Sin conexión","No se puede asignar sin conexión");
        return;
    }

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una tarea");
        return;
    }

    Equipo_Selection_Screen *seleccionEquipoScreen = new Equipo_Selection_Screen(this, empresa);

    seleccionEquipoScreen->getEquipo_OperariosFromServer();
    connect(seleccionEquipoScreen,&Equipo_Selection_Screen::equipoSelected,
            this,&Tabla::get_equipo_selected);

    if(!selection.empty())
    {
        if(seleccionEquipoScreen->exec())
        {
            show_loading("Espere, Asignando equipo...");
            ui->statusbar->showMessage("Espere, Asignando equipo...");
            QJsonArray jsonArray = getCurrentJsonArrayInTable();
            QJsonObject o, campos;
            QStringList numeros_internos_list, emplazament_codes;
            for(int i=0; i< selection.count(); i++)
            {
                QModelIndex index = selection.at(i);
                int posicion = index.row();

                if(jsonArray.size() > posicion){
                    o = jsonArray[posicion].toObject();
                    numeros_internos_list << o.value(numero_interno).toString();
                    if(!emplazament_codes.contains(o.value(codigo_de_geolocalizacion).toString())){
                        emplazament_codes << o.value(codigo_de_geolocalizacion).toString();
                    }
                }
            }
            campos.insert(equipo, equipoName);
            QString date_now = QDateTime::currentDateTime().toString(formato_fecha_hora);
            campos.insert(date_time_modified, date_now);
            if(!emplazament_codes.isEmpty()){
                QJsonObject campos_itacs;
                campos_itacs.insert(equipo_itacs, equipoName);
                campos_itacs.insert(date_time_modified_itacs, date_now);
                if(updateITACs(emplazament_codes, campos_itacs)){
                    ui->statusbar->showMessage("Asignados ITACs correctamente");
                }else{
                    ui->statusbar->showMessage("Fallo Asignando ITACs");
                }
            }
            if(update_fields(numeros_internos_list, campos)){
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
                ui->statusbar->showMessage("Asignado correctamente");
            }else{
                GlobalFunctions gf(this);
                GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
                ui->statusbar->showMessage("Fallo Asignando");
            }
            hide_loading();
            disconnect(seleccionEquipoScreen,&Equipo_Selection_Screen::equipoSelected,
                       this,&Tabla::get_equipo_selected);

            emit updateTableInfo();
        }
    }
}
void Tabla::on_actionAsignar_a_un_operario_triggered()
{
    if(!other_task_screen::conexion_activa){
        GlobalFunctions::showMessage(this,"Sin conexión","No se puede asignar sin conexión");
        return;
    }

    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();

    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una tarea");
        return;
    }
    if(seleccionOperarioScreen ==nullptr){
        seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);
    }
    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));

    if(!selection.empty())
    {
        if(seleccionOperarioScreen->exec())
        {
            show_loading("Espere, Asignando operario...");
            ui->statusbar->showMessage("Espere, Asignando operario...");
            QJsonArray jsonArray = getCurrentJsonArrayInTable();
            QJsonObject o, campos;
            QStringList numeros_internos_list, emplazament_codes;
            for(int i=0; i< selection.count(); i++)
            {
                QModelIndex index = selection.at(i);
                int posicion = index.row();

                if(jsonArray.size() > posicion){
                    o = jsonArray[posicion].toObject();
                    numeros_internos_list << o.value(numero_interno).toString();
                    if(!emplazament_codes.contains(o.value(codigo_de_geolocalizacion).toString())){
                        emplazament_codes << o.value(codigo_de_geolocalizacion).toString();
                    }
                }
            }
            campos.insert(operario,operatorName);
            QString date_now = QDateTime::currentDateTime().toString(formato_fecha_hora);
            campos.insert(date_time_modified, date_now);

            if(!emplazament_codes.isEmpty()){
                QJsonObject campos_itacs;
                campos_itacs.insert(operario_itacs, operatorName);
                campos_itacs.insert(date_time_modified_itacs, date_now);
                if(updateITACs(emplazament_codes, campos_itacs)){
                    ui->statusbar->showMessage("Asignados ITACs correctamente");
                }else{
                    ui->statusbar->showMessage("Fallo Asignando ITACs");
                }
            }
            if(update_fields(numeros_internos_list, campos)){
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor");
                ui->statusbar->showMessage("Asignado correctamente");
            }else{
                GlobalFunctions gf(this);
                GlobalFunctions::showWarning(this,"Fallo","La información no fue actualizada en el servidor.");
                ui->statusbar->showMessage("Fallo Asignando");
            }
            hide_loading();
            disconnect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));
            emit updateTableInfo();
        }
    }
}

void Tabla::on_actionN_Serie_2_triggered()
{
    ordenarPor(numero_serie_contador, "");
}
void Tabla::on_actionN_Abonado_2_triggered()
{
    ordenarPor(numero_abonado, 0);
}
void Tabla::on_actionPor_Titular_triggered()
{
    ordenarPor(nombre_cliente, "");
}
void Tabla::on_actionPor_Portal_triggered()
{
    ordenarPor(numero, "");
}
void Tabla::on_actionPor_BIS_triggered()
{
    ordenarPor(BIS, "");
}
void Tabla::on_actionPor_Piso_triggered()
{
    ordenarPor(piso, "");
}
void Tabla::on_actionDe_Importaci_n_triggered()
{
    ordenarPor(FechImportacion, "");
}
void Tabla::on_actionDe_Cierre_triggered()
{
    ordenarPor(FECH_CIERRE, "");
}
void Tabla::on_actionDe_Informe_triggered()
{
    ordenarPor(fech_informacionnew, "");
}
void Tabla::on_actionDe_Ejecuci_n_triggered()
{
    ordenarPor(F_INST, "");
}
void Tabla::on_actionDe_Modificacion_triggered()
{
    ordenarPor(date_time_modified, "MAYOR_MENOR");
}
void Tabla::on_actionDe_Cita_2_triggered()
{
    ordenarPor(fecha_hora_cita, "");
}

void Tabla::on_actionPor_Ubicacion_en_Bater_a_triggered()
{
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    filtering = true;
    jsonArrayInTableFiltered = ordenarPorBateria(jsonArray);
    fixModelforTable(jsonArrayInTableFiltered);
    setTableView();
}

void Tabla::ordenarPor(QString field, int type, bool salvarHistoria){
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    QList<int> array;
    int temp;
    QString element;
    bool ok;
    for (int j =0; j < jsonArray.size(); j++) {
        QString element = jsonArray[j].toObject().value(field).toString().trimmed();
        temp = element.toInt(&ok);
        if(!ok){
            temp=999999999;
        }
        if(!array.contains(temp)){
            array.append(temp);
        }
    }
    for (int i =0; i < array.size(); i++) {
        for (int j =0; j < array.size(); j++) {
            if(type != 0){
                if((array[i] > array[j])){
                    temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
            }else{
                if(array[i] < array[j]){
                    temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
            }
        }
    }
    QJsonArray jsonarraySaved = jsonArray;
    jsonArray = QJsonArray();
    for (int j = 0; j < array.size(); j++) {
        for (int i = 0; i < jsonarraySaved.size(); i++) {
            QString element = jsonarraySaved[i].toObject().value(field).toString().trimmed();
            temp = element.toInt(&ok);
            if(!ok){
                temp=999999999;
            }
            if(array[j] == temp){
                jsonArray.append(jsonarraySaved[i].toObject());
            }
        }

    }
    if(filtering){
        jsonArrayInTableFiltered = jsonArray;

    }else{
        jsonArrayInTable = jsonArray;
    }
    fixModelforTable(jsonArray, salvarHistoria);
    setTableView();
}

QString Tabla::returnIfSerieOrPrefijo(QJsonArray jsonArray, int pos, QString field, QString previous_value){
    QString temp;
    if(jsonArray.size() <= pos){
        return previous_value;
    }
    if(field  == numero_serie_contador_devuelto){
        QString prefijo = jsonArray[pos].toObject().value(CONTADOR_Prefijo_anno_devuelto).toString().trimmed();
        QString value = jsonArray[pos].toObject().value(numero_serie_contador_devuelto).toString().trimmed();
        if(!checkIfFieldIsValid(prefijo) || value == prefijo){
            prefijo = eliminarNumerosAlFinal(value).trimmed();
        }
        if(value != prefijo){
            if(!prefijo.isEmpty() &&
                    (value.mid(0, prefijo.size()) == prefijo)){
                value = value.remove(0, prefijo.size()).trimmed();
            }
        }
        temp = value.trimmed();
    }
    else if(field == CONTADOR_Prefijo_anno_devuelto){
        QString prefijo = jsonArray[pos].toObject().value(CONTADOR_Prefijo_anno_devuelto).toString().trimmed();
        QString serie = jsonArray[pos].toObject().value(numero_serie_contador_devuelto).toString().trimmed();
        if(!checkIfFieldIsValid(prefijo) || prefijo.contains(serie)){
            prefijo = eliminarNumerosAlFinal(serie).trimmed();
        }
        temp = prefijo.trimmed();
    }
    else if(field  == numero_serie_contador){
        QString prefijo = jsonArray[pos].toObject().value(CONTADOR_Prefijo_anno).toString().trimmed();
        QString value = jsonArray[pos].toObject().value(numero_serie_contador).toString().trimmed();
        if(!checkIfFieldIsValid(prefijo) || value == prefijo){
            prefijo = eliminarNumerosAlFinal(value).trimmed();
        }
        if(value != prefijo){
            if(!prefijo.isEmpty() &&
                    (value.mid(0, prefijo.size()) == prefijo)){
                value = value.remove(0, prefijo.size()).trimmed();
            }
        }
        temp = value.trimmed();
    }
    else if(field == CONTADOR_Prefijo_anno){
        QString prefijo = jsonArray[pos].toObject().value(CONTADOR_Prefijo_anno).toString().trimmed();
        QString serie = jsonArray[pos].toObject().value(numero_serie_contador).toString().trimmed();
        if(!checkIfFieldIsValid(prefijo) || prefijo.contains(serie)){
            prefijo = eliminarNumerosAlFinal(serie).trimmed();
        }
        temp = prefijo.trimmed();
    }else{
        return previous_value;
    }
    return temp;
}

void Tabla::ordenarPor(QString field, QString type, bool salvarHistoria){
    QJsonArray jsonArray;
    if(filtering){
        jsonArray = jsonArrayInTableFiltered;
    }else{
        jsonArray = jsonArrayInTable;
    }
    QStringList array;
    QString temp;

    for (int j =0; j < jsonArray.size(); j++) {
        temp = jsonArray[j].toObject().value(field).toString().trimmed().toLower().replace(" ", "");
        temp = returnIfSerieOrPrefijo(jsonArray, j, field, temp).toLower().replace(" ", "");
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
            QString saveValue = jsonarraySaved[i].toObject().value(field).toString().trimmed().toLower().replace(" ", "");
            saveValue = returnIfSerieOrPrefijo(jsonarraySaved, i, field, saveValue).trimmed().toLower().replace(" ", "");
            if(array[j] ==  saveValue){
                jsonArray.append(jsonarraySaved[i].toObject());
            }
        }
    }
    if(filtering){
        jsonArrayInTableFiltered = jsonArray;

    }else{
        jsonArrayInTable = jsonArray;
    }
    fixModelforTable(jsonArray, salvarHistoria);
    setTableView();
}

QString Tabla::getValidField(QJsonObject jsonObject, QStringList fields){
    QString field;
    foreach(field, fields){
        QString value = jsonObject.value(field).toString();
        if(checkIfFieldIsValid(value)){
            return value;
        }
    }
    return "";
}
QJsonArray Tabla::ordenarPor(QJsonArray jsonArray, QStringList fields, QString type){

    QStringList array;
    QString temp;

    for (int j =0; j < jsonArray.size(); j++) {
        temp = getValidField(jsonArray[j].toObject(), fields);
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
            QString saveValue = temp = getValidField(jsonarraySaved[i].toObject(), fields);
            if(array[j] == saveValue){
                jsonArray.append(jsonarraySaved[i].toObject());
            }
        }
    }
    return jsonArray;
}


void Tabla::on_pb_eliminar_clicked()
{
    QJsonDocument d;
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
    if(selection.empty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Seleccione una o más tareas para eliminar");
        return;
    }
    else
    {
        QJsonArray jsonArray = getCurrentJsonArrayInTable();
        if(selection.count() > jsonArray.size()){
            QMessageBox::critical(this,"Error","Mas tareas que las que existen en servidor");
            return;
        }
        QJsonObject o;

        GlobalFunctions gf(this, empresa);

        if(gf.showQuestion(this,"Apunto de eliminar","¿Seguro que desea borrar la(s) tarea(s) seleccionada(s)?",
                           QMessageBox::Ok, QMessageBox::No) == QMessageBox::No){
            ui->statusbar->showMessage("Cancelando subida");
            return;
        }
        show_loading("Eliminando tareas...");
        ui->statusbar->showMessage("Espere eliminando tareas...");
        QJsonArray jsonArrayToErase = QJsonArray();
        for(int i=0; i< selection.count(); i++)
        {
            QModelIndex index = selection.at(i);
            int posicion = index.row();

            if(jsonArray.size() > posicion){
                o = jsonArray[posicion].toObject();

                jsonArrayToErase.append(o);
            }
        }
        emit eraseJsonArrayInServer(jsonArrayToErase);
    }
}
void Tabla::on_posicionBorrado(int pos, int total){
    setLoadingText("Eliminando tareas... ("+ QString::number(pos) + "/" + QString::number(total)+ ")");
}
void Tabla::resultado_Eliminacion_Tareas(int result){
    switch(result)
    {
    case database_comunication::script_result::timeout:
        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        break;
    case database_comunication::script_result::ok:
        emit updateTableInfo();
        GlobalFunctions::showMessage(this,"Eliminadas","Tarea(s) eliminada(s) correctamente");
        break;
    case database_comunication::script_result::delete_task_failed:
        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        break;
    }
    hide_loading();
}



void Tabla::on_pb_nuevaTarea_clicked()
{
    on_actionNueva_Tarea_triggered();
}

void Tabla::on_pb_hide_filters_clicked()
{
    filter_enabled = false;
    hideAllFilters();
}

void Tabla::on_actionFiltrarPrioridad_triggered(){
    Selection_Priority *screen_priority = new Selection_Priority(this);
    connect(screen_priority, &Selection_Priority::priorityselected, this, &Tabla::filtrarPrioridad);
    screen_priority->show();
    screen_priority->raise();
}
void Tabla::on_actionTipo_Orden_triggered(){
    SelectionOrder *screen_orden = new SelectionOrder(this);
    connect(screen_orden, &SelectionOrder::order_selected, this, &Tabla::filtrarPorOrden);
    screen_orden->show();
    screen_orden->raise();
}
void Tabla::filtrarPrioridad(QString prioridad_selected)
{
    show_loading("Buscando Resultados...");
    hideAllFilters();
    filtering = true;

    QString queryStatus = getQueyStatus();
    QString query = "( (`" + prioridad + "` LIKE '%" + prioridad_selected + "%') AND (" + queryStatus + ") )";
    getTareasCustomQuery(query);
    jsonArrayInTableFiltered = jsonArrayAll;

    filter_type = 0;

    fixModelforTable(jsonArrayInTableFiltered);
    setTableView();
    ui->statusbar->showMessage("Mostrando tareas de Orden: "+ prioridad_selected, 3000);
    hide_loading();
}
void Tabla::filtrarPorOrden(QString ord)
{
    show_loading("Buscando Resultados...");
    hideAllFilters();
    filtering = true;

    QString queryStatus = getQueyStatus();
    QString query = "( (`" + TIPORDEN + "` LIKE '%" + ord + "%') AND (" + queryStatus + ") )";
    getTareasCustomQuery(query);
    jsonArrayInTableFiltered = jsonArrayAll;

    filter_type = 0;


    fixModelforTable(jsonArrayInTableFiltered);
    setTableView();
    ui->statusbar->showMessage("Mostrando tareas de Orden: "+ ord, 3000);

    hide_loading();
}
void Tabla::on_actionVer_Diarias_triggered()
{
    filtrarPorOrden("DIARIAS");
}

void Tabla::on_actionVer_Masivas_triggered()
{
    filtrarPorOrden("MASIVAS");
}

void Tabla::on_actionVer_Especiales_triggered()
{
    filtrarPorOrden("ESPECIALES");
}

void Tabla::on_pb_cercania_clicked()
{
    show_loading("Cargando Mapa...");
    GlobalFunctions gf(this, empresa);
    QStringList fields;
    fields << numero_interno << tipo_tarea << hibernacion << prioridad << poblacion << calle << numero << BIS
           << ultima_modificacion << fecha_hora_cita << status_tarea << MENSAJE_LIBRE << nuevo_citas << numero_abonado
           << nombre_cliente << numero_serie_contador << telefono1 << telefono2 << nuevo_citas << date_time_modified
           << codigo_de_geolocalizacion << codigo_de_localizacion << geolocalizacion << zona << observaciones
           << equipo << OPERARIO << TIPORDEN << tipoRadio;
    QString query = "(" + lastQuery + ") AND ( "
                                      "( (" + codigo_de_localizacion + " <> 'NULL') AND (" + codigo_de_localizacion + " <> 'null') )"
                                                                                                                      "OR ( (" + geolocalizacion + " <> 'NULL') AND (" + geolocalizacion + " <> 'null') )"                                                                                                                                                                                      ")";
    qDebug() << query;
    QJsonArray jsonArray = gf.getTareasFields(fields, query);
    qDebug() << "jsonArray" << jsonArray.size();
    Mapas_Cercania *mapa = new Mapas_Cercania(nullptr, jsonArray, empresa);
    QRect rect = QGuiApplication::screens().first()->geometry();
    mapa->setFixedSize(rect.width()*3/4, rect.height()*9/10);
    mapa->move(0, 0);
    connect(mapa, &Mapas_Cercania::openTarea, this, &Tabla::openTareaX);
    connect(mapa, &Mapas_Cercania::showJsonArrayInTable, this, &Tabla::setJsonArrayFilterbyPerimeter);
    connect(mapa, &Mapas_Cercania::updateTareas, this, &Tabla::updateTableWithServerInfo);
    mapa->show();
    hide_loading();
}

void Tabla::on_actionMostrarEnMapa(){
    QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
    if(selection.isEmpty()){
        GlobalFunctions::showMessage(this,"Sin Selección","Debe seleccionar al menos una tarea");
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

    Mapas_Cercania *mapa = new Mapas_Cercania(nullptr, jsonArrayToShow, empresa);
    QRect rect = QGuiApplication::screens().first()->geometry();
    mapa->setFixedSize(rect.width()*3/4, rect.height()*9/10);
    mapa->move(0, 0);
    connect(mapa, &Mapas_Cercania::openTarea, this, &Tabla::openTareaX);
    connect(mapa, &Mapas_Cercania::showJsonArrayInTable, this, &Tabla::setJsonArrayFilterbyPerimeter);
    connect(mapa, &Mapas_Cercania::updateTareas, this, &Tabla::updateTableWithServerInfo);
    mapa->show();
}
void Tabla::setJsonArrayFilterbyPerimeter(QStringList princ_vars){
    show_loading("Buscando Resultados...");
    hideAllFilters();
    filtering = true;
    QString query = "(";
    QString value;
    foreach(value, princ_vars){
        if(query == "("){
            query += " ( (`" + numero_interno + "` LIKE '" + value + "') ";
        }else{
            query += " OR (`" + numero_interno + "` LIKE '" + value + "')";
        }
    }
    if(query != "("){
        query += ") AND ";
    }
    QString queryStatus = getQueyStatus();
    query +=  " (" + queryStatus + ") )";
    getTareasCustomQuery(query);
    jsonArrayInTableFiltered = jsonArrayAll;

    filter_type = 0;
    fixModelforTable(jsonArrayInTableFiltered);
    setTableView();
    hide_loading();
}

void Tabla::getMenuClickedItem(int selected)
{
    if(this->isHidden()){
        return;
    }
    if(selected == ASIGNAR_COMUNES){
        on_actionAsignar_campos_comunes_triggered();
    }
    else if(selected == MOSTRAR_EN_MAPA){
        on_actionMostrarEnMapa();
    }
    else if(selected == DESCARGAR_FOTOS){
        on_actionDescargar_Fotos();
    }
    else if(selected == RESUMEN_TAREAS){
        on_actionResumen_Tareas_triggered();
    }
    else if(selected == ASIGNAR_A_EQUIPO){
        on_actionAsignar_a_un_equipo_triggered();
    }
    else if(selected == ASIGNAR_A_OPERARIO){
        on_actionAsignar_a_un_operario_triggered();
    }
    else if(selected == ELIMINAR){
        on_pb_eliminar_clicked();
    }
    else if(selected == ABRIR){
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
        if(!selection.isEmpty() && selection.size()==1){
            QModelIndex index = selection.at(0);
            int posicion = index.row();
            abrirTareaX(posicion);
        }else{
            if(selection.size()>1){
                GlobalFunctions::showMessage(this,"Selección múltiple","No puede abrir más de una tarea");
            }else if(selection.isEmpty()){
                GlobalFunctions::showMessage(this,"Sin Selección","Seleccione al menos una tarea");
            }
        }
    }
}

void Tabla::on_tableView_pressed(const QModelIndex &index)
{
    Q_UNUSED(index);
    emit tablePressed();
    if(QApplication::mouseButtons()==Qt::RightButton){
        ui->statusbar->showMessage("RightButton");
        RightClickMenu *rightClickMenu = new RightClickMenu(0, QCursor::pos(), RightClickMenu::FROM_TAREAS);
        connect(rightClickMenu, SIGNAL(clickPressed(int)), this, SLOT(getMenuClickedItem(int)));
        connect(this, &Tabla::closing, rightClickMenu, &RightClickMenu::deleteLater);
        connect(this, &Tabla::tablePressed, rightClickMenu, &RightClickMenu::deleteLater);
        connect(this, &Tabla::mouse_pressed, rightClickMenu, &RightClickMenu::deleteLater);

        rightClickMenu->show();
    }else if(QApplication::mouseButtons()==Qt::LeftButton){

    }
}

void Tabla::on_pb_atras_clicked()
{
    if(jsonArrayHistoryList.size() <= 1){
        habilitarBotonAtras(false);
        return;
    }
    QJsonArray jsonArray = jsonArrayHistoryList.at(jsonArrayHistoryList.size()-2);//si son 10 elementos va a la pos 8 del array
    int rb = radioButtonsHistory.at(radioButtonsHistory.size()-2);
    countTareas = countTareasHistory.at(countTareasHistory.size()-2);//si son 10 elementos va a la pos 8 del array
    QString showingText = showingTextHistory.at(showingTextHistory.size()-2);
    QString paginationText = paginationHistory.at(paginationHistory.size()-2);
    QStringList paginationItems = paginationItemsHistory.at(paginationItemsHistory.size()-2);
    lastQuery = queryHistory.at(queryHistory.size()-2);//si son 10 elementos va a la pos 8 del array
    last_id_start = id_startHistory.at(id_startHistory.size()-2);//si son 10 elementos va a la pos 8 del array

    jsonArrayHistoryList.removeAt(jsonArrayHistoryList.size()-1);
    radioButtonsHistory.removeAt(radioButtonsHistory.size()-1);
    countTareasHistory.removeAt(countTareasHistory.size()-1);
    showingTextHistory.removeAt(showingTextHistory.size()-1);
    paginationHistory.removeAt(paginationHistory.size()-1);
    paginationItemsHistory.removeAt(paginationItemsHistory.size()-1);
    queryHistory.removeAt(queryHistory.size()-1);
    id_startHistory.removeAt(id_startHistory.size()-1);

    if(filtering){
        jsonArrayInTableFiltered = jsonArray;
    }else{
        jsonArrayInTable = jsonArray;
    }

    ui->l_cantidad_de_tareas->setText(showingText);
    ui->l_current_pagination->setSpinnerList(paginationItems);
    ui->l_current_pagination->setText(paginationText);

    QStringList split = paginationText.split("/");
    if(split.size() > 1){
        currentPage = split.at(0).toInt();
        currentPages = split.at(1).toInt();
        checkPaginationButtons();
    }
    setRadiobutton(rb);
    fixModelforTable(jsonArray, false);
    setTableView();
}
int Tabla::getCurrentRadioButton(){
    if(ui->rb_todas->isChecked()){
        return TODAS;

    }else if(ui->rb_abierta->isChecked()){
        return ABIERTAS;

    }else if(ui->rb_ausente->isChecked()){
        return AUSENTE;

    }else if(ui->rb_citas->isChecked()){
        return CITA;

    }else if(ui->rb_ejecutada->isChecked()){
        return EJECUTADA;

    }else if(ui->rb_cerrada->isChecked()){
        return CERRADA;

    }else if(ui->rb_informada->isChecked()){
        return INFORMADA;

    }else if(ui->rb_requerida->isChecked()){
        return REQUERIDA;
    }
    return TODAS;
}
void Tabla::setRadiobutton(int type){
    uncheckAllRadioButtons(type);
    if(type == TODAS){
        ui->rb_todas->setChecked(true);
    }else if(type == ABIERTAS){
        ui->rb_abierta->setChecked(true);
    }else if(type == AUSENTE){
        ui->rb_ausente->setChecked(true);
    }else if(type == CITA){
        ui->rb_citas->setChecked(true);
    }else if(type == EJECUTADA){
        ui->rb_ejecutada->setChecked(true);
    }else if(type == CERRADA){
        ui->rb_cerrada->setChecked(true);
    }else if(type == INFORMADA){
        ui->rb_informada->setChecked(true);
    }else if(type == REQUERIDA){
        ui->rb_requerida->setChecked(true);
    }
}
void Tabla::habilitarBotonAtras(bool enable_disable){
    ui->pb_atras->setEnabled(enable_disable);
}

void Tabla::on_pb_new_info_clicked()
{
    checkIfNewInfoSync();
    updateTableWithServerInfo();
}

void Tabla::on_actionCambios_sin_Revisar_triggered()
{
    filter_type |= F_SIN_REVISAR;
    filtrarPorCambiosSinRevisar();
}
void Tabla::filtrarPorCambiosSinRevisar(int id_start)
{
    show_loading("Buscando Resultados...");
    hideAllFilters();
    filtering = true;

    QString queryStatus = getQueyStatus();
    QString query = "( (`" + ultima_modificacion + "` LIKE '%" + "ANDROID" + "%') AND (" + queryStatus + ") )";
    getTareasCustomQuery(query, id_start);
    jsonArrayInTableFiltered = jsonArrayAll;

    filter_type = 0;

    fixModelforTable(jsonArrayInTableFiltered);
    setTableView();
    ui->statusbar->showMessage("Mostrando Tareas con Cambios sin Revisar...");

    hide_loading();
}

void Tabla::on_pb_crear_excel_clicked()
{
    export_tasks_in_table_to_excel();
}

void Tabla::export_tasks_in_table_to_excel()
{
    QDir dir;
    dir.setPath(QDir::currentPath() +"/Trabajo Exportado");
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    QString rutaToXLSXFile_gestor_independiente = QFileDialog::getSaveFileName(this,"Seleccione la ruta y nombre del fichero xlsx.",
                                                                               dir.path()+"/"+ QDateTime::currentDateTime().toString(formato_fecha_hora).
                                                                               replace(":","_").replace("-","_").replace(" ","__")
                                                                               +"__Trabajo_Exportado", "Excel (*.xlsx)");

    ui->statusbar->showMessage("Creando excel...");
    //    QString prevdir = setDirExpToExplorer();
    //    QString rutaToXLSXFile_gestor_independiente ="/Trabajo_Exportado.xlsx";
    QString rutaToXLSXFile_table = rutaToXLSXFile_gestor_independiente;
    rutaToXLSXFile_table = rutaToXLSXFile_table.remove(".xlsx")+"_Extendido.xlsx";
    QString rutaToXLSXFile_table_without_headers = rutaToXLSXFile_table;
    rutaToXLSXFile_table_without_headers = rutaToXLSXFile_table_without_headers.remove(".xlsx")+"_Sin_Cabecera.csv";

    QMap<QString,QString> mapa_exportacion, mapa_exportacion_alternativo, mapa_exp_extendido;

    mapa_exp_extendido.insert("Población",poblacion);
    mapa_exp_extendido.insert("CALLE",calle);
    mapa_exp_extendido.insert("Nº",numero);
    mapa_exp_extendido.insert("BIS",BIS);
    mapa_exp_extendido.insert("PISO",piso);
    mapa_exp_extendido.insert("MANO",mano);
    mapa_exp_extendido.insert("AÑO O PREFIJO CONT. RETIRADO",CONTADOR_Prefijo_anno);
    mapa_exp_extendido.insert("Nº SERIE CONT. RETIRADO",numero_serie_contador);
    mapa_exp_extendido.insert("CALIBRE CONT. RETIRADO",calibre_toma);
    mapa_exp_extendido.insert("OPERARIO",operario);
    mapa_exp_extendido.insert("Anomalía (TAREA A REALIZAR)",ANOMALIA);
    mapa_exp_extendido.insert("EMPLAZAMIENTO",emplazamiento);
    mapa_exp_extendido.insert("UBICACIÓN BATERÍA",ubicacion_en_bateria);
    mapa_exp_extendido.insert("OBSERVACIONES",observaciones);
    mapa_exp_extendido.insert("ACTIVIDAD",actividad);
    mapa_exp_extendido.insert("TITULAR",nombre_cliente);
    mapa_exp_extendido.insert("NºABONADO",numero_abonado);
    mapa_exp_extendido.insert("TELEFONO",telefono1);
    mapa_exp_extendido.insert("ACCESO",acceso);
    mapa_exp_extendido.insert("NUEVO",nuevo_citas);
    mapa_exp_extendido.insert("ZONAS",zona);
    mapa_exp_extendido.insert("MARCA",marca_contador);
    mapa_exp_extendido.insert("GESTOR",GESTOR);
    mapa_exp_extendido.insert("CÓDIGO DE EMPLAZAMIENTO",codigo_de_geolocalizacion);
    mapa_exp_extendido.insert("LINK GEOLOCALIZACIÓN",url_geolocalizacion);
    mapa_exp_extendido.insert("FECHA",F_INST);
    mapa_exp_extendido.insert("RESULTADO",resultado);
    mapa_exp_extendido.insert("ÚLTIMA LECTURA",lectura_actual);
    mapa_exp_extendido.insert("LECTURA DE CONTADOR INSTALADO",lectura_contador_nuevo);
    mapa_exp_extendido.insert("Nº SERIE CONTADOR INSTALADO",numero_serie_contador_devuelto);
    mapa_exp_extendido.insert("Nº ANTENA CONTADOR INSTALADO",numero_serie_modulo);
    mapa_exp_extendido.insert("MARCA CONTADOR INSTALADO",marca_devuelta);
    mapa_exp_extendido.insert("CALIBRE CONTADOR INSTALADO",calibre_real);
    mapa_exp_extendido.insert("LONGITUD CONTADOR INSTALADO",largo_devuelto);
    mapa_exp_extendido.insert("DIGITOS",RUEDASDV);
    mapa_exp_extendido.insert("TIPO",TIPOFLUIDO_devuelto);
    mapa_exp_extendido.insert("CLASE CONTADOR INSTALADO",TIPO_devuelto);
    mapa_exp_extendido.insert("EMPLAZAMIENTO DEVUELTO",emplazamiento_devuelto);
    mapa_exp_extendido.insert("Causa Destino",AREALIZAR_devuelta);
    mapa_exp_extendido.insert("Código Observaciones",observaciones_devueltas);
    mapa_exp_extendido.insert("Piezas",piezas);
    mapa_exp_extendido.insert("NUMERO INTERNO",numero_interno);


    mapa_exportacion.insert("Población",poblacion);
    mapa_exportacion.insert("CALLE",calle);
    mapa_exportacion.insert("Nº",numero);
    mapa_exportacion.insert("BIS",BIS);
    mapa_exportacion.insert("PISO",piso);
    mapa_exportacion.insert("MANO",mano);
    mapa_exportacion.insert("AÑO O PREFIJO CONT. RETIRADO",CONTADOR_Prefijo_anno);
    mapa_exportacion.insert("Nº SERIE CONT. RETIRADO",numero_serie_contador);
    mapa_exportacion.insert("CALIBRE CONT. RETIRADO",calibre_toma);
    mapa_exportacion.insert("OPERARIO",operario);
    mapa_exportacion.insert("Anomalía (TAREA A REALIZAR)",AREALIZAR_devuelta);
    mapa_exportacion.insert("EMPLAZAMIENTO",emplazamiento_devuelto);
    mapa_exportacion.insert("UBICACIÓN BATERÍA",ubicacion_en_bateria);
    mapa_exportacion.insert("OBSERVACIONES",observaciones_devueltas);
    mapa_exportacion.insert("ACTIVIDAD",actividad);
    mapa_exportacion.insert("TITULAR",nombre_cliente);
    mapa_exportacion.insert("NºABONADO",numero_abonado);
    mapa_exportacion.insert("TELEFONO",telefono1);
    mapa_exportacion.insert("ACCESO",acceso);
    mapa_exportacion.insert("RESULTADO",resultado);
    mapa_exportacion.insert("NUEVO",nuevo_citas);
    mapa_exportacion.insert("FECHA",F_INST);
    mapa_exportacion.insert("ZONAS",zona);
    mapa_exportacion.insert("RUTA",ruta);
    mapa_exportacion.insert("MARCA",marca_contador);
    mapa_exportacion.insert("ÚLTIMA LECTURA",lectura_actual);
    mapa_exportacion.insert("GESTOR",GESTOR);
    mapa_exportacion.insert("LECTURA DE CONTADOR INSTALADO",lectura_contador_nuevo);
    mapa_exportacion.insert("Nº SERIE CONTADOR INSTALADO",numero_serie_contador_devuelto);
    mapa_exportacion.insert("Nº ANTENA CONTADOR INSTALADO",numero_serie_modulo);
    mapa_exportacion.insert("CALIBRE CONTADOR INSTALADO",calibre_real);
    mapa_exportacion.insert("MARCA CONTADOR INSTALADO",marca_devuelta);
    mapa_exportacion.insert("CLASE CONTADOR INSTALADO",TIPO_devuelto);
    mapa_exportacion.insert("LONGITUD CONTADOR INSTALADO",largo_devuelto);
    mapa_exportacion.insert("CÓDIGO DE EMPLAZAMIENTO",codigo_de_geolocalizacion);
    mapa_exportacion.insert("LINK GEOLOCALIZACIÓN",url_geolocalizacion);
    //    mapa_exportacion.insert("VER PDF de Trabajo","ordenarPDF");

    mapa_exportacion_alternativo.insert("Población",poblacion);
    mapa_exportacion_alternativo.insert("CALLE",calle);
    mapa_exportacion_alternativo.insert("Nº",numero);
    mapa_exportacion_alternativo.insert("BIS",BIS);
    mapa_exportacion_alternativo.insert("PISO",piso);
    mapa_exportacion_alternativo.insert("MANO",mano);
    mapa_exportacion_alternativo.insert("AÑO O PREFIJO CONT. RETIRADO",CONTADOR_Prefijo_anno);
    mapa_exportacion_alternativo.insert("Nº SERIE CONT. RETIRADO",numero_serie_contador);
    mapa_exportacion_alternativo.insert("CALIBRE CONT. RETIRADO",calibre_toma);
    mapa_exportacion_alternativo.insert("OPERARIO",operario);
    mapa_exportacion_alternativo.insert("Anomalía (TAREA A REALIZAR)",ANOMALIA);
    mapa_exportacion_alternativo.insert("EMPLAZAMIENTO",emplazamiento);
    mapa_exportacion_alternativo.insert("UBICACIÓN BATERÍA",ubicacion_en_bateria);
    mapa_exportacion_alternativo.insert("OBSERVACIONES",observaciones);
    mapa_exportacion_alternativo.insert("ACTIVIDAD",actividad);
    mapa_exportacion_alternativo.insert("TITULAR",nombre_cliente);
    mapa_exportacion_alternativo.insert("NºABONADO",numero_abonado);
    mapa_exportacion_alternativo.insert("TELEFONO",telefono1);
    mapa_exportacion_alternativo.insert("ACCESO",acceso);
    mapa_exportacion_alternativo.insert("RESULTADO",resultado);
    mapa_exportacion_alternativo.insert("NUEVO",nuevo_citas);
    mapa_exportacion_alternativo.insert("FECHA",F_INST);
    mapa_exportacion_alternativo.insert("ZONAS",zona);
    mapa_exportacion_alternativo.insert("RUTA",ruta);
    mapa_exportacion_alternativo.insert("MARCA",marca_contador);
    mapa_exportacion_alternativo.insert("ÚLTIMA LECTURA",lectura_ultima);
    mapa_exportacion_alternativo.insert("GESTOR",GESTOR);
    mapa_exportacion_alternativo.insert("LECTURA DE CONTADOR INSTALADO",lectura_contador_nuevo);
    mapa_exportacion_alternativo.insert("Nº SERIE CONTADOR INSTALADO",numero_serie_contador_devuelto);
    mapa_exportacion_alternativo.insert("Nº ANTENA CONTADOR INSTALADO",numero_serie_modulo);
    mapa_exportacion_alternativo.insert("CALIBRE CONTADOR INSTALADO",calibre_real);
    mapa_exportacion_alternativo.insert("MARCA CONTADOR INSTALADO",marca_devuelta);
    mapa_exportacion_alternativo.insert("CLASE CONTADOR INSTALADO",TIPO_devuelto);
    mapa_exportacion_alternativo.insert("LONGITUD CONTADOR INSTALADO",largo_devuelto);
    mapa_exportacion_alternativo.insert("CÓDIGO DE EMPLAZAMIENTO",codigo_de_geolocalizacion);
    mapa_exportacion_alternativo.insert("LINK GEOLOCALIZACIÓN",url_geolocalizacion);
    //    mapa_exportacion_alternativo.insert("VER PDF de Trabajo","ordenarPDF");

    if(!rutaToXLSXFile_gestor_independiente.isNull() && !rutaToXLSXFile_gestor_independiente.isEmpty())
    {
        QStringList listHeaders_gestor_independiente, listHeaders_extendido;

        listHeaders_gestor_independiente << "Población" << "CALLE" << "Nº" << "BIS" << "PISO"
                                         << "MANO" << "AÑO O PREFIJO CONT. RETIRADO" << "Nº SERIE CONT. RETIRADO"
                                         << "CALIBRE CONT. RETIRADO"
                                         << "OPERARIO" <<"Anomalía (TAREA A REALIZAR)"
                                         << "EMPLAZAMIENTO" << "UBICACIÓN BATERÍA" << "OBSERVACIONES" << "ACTIVIDAD"
                                         << "TITULAR" << "NºABONADO" << "TELEFONO" << "ACCESO"
                                         << "RESULTADO" << "NUEVO" << "FECHA" << "ZONAS"
                                         << "RUTA" << "MARCA"<< "ÚLTIMA LECTURA"<<"GESTOR"
                                         <<"LECTURA DE CONTADOR INSTALADO"<< "Nº SERIE CONTADOR INSTALADO"
                                        <<"Nº ANTENA CONTADOR INSTALADO" << "CALIBRE CONTADOR INSTALADO"
                                       <<"MARCA CONTADOR INSTALADO" << "CLASE CONTADOR INSTALADO"<< "LONGITUD CONTADOR INSTALADO"
                                      << "CÓDIGO DE EMPLAZAMIENTO" << "LINK GEOLOCALIZACIÓN" /*<< "VER PDF de Trabajo"*/;

        listHeaders_extendido << "Población" << "CALLE" << "Nº" << "BIS" << "PISO"
                              << "MANO" << "AÑO O PREFIJO CONT. RETIRADO" << "Nº SERIE CONT. RETIRADO"
                              << "CALIBRE CONT. RETIRADO"
                              << "OPERARIO" <<"Anomalía (TAREA A REALIZAR)"
                              << "EMPLAZAMIENTO" << "UBICACIÓN BATERÍA" << "OBSERVACIONES" << "ACTIVIDAD"
                              << "TITULAR" << "NºABONADO" << "TELEFONO" << "ACCESO"
                              << "NUEVO" << "ZONAS" << "MARCA" << "GESTOR"
                              << "CÓDIGO DE EMPLAZAMIENTO" << "LINK GEOLOCALIZACIÓN"<< "FECHA"<<"RESULTADO"
                              <<"ÚLTIMA LECTURA"
                             <<"LECTURA DE CONTADOR INSTALADO"<< "Nº SERIE CONTADOR INSTALADO"
                            <<"Nº ANTENA CONTADOR INSTALADO" << "MARCA CONTADOR INSTALADO" << "CALIBRE CONTADOR INSTALADO"
                           << "LONGITUD CONTADOR INSTALADO" << "DIGITOS" << "TIPO" << "CLASE CONTADOR INSTALADO"
                           <<"EMPLAZAMIENTO DEVUELTO" << "Causa Destino"<<"Código Observaciones"
                          << "Piezas"<<"NUMERO INTERNO" /*<< "VER PDF de Trabajo"*/;

        QJsonArray jsonArray, jsonArrayAllShowing;

        if(filtering){
            jsonArrayAllShowing = jsonArrayInTableFiltered;
        }else{
            jsonArrayAllShowing = jsonArrayInTable;
        }

        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows();
        GlobalFunctions gf(this,empresa);
        if(!selection.isEmpty() && gf.showQuestion(this, "Confirmación","Desea exportar solo las seleccionadas?."
                                                   "\nPresione \"Ok\" para subir solo seleccionadas"
                                                   "\nPresione \"No\" para subir todas",
                                                   QMessageBox::Ok, QMessageBox::No, "Ok", "No")
                == QMessageBox::Ok){
            for (int i =0; i < selection.count(); i++) {
                QModelIndex index = selection.at(i);
                int posicion = index.row();
                if(posicion < jsonArrayAllShowing.size()){
                    jsonArray.append(jsonArrayAllShowing.at(posicion).toObject());
                }
                else{ qDebug()<<"Esta posicion no esta en el jsonArrayAlLShowing:" << QString::number(posicion);}
            }
        }else{
            jsonArray = jsonArrayAllShowing;
        }

        crearFicheroTxtConTabla(jsonArray, rutaToXLSXFile_gestor_independiente);
        crearFicheroDATConTabla(jsonArray, rutaToXLSXFile_gestor_independiente);

        int rows = jsonArray.count() ;

        QXlsx::Document xlsx_gestor_independiente, xlsx_table_extended;
        QFile fileCSV(rutaToXLSXFile_table_without_headers);
        bool openCSV = false;
        if(fileCSV.open(QIODevice::WriteOnly | QIODevice::Text)){
            openCSV = true;
        }
        QTextStream xout(&fileCSV);


        //write headers-----------------------------------------------------------------------------------
        for (int i=0; i<listHeaders_gestor_independiente.count(); i++)
        {
            xlsx_gestor_independiente.write(1, i+1, listHeaders_gestor_independiente[i]/*.toUpper()*/);
        }
        for (int i=0; i<listHeaders_extendido.count(); i++)
        {
            xlsx_table_extended.write(1, i+1, listHeaders_extendido[i]/*.toUpper()*/);
        }
        //end write headers-------------------------------------------------------------------------------
        QString header;
        QString value_header;
        QString value_header_alternative;
        for(int i = 0, row=2; i < rows; i++)
        {
            for(int n=0; n < listHeaders_gestor_independiente.count(); n++){
                header = listHeaders_gestor_independiente.at(n);
                value_header = mapa_exportacion.value(header);
                value_header_alternative = mapa_exportacion_alternativo.value(header);
                QString temp;
                //                if(header != "VER PDF de Trabajo"){
                temp = jsonArray[i].toObject().value(value_header).toString();

                if(temp.isEmpty()){
                    temp = jsonArray[i].toObject().value(value_header_alternative).toString();
                }
                if(value_header == telefono1){
                    QString tel1 = jsonArray[i].toObject().value(telefono1).toString();
                    QString tel2 = jsonArray[i].toObject().value(telefono2).toString();
                    temp = other_task_screen::nullity_check(tel1) + " " + other_task_screen::nullity_check(tel2);
                }
                if(value_header == numero_serie_contador){
                    QString prefijo = jsonArray[i].toObject().value(CONTADOR_Prefijo_anno).toString();
                    if(checkIfFieldIsValid(prefijo)){
                        if(!prefijo.isEmpty() &&
                                (temp.mid(0, prefijo.size()) == prefijo)){
                            temp = temp.remove(0, prefijo.size()).trimmed();
                        }
                    }
                }
                if(value_header == observaciones_devueltas){
                    if(checkIfFieldIsValid(temp)){
                        QStringList list;
                        list = temp.split("\n");
                        temp = "";
                        for (int c=0; c < list.size(); c++) {
                            temp+= list.at(c).split("-").at(0).trimmed()+".";
                        }
                        temp = temp.trimmed();
                        temp.remove(temp.size()-1,1);
                    }
                }
                xlsx_gestor_independiente.write(row,n+1,temp);
            }
            row++;
        }
        for(int i = 0, row=2; i < rows; i++)
        {
            for(int n=0; n < listHeaders_extendido.count(); n++){
                header = listHeaders_extendido.at(n);
                value_header = mapa_exp_extendido.value(header);
                QJsonObject jsonObject = jsonArray[i].toObject();
                QString temp = jsonObject.value(value_header).toString();

                if(value_header == telefono1){
                    QString tel1 = jsonObject.value(telefono1).toString();
                    QString tel2 = jsonObject.value(telefono2).toString();
                    temp = other_task_screen::nullity_check(tel1) + " " + other_task_screen::nullity_check(tel2);
                }
                if(value_header == numero_serie_contador){
                    QString prefijo = jsonObject.value(CONTADOR_Prefijo_anno).toString();
                    if(checkIfFieldIsValid(prefijo)){
                        if(!prefijo.isEmpty() &&
                                (temp.mid(0, prefijo.size()) == prefijo)){
                            temp = temp.remove(0, prefijo.size()).trimmed();
                        }
                    }
                }
                if(value_header == lectura_contador_nuevo){
                    if(!checkIfFieldIsValid(temp)){
                        temp = "0";
                    }
                }
                if(value_header == observaciones_devueltas){
                    if(checkIfFieldIsValid(temp)){
                        QStringList list;
                        list = temp.split("\n");
                        temp = "";
                        for (int c=0; c < list.size(); c++) {
                            temp+= list.at(c).split("-").at(0).trimmed()+".";
                        }
                        temp = temp.trimmed();
                        temp.remove(temp.size()-1,1);
                    }
                }
                if(value_header == marca_devuelta || value_header == TIPO_devuelto
                        || value_header == AREALIZAR_devuelta){
                    if(temp.contains("-")){
                        temp = temp.split("-").at(0).trimmed();
                    }
                }
                if(value_header == calibre_real){
                    if(checkIfFieldIsValid(temp)){
                        while(temp.size()<3){
                            temp.prepend("0");
                        }
                    }
                }
                if(value_header == F_INST){
                    QDateTime dt = QDateTime::fromString(temp, formato_fecha_hora);
                    if(dt.isValid()){
                        temp = dt.toString("ddMMyyyy");
                    }
                }
                if(value_header == emplazamiento_devuelto){
                    temp += jsonObject.value(RESTO_EM).toString().trimmed();
                }
                if(value_header == observaciones){
                    QString msgLibre = jsonObject.value(MENSAJE_LIBRE).toString().trimmed();
                    if(checkIfFieldIsValid(msgLibre)){
                        temp += ". "+msgLibre;
                    }
                    temp = temp.toUpper();
                }
                if(!checkIfFieldIsValid(temp)){
                    temp = "";
                }
                xlsx_table_extended.write(row,n+1,temp);
                if(openCSV){
                    if(n == 0){
                        xout << temp.replace(";", ".").replace("\n", " ").replace("\r", " ").replace("\t", " ");
                    }else{
                        xout << "; " <<temp.replace(";", ".").replace("\n", " ").replace("\r", " ").replace("\t", " ");
                    }
                }
            }
            if(openCSV){
                xout << "\n";
            }
            row++;
        }
        xlsx_table_extended.saveAs(rutaToXLSXFile_table);
        if(openCSV){
            fileCSV.flush();
            fileCSV.close();
        }
        xlsx_gestor_independiente.saveAs(rutaToXLSXFile_gestor_independiente);

        //doneTasksToXlsAlredyExported = true;

        GlobalFunctions::showMessage(this,"Éxito","Fichero XLSX de tareas generado");

        if(getCurrentRadioButton() == CERRADA
                || getCurrentRadioButton() == INFORMADA){
            GlobalFunctions gf(this);
            if(gf.showQuestion(this, "Trabajo a Subir", "¿Desea subir este trabajo cerrado al cliente?"
                               , QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){

                show_loading("Comprimiendo Archivo...");
                abrirSeleccionGestores(jsonArray);

                QString path_to_compress = "";
                QStringList compress_files;

                QString gestor_selected = GlobalFunctions::readGestorSelected();

                if(!gestor_selected.isEmpty()){

                    int iteration = 0;

                    while(iteration < jsonArray.size()){
                        iteration = adjuntarFotosParaComprimir(jsonArray, rutaToXLSXFile_gestor_independiente, iteration, path_to_compress);

                        QString file = comprimirArchivos(path_to_compress, gestor_selected);
                        compress_files << file;

                        QDir dir(path_to_compress);
                        dir.removeRecursively();
                    }

                    QString name = gestor_selected + ", " + "trabajo devuelto, " +
                            QDateTime::currentDateTime().toString(formato_fecha_hora_to_file_upload);
                    copyFileToNewDirAndName(rutaToXLSXFile_gestor_independiente, path_to_compress, name + ".xlsx");
                    copyFileToNewDirAndName(rutaToXLSXFile_gestor_independiente.replace(".xlsx", ".dat"), path_to_compress, name + ".dat");
                    copyFileToNewDirAndName(rutaToXLSXFile_gestor_independiente.replace(".xlsx", ".txt"), path_to_compress, name + ".txt");

                    QString file = comprimirArchivos(path_to_compress, gestor_selected);
                    compress_files << file;

                    dir.setPath(path_to_compress);
                    dir.removeRecursively();
                }
                hide_loading();
                if(!gestor_selected.isEmpty()){
                    show_loading("Subiendo archivos comprimidos...");
                    QString compress_file;
                    foreach (compress_file, compress_files) {
                        if(!compress_file.isEmpty()){
                            QFile file(compress_file);
                            if(file.exists()){
                                double size = file.size();
                                size /= 1048576;
                                int limite_archivos_en_servidor_int = limite_archivos_en_servidor.remove("Mb", Qt::CaseInsensitive).trimmed().toInt();

                                if(limite_archivos_en_servidor_int > size){
                                    //                                    if(QMessageBox::information(this, "Archivo comprimido creado", "El archivo tiene un tamaño de "
                                    //                                                                + QString::number(size, 'f', 2)//muestra un lugar despues de la coma
                                    //                                                                +"Mb. El límite es de "+limite_archivos_en_servidor
                                    //                                                                +". Desea subir este archivo?", QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){

                                    if(!sendClientWorkFileToServer(compress_file)){
                                        GlobalFunctions gf(this);
                                        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                                        hide_loading();
                                        return;
                                    }
                                    //                                    }
                                }
                                else{
                                    GlobalFunctions gf(this);
                                    GlobalFunctions::showWarning(this,"Trabajo no subido","El trabajo no fue subido al servidor porque es demasiado grande, exporte menos tareas y vuelva a intentarlo");
                                    hide_loading();
                                    return;
                                }
                            }
                        }else{
                            GlobalFunctions gf(this);
                            GlobalFunctions::showWarning(this,"No se ha subido el trabajo","El gestor no fue seleccionado");
                            hide_loading();
                            return;
                        }
                    }
                    GlobalFunctions::showMessage(this,"Éxito","Subido al servidor correctamente.");
                    hide_loading();
                }
            }
        }


        if(gf.showQuestion(this, "Confirmación","¿Desea abrir carpeta de exportación?",
                           QMessageBox::Ok, QMessageBox::No) == QMessageBox::Ok){
            GlobalFunctions::showInExplorer(dir.path());
        }
    }
}
QString Tabla::selectGestor(){
    show_loading("Seleccionando gestor...");
    Seleccion_Gestor *seleccion_gestor = new Seleccion_Gestor(this, Gestor::getListaNombresGestores(), true);
    connect(seleccion_gestor,SIGNAL(selected_gestor(QString)),this,SLOT(get_gestor_selected(QString)));
    seleccion_gestor->moveCenter();
    if(seleccion_gestor->exec()){
        if(GlobalFunctions::readGestorSelected() != "Todos"){
            QJsonArray jsonArray = Gestor::readGestores();
            for (int i=0; i < jsonArray.size(); i++) {
                QString gestor_v, cod_v;
                gestor_v = jsonArray.at(i).toObject().value(gestor_gestores).toString();
                if(gestor_v == GlobalFunctions::readGestorSelected()){
                    ui->pb_gestor_inicial_seleccionado->setText(gestor_v);
                    Gestor *gestor = new Gestor(nullptr, false, empresa);
                    gestor->setData(jsonArray.at(i).toObject());
                    QPixmap logo = gestor->getPhotoGestor();
                    if(logo != QPixmap()){
                        scalePhoto(logo);
                        ui->pb_logotipo->setPixmap(logo);
                        break;
                    }
                }
            }
        }else{
            ui->pb_logotipo->setText("...");
            ui->pb_gestor_inicial_seleccionado->setText("Todos");
        }
        return GlobalFunctions::readGestorSelected();
    }
    return "";
}
QString Tabla::abrirSeleccionGestores(QJsonArray jsonArray, bool todos){    
    //    QRect rect = this->geometry();
    Seleccion_Gestor *seleccion_gestor = new Seleccion_Gestor(this, Gestor::getListaNombresGestores(), todos);
    connect(seleccion_gestor,SIGNAL(selected_gestor(QString)),this,SLOT(get_gestor_selected(QString)));
    seleccion_gestor->moveCenter();
    if(seleccion_gestor->exec()){
        if(GlobalFunctions::readGestorSelected() != "Todos"){
            jsonArray = Gestor::readGestores();
            for (int i=0; i < jsonArray.size(); i++) {
                QString gestor_v, cod_v;
                gestor_v = jsonArray.at(i).toObject().value(gestor_gestores).toString();
                if(gestor_v == GlobalFunctions::readGestorSelected()){
                    Gestor *gestor = new Gestor(nullptr, false, empresa);
                    gestor->setData(jsonArray.at(i).toObject());
                    QPixmap logo = gestor->getPhotoGestor();
                    if(logo != QPixmap()){
                        scalePhoto(logo);
                        ui->pb_logotipo->setPixmap(logo);
                    }
                }
            }
        }else{
            ui->pb_logotipo->setText("...");
        }
        return GlobalFunctions::readGestorSelected();
    }
    return "";
}
void Tabla::scalePhoto(QPixmap pixmap){
    QSize size = pixmap.size();
    int max_height = 50;
    int max_width = 50;
    double ratio;
    if(size.width() > size.height()){
        ratio = static_cast<double>(size.height())/ static_cast<double>(size.width());
        max_height = max_width * ratio;
    }else{
        ratio = static_cast<double>(size.width())/ static_cast<double>(size.height());
        max_width = max_height * ratio;
    }

    ui->pb_logotipo->setScaledContents(true);
    ui->pb_logotipo->setMaximumSize(max_width, max_height);
}
QString Tabla::comprimirArchivos(QString string_dir, QString current_gestor){
    QDir dir(string_dir);
    dir.cdUp();
    QString name = "/"+current_gestor + ", " + "trabajo devuelto, " +
            QDateTime::currentDateTime().toString(formato_fecha_hora_to_file_upload_ms)+".zip";
    if(!string_dir.isEmpty()){
        if(JlCompress::compressDir(dir.path()+ name /*"/Trabajo_Devuelto.zip"*/, string_dir)){
            //            QMessageBox::information(this, "Correcto", "Compresion exitosa");
            qDebug() << "Correcto Compresion exitosa: " << string_dir;
            return dir.path()+ name/*"/Trabajo_Devuelto.zip"*/;
        }
        else{
            //            QMessageBox::critical(this, "Error", "Compresion no terminada");
            qDebug() << "Error Compresion no terminada:" << string_dir;
            return  "";
        }
    }
    return "";
}

int Tabla::dir_size(const QString _wantedDirPath)
{
    long int sizex = 0;
    QFileInfo str_info(_wantedDirPath);
    if (str_info.isDir())
    {
        QDir dir(_wantedDirPath);
        QStringList ext_list;
        dir.setFilter(QDir::Files | QDir::Dirs |  QDir::Hidden | QDir::NoSymLinks);
        QFileInfoList list = dir.entryInfoList();

        for(int i = 0; i < list.size(); ++i)
        {
            QFileInfo fileInfo = list.at(i);
            if ((fileInfo.fileName() != ".") && (fileInfo.fileName() != ".."))
            {
                sizex += (fileInfo.isDir()) ? this->dir_size(fileInfo.filePath()) : fileInfo.size();
                QApplication::processEvents();
            }
        }
    }

    return sizex;
}
int Tabla::adjuntarFotosParaComprimir(QJsonArray jsonArray, QString dir_file, int iteration, QString &path){
    QDir dir(dir_file), dir_aux;
    dir.cdUp();//va al directorio donde esta el archivo
    QString upperPath = dir.path() + "/Trabajo_Devuelto/";
    dir.setPath(dir.path()+"/Trabajo_Devuelto/fotos_tareas");
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    dir_aux = dir;
    QString gestor_in_array, numAbonado_in_array;
    for (int i = iteration; i < jsonArray.size(); i++) {
        gestor_in_array = jsonArray.at(i).toObject().value(GESTOR).toString().trimmed();
        if(gestor_in_array == GlobalFunctions::readGestorSelected()){
            numAbonado_in_array = jsonArray.at(i).toObject().value(numero_abonado).toString().trimmed();
            if(checkIfFieldIsValid(numAbonado_in_array)){
                dir_aux.setPath(dir.path() + "/" + gestor_in_array + "/" + numAbonado_in_array); ///Creando la direccion para copiar las fotos
                if(!dir_aux.exists()){
                    dir_aux.mkpath(dir_aux.path());
                }
                copyRecursively("C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/"+ gestor_in_array + "/fotos_tareas/" + numAbonado_in_array, dir_aux.path());
            }
        }
        iteration = i+1;
        int size = dir_size(upperPath);
        size /= 1048576;
        if(size > 38){
            break;
        }
    }
    path = upperPath;
    return iteration;
}

void Tabla::copyFileToNewDirAndName(QString file_source, QString destiny_dir, QString new_fileName){
    QDir dir_destiny(destiny_dir);
    if(!dir_destiny.exists()){
        dir_destiny.mkpath(dir_destiny.path());
    }
    if(QFile::exists(file_source)){
        if(QFile::exists(dir_destiny.path()+"/"+new_fileName)){
            QFile::remove(dir_destiny.path()+"/"+new_fileName);
        }
        if(QFile::copy(file_source, dir_destiny.path()+"/"+new_fileName))
            qDebug() << "Copy ok: " << new_fileName;
        else
            qDebug() << "Not Copy:" << new_fileName;
    }
}
bool Tabla::copyRecursively(const QString &srcFilePath, const QString &tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        if (!targetDir.mkpath(QFileInfo(tgtFilePath).fileName()))
            return false;
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames) {
            const QString newSrcFilePath
                    = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                    = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath)){
                return false;
            }
        }
    } else {
        if (!QFile::copy(srcFilePath, tgtFilePath))
            return false;
    }
    return true;
}
void Tabla::copyAllFilesFromDirToDir(const QString source, const QString destiny){

    QDir dir_source(source);
    QDir dir_destiny(destiny);

    QStringList filesList = dir_source.entryList(QDir::AllEntries);
    QString fileName;
    foreach(fileName, filesList)
    {
        if(QFile::exists(dir_destiny.path()+"/"+fileName)){
            QFile::remove(dir_destiny.path()+"/"+fileName);
        }
        if(QFile::copy(dir_source.path()+"/"+fileName, dir_destiny.path()+"/"+fileName))
            qDebug() << "Copy ok: " << fileName;
        else
            qDebug() << "Not Copy:" << fileName;
    }
}

void Tabla::on_pb_logotipo_clicked()
{
    //    QString ruta = QFileDialog::getOpenFileName(this,"Seleccione la foto", QDir::current().path()+"/Logos de empresas", "Imagenes (*.png *.xpm *.jpg)");
    //    if(ruta.isEmpty() || ruta.isNull()){
    //        return;
    //    }
    //    ruta_logotipo = ruta;
    //    ui->pb_logotipo->setIcon(QIcon(ruta));
}
void Tabla::on_actionEquipo_triggered(){
    Equipo_Selection_Screen *seleccionEquipoScreen = new Equipo_Selection_Screen(this, empresa);

    seleccionEquipoScreen->getEquipo_OperariosFromServer();
    connect(seleccionEquipoScreen,SIGNAL(equipoSelected(QString)),this,SLOT(get_equipo_selected(QString)));


    if(seleccionEquipoScreen->exec())
    {
        show_loading("Buscando Resultados...");
        hideAllFilters();
        filtering = true;
        filter_type |= F_POR_EQUIPO;

        QString queryStatus = getQueyStatus();
        QString query = "( (`" + equipo + "` LIKE '%" + equipoName + "%') AND (" + queryStatus + ") )";
        getTareasCustomQuery(query);
        jsonArrayInTableFiltered = jsonArrayAll;

        filter_type = 0;


        fixModelforTable(jsonArrayInTableFiltered);
        setTableView();
        hide_loading();
    }
}
void Tabla::on_actionPor_Operario_triggered() //Ordenar por operario
{
    if(seleccionOperarioScreen ==nullptr){
        seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);
    }
    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,SIGNAL(user(QString)),this,SLOT(get_user_selected(QString)));


    if(seleccionOperarioScreen->exec())
    {
        show_loading("Buscando Resultados...");
        hideAllFilters();
        filtering = true;
        filter_type |= F_POR_OPERARIO;

        QString queryStatus = getQueyStatus();
        QString query = "( (`" + operario + "` LIKE '%" + operatorName + "%') AND (" + queryStatus + ") )";
        getTareasCustomQuery(query);
        jsonArrayInTableFiltered = jsonArrayAll;

        filter_type = 0;


        fixModelforTable(jsonArrayInTableFiltered);
        setTableView();
        hide_loading();
    }
}
QJsonArray Tabla::fillFilterPorEquipo(QJsonArray jsonArray, QString equipoSelected){
    QJsonArray jsonArrayforTable = QJsonArray();
    for(int i=0; i< jsonArray.size(); i++){
        QString equipo_t = jsonArray[i].toObject().value(equipo).toString().trimmed();
        if(equipo_t.contains(equipoSelected)){
            jsonArrayforTable.append(jsonArray[i]);
        }
    }
    return jsonArrayforTable;
}
QJsonArray Tabla::fillFilterPorOperario(QJsonArray jsonArray, QString operarioSelected){
    QJsonArray jsonArrayforTable = QJsonArray();
    for(int i=0; i< jsonArray.size(); i++){
        QString operario_t = jsonArray[i].toObject().value(operario).toString().trimmed();
        if(operario_t.contains(operarioSelected)){
            jsonArrayforTable.append(jsonArray[i]);
        }
    }
    return jsonArrayforTable;
}

void Tabla::on_pb_respaldar_clicked()
{
    QDir dir;
    dir.setPath(QDir::currentPath() +"/Respaldos");
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    QString filename = dir.path()+"/Trabajo_Respaldado_"+ QDateTime::currentDateTime().toString(formato_fecha_hora).
            replace(":","_").replace("-","_")
            +".txt";
    crearFicheroTxtConTabla(jsonArrayAll, filename);
    QString name = crearFicheroDATConTabla(jsonArrayAll, filename);

    if(uploadFileToServer(filename)){
        GlobalFunctions::showMessage(this,"Éxito","Subido al servidor correctamente.");
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
    }

    //    QFileDialog::getOpenFileName(this, "Aqui se ha guardado el trabajo", dir.path(), tr("Texto (*.txt)"));
}

void Tabla::on_actionContactar_triggered()
{
    GlobalFunctions::showMessage(this,"Mi Ruta " + versionMiRuta,"Desarrollado Por\n\nMichel Morales Veranes\nCorreo: mraguascontadores@gmail.com"
                                                                 "\n\nLuis Alejandro Reyes Morales\nCorreo: inglreyesm@gmail.com   ");
}

void Tabla::on_actionTrabajar_sin_conexion_a_Tabla_triggered()
{
    Tutoriales *tutoriales = new Tutoriales();
    tutoriales->show();
}

void Tabla::on_actionGeolocalizar_una_tarea_triggered()
{
    QString file_name = "googleMapsUbicacion.mp4";
    QString pr = QDir::currentPath()+"/videos/"+file_name;
    ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 500, true);
}

void Tabla::on_actionAsignar_campos_comunes_2_triggered()
{
    QString file_name = "asignar_campos_comunes.mp4";
    QString pr = QDir::currentPath()+"/videos/"+file_name;
    ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 500, true);
}

void Tabla::on_actionAsignar_a_un_operario_2_triggered()
{
    QString file_name = "asignar_a_operario.mp4";
    QString pr = QDir::currentPath()+"/videos/"+file_name;
    ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 500, true);
}

void Tabla::on_actionDesde_Fichero_DAT_triggered()
{
    emit importarDAT("buscar_fichero_en_PC");
}

void Tabla::on_pb_buscar_trabajo_clicked()
{
    if(database_comunication::checkInternetConnection()){
        buscarTrabajoDevueltoEnLinea();
    }else{
        QMessageBox::critical(this,"Error de conexión","No hay conexión a internet.");
        ui->statusbar->showMessage("No hay conexión a internet.", 3000);
    }
}

void Tabla::buscarTrabajoDevueltoEnLinea(){  //entra en la funcion getFilesDirSelected y continua alli
    Select_File_to_Download *selection_file_window =
            new Select_File_to_Download(nullptr, GlobalFunctions::readGestorSelected(), empresa);
    connect(selection_file_window,SIGNAL(files_in_return_folder(QStringList)),this,SLOT(getFilesDirSelected(QStringList)));
    selection_file_window->getFilesClientFromServer();
}

void Tabla::getFilesDirSelected(QStringList dirs)
{
    ui->statusbar->showMessage("Descargando Archivos...");
    show_loading("Descargando Archivos...");
    files_download_dir_selected  = dirs;
    if(!files_download_dir_selected.isEmpty()){
        for (int i =0; i < files_download_dir_selected.size(); i++) {
            QString file_i = files_download_dir_selected.at(i);
            if(!file_i.contains("Trabajo", Qt::CaseInsensitive)
                    || !file_i.contains("pendiente", Qt::CaseInsensitive)
                    || !file_i.contains(".zip", Qt::CaseInsensitive)){
                files_download_dir_selected.removeAt(i);
                i--;
            }
        }
        if(!files_download_dir_selected.isEmpty()){
            for (int i= 0; i< files_download_dir_selected.size(); i++) {
                QString file_to_download = files_download_dir_selected.at(i);
                QStringList lista_ficheros_descargados = other_task_screen::readficherosDescargados();
                if(!lista_ficheros_descargados.contains(file_to_download)){
                    QString file_downloaded = descargarArchivoCliente(file_to_download);

                    ui->statusbar->showMessage("Archivo "+file_to_download+" descargado");

                    QStringList archivos = JlCompress::getFileList(file_downloaded);
                    QDir dir(file_downloaded);
                    dir.cdUp();
                    if(!JlCompress::extractFiles(file_downloaded, archivos, dir.path()).isEmpty()){
                        //                 QMessageBox::information(this, "Correcto", "Descompresion exitosa");
                        qDebug()<<"Descompresion exitosa";
                        ui->statusbar->showMessage("Archivo "+file_to_download+" descomprimidos");
                        lista_ficheros_descargados.append(file_to_download);
                        other_task_screen::writeficherosDescargados(lista_ficheros_descargados);

                        delete_file_in_server(file_to_download);

                        if(dir.cd("fotos_tareas")){
                            if(dir.cd(GlobalFunctions::readGestorSelected())){
                                QDir destiny_dir("C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/"+
                                                 GlobalFunctions::readGestorSelected()+"/fotos_tareas");
                                if(!destiny_dir.exists()){
                                    destiny_dir.mkpath(destiny_dir.path());
                                }
                                copyRecursively(dir.path(), destiny_dir.path());
                            }
                        }
                    }
                    else{
                        //                 QMessageBox::critical(this, "Error", "Descompresion no terminada");
                        qDebug()<<"Descompresion no terminada";
                        ui->statusbar->showMessage("Error descomprimiendo archivo");
                    }
                }else{
                    ui->statusbar->showMessage("No existe nuevo trabajo devuelto en el servidor");
                }
            }
        }
        ui->statusbar->showMessage("No existe nuevo trabajo devuelto en el servidor");
    }
    hide_loading();

    if(buscarInformacionNueva()){
        GlobalFunctions gf(this);
        if(gf.showQuestion(this,"Nuevo Trabajo Devuelto","Existe nuevo trabajo para cargar, desea cargarlo?"
                           , QMessageBox::Ok, QMessageBox::No) == QMessageBox::Ok){
            cargarInformacionNueva();
        }
    }
}
bool Tabla::cargarInformacionNueva(){
    QString path = other_task_screen::buscarCarpetaDeTrabajoPendiente();
    if(!path.isEmpty()){
        QJsonArray jsonArray = Gestor::readGestores();
        for (int i=0; i < jsonArray.size(); i++) {
            QString gestor_v, cod_v;
            gestor_v = jsonArray.at(i).toObject().value(gestor_gestores).toString();

            QDir dir(path + gestor_v);
            QStringList names;
            names << "trabajo" << "pendiente" << ".dat";
            QStringList ficheros = other_task_screen::getFilesWithNamesInDir(names, dir);
            QStringList trabajos_cargados = other_task_screen::readTrabajosCargados();
            for (int i=0; i < ficheros.size(); i++) {
                if(!trabajos_cargados.contains(ficheros.at(i))){
                    emit importarDAT(dir.path() + "/" + ficheros.at(i));
                    return true;
                }
            }
        }
    }
    return false;
}

bool Tabla::buscarInformacionNueva(){
    QString path = other_task_screen::buscarCarpetaDeTrabajoPendiente();
    if(!path.isEmpty()){
        QJsonArray jsonArray = Gestor::readGestores();
        for (int i=0; i < jsonArray.size(); i++) {
            QString gestor_v, cod_v;
            gestor_v = jsonArray.at(i).toObject().value(gestor_gestores).toString();

            QDir dir(path + gestor_v);
            QStringList names;
            names << "trabajo" << "pendiente" << ".dat";
            QStringList ficheros = other_task_screen::getFilesWithNamesInDir(names, dir);
            QStringList trabajos_cargados = other_task_screen::readTrabajosCargados();
            for (int i=0; i < ficheros.size(); i++) {
                if(!trabajos_cargados.contains(ficheros.at(i))){
                    //                emit importarDAT(path + ficheros.at(i));
                    return true;
                }
            }
        }
    }
    return false;
}

void Tabla::download_save_work_file_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::LOAD_WORK,keys,values);
}
void Tabla::download_client_work_file_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_CLIENT_WORK,keys,values);
}
QString Tabla::descargarArchivoCliente(QString file){

    file_download_dir_selected = file; //nombre y puedo añadir direccion del archivo a descrgar

    QStringList keys, values;
    keys << "nombre" << "gestor" << "empresa";
    values << file << GlobalFunctions::readGestorSelected() << empresa;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, SLOT(download_client_work_file_request()));

    int res = q->exec();
    switch(res)
    {
    case database_comunication::script_result::timeout:
        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        break;
    case database_comunication::script_result::download_file_ok:
        break;
    case database_comunication::script_result::download_file_failed:
        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de tranferencia con el servidor.");
        break;
    case database_comunication::script_result::download_file_doesnt_exists:
        QMessageBox::critical(this,"Error darchivo no encontrado","No se pudo completar la solucitud porque el archivo no existe el servidor.");
        break;
    }
    delete q;

    return file_download_dir_string; //direccion de archivo descargado
}
QString Tabla::descargarArchivo(QString file){

    file_download_dir_selected = file; //nombre y puedo añadir direccion del archivo a descrgar

    QStringList keys, values;
    keys << "nombre" << "empresa";
    values << file  << empresa;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, SLOT(download_save_work_file_request()));

    int res = q->exec();
    switch(res)
    {
    case database_comunication::script_result::timeout:
        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        break;
    case database_comunication::script_result::download_file_ok:
        break;
    case database_comunication::script_result::download_file_failed:
        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de tranferencia con el servidor.");
        break;
    case database_comunication::script_result::download_file_doesnt_exists:
        QMessageBox::critical(this,"Error darchivo no encontrado","No se pudo completar la solucitud porque el archivo no existe el servidor.");
        break;
    }
    delete q;

    return file_download_dir_string; //direccion de archivo descargado
}

void Tabla::delete_file_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DELETE_FILE,keys,values);
}

bool Tabla::delete_file_in_server(QString file){

    QEventLoop *q = new QEventLoop();

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QStringList keys, values;
    keys << "nombre" << "gestor" << "empresa";
    values << file << GlobalFunctions::readGestorSelected() << empresa;

    this->keys = keys;
    this->values = values;

    QTimer::singleShot(DELAY, this, SLOT(delete_file_request()));

    bool retorno= false;
    int res = q->exec();
    switch(res)
    {
    case database_comunication::script_result::timeout:
        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        break;
    case database_comunication::script_result::delete_file_ok:
        retorno = true;
        break;
    case database_comunication::script_result::delete_file_failed:
        QMessageBox::critical(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de tranferencia con el servidor.");
        break;
    }
    //    delete timer;
    delete q;
    return  retorno;
}

void Tabla::on_pb_gestor_inicial_seleccionado_clicked()
{
    getTareas();
}

void Tabla::on_pb_gestor_inicial_seleccionado_2_clicked()
{
    on_pb_gestor_inicial_seleccionado_clicked();
}

void Tabla::on_actionMarcas_triggered()
{
    emit openMarcasTable();
}


void Tabla::on_actionPiezas_triggered()
{
    emit openPiezasTable();
}

void Tabla::on_actionZonas_triggered()
{
    emit openZonasTable();
}

void Tabla::on_actionObservaciones_triggered()
{
    emit openObservacionesTable();
}

void Tabla::on_actionCausas_triggered()
{
    emit openCausasTable();
}

void Tabla::on_actionResultados_triggered()
{
    emit openResultadosTable();
}

void Tabla::on_actionEmplazamientos_triggered()
{
    emit openEmplazamientosTable();
}
void Tabla::on_actionEquipo_Operarios_triggered()
{
    emit openEquipo_OperariosTable();
}
void Tabla::on_actionClases_triggered()
{
    emit openClasesTable();
}

void Tabla::on_actionTipo_triggered()
{
    emit openTiposTable();
}

void Tabla::on_actionRuedas_triggered()
{
    emit openRuedasTable();
}
void Tabla::on_actionRutas_triggered()
{
    emit openRutasTable();
}
void Tabla::on_actionLongitudes_triggered()
{
    emit openLongitudesTable();
}

void Tabla::on_actionCalibres_triggered()
{
    emit openCalibresTable();
}

void Tabla::on_pb_agregar_tipoTarea_clicked()
{
    int c = ui->widget_filtro_tiposTarea_checkboxes->children().size();
    QString name_anomalia = ui->cb_tipoTarea->currentText();
    for (int i=0; i < c; i++) {
        QString name = ui->widget_filtro_tiposTarea_checkboxes->
                children().at(i)->objectName();
        if(name.contains(name_anomalia)){
            return;
        }
    }

    QCheckBox *cb = new QCheckBox(ui->widget_filtro_tiposTarea_checkboxes);
    cb->setObjectName("cb_" + name_anomalia);
    cb->setText(name_anomalia);
    cb->setChecked(true);
    ui->widget_filtro_tiposTarea_checkboxes->layout()->addWidget(cb);

    add_calibres_to_select(name_anomalia);
}

void Tabla::on_actionInfos_triggered()
{
    emit openInfosTable();
}

void Tabla::on_actionGestores_triggered()
{
    emit openGestoresTable();
}
void Tabla::on_actionEmpresas_triggered()
{
    emit openEmpresasTable();
}
void Tabla::on_actionITACs_triggered()
{
    emit openITACsTable();
}
void Tabla::on_actionAdministradores_triggered()
{
    emit openAdministradoresTable();
}
QJsonArray Tabla::readJsonArrayFromFile(QString filename)
{
    QJsonArray jsonArray;
    QFile *data_base = new QFile(filename); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::ReadOnly))
        {
            QDataStream in(data_base);
            in>>jsonArray;
            data_base->close();
        }
    }
    return jsonArray;
}

void Tabla::facturarTrabajo(QStringList seleccionadas)
{
    QString filename_desde = seleccionadas.first();
    QString filename_hasta = seleccionadas.last();

    QStringList listFiles = Facturacion::getFilesList("dat");
    QStringList listFilestoRead;

    QDir dir = QDir::current();
    dir.setPath(dir.path()+"/Ficheros Informados");
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    for (int i =0; i < seleccionadas.size(); i++) {
        listFilestoRead << (dir.path() + "/GCT" + seleccionadas.at(i)+"_EXTRA.dat");
    }
    QJsonArray jsonArrayFactutacion;
    for (int i =0; i < listFilestoRead.size(); i++) {
        QJsonArray jsonArray = readJsonArrayFromFile(listFilestoRead.at(i));
        for (int i=0; i< jsonArray.size(); i++) {
            jsonArrayFactutacion.append(jsonArray.at(i).toObject());
        }
    }
    fillSheetFacturacion(jsonArrayFactutacion, seleccionadas);
}

void Tabla::on_actionFacturar_triggered()
{
    Facturacion *fact = new Facturacion(this);
    connect(fact, &Facturacion::selectedFacturation, this, &Tabla::facturarTrabajo);
    fact->show();
}

QMap<QString,QString> Tabla::fillMapaFacturacionCABB(){
    QMap<QString,QString> mapa_exp;

    mapa_exp.insert("idOrdenCABB",idOrdenCABB);
    mapa_exp.insert("ANOMALIA", ANOMALIA);
    mapa_exp.insert("ABONADO", numero_abonado);
    mapa_exp.insert("RS",resultado);
    mapa_exp.insert("FechEjecución",F_INST);
    mapa_exp.insert("INDICE",lectura_contador_nuevo);
    mapa_exp.insert("EMPLAZAMIENTO", emplazamiento_devuelto);
    mapa_exp.insert("LECT_LEVANTADO",lectura_actual);
    mapa_exp.insert("OBSERVADV",observaciones_devueltas);
    mapa_exp.insert("CODIGOMOTIVO",observaciones_devueltas);
    mapa_exp.insert("TIPO",TIPO_devuelto);
    mapa_exp.insert("Estado", status_tarea);
    mapa_exp.insert("MARCADV",marca_devuelta);
    mapa_exp.insert("CALIBREDV",calibre_real);
    mapa_exp.insert("RUEDASDV",RUEDASDV);
    mapa_exp.insert("LONGDV",largo_devuelto);
    mapa_exp.insert("PREFIJO DV",CONTADOR_Prefijo_anno_devuelto);
    mapa_exp.insert("SerieDV",numero_serie_contador_devuelto);
    mapa_exp.insert("ArealizarDV", AREALIZAR_devuelta);
    mapa_exp.insert("intervencidv",intervencion_devuelta);
    mapa_exp.insert("FECH_INFORMACION",fech_informacionnew);
    mapa_exp.insert("FECH_CIERRE",FECH_CIERRE);
    mapa_exp.insert("TIPORDEN",TIPORDEN);
    mapa_exp.insert("OPERARIO",operario);
    mapa_exp.insert("observaciones", MENSAJE_LIBRE);
    mapa_exp.insert("TIPOFLUIDO", TIPOFLUIDO_devuelto);
    mapa_exp.insert("tipoRadio",tipoRadio_devuelto);
    mapa_exp.insert("idexport",idexport);
    mapa_exp.insert("marcaR",marcaR);

    return mapa_exp;
}

QString Tabla::fillSheetFacturacion(QJsonArray jsonArray, QStringList seleccionadas)
{
    QString filename_desde = seleccionadas.first();
    QString filename_hasta = seleccionadas.last();

    QString prevdir = setDirExpToExplorer();
    QDir dir(prevdir);
    dir.cdUp();
    dir.setPath(dir.path()+"/Ficheros Facturados");
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    prevdir = dir.path();
    QString rutaToXLSXFile = prevdir +"/GCTF_"+ filename_desde +"_"+ filename_hasta +".xlsx";

    if(!rutaToXLSXFile.isNull() && !rutaToXLSXFile.isEmpty())
    {
        QMap<QString,QString> mapa_exp = fillMapaFacturacionCABB();
        QStringList listHeaders;
        listHeaders <<"idOrdenCABB" << "ANOMALIA" << "ABONADO" << "RS" << "FechEjecución" << "INDICE" << "EMPLAZAMIENTO"
                   << "LECT_LEVANTADO" << "OBSERVADV" << "TIPO" << "Estado" << "MARCADV" << "CALIBREDV" << "RUEDASDV"
                   << "LONGDV" << "PREFIJO DV" << "SerieDV" << "ArealizarDV" << "intervencidv" << "FECH_INFORMACION"
                   << "FECH_CIERRE" << "TIPORDEN" << "OPERARIO" << "observaciones" << "TIPOFLUIDO" << "tipoRadio"
                   << "idexport" << "marcaR";

        int rows = jsonArray.count();
        QXlsx::Document xlsx;
        xlsx.addSheet("FACTURACION");
        xlsx.selectSheet("FACTURACION");
        //write headers
        for (int i=0; i<listHeaders.count(); i++)
        {
            Format f;
            xlsx.write(1, i+1, listHeaders[i]/*.toUpper()*/);

        }
        //PAra gestor Geconta
        //write data
        QString temp;
        for(int i = 0, row=2; i < rows; i++)
        {
            for(int n=0; n < listHeaders.count(); n++){

                QString header = listHeaders.at(n);
                QString value_header = mapa_exp.value(listHeaders.at(n));
                if((header == "Estado") || (header == "TIPORDEN")){
                    QString value = jsonArray[i].toObject().value(mapa_exp.value(listHeaders.at(n))).toString();
                    temp = value.left(1);
                }
                else if(header.contains("Fech", Qt::CaseInsensitive)){
                    QString value = jsonArray[i].toObject().value(mapa_exp.value(listHeaders.at(n))).toString();
                    QDateTime date = QDateTime::fromString(value, formato_fecha_hora);
                    temp = date.toString(formato_fecha_hora_new_view);
                }
                else if(header == "SerieDV"){
                    QString prefijo = jsonArray[i].toObject().value(CONTADOR_Prefijo_anno_devuelto).toString();
                    QString value = jsonArray[i].toObject().value(numero_serie_contador_devuelto).toString().trimmed();
                    if(!checkIfFieldIsValid(prefijo)){
                        prefijo = eliminarNumerosAlFinal(value).trimmed();
                    }
                    if(!prefijo.isEmpty() &&
                            (value.mid(0, prefijo.size()) == prefijo)){
                        value = value.remove(0, prefijo.size()).trimmed();
                    }
                    temp = value;
                }
                else if(header == "PREFIJO DV"){
                    QString prefijo = jsonArray[i].toObject().value(CONTADOR_Prefijo_anno_devuelto).toString();
                    QString serie = jsonArray[i].toObject().value(numero_serie_contador_devuelto).toString().trimmed();
                    if(!checkIfFieldIsValid(prefijo)){
                        prefijo = eliminarNumerosAlFinal(serie).trimmed();
                    }
                    temp = prefijo;
                }
                else if((value_header != observaciones_devueltas)){
                    QString value = jsonArray[i].toObject().value(mapa_exp.value(listHeaders.at(n))).toString();
                    if(value.contains(" - ")){
                        value = value.split(" - ").at(0).trimmed();
                    }
                    temp = value;
                }
                if(value_header == observaciones_devueltas){
                    if(checkIfFieldIsValid(temp)){
                        QStringList list;
                        list = temp.split("\n");
                        temp = "";
                        for (int c=0; c < list.size(); c++) {
                            temp+= list.at(c).split("-").at(0).trimmed()+".";
                        }
                        temp = temp.trimmed();
                        temp.remove(temp.size()-1,1);
                    }
                }


                xlsx.write(row,n+1,temp);
            }
            row++;

        }
        xlsx.saveAs(rutaToXLSXFile);
        GlobalFunctions::showMessage(this,"Éxito","Fichero XLSX de facturación generado");
    }
    return prevdir;
}


QString Tabla::eliminarCharacteresAlFinal(QString string){
    for(int n = string.size()-1; n >= 0; n--) {
        if(!string.at(n).isDigit()){
            string.remove(n, 1);
        }else{
            break;
        }
    }
    return string;
}



//Nuevo --------------------------------------------------------------------------------------------------
void Tabla::on_pb_open_menu_clicked()
{
    hideMenu();
    ui->widget_menu->showWithAnimation();
}

void Tabla::hideMenu(const QString from){
    ui->pb_buscar->hideChilds();

    if(from.isEmpty()){
        ui->widget_menu->hideMenu();
        ui->l_archivo->hideChilds();
        ui->l_ordenar->hideChilds();
        ui->l_tareas->hideChilds();
        ui->l_ayuda->hideChilds();
        ui->l_tablas->hideChilds();
    }else{
        if(!from.contains("l_archivo")){
            ui->l_archivo->hideChilds();
        }
        if(!from.contains("l_ordenar")){
            ui->l_ordenar->hideChilds();
        }
        if(!from.contains("l_tareas")){
            ui->l_tareas->hideChilds();
        }
        if(!from.contains("l_ayuda")){
            ui->l_ayuda->hideChilds();
        }
        if(!from.contains("l_tablas")){
            ui->l_tablas->hideChilds();
        }
    }
}

void Tabla::on_pb_close_clicked()
{
    this->close();
}
void Tabla::on_pb_maximize_clicked()
{
    //    QPoint pos = this->pos();
    if(!this->isMaximized()){//punto en que deberia estar maximizado
        this->showMaximized();
    }else{
        //        this->setFixedSize(1300, 750);
        this->showNormal();
    }
}
void Tabla::on_pb_minimize_clicked()
{
    this->showMinimized();
}


void Tabla::on_drag_screen()
{
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

    }
}
void Tabla::on_drag_screen_released()
{
    if(isFullScreen()){
        return;
    }
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
    //current_win_Pos = QPoint(this->pos().x()-200,this->pos().y()-200);
}
void Tabla::on_start_moving_screen_timeout()
{
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
bool Tabla::checkGestor(QString gestor){//devuelve true si el gestor es valido
    if(!checkIfFieldIsValid(gestor) || gestor.toLower() == "todos"
            || gestor.toLower() == "sin_gestor"
            || gestor.toLower() == "sin gestor"){
        return false;
    }
    return true;
}
void Tabla::onActionPress(QString action){
    emit hideMenuFast("");

    //Menu Archivo-------------------------------------------------------------------------------------------------------
    if(action == "l_importar_excel_diarias"){
        if(!checkGestor(GlobalFunctions::readGestorSelected())){
            GlobalFunctions::showMessage(this, "Sin Gestor", "Debe seleccionar gestor para importar tareas");
            return;
        }
        emit importarExcel("DIARIAS");
    }
    else if(action == "l_importar_dat_diarias"){
        if(!checkGestor(GlobalFunctions::readGestorSelected())){
            GlobalFunctions::showMessage(this, "Sin Gestor", "Debe seleccionar gestor para importar tareas");
            return;
        }
        emit importarDAT("DIARIAS");
    }
    else if(action == "l_importar_excel_masivas"){
        if(!checkGestor(GlobalFunctions::readGestorSelected())){
            GlobalFunctions::showMessage(this, "Sin Gestor", "Debe seleccionar gestor para importar tareas");
            return;
        }
        emit importarExcel("MASIVAS");
    }
    else if(action == "l_importar_dat_masivas"){
        if(!checkGestor(GlobalFunctions::readGestorSelected())){
            GlobalFunctions::showMessage(this, "Sin Gestor", "Debe seleccionar gestor para importar tareas");
            return;
        }
        emit importarDAT("MASIVAS");
    }
    else if(action == "l_importar_excel_especiales"){
        if(!checkGestor(GlobalFunctions::readGestorSelected())){
            GlobalFunctions::showMessage(this, "Sin Gestor", "Debe seleccionar gestor para importar tareas");
            return;
        }
        emit importarExcel("ESPECIALES");
    }
    else if(action == "l_importar_dat_especiales"){
        if(!checkGestor(GlobalFunctions::readGestorSelected())){
            GlobalFunctions::showMessage(this, "Sin Gestor", "Debe seleccionar gestor para importar tareas");
            return;
        }
        emit importarDAT("ESPECIALES");
    }
    else if(action == "l_informar_diarias"){
        selection_Order = "DIARIAS";
        informarTareas("DIARIAS");
    }
    else if(action == "l_informar_masivas"){
        selection_Order = "MASIVAS";
        informarTareas("MASIVAS");
    }
    else if(action == "l_informar_especiales"){
        selection_Order = "ESPECIALES";
        informarTareas("ESPECIALES");
    }
    else if(action == "l_facturar"){
        on_actionFacturar_triggered();
    }
    else if(action == "l_cargar_de_servidor"){
        on_actionCargar_Trabajo_Salvado_triggered();
    }
    else if(action == "l_cargar_txt"){
        on_actionDesdde_Fichero_TXT_triggered();
    }
    else if(action == "l_cargar_dat"){
        on_actionDesde_Fichero_DAT_triggered();
    }
    else if(action == "l_subir_trabajo"){
        on_actionSubir_Trabajo_a_Servidor_triggered();
    }
    else if(action == "l_respaldar"){
        on_pb_respaldar_clicked();
    }
    //end Menu Archivo-------------------------------------------------------------------------------------------------------

    //Menu Tablas------------------------------------------------------------------------------------------------------------
    else if(action == "l_contadores"){
        on_actionContadores_triggered();
    }
    else if(action == "l_marcas"){
        on_actionMarcas_triggered();
    }
    else if(action == "l_clases"){
        on_actionClases_triggered();
    }
    else if(action == "l_equipo_operarios"){
        on_actionEquipo_Operarios_triggered();
    }
    else if(action == "l_tipos"){
        on_actionTipo_triggered();
    }
    else if(action == "l_calibres"){
        on_actionCalibres_triggered();
    }
    else if(action == "l_longitudes"){
        on_actionLongitudes_triggered();
    }
    else if(action == "l_ruedas"){
        on_actionRuedas_triggered();
    }
    else if(action == "l_rutas"){
        on_actionRutas_triggered();
    }
    else if(action == "l_operarios"){
        on_actionOperarios_triggered();
    }
    else if(action == "l_emplazamiento"){
        on_actionEmplazamientos_triggered();
    }
    else if(action == "l_zonas"){
        on_actionZonas_triggered();
    }
    else if(action == "l_observaciones"){
        on_actionObservaciones_triggered();
    }
    else if(action == "l_resultados"){
        on_actionResultados_triggered();
    }
    else if(action == "l_causas"){
        on_actionCausas_triggered();
    }
    else if(action == "l_piezas"){
        on_actionPiezas_triggered();
    }
    else if(action == "l_infos"){
        on_actionInfos_triggered();
    }
    else if(action == "l_gestores"){
        on_actionGestores_triggered();
    }
    else if(action == "l_empresas"){
        on_actionEmpresas_triggered();
    }
    else if(action == "l_administradores"){
        on_actionAdministradores_triggered();
    }
    else if(action == "l_itacs"){
        on_actionITACs_triggered();
    }
    else if(action == "l_clientes"){
        Screen_Table_Clientes *clientes = new Screen_Table_Clientes(nullptr, true, empresa);
        connect(clientes, &Screen_Table_Clientes::closing, clientes, &Screen_Table_Clientes::deleteLater);
        clientes->show();
    }
    //end Menu Tablas-------------------------------------------------------------------------------------------------------

    //Menu Ayuda----------------------------------------------------------------------------------------------------------
    else if(action == "l_contactar"){
        QString link = "http://mraguas.com/";//el valor /*GTvVlcSKjRFJlPwmRNMHnPsKPZldpQhZkLVHlFVXMqRKQlMxHZGnKpkGwfcLTjThvpJLsfRqnmBbh*/ parece que cambia cada vez que abro
        QDesktopServices::openUrl(QUrl(link));
        ui->statusbar->showMessage("Abriento sitio...", 3000);
    }
    else if(action == "l_acercade"){
        on_actionContactar_triggered();
    }
    else if(action == "l_tuto_sin_conexion"){
        on_actionTrabajar_sin_conexion_a_Tabla_triggered();
    }
    else if(action == "l_tuto_asignar_FNT"){
        on_actionAsignar_a_un_operario_2_triggered();
    }
    else if(action == "l_tuto_asignar_campos"){
        on_actionAsignar_campos_comunes_2_triggered();
    }
    else if(action == "l_tuto_google_map"){
        on_actionGeolocalizar_una_tarea_triggered();
    }
    //end Menu Ayuda-------------------------------------------------------------------------------------------------------

    //Menu Tareas----------------------------------------------------------------------------------------------------------
    else if(action == "l_asignar_campos"){
        on_actionAsignar_campos_comunes_triggered();
    }
    else if(action == "l_nueva_tarea"){
        on_actionNueva_Tarea_triggered();
    }
    else if(action == "l_asignar_FNT"){
        on_actionAsignar_a_un_operario_triggered();
    }
    //end Menu Tareas----------------------------------------------------------------------------------------------------------


    //Menu Ordenar----------------------------------------------------------------------------------------------------------
    else if(action == "l_ordenar_fecha_modificacion"){
        on_actionDe_Modificacion_triggered();
    }
    else if(action == "l_ordenar_fecha_importacion"){
        on_actionDe_Importaci_n_triggered();
    }
    else if(action == "ordenar_fecha_cita"){
        on_actionDe_Cita_2_triggered();
    }
    else if(action == "ordenar_fecha_ejecucion"){
        on_actionDe_Ejecuci_n_triggered();
    }
    else if(action == "ordenar_fecha_cierre"){
        on_actionDe_Cierre_triggered();
    }
    else if(action == "ordenar_fecha_informe"){
        on_actionDe_Informe_triggered();
    }
    else if(action == "l_ubicacion_bateria"){
        on_actionPor_Ubicacion_en_Bater_a_triggered();
    }
    //end Menu Ordenar----------------------------------------------------------------------------------------------------------


    //Menu Filtrar----------------------------------------------------------------------------------------------------------
    else if(action == "l_sin_revisar"){
        on_actionCambios_sin_Revisar_triggered();
    }
    else if(action == "l_filtrar_fecha_modificacion"){
        on_actionDe_Modificacion_2_triggered();
    }
    else if(action == "l_filtrar_fecha_importacion"){
        on_actionImportacion_triggered();
    }
    else if(action == "l_filtrar_fecha_cita"){
        on_actionDe_Cita_triggered();
    }
    else if(action == "l_filtrar_fecha_ejecucion"){
        on_actionEjecucion_triggered();
    }
    else if(action == "l_filtrar_fecha_cierre"){
        on_actionCierre_triggered();
    }
    else if(action == "l_filtrar_fecha_informe"){
        on_actionInformada_triggered();
    }
    else if(action == "l_ubicacion_bateria"){
        on_actionPor_Ubicacion_en_Bater_a_triggered();
    }
    else if(action == "l_direccion"){
        filter_enabled = true;
        on_actionDireccion_triggered();
    }
    else if(action == "l_tipo_orden"){
        on_actionTipo_Orden_triggered();
    }
    else if(action == "l_tipo_tarea"){
        on_actionTipo_de_Tarea_triggered();
    }
    else if(action == "l_numero_abonado"){
        on_actionN_Abonado_triggered();
    }
    else if(action == "l_titular"){
        on_actionTitular_triggered();
    }
    else if(action == "l_numero_serie"){
        on_actionN_Serie_triggered();
    }
    else if(action == "l_equipo"){
        on_actionEquipo_triggered();
    }
    else if(action == "l_zona"){
        on_actionZona_triggered();
    }
    else if(action == "l_geolocalizacion"){
        on_actionC_Geolocalizaci_n_triggered();
    }
    else if(action == "l_en_bateria"){
        on_actionContadores_en_Bater_a_triggered();
    }
    else if(action == "l_unitarios"){
        on_actionContadores_Unitarios_triggered();
    }
    else if(action == "l_fontanero"){
        on_actionPor_Operario_triggered();
    }
    else if(action == "l_resultado"){
        on_actionPor_Resultado_triggered();
    }
    else if(action == "l_prioridad"){
        on_actionFiltrarPrioridad_triggered();
    }
    //end Menu Filtrar----------------------------------------------------------------------------------------------------------

    return;
}
void Tabla::seleccionarInicial()
{
    ui->rb_abierta->setChecked(true);
    on_rb_abierta_clicked();
}

void Tabla::mouseReleaseEvent(QMouseEvent *event) ///al reimplementar esta funcion deja de funcionar el evento pressed
{
    emit mouse_Release();
    QWidget::mouseReleaseEvent(event);
}
void Tabla::rightClikedTable(QMouseEvent *event)
{
    Q_UNUSED(event);
    if( ui->tableView->selectionModel()->selectedRows().count() == 0){
        on_pb_open_menu_clicked();
    }
}
void Tabla::leftClikedTable(QMouseEvent *event)
{
    Q_UNUSED(event);
    hideMenu();
}
void Tabla::mousePressEvent(QMouseEvent *event)
{
    emit mouse_pressed();

    if(QApplication::mouseButtons()==Qt::RightButton){
        //if( ui->tableView->selectionModel()->selectedRows().count() == 0){
        on_pb_open_menu_clicked();
        //}
    }
    else if(QApplication::mouseButtons()==Qt::LeftButton){
        hideMenu();
    }
    QWidget::mousePressEvent(event);
}
void Tabla::mouseDoubleClickEvent(QMouseEvent *event)
{
    on_pb_maximize_clicked();
    QWidget::mouseDoubleClickEvent(event);
}
void Tabla::mouseMoveEvent(QMouseEvent *event)
{
    ui->statusbar->showMessage(QString::number(event->pos().x()));
    //    if(event->x() < 10){
    //        this->on_pb_open_menu_clicked();
    //    }
    QWidget::mouseMoveEvent(event);
}

void Tabla::keyPressEvent(QKeyEvent *event)
{
    int key =  event->key();
    if(key == Qt::Key_Minus){
        ui->statusbar->showMessage("---");
        QFont font = ui->tableView->font();
        int pointSize = font.pointSize();
        pointSize--;
        font.setPointSize(pointSize);
        ui->tableView->setFont(font);
        setTableView();
    } else if(key == Qt::Key_Plus){
        ui->statusbar->showMessage("---");
        QFont font = ui->tableView->font();
        int pointSize = font.pointSize();
        pointSize++;
        font.setPointSize(pointSize);
        ui->tableView->setFont(font);
        setTableView();
    }
    else{
        //        if(!lastSectionCliked.isEmpty() && !lastSectionField.isEmpty()
        //                && key != Qt::Key_Shift && key != Qt::Key_Control){

        //            ui->le_a_filtrar->setFocus();

        //            filter_type |= F_SECTION;

        //            hideAllFilters();
        //            QJsonArray jsonArray = getCurrentJsonArrayInTable();
        //            ui->l_tipo_filtro->setText(lastSectionCliked + ":");
        //            QStringList sections;
        //            for(int i=0; i< jsonArray.size(); i++){
        //                QString section_value = jsonArray[i].toObject().value(lastSectionField).toString().trimmed();
        //                if(section_value.contains(ui->le_a_filtrar->text(), Qt::CaseInsensitive) && !sections.contains(section_value)){
        //                    sections<<section_value;
        //                }
        //            }
        //            completer = new QCompleter(sections, this);
        //            completer->setCaseSensitivity(Qt::CaseInsensitive);
        //            completer->setFilterMode(Qt::MatchContains);
        //            completer->setMaxVisibleItems(10);
        //            ui->le_a_filtrar->setCompleter(completer);

        //            ui->widget_filtro_lineEdit->show();
        //            if(key == Qt::Key_Back){

        //            }else if(event->text().size() == 1 && (event->text().at(0).isDigit() || event->text().at(0).isLetter())){
        //                if(ui->le_a_filtrar->text().isEmpty()){
        //                    ui->le_a_filtrar->setText(ui->le_a_filtrar->text() + event->text());
        //                }
        //            }

        //            ui->widget_filtros->show();
        //        }
    }
    QWidget::keyPressEvent(event);
}


void Tabla::uncheckAllRadioButtons(int state_marked){
    emit hideMenuFast("");
    if(state_marked != TODAS){
        ui->rb_todas->setChecked(false);
    }
    if(state_marked != ABIERTAS){
        ui->rb_abierta->setChecked(false);
    }
    if(state_marked != CITA){
        ui->rb_citas->setChecked(false);
    }
    if(state_marked != AUSENTE){
        ui->rb_ausente->setChecked(false);
    }
    if(state_marked != EJECUTADA){
        ui->rb_ejecutada->setChecked(false);
    }
    if(state_marked != CERRADA){
        ui->rb_cerrada->setChecked(false);
    }
    if(state_marked != INFORMADA){
        ui->rb_informada->setChecked(false);
    }
    if(state_marked != REQUERIDA){
        ui->rb_requerida->setChecked(false);
    }


}

void Tabla::on_pb_buscar_clicked()
{
    emit hideMenuFast("");
}

void Tabla::on_le_a_filtrar_returnPressed()
{
    on_pb_filtrar_clicked();
}
//End Nuevo -----------------------------------------------------------------------------------------

void Tabla::update_itacs_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_ITAC_FIELDS,keys,values);
}


bool Tabla::updateITACs(QStringList lista_cod_emplazamientos, QJsonObject campos){

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

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Tabla::update_itacs_fields_request);

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

bool Tabla::update_fields(QStringList numeros_internos_list, QJsonObject campos){
    QJsonObject numeros_internos;
    for (int i=0; i < numeros_internos_list.size(); i++) {
        numeros_internos.insert(QString::number(i),
                                numeros_internos_list.at(i));
    }
    if(numeros_internos.isEmpty() || campos.isEmpty()){
        return false;
    }
    QStringList keys, values;
    QJsonDocument d;
    d.setObject(numeros_internos);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp_fields, temp_numins = QString::fromUtf8(ba);

    d.setObject(campos);
    ba = d.toJson(QJsonDocument::Compact);
    temp_fields = QString::fromUtf8(ba);

    keys << "json_numin" << "json_fields" << "empresa";
    values << temp_numins << temp_fields << empresa.toLower();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Tabla::update_tareas_fields_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::ok:
        //        GlobalFunctions::showMessage(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_tareas_fields_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}
void Tabla::update_tareas_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA_FIELDS,keys,values);
}
void Tabla::get_tareas_informadas_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_TAREAS_INFORMADAS,keys,values);
}
void Tabla::get_tareas_amount_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_TAREAS_AMOUNT,keys,values);
}
void Tabla::get_tareas_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_TAREAS_CUSTOM_QUERY,keys,values);
}
void Tabla::get_all_column_values_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES,keys,values);
}
void Tabla::get_all_column_values_custom_query_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_COLUMN_VALUES_CUSTOM_QUERY,keys,values);
}

bool Tabla::get_tareas_informadas(){

    QStringList keys, values;

    keys << "empresa";
    values << empresa.toLower();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Tabla::get_tareas_informadas_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::ok:
        //        GlobalFunctions::showMessage(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::get_tareas_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}
void Tabla::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    //    GlobalFunctions gf(this);
    //        GlobalFunctions::showWarning(this,"Éxito","Entro: tipo -> "+ QString::number(tipo)+"\nRespuesta: "+ respuesta);

    if(tipo == database_comunication::UPDATE_TAREA_FIELDS)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success update_tarea_fields"))
        {
            result = database_comunication::script_result::update_tareas_fields_to_server_failed;
        }
        else
        {
            if(ba.contains("success ok update_tarea_fields"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_ITAC_FIELDS)
    {
        qDebug()<<ba;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success update_itac_fields"))
        {
            result = database_comunication::script_result::update_itacs_fields_to_server_failed;
        }
        else
        {
            if(ba.contains("success ok update_itac_fields"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    else if(tipo == database_comunication::GET_ALL_COLUMN_VALUES_CUSTOM_QUERY){
        jsonObjectValues = database_comunication::getJsonObject(ba);
        result = database_comunication::script_result::ok;
    }
    else if(tipo == database_comunication::GET_ALL_COLUMN_VALUES)
    {
        jsonObjectValues = database_comunication::getJsonObject(ba);
        result = database_comunication::script_result::ok;
    }
    else if(tipo == database_comunication::GET_TAREAS_AMOUNT)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(ba.contains("ot success get_tareas_amount_custom_query"))
        {
            qDebug()<<ba;
            result = database_comunication::script_result::get_tareas_amount_failed;
        }
        else {
            jsonInfoTareasAmount = database_comunication::getJsonObject(ba);
            qDebug()<<"query return -> "<<jsonInfoTareasAmount.value("query").toString();
            QString count_tareas = jsonInfoTareasAmount.value("count_tareas").toString();
            countTareas = count_tareas.toInt();
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::GET_TAREAS_CUSTOM_QUERY)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(ba.contains("ot success get_tareas_with_limit_custom_query"))
        {
            qDebug()<<ba;
            result = database_comunication::script_result::get_tareas_custom_query_failed;
        }
        else {
            jsonArrayAll = database_comunication::getJsonArray(ba);
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::DELETE_FILE)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(ba.contains("not success delete_file"))
        {
            qDebug()<<ba;
            result = database_comunication::script_result::delete_file_failed;
        }
        else if(ba.contains("success delete_file")){
            result = database_comunication::script_result::delete_file_ok;
        }
    }
    else if(tipo == database_comunication::SEND_CLIENT_WORK)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(ba.contains("pload not success send_client_work") || ba.isEmpty())
        {
            qDebug()<<ba;
            result = database_comunication::script_result::upload_file_failed;
        }
        else if(ba.contains("pload success send_client_work")){
            result = database_comunication::script_result::upload_file_ok;
        }
    }
    else if(tipo == database_comunication::GET_CLIENT_WORK)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(ba.contains("not success get_client_work") || ba.isEmpty())
        {
            if(ba.contains("no se pudo obtener archivo"))
                //                emit script_excecution_result(database_comunication::script_result::download_task_image_failed);
                result = database_comunication::script_result::download_file_failed;
            else if(ba.contains("no existe archivo"))
                //                emit script_excecution_result(database_comunication::script_result::download_task_image_picture_doesnt_exists);
                result = database_comunication::script_result::download_file_doesnt_exists;
        }
        else{
            QString str = QString::fromUtf8(ba).replace("\n","");
            QByteArray decodeData = QByteArray::fromBase64(str.toUtf8());
            QDir dir;
            dir.setPath(QDir::currentPath()+"/Trabajo Descargado/"+GlobalFunctions::readGestorSelected());
            if(!dir.exists()){
                dir.mkpath(dir.path());
            }
            QString file_name = dir.path() +"/" + file_download_dir_selected;
            QFile file(file_name);
            if(file.open(QIODevice::WriteOnly)){
                file.write(decodeData);
                file.close();
            }
            result = database_comunication::script_result::download_file_ok;
            file_download_dir_string = file_name;
        }
    }
    else if(tipo == database_comunication::SAVE_WORK)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(ba.contains("pload not success save_work") || ba.isEmpty())
        {
            qDebug()<<ba;
            result = database_comunication::script_result::upload_file_failed;
        }
        else if(ba.contains("pload success save_work")){
            result = database_comunication::script_result::upload_file_ok;
        }
    }
    else if(tipo == database_comunication::LOAD_WORK)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        if(ba.contains("not success load_work") || ba.isEmpty())
        {
            if(ba.contains("no se pudo obtener archivo"))
                //                emit script_excecution_result(database_comunication::script_result::download_task_image_failed);
                result = database_comunication::script_result::download_file_failed;
            else if(ba.contains("no existe archivo"))
                //                emit script_excecution_result(database_comunication::script_result::download_task_image_picture_doesnt_exists);
                result = database_comunication::script_result::download_file_doesnt_exists;
        }
        else{
            QString str = QString::fromUtf8(ba).replace("\n","");
            QByteArray decodeData = QByteArray::fromBase64(str.toUtf8());
            QDir dir;
            dir.setPath(QDir::currentPath()+"/Trabajo Descargado/Trabajo de Empresa/");
            if(!dir.exists()){
                dir.mkpath(dir.path());
            }
            QString file_name = dir.path() +"/" + file_download_dir_selected;
            QFile file(file_name);
            if(file.open(QIODevice::WriteOnly)){
                file.write(decodeData);
                file.close();
            }
            result = database_comunication::script_result::download_file_ok;
            file_download_dir_string = file_name;
        }
    }
    //    else if(tipo == database_comunication::GET_TAREAS_INFORMADAS)
    //    {
    //        //        qDebug()<<respuesta;
    //        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

    //        ba.remove(0,2);
    //        ba.chop(2);

    //        if(ba.contains("ot success get_tareas_informadas"))
    //        {
    //            result = database_comunication::script_result::get_tareas_failed;
    //        }
    //        else
    //        {
    //            jsonArrayInTable = filtroInicialDeGestores(database_comunication::getJsonArray(ba), false);
    //            jsonArrayAllInformadas = jsonArrayInTable;
    //            fixModelforTable(jsonArrayInTable);
    //            setTableView();
    //            result = database_comunication::script_result::ok;
    //        }
    //    }
    emit script_excecution_result(result);
}

void Tabla::setInformadas(QJsonArray jsonArray)
{
    jsonArrayAllInformadas = jsonArray;
}

bool Tabla::getTareasFromServer(QString empresa, QString query, int limit, int id_start)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT" << "id_start";
    values << empresa.toLower() << query << QString::number(limit) << QString::number(id_start);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Tabla::get_tareas_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::get_tareas_custom_query_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}
bool Tabla::getTareasAmountFromServer(QString empresa, QString query, int limit)
{
    QStringList keys, values;

    keys << "empresa" << "query" << "LIMIT";
    values << empresa.toLower() << query << QString::number(limit);

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &Tabla::get_tareas_amount_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::ok:
        //        GlobalFunctions::showMessage(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::get_tareas_amount_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}

bool Tabla::getTareasValuesFieldServer(QString empresa, QString column)
{
    bool res = false;
    QStringList keys, values;

    keys << "empresa" << "columna" << "tabla";
    values << empresa << column << "tareas";

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, SLOT(get_all_column_values_request()));

    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;
    }
    delete q;
    return res;
}
bool Tabla::getTareasValuesFieldCustomQueryServer(QString empresa, QString column, QString query)
{
    bool res = false;
    QStringList keys, values;

    keys << "empresa" << "columna" << "tabla" << "query";
    values << empresa << column << "tareas" << query;

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();
    connect(this, &Tabla::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, SLOT(get_all_column_values_custom_query_request()));

    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;
    }
    delete q;
    return res;
}

void Tabla::fillValuesInLineEditToFilter(){
    QStringList values;
    QStringList keys = jsonObjectValues.keys();
    for (int i= 0; i < keys.size(); i++) {
        QString key = keys.at(i);
        if(!key.contains("query") && !key.contains("count_values")){
            QString value = jsonObjectValues.value(key).toString();
            if(!values.contains(value, Qt::CaseInsensitive) && checkIfFieldIsValid(value)){
                values << value;
            }
        }
    }
    QCompleter *completer = new QCompleter(values, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_a_filtrar->setCompleter(completer);
}



bool Tabla::checkValidJsonObjectFields(QJsonObject jsonObject){
    if(!checkIfFieldIsValid(jsonObject.value(idOrdenCABB).toString())
            || !checkIfFieldIsValid(jsonObject.value(numero_interno).toString())
            || !checkIfFieldIsValid(jsonObject.value(numero_abonado).toString())){
        return false;
    }
    return true;
}
QMap<QString, QString> Tabla::mapExcelImport(QStringList listHeaders){
    Q_UNUSED(listHeaders);
    QMap<QString, QString> map;
    //campos de excel de entrada
    map.insert("IDORDENCABB",idOrdenCABB);
    map.insert("NUMIN",numero_interno);
    map.insert("ABONADO",numero_abonado);

    return map;
}
QJsonArray Tabla::loadIDOrdenesExcel(){
    QString path = QFileDialog::getOpenFileName(this, "Seleccione excel con rutas", "", "Datos (*.xlsx *.xls)");

    if(path.isEmpty()){
        return QJsonArray();
    }
    QXlsx::Document xlsx(path);
    QString sheetName="";
    QJsonArray jsonArray;
    foreach(sheetName, xlsx.sheetNames()){
        xlsx.selectSheet(sheetName);
        QStringList listHeaders, row_content;
        QList<QStringList> lista;
        //        QFile file(path);
        //        file.open(QIODevice::ReadOnly);
        int number_of_row = xlsx.dimension().lastRow();
        int number_of_column = xlsx.dimension().lastColumn();

        //read headers
        ///Limitar el numero de headers 26 es la cantidad de columnas actual del excel
        //        if(number_of_column > 36){
        //            number_of_column = 36;
        //        }
        for (int i=1; i<= number_of_column; i++)
        {
            if( xlsx.cellAt(1,i) != nullptr){
                QVariant value = xlsx.cellAt(1,i)->value();
                if(value!=0){
                    QString header = value.toString().trimmed();
                    if(checkIfFieldIsValid(header)){
                        listHeaders << header;
                    }
                }
            }else{
                break;
            }
        }
        number_of_column = listHeaders.size();
        QMap<QString, QString> map = mapExcelImport(listHeaders);
        //read data
        for(int i = 1; i <= number_of_row; i++)
        {
            row_content.clear();
            for(int j = 1; j <= number_of_column; j++)
            {
                if(xlsx.cellAt(i,j) != nullptr)
                {
                    Cell *cell = xlsx.cellAt(i,j);
                    QVariant value = cell->value();
                    row_content << value.toString().trimmed();
                }
                else
                {
                    row_content << "";
                }
            }
            lista.insert(i-2, row_content);
        }

        //OJO debo hacer una comprobación de los encabezados
        //-2 por los encabezados y fila de filtros y -1 por la fila de resumen al final
        for(int i=0; i < number_of_row - 1; i++)//**************revisar que no da bateo tenia un -2 antes
        {
            QJsonObject o;
            row_content = lista.at(i);
            if(row_content.at(0).trimmed().isEmpty() && row_content.at(1).trimmed().isEmpty()){
                ///Se salta las filas vacias
                continue;
            }
            for(int j = 0; j < number_of_column; j++)
            {
                QString header = listHeaders.at(j).toUpper().trimmed();
                if(checkIfFieldIsValid(header) && map.keys().contains(header)){
                    QString value_header = map.value(header);
                    QString contenido_en_excel = row_content.at(j);
                    if(contenido_en_excel.contains("CN-")){
                        contenido_en_excel = contenido_en_excel.remove("CN-").trimmed();
                    }
                    o.insert(value_header, contenido_en_excel);
                }
            }

            if(checkValidJsonObjectFields(o)){
                o.remove("");
                if(!o.isEmpty()){
                    jsonArray.append(o);
                }
            }
        }
    }
    return jsonArray;
}

int Tabla::containsField(QJsonArray jsonArray, QString field, QString fieldValue){
    for(int i=0; i< jsonArray.size(); i++)
    {
        if(jsonArray.at(i).toObject().value(field).toString() == fieldValue){
            return i;
        }
    }
    return -1;
}
void Tabla::cargarDesdeExcel(){
    QJsonArray jsonArrayToUpdate = loadIDOrdenesExcel(), jsonArray = getCurrentJsonArrayInTable();
    QJsonObject jsonObjectToUpdate, campos;
    QStringList numeros_internos_list_fallidos,
            numeros_internos_list_no_encontrados;
    int total=jsonArrayToUpdate.size();
    for(int i=0; i< total; i++)
    {
        jsonObjectToUpdate = jsonArrayToUpdate.at(i).toObject();
        QString numin = jsonObjectToUpdate.value(numero_interno).toString();
        QString numAbo = jsonObjectToUpdate.value(numero_abonado).toString();
        QString idOrden = jsonObjectToUpdate.value(idOrdenCABB).toString();

        int index = containsField(jsonArray, numero_interno, numin);
        if(index >= 0){
            QString idOrdenCurrent = jsonArray.at(index).toObject().value(idOrdenCABB).toString();
            QString idSatCurrent = jsonArray.at(index).toObject().value(ID_SAT).toString();
            if(idOrdenCurrent != idOrden || idSatCurrent != idOrden){
                if(checkIfFieldIsValid(numin) && checkIfFieldIsValid(idOrden)){
                    QStringList numeros_internos_list;
                    numeros_internos_list << numin;
                    campos.insert(idOrdenCABB, idOrden);
                    campos.insert(ID_SAT, idOrden);

                    if(update_fields(numeros_internos_list, campos)){
                        qDebug()<<"Pos: ("<< i <<"/"<<total<<")"<<" -- Actulizado ok -> NUMIN : " + numin;
                    }else{
                        numeros_internos_list_fallidos << numin;
                    }
                }
            }
        }else{
            numeros_internos_list_no_encontrados << numin;
        }
    }
}

void Tabla::on_pb_idOrdenFix_clicked()
{
    cargarDesdeExcel();
}

void Tabla::moveToPage(QString page){
    show_loading("Cargando  (" + page + " / " + QString::number(currentPages) + ")  ...");
    qDebug()<<"page -> " + page << " -------------------------------------------------";
    currentPage = page.toInt();
    ui->l_current_pagination->setText(page + " / " + QString::number(currentPages));

    int key_id = (page.toInt() - 1)*limit_pagination;
    QString key_id_string = QString::number(key_id);
    if(key_id == 0){
        key_id_string = QString::number(1);
    }
    int id_start = 0;
    if(jsonInfoTareasAmount.contains("id_" + key_id_string)){
        id_start = jsonInfoTareasAmount.value("id_" + key_id_string).toString().toInt();
    }

    QString query = lastQuery;
    getTareasCustomQuery(query, id_start);
    if(filtering){
        jsonArrayInTableFiltered = jsonArrayAll;
    }else{
        jsonArrayInTable = jsonArrayAll;
    }
    QJsonArray jsonArray = getCurrentJsonArrayInTable();
    fixModelforTable(jsonArray);
    setTableView();

    checkPaginationButtons();
    hide_loading();
}

void Tabla::addItemsToPaginationInfo(int sizeShowing){
    QStringList keys = jsonInfoTareasAmount.keys();
    keys.sort();
    int paginas = 0;
    QStringList items;
    for (int i=0; i < keys.size(); i++) {
        QString key = keys.at(i);
        if(key.contains("id_")){
            paginas++;
            qDebug()<< key << " -> "<< jsonInfoTareasAmount.value(key).toString();
            items.append(QString::number(paginas));
        }
    }
    ui->l_current_pagination->addItems(items);
    currentPages = paginas;
    ui->l_current_pagination->setText(QString::number(currentPage) + " / "
                                      + ((currentPages==0)?"1":QString::number(currentPages)));
    ui->l_current_pagination->hideSpinnerList();

    ui->l_cantidad_de_tareas->setText("Mostrando "
                                      + (QString::number(((currentPage - 1) * limit_pagination) + 1) + "-"
                                         + (QString::number(sizeShowing + ((currentPage - 1) * limit_pagination)))
                                         + " de " + QString::number(countTareas))
                                      /*+ " " + ((sizeShowing != 1)?"tareas":"tarea")*/);
    checkPaginationButtons();
}

void Tabla::checkPaginationButtons(){
    if(currentPage >= currentPages){
        ui->pb_next_pagination->setEnabled(false);
    }else{
        ui->pb_next_pagination->setEnabled(true);
    }
    if(currentPage <= 1){
        ui->pb_previous_pagination->setEnabled(false);
    }else{
        ui->pb_previous_pagination->setEnabled(true);

    }
    if(currentPages <= 1){
        ui->l_current_pagination->set_Enabled(false);
    }
    else{
        ui->l_current_pagination->set_Enabled(true);
    }
}
void Tabla::on_pb_next_pagination_clicked()
{
    currentPage++;
    moveToPage(QString::number(currentPage));

}

void Tabla::on_pb_previous_pagination_clicked()
{
    currentPage--;
    moveToPage(QString::number(currentPage));
}

void Tabla::on_actionDireccion_triggered()
{
    fillFilterPoblacion();
    filter_type |= F_DIRECCION;

    hideAllFilters();
    ui->widget_filtro_direccion->show();
    ui->widget_filtros->show();
    ui->pb_buscar_trabajo->hide();
}
void Tabla::fillFilterPoblacion(){
    show_loading("Cargando Poblaciones ...");
    QString queryStatus = getQueyStatus();
    QString query = " (" + queryStatus +  ") ";
    bool res = getTareasValuesFieldCustomQueryServer(
                empresa, poblacion, query);
    if(res){
        QStringList values;
        QStringList keys = jsonObjectValues.keys();
        for (int i= 0; i < keys.size(); i++) {
            QString key = keys.at(i);
            if(!key.contains("query") && !key.contains("count_values")){
                QString value = jsonObjectValues.value(key).toString();
                if(!values.contains(value, Qt::CaseInsensitive) && checkIfFieldIsValid(value)){
                    values << value;
                }
            }
        }
        completer_poblaciones = new QCompleter(values, this);
        completer_poblaciones->setCaseSensitivity(Qt::CaseInsensitive);
        completer_poblaciones->setFilterMode(Qt::MatchContains);
        ui->le_poblacion->setCompleter(completer_poblaciones);
    }
    hide_loading();
}

void Tabla::on_le_poblacion_editingFinished()
{
    if(ui->le_poblacion->isHidden() || !filter_enabled){
        return;
    }
    show_loading("Cargando Calles ...");
    QString municipio_selected = ui->le_poblacion->text();
    QString queryStatus = getQueyStatus();
    QString query = " (" + poblacion + " LIKE '" + municipio_selected +  "') "
            + " AND (" + queryStatus + ") ";
    bool res = getTareasValuesFieldCustomQueryServer(empresa, calle, query);
    if(res){
        QStringList values;
        QStringList keys = jsonObjectValues.keys();
        for (int i= 0; i < keys.size(); i++) {
            QString key = keys.at(i);
            if(!key.contains("query") && !key.contains("count_values")){
                QString value = jsonObjectValues.value(key).toString();
                if(!values.contains(value, Qt::CaseInsensitive) && checkIfFieldIsValid(value)){
                    values << value;
                }
            }
        }
        completer_calles = new QCompleter(values, this);
        completer_calles->setCaseSensitivity(Qt::CaseInsensitive);
        completer_calles->setFilterMode(Qt::MatchContains);
        ui->le_calle->setCompleter(completer_calles);
    }
    hide_loading();
}

void Tabla::on_le_calle_editingFinished()
{
    if(ui->le_calle->isHidden() || !filter_enabled){
        return;
    }
    show_loading("Cargando Portales ...");
    QString municipio_selected = ui->le_poblacion->text();
    QString street_selected = ui->le_calle->text();
    QString queryStatus = getQueyStatus();
    QString query = " (" + poblacion + " LIKE '" + municipio_selected +  "') AND "
            + " (" + calle + " LIKE '" + street_selected +  "')"
            + " AND (" + queryStatus + ") ";

    bool res = getTareasValuesFieldCustomQueryServer(empresa, numero, query);
    if(res){
        QStringList values;
        QStringList keys = jsonObjectValues.keys();
        for (int i= 0; i < keys.size(); i++) {
            QString key = keys.at(i);
            if(!key.contains("query") && !key.contains("count_values")){
                QString value = jsonObjectValues.value(key).toString();
                if(!values.contains(value, Qt::CaseInsensitive) && checkIfFieldIsValid(value)){
                    values << value;
                }
            }
        }
        values.sort();
        values.prepend("Todos");
        ui->cb_portal->clear();
        ui->cb_portal->addItems(values);
    }
    hide_loading();
}

void Tabla::on_cb_portal_currentIndexChanged(const QString &arg1)
{
    if(arg1.isEmpty() || arg1 == "Todos"){
        return;
    }
    show_loading("Cargando BISs ...");
    QString municipio_selected = ui->le_poblacion->text();
    QString street_selected = ui->le_calle->text();
    QString portal_selected = arg1;
    QString queryStatus = getQueyStatus();
    QString query = " (" + poblacion + " LIKE '" + municipio_selected +  "') AND "
            + " (" + calle + " LIKE '" + street_selected +  "') AND "
            + " (" + numero + " LIKE '" + portal_selected +  "')"
            + " AND (" + queryStatus + ") ";

    bool res = getTareasValuesFieldCustomQueryServer(empresa, BIS, query);
    if(res){
        ui->cb_BIS->clear();
        QStringList values;
        QStringList keys = jsonObjectValues.keys();
        for (int i= 0; i < keys.size(); i++) {
            QString key = keys.at(i);
            if(!key.contains("query") && !key.contains("count_values")){
                QString value = jsonObjectValues.value(key).toString();
                if(!values.contains(value, Qt::CaseInsensitive) && checkIfFieldIsValid(value)){
                    values << value;
                }
            }
        }
        values.sort();
        values.prepend("Todos");
        ui->cb_BIS->addItems(values);
    }
    hide_loading();
}

void Tabla::on_pb_database_config_clicked()
{
    DatabaseOptions *options = new DatabaseOptions(this, limit_pagination);
    connect(options, &DatabaseOptions::sendTareasPorPagina, this, &Tabla::setTareasPorPagina);
    options->show();
}

void Tabla::setTareasPorPagina(int cant){
    limit_pagination = cant;
    QFile file(info_pagination); // ficheros .dat se puede utilizar formato txt tambien
    if(file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out<<limit_pagination;
        file.close();
    }
    on_rb_todas_clicked();
}




