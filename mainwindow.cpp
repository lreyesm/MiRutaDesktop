#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QBuffer>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QUrlQuery>
#include "database_comunication.h"
#include <QFile>
#include <QList>
#include <QtXlsx>
#include "selectionorder.h"
#include <QDesktopWidget>
#include "global_variables.h"
#include "new_table_structure.h"
#include "processesclass.h"
#include <QGraphicsDropShadowEffect>
#include "dbtareascontroller.h"
#include "structure_login.h"
#include "screen_upload_itacs.h"
#include "QProgressIndicator.h"
#include "globalfunctions.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    database_com()
{
    ui->setupUi(this);
    //    this->setFixedWidth(269);
    setWindowTitle("Login");
    setWindowFlags(Qt::CustomizeWindowHint);

    ////    setAttribute(Qt::WA_TranslucentBackground); //enable MainWindow to be transparent

    tablaEmpresasScreen = new Screen_Table_Empresas(nullptr, false);

    connect(tablaEmpresasScreen, &Screen_Table_Empresas::empresasReceived,
            this, &MainWindow::empresasDescargadas);


    serverAlredyAnswered = false;
    selected_user = "";

    //    connect(this, &MainWindow::download_administrator_image_signal,
    //            this, &MainWindow::download_administrator_image);

    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));

    iniciateFiles();

    ui->statusBar->showMessage(QString("Mi Ruta ") + versionMiRuta, 2000);

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(2);
    ui->pb_login->setGraphicsEffect(effect);

    ///Nuevo---------------------------------------------------------------------------------------
    QJsonArray jsonArray = loadCredentials();
    QStringList usuarios;
    for (int i =0; i < jsonArray.size(); i++) {
        usuarios << jsonArray.at(i).toObject().value("administrador").toString();
    }
    QCompleter *completer = new QCompleter(usuarios, this);
    connect(completer, SIGNAL(highlighted(QString)), this, SLOT(autocompleteCredentials(QString)));
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->le_username->setCompleter(completer);

    QDir dir = QDir::current();
    QFile file(dir.path()+"/Updater.exe");

    if(file.exists()){
        getFilesFromServer();
        qDebug()<<"Existe updater" + file.fileName();
    }else{
        qDebug()<<"No existe updater";
    }

    this->setFocusProxy(ui->le_username);

    QPoint p = QPoint(0,0);
    p.setX(p.x() + ui->widget_loading_gestor->width()/2);
    p.setY(p.y() + ui->widget_loading_gestor->height()/2);
    show_loading(ui->widget_loading_gestor, p);
    //---------------------------------------------------------------------------------------------
}

///Nuevo---------------------------------------------------------------------------------------------
void MainWindow::autocompleteCredentials(QString user)
{
    QJsonArray jsonArray = loadCredentials();
    for (int i =0; i < jsonArray.size(); i++) {
        QString usuario = jsonArray.at(i).toObject().value("administrador").toString();
        if(usuario == user){
            QString clave = jsonArray.at(i).toObject().value("clave").toString();
            ui->le_password->setText(clave);
            break;
        }
    }
}
void MainWindow::empresasDescargadas(database_comunication::serverRequestType tipo){ //true si la version en servidor es mayor
    Q_UNUSED(tipo);

    disconnect(tablaEmpresasScreen, &Screen_Table_Empresas::empresasReceived,
               this, &MainWindow::empresasDescargadas);

    connect(this, &MainWindow::mouse_pressed, ui->l_empresa, &MyLabelSpinner::hideSpinnerList);

    ui->l_empresa->setSpinnerList(Empresa::getListaNombresEmpresas());
    hide_loading();
}

