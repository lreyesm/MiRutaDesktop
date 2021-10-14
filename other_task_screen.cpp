#include "other_task_screen.h"
#include "ui_other_task_screen.h"

#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPicture>
#include <QMessageBox>
#include <QTimer>
#include <QDir>
#include "screen_tabla_tareas.h"
#include "new_table_structure.h"
#include <QDesktopWidget>
#include <counter.h>
#include <QPainter>
#include <QFileDialog>
#include <QPdfWriter>
#include <QDesktopServices>
#include <QPrinter>
#include <QPrintDialog>
#include "maildialog.h"
#include "obtener_geolocalizacion.h"
#include "informe_instalacion_servicios.h"
#include "marca.h"
#include "tipo.h"
#include "clase.h"
#include "observacion.h"
#include "operario.h"
#include "equipo_operario.h"
#include "emplazamiento.h"
#include "calibre.h"
#include "longitud.h"
#include "rueda.h"
#include "causa.h"
#include "zona.h"
#include "gestor.h"
#include "screen_table_piezas.h"
#include "resultado.h"
#include "info.h"
#include <QAbstractItemView>
#include <QScreen>
#include "mapas.h"
#include "itac.h"
#include "barcodegenerator.h"
#include <QFontDatabase>
#include <QFont>
#include "structure_contador.h"
#include "ruta.h"
#include "observationseletionscreen.h"
#include "QProgressIndicator.h"
#include "processesclass.h"
#include "globalfunctions.h"
#include "mylabelshine.h"
#include "sendwhatsappmessage.h"
#include "screen_tabla_contadores.h"

QString other_task_screen::administrator_loged="";
QString other_task_screen::last_operario="";
QString other_task_screen::last_fecha_ejecucion="";
bool other_task_screen::conexion_activa = true;

other_task_screen::other_task_screen(QWidget *parent, bool sin_revisar, bool mostrar_botones, QString empresa) :
    QDialog(parent),
    ui(new Ui::other_task_screen)
{
    setWindowFlags(Qt::CustomizeWindowHint);
    ui->setupUi(this);

    this->empresa = empresa;

    if(sin_revisar){
        ui->pb_tarea_sin_revisar->show();
    }else{
        ui->pb_tarea_sin_revisar->hide();
    }
    ui->pb_play_audio->hide();

    photo_request = false;
    showMesageBox = true;
    Script_excecution_result = -1;

    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));

    hide_loading();

    if(!mostrar_botones){
        ui->cb_editar_fotos->setEnabled(false);
        toogleEdit(false);
        ui->widget_buttons->hide();
        this->on_cb_editar_fotos_toggled(false);
        this->setFixedHeight(640);
        ui->cb_editar_fotos->hide();
        ui->pb_mostrar_carpeta->hide();
        ui->widget_fotos->hide();
    }
    foto_cambiada = false;

    ui->cb_estado_tarea->hide();

    this->setFocusProxy(ui->le_lectura_actual);
    ui->pb_minimizar->setFocusProxy(ui->le_lectura_actual);
    ui->cb_editar_fotos->setFocusProxy(ui->le_resultado);

    //Nuevo ---------------------------------------------------------------------------------------------
    connect(this,SIGNAL(hideMenuFast(QString)),this,SLOT(hideMenu(QString)));
    connect(ui->l_opciones_pdf,SIGNAL(actionPress(QString)),this,SLOT(onActionPress(QString)));
    //End Nuevo -----------------------------------------------------------------------------------------

    QStringList prioridades;
    prioridades << "MEDIA" << "BAJA" << "ALTA"<< "HIBERNAR";
    ui->l_prioridad->addItems(prioridades);

    QStringList dias;
//    dias << "NO ASIGNADO" << "LUNES" << "MARTES" << "MIÉRCOLES" << "JUEVES"<< "VIERNES" << "SÁBADO"<< "DOMINGO";
    dias << "NO ASIGNADO" << "1" << "2" << "3" << "4"<< "5";
    ui->l_dia_predeterminado->addItems(dias);

    QStringList ordenes;
    ordenes << "D" << "M" <<"E";
    ui->le_TIPORDEN->addItems(ordenes);

    initializeMaps();

    connect(ui->l_prioridad, &MyLabelSpinner::currentTextChangedGetPrevious,
            this, &other_task_screen::checkHibernacionSelected);

    connect(ui->lb_foto_antes_instalacion, &my_label::changedFoto,this, &other_task_screen::changeFoto);
    connect(ui->lb_foto_lectura, &my_label::changedFoto,this, &other_task_screen::changeFoto);
    connect(ui->lb_foto_numero_serie, &my_label::changedFoto,this, &other_task_screen::changeFoto);
    connect(ui->lb_foto_despues_instalacion, &my_label::changedFoto,this, &other_task_screen::changeFoto);
    connect(ui->lb_foto_incidencia_1, &my_label::changedFoto,this, &other_task_screen::changeFoto);
    connect(ui->lb_foto_incidencia_2, &my_label::changedFoto,this, &other_task_screen::changeFoto);
    connect(ui->lb_foto_incidencia_3, &my_label::changedFoto,this, &other_task_screen::changeFoto);
    connect(ui->lb_firma_cliente, &my_label::changedFoto,this, &other_task_screen::changeFoto);
}

other_task_screen::~other_task_screen()
{
    delete ui;
}

void other_task_screen::checkHibernacionSelected(QString item, QString previous){
    if(item == "HIBERNAR"){
        CalendarDialog *calendarDialog = new CalendarDialog(nullptr, true, empresa, false);
        connect(calendarDialog,SIGNAL(date_selected(QDate)),this,SLOT(get_date_selected(QDate)));
        connect(calendarDialog,SIGNAL(time_selected_end(QTime)),this,SLOT(get_time_selected_end(QTime)));
        connect(calendarDialog, &CalendarDialog::operator_selected,this, &other_task_screen::get_users_selected);
        connect(calendarDialog, &CalendarDialog::equipo_selected, this, &other_task_screen::get_team_selected);

        if(calendarDialog->exec()){
            hibernateTarea();
        }else{
            ui->l_prioridad->setText(previous);
        }
    }
}

bool other_task_screen::hibernateTarea(){
    QJsonObject numeros_internos;
    QJsonObject campos;
    QString numin = tarea_a_actualizar.value(numero_interno).toString().trimmed();
    QString previousPriority = tarea_a_actualizar.value(prioridad).toString().trimmed();
    if(!checkIfFieldIsValid(previousPriority)){
        previousPriority = "MEDIA";
    }
    numeros_internos.insert("1", numin);

    if(numeros_internos.isEmpty()){
        return true;
    }
    QDateTime dateTime;
    dateTime.setDate(selection_date);
    dateTime.setTime(selection_time_end);

    if(!operario_selected.isEmpty()){
        campos.insert(operario, operario_selected);
        tarea_a_actualizar.insert(operario, operario_selected);
    }
    if(!equipo_selected.isEmpty()){
        campos.insert(equipo, equipo_selected);
        tarea_a_actualizar.insert(equipo, equipo_selected);
    }
    campos.insert(prioridad, "HIBERNAR");
    campos.insert(hibernacion, dateTime.toString(formato_fecha_hora) + " :: " + previousPriority);
    campos.insert(date_time_modified, QDateTime::currentDateTime().toString(formato_fecha_hora));

    tarea_a_actualizar.insert(prioridad, "HIBERNAR");
    tarea_a_actualizar.insert(hibernacion, dateTime.toString(formato_fecha_hora) + " :: " + previousPriority);
    tarea_a_actualizar.insert(date_time_modified, QDateTime::currentDateTime().toString(formato_fecha_hora));

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

    connect(this, &other_task_screen::script_excecution_result,q,&QEventLoop::exit);
    QTimer::singleShot(DELAY, this, &other_task_screen::update_tareas_fields_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        updateTareasFromServer();
        GlobalFunctions::showMessage(this, "Actualizada", "Tarea hibernada correctamente");
        res = true;
        break;

    case database_comunication::script_result::update_tareas_fields_to_server_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}
void other_task_screen::changeFoto()
{
    foto_cambiada = true;
}

//Nuevo ---------------------------------------------------------------------------------------------
void other_task_screen::scalePhoto(QPixmap pixmap, QLabel *label, int max_w, int max_h){
    QSize size = pixmap.size();
    int max_height = max_h;
    int max_width = max_w;
    double ratio;
    if(size.width() > size.height()){
        ratio = static_cast<double>(size.height())/ static_cast<double>(size.width());
        max_height = max_width * ratio;
    }else{
        ratio = static_cast<double>(size.width())/ static_cast<double>(size.height());
        max_width = max_height * ratio;
    }
    label->setScaledContents(true);
    label->setMaximumSize(max_width, max_height);
    label->setPixmap(pixmap);
}

void other_task_screen::setLogoType(QPixmap logo){
    scalePhoto(logo, ui->l_logo, 61, 61);
}
void other_task_screen::getLogoType(){
    QJsonArray jsonArray = Gestor::readGestores();
    for (int i=0; i < jsonArray.size(); i++) {
        QString gestor_v, cod_v;
        gestor_v = jsonArray.at(i).toObject().value(gestor_gestores).toString();
        if(gestor_v == o.value(GESTOR).toString()){
            Gestor *gestor = new Gestor(nullptr, false, empresa);
            gestor->setData(jsonArray.at(i).toObject());
            QPixmap logo = gestor->getPhotoGestor();
            if(logo != QPixmap()){
                //                scalePhoto(logo);
                scalePhoto(logo, ui->l_logo, 61, 61);
            }
        }
    }
}
void other_task_screen::mousePressEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
{
    emit hideMenuFast("");
    if(e->buttons()==Qt::RightButton){
        ui->l_opciones_pdf->generateAnimation();
    }
    emit mouse_pressed();
    QWidget::mousePressEvent(e);
}

void other_task_screen::mouseReleaseEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
{
    Q_UNUSED(e);
    emit mouse_Release();
}

void other_task_screen::mouseDoubleClickEvent(QMouseEvent *event)
{
    on_pb_maximizar_clicked();
    QWidget::mouseDoubleClickEvent(event);
}


void other_task_screen::on_pb_close_clicked()
{
    this->close();
}

void other_task_screen::closeEvent(QCloseEvent *event)
{
    closing_window = true;
    disconnect(&timerChangingGeoCode,SIGNAL(timeout()),this,SLOT(setGeoCodeByCodEmplazamiento()));
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(requestContadoresList()));
    timerChangingGeoCode.stop();
    timer.stop();
    hidingLoading();
    emit closing();
    QWidget::closeEvent(event);
}
void other_task_screen::clearTask(){
    //OJO no quitar esto puede dar fallos en asignar campos comunes**************************************************
    o = QJsonObject();
    tarea_a_actualizar = QJsonObject();
    clear_all_pictures();
    //End OJO no quitar esto puede dar fallos en asignar campos comunes**************************************************
}
void other_task_screen::hideMenu(const QString from){
    Q_UNUSED(from);
    ui->l_opciones_pdf->hideChilds();
}
void other_task_screen::onActionPress(QString action){
    emit hideMenuFast("");
    if(action == "l_crear_pdf"){
        on_pb_crear_pdf_clicked();
    }
    if(action == "l_ver_pdf"){
        on_pb_ver_pdf_clicked();
    }
    if(action == "l_enviar_pdf"){
        on_pb_enviar_pdf_clicked();
    }
    if(action == "l_imprimir_pdf"){
        on_pb_imprimir_pdf_clicked();
    }
}
//End Nuevo -----------------------------------------------------------------------------------------

QJsonArray other_task_screen::readJsonArrayTasks()
{
    QJsonArray jsonArray;
    QFile *data_base = new QFile(tareas_sincronizadas); // ficheros .dat se puede utilizar formato txt tambien
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
QStringList other_task_screen::readModifiedTasks()
{
    QStringList lista_tareas_mod;
    QFile *data_base = new QFile(tareas_modificadas); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::ReadOnly))
        {
            QDataStream in(data_base);
            in>>lista_tareas_mod;
            data_base->close();
        }
    }
    return lista_tareas_mod;
}
void other_task_screen::writeJsonArrayTasks(QJsonArray jsonArray)
{
    QFile *data_base = new QFile(tareas_sincronizadas); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::WriteOnly))
        {
            data_base->seek(0);
            QDataStream out(data_base);
            out<<jsonArray;
            data_base->close();
        }
    }
}
void other_task_screen::writeModifiedTasks(QStringList lista_tareas_mod)
{
    QFile *data_base = new QFile(tareas_modificadas); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::WriteOnly))
        {
            data_base->seek(0);
            QDataStream out(data_base);
            out<<lista_tareas_mod;
            data_base->close();
        }
    }
}
void other_task_screen::ordenarLista(QStringList &array, QString order){
    QString temp;
    for (int i =0; i < array.size(); i++) {
        for (int j =0; j < array.size(); j++) {
            if(array[j].isEmpty()){
                array[j] = "zzzzzzzzz";
            }
            if(order == "MAYOR_MENOR"){
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
}
void other_task_screen::getContadoresList(QString serie){
    GlobalFunctions gf(this, empresa);
    QStringList numeros_serie_de_contadores = gf.getContadoresList(true, serie);
    if(closing_window){
        return;
    }
    completer_numeros_serie = new QCompleter(numeros_serie_de_contadores, this);
    completer_numeros_serie_devueltos = new QCompleter(numeros_serie_de_contadores, this);

    completer_numeros_serie->setCaseSensitivity(Qt::CaseInsensitive);
    completer_numeros_serie->setFilterMode(Qt::MatchContains);
    completer_numeros_serie_devueltos->setCaseSensitivity(Qt::CaseInsensitive);
    ui->le_numero_serie_contador->setCompleter(completer_numeros_serie);
    ui->le_numero_serie_contador_devuelto->setCompleter(completer_numeros_serie_devueltos);
    connect(completer_numeros_serie,SIGNAL(highlighted(QString)),this,SLOT(fill_counter_data(QString)));
    connect(completer_numeros_serie_devueltos,SIGNAL(highlighted(QString)),this,SLOT(fill_counter_data_devuelto(QString)));
}

void other_task_screen::initializeMaps(){

    QJsonArray jsonArray;
    mapaEstados.insert("Abierta", "IDLE");
    mapaEstados.insert("Abierta (Cita)", "IDLE CITA");
    mapaEstados.insert("Abierta (En Batería)", "IDLE TO_BAT");
    mapaEstados.insert("Ejecutada", "DONE");
    mapaEstados.insert("Cerrada", "CLOSED");
    mapaEstados.insert("Informada", "INFORMADA");
    mapaEstados.insert("Requerida", "REQUERIDA");

    map_days_week.insert(1,"Lunes");
    map_days_week.insert(2,"Martes");
    map_days_week.insert(3,"Miércoles");
    map_days_week.insert(4,"Jueves");
    map_days_week.insert(5,"Viernes");
    map_days_week.insert(6,"Sábado");
    map_days_week.insert(7,"Domingo");

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

    lista_tipo_radio.append("R3");
    lista_tipo_radio.append("R4");
    lista_tipo_radio.append("W4");
    lista_tipo_radio.append("LRW");

    QStringList listaGestores, listaResultados, listaCausas, listaAccionesOrdenadas, listaZonas
            , listaCalibres , listaLongitudes , listaRuedas , listaTipos, listaOperarios,
            listaEquipos;

    jsonArray = Gestor::readGestores();
    for (int i=0; i < jsonArray.size(); i++) {
        QString gestores_v, cod_v;
        gestores_v = jsonArray.at(i).toObject().value(gestor_gestores).toString();

        listaGestores.append(gestores_v);
    }
    ui->cb_gestor->clear();
    ui->cb_gestor->addItems(listaGestores);

    jsonArray = Calibre::readCalibres();
    for (int i=0; i < jsonArray.size(); i++) {
        QString calibre_v, cod_v;
        calibre_v = jsonArray.at(i).toObject().value(calibre_calibres).toString();

        listaCalibres.append(calibre_v);
    }
    jsonArray = Longitud::readLongitudes();
    for (int i=0; i < jsonArray.size(); i++) {
        QString long_v, cod_v;
        long_v = jsonArray.at(i).toObject().value(longitud_longitudes).toString();

        listaLongitudes.append(long_v);
    }
    jsonArray = Rueda::readRuedas();
    for (int i=0; i < jsonArray.size(); i++) {
        QString rueda_v, cod_v;
        rueda_v = jsonArray.at(i).toObject().value(rueda_ruedas).toString();

        listaRuedas.append(rueda_v);
    }

    jsonArray = Zona::readZonas();
    for (int i=0; i < jsonArray.size(); i++) {
        QString zona_v, cod_v;
        zona_v = jsonArray.at(i).toObject().value(zona_zonas).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_zona_zonas).toString();

        listaZonas.append(cod_v + " - " + zona_v);
        ordenarLista(listaZonas, ""); //"" - menor a mayor
    }

    jsonArray = Causa::readCausas();
    for (int i=0; i < jsonArray.size(); i++) {
        QString causa_v, cod_v, accion_v;
        causa_v = jsonArray.at(i).toObject().value(causa_causas).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_causa_causas).toString();
        accion_v = jsonArray.at(i).toObject().value(accion_ordenada_causas).toString();
        if(!listaAccionesOrdenadas.contains(accion_v)){
            listaAccionesOrdenadas.append(accion_v);
        }
        listaCausas.append(cod_v + " - " + causa_v);
    }
    jsonArray = Resultado::readResultados();
    for (int i=0; i < jsonArray.size(); i++) {
        QString resultado_v, cod_v;
        resultado_v = jsonArray.at(i).toObject().value(resultado_resultados).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_resultado_resultados).toString();

        listaResultados.append(cod_v + " - " + resultado_v);
    }

    jsonArray = Emplazamiento::readEmplazamientos();
    for (int i=0; i < jsonArray.size(); i++) {
        QString resto_v, descripcion_v, cod_v;
        resto_v = jsonArray.at(i).toObject().value(resto_emplazamientos).toString();
        descripcion_v = jsonArray.at(i).toObject().value(emplazamiento_emplazamientos).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_emplazamiento_emplazamientos).toString();

        mapaTiposDeRestoEmplazamiento.insert(cod_v, resto_v);
    }

    jsonArray = Tipo::readTipos();
    for (int i=0; i < jsonArray.size(); i++) {
        QString tipo_v;
        tipo_v = jsonArray.at(i).toObject().value(tipo_tipos).toString();
        lista_tipo_fluido.append(tipo_v);
    }

    jsonArray = Clase::readClases();
    for (int i=0; i < jsonArray.size(); i++) {
        QString clase_v, cod_v;
        clase_v = jsonArray.at(i).toObject().value(clase_clases).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_clase_clases).toString();
        mapaTiposDeClase.insert(cod_v, clase_v);
    }

    jsonArray = Marca::readMarcas();
    for (int i=0; i < jsonArray.size(); i++) {
        QString marca_v, modelo_v, cod_v;
        marca_v = jsonArray.at(i).toObject().value(marca_marcas).toString();
        modelo_v = jsonArray.at(i).toObject().value(modelo_marcas).toString();
        cod_v = jsonArray.at(i).toObject().value(codigo_marca_marcas).toString();

        mapaTiposDeMarca.insert(cod_v, marca_v + " - " + modelo_v);
    }

    QStringList marcas;
    for (int i=0;i<mapaTiposDeMarca.size();i++) {

        marcas<< mapaTiposDeMarca.keys().at(i) + " - " + mapaTiposDeMarca.values().at(i);
    }
    QStringList clases;
    for (int i=0;i<mapaTiposDeClase.size();i++) {

        clases<< mapaTiposDeClase.keys().at(i) + " - " + mapaTiposDeClase.values().at(i);
    }

    QCompleter *completer = new QCompleter(marcas, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_marca_contador->setCompleter(completer);
    ui->le_marca_devuelta->setCompleter(completer);

    completer = new QCompleter(lista_tipo_fluido, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_TIPO_FLUIDO->setCompleter(completer);
    ui->le_TIPO_FLUIDO_DEVUELTO->setCompleter(completer);

    completer = new QCompleter(clases, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_TIPO->setCompleter(completer);
    ui->le_TIPO_DEVUELTO->setCompleter(completer);

    completer_radios = new QCompleter(lista_tipo_radio, this);
    completer_radios->setCaseSensitivity(Qt::CaseInsensitive);
    completer_radios->setFilterMode(Qt::MatchContains);
    ui->le_tipoRadio_levantado->setCompleter(completer_radios);
    ui->le_tipoRadio->setCompleter(completer_radios);
    connect(completer_radios,SIGNAL(activated(QString)),this,SLOT(autocompleteModule(QString)));

    completer = new QCompleter(listaZonas, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_zona->setCompleter(completer);


    completer_emplazamiento = new QCompleter(mapaTiposDeRestoEmplazamiento.keys(), this);
    completer_emplazamiento->setCaseSensitivity(Qt::CaseInsensitive);
    completer_emplazamiento->setFilterMode(Qt::MatchContains);
    ui->le_emplazamiento->setCompleter(completer_emplazamiento);

    completer_emplazamientoDV = new QCompleter(mapaTiposDeRestoEmplazamiento.keys(), this);
    completer_emplazamientoDV->setCaseSensitivity(Qt::CaseInsensitive);
    completer_emplazamientoDV->setFilterMode(Qt::MatchContains);
    ui->le_emplazamiento_devuelto->setCompleter(completer_emplazamientoDV);

    completer_resultados = new QCompleter(listaResultados, this);
    completer_resultados->setCaseSensitivity(Qt::CaseInsensitive);
    completer_resultados->setFilterMode(Qt::MatchContains);
    ui->le_resultado->setCompleter(completer_resultados);

    completer_causas = new QCompleter(listaCausas, this);
    completer_causas->setCaseSensitivity(Qt::CaseInsensitive);
    completer_causas->setFilterMode(Qt::MatchContains);
    ui->le_causa_origen->setCompleter(completer_causas);

    completer = new QCompleter(listaCausas, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_AREALIZAR_devuelta->setCompleter(completer);

    completer = new QCompleter(listaAccionesOrdenadas, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->le_accion_ordenada->setCompleter(completer);

    //    connect(completer_causas,SIGNAL(highlighted(QString)),this,SLOT(on_le_causa_origen_textEdited(QString)));
    connect(completer_resultados,SIGNAL(activated(QString)),this,SLOT(fill_Resultados_data(QString)));
    completer_resultados->popup()->setFixedWidth(300);
    connect(completer_emplazamiento,SIGNAL(highlighted(QString)),this,SLOT(fill_Emplazamiento_data(QString)));
    connect(completer_emplazamientoDV,SIGNAL(highlighted(QString)),this,SLOT(fill_Emplazamiento_data_devuelto(QString)));

}

void other_task_screen::fill_Causas_data(QString string_completed)
{
    Q_UNUSED(string_completed);
    QTimer::singleShot(100, this, SLOT(fill_Causas()));
}

void other_task_screen::fill_Causas()
{
    QString string_completed = tarea_a_actualizar.value(AREALIZAR).toString();
    if(string_completed.contains(" - ")){
        if(string_completed.split(" - ").size() > 1){
            tarea_a_actualizar.insert(ANOMALIA, string_completed.split(" - ").at(0));
        }
    }
}

void other_task_screen::fill_Resultados_data(QString string_completed)
{
    Q_UNUSED(string_completed);
    QTimer::singleShot(100, this, SLOT(fill_Resultados()));
}
void other_task_screen::fill_Resultados(){
    QString string_completed = ui->le_resultado->text();
    if(string_completed.contains(" - ")){
        QStringList split = string_completed.split(" - ");
        if(split.size() > 1){
            QString cod = split.at(0);
            ui->le_resultado->setText(cod);
        }
    }
}

void other_task_screen::fill_Emplazamiento_data(QString string_completed)
{
    if(mapaTiposDeRestoEmplazamiento.keys().contains(string_completed)){
        tarea_a_actualizar.insert(RESTEMPLAZA, mapaTiposDeRestoEmplazamiento.value(string_completed));
    }
}
void other_task_screen::fill_Emplazamiento_data_devuelto(QString string_completed)
{
    if(string_completed == "BA" || string_completed == "BT"){
        QString ubic = ui->le_ubicacion_en_bateria->text().trimmed();
        if(checkIfFieldIsValid(ubic)){
            if(ubic.contains("BA") || ubic.contains("BA")){
                ubic.replace("BA", "").replace("BT", "");
                ui->le_RESTO_EM->setText(ubic);
            }
        }

    }else{
        if(mapaTiposDeRestoEmplazamiento.keys().contains(string_completed)){
            ui->le_RESTO_EM->setText(mapaTiposDeRestoEmplazamiento.value(string_completed));
        }
    }
}
QString other_task_screen::transformarFecha(QString fecha_old_view)
{
    QDateTime fecha = QDateTime::fromString(fecha_old_view, formato_fecha_hora);
    if(fecha.isValid()){
        QString fecha_new_view = fecha.toString(formato_fecha_hora_new_view);
        return fecha_new_view;
    }
    return "";
}

QStringList other_task_screen::readficherosDescargados()
{
    QStringList lista_ficheros_descargados;
    QFile *data_base = new QFile(ficheros_comprimidos_descargados); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::ReadOnly))
        {
            QDataStream in(data_base);
            in>>lista_ficheros_descargados;
            data_base->close();
        }
    }
    return lista_ficheros_descargados;
}
void other_task_screen::writeficherosDescargados(QStringList lista_ficheros_descargados)
{
    QFile *data_base = new QFile(ficheros_comprimidos_descargados); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::WriteOnly))
        {
            data_base->seek(0);
            QDataStream out(data_base);
            out<<lista_ficheros_descargados;
            data_base->close();
        }
    }
}
QStringList other_task_screen::getFilesWithNamesInDir(QStringList names, QDir dir_file){ //busca archivos con estos nombres en nombre de fichero
    QStringList files_paths = dir_file.entryList();
    QStringList files;
    bool found;
    for(int i = 0; i < files_paths.size(); i++){
        found = true;
        for(int j = 0; j < names.size(); j++){
            if(!files_paths.at(i).contains(names.at(j), Qt::CaseInsensitive)){
                found = false;
            }
        }
        if(found){
            QString path = files_paths.at(i);
            files << path;
        }
    }
    return files;
}
QString other_task_screen::buscarCarpetaDeFotos(){
    QDir dir;
    QString path = buscarCarpetaDeTrabajoPendiente();
    QStringList names;
    if(!path.isEmpty()){
        dir.setPath(path);
        names << "fotos"<<"tareas";
        path = getDirWithNames(dir, names);
        return path;
    }else{
        return "";
    }
}
QString other_task_screen::buscarCarpetaDeTrabajoPendiente(){
    QDir dir = QDir::current();
    QString path ="";
    QStringList names;
    names << "trabajo"<<"descargado";
    path = getDirWithNames(dir, names);
    return path;
}

