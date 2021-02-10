#include "resumen_tareas.h"
#include "ui_resumen_tareas.h"
#include "calendardialog.h"
#include "global_variables.h"
#include <QListWidget>
#include <QStandardItemModel>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include "new_table_structure.h"

Resumen_Tareas::Resumen_Tareas(QWidget *parent, QJsonArray jsonArray, QString empresa) :
    QWidget(parent),
    ui(new Ui::Resumen_Tareas)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);
    this->empresa = empresa;
    this->setWindowTitle("Resumen");
    jsonArrayTareas = jsonArray;
    fillListWidgetWithTypes(jsonArrayTareas);
    ui->widget_tareas_por_emplazamiento->hide();

    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));
}

Resumen_Tareas::~Resumen_Tareas()
{
    delete ui;
}
void Resumen_Tareas::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    if(e->size().width() > 380){
       this->setFixedWidth(380);
    }

}

void Resumen_Tareas::on_drag_screen(){

    if(isFullScreen()){
        if(QApplication::mouseButtons()==Qt::RightButton){


        }
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

void Resumen_Tareas::on_start_moving_screen_timeout(){

    int x_pos = static_cast<int>(this->pos().x()+((QWidget::mapFromGlobal(QCursor::pos())).x() - init_pos_x));
    int y_pos = static_cast<int>(this->pos().y()+((QWidget::mapFromGlobal(QCursor::pos())).y() - init_pos_y));
    x_pos = (x_pos < 0)?0:x_pos;
    y_pos = (y_pos < 0)?0:y_pos;

    x_pos = (x_pos > QApplication::desktop()->width()-100)?QApplication::desktop()->width()-100:x_pos;
    y_pos = (y_pos > QApplication::desktop()->height()-180)?QApplication::desktop()->height()-180:y_pos;

    this->move(x_pos,y_pos);

    init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
    init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
}

void Resumen_Tareas::on_drag_screen_released()
{
    if(isFullScreen()){

        return;
    }
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
    //current_win_Pos = QPoint(this->pos().x()-200,this->pos().y()-200);
}

void Resumen_Tareas::showWithAnimation(){

    QRect startValue = QRect(this->pos().x(), this->pos().y(), 0, this->height());
    QRect endValue= QRect(startValue.x(),startValue.y() //posicion sinmodificar
                          ,this->width()
                          ,startValue.height());

    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(200);
    animation->setStartValue(startValue);
    connect(animation, &QPropertyAnimation::finished, animation, &QPropertyAnimation::deleteLater);
    animation->setEndValue(endValue);

}

void Resumen_Tareas::setTareas(QJsonArray jsonArray)
{
    jsonArrayTareas = jsonArray;
    fillListWidgetWithTypes(jsonArrayTareas);
}

bool Resumen_Tareas::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
QString Resumen_Tareas::getStringFromCoord(QGeoCoordinate coords){
    if(coords.isValid()){
        QString latitud = QString::number(coords.latitude(), 'f', 13);
        QString longitud = QString::number(coords.longitude(), 'f', 13);
//        qDebug()<<"Coords: -> "<<coords;
//        qDebug()<<"Conversion: -> " + latitud + "," + longitud;
        return latitud + "," + longitud;
    }
    return "";
}
QGeoCoordinate Resumen_Tareas::getCoordsFromString(QString string, double &xcoord, double &ycoord){
    QStringList split;
    split = string.split(",");
    if(split.length() > 1){
        QString x_string = split.at(0).trimmed();
        QString y_string = split.at(1).trimmed();
        bool x_ok, y_ok;
        xcoord = x_string.toDouble(&x_ok);
        ycoord = y_string.toDouble(&y_ok);
        if(x_ok && y_ok){
            return QGeoCoordinate(xcoord, ycoord);
        }
    }
    return QGeoCoordinate();
}
void Resumen_Tareas::fillListWidgetWithTypes(QJsonArray jsonArray){
    ui->listWidget->clear();
    double x,y;
    QMap<QString, int> tipos_t;
    mapa_tipos_geocodes.clear();
    mapa_tipos_description.clear();
    for (int i =0; i< jsonArray.size(); i++) {
        QString geoCode = jsonArray.at(i).toObject().value(codigo_de_localizacion).toString();
        if(!checkIfFieldIsValid(geoCode)){
            geoCode = jsonArray.at(i).toObject().value(geolocalizacion).toString();
        }
        QString tipo_t = jsonArray.at(i).toObject().value(tipo_tarea).toString().trimmed();
        if(!checkIfFieldIsValid(tipo_t)){
            tipo_t = "?";
        }
        geoCode = getStringFromCoord(getCoordsFromString(geoCode, x, y));
        if(tipos_t.keys().contains(tipo_t)){
            int cant = tipos_t.value(tipo_t);
            cant++;
            tipos_t.remove(tipo_t);
            tipos_t.insert(tipo_t, cant);
            mapa_tipos_geocodes.insert(tipo_t + " : "+ QString::number(cant), geoCode);
            mapa_tipos_description.insert(tipo_t + " : "+ QString::number(cant), i);
        }else{
            tipos_t.insert(tipo_t, 1);
            mapa_tipos_geocodes.insert(tipo_t + " : 1", geoCode);
            mapa_tipos_description.insert(tipo_t + " : 1",i);
        }
    }
    QStringList list;
    if(!tipos_t.isEmpty()){
        QString tipo_t = "";
        for (int i = 0; i < tipos_t.keys().size(); i++) {
            tipo_t = QString::number(tipos_t.value(tipos_t.keys().at(i))) + " - " + tipos_t.keys().at(i);
            list << tipo_t;
        }
    }
    list.sort();
    ui->listWidget->addItems(list);
}
void Resumen_Tareas::on_pb_cruz_clicked()
{
    emit closing();
    this->close();
}
void Resumen_Tareas::on_pb_minimizar_clicked()
{
    this->showMinimized();
}

void Resumen_Tareas::on_listWidget_tareas_emplazamiento_itemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    on_pb_openTarea_clicked();
}

void Resumen_Tareas::on_listWidget_tareas_emplazamiento_itemClicked(QListWidgetItem *item)
{
    if(jsonArrayTareasEmplazamiento.isEmpty()){
        return;
    }
    double x,y;
    QString view = item->text();
    QModelIndexList selection = ui->listWidget_tareas_emplazamiento->selectionModel()->selectedRows();
    if(!selection.isEmpty()){
        int pos = selection.at(0).row();
        if(pos < jsonArrayTareasEmplazamiento.size()){
            lastTareasSelected = jsonArrayTareasEmplazamiento.at(pos).toObject();
            ui->l_descripcion->setText(getViewOfTarea(lastTareasSelected));
            QString c_accion_ordenada = lastTareasSelected.value(tipo_tarea).toString().trimmed();
            if(checkIfFieldIsValid(c_accion_ordenada)){
                for (int i=0; i < ui->listWidget->count(); i++) {
                    QListWidgetItem *item = ui->listWidget->item(i);
                    if(item->text().contains(c_accion_ordenada)){
                        item->setSelected(true);
                    }
                }
            }
            QString geoCode = lastTareasSelected.value(codigo_de_localizacion).toString();
            if(!checkIfFieldIsValid(geoCode)){
                geoCode = lastTareasSelected.value(geolocalizacion).toString();
            }
            geoCode = getStringFromCoord(getCoordsFromString(geoCode, x, y));
            emit moveMapCenter(geoCode);
            if(!selection.isEmpty()){
                ui->l_total_tareas_emplzamientos->setText(QString::number(pos + 1)+
                                                          "/" + QString::number(ui->listWidget_tareas_emplazamiento->count()));
            }
        }
    }
}
void Resumen_Tareas::focusTareaCoords(QJsonArray jsonArray)
{
    if(jsonArray.isEmpty()){
        return;
    }
    jsonArrayTareasEmplazamiento = jsonArray;
    QJsonObject jsonObject;
    QString c_emplazamineto ="";
    ui->listWidget_tareas_emplazamiento->clear();
    
    for (int i =0; i < jsonArray.size(); i++) {
        jsonObject = jsonArray.at(i).toObject();
        if(c_emplazamineto.isEmpty()){
            QString field = jsonObject.value(codigo_de_geolocalizacion).toString().trimmed();
            if(checkIfFieldIsValid(field)){
                c_emplazamineto = field;
                ui->l_emplazamiento_selected->setText(c_emplazamineto);
            }
        }
        QListWidgetItem *item = getItemView(jsonObject);
        ui->listWidget_tareas_emplazamiento->addItem(item);
    }
    
    lastTareasSelected = jsonArray.at(0).toObject();
    QString c_accion_ordenada = lastTareasSelected.value(tipo_tarea).toString().trimmed();
    if(checkIfFieldIsValid(c_accion_ordenada)){
        for (int i=0; i < ui->listWidget->count(); i++) {
            QListWidgetItem *item = ui->listWidget->item(i);
            if(item->text().contains(c_accion_ordenada)){
                item->setSelected(true);
            }
        }
    }
    ui->l_descripcion->setText(getViewOfTarea(lastTareasSelected));
    ui->l_total_tareas_emplzamientos->setText(QString::number(1)+
                                              "/" + QString::number( jsonArray.size()));
    
    ui->widget_tareas_por_emplazamiento->show();
    ui->widget_tareas_por_accion_odenada->hide();
}
QListWidgetItem* Resumen_Tareas::getItemView(QJsonObject jsonObject){
    QString view = getViewOfTarea(jsonObject);
    QListWidgetItem *item =new QListWidgetItem;

    bool nueva_info = false;
    QString ult_mod = jsonObject.value(ultima_modificacion).toString().trimmed();
    QString prioridad_l = jsonObject.value(prioridad).toString().trimmed();
    QString fecha_cita = jsonObject.value(fecha_hora_cita).toString().trimmed();
    QString msg = "", obs = jsonObject.value(observaciones).toString().trimmed();
    if(checkIfFieldIsValid(obs)){
        msg += "OBS ->  "+ obs +"\n";
    }
    obs = jsonObject.value(observaciones_devueltas).toString().trimmed();
    if(checkIfFieldIsValid(obs)){
        msg += "OBS DV ->  "+ obs + "\n";
    }
    obs = jsonObject.value(MENSAJE_LIBRE).toString().trimmed();
    if(checkIfFieldIsValid(obs)){
        msg += "MSG LIBRE ->  "+ obs + "\n";
    }
    obs = jsonObject.value(f_instnew).toString().trimmed();
    if(checkIfFieldIsValid(obs)){
        msg += "ULT MODIFICACIÓN ->  "+ obs;
    }

    if(checkIfFieldIsValid(fecha_cita)){
        QDateTime date_cita = QDateTime::fromString(fecha_cita, formato_fecha_hora);
        if(date_cita <= QDateTime::currentDateTime()){
            QString status = jsonObject.value(status_tarea).toString().trimmed();
            if(status.contains("IDLE")){
                item->setData(Qt::BackgroundRole, QColor(229,190,50));//citas vencidas
                msg = "Cita Vencida -> " + date_cita.toString(formato_fecha_hora_new_view) + "\n" + msg;
            }
        }
    }
    if(ult_mod.contains("ANDROID", Qt::CaseInsensitive)){
        nueva_info = true;
        item->setData(Qt::BackgroundRole, QColor(0,100,0));//Tareas sin revisar
        item->setData(Qt::ForegroundRole, QColor(255,255,255));
    }
    if(prioridad_l == "ALTA"){
        if(nueva_info){
            item->setData(Qt::ForegroundRole, QColor(255,100,100));
        }else{
            item->setData(Qt::ForegroundRole, QColor(255,50,50));
        }
        QFont font = ui->listWidget->font();
        font.setBold(true);
        font.setPointSize(8);
        item->setData(Qt::FontRole, font);
    }
    if(checkIfFieldIsValid(msg)){
        item->setData(Qt::ToolTipRole, msg);
    }else{
        item->setData(Qt::ToolTipRole, "Esta tarea no tiene mensaje libre");
    }

    item->setData(Qt::DisplayRole, view);
    return item;
}
QListWidgetItem* Resumen_Tareas::getItemView(QJsonObject jsonObject, QListWidgetItem* item){
    QString view = getViewOfTarea(jsonObject);
    bool nueva_info = false;
    QString ult_mod = jsonObject.value(ultima_modificacion).toString().trimmed();
    QString prioridad_l = jsonObject.value(prioridad).toString().trimmed();
    QString fecha_cita = jsonObject.value(fecha_hora_cita).toString().trimmed();
    QString msg = "", obs = jsonObject.value(observaciones).toString().trimmed();
    if(checkIfFieldIsValid(obs)){
        msg += "OBS ->  "+ obs +"\n";
    }
    obs = jsonObject.value(observaciones_devueltas).toString().trimmed();
    if(checkIfFieldIsValid(obs)){
        msg += "OBS DV ->  "+ obs + "\n";
    }
    obs = jsonObject.value(MENSAJE_LIBRE).toString().trimmed();
    if(checkIfFieldIsValid(obs)){
        msg += "MSG LIBRE ->  "+ obs + "\n";
    }
    obs = jsonObject.value(f_instnew).toString().trimmed();
    if(checkIfFieldIsValid(obs)){
        msg += "ULT MODIFICACIÓN ->  "+ obs;
    }

    if(checkIfFieldIsValid(fecha_cita)){
        QDateTime date_cita = QDateTime::fromString(fecha_cita, formato_fecha_hora);
        if(date_cita <= QDateTime::currentDateTime()){
            QString status = jsonObject.value(status_tarea).toString().trimmed();
            if(status.contains("IDLE")){
                item->setData(Qt::BackgroundRole, QColor(229,190,50));//citas vencidas
                msg = "Cita Vencida -> " + date_cita.toString(formato_fecha_hora_new_view) + "\n" + msg;
            }
        }
    }
    if(ult_mod.contains("ANDROID", Qt::CaseInsensitive)){
        nueva_info = true;
        item->setData(Qt::BackgroundRole, QColor(0,100,0));//Tareas sin revisar
        item->setData(Qt::ForegroundRole, QColor(255,255,255));
    }
    if(prioridad_l == "ALTA"){
        if(nueva_info){
            item->setData(Qt::ForegroundRole, QColor(255,100,100));
        }else{
            item->setData(Qt::ForegroundRole, QColor(255,50,50));
        }
        QFont font = ui->listWidget->font();
        font.setBold(true);
        font.setPointSize(8);
        item->setData(Qt::FontRole, font);
    }
    if(checkIfFieldIsValid(msg)){
        item->setData(Qt::ToolTipRole, msg);
    }else{
        item->setData(Qt::ToolTipRole, "Esta tarea no tiene mensaje libre");
    }

    item->setData(Qt::DisplayRole, view);
    return item;
}

void Resumen_Tareas::on_listWidget_tareas_tipo_itemDoubleClicked(QListWidgetItem *item)
{Q_UNUSED(item);
    on_pb_openTarea_clicked();
}

void Resumen_Tareas::on_listWidget_tareas_tipo_itemClicked(QListWidgetItem *item)
{
    double x,y;
    QString view = item->text();
    int pos = mapa_tareas_views.value(view);
    if(pos < jsonArrayTareas.size()){
        lastTareasSelected = jsonArrayTareas.at(pos).toObject();
        ui->l_descripcion->setText(getViewOfTarea(lastTareasSelected));
        QString geoCode = lastTareasSelected.value(codigo_de_localizacion).toString();
        if(!checkIfFieldIsValid(geoCode)){
            geoCode = lastTareasSelected.value(geolocalizacion).toString();
        }
        geoCode = getStringFromCoord(getCoordsFromString(geoCode, x, y));
        emit moveMapCenter(geoCode);
        QModelIndexList selection = ui->listWidget_tareas_tipo->selectionModel()->selectedRows();
        if(!selection.isEmpty()){
            ui->l_total_tareas_tipo->setText(QString::number(selection.at(0).row() + 1)+
                                             "/" + QString::number(ui->listWidget_tareas_tipo->count()));
        }
    }
}

void Resumen_Tareas::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QString tipo = item->text();
    focusTarea(tipo);
    ui->widget_tareas_por_emplazamiento->hide();
    ui->widget_tareas_por_accion_odenada->show();
}