bool MainWindow::checkVersions(QString serverVersion){ //true si la version en servidor es mayor
    serverVersion.remove("Mi"); //Ej: Mi Ruta Beta 1.66.exe
    serverVersion.remove("Ruta");
    serverVersion.remove("Beta");
    serverVersion.remove(".exe");
    serverVersion = serverVersion.trimmed();
    QString thisVersion = versionMiRuta; //Ej: "V1.66 Servidor Oficial"
    thisVersion.remove("Servidor Oficial");
    thisVersion.remove("V");
    thisVersion = thisVersion.trimmed();
    if(thisVersion < serverVersion){
        QString n = "Actualizar a version: " + serverVersion + "  actual: " + thisVersion;
        qDebug()<<n;
        return true;
    }else{
        QString n = "No actualizar a version: " + serverVersion + "  actual: " + thisVersion;
        qDebug()<<n;
        return false;
    }
}
void MainWindow::callUpdater(){
    GlobalFunctions gf(this);
    if(gf.showQuestion(this, "Actualización disponible", "Desea actualizar Mi Ruta?",
                             QMessageBox::Ok, QMessageBox::No) == QMessageBox::Ok){
        QDir dir = QDir::current();
        QFile file(dir.path()+"/Updater.exe");
        qDebug()<<"Abriendo updater" + file.fileName();
        QString result = ProcessesClass::executeProcess(this, file.fileName(),
                                                        ProcessesClass::WINDOWS,1000, false);
        this->on_pb_cruz_clicked();
    }
}
void MainWindow::getFilesFromServer()
{
    QStringList keys, values;
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_FILES_TO_UPDATE,keys,values);

}
//----------------------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::iniciateFiles(){
    QDir dir;
    dir.setPath(QDir::currentPath() +"/Respaldos");
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    dir.setPath(QDir::currentPath() +"/BD");
    if(!dir.exists()){
        dir.mkpath(dir.path());
    }
    QFile data_base_empresa_selected(GlobalFunctions::empresa_selected_file); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base_empresa_selected.open(QIODevice::WriteOnly)){

        QDataStream out(&data_base_empresa_selected);
        data_base_empresa_selected.close();
    }
    QFile data_base_gestor_selected(GlobalFunctions::gestor_selected_file); // ficheros .dat se puede utilizar formato txt tambien
    if(data_base_gestor_selected.open(QIODevice::WriteOnly)){

        QDataStream out(&data_base_gestor_selected);
        data_base_gestor_selected.close();
    }
    QFile data_base_download_clientes(clientes_descargados); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_clientes.exists()){
        if(data_base_download_clientes.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_clientes);
            data_base_download_clientes.close();
        }
    }
    QFile data_base_download_operarios(operarios_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_operarios.exists()){
        if(data_base_download_operarios.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_operarios);
            data_base_download_operarios.close();
        }
    }
    QFile data_base_download_itacs(itacs_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_itacs.exists()){
        if(data_base_download_itacs.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_itacs);
            data_base_download_itacs.close();
        }
    }
    QFile data_base_download_admin(administradores_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_admin.exists()){
        if(data_base_download_admin.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_admin);
            data_base_download_admin.close();
        }
    }
    QFile data_base_download_empresas(empresas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_empresas.exists()){
        if(data_base_download_empresas.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_empresas);
            data_base_download_empresas.close();
        }
    }
    QFile data_base_download_gestores(gestores_descargados); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_gestores.exists()){
        if(data_base_download_gestores.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_gestores);
            data_base_download_gestores.close();
        }
    }
    QFile data_base_download_infos(infos_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_infos.exists()){
        if(data_base_download_infos.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_infos);
            data_base_download_infos.close();
        }
    }
    QFile data_base_download_calibres(calibres_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_calibres.exists()){
        if(data_base_download_calibres.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_calibres);
            data_base_download_calibres.close();
        }
    }
    QFile data_base_download_longitudes(longitudes_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_longitudes.exists()){
        if(data_base_download_longitudes.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_longitudes);
            data_base_download_longitudes.close();
        }
    }
    QFile data_base_download_ruedas(ruedas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_ruedas.exists()){
        if(data_base_download_ruedas.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_ruedas);
            data_base_download_ruedas.close();
        }
    }
    QFile data_base_download_rutas(rutas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_rutas.exists()){
        if(data_base_download_rutas.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_rutas);
            data_base_download_rutas.close();
        }
    }
    QFile data_base_download_causas(causas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_causas.exists()){
        if(data_base_download_causas.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_causas);
            data_base_download_causas.close();
        }
    }
    QFile data_base_download_resultados(resultados_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_resultados.exists()){
        if(data_base_download_resultados.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_resultados);
            data_base_download_resultados.close();
        }
    }
    QFile data_base_download_equipo_operarios(equipo_operarios_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_equipo_operarios.exists()){
        if(data_base_download_equipo_operarios.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_equipo_operarios);
            data_base_download_equipo_operarios.close();
        }
    }
    QFile data_base_download_clases(clases_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_clases.exists()){
        if(data_base_download_clases.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_clases);
            data_base_download_clases.close();
        }
    }
    QFile data_base_download_emplazamientos(emplazamientos_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_emplazamientos.exists()){
        if(data_base_download_emplazamientos.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_emplazamientos);
            data_base_download_emplazamientos.close();
        }
    }
    QFile data_base_download_tipos(tipos_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_tipos.exists()){
        if(data_base_download_tipos.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_tipos);
            data_base_download_tipos.close();
        }
    }
    QFile data_base_download_observaciones(observaciones_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_observaciones.exists()){
        if(data_base_download_observaciones.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_observaciones);
            data_base_download_observaciones.close();
        }
    }
    QFile data_base_download_zonas(zonas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_zonas.exists()){
        if(data_base_download_zonas.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_zonas);
            data_base_download_zonas.close();
        }
    }
    QFile data_base_download_piezas(piezas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_piezas.exists()){
        if(data_base_download_piezas.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_piezas);
            data_base_download_piezas.close();
        }
    }
    QFile data_base_download_series(numeros_serie_descargados); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_series.exists()){
        if(data_base_download_series.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_series);
            data_base_download_series.close();
        }
    }
    QFile data_base_download_counters(contadores_descargados); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_counters.exists()){
        if(data_base_download_counters.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_counters);
            data_base_download_counters.close();
        }
    }
    QFile data_base_download_marks(marcas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_marks.exists()){
        if(data_base_download_marks.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_marks);
            data_base_download_marks.close();
        }
    }
    QFile data_base_download_works(ficheros_comprimidos_descargados); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_download_works.exists()){
        if(data_base_download_works.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_download_works);
            data_base_download_works.close();
        }
    }
    QFile *data_base_load_works = new QFile(trabajos_cargados); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_load_works->exists()){
        if(data_base_load_works->open(QIODevice::WriteOnly)){

            QDataStream out(data_base_load_works);
            data_base_load_works->close();
        }
    }
    QFile *data_base_tareas = new QFile(tareas_descargadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_tareas->exists()){
        if(data_base_tareas->open(QIODevice::WriteOnly)){

            QDataStream out(data_base_tareas);
            data_base_tareas->close();
        }
    }
    QFile *data_base_tasks = new QFile(tareas_sincronizadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_tasks->exists()){
        if(data_base_tasks->open(QIODevice::WriteOnly)){

            QDataStream out(data_base_tasks);
            data_base_tasks->close();
        }
    }
    QFile *data_base_mod = new QFile(tareas_modificadas); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_mod->exists()){
        if(data_base_mod->open(QIODevice::WriteOnly)){

            QDataStream out(data_base_mod);
            data_base_mod->close();
        }
    }
    QFile data_base_information_local(informacion_local); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base_information_local.exists()){
        if(data_base_information_local.open(QIODevice::WriteOnly)){

            QDataStream out(&data_base_information_local);
            data_base_information_local.close();
        }
    }else if(data_base_information_local.open(QIODevice::ReadOnly)){
        QDataStream in(&data_base_information_local);
        in>>lastEmpresaLogin;
        ui->l_empresa->setText(lastEmpresaLogin);
        data_base_information_local.close();
    }
    QFile *data_base = new QFile(datos); // ficheros .dat se puede utilizar formato txt tambien
    if(!data_base->exists()){
        if(data_base->open(QIODevice::WriteOnly)){

            QDataStream out(data_base);
            out<<screen_tabla_tareas::lastIDSAT;
            out<<screen_tabla_tareas::lastNUMFICHERO_EXPORTACION;
            out<<screen_tabla_tareas::lastIDExp;
            out<<screen_tabla_tareas::emailPermission;
            out<<screen_tabla_tareas::lastSync;
            data_base->close();
        }
    }
    else if(data_base->open(QIODevice::ReadOnly))
    {
        QDataStream in(data_base);
        readVariablesInDB(in);
        data_base->close();
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    on_pb_cruz_clicked();
    QWidget::closeEvent(event);
}