QString other_task_screen::getDirWithNames(QDir dir_fotos, QStringList names){ //busca carpetas con estos nombres en nombre de carpeta
    QStringList dirs_paths = dir_fotos.entryList();
    QString path ="";
    bool found;
    for(int i = 0; i < dirs_paths.size(); i++){
        found = true;
        for(int j = 0; j < names.size(); j++){
            if(!dirs_paths.at(i).contains(names.at(j), Qt::CaseInsensitive)){
                found = false;
            }
        }
        if(found){
            dir_fotos.cd(dirs_paths.at(i));
            path = dir_fotos.path() + "/";
            return path;
        }
    }
    return "";
}
QStringList other_task_screen::readTrabajosCargados()
{
    QStringList lista_trabajos_cargados;
    QFile *data_base = new QFile(trabajos_cargados); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::ReadOnly))
        {
            QDataStream in(data_base);
            in>>lista_trabajos_cargados;
            data_base->close();
        }
    }
    return lista_trabajos_cargados;
}
void other_task_screen::writeTrabajosCargados(QStringList lista_trabajos_cargados)
{
    QFile *data_base = new QFile(trabajos_cargados); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::WriteOnly))
        {
            data_base->seek(0);
            QDataStream out(data_base);
            out<<lista_trabajos_cargados;
            data_base->close();
        }
    }
}


QString other_task_screen::transformarFecha_to_old_view(QString fecha_new_view)
{
    QDateTime fecha = QDateTime::fromString(fecha_new_view, formato_fecha_hora_new_view);
    if(fecha.isValid()){
        QString fecha_old_view = fecha.toString(formato_fecha_hora);
        return fecha_old_view;
    }
    return "";
}

bool other_task_screen::checkIfExistFoto(QJsonObject o){
    if(checkIfFieldIsValid(o.value(foto_antes_instalacion).toString())){
        return true;
    }
    if(checkIfFieldIsValid(o.value(foto_despues_instalacion).toString())){
        return true;
    }
    if(checkIfFieldIsValid(o.value(foto_numero_serie).toString())){
        return true;
    }
    if(checkIfFieldIsValid(o.value(foto_lectura).toString())){
        return true;
    }
    if(checkIfFieldIsValid(o.value(foto_incidencia_1).toString())){
        return true;
    }
    if(checkIfFieldIsValid(o.value(foto_incidencia_2).toString())){
        return true;
    }
    if(checkIfFieldIsValid(o.value(foto_incidencia_3).toString())){
        return true;
    }
    if(checkIfFieldIsValid(o.value(firma_cliente).toString())){
        return true;
    }
    return false;
}
void other_task_screen::populateDataView(){
    //OJO SOLO PARA DEPURAR
    QString calibre, anomalia;
    calibre = o.value(calibre_toma).toString();
    anomalia = o.value(ANOMALIA).toString();

    //    ui->plainTextEdit->setPlainText("calibre = " + calibre + "anomalia = " + anomalia);

    QString gestor = o.value(GESTOR).toString();
    if(!checkIfFieldIsValid(gestor)){
        if(checkIfFieldIsValid(GlobalFunctions::readGestorSelected()) && GlobalFunctions::readGestorSelected()!="Todos"){
            gestor = GlobalFunctions::readGestorSelected();
        }else {
            gestor = "Sin_Gestor";
        }
    }
    QString priority = o.value(prioridad).toString().trimmed();
    if(checkIfFieldIsValid(priority)){
        if(priority == "HIBERNAR"){//gris
            QString field = o.value(hibernacion).toString().trimmed();
            if(checkIfFieldIsValid(field)){
                QStringList split = field.split("::");
                if(split.size()>1){
                    field = split.at(0).trimmed();
                }
                QDateTime dt = QDateTime::fromString(field, formato_fecha_hora);
                if(dt <= QDateTime::currentDateTime()){
                    if(split.size()>1){
                        priority = split.at(1).trimmed();
                    }else{
                        priority = "MEDIA";//naranja
                    }
                    o.insert(prioridad, priority);
                    tarea_a_actualizar.insert(prioridad, priority);
                }
            }
        }
        ui->l_prioridad->setText(priority);
    }
    QString default_day = o.value(dia_predeterminado).toString().trimmed();
    if(checkIfFieldIsValid(default_day)){
        ui->l_dia_predeterminado->setText(default_day);
    }
    if(checkIfFieldIsValid(gestor)){
        if(!ui->cb_gestor->items().contains(gestor)){
            ui->cb_gestor->addItem(gestor);
        }
    }
    ui->cb_gestor->setCurrentText(this->nullity_check(gestor));
    ui->le_poblacion->setText(this->nullity_check(o.value(poblacion).toString()));
    ui->le_calle->setText(this->nullity_check(o.value(calle).toString()));

    ui->le_piso->setText(this->nullity_check(o.value(piso).toString()));
    ui->le_mano->setText(this->nullity_check(o.value(mano).toString()));
    ui->le_numero_serie_contador->setText(this->nullity_check(o.value(numero_serie_contador).toString()));

    QString cal = o.value(calibre_toma).toString();

    ui->le_calibre_toma->setText(this->nullity_check(cal));
    ui->le_calibre_toma_real->setText(this->nullity_check(o.value(calibre_real).toString()));
    ui->le_equipo->setText(this->nullity_check(o.value(equipo).toString()));
    QString operario_l = o.value(operario).toString();
    if(!checkIfFieldIsValid(operario_l)){
        operario_l= last_operario;
    }else{
        last_operario = operario_l;
    }
    ui->le_operario->setText(this->nullity_check(operario_l));
    ui->le_emplazamiento->setText(this->nullity_check(o.value(emplazamiento).toString()));
    ui->le_observaciones->setText(this->nullity_check(o.value(observaciones).toString()));
    ui->le_actividad->setText(this->nullity_check(o.value(actividad).toString()));
    //separar nombre de apellidos
    QString raw_name = this->nullity_check(o.value(nombre_cliente).toString());
    if(!raw_name.isEmpty() & !raw_name.isNull())
    {
        QStringList list =  raw_name.split(" ");

        if(list.count() >= 4){
            ui->le_nombre_cliente->setText(this->nullity_check(list.at(0) +" "+ list.at(1)));
            ui->le_apellido1->setText(this->nullity_check(list.at(2)));
            int c = 3;
            QString secondLastName="";
            while( c < list.size() && (list.at(c).compare("de", Qt::CaseInsensitive)==0
                                       || list.at(c).compare("del", Qt::CaseInsensitive)==0
                                       || list.at(c).compare("el", Qt::CaseInsensitive)==0
                                       //                   || list.at(c).compare("los", Qt::CaseInsensitive)==0
                                       //                   || list.at(c).compare("la", Qt::CaseInsensitive)==0
                                       || list.at(c).compare("las", Qt::CaseInsensitive)==0) ){
                ui->le_apellido1->setText(ui->le_apellido1->text() + " " +list.at(c));
                c++;
                if(c >= list.size()){
                    break;
                }
            }
            if(c < list.size()){
                ui->le_apellido1->setText(ui->le_apellido1->text() + " " +list.at(c));
            }
            c++;
            ui->le_apellido2->setText(secondLastName);
            for(; c< list.size(); c++){
                secondLastName += list.at(c) + " ";
            }
            ui->le_apellido2->setText(secondLastName.trimmed());
        }
        else if(list.count() == 3)
        {
            ui->le_nombre_cliente->setText(this->nullity_check(list.at(0)));
            ui->le_apellido1->setText(this->nullity_check(list.at(1)));
            ui->le_apellido2->setText(this->nullity_check(list.at(2)));
        }
        else
        {
            if(list.count() == 1)
            {
                ui->le_nombre_cliente->setText(this->nullity_check(list.at(0)));
            }

            if(list.count() == 2)
            {
                ui->le_nombre_cliente->setText(this->nullity_check(list.at(0)));
                ui->le_apellido1->setText(this->nullity_check(list.at(1)));
            }
        }
        QString apellidos = ui->le_apellido1->text();
        if(apellidos.split(" ").size()>= 2 &&  ui->le_apellido2->text().isEmpty()){
            ui->le_apellido1->setText(apellidos.split(" ").at(0));
            ui->le_apellido2->setText(apellidos.split(" ").at(1));
        }
    }
    ui->le_numero_abonado->setText(this->nullity_check(o.value(numero_abonado).toString()));
    QString tels = o.value(telefono1).toString().trimmed();
    QString tel2 = o.value(telefono2).toString().trimmed();
    if(tels.contains("-") && tels.split("-").length()>=2){
        tel2 = tels.split("-").at(1).trimmed();
        tels = tels.split("-").at(0).trimmed();
    }
    if(tels.contains(" ") && tels.split(" ").length()>=2){
        tel2 = tels.split(" ").at(1).trimmed();
        tels = tels.split(" ").at(0).trimmed();
    }
    ui->le_telefono1->setText(this->nullity_check(tels));
    ui->le_telefono2->setText(this->nullity_check(tel2));
    QString telefonos_info =  o.value(telefonos_cliente).toString().trimmed();
    if(checkIfFieldIsValid(telefonos_info)){

        if(telefonos_info.contains("TEL1_NO_CONTESTA")){
            ui->le_telefono1->setStyleSheet(QStringLiteral("background-color: #EFEFEF;"
                                                           "border-radius: 3px;"
                                                           "\ncolor:rgb(50, 200, 100);"));
            ui->lb_telefono1->setText("Tel.1: No contesta");
        }
        if(telefonos_info.contains("TEL1_INCORRECTO")){
            ui->le_telefono1->setStyleSheet(QStringLiteral("background-color: #EFEFEF;"
                                                           "border-radius: 3px;"
                                                           "\ncolor:rgb(255, 80, 80);"));
            ui->lb_telefono1->setText("Tel.1: Incorrecto");
        }

        if(telefonos_info.contains("TEL2_NO_CONTESTA")){
            ui->le_telefono2->setStyleSheet(QStringLiteral("background-color: #EFEFEF;"
                                                           "border-radius: 3px;"
                                                           "\ncolor:rgb(50, 200, 100);"));
            ui->lb_telefono2->setText("Tel.2: No contesta");
        }
        if(telefonos_info.contains("TEL2_INCORRECTO")){
            ui->le_telefono2->setStyleSheet(QStringLiteral("background-color: #EFEFEF;"
                                                           "border-radius: 3px;"
                                                           "\ncolor:rgb(255, 80, 80);"));
            ui->lb_telefono2->setText("Tel.2: Incorrecto");
        }
    }
    //    ui->le_fecha_tocado_puerta->setText(this->nullity_check(o.value(fechas_tocado_puerta).toString()));

    QStringList list = this->nullity_check(o.value(fechas_tocado_puerta).toString().replace("null", "").trimmed()).split("\n");
    for (int i=0; i < list.size(); i++) {
        QString fecha = list.at(i);
        fecha = transformarFecha(fecha);
        list.replace(i, fecha);
    }
    ui->cb_fecha_tocado_puerta->clear();
    ui->cb_fecha_tocado_puerta->insertItems(0,list);
    //    ui->le_fechas_nota_aviso->setText(this->nullity_check(o.value(fechas_nota_aviso).toString()));
    list.clear();
    list = this->nullity_check(o.value(fechas_nota_aviso).toString().replace("null", "").trimmed()).split("\n");
    for (int i=0; i < list.size(); i++) {
        QString fecha = list.at(i);
        fecha = transformarFecha(fecha);
        list.replace(i, fecha);
    }
    if(!list.isEmpty()){
        ui->cb_fechas_nota_aviso->clear();
        ui->cb_fechas_nota_aviso->insertItems(0,list);
    }

    list.clear();
    list = this->nullity_check(o.value(piezas).toString().replace("null", "").trimmed()).split("\n");
    for (int i=0; i < list.size(); i++) {
        QString pieza = list.at(i);
        list.replace(i, pieza);
    }
    if(!list.isEmpty()){
        ui->cb_piecerio->clear();
        ui->cb_piecerio->insertItems(0,list);
    }

    ui->le_acceso->setText(this->nullity_check(o.value(acceso).toString()));
    ui->le_resultado->setText(this->nullity_check(o.value(resultado).toString()));
    ui->le_nuevo_citas->setText(this->nullity_check(o.value(nuevo_citas).toString()));

    QString fecha_instalacion_vieja = o.value(fecha_instalacion).toString().trimmed();
    if(checkIfFieldIsValid(transformarFecha(fecha_instalacion_vieja))){
        fecha_instalacion_vieja = transformarFecha(fecha_instalacion_vieja);
    }
    ui->le_FECINST->setText(this->nullity_check(fecha_instalacion_vieja));

    ui->le_zona->setText(this->nullity_check(o.value(zona).toString()));
    ui->le_ruta->setText(this->nullity_check(o.value(ruta).toString()));
    ui->le_marca_contador->setText(this->nullity_check(o.value(marca_contador).toString()));
    ui->le_marca_contador->setToolTip(this->nullity_check(o.value(marca_contador).toString()));

    ui->le_numero_serie_modulo->setText(this->nullity_check(o.value(numero_serie_modulo).toString()));
    ui->le_lectura_ultima->setText(this->nullity_check(o.value(lectura_ultima).toString()));
    ui->le_lectura_actual->setText(this->nullity_check(o.value(lectura_actual).toString()));

    ui->le_codigo_geolocalizacion->setText(this->nullity_check(o.value(codigo_de_geolocalizacion).toString()));
    ui->le_geolocalizacion->setText(this->nullity_check(o.value(geolocalizacion).toString()));
    QString url_geo = o.value(url_geolocalizacion).toString();
    if(!checkIfFieldIsValid(url_geo) && ((!ui->le_geolocalizacion->text().trimmed().isEmpty()))){
        url_geo = o.value(codigo_de_localizacion).toString();
        if(!checkIfFieldIsValid(url_geo)){
            url_geo = ui->le_geolocalizacion->text().trimmed();
        }
        url_geo = "https://maps.google.com/?q="+url_geo;
        tarea_a_actualizar.insert(url_geolocalizacion, url_geo);
    }
    ui->le_url_google_maps->setText(this->nullity_check(url_geo));

    ui->le_LECTURA_CONTADOR_NUEVO->setText(this->nullity_check(o.value(lectura_contador_nuevo).toString()));
    ui->le_ubicacion_en_bateria->setText(this->nullity_check(o.value(ubicacion_en_bateria).toString()));
    ui->le_propiedad->setText(this->nullity_check(o.value(propiedad).toString()));
    ui->le_ruedas->setText(this->nullity_check(o.value(ruedas).toString()));
    ui->le_numero->setText(this->nullity_check(o.value(numero).toString()));
    ui->le_numero_interno->setText(this->nullity_check(o.value(numero_interno).toString()));
    QString status = o.value(status_tarea).toString().trimmed();
    ui->cb_estado_tarea->setCurrentText(mapaEstados.key(status));
    ui->le_status_tarea_just_reading->setText(mapaEstados.key(status).left(1));

    //CAMPOS AÑADIDOS
    QString idCabb = o.value(idOrdenCABB).toString();
    //    if(!checkIfFieldIsValid(idCabb)){
    //       idCabb = o.value(ID_SAT).toString();
    //    }
    ui->le_idOrdenCABB->setText(this->nullity_check(idCabb));
    QString fecha_importacion = o.value(FechImportacion).toString().trimmed();
    fecha_importacion = transformarFecha(fecha_importacion);

    QString fecha_emision = o.value(FECEMISIO).toString().trimmed();
    fecha_emision = transformarFecha(fecha_emision);
    if(!checkIfFieldIsValid(fecha_emision)){
        fecha_emision = fecha_importacion;
    }
    ui->le_FECEMISIO->setText(this->nullity_check(fecha_emision));

    ui->le_accion_ordenada->setText(this->nullity_check(o.value(accion_ordenada).toString()));
    QString causa_origen_l = o.value(causa_origen).toString();
    if(causa_origen_l.isEmpty()){
        if(!anomalia.isEmpty()){
            causa_origen_l = changeCausaOrigen(anomalia);
        }
    }
    ui->le_causa_origen->setText(this->nullity_check(causa_origen_l));

    ui->le_BIS->setText(this->nullity_check(o.value(BIS).toString()));
    QString fecha_ejecucion = o.value(F_INST).toString().trimmed();
    if(!checkIfFieldIsValid(fecha_ejecucion)){
        fecha_ejecucion = last_fecha_ejecucion;
    }else {
        last_fecha_ejecucion = fecha_ejecucion;
    }
    fecha_ejecucion = transformarFecha(fecha_ejecucion);
    ui->le_F_INST->setText(this->nullity_check(fecha_ejecucion));
    ui->le_emplazamiento_devuelto->setText(this->nullity_check(o.value(emplazamiento_devuelto).toString()));
    ui->le_RESTO_EM->setText(this->nullity_check(o.value(RESTO_EM).toString()));

    QString obs = o.value(observaciones_devueltas).toString();
    QStringList listObs;
    listObs = obs.split("\n");
    for (int i=0; i < listObs.size(); i++) {
        if(!checkIfFieldIsValid(listObs.at(i))){
            listObs.removeAt(i);
            i--;
        }
    }
    if(!list.isEmpty()){
        ui->le_observaciones_devueltas->addItems(listObs);
    }

    QString state = o.value(Estado).toString();
    if(state.contains("NORMAL")){
        ui->widget_fotos_normales->show();
        ui->widget_fotos_incidencias->hide();
    }else if (state.contains("INCIDENCIA")){
        ui->widget_fotos_incidencias->show();
        ui->widget_fotos_normales->hide();
    }
    ui->le_marca_devuelta->setText(this->nullity_check(o.value(marca_devuelta).toString()));
    ui->le_marca_devuelta->setToolTip(this->nullity_check(o.value(marca_devuelta).toString()));
    ui->le_RUEDASDV->setText(this->nullity_check(o.value(RUEDASDV).toString()));
    ui->le_LARGO->setText(this->nullity_check(o.value(LARGO).toString()));
    //    if(!longitud_desde_cal.isEmpty()){
    //        ui->le_LARGO->setText(longitud_desde_cal);
    //    }
    ui->le_largo_devuelto->setText(this->nullity_check(o.value(largo_devuelto).toString()));
    ui->le_numero_serie_contador_devuelto->setText(this->nullity_check(o.value(numero_serie_contador_devuelto).toString()));
    ui->le_AREALIZAR_devuelta->setText(this->nullity_check(o.value(AREALIZAR_devuelta).toString()));
    ui->le_intervencion_devuelta->setText(this->nullity_check(o.value(intervencion_devuelta).toString()));

    QString fecha_cierre = o.value(FECH_CIERRE).toString().trimmed();
    fecha_cierre = transformarFecha(fecha_cierre);
    ui->le_FECH_CIERRE->setText(this->nullity_check(fecha_cierre));
    QString tipoOrden = o.value(TIPORDEN).toString().trimmed();
    if(!checkIfFieldIsValid(tipoOrden)){
        tipoOrden = "ESPECIALES";
    }
    tipoOrden = convertirTipoOrdenToShort(tipoOrden);
    ui->le_TIPORDEN->setText(this->nullity_check(tipoOrden));
    ui->le_TIPO_FLUIDO->setText(this->nullity_check(o.value(TIPOFLUIDO).toString()));
    ui->le_idexport->setText(this->nullity_check(o.value(idexport).toString()));
    QString fecha_informacion = o.value(fech_informacionnew).toString().trimmed();
    fecha_informacion = transformarFecha(fecha_informacion);
    ui->le_fech_informacionnew->setText(this->nullity_check(fecha_informacion));
    ui->le_tipoRadio_levantado->setText(this->nullity_check(o.value(tipoRadio).toString()));
    ui->le_tipoRadio->setText(this->nullity_check(o.value(tipoRadio_devuelto).toString()));
    ui->le_marcaR->setText(this->nullity_check(o.value(marcaR).toString()));
    ui->le_tipo_tarea->setText(this->nullity_check(o.value(tipo_tarea).toString()));

    ui->le_TIPO_DEVUELTO->setText(this->nullity_check(o.value(TIPO_devuelto).toString()));
    ui->le_TIPO_FLUIDO_DEVUELTO->setText(this->nullity_check(o.value(TIPOFLUIDO_devuelto).toString()));
    ui->le_ID_FINCA->setText(this->nullity_check(o.value(ID_FINCA).toString()));
    ui->le_COMENTARIOS->setText(this->nullity_check(o.value(COMENTARIOS).toString()));
    ui->le_DNI_CIF_COMUNIDAD->setText(this->nullity_check(o.value(DNI_CIF_COMUNIDAD).toString()));
    ui->le_TARIFA->setText(this->nullity_check(o.value(TARIFA).toString()));
    ui->le_TOTAL_CONTADORES->setText(this->nullity_check(o.value(TOTAL_CONTADORES).toString()));
    ui->le_C_CANAL->setText(this->nullity_check(o.value(C_CANAL).toString()));
    ui->le_C_LYC->setText(this->nullity_check(o.value(C_LYC).toString()));
    ui->le_C_AGRUPA->setText(this->nullity_check(o.value(C_AGRUPA).toString()));
    ui->le_DNI_CIF_ABONADO->setText(this->nullity_check(o.value(DNI_CIF_ABONADO).toString()));
    ui->le_C_COMUNERO->setText(this->nullity_check(o.value(C_COMUNERO).toString()));
    ui->le_MENSAJE_LIBRE->setText(this->nullity_check(o.value(MENSAJE_LIBRE).toString()));
    ui->le_TIPO->setText(this->nullity_check(o.value(TIPO).toString()));

    //    if(ui->le_emplazamiento_devuelto->text().isEmpty()){ // Si la tarea no esta ejecutado esto puede confundir si no se ha hecho nada del trabajo
    //       ui->le_emplazamiento_devuelto->setText(ui->le_emplazamiento->text());
    //       ui->le_RESTO_EM->setText(ui->le_RESTEMPLAZA->text());
    //    }

    ui->le_numero_precinto->setText(this->nullity_check(o.value(numero_precinto).toString()));
}
bool other_task_screen::populateView(bool load_photos)
{

    populateDataView();//popular los datos de la tarea

    highlightMapIcon();
    ////////////////////////////////////////////////////cargar las 8 fotos//////////////////////////////////////////////////////////////////
    if(load_photos){ //load_photos //habilita la descarga de fotos y audio

        loadLocalPhotos();
        checkDisponibleAudio();

        int cant_fotos = 0;
        QStringList keys, values, all_values;
        keys << "nombre" << "tipo" << GESTOR << ANOMALIA << numero_abonado << "empresa";
        if(checkIfFieldIsValid(o.value(foto_antes_instalacion).toString())){
            all_values << o.value(foto_antes_instalacion).toString().trimmed() << foto_antes_instalacion << o.value(GESTOR).toString().trimmed() << o.value(numero_abonado).toString().trimmed();
            cant_fotos++;
        }
        if(checkIfFieldIsValid(o.value(foto_numero_serie).toString())){
            all_values<< o.value(foto_numero_serie).toString().trimmed() << foto_numero_serie << o.value(GESTOR).toString().trimmed() << o.value(numero_abonado).toString().trimmed();
            cant_fotos++;
        }
        if(checkIfFieldIsValid(o.value(foto_lectura).toString())){
            all_values<< o.value(foto_lectura).toString().trimmed() << foto_lectura << o.value(GESTOR).toString().trimmed() << o.value(numero_abonado).toString().trimmed();
            cant_fotos++;
        }
        if(checkIfFieldIsValid(o.value(foto_despues_instalacion).toString())){
            all_values<< o.value(foto_despues_instalacion).toString().trimmed() << foto_despues_instalacion << o.value(GESTOR).toString().trimmed() << o.value(numero_abonado).toString().trimmed();
            cant_fotos++;
        }
        if(checkIfFieldIsValid(o.value(foto_incidencia_1).toString())){
            all_values<< o.value(foto_incidencia_1).toString().trimmed() << foto_incidencia_1 << o.value(GESTOR).toString().trimmed() << o.value(numero_abonado).toString().trimmed();
            cant_fotos++;
        }
        if(checkIfFieldIsValid(o.value(foto_incidencia_2).toString())){
            all_values<< o.value(foto_incidencia_2).toString().trimmed() << foto_incidencia_2 << o.value(GESTOR).toString().trimmed() << o.value(numero_abonado).toString().trimmed();
            cant_fotos++;
        }
        if(checkIfFieldIsValid(o.value(foto_incidencia_3).toString())){
            all_values<< o.value(foto_incidencia_3).toString().trimmed() << foto_incidencia_3 << o.value(GESTOR).toString().trimmed() << o.value(numero_abonado).toString().trimmed();
            cant_fotos++;
        }
        if(checkIfFieldIsValid(o.value(firma_cliente).toString())){
            all_values<< o.value(firma_cliente).toString().trimmed()<< firma_cliente << o.value(GESTOR).toString().trimmed() << o.value(numero_abonado).toString().trimmed();
            cant_fotos++;
        }

        QList<my_label *> my_labels;
        my_labels.append(ui->lb_foto_antes_instalacion);
        my_labels.append(ui->lb_foto_numero_serie);
        my_labels.append(ui->lb_foto_lectura);
        my_labels.append(ui->lb_foto_despues_instalacion);
        my_labels.append(ui->lb_foto_incidencia_1);
        my_labels.append(ui->lb_foto_incidencia_2);
        my_labels.append(ui->lb_foto_incidencia_3);
        my_labels.append(ui->lb_firma_cliente);

        for(int i = 0, reintentos = 0; i < cant_fotos; i++)
        {
            if(closing_window){
                return false;
            }
            connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                    this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
            //OJO ESPERAR POR RESPUESTA
            QEventLoop q;
            connect(this, &other_task_screen::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            values.clear();
            QString nombre = all_values[i*4];
            QString tipo = all_values[i*4+1];
            QString gestor = all_values[i*4+2];
            QString numAbonado = all_values[i*4+3];
            QString anomalia = o.value(ANOMALIA).toString().trimmed();
            values << nombre << tipo << gestor << anomalia << numAbonado << empresa;
            QString selected = tipo;
            if(selected.contains(foto_antes_instalacion)){
                currentPhotoLooking = 1;
                my_label *label = ui->lb_foto_antes_instalacion;
                displayLoading(label);
            }
            else if(selected == foto_numero_serie){
                currentPhotoLooking = 2;
                my_label *label = ui->lb_foto_numero_serie;
                displayLoading(label);
            }
            else if(selected == foto_lectura){
                currentPhotoLooking = 3;
                my_label *label = ui->lb_foto_lectura;
                displayLoading(label);
            }
            else if(selected == foto_despues_instalacion){
                currentPhotoLooking = 4;
                my_label *label = ui->lb_foto_despues_instalacion;
                displayLoading(label);
            }
            else if(selected == foto_incidencia_1){
                currentPhotoLooking = 5;
                my_label *label = ui->lb_foto_incidencia_1;
                displayLoading(label);
            }
            else if(selected == foto_incidencia_2){
                currentPhotoLooking = 6;
                my_label *label = ui->lb_foto_incidencia_2;
                displayLoading(label);
            }
            else if(selected == foto_incidencia_3){
                currentPhotoLooking = 7;
                my_label *label = ui->lb_foto_incidencia_3;
                displayLoading(label);
            }
            else if(selected == firma_cliente){
                currentPhotoLooking = 8;
                my_label *label = ui->lb_firma_cliente;
                displayLoading(label);
            }

            this->values = values;
            QTimer::singleShot(DELAY, this, SLOT(download_task_image_request()));

            switch (q.exec())
            {
            case database_comunication::script_result::timeout:
                if(!closing_window){
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        GlobalFunctions gf(this);
                        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = cant_fotos;
                    }
                }
                break;

            case database_comunication::script_result::download_task_image_failed:
                if(!closing_window){
                    if(currentPhotoLooking > 0 && currentPhotoLooking < 9){
                        my_labels[currentPhotoLooking-1]->setDefaultPhoto(true);
                    }
                    loadLocalPhoto(currentPhotoLooking);
                    emit hidingLoading();
                }
                break;

            case database_comunication::script_result::download_task_image_picture_doesnt_exists:
                if(!closing_window){
                    if(currentPhotoLooking > 0 && currentPhotoLooking < 9){
                        my_labels[currentPhotoLooking-1]->setDefaultPhoto(true);
                    }
                    loadLocalPhoto(currentPhotoLooking);
                    emit hidingLoading();
                }
                break;

            case database_comunication::script_result::ok:
                if(closing_window){
                    return false;
                }
                emit hidingLoading();
                break;
            }
        }

        emit imagesDownloaded();

        if(!closing_window){
            createAlternativePhotos();

            if(descargarAudio()){
                ui->pb_play_audio->show();
            }
        }
    }

    currentPhotoLooking=0;

    //    if(conexion_activa && load_photos){
    //        emit getContadoresFromServer();
    //    }
    //////////////////////////////////////////////////////////////////////////////////////end cargar fotos
    if(!closing_window){
        ui->pb_update_server_info->setEnabled(true);
        return true;
    }
    return false;
}
void other_task_screen::displayLoading(my_label *label){
    if(label->isDefaultPhoto()){
        label->setText(" ");
    }
    QPoint p = label->pos();
    p.setX(p.x() + label->width()/2);
    p.setY(p.y() + label->height()/2);
    show_loading(label->parentWidget(), p);
}
void other_task_screen::show_loading(QWidget *parent, QPoint pos, QColor color, int w, int h, bool show_white_background){
    emit hidingLoading();

    int border = 1;
    pos.setX(pos.x()-w/2);
    pos.setY(pos.y()-h/2);
    if(show_white_background){
        border = 12;
        QLabel *label_back = new QLabel(parent);
        connect(this, &other_task_screen::hidingLoading, label_back, &QLabel::hide);
        connect(this, &other_task_screen::hidingLoading, label_back, &QLabel::deleteLater);
        label_back->setFixedSize(w + border, h + border);
        label_back->move(pos);
        QString circle_radius_string = QString::number(static_cast<int>((w+border)/2));
        //    QString colorRBG = getColorString(color);
        label_back->setStyleSheet("background-color: #FFFFFF;"
                                  "border-radius: "+circle_radius_string+"px;");
        label_back->show();
    }

    QProgressIndicator *pi = new QProgressIndicator(parent);
    connect(this, &other_task_screen::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &other_task_screen::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color);
    pi->setFixedSize(w, h);
    pi->startAnimation();
    pos.setX(pos.x()+border/2 + 1);
    pos.setY(pos.y()+border/2 + 1);
    pi->move(pos);
    pi->show();
}