void Resumen_Tareas::focusTarea(QString tipo, bool centerMap){
    int total =1;
    QStringList split = tipo.split("-");
    if(split.size() > 1){
        total = split.at(0).trimmed().toInt();
        split.removeFirst();
        tipo = split.join("-").trimmed();
        
        QString key;

        if(last_tipo_t_selected != tipo){
            last_tipo_t_selected = tipo;
            tipo_t_counter=1;
            
            int i = 1;
            
            key = tipo+" : "+ QString::number(i);
            mapa_tareas_views.clear();
            ui->listWidget_tareas_tipo->clear();
            while (mapa_tipos_description.contains(key)) {
                int pos = mapa_tipos_description.value(key);
                QString view = getViewOfTarea( jsonArrayTareas.at(pos).toObject());
                
                QListWidgetItem *item = getItemView(jsonArrayTareas.at(pos).toObject());
                ui->listWidget_tareas_tipo->addItem(item);
                mapa_tareas_views.insert(view, pos);
                i++;
                key = tipo+" : "+ QString::number(i);
            }
            ui->l_tipo_selected->setText(tipo);
            
        }else{
            tipo_t_counter++;
        }
        key = tipo+" : "+ QString::number(tipo_t_counter);
        if(mapa_tipos_geocodes.contains(key)){
            QString geoCode = mapa_tipos_geocodes.value(key);
            if(centerMap){
                emit moveMapCenter(geoCode);
            }
            lastTareasSelected = jsonArrayTareas.at(
                        mapa_tipos_description.value(key)).toObject();
            ui->l_descripcion->setText(getViewOfTarea(lastTareasSelected));
            ui->l_total_tareas_tipo->setText(QString::number(tipo_t_counter)+
                                             "/" + QString::number(total));
        }else{
            tipo_t_counter=1;
            key = tipo+" : "+ QString::number(tipo_t_counter);
            if(mapa_tipos_geocodes.contains(key)){
                QString geoCode = mapa_tipos_geocodes.value(key);
                if(centerMap){
                    emit moveMapCenter(geoCode);
                }
                lastTareasSelected = jsonArrayTareas.at(
                            mapa_tipos_description.value(key)).toObject();
                ui->l_descripcion->setText(getViewOfTarea(lastTareasSelected));
                ui->l_total_tareas_tipo->setText(QString::number(tipo_t_counter)+
                                                 "/" + QString::number(total));
            }
        }
        
    }
}
QString Resumen_Tareas::getViewOfTarea(QJsonObject jsonObject){
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
        view += "\nCITA "+ field;
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

void Resumen_Tareas::on_pb_openTarea_clicked()
{
    if(!lastTareasSelected.isEmpty()){
        emit openTarea(lastTareasSelected.value(principal_variable).toString());
    }
}

void Resumen_Tareas::on_pb_hibernar_clicked()
{
    if(!lastTareasSelected.isEmpty()){
        CalendarDialog *calendar = new CalendarDialog(nullptr, true, empresa, false);
        connect(calendar, &CalendarDialog::date_selected, this, &Resumen_Tareas::setHibernateDate);
        connect(calendar, &CalendarDialog::time_selected_end, this, &Resumen_Tareas::setHibernateTime);
        connect(calendar, &CalendarDialog::operator_selected, this, &Resumen_Tareas::setOperarioSelected);
        connect(calendar, &CalendarDialog::equipo_selected, this, &Resumen_Tareas::setEquipoSelected);
        
        connect(calendar, &CalendarDialog::accepted, this, &Resumen_Tareas::hibernateTarea);
        calendar->show();
    }
}
void Resumen_Tareas::setEquipoSelected(QString equipo){
    equipo_selected = equipo;
}
void Resumen_Tareas::setOperarioSelected(QString operario){
    operario_selected = operario;
}
void Resumen_Tareas::setHibernateDate(QDate date){
    date_selected = date;
}
void Resumen_Tareas::setHibernateTime(QTime time){
    time_selected = time;
}

void Resumen_Tareas::hibernateTarea(){
    if(date_selected.isValid() && time_selected.isValid()){
        updateTareaField();
    }
}

void Resumen_Tareas::update_tareas_fields_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_TAREA_FIELDS,keys,values);
}
bool Resumen_Tareas::updateTareaField(){
    QJsonObject numeros_internos;
    QJsonObject campos;
    QJsonObject tarea;
    QString numin = lastTareasSelected.value(principal_variable).toString().trimmed();
    QString previousPriority = lastTareasSelected.value(prioridad).toString().trimmed();
    if(!checkIfFieldIsValid(previousPriority)){
        previousPriority = "MEDIA";
    }
    numeros_internos.insert("1", numin);
    
    if(numeros_internos.isEmpty()){
        return true;
    }
    QDateTime dateTime;
    dateTime.setDate(date_selected);
    dateTime.setTime(time_selected);
    
    int pos = -1;
    QString view, previous_view = getViewOfTarea(lastTareasSelected);
    if(mapa_tareas_views.contains(previous_view)){
        pos = mapa_tareas_views.value(previous_view);
        mapa_tareas_views.remove(previous_view);
    }
    
    if(!operario_selected.isEmpty()){
        campos.insert(operario, operario_selected);
        lastTareasSelected.insert(operario, operario_selected);
    }
    if(!equipo_selected.isEmpty()){
        campos.insert(equipo, equipo_selected);
        lastTareasSelected.insert(equipo, equipo_selected);
    }
    campos.insert(prioridad, "HIBERNAR");
    campos.insert(hibernacion, dateTime.toString(formato_fecha_hora) + " :: " + previousPriority);
    campos.insert(date_time_modified, QDateTime::currentDateTime().toString(formato_fecha_hora));
    
    lastTareasSelected.insert(prioridad, "HIBERNAR");
    lastTareasSelected.insert(hibernacion, dateTime.toString(formato_fecha_hora) + " :: " + previousPriority);
    lastTareasSelected.insert(date_time_modified, QDateTime::currentDateTime().toString(formato_fecha_hora));
    
    
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
    
    connect(this, &Resumen_Tareas::script_excecution_result,q,&QEventLoop::exit);
    QTimer::singleShot(DELAY, this, &Resumen_Tareas::update_tareas_fields_request);
    
    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;
        
    case database_comunication::script_result::ok:
        campos.insert(principal_variable, numin);
        emit updateTareas(campos);
        view = getViewOfTarea(lastTareasSelected);
        ui->l_descripcion->setText(view);
        if(pos > 0){
            mapa_tareas_views.insert(view, pos);
            if(pos < jsonArrayTareas.size()){
                jsonArrayTareas.replace(pos, lastTareasSelected);
                for (int i=0; i < ui->listWidget_tareas_tipo->count(); i++) {
                    QListWidgetItem *item = ui->listWidget_tareas_tipo->item(i);
                    if(item->text() == previous_view){
                        getItemView(lastTareasSelected, item);
                    }
                }
            }
        }
        for (int i=0; i < ui->listWidget_tareas_emplazamiento->count(); i++) {
            QListWidgetItem *item = ui->listWidget_tareas_emplazamiento->item(i);
            if(item->text() == previous_view){
                getItemView(lastTareasSelected, item);
                jsonArrayTareasEmplazamiento.replace(i, lastTareasSelected);
            }
        }
        res = true;
        break;
        
    case database_comunication::script_result::update_tareas_fields_to_server_failed:
        res = false;
        break;
    }
    delete q;
    
    return res;
}
void Resumen_Tareas::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(ba);
    int result = -1;
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
    
    emit script_excecution_result(result);
}