void MainWindow::readVariablesInDB(QDataStream &in){
    in>>screen_tabla_tareas::lastIDSAT;
    in>>screen_tabla_tareas::lastNUMFICHERO_EXPORTACION;
    in>>screen_tabla_tareas::lastIDExp;
    in>>screen_tabla_tareas::emailPermission;
}


void MainWindow::on_drag_screen(){

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

void MainWindow::on_start_moving_screen_timeout(){

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

void MainWindow::on_drag_screen_released()
{
    if(isFullScreen()){

        return;
    }
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
    //current_win_Pos = QPoint(this->pos().x()-200,this->pos().y()-200);
}

void MainWindow::login_request()
{
    QString empresa = Empresa::getCurrentFieldEmpresa(ui->l_empresa->text().trimmed());
    GlobalFunctions::writeEmpresaSelected(empresa);
    QStringList keys, values;
    keys << "empresa" << "administrador" << "clave";
    values << empresa.toLower() << ui->le_username->text().trimmed() << ui->le_password->text();
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::LOGIN,keys,values);
}

void MainWindow::save_login_request()
{
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::SAVE_LOGIN,keys,values);
}

void MainWindow::serverAnswer(QByteArray reply, database_comunication::serverRequestType tipo)
{
    QString respuesta = QString::fromUtf8(reply);
    //    qDebug()<<reply;
    int result = -1;
    if(tipo == database_comunication::LOGIN)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
        //        ui->plainTextEdit->setPlainText(respuesta);
        if(respuesta.contains("login not success"))
        {
            result = database_comunication::script_result::login_failed;
        }
        else
        {
            if(respuesta.contains("login success"))
            {
                result = database_comunication::script_result::ok;
            }
        }
    }
    ///Nuevo---------------------------------------------------------------------------------------------
    else if(tipo == database_comunication::SAVE_LOGIN){
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        reply.remove(0,2);
        reply.chop(2);

        if(reply.contains("ot success save_login"))
        {
            result = database_comunication::script_result::save_login_failed;
        }
        else if(reply.contains("success save_login"))
        {
            result = database_comunication::script_result::ok;
        }
    }
    else if(tipo == database_comunication::GET_FILES_TO_UPDATE)
    {
        reply.chop(2);
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
        QString str = QString::fromUtf8(reply).replace("\n","");
        QStringList files;
        if(str.contains(" -- ")){
            files = str.split(" -- ");
        }
        QString file;
        foreach(file, files){
            if(file.contains("Mi") && file.contains("Ruta")
                    && file.contains(".exe")){
                if(checkVersions(file)){
                    callUpdater();
                }
                break;
            }
        }
    }
    ///---------------------------------------------------------------------------------------------

    else if(tipo == database_comunication::DOWNLOAD_ADMINISTRADOR_IMAGE)
    {
        QString foto = QString::fromUtf8(reply);
        QImage temp_img = database_comunication::getImageFromString(foto);
        if(temp_img.isNull())
        {
            ui->lb_foto->setScaledContents(false);
            ui->lb_foto->setPixmap((QPixmap(":/icons/User_Big.png")));
        }
        else
        {
            ui->lb_foto->setPixmap(QPixmap::fromImage(temp_img));
            ui->lb_foto->setScaledContents(true);
        }

        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        ui->statusBar->showMessage("Bienvenido "+ui->le_username->text(),2000);

        ui->pb_login->setEnabled(true);

        QFile data_base_information_local(informacion_local);
        if(data_base_information_local.open(QIODevice::WriteOnly)){
            QDataStream out(&data_base_information_local);
            lastEmpresaLogin = ui->l_empresa->text().trimmed();
            out<<lastEmpresaLogin;
            data_base_information_local.close();
        }
        QTimer::singleShot(100, this, SLOT(openTableTareas()));

    }
    else if(tipo == database_comunication::DOWNLOAD_USER_IMAGE)
    {
        QString foto = QString::fromUtf8(reply);
        QImage temp_img = database_comunication::getImageFromString(foto);
        if(temp_img.isNull())
        {
            ui->lb_foto->setScaledContents(false);
            ui->lb_foto->setPixmap((QPixmap(":/icons/User_Big.png")));
        }
        else
        {
            ui->lb_foto->setPixmap(QPixmap::fromImage(temp_img));
            ui->lb_foto->setScaledContents(true);
        }

        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        ui->statusBar->showMessage("Bienvenido "+ui->le_username->text(),2000);

        ui->pb_login->setEnabled(true);
        if(!myTable){
            myTable = new Tabla(nullptr, empresa);
            myTable->showMaximized();
            myTable->getTareas();


            conectarSignalsDeTabla();
        }
    }
    else if(tipo == database_comunication::GET_TAREAS_INFORMADAS)
    {
        qDebug()<<respuesta;
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        reply.remove(0,2);
        reply.chop(2);

        if(reply.contains("ot success get_tareas_informadas"))
        {
            result = database_comunication::script_result::get_tareas_failed;
        }
        else
        {
            jsonArrayInformadas = database_comunication::getJsonArray(reply);
            //            insertTareasInSQLite(jsonArray);
            result = database_comunication::script_result::ok;
        }
    }
    emit script_excecution_result(result);

}