void other_task_screen::show_loading(QString mess){
    emit hidingLoading();

    QWidget *widget_blur = new QWidget(this);
    widget_blur->setFixedSize(this->size()+QSize(0,30));
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &other_task_screen::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &other_task_screen::hidingLoading, widget_blur, &QWidget::deleteLater);

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
    connect(this, &other_task_screen::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &other_task_screen::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &other_task_screen::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &other_task_screen::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &other_task_screen::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}
void other_task_screen::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void other_task_screen::hide_loading(){
    emit hidingLoading();
}
QString other_task_screen::getColorString(QColor color, bool hex){
    int r, g, b;
    color.getRgb(&r, &g, &b);
    QString rgb="";
    if(hex){
        rgb = QString::number(r, 16) + QString::number(g, 16) + QString::number(b, 16);
    }else{
        rgb = QString::number(r) + "," + QString::number(g) + "," + QString::number(b);
        rgb = "rgb("+ rgb +")";
    }
    return rgb;
}
bool other_task_screen::descargarAudio(){
    QString audio = o.value(audio_detalle).toString().trimmed();
    QString gestor_actual = o.value(GESTOR).toString().trimmed();
    QString anomalia = o.value(ANOMALIA).toString().trimmed();
    QString numAbonado = o.value(numero_abonado).toString().trimmed();

    bool retorno = false;

    if(checkIfFieldIsValid(audio)){
        QStringList keys, values;
        keys << "url" << "dirLocal";
        values << "Empresas/"+empresa+"/Gestores/"+gestor_actual+"/fotos_tareas/" + numAbonado + "/" + anomalia + "/" + audio;
        values << getLocalDirofCurrentTask();

        connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        //OJO ESPERAR POR RESPUESTA
        QEventLoop q;

        connect(this, &other_task_screen::script_excecution_result,&q,&QEventLoop::exit);

        this->keys = keys;
        this->values = values;
        QTimer::singleShot(DELAY, this, &other_task_screen::download_audio_request);

        switch (q.exec())
        {
        case database_comunication::script_result::timeout:
            retorno = false;
            break;

        case database_comunication::script_result::download_audio_failed:
            retorno = false;
            break;

        case database_comunication::script_result::download_audio_doesnt_exists:
            retorno = false;
            break;

        case database_comunication::script_result::ok:
            break;
        }
    }
    return retorno;

}
void other_task_screen::download_audio_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_FILE,keys,values);
}
QString other_task_screen::getLocalDirofCurrentTask(){
    QString path = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/";
    QString gestor_actual = o.value(GESTOR).toString().trimmed();
    QString anomalia = o.value(ANOMALIA).toString().trimmed();
    QString numAbonado = o.value(numero_abonado).toString().trimmed();
    if(!checkIfFieldIsValid(gestor_actual)){
        gestor_actual = "Sin_Gestor";
    }
    QDir dir;
    dir.setPath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" + anomalia);
    if(!dir.exists()){
        dir.mkpath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" + anomalia);
    }
    return dir.path();
}

void other_task_screen::createAlternativePhotos(){

    QString idOrden = o.value(idOrdenCABB).toString().trimmed();
    if(checkIfFieldIsValid(idOrden)){
        QString path = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/";
        QString gestor_actual = o.value(GESTOR).toString().trimmed();
        QString anomalia = o.value(ANOMALIA).toString().trimmed();
        QString numAbonado = o.value(numero_abonado).toString().trimmed();
        if(!checkIfFieldIsValid(gestor_actual)){
            gestor_actual = "Sin_Gestor";
        }
        QDir dir;
        dir.setPath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" + anomalia);
        if(!dir.exists()){
            dir.setPath(path + gestor_actual + "/fotos_tareas/" + numAbonado);
        }
        QDir idOrdenDir(dir);
        idOrdenDir.setPath(dir.path() + "/idOrden - " + idOrden);
        if(!idOrdenDir.exists()){
            idOrdenDir.mkpath(dir.path() + "/idOrden - " + idOrden);
        }

        QString fileName;
        QString newFileName;
        QStringList files = dir.entryList(QDir::Files);
        foreach(fileName, files){
            if(fileName.contains(".jpg")){
                newFileName = idOrden + "_" + fileName;
                QFile file(idOrdenDir.path() + "/" + newFileName);
                if(file.exists()){
                    file.remove();
                }
                QFile::copy(dir.path() + "/" + fileName, idOrdenDir.path() + "/" + newFileName);
            }
        }
    }
}
void other_task_screen::on_pb_mostrar_carpeta_clicked()
{
    QString path = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/";
    QString gestor_actual = o.value(GESTOR).toString().trimmed();
    QString anomalia = o.value(ANOMALIA).toString().trimmed();
    QString numAbonado = o.value(numero_abonado).toString().trimmed();
    if(!checkIfFieldIsValid(gestor_actual)){
        gestor_actual = "Sin_Gestor";
    }
    QDir dir;
    dir.setPath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" + anomalia);
    if(!dir.exists()){
        dir.setPath(path + gestor_actual + "/fotos_tareas/" + numAbonado);
    }
    GlobalFunctions::showInExplorer(dir.path());
}

QString other_task_screen::convertirTipoOrdenToShort(QString orden_long){
    if(orden_long == "DIARIAS"){
        return "D";
    }
    else if(orden_long == "MASIVAS"){
        return "M";
    }
    else if(orden_long == "ESPECIALES"){
        return "E";
    }
    return "E";
}
QString other_task_screen::convertirTipoOrdenToLong(QString orden_short){
    if(orden_short == "D"){
        return "DIARIAS";
    }
    else if(orden_short == "M"){
        return "MASIVAS";
    }
    else if(orden_short == "E"){
        return "ESPECIALES";
    }
    return "ESPECIALES";
}

void other_task_screen::createAutoPDF(bool show, bool force_create){
    if(!this->isHidden() || force_create){
        QString path = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/";
        QString gestor_actual = o.value(GESTOR).toString().trimmed();
        if(!checkIfFieldIsValid(gestor_actual)){
            gestor_actual = "Sin_Gestor";
        }
        QString anomalia = o.value(ANOMALIA).toString().trimmed();
        QString numAbonado = o.value(numero_abonado).toString().trimmed();
        QDir dir;
        dir.setPath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" +anomalia);
        if(!dir.exists()){
            dir.mkpath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" +anomalia);
        }
        filename = dir.path() +"/Pdf_de_Trabajo.pdf";

        configuraAndCreatePdf(show);
    }
}

QString other_task_screen::get_coordinades_From_Link_GoogleMaps(const QString &link_google_maps)
{
    //    QString link_google_maps = "https://www.google.com/maps/dir//23.0504409,-82.5170422/@23.0382777,-82.5225353,13z";
    if(link_google_maps.contains("dir//")){
        if(link_google_maps.split("dir//").size() >= 2){
            QString posicion_string = link_google_maps.split("dir//").at(1);
            if(posicion_string.contains("/@")){
                QString latLang = posicion_string.split("/@").at(0);
                return latLang;
            }
        }
    }
    return "...";
}

bool other_task_screen::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}

void other_task_screen::toogleEdit(bool toogle, bool new_task)
{
    //    ui->cb_editar_fotos->setEnabled(toogle);
    ui->le_TOTAL_ORDENES->setEnabled(toogle);
    ui->le_poblacion->setEnabled(toogle);
    if(new_task){
        ui->cb_gestor->setEnabled(true);
    }else{
        ui->cb_gestor->setEnabled(false);
    }
    ui->le_calle->setEnabled(toogle);
    ui->le_piso->setEnabled(toogle);
    ui->le_mano->setEnabled(toogle);
    if(new_task){
        ui->le_numero_serie_contador->setEnabled(true);
    }
    else {
        ui->le_numero_serie_contador->setEnabled(false);
    }

    ui->le_causa_origen->setEnabled(toogle);
    ui->le_accion_ordenada->setEnabled(toogle);

    ui->le_calibre_toma->setEnabled(toogle);
    ui->le_calibre_toma_real->setEnabled(toogle);
    ui->le_operario->setEnabled(toogle);
    ui->le_emplazamiento->setEnabled(toogle);
    ui->le_observaciones->setEnabled(toogle);
    ui->le_actividad->setEnabled(toogle);
    ui->le_nombre_cliente->setEnabled(toogle);
    ui->le_apellido1->setEnabled(toogle);
    ui->le_apellido2->setEnabled(toogle);
    //    ui->le_numero_abonado->setEnabled(toogle);

    ui->le_telefono1->setEnabled(toogle);
    ///siempre desabilitado
    ui->le_telefono2->setEnabled(toogle);
    //    ui->le_fecha_tocado_puerta->setEnabled(toogle);
    //    ui->le_fechas_nota_aviso->setEnabled(toogle);
    ui->le_acceso->setEnabled(toogle);
    ui->le_resultado->setEnabled(toogle);
    ui->le_nuevo_citas->setEnabled(toogle);

    if(ui->le_FECINST->text().isEmpty()){
        ui->le_FECINST->setEnabled(toogle);
    }else {
        ui->le_FECINST->setEnabled(false);
    }

    //    ui->le_FECINST->setEnabled(false);
    ui->le_zona->setEnabled(toogle);
    ui->le_ruta->setEnabled(toogle);
    if(new_task){
        ui->le_marca_contador->setEnabled(true);
    }
    else {
        ui->le_marca_contador->setEnabled(false);
    }

    ui->le_codigo_geolocalizacion->setEnabled(toogle);
    ui->le_numero_serie_modulo->setEnabled(toogle);
    ui->le_lectura_ultima->setEnabled(toogle);
    ui->le_lectura_actual->setEnabled(toogle);
    ui->le_geolocalizacion->setEnabled(toogle);
    ui->le_url_google_maps->setEnabled(false); ///siempre desabilitado
    ui->le_LECTURA_CONTADOR_NUEVO->setEnabled(toogle);
    ui->le_ubicacion_en_bateria->setEnabled(toogle);
    ui->le_propiedad->setEnabled(toogle);
    ui->le_ruedas->setEnabled(toogle);
    ui->le_numero->setEnabled(toogle);
    ui->le_numero_interno->setEnabled(toogle);



    //CAMPOS AÑADIDOS
    ui->le_idOrdenCABB->setEnabled(toogle);

    ui->le_BIS->setEnabled(toogle);
    ui->le_FECEMISIO->setEnabled(false);


    if(ui->le_F_INST->text().isEmpty()){
        ui->le_F_INST->setEnabled(toogle);
    }else {
        ui->le_F_INST->setEnabled(false);
    }

    //    ui->le_F_INST->setEnabled(false);
    ui->le_emplazamiento_devuelto->setEnabled(toogle);
    ui->le_RESTO_EM->setEnabled(toogle);
    ui->le_observaciones_devueltas->setEnabled(toogle);
    ui->le_marca_devuelta->setEnabled(toogle);
    ui->le_RUEDASDV->setEnabled(toogle);
    ui->le_LARGO->setEnabled(toogle);
    ui->le_largo_devuelto->setEnabled(toogle);
    ui->le_numero_serie_contador_devuelto->setEnabled(toogle);
    ui->le_AREALIZAR_devuelta->setEnabled(toogle);
    ui->le_intervencion_devuelta->setEnabled(toogle);

    if(ui->le_FECH_CIERRE->text().isEmpty()){
        ui->le_FECH_CIERRE->setEnabled(toogle);
    }else {
        ui->le_FECH_CIERRE->setEnabled(false);
    }

    ui->le_TIPORDEN->setEnabled(toogle);
    ui->le_TIPO_FLUIDO->setEnabled(toogle);
    ui->le_idexport->setEnabled(toogle);

    if(ui->le_fech_informacionnew->text().isEmpty()){
        ui->le_fech_informacionnew->setEnabled(toogle);
    }else {
        ui->le_fech_informacionnew->setEnabled(false);
    }

    //    ui->le_fech_informacionnew->setEnabled(false);
    ui->le_tipoRadio_levantado->setEnabled(toogle);
    ui->le_tipoRadio->setEnabled(toogle);
    ui->le_marcaR->setEnabled(toogle);
    ui->le_tipo_tarea->setEnabled(toogle);

    ui->le_TIPO_DEVUELTO->setEnabled(toogle);
    ui->le_TIPO_FLUIDO_DEVUELTO->setEnabled(toogle);
    ui->le_ID_FINCA->setEnabled(toogle);
    ui->le_COMENTARIOS->setEnabled(toogle);
    ui->le_DNI_CIF_COMUNIDAD->setEnabled(toogle);
    ui->le_TARIFA->setEnabled(toogle);
    ui->le_TOTAL_CONTADORES->setEnabled(toogle);
    ui->le_C_CANAL->setEnabled(toogle);
    ui->le_C_LYC->setEnabled(toogle);
    ui->le_C_AGRUPA->setEnabled(toogle);
    ui->le_DNI_CIF_ABONADO->setEnabled(toogle);
    ui->le_C_COMUNERO->setEnabled(toogle);
    ui->le_MENSAJE_LIBRE->setEnabled(toogle);
    ui->le_TIPO->setEnabled(toogle);
}

void other_task_screen::getData(QJsonObject data)
{
    o = data;
    tarea_a_actualizar = o;

    if(checkIfExistFoto(o)){
        ui->widget_fotos->show();
    }else {
        //        ui->widget_fotos->hide();
    }
    if(o.value(status_tarea).toString() == state_cerrada
            || o.value(status_tarea).toString() == state_informada){
        //ui->pb_cerrar_tarea->hide(); //esconder boton para tareas que ya fueron cerradas
    }

    QString numSerie = o.value(numero_serie_contador).toString();
    if(checkIfFieldIsValid(numSerie)){
        numSerie = "NÚMERO SERIE RETIRADO:  "+numSerie;
    }else {
        numSerie = "";
    }
    QString numSerieDV = o.value(numero_serie_contador_devuelto).toString();
    if(!checkIfFieldIsValid(numSerieDV)){
        numSerieDV = numSerie;
    }else {
        numSerieDV = "NÚMERO SERIE INSTALADO:  " + numSerieDV;
    }
    QString lectura_cont = o.value(lectura_actual).toString();
    if(!checkIfFieldIsValid(lectura_cont)){
        lectura_cont = o.value(lectura_ultima).toString();
    }
    if(checkIfFieldIsValid(lectura_cont)){
        lectura_cont = "      LECTURA DE CONTADOR RETIRADO:  " +  lectura_cont;
    }else{
        lectura_cont = "";
    }
    QString lectura_cont_nuevo = o.value(lectura_contador_nuevo).toString();
    if(checkIfFieldIsValid(lectura_cont_nuevo)){
        lectura_cont_nuevo = "       LECTURA DE CONTADOR INSTALADO:  " +  lectura_cont_nuevo;
    }else{
        lectura_cont_nuevo = "";
    }
    QString firmante = o.value(nombre_firmante).toString();
    if(checkIfFieldIsValid(firmante)){
        firmante = o.value(nombre_cliente).toString();
    }

    ui->lb_foto_lectura->setTextInfo(numSerie + lectura_cont);
    ui->lb_foto_incidencia_1->setTextInfo(numSerie + lectura_cont);
    ui->lb_foto_incidencia_2->setTextInfo(numSerie + lectura_cont);
    ui->lb_foto_incidencia_3->setTextInfo(numSerie + lectura_cont);
    ui->lb_foto_numero_serie->setTextInfo(numSerie + lectura_cont + "           " + numSerieDV + lectura_cont_nuevo);
    ui->lb_foto_antes_instalacion->setTextInfo(numSerie + lectura_cont);
    ui->lb_foto_despues_instalacion->setTextInfo(numSerieDV + lectura_cont_nuevo);
    ui->lb_firma_cliente->setTextInfo(nullity_check(firmante));

    ui->lb_foto_lectura->setJsonInfo(o);
    ui->lb_foto_incidencia_1->setJsonInfo(o);
    ui->lb_foto_incidencia_2->setJsonInfo(o);
    ui->lb_foto_incidencia_3->setJsonInfo(o);
    ui->lb_foto_numero_serie->setJsonInfo(o);
    ui->lb_foto_antes_instalacion->setJsonInfo(o);
    ui->lb_foto_despues_instalacion->setJsonInfo(o);
    ui->lb_firma_cliente->setJsonInfo(o);

}

void other_task_screen::on_pb_edit_clicked()
{
    toogleEdit(true);
}

