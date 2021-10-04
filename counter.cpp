#include "counter.h"
#include "ui_counter.h"
#include "structure_contador.h"
#include "marca.h"
#include "clase.h"
#include "tipo.h"
#include "longitud.h"
#include "calibre.h"
#include <QDateTime>
#include <QException>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QCompleter>
#include "global_variables.h"
#include "operario.h"
#include "globalfunctions.h"
#include "QProgressIndicator.h"
#include <QScreen>
#include "mylabelshine.h"

Counter::Counter(QWidget *parent, QString empresa) :
    QWidget(parent),
    ui(new Ui::Counter)
{
    setWindowFlags(Qt::CustomizeWindowHint);
    ui->setupUi(this);
    this->empresa = empresa;

    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));

    hide_loading();

    initializeMaps();

    ui->widget_desde_hasta->hide();

    QStringList status;
    status  << "DISPONIBLE" << "INSTALLED";
    ui->l_status->addItems(status);

    this->setAttribute(Qt::WA_DeleteOnClose);
}

void Counter::setOperariosDisponibles(QStringList operarios){

    QCompleter *completer = new QCompleter(operarios, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_encargado->setCompleter(completer);
}

Counter::~Counter()
{
    delete ui;
}
bool Counter::writeSeries(QStringList list){
    QFile *data_base = new QFile(numeros_serie_descargados); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::WriteOnly))
        {
            data_base->seek(0);
            QDataStream out(data_base);
            out<<list;
            data_base->close();
            return true;
        }
    }
    return false;
}
QStringList Counter::readSeries(){
    GlobalFunctions gf(nullptr, GlobalFunctions::readEmpresaSelected());
    QStringList list = gf.getContadoresList();
    return list;
}
QStringList Counter::getSeriesWithJsonArray(QJsonArray jsonArray){
    QStringList numeros_serie_de_contadores;
    for (int i =0; i < jsonArray.size(); i++) {
        QString serie = jsonArray[i].toObject().value(numero_serie_contadores).toString().trimmed();
        if(!numeros_serie_de_contadores.contains(serie)){
            numeros_serie_de_contadores << serie;
        }
    }
    return numeros_serie_de_contadores;
}
bool Counter::writeCounters(QJsonArray jsonArray){
    QFile *data_base = new QFile(contadores_descargados); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base->exists()) {
        if(data_base->open(QIODevice::WriteOnly))
        {
            data_base->seek(0);
            QDataStream out(data_base);
            out<<jsonArray;
            data_base->close();
            writeSeries(getSeriesWithJsonArray(jsonArray));
            return true;
        }
    }
    return false;
}