void MainWindow::openTableTareas(){
    descargarTablas();

    myTable = new Tabla(nullptr, empresa);
    myTable->showMaximized();
    myTable->getTareas();

    conectarSignalsDeTabla();

    hide_loading();
}

void MainWindow::descargarTablas(){

    empresa = Empresa::getCurrentFieldEmpresa(ui->l_empresa->text().trimmed());

    tablaGestoresScreen = new Screen_Table_Gestores(nullptr, false, empresa);

    tablaMarcasScreen = new Screen_Table_Marcas(nullptr, false);
    tablaPiezasScreen = new Screen_Table_Piezas(nullptr, false);
    tablaCausasScreen = new Screen_Table_Causas_Intervenciones(nullptr, false);
    tablaEmplazamientosScreen = new Screen_Table_Emplazamientos(nullptr, false);
    tablaClasesScreen = new Screen_Table_Clases(nullptr, false);
    tablaTiposScreen = new Screen_Table_Tipos(nullptr, false);
    tablaCalibresScreen = new Screen_Table_Calibres(nullptr, false);
    tablaLongitudesScreen = new Screen_Table_Longitudes(nullptr, false);
    tablaRuedasScreen = new Screen_Table_Ruedas(nullptr, false);
    tablaRutasScreen = new Screen_Table_Rutas(nullptr, false);
    tablaResultadosScreen = new Screen_Table_Resultado_Intervenciones(nullptr, false);
    tablaObservacionesScreen = new Screen_Table_Observaciones(nullptr, false);

    tablaEquipo_OperariosScreen = new Screen_Table_Equipo_Operarios(nullptr, false, empresa);

    tablaZonasScreen = new Screen_Table_Zonas(nullptr, false, empresa);
    tablaOperariosScreen = new Screen_Table_Operarios(nullptr, false, empresa);
    tablaITACsScreen = new Screen_Table_ITACs(nullptr, false, empresa);
    tablaTareasScreen = new screen_tabla_tareas(nullptr, empresa);
}
void MainWindow::get_tareas_informadas_request(){
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_TAREAS_INFORMADAS,keys,values);
}
bool MainWindow::get_tareas_informadas(){

    QStringList keys, values;

    keys << "empresa";
    values << empresa.toLower();

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &MainWindow::script_excecution_result,q,&QEventLoop::exit);

    QTimer::singleShot(DELAY, this, &MainWindow::get_tareas_informadas_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        GlobalFunctions::showWarning(this,"Error de comunicación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;

    case database_comunication::script_result::ok:
        //        QMessageBox::information(this,"Éxito","Información actualizada correctamente servidor.");
        res = true;
        break;

    case database_comunication::script_result::get_tareas_failed:
        GlobalFunctions::showWarning(this,"Error de comun/*i*/cación con el servidor","No se pudo completar la solucitud por un error de comunicación con el servidor.");
        res = false;
        break;
    }
    delete q;

    return res;
}

void MainWindow::insertTareasInSQLite(QJsonArray jsonArray){ //Borra las tareas q no envias en el json si envias un json de informadas se borra el de tareas normales y viceversa
    QStringList principals_variables;
    DBtareasController db(empresa + db_tareas_path);

    if (db.isOpen())
    {
        //if not exist create the database tables.
        db.createTableTareas();

        for (int i=0; i < jsonArray.size(); i++) {
            QJsonObject jsonObject = jsonArray.at(i).toObject();
            QString principal_varJsonArray = jsonObject.value(principal_variable).toString().trimmed();
            principals_variables << principal_varJsonArray;
            if(db.checkIfTareaExists(principal_varJsonArray)){
                QJsonObject oldJson = db.getTareas(principal_variable, principal_varJsonArray).first().toObject();
                if(compareJsonObjectByDateModified(oldJson, jsonObject)){
                    db.updateTarea(jsonObject);
                }
            }else{
                db.insertTarea(jsonObject);
            }
        }
        //        int rows = db.getCountTableRows();
        QStringList princ_vars_in_sqlite = db.getOneColumnValues(principal_variable);
        QString pv;
        foreach(pv, princ_vars_in_sqlite){
            if(!principals_variables.contains(pv)){
                db.removeTarea(pv);
            }
        }
    }
    else
    {
        qDebug() << "Database could not be opened!";
    }
}