void other_task_screen::fill_counter_data(QString serie){
    GlobalFunctions gf(this, empresa);
    QJsonObject jsonObject = gf.getContadorFromServer(serie);
    if(!jsonObject.isEmpty()){
        QString serieTarea = o.value(numero_serie_contador).toString().trimmed();
        if(serieTarea != serie){
            QString status_cont = jsonObject.
                    value(status_contadores).toString().trimmed();
            if(status_cont == "INSTALLED"){
                ui->le_numero_serie_contador_devuelto->setText(serieTarea);
                GlobalFunctions gf(this);
                GlobalFunctions::showWarning(this, "Contador no disponible",
                                             "Este contador no esta disponible, fue utilizado en otro instalacion");
                fill_counter_data(serieTarea);
                return;
            }
        }
        ui->le_tipoRadio_levantado->setText(jsonObject.
                                            value(tipo_radio_contadores).toString());

        QString marca_modelo_y_codigo  =  jsonObject.
                value(codigo_marca_contadores).toString().trimmed() + " - "+ jsonObject.
                value(marca_contadores).toString().trimmed()+ " - "+ jsonObject.
                value(modelo_contadores).toString().trimmed();
        ui->le_marca_contador->setText(marca_modelo_y_codigo);
        ui->le_marca_contador->setToolTip(marca_modelo_y_codigo);

        ui->le_calibre_toma->setText(jsonObject.
                                     value(calibre_contadores).toString());
        ui->le_LARGO->setText(jsonObject.
                              value(longitud_contadores).toString());
        ui->le_ruedas->setText(jsonObject.
                               value(ruedas_contador_contadores).toString());
        ui->le_TIPO_FLUIDO->setText(jsonObject.
                                    value(tipo_fluido_contadores).toString());
        QString clase_y_codigo  =  jsonObject.
                value(codigo_clase_contadores).toString().trimmed() + " - "+ jsonObject.
                value(clase_contadores).toString().trimmed();
        ui->le_TIPO->setText(clase_y_codigo);
    }
}
void other_task_screen::fill_counter_data_devuelto(QString serie){
    GlobalFunctions gf(this, empresa);
    QJsonObject jsonObject = gf.getContadorFromServer(serie);
    if(!jsonObject.isEmpty()){
        QString serieDVTarea = o.value(numero_serie_contador_devuelto).toString().trimmed();
        if(serieDVTarea != serie){
            QString status_cont = jsonObject.
                    value(status_contadores).toString().trimmed();
            if(status_cont == "INSTALLED"){
                ui->le_numero_serie_contador_devuelto->setText(serieDVTarea);
                GlobalFunctions gf(this);
                GlobalFunctions::showWarning(this, "Contador no disponible",
                                             "Este contador no esta disponible, fue utilizado en otro instalacion");
                fill_counter_data_devuelto(serieDVTarea);
                return;
            }
        }
        if(checkIfFieldIsValid(jsonObject.value(lectura_inicial_contadores).toString())){
            ui->le_LECTURA_CONTADOR_NUEVO->setText(jsonObject.value(lectura_inicial_contadores).toString());
        }
        ui->le_tipoRadio->setText(jsonObject.
                                  value(tipo_radio_contadores).toString());

        QString marca_modelo_y_codigo  = jsonObject.
                value(codigo_marca_contadores).toString().trimmed() + " - "+ jsonObject.
                value(marca_contadores).toString().trimmed()+ " - "+ jsonObject.
                value(modelo_contadores).toString().trimmed();
        ui->le_marca_devuelta->setText(marca_modelo_y_codigo);
        ui->le_marca_devuelta->setToolTip(marca_modelo_y_codigo);

        ui->le_calibre_toma_real->setText(jsonObject.
                                          value(calibre_contadores).toString());
        ui->le_largo_devuelto->setText(jsonObject.
                                       value(longitud_contadores).toString());
        ui->le_RUEDASDV->setText(jsonObject.
                                 value(ruedas_contador_contadores).toString());
        ui->le_TIPO_FLUIDO_DEVUELTO->setText(jsonObject.
                                             value(tipo_fluido_contadores).toString());
        QString clase_y_codigo  =  jsonObject.
                value(codigo_clase_contadores).toString().trimmed() + " - "+ jsonObject.
                value(clase_contadores).toString().trimmed();
        ui->le_TIPO_DEVUELTO->setText(clase_y_codigo);
    }
}

QString other_task_screen::guardar_cambios()
{
    QString Numero_Interno = ui->le_numero_interno->text().trimmed();

    if(Numero_Interno.isEmpty() || Numero_Interno.isNull()
            || Numero_Interno.contains("null") || Numero_Interno.contains("NULL")){
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
        Numero_Interno = timestamp;
        ui->le_numero_interno->setText(timestamp);
    }
    tarea_a_actualizar.insert(ultima_modificacion, "ESCRITORIO "+ administrator_loged);
    tarea_a_actualizar.insert(prioridad,ui->l_prioridad->currentText().trimmed());
    QString default_day = ui->l_dia_predeterminado->currentText().trimmed();
    if(checkIfFieldIsValid(default_day)){
        if(default_day == "NO ASIGNADO"){
            default_day = "";
        }
        tarea_a_actualizar.insert(dia_predeterminado, default_day);
    }
    tarea_a_actualizar.insert(GESTOR,ui->cb_gestor->currentText().trimmed());
    tarea_a_actualizar.insert(numero_interno,ui->le_numero_interno->text().trimmed());
    tarea_a_actualizar.insert(poblacion,ui->le_poblacion->text().trimmed());
    tarea_a_actualizar.insert(calle,ui->le_calle->text().trimmed());

    tarea_a_actualizar.insert(numero_serie_contador,ui->le_numero_serie_contador->text().trimmed());
    QString operario_m = ui->le_operario->text().trimmed();
    if(checkIfFieldIsValid(operario_m)){
        last_operario = operario_m;
    }
    tarea_a_actualizar.insert(operario, operario_m);
    tarea_a_actualizar.insert(equipo, ui->le_equipo->text().trimmed());
    tarea_a_actualizar.insert(piso,ui->le_piso->text().trimmed());
    tarea_a_actualizar.insert(mano,ui->le_mano->text().trimmed());

    tarea_a_actualizar.insert(calibre_toma,ui->le_calibre_toma->text().trimmed());
    tarea_a_actualizar.insert(calibre_real,ui->le_calibre_toma_real->text().trimmed());
    QString Emplazamiento = ui->le_emplazamiento->text().trimmed();
    tarea_a_actualizar.insert(emplazamiento, Emplazamiento);
    if(!Emplazamiento.isEmpty() && Emplazamiento.contains("BA-")){
        tarea_a_actualizar.insert(acceso,"BAT");
    }else{
        tarea_a_actualizar.insert(acceso,ui->le_acceso->text().trimmed());
    }
    tarea_a_actualizar.insert(observaciones,ui->le_observaciones->text().trimmed());
    tarea_a_actualizar.insert(actividad,ui->le_actividad->text().trimmed());
    tarea_a_actualizar.insert(nombre_cliente,ui->le_nombre_cliente->text().trimmed()
                              //+ " " + ui->le_se->text()
                              + " " + ui->le_apellido1->text().trimmed()
                              + " " + ui->le_apellido2->text().trimmed());
    tarea_a_actualizar.insert(numero_abonado,ui->le_numero_abonado->text().trimmed());
    tarea_a_actualizar.insert(telefono1,ui->le_telefono1->text().trimmed());
    tarea_a_actualizar.insert(telefono2,ui->le_telefono2->text().trimmed());

    tarea_a_actualizar.insert(resultado,ui->le_resultado->text().trimmed());
    tarea_a_actualizar.insert(nuevo_citas,ui->le_nuevo_citas->text().trimmed());

    QString fecha_instalacion_vieja = ui->le_FECINST->text().trimmed();
    fecha_instalacion_vieja = transformarFecha_to_old_view(fecha_instalacion_vieja);
    tarea_a_actualizar.insert(fecha_instalacion, fecha_instalacion_vieja);

    tarea_a_actualizar.insert(zona,ui->le_zona->text().trimmed());
    tarea_a_actualizar.insert(ruta,ui->le_ruta->text().trimmed());
    tarea_a_actualizar.insert(marca_contador,ui->le_marca_contador->text().trimmed());
    tarea_a_actualizar.insert(codigo_de_geolocalizacion,ui->le_codigo_geolocalizacion->text().trimmed());
    tarea_a_actualizar.insert(numero_serie_modulo,ui->le_numero_serie_modulo->text().trimmed());
    tarea_a_actualizar.insert(lectura_ultima,ui->le_lectura_ultima->text().trimmed());
    tarea_a_actualizar.insert(lectura_actual,ui->le_lectura_actual->text().trimmed());
    tarea_a_actualizar.insert(lectura_contador_nuevo,ui->le_LECTURA_CONTADOR_NUEVO->text().trimmed());
    tarea_a_actualizar.insert(ubicacion_en_bateria,ui->le_ubicacion_en_bateria->text().trimmed());
    tarea_a_actualizar.insert(propiedad,ui->le_propiedad->text().trimmed());
    tarea_a_actualizar.insert(ruedas,ui->le_ruedas->text().trimmed());
    tarea_a_actualizar.insert(numero,ui->le_numero->text().trimmed());
    QString status_task = tarea_a_actualizar.value(status_tarea).toString();
    if(!checkIfFieldIsValid(status_task))
    {
        tarea_a_actualizar.insert(status_tarea,"IDLE");
    }
    else
    {
        if(status_task.contains("TO_UPLOAD") || status_task.contains("TO_UPDATE")){
            status_task=status_task.replace("TO_UPLOAD", "")
                    .replace("TO_UPDATE","").replace(",","").trimmed();
        }
        tarea_a_actualizar.insert(status_tarea,status_task);
    }
    tarea_a_actualizar.insert(date_time_modified,QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    //CAMPOS AÑADIDOS
    tarea_a_actualizar.insert(causa_origen, ui->le_causa_origen->text().trimmed());
    tarea_a_actualizar.insert(accion_ordenada,ui->le_accion_ordenada->text().trimmed());
    tarea_a_actualizar.insert(BIS,ui->le_BIS->text().trimmed());
    QString fecha_emision = ui->le_FECEMISIO->text().trimmed();
    fecha_emision = transformarFecha_to_old_view(fecha_emision);
    tarea_a_actualizar.insert(FECEMISIO, fecha_emision);
    QString fecha_ejecucion = ui->le_F_INST->text().trimmed();
    if(checkIfFieldIsValid(fecha_ejecucion)){
        last_fecha_ejecucion = fecha_ejecucion;
    }
    fecha_ejecucion = transformarFecha_to_old_view(fecha_ejecucion);
    tarea_a_actualizar.insert(F_INST,fecha_ejecucion);
    tarea_a_actualizar.insert(emplazamiento_devuelto,ui->le_emplazamiento_devuelto->text().trimmed());
    tarea_a_actualizar.insert(RESTO_EM,ui->le_RESTO_EM->text().trimmed());
    QStringList list = ui->le_observaciones_devueltas->items();
    if(!list.isEmpty()){
        tarea_a_actualizar.insert(observaciones_devueltas,list.join("\n"));
    }else{
        tarea_a_actualizar.insert(observaciones_devueltas,"");
    }

    tarea_a_actualizar.insert(marca_devuelta,ui->le_marca_devuelta->text().trimmed());
    tarea_a_actualizar.insert(RUEDASDV,ui->le_RUEDASDV->text().trimmed());
    tarea_a_actualizar.insert(LARGO,ui->le_LARGO->text().trimmed());
    tarea_a_actualizar.insert(largo_devuelto,ui->le_largo_devuelto->text().trimmed());
    tarea_a_actualizar.insert(numero_serie_contador_devuelto,ui->le_numero_serie_contador_devuelto->text().trimmed());
    tarea_a_actualizar.insert(AREALIZAR_devuelta,ui->le_AREALIZAR_devuelta->text().trimmed());
    tarea_a_actualizar.insert(intervencion_devuelta,ui->le_intervencion_devuelta->text().trimmed());

    QString fecha_cierre = ui->le_FECH_CIERRE->text().trimmed();
    fecha_cierre = transformarFecha_to_old_view(fecha_cierre);
    tarea_a_actualizar.insert(FECH_CIERRE,fecha_cierre);
    QString tipoOrden = ui->le_TIPORDEN->text().trimmed();
    tipoOrden = convertirTipoOrdenToLong(tipoOrden);
    tarea_a_actualizar.insert(TIPORDEN,tipoOrden);
    tarea_a_actualizar.insert(TIPOFLUIDO,ui->le_TIPO_FLUIDO->text().trimmed());
    tarea_a_actualizar.insert(idexport,ui->le_idexport->text().trimmed());

    QString fecha_informacion = ui->le_fech_informacionnew->text().trimmed();
    fecha_informacion = transformarFecha_to_old_view(fecha_informacion);
    tarea_a_actualizar.insert(fech_informacionnew, fecha_informacion);
    tarea_a_actualizar.insert(tipoRadio,ui->le_tipoRadio_levantado->text().trimmed());
    tarea_a_actualizar.insert(tipoRadio_devuelto,ui->le_tipoRadio->text().trimmed());
    tarea_a_actualizar.insert(marcaR,ui->le_marcaR->text().trimmed());
    tarea_a_actualizar.insert(tipo_tarea,ui->le_tipo_tarea->text().trimmed());

    tarea_a_actualizar.insert(TIPO_devuelto,ui->le_TIPO_DEVUELTO->text().trimmed());
    tarea_a_actualizar.insert(TIPOFLUIDO_devuelto,ui->le_TIPO_FLUIDO_DEVUELTO->text().trimmed());
    tarea_a_actualizar.insert(ID_FINCA,ui->le_ID_FINCA->text().trimmed());
    tarea_a_actualizar.insert(COMENTARIOS,ui->le_COMENTARIOS->text().trimmed());
    tarea_a_actualizar.insert(DNI_CIF_COMUNIDAD,ui->le_DNI_CIF_COMUNIDAD->text().trimmed());
    tarea_a_actualizar.insert(TARIFA,ui->le_TARIFA->text().trimmed());
    tarea_a_actualizar.insert(TOTAL_CONTADORES,ui->le_TOTAL_CONTADORES->text().trimmed());
    tarea_a_actualizar.insert(C_CANAL,ui->le_C_CANAL->text().trimmed());
    tarea_a_actualizar.insert(C_LYC,ui->le_C_LYC->text().trimmed());
    tarea_a_actualizar.insert(C_AGRUPA,ui->le_C_AGRUPA->text().trimmed());
    tarea_a_actualizar.insert(DNI_CIF_ABONADO,ui->le_DNI_CIF_ABONADO->text());
    tarea_a_actualizar.insert(C_COMUNERO,ui->le_C_COMUNERO->text().trimmed());
    tarea_a_actualizar.insert(MENSAJE_LIBRE,ui->le_MENSAJE_LIBRE->text().trimmed());
    tarea_a_actualizar.insert(TIPO,ui->le_TIPO->text().trimmed());

    tarea_a_actualizar.insert(numero_precinto, ui->le_numero_precinto->text().trimmed());

    return Numero_Interno;
}
QString other_task_screen::getStringFromPhoto(QImage image){
    QByteArray byteArray;
    QString foto;
    QBuffer *buffer;
    buffer = new QBuffer(&byteArray);
    image.save(buffer,"jpg");
    foto = QString::fromLatin1(byteArray.toBase64().data());
    delete buffer;
    return foto;
}
void other_task_screen::on_pb_update_server_info_clicked()
{
    if(!this->isHidden()){
        if(ui->cb_gestor->currentText().isEmpty() || ui->le_numero_abonado->text().isEmpty()){
            GlobalFunctions::showMessage(this, "Campos faltantes", "Los campos del gestor y el número de abonado no pueden estar vacíos, por favor inserte");
            return;
        }
    }
    QString idSat = tarea_a_actualizar.value(ID_SAT).toString();
    QString idCabb = tarea_a_actualizar.value(idOrdenCABB).toString();
    if(!checkIfFieldIsValid(idSat) || !checkIfFieldIsValid(idCabb)){
        Info *info = new Info(nullptr, false, empresa);
        info->getInfoInServer();
        QJsonArray jsonArrayInfo = Info::readInfos();
        QJsonObject jsonObject = jsonArrayInfo.at(0).toObject();

        if(!checkIfFieldIsValid(idSat)){
            int ultimoIDSAT = jsonObject.value(lastIDSAT_infos).toString().trimmed().toInt();
            ultimoIDSAT++;
            tarea_a_actualizar.insert(ID_SAT, QString::number(ultimoIDSAT));
            QString id_sat_string = QString::number(ultimoIDSAT);
            jsonObject.insert(lastIDSAT_infos, id_sat_string);
        }
        if(!checkIfFieldIsValid(idCabb)){
            int ultimoIDOrden = jsonObject.value(lastIDOrden_infos).toString().trimmed().toInt();
            ultimoIDOrden++;
            tarea_a_actualizar.insert(idOrdenCABB, QString::number(ultimoIDOrden));
            QString id_orden_string = QString::number(ultimoIDOrden);
            jsonObject.insert(lastIDOrden_infos, id_orden_string);
        }
        info->actualizarInfoInServer(jsonObject);
    }
    QString Numero_Interno = guardar_cambios();

    int result = -1;
    QJsonDocument d;
    QStringList keys, values, photos, all_values;

    //SUBIR FOTOS
    //faltan subir las 8 fotos
    QString principal_var = tarea_a_actualizar.value(principal_variable).toString().trimmed();
    QString serie = tarea_a_actualizar.value(numero_serie_contador).toString().trimmed().replace(" ", "");
    QString serieDV = tarea_a_actualizar.value(numero_serie_contador_devuelto).toString().trimmed().replace(" ", "");
    QString prefijoDV = tarea_a_actualizar.value(CONTADOR_Prefijo_anno_devuelto).toString().trimmed();
    if(!checkIfFieldIsValid(serieDV)){
        serieDV = serie;
    }
    if(!serieDV.contains(prefijoDV)){
        prefijoDV = Counter::eliminarNumerosAlFinal(serieDV);
        tarea_a_actualizar.insert(CONTADOR_Prefijo_anno_devuelto, prefijoDV);
    }

    //Integrando fotos a JSON-------------------------------------------------------------------
    QPixmap default_foto = QPixmap(":/icons/add_photo.png");
    QString path = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/";
    QString gestor_actual = o.value(GESTOR).toString().trimmed();
    if(!checkIfFieldIsValid(gestor_actual)){
        gestor_actual = "Sin_Gestor";
    }
    QString anomalia = o.value(ANOMALIA).toString().trimmed();
    QString numAbonado = o.value(numero_abonado).toString().trimmed();
    QDir dir;
    dir.setPath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" +anomalia);
    if(!dir.exists()){
        dir.mkpath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" +anomalia);
    }
    //foto_antes_instalacion
    if(ui->lb_foto_antes_instalacion->pixmap() != nullptr
            && ui->lb_foto_antes_instalacion->pixmap()->toImage() != default_foto.toImage())
    {
        QImage image = ui->lb_foto_antes_instalacion->pixmap()->toImage();
        photos << getStringFromPhoto(image);
        //OJO poner delete buffer
        tarea_a_actualizar.insert(foto_antes_instalacion, serie+"_"+foto_antes_instalacion+".jpg");
        image.save(dir.path() + "/" + serie + "_" + foto_antes_instalacion + ".jpg");
    }
    else
    {
        photos << "null";
    }

    if(ui->lb_foto_numero_serie->pixmap() != nullptr
            && ui->lb_foto_numero_serie->pixmap()->toImage() != default_foto.toImage())
    {
        QImage image = ui->lb_foto_numero_serie->pixmap()->toImage();
        photos << getStringFromPhoto(image);
        tarea_a_actualizar.insert(foto_numero_serie,serie+"_"+foto_numero_serie+".jpg");
        image.save(dir.path() + "/" + serie + "_" + foto_numero_serie + ".jpg");
    }
    else
    {
        photos << "null";
    }

    if(ui->lb_foto_lectura->pixmap() != nullptr
            && ui->lb_foto_lectura->pixmap()->toImage() != default_foto.toImage())
    {
        QImage image = ui->lb_foto_lectura->pixmap()->toImage();
        photos << getStringFromPhoto(image);
        tarea_a_actualizar.insert(foto_lectura,serie+"_"+foto_lectura+".jpg");
        image.save(dir.path() + "/" + serie + "_" + foto_lectura + ".jpg");
    }
    else
    {
        photos << "null";
    }

    if(ui->lb_foto_despues_instalacion->pixmap() != nullptr
            && ui->lb_foto_despues_instalacion->pixmap()->toImage() != default_foto.toImage())
    {
        QImage image = ui->lb_foto_despues_instalacion->pixmap()->toImage();
        photos << getStringFromPhoto(image);
        tarea_a_actualizar.insert(foto_despues_instalacion,serieDV+"_"+foto_despues_instalacion+".jpg");
        image.save(dir.path() + "/" + serieDV + "_" + foto_despues_instalacion + ".jpg");
    }
    else
    {
        photos << "null";
    }

    if(ui->lb_foto_incidencia_1->pixmap() != nullptr
            && ui->lb_foto_incidencia_1->pixmap()->toImage() != default_foto.toImage())
    {
        QImage image = ui->lb_foto_incidencia_1->pixmap()->toImage();
        photos << getStringFromPhoto(image);
        tarea_a_actualizar.insert(foto_incidencia_1,serie+"_"+foto_incidencia_1+".jpg");
        image.save(dir.path() + "/" + serie + "_" + foto_incidencia_1 + ".jpg");
    }
    else
    {
        photos << "null";
    }

    if(ui->lb_foto_incidencia_2->pixmap() != nullptr
            && ui->lb_foto_incidencia_2->pixmap()->toImage() != default_foto.toImage())
    {
        QImage image = ui->lb_foto_incidencia_2->pixmap()->toImage();
        photos << getStringFromPhoto(image);
        tarea_a_actualizar.insert(foto_incidencia_2,serie+"_"+foto_incidencia_2+".jpg");
        image.save(dir.path() + "/" + serie + "_" + foto_incidencia_2 + ".jpg");
    }
    else
    {
        photos << "null";
    }

    if(ui->lb_foto_incidencia_3->pixmap() != nullptr
            && ui->lb_foto_incidencia_3->pixmap()->toImage() != default_foto.toImage())
    {
        QImage image = ui->lb_foto_incidencia_3->pixmap()->toImage();
        photos << getStringFromPhoto(image);
        tarea_a_actualizar.insert(foto_incidencia_3,serie+"_"+foto_incidencia_3+".jpg");
        image.save(dir.path() + "/" + serie + "_" + foto_incidencia_3 + ".jpg");
    }
    else
    {
        photos << "null";
    }

    if(ui->lb_firma_cliente->pixmap() != nullptr
            && ui->lb_firma_cliente->pixmap()->toImage() != default_foto.toImage())
    {
        QImage image = ui->lb_firma_cliente->pixmap()->toImage();
        photos << getStringFromPhoto(image);
        tarea_a_actualizar.insert(firma_cliente, tarea_a_actualizar.value(nombre_cliente).toString().trimmed().replace(" ", "_")+"_firma.jpg");
        image.save(dir.path() + "/" + o.value(nombre_cliente).toString().trimmed().replace(" ", "_") + "_firma" + ".jpg");
    }
    else
    {
        photos << "null";
    }
    //Fin Integrando fotos a JSON---------------------------------------------------------------------------------------------------------------------------------

    if(conexion_activa){
        if(showMesageBox){
            show_loading("Actualizando información de tarea...");
        }
        //    QBuffer buffer;
        d.setObject(tarea_a_actualizar);
        QByteArray ba = d.toJson(QJsonDocument::Compact);
        ba.replace("&","y");
        keys << "json" << "empresa";
        QString temp = QString::fromUtf8(ba);
        values << temp << empresa.toLower();
        connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        //SUBIR INFO DE LA TAREA
        if(o.isEmpty())//quiere decir que no estoy actualizando una tarea sino creando una nueva
        {
            GlobalFunctions gf(this, empresa);
            QStringList numInternos = gf.getTareasList();

            if(numInternos.contains(principal_var)){
                GlobalFunctions gf(this);
                GlobalFunctions::showWarning(this, "Variable pricipal repetido", "La variable pricipal de esta tarea ya existe");
                disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
                return;
            }

            //OJO ESPERAR POR RESPUESTA
            QEventLoop q;
            connect(this, &other_task_screen::script_excecution_result,&q,&QEventLoop::exit);

            this->keys = keys;
            this->values = values;
            QTimer::singleShot(DELAY, this, SLOT(create_task_request()));

            switch (q.exec())
            {
            case database_comunication::script_result::timeout:
                //OJO ESPERAR POR RESULTADO
                result = database_comunication::script_result::create_task_to_server_failed;
                break;

            case database_comunication::script_result::create_tarea_failed:
                //OJO ESPERAR POR RESULTADO
                result = database_comunication::script_result::create_task_to_server_failed;
                break;

            case database_comunication::script_result::ok:
                result = database_comunication::script_result::task_to_server_ok;
                break;
            }
        }
        else
        {
            if(tarea_a_actualizar.value(id) == "")//quiere decir que la tarea se cargo de fichero y no esta en la base de datos del servidor
            {
                if(principal_var.isEmpty() || principal_var.isNull()){
                    GlobalFunctions gf(this);
                    GlobalFunctions::showWarning(this, "Variable pricipal desconocida", ">Debe insertar la variable pricipal");
                    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
                    return;
                }
                GlobalFunctions gf(this, empresa);
                QStringList numInternos = gf.getTareasList();
                if(numInternos.contains(principal_var)){
                    GlobalFunctions gf(this);
                    GlobalFunctions::showWarning(this, "Variable pricipal  repetido", "La Variable pricipal  de esta tarea ya existe");
                    if(QMessageBox::Ok == (QMessageBox::question(this,"Actualizar", "¿Desea actualizar la tarea?", QMessageBox::Ok, QMessageBox::No))){
                        //                   database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA,keys,values);

                        //OJO ESPERAR POR RESPUESTA
                        QEventLoop q;
                        connect(this, &other_task_screen::script_excecution_result,&q,&QEventLoop::exit);

                        this->keys = keys;
                        this->values = values;
                        QTimer::singleShot(DELAY, this, SLOT(update_task_request()));
                        switch (q.exec())
                        {
                        case database_comunication::script_result::timeout:
                            //OJO ESPERAR POR RESULTADO
                            result = database_comunication::script_result::update_task_to_server_failed;
                            break;

                        case database_comunication::script_result::update_tarea_failed:
                            //OJO ESPERAR POR RESULTADO
                            result = database_comunication::script_result::update_task_to_server_failed;

                            break;

                        case database_comunication::script_result::ok:
                            if(showMesageBox)
                            {
                                hide_loading();
                                GlobalFunctions::showMessage(this,"Éxito",
                                                             "Se actualizó la información correctamente");
                                //                                GlobalFunctions::showMessage(this,"Éxito","Se actualizó la información correctamente.");
                                show_loading("Actualizando vista de tareas...");
                            }
                            result = database_comunication::script_result::task_to_server_ok;
                            break;
                        }
                    }
                }else {
                    QEventLoop q;
                    connect(this, &other_task_screen::script_excecution_result,&q,&QEventLoop::exit);

                    this->keys = keys;
                    this->values = values;
                    QTimer::singleShot(DELAY, this, SLOT(create_task_request()));
                    switch (q.exec())
                    {
                    case database_comunication::script_result::timeout:
                        //OJO ESPERAR POR RESULTADO
                        result = database_comunication::script_result::create_task_to_server_failed;
                        break;

                    case database_comunication::script_result::create_tarea_failed:
                        //OJO ESPERAR POR RESULTADO
                        result = database_comunication::script_result::create_task_to_server_failed;
                        break;

                    case database_comunication::script_result::ok:
                        result = database_comunication::script_result::task_to_server_ok;
                        if(!geoCodeChanged){
                            if(!closing_window){
                                emit task_upload_excecution_result(result);
                            }
                        }
                        if(showMesageBox)
                        {
                            hide_loading();
                            GlobalFunctions::showMessage(this,"Éxito",
                                                         "Se actualizó la información correctamente");
                            //                            GlobalFunctions::showMessage(this,"Éxito","Se actualizó la información correctamente.");
                            show_loading("Actualizando vista de tareas...");
                        }
                        break;
                    }
                }
            }
            else
            {
                QEventLoop q;
                connect(this, &other_task_screen::script_excecution_result,&q,&QEventLoop::exit);

                this->keys = keys;
                this->values = values;
                QTimer::singleShot(DELAY, this, SLOT(update_task_request()));
                switch (q.exec())
                {
                case database_comunication::script_result::timeout:
                    result = database_comunication::script_result::update_task_to_server_failed;
                    break;

                case database_comunication::script_result::update_tarea_failed:
                    result = database_comunication::script_result::update_task_to_server_failed;
                    break;

                case database_comunication::script_result::ok:
                    result = database_comunication::script_result::task_to_server_ok;
                    if(!geoCodeChanged){
                        if(!closing_window){
                            emit task_upload_excecution_result(result);
                        }
                    }
                    if(showMesageBox)
                    {
                        hide_loading();
                        GlobalFunctions::showMessage(
                                    this,"Éxito","Se actualizó la información correctamente");
                        show_loading("Actualizando vista de tareas...");
                    }
                    break;
                }
            }
        }
        if(foto_cambiada){
            qDebug()<<"Foto cambiada********************";
            keys.clear();
            keys << "foto" << GESTOR << ANOMALIA << numero_abonado << "nombre_foto"<< "empresa";
            //    values.clear();

            QString anomalia = tarea_a_actualizar.value(ANOMALIA).toString().trimmed();
            QString numAbonado = tarea_a_actualizar.value(numero_abonado).toString().trimmed();
            QString gest = tarea_a_actualizar.value(GESTOR).toString().trimmed();

            all_values << photos.at(0) << gest << anomalia << numAbonado << serie+"_"+foto_antes_instalacion+".jpg"
                       << photos.at(1) << gest << anomalia << numAbonado << serie+"_"+foto_numero_serie+".jpg"
                       << photos.at(2) << gest << anomalia << numAbonado << serie+"_"+foto_lectura+".jpg"
                       << photos.at(3) << gest << anomalia << numAbonado << serieDV+"_"+foto_despues_instalacion+".jpg"
                       << photos.at(4) << gest << anomalia << numAbonado << serie+"_"+foto_incidencia_1+".jpg"
                       << photos.at(5) << gest << anomalia << numAbonado << serie+"_"+foto_incidencia_2+".jpg"
                       << photos.at(6) << gest << anomalia << numAbonado << serie+"_"+foto_incidencia_3+".jpg"
                       << photos.at(7) << gest << anomalia << numAbonado << tarea_a_actualizar.value(nombre_cliente).toString().trimmed().replace(" ", "_")+"_firma.jpg";


            for(int i = 0, reintentos = 0; i < 8; i++)
            {
                connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

                if(!checkIfFieldIsValid(photos.at(i)))
                {
                    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
                    continue;
                }
                QEventLoop q;

                connect(this, &other_task_screen::script_excecution_result,&q,&QEventLoop::exit);

                values.clear();
                values << all_values[i*5] << all_values[i*5+1] << all_values[i*5+2] << all_values[i*5+3] << all_values[i*5+4] << empresa;
                //        values << all_values.at(i*4) << all_values.at(i*4 + 1) << all_values.at(i*4 + 2) << all_values.at(i*4 + 3);

                this->keys = keys;
                this->values = values;
                //ojo cambiar por upload
                QTimer::singleShot(DELAY, this, SLOT(upload_task_image_request()));

                switch (q.exec())
                {
                case database_comunication::script_result::timeout:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        //OJO PENSAR MEJOR DESPUES QUE HACER
                        GlobalFunctions gf(this);
                        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = 8;
                        //                i = jsonArrayAllTask.size();
                    }
                    break;
                case database_comunication::script_result::upload_task_image_failed:
                    break;
                case database_comunication::script_result::ok:
                    break;
                }
            }
        }
        //OJO EMITIR RESULTADO GLOBAL DE LA FUNCION
        //mostrar cartel en consecuencia si esta todo ok o si hubo error
        if(result != database_comunication::script_result::task_to_server_ok){
            if(!geoCodeChanged){
                if(!closing_window){
                    emit task_upload_excecution_result(result);
                }
            }
        }
    }
    else{
        if(foto_cambiada){

            keys.clear();
            keys << "foto" << GESTOR << ANOMALIA << numero_abonado << "nombre_foto"<< "empresa";
            //    values.clear();
            QString anomalia = tarea_a_actualizar.value(ANOMALIA).toString().trimmed();
            QString numAbonado = tarea_a_actualizar.value(numero_abonado).toString().trimmed();
            QString gest = tarea_a_actualizar.value(GESTOR).toString().trimmed();

            all_values << photos.at(0) << gest << anomalia << numAbonado << serie+"_"+foto_antes_instalacion+".jpg"
                       << photos.at(1) << gest << anomalia << numAbonado << serie+"_"+foto_numero_serie+".jpg"
                       << photos.at(2) << gest << anomalia << numAbonado << serie+"_"+foto_lectura+".jpg"
                       << photos.at(3) << gest << anomalia << numAbonado << serieDV+"_"+foto_despues_instalacion+".jpg"
                       << photos.at(4) << gest << anomalia << numAbonado << serie+"_"+foto_incidencia_1+".jpg"
                       << photos.at(5) << gest << anomalia << numAbonado << serie+"_"+foto_incidencia_2+".jpg"
                       << photos.at(6) << gest << anomalia << numAbonado << serie+"_"+foto_incidencia_3+".jpg"
                       << photos.at(7) << gest << anomalia << numAbonado << tarea_a_actualizar.value(nombre_cliente).toString().trimmed().replace(" ", "_")+"_firma.jpg";


            for(int i = 0, reintentos = 0; i < 8; i++)
            {
                connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

                if(!checkIfFieldIsValid(photos.at(i)))
                {
                    disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
                    continue;
                }
                QEventLoop q;
                connect(this, &other_task_screen::script_excecution_result,&q,&QEventLoop::exit);

                values.clear();
                values << all_values[i*5] << all_values[i*5+1] << all_values[i*5+2] << all_values[i*5+3] << all_values[i*5+4] << empresa;
                //        values << all_values.at(i*4) << all_values.at(i*4 + 1) << all_values.at(i*4 + 2) << all_values.at(i*4 + 3);

                this->keys = keys;
                this->values = values;
                //ojo cambiar por upload
                QTimer::singleShot(DELAY, this, SLOT(upload_task_image_request()));

                switch (q.exec())
                {
                case database_comunication::script_result::timeout:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        //OJO PENSAR MEJOR DESPUES QUE HACER
                        GlobalFunctions gf(this);
                        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = 8;
                        //                i = jsonArrayAllTask.size();
                    }
                    break;
                case database_comunication::script_result::upload_task_image_failed:
                    i--;
                    reintentos++;
                    if(reintentos == RETRIES)
                    {
                        //OJO PENSAR MEJOR DESPUES QUE HACER
                        GlobalFunctions gf(this);
                        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
                        i = 8;
                        //                i = jsonArrayAllTask.size();
                    }
                    break;
                case database_comunication::script_result::ok:
                    //OJO Aqui se puede subir el nombre de la foto al servidor
                    //            GlobalFunctions::showMessage(this,"Fotos subidas","Fueron subidas las fotos al servidor.");
                    break;
                }

            }

            createAutoPDF(false);
        }

        QJsonArray jsonArray = readJsonArrayTasks();
        updateTaskInJsonArrayAllLocal(jsonArray, tarea_a_actualizar);
        if(!closing_window){
            emit task_upload_excecution_result(database_comunication::task_to_server_ok);
        }
        if(!this->isHidden() ){
            GlobalFunctions::showMessage(this,"Información actualizada","Información actualizada en el respaldo local satisfactoriamente.");
        }
    }

    if(geoCodeChanged){
        geoCodeChanged = false;
        updateTareas();
        updateITACsGeoCode();
        emit updateITACs();
        if(!closing_window){
            emit task_upload_excecution_result(result);
        }
    }
    hide_loading();
}
int other_task_screen::getJsonObjectPositionInJsonArray(QJsonArray jsonArray, QJsonObject jsonObject){
    QString principal_var = jsonObject.value(principal_variable).toString().trimmed();
    QString principal_varJsonArray;
    for (int i=0; i < jsonArray.size(); i++) {
        principal_varJsonArray = jsonArray.at(i).toObject().value(principal_variable).toString().trimmed();
        if(principal_varJsonArray == principal_var){
            return i;
        }
    }
    return -1;
}
QJsonObject other_task_screen::checkIfDuplicateByInformation(QJsonObject o){ //Retorna true si la tarea esta repetida
    QJsonObject jsonToSearch;

    jsonToSearch.insert(poblacion, o.value(poblacion).toString().trimmed());
    jsonToSearch.insert(calle, o.value(calle).toString().trimmed());
    jsonToSearch.insert(numero, o.value(numero).toString().trimmed());
    jsonToSearch.insert(BIS, o.value(BIS).toString().trimmed());
    jsonToSearch.insert(piso, o.value(piso).toString().trimmed());
    jsonToSearch.insert(mano, o.value(mano).toString().trimmed());

    jsonToSearch.insert(numero_abonado, o.value(numero_abonado).toString().trimmed());
    jsonToSearch.insert(tipo_tarea, o.value(tipo_tarea).toString().trimmed());
    jsonToSearch.insert(calibre_toma, o.value(calibre_toma).toString().trimmed());
    jsonToSearch.insert(numero_serie_contador, o.value(numero_serie_contador).toString().trimmed());

    GlobalFunctions gf(this, empresa);
    QJsonObject jsonObjectFound = gf.getTareaFromServer(jsonToSearch);

    if(!jsonObjectFound.isEmpty()){
        return jsonObjectFound;
    }
    else{
        return QJsonObject();
    }
}