void Counter::initializeMaps(){
    QJsonArray jsonArray;
    QStringList lista_calibres, lista_longitudes, lista_operarios;

    lista_tipo_radio.append("R3");
    lista_tipo_radio.append("R4");
    lista_tipo_radio.append("W4");    
    lista_tipo_radio.append("LRW");

    jsonArray = Operario::readOperarios();
    for (int i=0; i < jsonArray.size(); i++) {
        QString tipo_v;
        tipo_v = jsonArray.at(i).toObject().value(operario_operarios).toString();
        lista_operarios.append(tipo_v);
    }

    jsonArray = Tipo::readTipos();
    for (int i=0; i < jsonArray.size(); i++) {
        QString tipo_v;
        tipo_v = jsonArray.at(i).toObject().value(tipo_tipos).toString();
        lista_tipo_fluido.append(tipo_v);
    }
    jsonArray = Longitud::readLongitudes();
    for (int i=0; i < jsonArray.size(); i++) {
        QString longitud_v;
        longitud_v = jsonArray.at(i).toObject().value(longitud_longitudes).toString();
        lista_longitudes.append(longitud_v);
    }

    jsonArray = Calibre::readCalibres();
    for (int i=0; i < jsonArray.size(); i++) {
        QString calibre_v;
        calibre_v = jsonArray.at(i).toObject().value(calibre_calibres).toString();
        lista_calibres.append(calibre_v);
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
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_marca->setCompleter(completer);

    completer = new QCompleter(lista_operarios, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_encargado->setCompleter(completer);

    completer = new QCompleter(lista_calibres, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_calibre->setCompleter(completer);

    completer = new QCompleter(lista_longitudes, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_longitud->setCompleter(completer);

    completer = new QCompleter(lista_tipo_fluido, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_fluido->setCompleter(completer);

    completer = new QCompleter(clases, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_clase->setCompleter(completer);

    completer = new QCompleter(lista_tipo_radio, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive); completer->setFilterMode(Qt::MatchContains);
    ui->le_radio->setCompleter(completer);

}

void Counter::closeEvent(QCloseEvent *event)
{
    emit closing();
    QWidget::closeEvent(event);
}

void Counter::show_loading(QString mess){
    emit hidingLoading();

    QWidget *widget_blur = new QWidget(this);
    QSize size = this->size();
    size += QSize(0,30);
    widget_blur->move(0,0);
    widget_blur->setFixedSize(size);
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &Counter::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &Counter::hidingLoading, widget_blur, &QWidget::deleteLater);

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
    connect(this, &Counter::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &Counter::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &Counter::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &Counter::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &Counter::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}

void Counter::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void Counter::hide_loading(){
    emit hidingLoading();
}
void Counter::on_drag_screen(){

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

void Counter::on_start_moving_screen_timeout(){

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

void Counter::on_drag_screen_released()
{
    if(isFullScreen()){

        return;
    }
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
    //current_win_Pos = QPoint(this->pos().x()-200,this->pos().y()-200);
}

QJsonArray Counter::fixToView(QJsonArray jsonarray)
{
    for (int i=0; i< jsonarray.size();i++) {
        jsonarray[i]= populateFixView(jsonarray[i].toObject());
    }
    return jsonarray;
}

QJsonObject Counter::populateFixView(QJsonObject o)
{
    QString clase_o, radio_o, fluido_o, marca_o, modelo_o, lectura_init;
    marca_o = o.value(marca_contadores).toString();
    modelo_o = o.value(modelo_contadores).toString();
    clase_o = o.value(clase_contadores).toString();
    radio_o = o.value(tipo_radio_contadores).toString();
    fluido_o = o.value(tipo_fluido_contadores).toString();

    QString status = o.value(status_contadores).toString();
    if(!checkIfFieldIsValid(status)){
        status = "DISPONIBLE";
    }
    ui->l_status->setText(status);

    for (int i=0; i < ui->le_marca->completer()->completionCount();i++) {
        ui->le_marca->completer()->setCurrentRow(i);
        QString m = ui->le_marca->completer()->currentCompletion();
        if(m.contains(marca_o, Qt::CaseInsensitive) && m.contains(modelo_o, Qt::CaseInsensitive)){
            if(m.contains(" - ")){
                QStringList string = m.split(" - ");
                if(string.length()>= 3){
                    QString marca_contador = string.at(0);
                    QString modelo_contador = string.at(1);
                    QString codigo_marca_contador = string.at(2);

                    o.insert(marca_contadores, marca_contador);
                    o.insert(modelo_contadores, modelo_contador);
                    o.insert(codigo_marca_contadores, codigo_marca_contador);
                }
            }
            ui->le_marca->setText(m);
            break;
        }
        qDebug()<<"item -> "+m;
    }
    for (int i=0; i < ui->le_clase->completer()->completionCount();i++) {
        ui->le_clase->completer()->setCurrentRow(i);
        QString m = ui->le_clase->completer()->currentCompletion();
        if(m.contains(clase_o, Qt::CaseInsensitive)){
            if(m.contains(" - ")){
                QStringList string = m.split(" - ");
                if(string.length()>= 2){
                    QString clase_contador = string.at(1);
                    QString codigo_clase_contador = string.at(0);

                    o.insert(clase_contadores, clase_contador);
                    o.insert(codigo_clase_contadores, codigo_clase_contador);
                }
            }
            break;
        }
        qDebug()<<"item -> "+m;
    }
    for (int i=0; i < ui->le_radio->completer()->completionCount();i++) {
        ui->le_radio->completer()->setCurrentRow(i);
        QString m = ui->le_radio->completer()->currentCompletion();
        if(m.contains(radio_o, Qt::CaseInsensitive)){
            o.insert(tipo_fluido_contadores, m);
            break;
        }
        qDebug()<<"item -> "+m;
    }
    for (int i=0; i < ui->le_fluido->completer()->completionCount();i++) {
        ui->le_fluido->completer()->setCurrentRow(i);
        QString m = ui->le_fluido->completer()->currentCompletion();
        if(m.contains(fluido_o, Qt::CaseInsensitive)){
            o.insert(tipo_radio_contadores, m);
            break;
        }
        qDebug()<<"item -> "+m;
    }
    return o;
}
bool Counter::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(var!=nullptr && !var.isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
void Counter::populateView(QJsonObject o)
{
    ui->le_serie->setText(o.value(numero_serie_contadores).toString());
    ui->le_calibre->setText(o.value(calibre_contadores).toString());
    ui->le_longitud->setText(o.value(longitud_contadores).toString());
    ui->le_ruedas->setText(o.value(ruedas_contador_contadores).toString());
    ui->le_lectura_inicial->setText(o.value(lectura_inicial_contadores).toString());

    QString clase_o, radio_o, fluido_o, codigo_marca_o, encargado;
    codigo_marca_o = o.value(codigo_marca_contadores).toString();
    clase_o = o.value(clase_contadores).toString();
    radio_o = o.value(tipo_radio_contadores).toString();
    fluido_o = o.value(tipo_fluido_contadores).toString();
    encargado = o.value(encargado_contadores).toString();
    ui->le_encargado->setText(encargado);

    QString status = o.value(status_contadores).toString();
    if(!checkIfFieldIsValid(status)){
        status = "DISPONIBLE";
    }
    ui->l_status->setText(status);

    for (int i=0; i < ui->le_marca->completer()->completionCount();i++) {
        ui->le_marca->completer()->setCurrentRow(i);
        QString m = ui->le_marca->completer()->currentCompletion();
        if(m.contains(codigo_marca_o + " - ", Qt::CaseInsensitive)){
            ui->le_marca->setText(m);
            break;
        }
        qDebug()<<"item -> "+m;
    }
    for (int i=0; i < ui->le_clase->completer()->completionCount();i++) {
        ui->le_clase->completer()->setCurrentRow(i);
        QString m = ui->le_clase->completer()->currentCompletion();
        QString n = m;
        if(n.replace(" ", "").contains(clase_o.replace(" ", ""), Qt::CaseInsensitive)){
            ui->le_clase->setText(m);
            break;
        }
        qDebug()<<"item -> "+m;
    }
    for (int i=0; i < ui->le_radio->completer()->completionCount();i++) {
        ui->le_radio->completer()->setCurrentRow(i);
        QString m = ui->le_radio->completer()->currentCompletion();
        if(m.contains(radio_o, Qt::CaseInsensitive)){
            ui->le_radio->setText(m);
            break;
        }
        qDebug()<<"item -> "+m;
    }
    for (int i=0; i < ui->le_fluido->completer()->completionCount();i++) {
        ui->le_fluido->completer()->setCurrentRow(i);
        QString m = ui->le_fluido->completer()->currentCompletion();
        if(m.contains(fluido_o, Qt::CaseInsensitive)){
            ui->le_fluido->setText(m);
            break;
        }
        qDebug()<<"item -> "+m;
    }
}

QString Counter::eliminarNumerosAlFinal(QString string){
    for(int n = string.size()-1; n >= 0; n--) {
        if(string.at(n).isDigit()){
            string.remove(n, 1);
        }else{
            break;
        }
    }
    return string;
}

QString Counter::eliminarCharacteresAlFinal(QString string){
    for(int n = string.size()-1; n >= 0; n--) {
        if(!string.at(n).isDigit()){
            string.remove(n, 1);
        }else{
            break;
        }
    }
    return string;
}

bool Counter::subirContador(QString serie){
    serie = serie.trimmed();

    if(serie.contains(" ")){
        QStringList list = serie.split(" ");
        if(list.length()>=2){
            contador.insert(anno_o_prefijo_contadores, list.at(0).trimmed());
        }
    }else {
        QString pref = eliminarNumerosAlFinal(serie).trimmed();
        contador.insert(anno_o_prefijo_contadores, pref);
    }
    //    serie = serie.replace(" ", "");
    contador.insert(numero_serie_contadores, serie);

    if(!ui->le_encargado->text().isEmpty()){
        contador.insert(encargado_contadores,ui->le_encargado->text());
    }
    if(!ui->le_calibre->text().isEmpty()){
        contador.insert(calibre_contadores,ui->le_calibre->text());
    }
    if(!ui->le_longitud->text().isEmpty()){
        contador.insert(longitud_contadores,ui->le_longitud->text());
    }
    if(!ui->le_ruedas->text().isEmpty()){
        contador.insert(ruedas_contador_contadores,ui->le_ruedas->text());
    }
    if(!ui->le_lectura_inicial->text().isEmpty()){
        contador.insert(lectura_inicial_contadores,ui->le_lectura_inicial->text());
    }
    if(!ui->le_marca->text().isEmpty()){
        QString marca_contador;
        QString modelo_contador;
        QString codigo_marca_contador;

        QString marca_modelo = ui->le_marca->text();
        if(marca_modelo.contains(" - ")){
            QStringList string = marca_modelo.split(" - ");
            if(string.length()>= 3){
                marca_contador = string.at(1);
                modelo_contador = string.at(2);
                codigo_marca_contador = string.at(0);
            }
        }
        contador.insert(marca_contadores, marca_contador);
        contador.insert(modelo_contadores, modelo_contador);
        contador.insert(codigo_marca_contadores, codigo_marca_contador);
    }
    if(!ui->le_clase->text().isEmpty()){
        QString clase_y_codigo;
        QString clase_contador;
        QString codigo_clase_contador;

        clase_y_codigo = ui->le_clase->text();
        if(clase_y_codigo.contains(" - ")){
            QStringList string = clase_y_codigo.split(" - ");
            if(string.length()>= 2){
                clase_contador = string.at(1);
                codigo_clase_contador = string.at(0);
            }
        }
        contador.insert(clase_contadores, clase_contador);
        contador.insert(codigo_clase_contadores, codigo_clase_contador);
    }
    if(!ui->le_fluido->text().isEmpty()){
        QString fluido= ui->le_fluido->text();
        contador.insert(tipo_fluido_contadores, fluido);
    }
    if(!ui->le_radio->text().isEmpty()){
        QString radio= ui->le_radio->text();
        contador.insert(tipo_radio_contadores, radio);
    }

    QString status = ui->l_status->text();
    if(status == "DISPONIBLE"){
        status = "";
    }
    contador.insert(status_contadores, status);

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    contador.insert(date_time_modified_contadores, timestamp);

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(contador);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    keys << "json" << "empresa";
    QString temp = QString::fromUtf8(ba);
    values << temp << empresa.toLower();


    QEventLoop *q = new QEventLoop();

    connect(this, &Counter::script_excecution_result,q,&QEventLoop::exit);

    GlobalFunctions gf(this, empresa);
    if(gf.checkIfCounterExist(numero_serie_contadores, serie)){
        update_contador_request(keys, values);
    }
    else{
        create_contador_request(keys, values);
    }

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::contador_to_server_ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::update_contador_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::create_contador_to_server_failed:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }


    delete q;

    return res;
}

void Counter::on_pb_create_new_or_update_contador_clicked()
{
    if(ui->radioButton->isChecked()){
        QStringList series;
        QString serie;
        int cant = ui->sb_hasta->value() - ui->sb_desde->value();
        if(!ui->le_serie->text().isEmpty()){
            serie = (ui->le_serie->text().trimmed());
        }
        else{
            GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Numero de serie vacio","Inserte el numero de serie");
            return;
        }
        if(cant > 0){
            ui->pb_create_new_or_update_contador->setEnabled(false);
            show_loading("Espere, subiendo información...");
            bool res = true;
            int pos = getFirstLetterPosition(serie);
            for(int i=0; i<= cant; i++){
                setLoadingText("Subiendo ("+QString::number(i)+" / "+QString::number(cant)+")...");
                QString last_part_serie = QString::number(ui->sb_desde->value()+i);
                while(last_part_serie.length()< pos){
                    last_part_serie ="0"+last_part_serie;
                }
                serie = serie.left(serie.length() - pos) + last_part_serie;
                qDebug()<<serie;
                res = subirContador(serie);
            }
            if(res){
                hide_loading();
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada correctamente en servidor.");
            }else{
                hide_loading();
                GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
            }
            ui->pb_create_new_or_update_contador->setEnabled(true);
            emit updateTablecontadores();

        }else{
            GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"1 solo Numero de serie","En modo multiple inserte desde el "
                                                               "primer numero de serie hasta el "
                                                               "ultimo a insertar");
            return;
        }
    }
    else{
        if(!ui->le_serie->text().isEmpty()){
            ui->pb_create_new_or_update_contador->setEnabled(false);
            show_loading("Espere, subiendo información...");

            if(subirContador(ui->le_serie->text().trimmed())){
                hide_loading();
                GlobalFunctions::showMessage(this,"Éxito","Información actualizada correctamente en servidor.");
            }else{
                hide_loading();
                GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
            }
            ui->pb_create_new_or_update_contador->setEnabled(true);
            emit updateTablecontadores();
        }
        else{
            GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Numero de serie vacio","Inserte el numero de serie");
            return;
        }
    }
    on_pb_cruz_clicked();
}
void Counter::create_contador_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::CREATE_CONTADOR,keys,values);
}
void Counter::update_contador_request(QStringList keys, QStringList values)
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::UPDATE_CONTADOR,keys,values);
}
void Counter::get_all_serial_numbers_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_ALL_SERIAL_NUMBERS,keys,values);
}
bool Counter::getAllSerialNumbers()
{
    QStringList keys,values;

    keys << "empresa";
    values << empresa.toLower();

    for(int i = 0, reintentos = 0; i < 1;i++)
    {
        QEventLoop q;

        connect(this, &Counter::script_excecution_result,&q,&QEventLoop::exit);

        this->keys = keys;
        this->values = values;

        QTimer::singleShot(DELAY, this, SLOT(get_all_serial_numbers_request()));

        switch (q.exec())
        {
        case database_comunication::script_result::timeout:
            i--;
            reintentos++;
            if(reintentos == RETRIES)
            {
                //                i = 1;
                return false;
            }
            break;

        case database_comunication::script_result::get_all_serial_numbers_failed:
            i--;
            reintentos++;
            if(reintentos == RETRIES)
            {
                //                i = 1;
                return false;
            }
            break;
        case database_comunication::script_result::ok:
            return true;
        }
    }
    return false;
}
void Counter::on_pb_cruz_clicked()
{
    this->close();
}

void Counter::on_radioButton_clicked()
{

    if(!ui->radioButton->isChecked()){
        ui->widget_desde_hasta->hide();
    }
    else{
        if(ui->le_serie->text().isEmpty()){
            GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Numero de serie vacio","Inserte el numero de serie");
            ui->radioButton->setChecked(false);
            return;
        }else{
            ui->widget_desde_hasta->show();
            QString serie = ui->le_serie->text();
            int pos = getFirstLetterPosition(serie);
            serie = serie.right(pos);
            bool ok;
            serie.toInt(&ok);
            if(ok){
                ui->sb_desde->setValue(serie.toInt());
                ui->sb_hasta->setValue(serie.toInt());
            }
        }
    }
}

int Counter::getFirstLetterPosition(QString string){
    int pos = 0;
    for(int i = string.size()-1; i>=0; i--){
        QChar c = string.at(i);
        if(c.isLetter()){
            break;
        }
        pos++;
    }
    if( pos == -1){
        pos = string.size();
    }
    return pos;
}

void Counter::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo){
    int result = -1;
    QString respuesta = QString::fromUtf8(ba);
    if(tipo == database_comunication::GET_ALL_SERIAL_NUMBERS)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        ba.remove(0,2);
        ba.chop(2);

        if(ba.contains("not success get_all_numeros_series"))
        {
            result = database_comunication::script_result::get_all_serial_numbers_failed;
        }
        else
        {
            QJsonArray jsonArrayAllSerialNumbers = database_comunication::getJsonArray(ba);
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::CREATE_CONTADOR)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            if(ba.contains("updating")){
                result = database_comunication::script_result::update_contador_to_server_failed;
            }else if(ba.contains("creating")){
                //            emit script_excecution_result(database_comunication::script_result::upload_task_image_failed);
                result = database_comunication::script_result::create_contador_to_server_failed;
            }
        }
        else
        {
            if(ba.contains("success ok update_contador"))
            {
                result = database_comunication::script_result::contador_to_server_ok;
            }
        }
    }
    else if(tipo == database_comunication::UPDATE_CONTADOR)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        if(ba.contains("ot success"))
        {
            result = database_comunication::script_result::update_contador_to_server_failed;

        }
        else
        {
            if(ba.contains("success ok update_contador"))
            {
                result = database_comunication::script_result::contador_to_server_ok;
            }
        }
    }
    script_excecution_result(result);
}