bool MainWindow::compareJsonObjectByDateModified(QJsonObject jsonObject_old, QJsonObject jsonObject_new){ //devuelve true si el segundo es mas actualizado
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
void MainWindow::conectarSignalsDeTabla(bool conexion)
{
    Q_UNUSED(conexion);

    connect(myTable, &Tabla::gestorSelected, tablaITACsScreen, &Screen_Table_ITACs::setGestor);
    connect(myTable, SIGNAL(abrirTablaOperarios()), this, SLOT(on_pb_tabla_operarios_clicked()));
    connect(myTable, SIGNAL(openMarcasTable()), this, SLOT(on_pb_tabla_marcas_clicked()));
    connect(myTable, SIGNAL(openPiezasTable()), this, SLOT(on_pb_tabla_piezas_clicked()));
    connect(myTable, SIGNAL(openZonasTable()), this, SLOT(on_pb_tabla_zonas_clicked()));
    connect(myTable, SIGNAL(openCausasTable()), this, SLOT(on_pb_tabla_causas_clicked()));
    connect(myTable, SIGNAL(openEmplazamientosTable()), this, SLOT(on_pb_tabla_emplazamientos_clicked()));
    connect(myTable, SIGNAL(openClasesTable()), this, SLOT(on_pb_tabla_clases_clicked()));
    connect(myTable, SIGNAL(openTiposTable()), this, SLOT(on_pb_tabla_tipos_clicked()));
    connect(myTable, SIGNAL(openCalibresTable()), this, SLOT(on_pb_tabla_calibres_clicked()));
    connect(myTable, SIGNAL(openInfosTable()), this, SLOT(on_pb_tabla_infos_clicked()));
    connect(myTable, &Tabla::openEquipo_OperariosTable, this, &MainWindow::on_pb_tabla_equipo_operarios_clicked);
    connect(myTable, &Tabla::updateITACsInfo, tablaITACsScreen, &Screen_Table_ITACs::updateItacsInTable);
    connect(myTable, &Tabla::openITACsTable, this, &MainWindow::on_pb_tabla_itacs_clicked);
    connect(myTable, &Tabla::openGestoresTable, this, &MainWindow::on_pb_tabla_gestores_clicked);
    connect(myTable, &Tabla::openLongitudesTable, this, &MainWindow::on_pb_tabla_longitudes_clicked);
    connect(myTable, &Tabla::openEmpresasTable, this, &MainWindow::on_pb_tabla_empresas_clicked);
    connect(myTable, &Tabla::openAdministradoresTable, this, &MainWindow::on_pb_tabla_administradores_clicked);
    connect(myTable, SIGNAL(openRuedasTable()), this, SLOT(on_pb_tabla_ruedas_clicked()));

    connect(myTable, SIGNAL(openRutasTable()), this, SLOT(on_pb_tabla_rutas_clicked()));

    connect(myTable, SIGNAL(openResultadosTable()), this, SLOT(on_pb_tabla_resultados_clicked()));
    connect(myTable, SIGNAL(openObservacionesTable()), this, SLOT(on_pb_tabla_observaciones_clicked()));
    connect(myTable, SIGNAL(abrirTablaContadores(bool)), this, SLOT(on_pb_tabla_contadores_clicked(bool)));
    connect(myTable, SIGNAL(importarTXT(QString)), this, SLOT(on_pb_cargar_txt_clicked(QString)));
    connect(myTable, SIGNAL(importarExcel(QString)), this, SLOT(on_pb_cargar_xls_clicked(QString)));
    connect(myTable, SIGNAL(importarDAT(QString)), this, SLOT(on_pb_cargar_dat_clicked(QString)));
    connect(myTable, SIGNAL(eraseJsonArrayInServer(QJsonArray)), this, SLOT(jsonArrayToEraseInServer(QJsonArray)));

    connect(tablaTareasScreen, &screen_tabla_tareas::updateOtherTable,
            myTable, &Tabla::updateTareasInTable);
    connect(tablaOperariosScreen, &Screen_Table_Operarios::updateTareas,
            myTable, &Tabla::updateTareasInTable);
    connect(tablaGestoresScreen, &Screen_Table_Gestores::updateTareas,
            myTable, &Tabla::updateTareasInTable);
    connect(tablaEquipo_OperariosScreen, &Screen_Table_Equipo_Operarios::updateTareas,
            myTable, &Tabla::updateTareasInTable);
    connect(tablaITACsScreen, &Screen_Table_ITACs::updateTableTareas,
            myTable, &Tabla::updateTareasInTable);
}

void MainWindow::updateTableInfoWithServer(database_comunication::serverRequestType type)
{
    Q_UNUSED(type);
    ui->statusBar->showMessage("Tareas cargadas", 3000);
    if(myTable){
        if(myTable->isHidden()){
            myTable->showMaximized();
        }
        //        insertTareasInSQLite(tablaTareasScreen->getAllTask());
        myTable->setJsonArrayAll(tablaTareasScreen->getAllTask());
    }
}

void MainWindow::jsonArrayToEraseInServer(QJsonArray jsonArray)
{
    QJsonObject o;
    QJsonDocument d;
    int result = -1;
    //    int total = jsonArray.size();
    int total= jsonArray.size();
    for(int i=0, reintentos = 0; i< jsonArray.size(); i++)
    {
        myTable->on_posicionBorrado(i, total);
        o = jsonArray[i].toObject();
        QEventLoop *q = new QEventLoop();

        if(tablaTareasScreen==nullptr){
            tablaTareasScreen= new screen_tabla_tareas();
        }
        connect(tablaTareasScreen, &screen_tabla_tareas::task_delete_excecution_result,q,&QEventLoop::exit);

        QTimer *timer;
        timer = new QTimer();
        //            timer->setInterval(DELAY);
        timer->setSingleShot(true);

        tablaTareasScreen->keys.clear();//este es para delete_tarea - estoy usando delete_tarea.php
        tablaTareasScreen->keys << numero_interno << numero_abonado << GESTOR << ANOMALIA
                                << numero_serie_contador_devuelto << "empresa";
        tablaTareasScreen->values.clear();
        tablaTareasScreen->values << o.value(numero_interno).toString().trimmed()
                                  << o.value(numero_abonado).toString().trimmed()
                                  << o.value(GESTOR).toString().trimmed()
                                  << o.value(ANOMALIA).toString().trimmed()
                                  << o.value(numero_serie_contador_devuelto).toString().trimmed()
                                  << empresa;

        connect(&tablaTareasScreen->database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
                tablaTareasScreen, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));

        connect(timer,SIGNAL(timeout()),tablaTareasScreen,SLOT(delete_one_task()));
        timer->start(DELAY);

        switch(q->exec())
        {
        case database_comunication::script_result::timeout:
            i--;
            reintentos++;
            if(reintentos == RETRIES)
            {
                result = database_comunication::script_result::timeout;
                i = jsonArray.size();
            }
            break;
        case database_comunication::script_result::ok:
            result = database_comunication::script_result::ok;
            reintentos = 0;
            break;
        case database_comunication::script_result::delete_task_failed:
            i--;
            reintentos++;
            if(reintentos == RETRIES)
            {
                result = database_comunication::script_result::delete_task_failed;
                i = jsonArray.size();
            }
            break;
        }
        delete timer;
        delete q;


    }
    myTable->resultado_Eliminacion_Tareas(result);
}