void other_task_screen::updateTaskInJsonArrayAllLocal(QJsonArray jsonArray, QJsonObject jsonObject){
    int index = getJsonObjectPositionInJsonArray(jsonArray, jsonObject);
    if(index != -1){
        jsonArray = updateTaskIfNeeded(jsonArray, jsonObject, index);
    }else{
        //        if(!checkIfDuplicateByInformation(jsonObject)){
        jsonArray.append(jsonObject);
        //        }
    }
    writeJsonArrayTasks(jsonArray);
}
QJsonArray other_task_screen::updateTaskIfNeeded(QJsonArray jsonArray, QJsonObject jsonObject, int index){
    if(jsonArray.size() > index){
        QJsonObject jsonObject_old = jsonArray[index].toObject();
        if(compareJsonObjectByDateModified(jsonObject_old, jsonObject)){
            jsonArray[index] = jsonObject;
        }
    }
    return jsonArray;
}
bool other_task_screen::compareJsonObjectByDateModified(QJsonObject jsonObject_old, QJsonObject jsonObject_new){ //devuelve true si el segundo es mas actualizado
    QString oldMod = jsonObject_old.value(date_time_modified).toString();
    QDateTime date_oldMod =  QDateTime::fromString(oldMod, formato_fecha_hora);
    QString newMod = jsonObject_new.value(date_time_modified).toString();
    QDateTime date_newMod =  QDateTime::fromString(newMod, formato_fecha_hora);
    if(date_newMod > date_oldMod){
        return true;
    }else {
        return false;
    }
}

void other_task_screen::upload_save_work_file_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::SAVE_WORK,keys,values);
}
void other_task_screen::download_save_work_file_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::LOAD_WORK,keys,values);
}

void other_task_screen::create_task_request(QStringList keys, QStringList values)
{
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_TAREA,keys,values);
}
void other_task_screen::create_task_request()
{
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_TAREA,keys,values);
}
void other_task_screen::download_task_image_request()
{
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_TASK_IMAGE,keys,values);
}
void other_task_screen::upload_task_image_request()
{
    database_com.serverRequest(database_comunication::serverRequestType::UPLOAD_TASK_IMAGE,keys,values);
}
void other_task_screen::update_task_request(QStringList keys, QStringList values)
{
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA,keys,values);
}
void other_task_screen::update_task_request()
{
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA,keys,values);
}
void other_task_screen::download_task_image_request(QStringList keys, QStringList values)
{
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_TASK_IMAGE,keys,values);
}

void other_task_screen::setFileDirSelected(QString file_name)
{
    file_download_dir_selected  = file_name;
}
bool other_task_screen::checkDisponibleAudio()
{
    QString audio = tarea_a_actualizar.value(audio_detalle).toString().trimmed();

    if(checkIfFieldIsValid(audio)){
        QString audio_dir_file = getLocalDirofCurrentTask() + "/" + audio;
        QFile file(audio_dir_file);
        if(file.exists()){
            ui->pb_play_audio->show();
            return true;
        }else{
            ui->pb_play_audio->hide();
        }
    }
    return false;
}
void other_task_screen::loadLocalPhoto(int currentPhoto){ //curent Photo del 1 al 8
    QStringList fotos;
    QList<my_label *> labels;
    fotos << foto_antes_instalacion << foto_numero_serie << foto_lectura << foto_despues_instalacion
          << foto_incidencia_1 << foto_incidencia_2 << foto_incidencia_3 << firma_cliente;

    labels.append(ui->lb_foto_antes_instalacion);
    labels.append(ui->lb_foto_numero_serie);
    labels.append(ui->lb_foto_lectura);
    labels.append(ui->lb_foto_despues_instalacion);
    labels.append(ui->lb_foto_incidencia_1);
    labels.append(ui->lb_foto_incidencia_2);
    labels.append(ui->lb_foto_incidencia_3);
    labels.append(ui->lb_firma_cliente);


    if(currentPhoto > 0 && currentPhoto < 9){
        QString foto = fotos.at(currentPhoto -1);
        QString local_foto = o.value(foto).toString();
        if(checkIfFieldIsValid(local_foto)){
            QString path = getLocalDirofCurrentTask() + "/" + local_foto;
            if(QFile::exists(path)){
                labels[currentPhoto -1]->setPixmap(QPixmap(path));
            }
        }
    }
}
void other_task_screen::loadLocalPhotos(){
    QStringList fotos;
    QList<my_label *> labels;
    fotos << foto_antes_instalacion << foto_numero_serie << foto_lectura << foto_despues_instalacion
          << foto_incidencia_1 << foto_incidencia_2 << foto_incidencia_3 << firma_cliente;

    labels.append(ui->lb_foto_antes_instalacion);
    labels.append(ui->lb_foto_numero_serie);
    labels.append(ui->lb_foto_lectura);
    labels.append(ui->lb_foto_despues_instalacion);
    labels.append(ui->lb_foto_incidencia_1);
    labels.append(ui->lb_foto_incidencia_2);
    labels.append(ui->lb_foto_incidencia_3);
    labels.append(ui->lb_firma_cliente);

    for(int i=0; i< fotos.size(); i++) {
        QString foto = fotos.at(i);
        QString local_foto = o.value(foto).toString();
        if(checkIfFieldIsValid(local_foto)){
            QString path = getLocalDirofCurrentTask() + "/" + local_foto;
            if(QFile::exists(path)){
                labels[i]->setPixmap(QPixmap(path));
            }
        }
    }
}