void MainWindow::on_pb_tabla_marcas_clicked()
{
    if(tablaMarcasScreen==nullptr){
        tablaMarcasScreen = new Screen_Table_Marcas();
    }else{
        tablaMarcasScreen->getMarcasFromServer();
    }
    tablaMarcasScreen->show();
}

void MainWindow::on_pb_tabla_piezas_clicked()
{
    if(tablaPiezasScreen==nullptr){
        tablaPiezasScreen = new Screen_Table_Piezas();
    }else{
        tablaPiezasScreen->getPiezasFromServer();
    }
    tablaPiezasScreen->show();
}

void MainWindow::on_pb_tabla_zonas_clicked()
{
    if(tablaZonasScreen==nullptr){
        tablaZonasScreen = new Screen_Table_Zonas(
                    );
    }else{
        tablaZonasScreen->getZonasFromServer();
    }
    tablaZonasScreen->show();
}

void MainWindow::on_pb_tabla_resultados_clicked()
{
    if(tablaResultadosScreen==nullptr){
        tablaResultadosScreen = new Screen_Table_Resultado_Intervenciones();
    }else{
        tablaResultadosScreen->getResultadosFromServer();
    }
    tablaResultadosScreen->show();
}

void MainWindow::on_pb_tabla_causas_clicked()
{
    if(tablaCausasScreen==nullptr){
        tablaCausasScreen = new Screen_Table_Causas_Intervenciones();
    }else{
        tablaCausasScreen->getCausasFromServer();
    }
    tablaCausasScreen->show();
}

void MainWindow::on_pb_tabla_emplazamientos_clicked()
{
    if(tablaEmplazamientosScreen==nullptr){
        tablaEmplazamientosScreen = new Screen_Table_Emplazamientos();
    }else{
        tablaEmplazamientosScreen->getEmplazamientosFromServer();
    }
    tablaEmplazamientosScreen->show();
}

void MainWindow::on_pb_tabla_empresas_clicked()
{
    if(tablaEmpresasScreen==nullptr){
        tablaEmpresasScreen = new Screen_Table_Empresas();
    }else{
        tablaEmpresasScreen->getEmpresasFromServer();
    }
    tablaEmpresasScreen->show();
}
void MainWindow::on_pb_tabla_administradores_clicked()
{
    if(tablaAdministradoresScreen==nullptr){
        tablaAdministradoresScreen = new Screen_Table_Administradores(
                    nullptr, true, empresa);
    }else {
        tablaAdministradoresScreen->getAdministradoresFromServer();
    }
    tablaAdministradoresScreen->show();
}
void MainWindow::on_pb_tabla_equipo_operarios_clicked(){
    if(tablaEquipo_OperariosScreen==nullptr){
        tablaEquipo_OperariosScreen = new Screen_Table_Equipo_Operarios(
                    nullptr, true, empresa);
    }else{
        tablaEquipo_OperariosScreen->getEquipo_OperariosFromServer();
    }
    tablaEquipo_OperariosScreen->show();
}
void MainWindow::on_pb_tabla_clases_clicked()
{
    if(tablaClasesScreen==nullptr){
        tablaClasesScreen = new Screen_Table_Clases();
    }else{
        tablaClasesScreen->getClasesFromServer();
    }
    tablaClasesScreen->show();
}

void MainWindow::on_pb_tabla_tipos_clicked()
{
    if(tablaTiposScreen==nullptr){
        tablaTiposScreen = new Screen_Table_Tipos();
    }else{
        tablaTiposScreen->getTiposFromServer();
    }
    tablaTiposScreen->show();
}

void MainWindow::on_pb_tabla_calibres_clicked()
{
    if(tablaCalibresScreen==nullptr){
        tablaCalibresScreen = new Screen_Table_Calibres();
    }else{
        tablaCalibresScreen->getCalibresFromServer();
    }
    tablaCalibresScreen->show();
}

void MainWindow::on_pb_tabla_infos_clicked()
{
    if(tablaInfosScreen==nullptr){
        tablaInfosScreen = new Screen_Table_Infos(
                    nullptr, true, empresa);
    }else{
        tablaInfosScreen->getInfosFromServer();
    }
    tablaInfosScreen->show();
}

void MainWindow::on_pb_tabla_longitudes_clicked()
{
    if(tablaLongitudesScreen==nullptr){
        tablaLongitudesScreen = new Screen_Table_Longitudes();
    }else{
        tablaLongitudesScreen->getLongitudesFromServer();
    }
    tablaLongitudesScreen->show();
}

void MainWindow::on_pb_tabla_itacs_clicked()
{
    if(tablaITACsScreen==nullptr){
        tablaITACsScreen = new Screen_Table_ITACs(
                    nullptr, true, empresa);
    }
    tablaITACsScreen->showMaximized();
}
void MainWindow::on_pb_tabla_gestores_clicked()
{
    if(tablaGestoresScreen==nullptr){
        tablaGestoresScreen = new Screen_Table_Gestores(
                    nullptr, true, empresa);
    }else{
        tablaGestoresScreen->getGestoresFromServer();
    }
    tablaGestoresScreen->show();
}
void MainWindow::on_pb_tabla_rutas_clicked(){
    if(tablaRutasScreen==nullptr){
        tablaRutasScreen = new Screen_Table_Rutas();
    }
    tablaRutasScreen->show();
}
void MainWindow::on_pb_tabla_ruedas_clicked()
{
    if(tablaRuedasScreen==nullptr){
        tablaRuedasScreen = new Screen_Table_Ruedas();
    }else{
        tablaRuedasScreen->getRuedasFromServer();
    }
    tablaRuedasScreen->show();
}

void MainWindow::on_pb_tabla_observaciones_clicked()
{
    if(tablaObservacionesScreen==nullptr){
        tablaObservacionesScreen = new Screen_Table_Observaciones();
    }else{
        tablaObservacionesScreen->getObservacionesFromServer();
    }
    tablaObservacionesScreen->show();
}

void MainWindow::on_pb_tabla_operarios_clicked()
{
    if(tablaOperariosScreen==nullptr){
        tablaOperariosScreen = new Screen_Table_Operarios(
                    nullptr, true, empresa);
    }else{
        tablaOperariosScreen->getOperariosFromServer();
    }
    tablaOperariosScreen->show();
}

void MainWindow::on_pb_lupa_clicked()
{

}

void MainWindow::on_pb_punta_flecha_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_pb_cruz_clicked()
{
    this->close();
    QApplication::closeAllWindows();
}

void MainWindow::on_pb_cargar_dat_clicked(QString order)
{
    if(order == "buscar_fichero_en_PC"){
        tablaTareasScreen->set_file_type(4);
        tablaTareasScreen->setRutaFile(order);
    }
    else if(order == "DIARIAS" || order == "MASIVAS" || order == "ESPECIALES"){
        selected_order = order;
        tablaTareasScreen->set_file_type(1);
        tablaTareasScreen->set_tipo_orden(order);
    }
    else{
        QString ruta = order;
        tablaTareasScreen->set_file_type(4);
        tablaTareasScreen->setRutaFile(ruta);
    }
    tablaTareasScreen->showMaximized();
}

void MainWindow::get_user_selected(QString u)
{
    selected_user = u;
}

void MainWindow::get_order_selected(QString o)
{
    selected_order = o;
}

QJsonArray MainWindow::loadCredentials(){
    QJsonArray jsonArray;
    QFile *data_base = new QFile(credentials); // ficheros .dat se puede utilizar formato txt tambien
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
void MainWindow::saveCredentials(QJsonObject credentials_info){
    QJsonArray jsonArray;
    jsonArray = loadCredentials();
    if(!jsonArray.contains((QJsonValue)credentials_info)){
        jsonArray.append(credentials_info);
        QFile file(credentials);

        if(file.open(QIODevice::WriteOnly)){
            QDataStream out(&file);
            out << jsonArray;
            file.close();
        }
    }


}
bool MainWindow::save_login(){
    QJsonObject login_jsonObject;

    login_jsonObject.insert(usuario_login, ui->le_username->text());
    login_jsonObject.insert(clave_login, ui->le_password->text());
    login_jsonObject.insert(empresa_login, ui->l_empresa->text());
    login_jsonObject.insert(app_login, "ESCRITORIO");
    login_jsonObject.insert(date_time_modified_login, QDateTime::currentDateTime().toString(formato_fecha_hora));

    QStringList keys, values;
    QJsonDocument d;
    d.setObject(login_jsonObject);
    QByteArray ba = d.toJson(QJsonDocument::Compact);
    QString temp_json_login_bytes = QString::fromUtf8(ba);

    keys << "json" << "server";
    values << temp_json_login_bytes << "";

    this->keys = keys;
    this->values = values;

    QEventLoop *q = new QEventLoop();

    connect(this, &MainWindow::script_excecution_result,q,&QEventLoop::exit);
    QTimer::singleShot(DELAY, this, &MainWindow::save_login_request);

    bool res = false;
    switch(q->exec())
    {
    case database_comunication::script_result::timeout:
        res = false;
        break;

    case database_comunication::script_result::ok:
        res = true;
        break;

    case database_comunication::script_result::save_login_failed:
        res = false;
        break;
    }
    delete q;

    return res;
}
void MainWindow::on_pb_login_clicked()
{
    ui->pb_login->setEnabled(false);
    ui->statusBar->showMessage("Comprobando información...");
    show_loading();

    if(myTable){
        if(!myTable->isHidden()){
            myTable->close();
            delete myTable;
        }
    }

    if(database_com.checkConnection()){
        other_task_screen::conexion_activa = true;
        QEventLoop q;

        connect(this, &MainWindow::script_excecution_result,&q,&QEventLoop::exit);
        QTimer::singleShot(DELAY, this, SLOT(login_request()));
        switch (q.exec())
        {
        case database_comunication::script_result::timeout:
            ui->statusBar->showMessage("Error de comunicación con el servidor",2000);
            ui->pb_login->setEnabled(true);
            hide_loading();
            break;

        case database_comunication::script_result::login_failed:
            ui->statusBar->showMessage("Nombre de usuario o contraseña incorrecto para esta empresa",2000);
            ui->pb_login->setEnabled(true);
        GlobalFunctions::showWarning(this,"Error de autenticación","Nombre de Usuario o contraseña incorrectos.");
            ui->lb_foto->setScaledContents(false);
            ui->lb_foto->setPixmap((QPixmap(":/icons/User_Big.png")));
            hide_loading();
            break;

        case database_comunication::script_result::ok:
            QJsonObject credential;
            credential.insert("administrador", ui->le_username->text());
            credential.insert("clave", ui->le_password->text());
            saveCredentials(credential);
            ui->statusBar->showMessage("Bienvenido "+ui->le_username->text());
            other_task_screen::administrator_loged = ui->le_username->text().trimmed();
//            save_login();
            download_administrator_image();
            break;
        }
    }else {
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this,"Error de comunicación","No hay acceso a la tabla en servidor, se cargará el último trabajo guardado");
        if(myTable == nullptr){
            other_task_screen::conexion_activa = false;
            myTable = new Tabla(nullptr, empresa);
            conectarSignalsDeTabla(false);
            ui->pb_login->setEnabled(true);
        }
    }
}