void other_task_screen::getPhotosLocal(){
    QString path = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/";
    QString gestor_actual = o.value(GESTOR).toString().trimmed();
    QString anomalia = o.value(ANOMALIA).toString().trimmed();
    QString numAbonado = o.value(numero_abonado).toString().trimmed();
    if(!checkIfFieldIsValid(gestor_actual)){
        gestor_actual = "Sin_Gestor";
    }
    QDir dir;
    dir.setPath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" + anomalia);
    if(!dir.exists()){
        dir.setPath(path + gestor_actual + "/fotos_tareas/" + numAbonado);
    }
    QImage img;
    QString serie = o.value(numero_serie_contador).toString().trimmed().replace(" ", "");
    QString serieDV = o.value(numero_serie_contador_devuelto).toString().trimmed().replace(" ", "");
    if(!checkIfFieldIsValid(serieDV)){
        serieDV = serie;
    }

    switch (currentPhotoLooking)
    {
    case 1://foto_antes_instalacion
        if(checkIfFieldIsValid(o.value(foto_antes_instalacion).toString())){
            path = dir.path() + "/" + serie + "_" + "foto_antes_instalacion" + ".jpg";
            //                    img = database_comunication::getImageFromString();
            ui->lb_foto_antes_instalacion->setMinimumWidth(0);
            ui->lb_foto_antes_instalacion->setPixmap(QPixmap(path));
            ui->lb_foto_antes_instalacion->setScaledContents(true);
        }
        break;
    case 2://foto_numero_serie
        if(checkIfFieldIsValid(o.value(foto_numero_serie).toString())){
            path = dir.path() + "/" + serie + "_" + "foto_numero_serie" + ".jpg";
            ui->lb_foto_numero_serie->setMinimumWidth(0);
            ui->lb_foto_numero_serie->setPixmap(QPixmap(path));
            ui->lb_foto_numero_serie->setScaledContents(true);
        }
        break;
    case 3://foto_lectura
        if(checkIfFieldIsValid(o.value(foto_lectura).toString())){
            path = dir.path() + "/" + serie + "_" + "foto_lectura" + ".jpg";
            ui->lb_foto_lectura->setMinimumWidth(0);
            ui->lb_foto_lectura->setPixmap(QPixmap(path));
            ui->lb_foto_lectura->setScaledContents(true);
        }
        break;
    case 4://foto_despues_instalacion
        if(checkIfFieldIsValid(o.value(foto_despues_instalacion).toString())){
            path = dir.path() + "/" + serieDV + "_" + "foto_despues_instalacion" + ".jpg";
            ui->lb_foto_despues_instalacion->setMinimumWidth(0);
            ui->lb_foto_despues_instalacion->setPixmap(QPixmap(path));
            ui->lb_foto_despues_instalacion->setScaledContents(true);
        }
        break;
    case 5://foto_incidencia_1
        if(checkIfFieldIsValid(o.value(foto_incidencia_1).toString())){
            path = dir.path() + "/" + serie + "_" + "foto_incidencia_1" + ".jpg";
            ui->lb_foto_incidencia_1->setMinimumWidth(0);
            ui->lb_foto_incidencia_1->setPixmap(QPixmap(path));
            ui->lb_foto_incidencia_1->setScaledContents(true);
        }
        break;
    case 6://foto_incidencia_2
        if(checkIfFieldIsValid(o.value(foto_incidencia_2).toString())){
            path = dir.path() + "/" + serie + "_" + "foto_incidencia_2" + ".jpg";
            ui->lb_foto_incidencia_2->setMinimumWidth(0);
            ui->lb_foto_incidencia_2->setPixmap(QPixmap(path));
            ui->lb_foto_incidencia_2->setScaledContents(true);
        }
        break;
    case 7://foto_incidencia_3
        if(checkIfFieldIsValid(o.value(foto_incidencia_3).toString())){
            path = dir.path() + "/" + serie + "_" + "foto_incidencia_3" + ".jpg";
            ui->lb_foto_incidencia_3->setMinimumWidth(0);
            ui->lb_foto_incidencia_3->setPixmap(QPixmap(path));
            ui->lb_foto_incidencia_3->setScaledContents(true);
        }
        break;
    case 8://firma_cliente
        if(checkIfFieldIsValid(o.value(firma_cliente).toString())){
            path = dir.path() + "/" + o.value(nombre_cliente).toString().trimmed().replace(" ", "_") + "_firma" + ".jpg";
            ui->lb_firma_cliente->setMinimumWidth(0);
            ui->lb_firma_cliente->setPixmap(QPixmap(path));
            ui->lb_firma_cliente->setScaledContents(true);
        }
        break;
    }
}
void other_task_screen::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
    if(tipo == database_comunication::DOWNLOAD_TASK_IMAGE)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        int t = 0;
        for (int i =0; i < ba.length(); i++) {
            if(i > 10){
                break;
            }
            if(QChar(ba.at(i)).isNumber()){
                t = QString(ba.at(i)).toInt();
                break;
            }
        }
        //        int t = (QString::fromUtf8(ba.left(3)).right(1)).toInt();
        if(ba.left(3).contains("\n")){
            ba.remove(2,1);
        }else {
            ba.remove(0,1);
        }
        QString foto = QString::fromUtf8(ba).replace("\n","");

        if(ba.contains("ot success download_task_image") || ba.isEmpty())
        {
            if(ba.contains("o se pudo obtener imagen de la tarea"))
                //                emit script_excecution_result(database_comunication::script_result::download_task_image_failed);
                result = database_comunication::script_result::download_task_image_failed;
            else if(ba.contains("o existe imagen de la tarea")){
                //                emit script_excecution_result(database_comunication::script_result::download_task_image_picture_doesnt_exists);
                result = database_comunication::script_result::download_task_image_picture_doesnt_exists;
            }
        }
        else{
            QImage img;
            QString path = "C:/Mi_Ruta/Empresas/"+empresa+"/Gestores/";
            QString gestor_actual = o.value(GESTOR).toString().trimmed();
            if(!checkIfFieldIsValid(gestor_actual)){
                gestor_actual = "Sin_Gestor";
            }

            QString anomalia = o.value(ANOMALIA).toString().trimmed();
            QString numAbonado = o.value(numero_abonado).toString().trimmed();
            QDir dir;
            dir.setPath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" +anomalia);
            if(!dir.exists()){
                dir.mkpath(path + gestor_actual + "/fotos_tareas/" + numAbonado + "/" +anomalia);
            }
            QString serie = o.value(numero_serie_contador).toString().trimmed().replace(" ", "");
            QString serieDV = o.value(numero_serie_contador_devuelto).toString().trimmed().replace(" ", "");
            QString foto_name="";
            if(!checkIfFieldIsValid(serieDV)){
                serieDV = serie;
            }
            switch (t)
            {
            case 1://foto_antes_instalacion
                img = database_comunication::getImageFromString(foto);
                ui->lb_foto_antes_instalacion->setMinimumWidth(0);
                ui->lb_foto_antes_instalacion->setPixmap(QPixmap::fromImage(img));
                ui->lb_foto_antes_instalacion->setScaledContents(true);
                //SALVAR EN PC
                foto_name = o.value(foto_antes_instalacion).toString().trimmed();
                img.save(dir.path() + "/" + foto_name);
                dir_foto_antes_instalacion = dir.path() + "/" + foto_name;
                break;

            case 2://foto_numero_serie
                img = database_comunication::getImageFromString(foto);
                ui->lb_foto_numero_serie->setMinimumWidth(0);
                ui->lb_foto_numero_serie->setPixmap(QPixmap::fromImage(img));
                ui->lb_foto_numero_serie->setScaledContents(true);
                //SALVAR EN PC
                foto_name = o.value(foto_numero_serie).toString().trimmed();
                img.save(dir.path() + "/" + foto_name);
                dir_foto_numero_serie = dir.path() + "/" + foto_name;
                break;

            case 3://foto_lectura
                img = database_comunication::getImageFromString(foto);
                ui->lb_foto_lectura->setMinimumWidth(0);
                ui->lb_foto_lectura->setPixmap(QPixmap::fromImage(img));
                ui->lb_foto_lectura->setScaledContents(true);
                //SALVAR EN PC
                foto_name = o.value(foto_lectura).toString().trimmed();
                img.save(dir.path() + "/" + foto_name);
                dir_foto_lectura = dir.path() + "/" + foto_name;
                break;

            case 4://foto_despues_instalacion
                img = database_comunication::getImageFromString(foto);
                ui->lb_foto_despues_instalacion->setMinimumWidth(0);
                ui->lb_foto_despues_instalacion->setPixmap(QPixmap::fromImage(img));
                ui->lb_foto_despues_instalacion->setScaledContents(true);
                //SALVAR EN PC
                foto_name = o.value(foto_despues_instalacion).toString().trimmed();
                img.save(dir.path() + "/" + foto_name);
                dir_foto_despues_instalacion = dir.path() + "/" + foto_name;
                break;

            case 5://foto_incidencia_1
                img = database_comunication::getImageFromString(foto);
                ui->lb_foto_incidencia_1->setMinimumWidth(0);
                ui->lb_foto_incidencia_1->setPixmap(QPixmap::fromImage(img));
                ui->lb_foto_incidencia_1->setScaledContents(true);
                //SALVAR EN PC
                foto_name = o.value(foto_incidencia_1).toString().trimmed();
                img.save(dir.path() + "/" + foto_name);
                dir_foto_incidencia_1 = dir.path() + "/" + foto_name;
                break;

            case 6://foto_incidencia_2
                img = database_comunication::getImageFromString(foto);
                ui->lb_foto_incidencia_2->setMinimumWidth(0);
                ui->lb_foto_incidencia_2->setPixmap(QPixmap::fromImage(img));
                ui->lb_foto_incidencia_2->setScaledContents(true);
                //SALVAR EN PC
                foto_name = o.value(foto_incidencia_1).toString().trimmed();
                img.save(dir.path() + "/" + foto_name);
                dir_foto_incidencia_2 = dir.path() + "/" + foto_name;
                break;

            case 7://foto_incidencia_3
                img = database_comunication::getImageFromString(foto);
                ui->lb_foto_incidencia_3->setMinimumWidth(0);
                ui->lb_foto_incidencia_3->setPixmap(QPixmap::fromImage(img));
                ui->lb_foto_incidencia_3->setScaledContents(true);
                //SALVAR EN PC
                foto_name = o.value(foto_incidencia_1).toString().trimmed();
                img.save(dir.path() + "/" + foto_name);
                dir_foto_incidencia_3 = dir.path() + "/" + foto_name;
                break;
            case 8://firma_cliente
                img = database_comunication::getImageFromString(foto);
                ui->lb_firma_cliente->setMinimumWidth(0);
                ui->lb_firma_cliente->setPixmap(QPixmap::fromImage(img));
                ui->lb_firma_cliente->setScaledContents(true);
                //SALVAR EN PC
                foto_name = o.value(firma_cliente).toString().trimmed();
                img.save(dir.path() + "/" + foto_name);
                dir_foto_firma = dir.path() + "/" + foto_name;
                break;
            }
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::DOWNLOAD_FILE){
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        qDebug()<<ba;
        if(ba.contains("archivo_descargado"))
        {
            result = database_comunication::script_result::ok;
        }
        else if(ba.contains("error"))
        {
            result = database_comunication::script_result::download_audio_failed;
        }
    }
    else if(tipo == database_comunication::UPDATE_TAREA)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        qDebug()<<ba;
        if(ba.contains("ot success update_tarea"))
        {
            if(showMesageBox)
            {
                hide_loading();
            }
            result = database_comunication::script_result::update_tarea_failed;
        }
        else if(ba.contains("success"))
        {
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::UPDATE_TAREA_FIELDS)
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
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

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
    else if(tipo == database_comunication::CREATE_TAREA)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
        //        GlobalFunctions::showMessage(this,"Éxito","Información actualizada en el servidor satisfactoriamente.");

        if(ba.contains("not success create_task") || ba.contains("not success update_tarea"))
        {
            respuesta.prepend("\n\nCREATE_TAREA FAIL-> "+ QDateTime::currentDateTime().toString(formato_fecha_hora_new_view)+"\n");
            QFile file("log_error.txt");
            if(file.open(QIODevice::Append)){
                file.write(respuesta.toUtf8());
            }
            if(showMesageBox)
            {
                GlobalFunctions gf(this);
                GlobalFunctions::showWarning(this,"Información del servidor no actualizada","Ocurrió un error durante la actualización de la información al servidor.");
            }

            //            emit script_excecution_result(database_comunication::script_result::create_tarea_failed);
            result = database_comunication::script_result::create_tarea_failed;
        }
        else if(ba.contains("success"))
        {
            if(showMesageBox)
            {
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada correctamente en el servidor.");
            }
            //            emit script_excecution_result(database_comunication::script_result::ok);
            result = database_comunication::script_result::ok;
        }
        //        emit update_process_finished();
    }
    else if(tipo == database_comunication::UPLOAD_TASK_IMAGE)
    {
        qDebug()<<ba;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success upload_task_image"))
        {
            //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
            result = database_comunication::script_result::upload_task_image_failed;
        }
        else
        {
            if(ba.contains("Subio imagen Correctamente"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    emit script_excecution_result(result);
}

void other_task_screen::on_drag_screen()
{
    if(isFullScreen()){
        if(QApplication::mouseButtons()==Qt::RightButton){

        }
        return;
    }
    //ui->statusBar->showMessage("Moviendo");
    if(QApplication::mouseButtons()==Qt::LeftButton){
        if(!this->isMaximized() && !this->isFullScreen()){
            start_moving_screen.start(10);
            init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
            init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
        }
    }
    else if(QApplication::mouseButtons()==Qt::RightButton){

    }
}

void other_task_screen::on_drag_screen_released()
{
    if(isFullScreen()){

        return;
    }
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
    //current_win_Pos = QPoint(this->pos().x()-200,this->pos().y()-200);
}

void other_task_screen::on_start_moving_screen_timeout()
{
    if(QApplication::mouseButtons()==Qt::LeftButton){
        int x_pos = (int)this->pos().x()+((QWidget::mapFromGlobal(QCursor::pos())).x() - init_pos_x);
        int y_pos = (int)this->pos().y()+((QWidget::mapFromGlobal(QCursor::pos())).y() - init_pos_y);
        x_pos = (x_pos < 0)?0:x_pos;
        y_pos = (y_pos < 0)?0:y_pos;

        x_pos = (x_pos > QApplication::desktop()->width()-100)?QApplication::desktop()->width()-100:x_pos;
        y_pos = (y_pos > QApplication::desktop()->height()-180)?QApplication::desktop()->height()-180:y_pos;

        this->move(x_pos,y_pos);

        init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
        init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
    }else{
        on_drag_screen_released();
    }
}

void other_task_screen::clear_all_pictures()
{
    ui->lb_foto_lectura->clear();
    ui->lb_foto_antes_instalacion->clear();
    ui->lb_foto_numero_serie->clear();
    ui->lb_foto_despues_instalacion->clear();
    ui->lb_foto_incidencia_1->clear();
    ui->lb_foto_incidencia_2->clear();
    ui->lb_foto_incidencia_3->clear();
    ui->lb_firma_cliente->clear();
}

void other_task_screen::checkAndFillEmptyField(){
    QString emplaza_dv = ui->le_emplazamiento_devuelto->text().trimmed();
    QString rest_dv = ui->le_RESTO_EM->text().trimmed();
    QString serie_dv = ui->le_numero_serie_contador_devuelto->text().trimmed();
    if(!checkIfFieldIsValid(emplaza_dv)){
        ui->le_emplazamiento_devuelto->setText(ui->le_emplazamiento->text());
    }
    if(!checkIfFieldIsValid(rest_dv)){
        ui->le_RESTO_EM->setText(tarea_a_actualizar.value(RESTEMPLAZA).toString());
    }
    if(!checkIfFieldIsValid(serie_dv)){
        ui->le_numero_serie_contador_devuelto->setText(ui->le_numero_serie_contador->text());
        QString prefijo= tarea_a_actualizar.value(CONTADOR_Prefijo_anno).toString().trimmed();
        tarea_a_actualizar.insert(CONTADOR_Prefijo_anno_devuelto, prefijo);
    }
}
void other_task_screen::on_pb_cerrar_tarea_clicked()
{
    if(true/*QMessageBox::question(this,"Cerrando Tarea","Seguro que desea cerrar esta tarea?",
                                                                                                                                                                                                                                                                                                                                                                                                                                                     QMessageBox::Ok, QMessageBox::No)== QMessageBox::Ok*/){

        tarea_a_actualizar.insert(status_tarea, "CLOSED");
        QString timestamp = QDateTime::currentDateTime().toString(formato_fecha_hora_new_view);
        QString timestamp_format = QDateTime::currentDateTime().toString(formato_fecha_hora);
        ui->le_FECH_CIERRE->setText(timestamp);
        if(ui->le_F_INST->text().isEmpty()){
            ui->le_F_INST->setText(timestamp);
            tarea_a_actualizar.insert(F_INST, timestamp_format);
        }else{
            tarea_a_actualizar.insert(F_INST, QDateTime::fromString(ui->le_F_INST->text(), timestamp_format)
                                      .toString(formato_fecha_hora));
        }
        checkAndFillEmptyField();

        Informe_Instalacion_Servicios *informe = new Informe_Instalacion_Servicios(this, empresa);
        QEventLoop loop;
        connect(informe, &Informe_Instalacion_Servicios::sendData, this,&other_task_screen::getData);
        connect(informe, &Informe_Instalacion_Servicios::finalizado_informe, &loop,&QEventLoop::exit);
        connect(this, &other_task_screen::closing, informe, &Informe_Instalacion_Servicios::close);
        guardar_cambios();
        informe->setData(tarea_a_actualizar);
        informe->show();
        switch (loop.exec())
        {
        case QDialog::Accepted:
            o = tarea_a_actualizar;
            populateView(false);
            on_pb_update_server_info_clicked();
            on_pb_close_clicked();
            break;
        case QDialog::Rejected:
            break;
        }
    }
}




void other_task_screen::Pdf_printer(QPixmap Before_installation, QPixmap After_installation, QPixmap Lectura, QPixmap Firma,
                                    QPixmap Empresa, QString Direccion , QString Num_abonado, QString tels, QString Observaciones,
                                    QString Cambiado, QString Num_serie, QString Calibre, bool incidencia_normal)
{
    QString string1, string2, string3, num_serie_antes, lectura_ult, lect_new,
            nom_firmante, num_carnet_firmante;

    QString obs = "";
    QString temp = tarea_a_actualizar.value(MENSAJE_LIBRE).toString().trimmed();
    if(checkIfFieldIsValid(temp)) {
        obs += temp;
    }
    temp = tarea_a_actualizar.value(observaciones).toString().trimmed();
    if(checkIfFieldIsValid(temp)) {
        obs += ".\n"+temp;
    }
    if(obs.size() > 80){
        obs.truncate(80);
        obs+="...";
    }
    Observaciones = obs;

    nom_firmante = nullity_check(tarea_a_actualizar.value(nombre_firmante).toString());
    num_carnet_firmante = nullity_check(tarea_a_actualizar.value(numero_carnet_firmante).toString());
    lectura_ult = tarea_a_actualizar.value(lectura_actual).toString();
    lect_new = tarea_a_actualizar.value(lectura_contador_nuevo).toString();
    num_serie_antes = tarea_a_actualizar.value(numero_serie_contador).toString();
    if(!checkIfFieldIsValid(lect_new)){
        lect_new = "0";
    }
    if(!checkIfFieldIsValid(lectura_ult)){
        lectura_ult = tarea_a_actualizar.value(lectura_ultima).toString();
        if(!checkIfFieldIsValid(lectura_ult)){
            lectura_ult = "0";
        }
    }
    if(incidencia_normal){
        string1= "FOTO INCIDENCIA 1 N.SERIE, " + Num_serie;
        string2= "FOTO INCIDENCIA 2";
        string3= "FOTO INCIDENCIA 3";
    }else{
        string1= "ANTES DEL CAMBIO. N.SERIE, " + num_serie_antes;
        string2= "DESPUÉS DEL CAMBIO. N.SERIE, " + Num_serie + ", LECTURA INICIAL "+lect_new+"m3";
        string3= "FOTO DE ÚLTIMA LECTURA, "+ lectura_ult +"m3";
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName("desire printer name");
    QPrintDialog dialog(&printer,this);
    if(dialog.exec()==QDialog::Rejected) return;

    QPainter painter(&printer);

    painter.setPen(Qt::black);
    painter.drawText(300,200,Direccion);
    painter.drawText(300,400,"NUMERO DE ABONADO: " + Num_abonado
                     + "  Tel: "+ tels);
    painter.drawPixmap(QRect(4000,200,700,417),Empresa);
    painter.drawText(300,600,"OBSERVACIONES: " + Observaciones);
    painter.drawText(300,800,"CAMBIADO POR: " + Cambiado);
    painter.drawText(300,1000,"NUMERO DE SERIE: " + Num_serie);
    painter.drawText(1800,1000,"CALIBRE: " + Calibre);


    painter.setPen(Qt::black);
    painter.drawText(700,1475,string1);
    painter.drawPixmap(QRect(300,1650,2000,2000),Before_installation);

    painter.drawText(3000,1475,string2);
    painter.drawPixmap(QRect(2800,1650,2000,2000),After_installation);

    painter.drawText(950,3800,string3);
    painter.drawPixmap(QRect(300,3900,2000,2000),Lectura);

    painter.drawText(2800,3950,"NOMBRE DEL FIRMANTE: "+ nom_firmante);
    painter.drawText(3000,4100,"NUMERO DE CARNET: "+ num_carnet_firmante);
    painter.drawText(3500,4300,"FIRMA:");
    painter.drawPixmap(QRect(3100,4400,1000,1000),Firma);

    if(checkIfFieldIsValid(num_serie_antes)){
        QString barcodeText = num_serie_antes;
        QRect barcodeRect = QRect(515,950,2750,365);
        //        QRect barcodeTextRect = QRect(5*MmToDot,20.5*MmToDot,67.5*MmToDot,5*MmToDot);

        int id = QFontDatabase::addApplicationFont(":/fonts/Code-128.ttf");
        QFontDatabase::applicationFontFamilies(id).at(0);
        QFont barcodefont = QFont("Code-128", 46, QFont::Normal);
        barcodefont.setLetterSpacing(QFont::AbsoluteSpacing,0.0);
        painter.setFont(barcodefont);

        QString arr = BarcodeGenerator::encodeBarcode(barcodeText);
        painter.drawText(barcodeRect, Qt::AlignCenter, arr);

        //        painter.setFont(QFont("PT Sans", 10));
        //        painter.drawText(barcodeTextRect, Qt::AlignCenter, barcodeText);
    }
    if(checkIfFieldIsValid(Num_serie) && Num_serie!=num_serie_antes){
        QString barcodeText = Num_serie;
        QRect barcodeRect = QRect(3860,950,2750,365);
        //        QRect barcodeTextRect = QRect(5*MmToDot,20.5*MmToDot,67.5*MmToDot,5*MmToDot);

        int id = QFontDatabase::addApplicationFont(":/fonts/Code-128.ttf");
        QFontDatabase::applicationFontFamilies(id).at(0);
        QFont barcodefont = QFont("Code-128", 46, QFont::Normal);
        barcodefont.setLetterSpacing(QFont::AbsoluteSpacing,0.0);
        painter.setFont(barcodefont);

        QString arr = BarcodeGenerator::encodeBarcode(barcodeText);
        painter.drawText(barcodeRect, Qt::AlignCenter, arr);

        //        painter.setFont(QFont("PT Sans", 10));
        //        painter.drawText(barcodeTextRect, Qt::AlignCenter, barcodeText);
    }
    painter.end();

    //    GlobalFunctions::showMessage(this,"Creado PDF", "El PDF ha sido creado correctamente");
    //    bool c = QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
    //    if(!c){
    //        GlobalFunctions::showMessage(this, "Error abriendo", "No se encoontró el archivo PDF de esta tarea");
    //    }
    //bool c = QDesktopServices::openUrl(QUrl::fromLocalFile("C:/Users/Adrian/Documents/build-untitled3-Desktop_Qt_5_13_0_MinGW_32_bit-Debug/3.oxps"));

}

void other_task_screen::configuraAndCreatePdf(bool show){

    QString dir = tarea_a_actualizar.value(poblacion).toString() + ", " + tarea_a_actualizar.value(calle).toString()
            + ", " + tarea_a_actualizar.value(numero).toString()
            + ", " + tarea_a_actualizar.value(BIS).toString()  + ", " + tarea_a_actualizar.value(piso).toString()
            + " " + tarea_a_actualizar.value(mano).toString()  + ", "+ tarea_a_actualizar.value(nombre_cliente).toString() ;

    dir.replace(", ,", ", ").replace(",,", ",");
    QPixmap before_installation;
    QPixmap after_installation;
    QPixmap lectura;
    QPixmap incidencia_1;
    QPixmap incidencia_2;
    QPixmap incidencia_3;
    QPixmap firma;
    QPixmap gestor;

    if(ui->lb_foto_antes_instalacion->pixmap() != NULL
            && !ui->lb_foto_antes_instalacion->isDefaultPhoto()){
        before_installation = *ui->lb_foto_antes_instalacion->pixmap();
    }
    if(ui->lb_foto_despues_instalacion->pixmap() != NULL
            && !ui->lb_foto_despues_instalacion->isDefaultPhoto()){
        after_installation = *ui->lb_foto_despues_instalacion->pixmap();
    }
    if(ui->lb_foto_lectura->pixmap() != NULL
            && !ui->lb_foto_lectura->isDefaultPhoto()){
        lectura = *ui->lb_foto_lectura->pixmap();
    }
    if(ui->lb_foto_incidencia_1->pixmap() != NULL
            && !ui->lb_foto_incidencia_1->isDefaultPhoto()){
        incidencia_1 = *ui->lb_foto_incidencia_1->pixmap();
    }
    if(ui->lb_foto_incidencia_2->pixmap() != NULL
            && !ui->lb_foto_incidencia_2->isDefaultPhoto()){
        incidencia_2 = *ui->lb_foto_incidencia_2->pixmap();
    }
    if(ui->lb_foto_incidencia_3->pixmap() != NULL
            && !ui->lb_foto_incidencia_3->isDefaultPhoto()){
        incidencia_3 = *ui->lb_foto_incidencia_3->pixmap();
    }

    if(ui->lb_firma_cliente->pixmap() != NULL
            && !ui->lb_firma_cliente->isDefaultPhoto()){
        firma = *ui->lb_firma_cliente->pixmap();
    }
    if(ui->l_logo->pixmap() != NULL){
        gestor = *ui->l_logo->pixmap();
    }

    QString  numSerie = tarea_a_actualizar.value(numero_serie_contador_devuelto).toString();
    if(!checkIfFieldIsValid(numSerie)){
        numSerie = tarea_a_actualizar.value(numero_serie_contador).toString();
    }
    if(tarea_a_actualizar.value(Estado).toString().trimmed() == "INCIDENCIA"){
        Pdf_creator(incidencia_1, incidencia_2, incidencia_3,
                    firma, gestor, dir, tarea_a_actualizar.value(numero_abonado).toString(),
                    nullity_check(tarea_a_actualizar.value(telefono1).toString()) +" " + nullity_check(tarea_a_actualizar.value(telefono2).toString()),
                    tarea_a_actualizar.value(observaciones_devueltas).toString(), "MICHEL MORALES VERANES "+ tarea_a_actualizar.value(F_INST).toString(),
                    numSerie,  tarea_a_actualizar.value(calibre_real).toString(), true, show);
    }else{
        Pdf_creator(before_installation, after_installation, lectura,
                    firma, gestor, dir, tarea_a_actualizar.value(numero_abonado).toString(),
                    nullity_check(tarea_a_actualizar.value(telefono1).toString()) +" " + nullity_check(tarea_a_actualizar.value(telefono2).toString()),
                    tarea_a_actualizar.value(observaciones_devueltas).toString(), "MICHEL MORALES VERANES "+ tarea_a_actualizar.value(F_INST).toString(),
                    numSerie,  tarea_a_actualizar.value(calibre_real).toString(), false, show);
    }
}
void other_task_screen::on_pb_crear_pdf_clicked()
{
    configuraAndCreatePdf(true);
}

void other_task_screen::Pdf_creator(QPixmap Before_installation,QPixmap After_installation,QPixmap Lectura,QPixmap Firma,
                                    QPixmap Empresa,QString Direccion , QString Num_abonado, QString tels, QString Observaciones,
                                    QString Cambiado,QString Num_serie,QString Calibre, bool incidencia_normal, bool show)
{

    QString string1, string2, string3, num_serie_antes, lectura_ult, lect_new,
            nom_firmante, num_carnet_firmante;

    QString obs = "";
    QString temp = tarea_a_actualizar.value(MENSAJE_LIBRE).toString().trimmed();
    if(checkIfFieldIsValid(temp)) {
        obs += temp;
    }
    temp = tarea_a_actualizar.value(observaciones).toString().trimmed();
    if(checkIfFieldIsValid(temp)) {
        obs += ".\n"+temp;
    }
    if(obs.size() > 80){
        obs.truncate(80);
        obs+="...";
    }
    Observaciones = obs;

    nom_firmante = nullity_check(tarea_a_actualizar.value(nombre_firmante).toString());
    num_carnet_firmante = nullity_check(tarea_a_actualizar.value(numero_carnet_firmante).toString());
    lectura_ult = tarea_a_actualizar.value(lectura_actual).toString();
    lect_new = tarea_a_actualizar.value(lectura_contador_nuevo).toString();
    num_serie_antes = tarea_a_actualizar.value(numero_serie_contador).toString();
    if(!checkIfFieldIsValid(lect_new)){
        lect_new = "0";
    }
    if(!checkIfFieldIsValid(lectura_ult)){
        lectura_ult = tarea_a_actualizar.value(lectura_ultima).toString();
        if(!checkIfFieldIsValid(lectura_ult)){
            lectura_ult = "0";
        }
    }
    if(incidencia_normal){
        string1= "FOTO INCIDENCIA 1 N.SERIE, " + Num_serie;
        string2= "FOTO INCIDENCIA 2";
        string3= "FOTO INCIDENCIA 3";
    }else{
        string1= "ANTES DEL CAMBIO. N.SERIE, " + num_serie_antes;
        string2= "DESPUÉS DEL CAMBIO. N.SERIE, " + Num_serie + ", LECTURA INICIAL "+lect_new+"m3";
        string3= "FOTO DE ÚLTIMA LECTURA, "+ lectura_ult +"m3";
    }
    files.clear();
    if(filename.isEmpty()){
        filename = QFileDialog::getSaveFileName(this, "Save file","Pdf_de_Trabajo.pdf");
    }
    QPdfWriter pdf(filename);

    QPainter painter(&pdf);

    painter.setPen(Qt::black);
    painter.drawText(500,200,Direccion);
    painter.drawText(500,400,"NUMERO DE ABONADO: " + Num_abonado
                     + "  Tel: "+ tels);
    painter.drawPixmap(QRect(7500,100,1500,800),Empresa);
    painter.drawText(500,600,"OBSERVACIONES: " + Observaciones);
    painter.drawText(500,800,"CAMBIADO POR: " + Cambiado);
    painter.drawText(500,1000,"NUMERO DE SERIE: " + Num_serie);
    painter.drawText(3000,1000,"CALIBRE: " + Calibre);

    painter.setPen(Qt::black);
    if(Before_installation!=QPixmap()){
        painter.drawText(1300,2000,string1);
        painter.drawPixmap(QRect(500,2200,4000,5000),Before_installation);
    }
    if(After_installation!=QPixmap()){
        painter.drawText(4950,2000,string2);
        painter.drawPixmap(QRect(5100,2200,4000,5000),After_installation);
    }
    if(Lectura!=QPixmap()){
        painter.drawText(1500,7900,string3);
        painter.drawPixmap(QRect(500,8000,4000,5000),Lectura);
    }
    if(Firma!=QPixmap()){
        painter.drawText(5100,8100,"NOMBRE DEL FIRMANTE: " +nom_firmante);
        painter.drawText(5550,8500,"NUMERO DE CARNET: " +num_carnet_firmante);
        painter.drawText(6800,8900,"Firma");

        painter.drawPixmap(QRect(6050,9000,2000,2500),Firma);
    }

    if(checkIfFieldIsValid(num_serie_antes)){
        QString barcodeText = num_serie_antes;
        QRect barcodeRect = QRect(700,1300,3750,500);
        //        QRect barcodeTextRect = QRect(5*MmToDot,20.5*MmToDot,67.5*MmToDot,5*MmToDot);

        int id = QFontDatabase::addApplicationFont(":/fonts/Code-128.ttf");
        QFontDatabase::applicationFontFamilies(id).at(0);
        QFont barcodefont = QFont("Code-128", 46, QFont::Normal);
        barcodefont.setLetterSpacing(QFont::AbsoluteSpacing,0.0);
        painter.setFont(barcodefont);

        QString arr = BarcodeGenerator::encodeBarcode(barcodeText);
        painter.drawText(barcodeRect, Qt::AlignCenter, arr);

        //        painter.setFont(QFont("PT Sans", 10));
        //        painter.drawText(barcodeTextRect, Qt::AlignCenter, barcodeText);
    }
    if(checkIfFieldIsValid(Num_serie) && Num_serie!=num_serie_antes){
        QString barcodeText = Num_serie;
        QRect barcodeRect = QRect(5250,1300,3750,500);
        //        QRect barcodeTextRect = QRect(5*MmToDot,20.5*MmToDot,67.5*MmToDot,5*MmToDot);

        int id = QFontDatabase::addApplicationFont(":/fonts/Code-128.ttf");
        QFontDatabase::applicationFontFamilies(id).at(0);
        QFont barcodefont = QFont("Code-128", 46, QFont::Normal);
        barcodefont.setLetterSpacing(QFont::AbsoluteSpacing,0.0);
        painter.setFont(barcodefont);

        QString arr = BarcodeGenerator::encodeBarcode(barcodeText);
        painter.drawText(barcodeRect, Qt::AlignCenter, arr);

        //        painter.setFont(QFont("PT Sans", 10));
        //        painter.drawText(barcodeTextRect, Qt::AlignCenter, barcodeText);
    }

    painter.end();

    //    QImage image;
    //    painter.begin()

    files<<filename;
    //  bool c = QDesktopServices::openUrl(QUrl::fromLocalFile("C:/Users/Adrian/Desktop/file2.pdf"));

    if(show){
        GlobalFunctions gf(this, empresa);
        if(gf.showQuestion(this,"Creado PDF", "El PDF ha sido creado correctamente.¿Desea abrirlo?",
                           QMessageBox::Ok, QMessageBox::No)==QMessageBox::Ok){
            bool c = QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
            if(!c){
                GlobalFunctions::showMessage(this, "Error abriendo", "No se encoontró el archivo PDF de esta tarea");
            }
        }
    }
}

void other_task_screen::on_pb_imprimir_pdf_clicked()
{
    QString dir = tarea_a_actualizar.value(poblacion).toString() + ", " + tarea_a_actualizar.value(calle).toString()
            + ", " + tarea_a_actualizar.value(numero).toString()
            + ", " + tarea_a_actualizar.value(BIS).toString()  + ", " + tarea_a_actualizar.value(piso).toString()
            + " " + tarea_a_actualizar.value(mano).toString()  + ", MICHEL MORALES VERANES";

    dir.replace(", ,", ", ").replace(",,", ",");
    QPixmap before_installation;
    QPixmap after_installation;
    QPixmap lectura;
    QPixmap incidencia_1;
    QPixmap incidencia_2;
    QPixmap incidencia_3;
    QPixmap firma;
    QPixmap gestor;

    if(ui->lb_foto_antes_instalacion->pixmap() != NULL
            && !ui->lb_foto_antes_instalacion->isDefaultPhoto()){
        before_installation = *ui->lb_foto_antes_instalacion->pixmap();
    }
    if(ui->lb_foto_despues_instalacion->pixmap() != NULL
            && !ui->lb_foto_despues_instalacion->isDefaultPhoto()){
        after_installation = *ui->lb_foto_despues_instalacion->pixmap();
    }
    if(ui->lb_foto_lectura->pixmap() != NULL
            && !ui->lb_foto_lectura->isDefaultPhoto()){
        lectura = *ui->lb_foto_lectura->pixmap();
    }
    if(ui->lb_foto_incidencia_1->pixmap() != NULL
            && !ui->lb_foto_incidencia_1->isDefaultPhoto()){
        incidencia_1 = *ui->lb_foto_incidencia_1->pixmap();
    }
    if(ui->lb_foto_incidencia_2->pixmap() != NULL
            && !ui->lb_foto_incidencia_2->isDefaultPhoto()){
        incidencia_2 = *ui->lb_foto_incidencia_2->pixmap();
    }
    if(ui->lb_foto_incidencia_3->pixmap() != NULL
            && !ui->lb_foto_incidencia_3->isDefaultPhoto()){
        incidencia_3 = *ui->lb_foto_incidencia_3->pixmap();
    }

    if(ui->lb_firma_cliente->pixmap() != NULL
            && !ui->lb_firma_cliente->isDefaultPhoto()){
        firma = *ui->lb_firma_cliente->pixmap();
    }
    if(ui->l_logo->pixmap() != NULL){
        gestor = *ui->l_logo->pixmap();
    }
    //    QPixmap before_installation (dir_foto_antes_instalacion);
    //    QPixmap after_installation (dir_foto_despues_instalacion);
    //    QPixmap lectura (dir_foto_lectura);
    //    QPixmap incidencia_1 (dir_foto_incidencia_1);
    //    QPixmap incidencia_2 (dir_foto_incidencia_2);
    //    QPixmap incidencia_3 (dir_foto_incidencia_3);

    //    QPixmap firma (dir_foto_firma);
    //    QPixmap empresa (*ui->l_logo->pixmap());


    QString  numSerie = tarea_a_actualizar.value(numero_serie_contador_devuelto).toString();
    if(!checkIfFieldIsValid(numSerie)){
        numSerie = tarea_a_actualizar.value(numero_serie_contador).toString();
    }
    if(tarea_a_actualizar.value(Estado).toString().trimmed() == "INCIDENCIA"){
        Pdf_printer(incidencia_1, incidencia_2, incidencia_3,
                    firma, gestor, dir, tarea_a_actualizar.value(numero_abonado).toString(),
                    nullity_check(tarea_a_actualizar.value(telefono1).toString()) +" " + nullity_check(tarea_a_actualizar.value(telefono2).toString()),
                    tarea_a_actualizar.value(observaciones_devueltas).toString(), "MICHEL MORALES VERANES "+tarea_a_actualizar.value(F_INST).toString(),
                    numSerie,  tarea_a_actualizar.value(calibre_real).toString(), true);
    }else{
        Pdf_printer(before_installation, after_installation, lectura,
                    firma, gestor, dir, tarea_a_actualizar.value(numero_abonado).toString(),
                    nullity_check(tarea_a_actualizar.value(telefono1).toString()) +" " + nullity_check(tarea_a_actualizar.value(telefono2).toString()),
                    tarea_a_actualizar.value(observaciones_devueltas).toString(), "MICHEL MORALES VERANES "+tarea_a_actualizar.value(F_INST).toString(),
                    numSerie,  tarea_a_actualizar.value(calibre_real).toString(), false);
    }

}

void other_task_screen::on_pb_enviar_pdf_clicked()
{
    if(screen_tabla_tareas::emailPermission==0){
        screen_tabla_tareas::emailPermission = 1;
        screen_tabla_tareas::writeVariablesInBD();
        QString link = "https://myaccount.google.com/lesssecureapps";
        QDesktopServices::openUrl(QUrl(link));
        GlobalFunctions::showMessage(this,"Acceso de aplicacion al GMail", "Por favor, permita el acceso de la aplicacion para poder enviar el mensaje y luego presione ok");
    }
    MailDialog *mail_dialog= new MailDialog();
    connect(mail_dialog,SIGNAL(send_my_email(QString)),this,SLOT(my_mail(QString)));
    connect(mail_dialog,SIGNAL(send_my_clave(QString)),this,SLOT(my_clave(QString)));
    connect(mail_dialog,SIGNAL(send_destiny_email(QString)),this,SLOT(destiny_mail(QString)));

    if(mail_dialog->exec()){
        //        smtp = new Smtp("131995adrian@gmail.com", "epicentro", "smtp.gmail.com", 465);
        smtp = new Smtp(myMail, myClave, "smtp.gmail.com", 465);

        connect(smtp, SIGNAL(status(QString)), this, SLOT(mailSent(QString)));

        if  ( !files.isEmpty() )
            //        smtp->sendMail("131995adrian@gmail.com","adriannieves201912@gmail.com", "5" , "hola", files);
            smtp->sendMail(myMail,destinyMail, "PDF validado" , "Tarea realizada", files);
        else {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","No existe Pdf !");
            messageBox.setFixedSize(500,200);
        }
    }

    disconnect(mail_dialog,SIGNAL(send_my_email(QString)),this,SLOT(my_mail(QString)));
    disconnect(mail_dialog,SIGNAL(send_my_clave(QString)),this,SLOT(my_clave(QString)));
    disconnect(mail_dialog,SIGNAL(send_destiny_email(QString)),this,SLOT(destiny_mail(QString)));
}

void other_task_screen::my_mail(QString mail)
{
    myMail = mail;
}

void other_task_screen::my_clave(QString clave)
{
    myClave = clave;
}

void other_task_screen::destiny_mail(QString mail)
{
    destinyMail = mail;
}

void other_task_screen::mailSent(QString status)
{

    if(status == "Message sent")
    {
        //       smtp->disconnect();
        show_loading("Mensaje Enviado");
        timer.setInterval(2000);
        connect(&timer,SIGNAL(timeout()),this,SLOT(close_message()));
        timer.start();
        //       GlobalFunctions::showMessage(this,"Información","Mensaje Enviado");
        //       ui->statusBar->showMessage("Ok","Mensaje Enviado !");

    }
    else if (status == "Failed to send message"){
        show_loading("Mensaje Fallido");
        timer.setInterval(2000);
        connect(&timer,SIGNAL(timeout()),this,SLOT(close_message()));
        timer.start();
        //       GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Información","Mensaje Fallido");
        //       ui->statusBar->showMessage("Error","Mensaje Fallido !");

    }
    //   GlobalFunctions gf(this);
    GlobalFunctions::showWarning( 0, tr( "Qt Simple SMTP client" ), tr( "Message sent!\n\n" ) );
    //               //smtp->disconnect();}

}
void other_task_screen::close_message(){
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(close_message()));
    hide_loading();
}

void other_task_screen::highlightMapIcon(){
    QString geo_google_maps = o.value(url_geolocalizacion).toString().trimmed();
    if(!geo_google_maps.isEmpty() && geo_google_maps!= "null" && geo_google_maps!= "NULL"){
        ui->pb_abrir_ubicacion_en_mapa->setIcon(QIcon(":/icons/mapa_icon_2.png"));
        QFont font = ui->pb_abrir_ubicacion_en_mapa->font();
        font.setPointSize(12);
        font.setBold(true);
        ui->pb_abrir_ubicacion_en_mapa->setStyleSheet(QStringLiteral("QPushButton{"
                                                                     "border-radius: 6px;"
                                                                     "color: rgb(54, 141, 206);"
                                                                     "}"
                                                                     "QPushButton:hover:!pressed{"
                                                                     "background-color: #EEEEEE; "
                                                                     "}"));
        ui->pb_abrir_ubicacion_en_mapa->setFont(font);
        ui->pb_abrir_ubicacion_en_mapa->setText("  Geolocalización");
    }else{
        QFont font = ui->pb_abrir_ubicacion_en_mapa->font();
        font.setPointSize(12);
        font.setBold(false);
        ui->pb_abrir_ubicacion_en_mapa->setStyleSheet(QStringLiteral("QPushButton{"
                                                                     "border-radius: 6px;"
                                                                     "color: rgb(113, 113, 113);"
                                                                     "}"
                                                                     "QPushButton:hover:!pressed{"
                                                                     "background-color: #EEEEEE; "
                                                                     "}"));
        ui->pb_abrir_ubicacion_en_mapa->setFont(font);
        ui->pb_abrir_ubicacion_en_mapa->setIcon(QIcon(":/icons/mapa_icon_2_off.png"));
        ui->pb_abrir_ubicacion_en_mapa->setText("  Abrir Mapa");
    }
}

void other_task_screen::setGeoCode(const QString geocode)
{
    ui->le_geolocalizacion->setText(geocode);
    ui->le_url_google_maps->setText("https://maps.google.com/?q="+ geocode);

    tarea_a_actualizar.insert(geolocalizacion, geocode);
    tarea_a_actualizar.insert(url_geolocalizacion, "https://maps.google.com/?q="+ geocode);

    o.insert(geolocalizacion, geocode);
    o.insert(url_geolocalizacion, "https://maps.google.com/?q="+ geocode);

    highlightMapIcon();

    geoCodeChanged = true;
}

void other_task_screen::setGeoCodeHand(const QString geocode)
{
    ui->le_url_google_maps->setText("https://maps.google.com/?q="+ geocode);

    tarea_a_actualizar.insert(codigo_de_localizacion, geocode);
    tarea_a_actualizar.insert(url_geolocalizacion, "https://maps.google.com/?q="+ geocode);

    o.insert(codigo_de_localizacion, geocode);
    o.insert(url_geolocalizacion, "https://maps.google.com/?q="+ geocode);

    highlightMapIcon();

    geoCodeChanged = true;
}

void other_task_screen::eraseHomeMarker()
{
    if(!checkIfFieldIsValid(o.value(codigo_de_localizacion).toString().trimmed())){
        o.insert(url_geolocalizacion, "");
        tarea_a_actualizar.insert(url_geolocalizacion, "");
    }
    tarea_a_actualizar.insert(geolocalizacion, "");
    o.insert(geolocalizacion, "");
    highlightMapIcon();
}

void other_task_screen::eraseHandMarker()
{
    if(!checkIfFieldIsValid(o.value(geolocalizacion).toString().trimmed())){
        o.insert(url_geolocalizacion, "");
        tarea_a_actualizar.insert(url_geolocalizacion, "");
    }
    tarea_a_actualizar.insert(codigo_de_localizacion, "");
    o.insert(codigo_de_localizacion, "");
    highlightMapIcon();
}

void other_task_screen::on_pb_abrir_ubicacion_en_mapa_clicked()
{
    QString coords = tarea_a_actualizar.value(geolocalizacion).toString().trimmed();
    QString coords_hand = tarea_a_actualizar.value(codigo_de_localizacion).toString().trimmed();

    QStringList split;
    if(checkIfFieldIsValid(coords) || checkIfFieldIsValid(coords_hand)){
    }
    else{
        GlobalFunctions::showMessage(this, "Información", "No hay asociada una geolocalización a este contador, localice el contador");
        //         xcoordenade = 43.263005; //España Bilbao
        //         ycoordenade = -2.934991;
    }
    Mapas *mapa = new Mapas(nullptr, tarea_a_actualizar);
    mapa->setAttribute(Qt::WA_DeleteOnClose);
    QObject::connect(mapa, &Mapas::settedMarker, this, &other_task_screen::setGeoCode);
    QObject::connect(mapa, &Mapas::settedMarkerHand, this, &other_task_screen::setGeoCodeHand);
    QObject::connect(mapa, &Mapas::erase_marker_hand, this, &other_task_screen::eraseHandMarker);
    QObject::connect(mapa, &Mapas::erase_marker_home, this, &other_task_screen::eraseHomeMarker);
    mapa->show();
}

void other_task_screen::update_itacs_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_ITAC_FIELDS,keys,values);
}
bool other_task_screen::updateITACsGeoCode(){

    QStringList cods_emplazamiento;
    QString cod_emplazamiento = ui->le_codigo_geolocalizacion->text().trimmed();
    if(!GlobalFunctions::checkIfFieldIsValid(cod_emplazamiento)){
        return false;
    }

    GlobalFunctions gf(this, empresa);
    if(gf.checkIfItacExist(cod_emplazamiento)){
        cods_emplazamiento << cod_emplazamiento;
        QString geoCode = tarea_a_actualizar.value(codigo_de_localizacion).toString();
        if(!checkIfFieldIsValid(geoCode)){
            geoCode = tarea_a_actualizar.value(geolocalizacion).toString();
        }
        if(checkIfFieldIsValid(geoCode)){
            QJsonObject campos;
            campos.insert(geolocalizacion_itacs, geoCode);
            return updateITAC(cods_emplazamiento, campos);
        }
    }
    return false;
}
bool other_task_screen::updateITAC(QStringList lista_cod_emplazamientos, QJsonObject campos){

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

    connect(this, &other_task_screen::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &other_task_screen::update_itacs_fields_request);

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
bool other_task_screen::updateTareas(){
    QString numIn, geoCodeCasa = tarea_a_actualizar.value(geolocalizacion).toString();
    QString geoCodeMano = tarea_a_actualizar.value(codigo_de_localizacion).toString();
    QString cod_emplazamiento_itac = ui->le_codigo_geolocalizacion->text().trimmed();
    if(!GlobalFunctions::checkIfFieldIsValid(cod_emplazamiento_itac)){
        return false;
    }
    QJsonObject numeros_internos;
    QJsonObject campos;
    QJsonObject tarea;
    QStringList numInternos;

    GlobalFunctions gf(this, empresa);
    numInternos = gf.getTareasList(codigo_de_geolocalizacion, cod_emplazamiento_itac);

    for (int i=0; i < numInternos.size(); i++) {
        numIn = numInternos.at(i);
        numeros_internos.insert(QString::number(i), numIn);
    }

    if(numeros_internos.isEmpty()){
        return true;
    }
    if(checkIfFieldIsValid(geoCodeMano)){
        campos.insert(url_geolocalizacion, "https://maps.google.com/?q="+geoCodeMano);
        campos.insert(codigo_de_localizacion, geoCodeMano);
    }
    if(checkIfFieldIsValid(geoCodeCasa)){
        campos.insert(geolocalizacion, geoCodeCasa);
    }
    campos.insert(date_time_modified, QDateTime::currentDateTime().toString(formato_fecha_hora));

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

    connect(this, &other_task_screen::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &other_task_screen::update_tareas_fields_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::update_tareas_fields_to_server_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}

void other_task_screen::setear_coordenadas(QString coords)
{
    coordenadas_obtenidas = coords;
}

void other_task_screen::on_cb_editar_fotos_toggled(bool checked)
{
    ui->lb_foto_lectura->setEditable(checked);
    ui->lb_foto_incidencia_1->setEditable(checked);
    ui->lb_foto_incidencia_2->setEditable(checked);
    ui->lb_foto_incidencia_3->setEditable(checked);
    ui->lb_foto_numero_serie->setEditable(checked);
    ui->lb_foto_antes_instalacion->setEditable(checked);
    ui->lb_foto_despues_instalacion->setEditable(checked);
    ui->lb_firma_cliente->setEditable(checked);
}


void other_task_screen::on_pb_maximizar_clicked()
{
    if(!this->isMaximized()){
        this->showMaximized();
        //        disconnect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
        disconnect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
        disconnect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));
    }else{
        this->showNormal();
        connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
        connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
        connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));
    }
}