void MainWindow::download_administrator_image()
{
    QString empresa = Empresa::getCurrentFieldEmpresa(ui->l_empresa->text().trimmed());

    qDebug()<< "Descangardo imagen "+empresa+", " +ui->le_username->text();
    QStringList keys, values;
    keys << "empresa" << "administrador";
    values << empresa << ui->le_username->text();

    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),
            this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::
                               DOWNLOAD_ADMINISTRADOR_IMAGE,keys,values);
}

void MainWindow::download_user_image()
{
    QStringList keys, values;
    keys << "user_name";
    values << ui->le_username->text();

    connect(&database_com, SIGNAL(alredyAnswered(QByteArray,database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray,database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::DOWNLOAD_USER_IMAGE,keys,values);

}

void MainWindow::on_pb_cargar_xls_clicked()
{
    tablaTareasScreen->set_file_type(2);
    tablaTareasScreen->showMaximized();
}

void MainWindow::on_pb_cargar_txt_clicked(QString dir)
{
    if(dir.contains("itacs", Qt::CaseInsensitive)){
        Screen_Upload_Itacs *itacs = new Screen_Upload_Itacs(nullptr, empresa);
        itacs->getITACsFromFile(dir);
        itacs->show();
        return;
    }
    tablaTareasScreen->set_file_type(3);
    tablaTareasScreen->setRutaFile(dir);
    tablaTareasScreen->showMaximized();
}

void MainWindow::on_pb_cargar_xls_clicked(QString order)
{
    selected_order = order;
    tablaTareasScreen->set_file_type(2);
    tablaTareasScreen->set_tipo_orden(order);
    tablaTareasScreen->showMaximized();
}

void MainWindow::on_pb_web_browser_clicked()
{

    web_browser = new Navegador();
    web_browser->show();
}
void MainWindow::on_pb_tabla_contadores_clicked(bool showTable)
{
    if(tabla_contadores == nullptr){
        tabla_contadores = new Screen_tabla_contadores(
                    nullptr, empresa);
    }
    if(showTable){
        tabla_contadores->showMaximized();
    }
}

void MainWindow::on_pb_tabla_contadores_clicked()
{
    if(tabla_contadores == nullptr){
        tabla_contadores = new Screen_tabla_contadores(
                    nullptr, empresa);
    }
    tabla_contadores->showMaximized();
}
void MainWindow::show_loading(QWidget *parent, QPoint pos, QColor color, int w, int h, bool show_white_background){
    emit hidingLoading();

    int border = 1;
    pos.setX(pos.x()-w/2);
    pos.setY(pos.y()-h/2);
    if(show_white_background){
        QLabel *label_back = new QLabel(parent);
        connect(this, &MainWindow::hidingLoading, label_back, &QLabel::hide);
        connect(this, &MainWindow::hidingLoading, label_back, &QLabel::deleteLater);
        label_back->setFixedSize(w + border, h + border);
        label_back->move(pos);
        QString circle_radius_string = QString::number(static_cast<int>((w+border)/2));
        //    QString colorRBG = getColorString(color);
        label_back->setStyleSheet("background-color: #FFFFFF;"
                                  "border-radius: "+circle_radius_string+"px;");
        label_back->show();
    }

    QProgressIndicator *pi = new QProgressIndicator(parent);
    connect(this, &MainWindow::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &MainWindow::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color);
    pi->setFixedSize(w, h);
    pi->startAnimation();
    pos.setX(pos.x()+border/2 + 1);
    pos.setY(pos.y()+border/2 + 1);
    pi->move(pos);
    pi->show();
}
void MainWindow::show_loading(){
    emit hidingLoading();

    QProgressIndicator *pi = new QProgressIndicator(ui->widget_loading);
    connect(this, &MainWindow::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &MainWindow::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(ui->widget_loading->size());
    pi->startAnimation();
    pi->move(0,0);
    pi->show();
}

void MainWindow::hide_loading(){
    emit hidingLoading();
}