void other_task_screen::on_pb_minimizar_clicked()
{
    if(!this->isMinimized()){
        this->showMinimized();
    }
}
void other_task_screen::get_date_selected(QDate d)
{
    selection_date = d;
}
void other_task_screen::get_time_selected_init(QTime t)
{
    selection_time_init = t;
}
void other_task_screen::get_time_selected_end(QTime t)
{
    selection_time_end = t;
}
void other_task_screen::get_users_selected(QString user)
{
    operario_selected=user;
}
void other_task_screen::get_team_selected(QString team)
{
    equipo_selected=team;
}
void other_task_screen::on_pb_agregar_cita_clicked()
{
    CalendarDialog *calendarDialog = new CalendarDialog(nullptr, true, empresa);
    connect(calendarDialog,SIGNAL(date_selected(QDate)),this,SLOT(get_date_selected(QDate)));
    connect(calendarDialog,SIGNAL(time_selected_init(QTime)),this,SLOT(get_time_selected_init(QTime)));
    connect(calendarDialog,SIGNAL(time_selected_end(QTime)),this,SLOT(get_time_selected_end(QTime)));
    connect(calendarDialog, &CalendarDialog::operator_selected,this, &other_task_screen::get_users_selected);
    connect(calendarDialog, &CalendarDialog::equipo_selected, this, &other_task_screen::get_team_selected);

    if(calendarDialog->exec()){

        QDateTime dateTime = QDateTime(selection_date, selection_time_init);
        QString nuevo_cita_toSet = get_current_date_in_format(selection_date, selection_time_init, selection_time_end);
        QString fecha_hora_cita_seteada = dateTime.toString(formato_fecha_hora);
        QString obs = ui->le_observaciones->text();
        if(obs.contains("CITA", Qt::CaseSensitive) || !checkIfFieldIsValid(obs)){
            ui->le_observaciones->setText("Cita -> "+nuevo_cita_toSet);
        }
        if(!operario_selected.isEmpty()){
            ui->le_operario->setText(operario_selected);
        }
        if(!equipo_selected.isEmpty()){
            ui->le_equipo->setText(equipo_selected);
        }
        ui->le_nuevo_citas->setText(nuevo_cita_toSet);
        tarea_a_actualizar.insert(fecha_hora_cita, fecha_hora_cita_seteada);
        tarea_a_actualizar.insert(status_tarea, "IDLE CITA");
    }
}

QString other_task_screen::get_current_date_in_format(QDate date, QTime time, QTime time_end)
{
    QString day_week = map_days_week.value(date.dayOfWeek());
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
void other_task_screen::update_tareas_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA_FIELDS,keys,values);
}
bool other_task_screen::on_pb_tarea_sin_revisar_clicked()
{
    ui->pb_tarea_sin_revisar->hide();

    QJsonObject numeros_internos;
    QJsonObject campos;

    numeros_internos.insert("1", o.value(numero_interno).toString().trimmed());
    campos.insert(ultima_modificacion, "ESCRITORIO "+ administrator_loged);

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

    connect(this, &other_task_screen::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &other_task_screen::update_tareas_fields_request);

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
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;
    emit tarea_revisada(o.value(principal_variable).toString());
    updateTareasFromServer();
    return res;
}

void other_task_screen::on_pb_eliminar_cita_clicked()
{
    GlobalFunctions gf(this, empresa);
    if(QMessageBox::Ok == (gf.showQuestion(this,"Confirmación", "¿Desea eliminar cita de la tarea?", QMessageBox::Ok, QMessageBox::No))){
        ui->le_nuevo_citas->setText("");

        tarea_a_actualizar.insert(fecha_hora_cita,"");
        if(ui->le_observaciones->text().contains("CITA",Qt::CaseInsensitive)
                && ui->le_observaciones->text().contains("LAS",Qt::CaseInsensitive)
                ){
            ui->le_observaciones->setText("");
        }
        if(ui->le_MENSAJE_LIBRE->text().contains("CITA",Qt::CaseInsensitive)
                && ui->le_MENSAJE_LIBRE->text().contains("LAS",Qt::CaseInsensitive)){
            ui->le_MENSAJE_LIBRE->setText("");
        }
        if(!checkIfFieldIsValid(o.value(fechas_tocado_puerta).toString().trimmed()) &&
                !checkIfFieldIsValid(o.value(fechas_nota_aviso).toString().trimmed()) &&
                !checkIfFieldIsValid(o.value(telefonos_cliente).toString().trimmed())
                ){
            tarea_a_actualizar.insert(status_tarea, tarea_a_actualizar.value(status_tarea).toString().remove("CITA").trimmed());
        }
    }
}



//void other_task_screen::on_le_geolocalizacion_textChanged(const QString &arg1)
//{
//    QString coord = get_coordinades_From_Link_GoogleMaps(arg1);
//    if(coord != "..."){
//        ui->le_geolocalizacion->setText(coord);
//        QString url_link = "https://maps.google.com/?q=" + coord;
//        ui->le_url_google_maps->setText(url_link);
//    }
//}


QString other_task_screen::get_date_from_status(QJsonObject object, QString status)
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

QJsonObject other_task_screen::set_date_from_status(QJsonObject object, QString status, QString date, QTime time)
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

void other_task_screen::on_le_status_tarea_editingFinished()
{
    //    QString status = tarea_a_actualizar.value(status_tarea).toString();
    //    status = status.toUpper();
    //    if(status!="IDLE" && status!="IDLE TO_BAT" && status!="IDLE CITA" && status!="DONE"
    //            && status!="CLOSED" && status!="INFORMADA" && status!="REQUERIDA"){
    //        GlobalFunctions gf(this);
    //        GlobalFunctions::showWarning(this,"Campo no válido","No se puede ingresar este estado de tarea porque no es válido");
    //        tarea_a_actualizar.insert(status_tarea, o.value(status_tarea).toString().trimmed());
    //    }
    //    else{
    //        tarea_a_actualizar.insert(status_tarea, status);
    //        o.insert(status_tarea, status);
    //        QString fecha_status = get_date_from_status(o, status);
    //        if(!checkIfFieldIsValid(fecha_status)){
    //            fecha_status = QDateTime::currentDateTime().toString(formato_fecha_hora);
    //            o = set_date_from_status(o, status, fecha_status);
    //            populateView(false);
    //        }
    //    }
}

void other_task_screen::on_le_telefono2_editingFinished()
{
    QString tel = ui->le_telefono2->text();
    if(checkIfFieldIsValid(tel) && checkIfOnlyNumbers(tel)){
        o.insert(telefono2, tel);
        o.insert(telefonos_cliente,o.value(telefonos_cliente).toString().remove("TEL2_INCORRECTO"));
    }
    else {
        if(!tel.isEmpty()){
            GlobalFunctions gf(this);
            GlobalFunctions::showWarning(this,"Campo no válido","No se puede ingresar este télefono porque no es válido");
            ui->le_telefono2->setText(nullity_check(o.value(telefono2).toString().trimmed()));
        }
    }
}

bool other_task_screen::checkIfOnlyNumbers(QString tel)
{
    for (int i=0; i < tel.length(); i++) {
        if(!tel.at(i).isNumber())
        {
            return false;
        }
    }
    return true;
}


void other_task_screen::on_le_numero_serie_contador_editingFinished()
{
    if(ui->le_numero_serie_contador->text().trimmed() != o.value(numero_serie_contador).toString().trimmed()){
        foto_cambiada = true;
    }
}

void other_task_screen::on_le_numero_serie_contador_devuelto_editingFinished()
{
    if(ui->le_numero_serie_contador_devuelto->text().trimmed() != o.value(numero_serie_contador_devuelto).toString().trimmed()){
        foto_cambiada = true;
    }
}

void other_task_screen::on_pb_ver_pdf_clicked()
{
    bool c = QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
    if(!c){
        GlobalFunctions::showMessage(this, "Error abriendo", "No existe el archivo PDF de esta tarea");
    }
}

void other_task_screen::on_le_marca_devuelta_textChanged(const QString &arg1)
{
    ui->le_marca_devuelta->setToolTip(arg1);
}

void other_task_screen::on_le_marca_contador_textChanged(const QString &arg1)
{
    ui->le_marca_contador->setToolTip(arg1);
}

void other_task_screen::on_le_numero_serie_contador_devuelto_textChanged(const QString &arg1)
{
    ui->le_numero_serie_contador_devuelto->setToolTip(arg1);
}

void other_task_screen::on_le_numero_serie_contador_textChanged(const QString &arg1)
{
    ui->le_numero_serie_contador->setToolTip(arg1);
}

void other_task_screen::on_le_nuevo_citas_textChanged(const QString &arg1)
{
    ui->le_nuevo_citas->setToolTip(arg1);
}

void other_task_screen::on_le_operario_textChanged(const QString &arg1)
{
    ui->le_operario->setToolTip(arg1);
}

void other_task_screen::on_cb_estado_tarea_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(!ui->cb_estado_tarea->currentText().isEmpty() && ui->cb_estado_tarea->currentText()!= "POR DEFECTO"){
        tarea_a_actualizar.insert(status_tarea, mapaEstados.value(ui->cb_estado_tarea->currentText(), "IDLE"));
    }else {
        QString status = o.value(status_tarea).toString();
        //        ui->cb_estado_tarea->setCurrentText(mapaEstados.key(status));
        tarea_a_actualizar.insert(status_tarea, this->nullity_check(status));
    }
}

void other_task_screen::on_le_ubicacion_en_bateria_editingFinished()
{
    QString ubi = ui->le_ubicacion_en_bateria->text().trimmed();
    if(ubi.contains("BA", Qt::CaseInsensitive) && ubi.contains("-") && tarea_a_actualizar.value(status_tarea).toString()=="IDLE"){
        int key = mapaEstados.values().indexOf("IDLE TO_BAT");
        if(key!=-1 && key < ui->cb_estado_tarea->count()){
            ui->cb_estado_tarea->setCurrentIndex(key + 1);
        }
    }
}

void other_task_screen::agregarPiezas(QStringList piezasList)
{
    ui->cb_piecerio->clear();
    ui->cb_piecerio->addItems(piezasList);
    tarea_a_actualizar.insert(piezas, piezasList.join("\n"));
}

void other_task_screen::on_pb_add_pieza_clicked()
{
    Screen_Table_Piezas *tablePiezas = new Screen_Table_Piezas(nullptr, true, true);
    connect(tablePiezas, &Screen_Table_Piezas::add_piezas,this,
            &other_task_screen::agregarPiezas);

    QString piezas_l = tarea_a_actualizar.value(piezas).toString().trimmed();
    if(checkIfFieldIsValid(piezas_l)){
        tablePiezas->addAlreadyAssingPiezas(piezas_l);
    }
    tablePiezas->show();
}

void other_task_screen::on_pb_eliminar_piezas_clicked()
{
    GlobalFunctions gf(this, empresa);
    if(gf.showQuestion(this,"Confirmación","¿Seguro desea eliminar todas las piezas?"
                       , QMessageBox::Ok, QMessageBox::No)== QMessageBox::Ok){
        ui->cb_piecerio->clear();
        tarea_a_actualizar.insert(piezas, "");
    }
}

void other_task_screen::on_le_causa_origen_textChanged(const QString &arg1)
{
    if(arg1.size() >= 3){
        QString anom = arg1.left(3);
        changeCausaOrigen(anom);
    }
}

QString other_task_screen::changeCausaOrigen(QString anom, bool change_le_causa_origen){ //Si uso esto fuera de  on_le_causa_origen_textChanged debo cambiar tambien el contenido de le_causa_origen

    QString arealizar = Causa::getARealizarFromCodeCausa(anom);
    QString intervencion_l = Causa::getIntervencionFromCodeCausa(anom);
    QString acc_ord = Causa::getAccionOrdenadaFromCodeCausa(anom);
    QString tipo_tarea_l = screen_tabla_tareas::parse_tipo_tarea
            (anom, ui->le_calibre_toma->text().trimmed(), ui->le_marca_contador->text().trimmed());
    if(!anom.isEmpty()){
        tarea_a_actualizar.insert(ANOMALIA, anom);
    }
    if(!arealizar.isEmpty()){
        tarea_a_actualizar.insert(AREALIZAR, arealizar);
    }
    if(!intervencion_l.isEmpty()){
        tarea_a_actualizar.insert(INTERVENCION, intervencion_l);
    }
    if(!acc_ord.isEmpty()){
        ui->le_accion_ordenada->setText(acc_ord);
    }
    if(!tipo_tarea_l.isEmpty()){
        ui->le_tipo_tarea->setText(tipo_tarea_l);
    }
    QString causa_orige_l = anom + " - " + intervencion_l;
    if(change_le_causa_origen && !intervencion_l.isEmpty()){
        ui->le_causa_origen->setText(causa_orige_l);
    }
    return causa_orige_l;
}

void other_task_screen::on_pb_ITAC_clicked()
{
    QString cod_emplazamiento = ui->le_codigo_geolocalizacion->text().trimmed();
    GlobalFunctions gf(this, empresa);
    QJsonObject jsonObject = gf.getItacFromServer(cod_emplazamiento);
    if (!jsonObject.isEmpty()) {
        QJsonObject itac = jsonObject;
        QJsonObject campos;
        campos.insert(gestor_itacs, tarea_a_actualizar.value(GESTOR).toString());
        campos.insert(prioridad_itacs, tarea_a_actualizar.value(prioridad).toString());

        ITAC *oneITACScreen = new ITAC(nullptr, false, empresa, campos);
        connect(oneITACScreen, &ITAC::update_tableITACs,this,
                &other_task_screen::updateITACsFromServer);
        connect(oneITACScreen, &ITAC::updateTableTareas, this, &other_task_screen::updateTareasFromServer);
        connect(oneITACScreen, &ITAC::changedGeoCode, this, &other_task_screen::setGeoCode);
        connect(oneITACScreen, &ITAC::changedGeoCode, this, &other_task_screen::setGeoCodeHand);
        QRect rect = QGuiApplication::screens().first()->geometry();
        if(rect.width() <= 1366
                && rect.height() <= 768){
            oneITACScreen->showMaximized();
        }else {
            oneITACScreen->show();
        }
        oneITACScreen->setData(itac);
        return;
    } //Si no existe el itac abro uno nuevo
    QString dir = "";
    QString field = ui->le_poblacion->text();
    if(checkIfFieldIsValid(field)){
        dir += field + "  ";
    }
    field = ui->le_calle->text();
    if(checkIfFieldIsValid(field)){
        dir += field + "  ";
    }
    field = ui->le_numero->text();
    if(checkIfFieldIsValid(field)){
        dir += field;
    }
    field = ui->le_BIS->text();
    if(checkIfFieldIsValid(field)){
        dir += "-"+field;
    }
    QJsonObject campos;
    campos.insert(itac_itacs,  dir.trimmed());
    campos.insert(prioridad_itacs, tarea_a_actualizar.value(prioridad).toString());
    campos.insert(gestor_itacs, tarea_a_actualizar.value(GESTOR).toString());
    campos.insert(zona_itacs, tarea_a_actualizar.value(zona).toString());
    campos.insert(equipo_itacs, tarea_a_actualizar.value(equipo).toString());
    campos.insert(operario_itacs, tarea_a_actualizar.value(operario).toString());
    campos.insert(codigo_itac_itacs,  ui->le_codigo_geolocalizacion->text().trimmed());

    QString geoCode = getValidGeoCode(tarea_a_actualizar);
    campos.insert(geolocalizacion_itacs, geoCode);

    ITAC *oneITACScreen = new ITAC(nullptr, true, empresa, campos);
    connect(oneITACScreen, &ITAC::update_tableITACs,this,
            &other_task_screen::updateITACsFromServer);
    connect(oneITACScreen, &ITAC::updateTableTareas, this, &other_task_screen::updateTareasFromServer);
    connect(oneITACScreen, &ITAC::changedGeoCode, this, &other_task_screen::setGeoCode);
    connect(oneITACScreen, &ITAC::changedGeoCode, this, &other_task_screen::setGeoCodeHand);
    QRect rect = QGuiApplication::screens().first()->geometry();
    if(rect.width() <= 1366
            && rect.height() <= 768){
        oneITACScreen->showMaximized();
    }else {
        oneITACScreen->show();
    }

}
QString other_task_screen::getValidGeoCode(QJsonObject jsonObject){
    QString geoCode = jsonObject.value(codigo_de_localizacion).toString();
    if(!checkIfFieldIsValid(geoCode)){
        geoCode = jsonObject.value(geolocalizacion).toString();
    }
    return geoCode;
}
void other_task_screen::updateITACsFromServer(){
    emit updateITACs();
}
void other_task_screen::updateTareasFromServer(){
    if(!closing_window){
        emit task_upload_excecution_result(database_comunication::task_to_server_ok);
    }
}

void other_task_screen::setGeoCodeByCodEmplazamiento()
{
    timerChangingGeoCode.stop();
    disconnect(&timerChangingGeoCode,SIGNAL(timeout()),this,SLOT(setGeoCodeByCodEmplazamiento()));

    if(closing_window){
        return;
    }
    QString cod_emplazamiento = ui->le_codigo_geolocalizacion->text();
    QString zona_l = Ruta::getZonaRutaFromCodEmplazamiento(cod_emplazamiento);

    QJsonObject jsonObjectRuta = Ruta::getRutaObjectFromCodEmplamiento(cod_emplazamiento);
    if(closing_window){
        return;
    }
    if(!jsonObjectRuta.isEmpty()){
        QString radio_l = jsonObjectRuta.value(radio_portal_rutas).toString();
        QString zona_l = jsonObjectRuta.value(barrio_rutas).toString();
        if(checkIfFieldIsValid(zona_l)){
            tarea_a_actualizar.insert(zona, zona_l);
            ui->le_zona->setText(zona_l);
        }
        if(checkIfFieldIsValid(radio_l)){
            tarea_a_actualizar.insert(tipoRadio, radio_l);
        }
    }

    QString geoCode = ITAC::getGeoCodeFromCodeItac(cod_emplazamiento);
    if(closing_window){
        return;
    }
    if(checkIfFieldIsValid(geoCode)){

        if(tarea_a_actualizar.value(url_geolocalizacion).toString() != "https://maps.google.com/?q="+geoCode){
            geoCodeChanged = true;
        }

        tarea_a_actualizar.insert(geolocalizacion, geoCode);
        tarea_a_actualizar.insert(codigo_de_localizacion, geoCode);
        tarea_a_actualizar.insert(url_geolocalizacion, "https://maps.google.com/?q="+geoCode);

        ui->le_geolocalizacion->setText(geoCode);
        ui->le_url_google_maps->setText("https://maps.google.com/?q="+geoCode);
    }
}
void other_task_screen::on_le_codigo_geolocalizacion_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(timerChangingGeoCode.isActive()){
        timerChangingGeoCode.stop();
    }
    timerChangingGeoCode.setInterval(350);
    timerChangingGeoCode.start();
    connect(&timerChangingGeoCode,SIGNAL(timeout()),this,SLOT(setGeoCodeByCodEmplazamiento()));

}
void other_task_screen::setObservation(QString obs){
    observacion_seleccionada = obs;
}
void other_task_screen::on_pb_add_observacion_clicked()
{
    ObservationSeletionScreen *observaciones = new ObservationSeletionScreen(this);
    connect(observaciones, &ObservationSeletionScreen::selected_observation, this, &other_task_screen::setObservation);
    if(observaciones->exec()){
        QString obs = tarea_a_actualizar.value(observaciones_devueltas).toString();
        if(!obs.contains(observacion_seleccionada)){
            if(checkIfFieldIsValid(obs)){
                obs += "\n"+observacion_seleccionada;
            }else{
                obs = observacion_seleccionada;
            }
            tarea_a_actualizar.insert(observaciones_devueltas, obs);
            ui->le_observaciones_devueltas->addItem(observacion_seleccionada);
        }
    }
}

void other_task_screen::on_pb_erase_observacion_clicked()
{
    ui->le_observaciones_devueltas->removeSelected();
    QStringList list = ui->le_observaciones_devueltas->items();
    if(!list.isEmpty()){
        tarea_a_actualizar.insert(observaciones_devueltas, list.join("\n"));
    }
}


void other_task_screen::on_pb_play_audio_clicked()
{
    QString audio = tarea_a_actualizar.value(audio_detalle).toString().trimmed();
    if(checkIfFieldIsValid(audio)){
        ui->pb_play_audio->setText("Cargando Audio...");
        ui->pb_play_audio->setIcon(QIcon(":/icons/loading.png"));
        QTimer::singleShot(100, this, SLOT(reproducirAudio()));
    }else{

    }
}
void other_task_screen::reproducirAudio(){
    QString audio = tarea_a_actualizar.value(audio_detalle).toString().trimmed();
    QString audio_dir_file = getLocalDirofCurrentTask() + "/" + audio;
    QFile file(audio_dir_file);
    if(file.exists()){
        QString pr = audio_dir_file;
        ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 2000, true);
        ui->pb_play_audio->setText("Reproducir Detalles");
        ui->pb_play_audio->setIcon(QIcon(":/icons/play.png"));
    }else{
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"No encontrado", "Audio no disponible");
        ui->pb_play_audio->hide();
    }
}

void other_task_screen::on_le_numero_serie_contador_devuelto_textEdited(const QString &arg1)
{
    devuelto = true;
    timer.stop();
    if(arg1.size() >= 4 && (!arg1.contains(lastSerieRequested) || lastSerieRequested.trimmed().isEmpty()) ){
        timer.setInterval(1500);
        connect(&timer,SIGNAL(timeout()),this,SLOT(requestContadoresList()));
        timer.start();
    }
}
void other_task_screen::requestContadoresList(){
    if(closing_window){
        return;
    }
    disconnect(&timer,SIGNAL(timeout()),this,SLOT(requestContadoresList()));
    if(devuelto){
        lastSerieRequested = ui->le_numero_serie_contador_devuelto->text();
    }else{
        lastSerieRequested = ui->le_numero_serie_contador->text();
    }
    if(devuelto){
        show_loading(ui->widget_loading_series_devuelto,
                     QPoint(0 + ui->widget_loading_series_devuelto->width()/2 ,
                            0+ ui->widget_loading_series_devuelto->height()/2),
                     color_blue_app, 25, 25, false);
    }else{
        show_loading(ui->widget_loading_series,
                     QPoint(0 + ui->widget_loading_series->width()/2 ,
                            0+ ui->widget_loading_series->height()/2),
                     color_blue_app, 25, 25, false);
    }
    getContadoresList(lastSerieRequested);
    hide_loading();
}

void other_task_screen::on_le_numero_serie_contador_textEdited(const QString &arg1)
{
    devuelto = false;
    timer.stop();
    if(arg1.size() >= 4 && (!arg1.contains(lastSerieRequested) || lastSerieRequested.trimmed().isEmpty()) ){
        timer.setInterval(1500);
        connect(&timer,SIGNAL(timeout()),this,SLOT(requestContadoresList()));
        timer.start();
    }
}

void other_task_screen::on_le_numero_interno_textEdited(const QString &arg1)
{
    if(arg1.isEmpty()){
        ui->pb_update_server_info->setEnabled(false);
    }
    else{
        ui->pb_update_server_info->setEnabled(true);
    }
}

void other_task_screen::on_pb_enviar_mensaje_clicked()
{
    QString phone1 = ui->le_telefono1->text();
    QString phone2 = ui->le_telefono2->text();
    QString dir = GlobalFunctions::getDirOfTask(tarea_a_actualizar);
    QString gestor = ui->cb_gestor->currentText();
    QString abonado = ui->le_numero_abonado->text();
    if(!GlobalFunctions::checkIfFieldIsValid(phone1) && !GlobalFunctions::checkIfFieldIsValid(phone2)){
        GlobalFunctions::showWarning(this, "Sin Teléfonos", "No hay teléfonos disponibles en esta tarea");
        return;
    }
    SendWhatsappMessage *sendWhatsappMessage = new SendWhatsappMessage(this, gestor, dir, abonado, phone1, phone2);
    sendWhatsappMessage->setAttribute(Qt::WA_DeleteOnClose);
    sendWhatsappMessage->show();
}

void other_task_screen::on_pb_duplicate_info_clicked()
{
    tarea_a_actualizar.insert(numero_serie_contador_devuelto, tarea_a_actualizar.value(numero_serie_contador).toString());
    QString mark = tarea_a_actualizar.value(marca_contador).toString();
    QString largo = tarea_a_actualizar.value(LARGO).toString();
    QString tipo_fluido =  tarea_a_actualizar.value(TIPOFLUIDO).toString();
    QString clase = tarea_a_actualizar.value(TIPO).toString();
    if(mark.isEmpty()){
        largo = "115";
        tipo_fluido = "FRIA";
        clase = "R";
    }
    else if (mark.contains("-")){
        QStringList split = mark.split("-");
        if(split.length() > 0 && split.at(0).contains("0")){
            mark = split.at(0);
        }
    }
    tarea_a_actualizar.insert(marca_devuelta, mark);
    tarea_a_actualizar.insert(calibre_real, tarea_a_actualizar.value(calibre_toma).toString());
    tarea_a_actualizar.insert(largo_devuelto, largo);
    tarea_a_actualizar.insert(RUEDASDV, tarea_a_actualizar.value(ruedas).toString());
    tarea_a_actualizar.insert(TIPOFLUIDO_devuelto, tipo_fluido);
    tarea_a_actualizar.insert(TIPO_devuelto, clase);
    tarea_a_actualizar.insert(tipoRadio_devuelto, tarea_a_actualizar.value(tipoRadio).toString());

    ui->le_numero_serie_contador_devuelto->setText(tarea_a_actualizar.value(numero_serie_contador).toString());
    ui->le_marca_devuelta->setText(mark);
    ui->le_calibre_toma_real->setText(tarea_a_actualizar.value(calibre_toma).toString());
    ui->le_largo_devuelto->setText(largo);
    ui->le_RUEDASDV->setText(tarea_a_actualizar.value(ruedas).toString());
    ui->le_TIPO_FLUIDO_DEVUELTO->setText(tipo_fluido);
    ui->le_TIPO_DEVUELTO->setText(clase);
    ui->le_tipoRadio->setText(tarea_a_actualizar.value(tipoRadio).toString());
}

void other_task_screen::on_le_tipoRadio_textEdited(const QString &arg1)
{
    autocompleteModule(arg1);
}

void other_task_screen::on_le_tipoRadio_editingFinished()
{
    QString arg1 = ui->le_tipoRadio->text();
    autocompleteModule(arg1);
}

void other_task_screen::autocompleteModule(const QString &arg1){
    QString module_value = "";
    if(arg1 == "R3"){
        module_value = "800812";
    }
    else if(arg1 == "R4"){
        module_value = "3031799";
    }
    else if(arg1 == "W4"){
        module_value = "3062884";
    }
    else if(arg1 == "LRW"){
        module_value = "3088016";
    }

    if(!module_value.isEmpty()) {
        QString currentModule = ui->le_numero_serie_modulo->text();
        if(currentModule.trimmed().isEmpty()){
            ui->le_numero_serie_modulo->setText(module_value + ";;;;;");
        }
        else if (currentModule.contains(";")){
            QStringList split = currentModule.split(";");
            split[0] = module_value;
            currentModule = split.join(";");
            ui->le_numero_serie_modulo->setText(currentModule);
            tarea_a_actualizar.insert(numero_serie_modulo, currentModule);
        }
    }
}

void other_task_screen::on_le_MENSAJE_LIBRE_textChanged(const QString &arg1)
{
    QString anomaly = "NZ0";
    if(arg1.contains(anomaly)){
        ui->le_intervencion_devuelta->setText(anomaly);
        tarea_a_actualizar.insert(intervencion_devuelta, anomaly);
    }
}

void other_task_screen::on_pb_set_counter_as_installed_clicked()
{
    show_loading("Cambiando estado de contador...");
    QString counter_string = ui->le_numero_serie_contador_devuelto->text();
    GlobalFunctions gf(nullptr, empresa);
    QStringList list = gf.getContadoresList(true, counter_string);
    if(!list.isEmpty() && list.at(0) == counter_string){
        Screen_tabla_contadores counters(this, empresa);
        QJsonObject campos;
        campos.insert(status_contadores, "INSTALLED");
        counters.updateContadores(list, campos);

        hide_loading();
        GlobalFunctions::showMessage(this,"Éxito","El contador ha sido seteado como instalado.");
        return;
    }
    hide_loading();
    GlobalFunctions::showWarning(this,"Advertencia","El contador no existe o ya está en estado de instalado");
}
