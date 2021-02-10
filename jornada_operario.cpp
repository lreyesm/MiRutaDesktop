#include "jornada_operario.h"
#include "ui_jornada_operario.h"
#include "operario.h"
#include "global_variables.h"

Jornada_Operario::Jornada_Operario(QWidget *parent, QJsonObject json) :
    QMainWindow(parent),
    ui(new Ui::Jornada_Operario)
{
    ui->setupUi(this);
    fillJsonDate();
    operario_json = json;
    ui->l_operario->setText(operario_json.value(operario_operarios).toString());
    QString nombre = operario_json.value(nombre_operarios).toString()+ " "+
            operario_json.value(apellidos_operarios).toString();
    if(Operario::checkIfFieldIsValid(nombre)){
        ui->l_nombre->setText(nombre);
    }
    QJsonDocument d = QJsonDocument::
            fromJson(operario_json.value(fechas_inicio_operarios).toString().toUtf8());
    if(d.isObject()){
        jornada_json = d.object();
    }
    QDateTime dt = QDateTime::currentDateTime();
    setCurrentMonth(dt.date().month());
    ui->l_year->setText(QString::number(dt.date().year()));
    fixModelForTable(jornada_json, dt.date().month(), dt.date().year());
    setTableView();
}

Jornada_Operario::~Jornada_Operario()
{
    delete ui;
}

void Jornada_Operario::resizeEvent(QResizeEvent *event){

    if(timer.isActive()){
        timer.stop();
    }
    timer.setInterval(200);
    timer.start();
    connect(&timer,SIGNAL(timeout()),this,SLOT(setTableView()));

    QWidget::resizeEvent(event);
}

QString Jornada_Operario::getHourInSeconds(qint64 seconds){
    int hours = (int)seconds/3600;
    int reminder = (int)seconds - hours*3600;
    int mins = reminder/60;
    reminder = reminder - mins * 60;
    int secs = reminder;

    QString time = QString::number(hours)+"h "+
            QString::number(mins) + "m " +
            QString::number(secs) +"seg";

    return time;
}
void Jornada_Operario::fixModelForTable(QJsonObject jsonObject, int month, int year)
{
    //    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QStringList keys = jsonObject.keys();
    QDate date(year, month, 1);
    QStringList split = date.toString(formato_fecha_sin_hora).split("_");
    split.removeLast();
    QString month_year = split.join("_");

    QMap <QString,QString> mapa;

    for(int i = 0; i < keys.size(); i++){
        if(!(keys.at(i).contains("fecha_comenzar") && keys.at(i).contains(month_year))){
            keys.removeAt(i);
            i--;
        }
    }
    keys.sort();


    mapa.insert("Día del Mes",month_year);
    mapa.insert("Hora Inicio","fecha_comenzar_");
    mapa.insert("Hora Pausada","fecha_pausar_");
    mapa.insert("Hora Reanudada","fecha_reanudar_");
    mapa.insert("Hora Finalizada","fecha_finalizar_");
    mapa.insert("Horas Totales","?");


    QStringList listHeaders;
    listHeaders <<"Día del Mes" << "Hora Inicio"
               << "Hora Pausada" << "Hora Reanudada"
               << "Hora Finalizada" << "Horas Totales";

    model = new QStandardItemModel(31, listHeaders.size());
    model->setHorizontalHeaderLabels(listHeaders);

    int c = 0; //el index del cada row añadirlo al setData en vez de i
    //insertando los datos
    qint64 inicio =0;
    qint64 pausa =0;
    qint64 reanudar =0;
    qint64 final =0;
    qint64 horas_totales_mes = 0;
    QString column_info;
    for(int i = 1; i <= 31; i++)
    {
        date.setDate(year, month, i);
        if(!keys.contains("fecha_comenzar_"+date.toString(formato_fecha_sin_hora))){
            continue;
        }
        for (int n = 0; n < listHeaders.size(); n++) {

            if(listHeaders.at(n) == "Día del Mes"){
                column_info = QString::number(i);
            }else if(listHeaders.at(n) == "Horas Totales"){
                int descanso = 0, trabajo = 0;
                if(reanudar > 0 && pausa > 0){
                    descanso = reanudar - pausa;
                }
                if(final > 0 && inicio > 0){
                    trabajo = final - inicio;
                }
                qint64 total_segundos = (((trabajo) - (descanso))/* segundos*/);
                if(total_segundos >= 0){
                    horas_totales_mes+=total_segundos;
                    column_info = getHourInSeconds(total_segundos);
                }
            }else{
                QString key = mapa.value(listHeaders.at(n))+
                        date.toString(formato_fecha_sin_hora);
                if(jsonObject.contains(key)){
                    column_info = jsonObject.value(key).toString();
                    QDateTime dt = QDateTime::fromString(column_info, formato_fecha_hora);
                    if(dt.isValid()){
                        dt.toSecsSinceEpoch();
                        column_info = dt.toString("HH:mm");
                    }else{
                        column_info = "-";
                    }
                    if(listHeaders.at(n) == "Hora Inicio"){
                        if(dt.isValid()){
                            inicio = dt.toSecsSinceEpoch();
                        }
                    }else if(listHeaders.at(n) == "Hora Pausada"){
                        if(dt.isValid()){
                            pausa = dt.toSecsSinceEpoch();
                        }
                    }else if(listHeaders.at(n) == "Hora Reanudada"){
                        if(dt.isValid()){
                            reanudar = dt.toSecsSinceEpoch();
                        }
                    }else if(listHeaders.at(n) == "Hora Finalizada"){
                        if(dt.isValid()){
                            final = dt.toSecsSinceEpoch();
                        }
                    }
                }
            }
            //            item->setData(column_info,Qt::EditRole);
            QModelIndex index = model->index(c, n, QModelIndex());
            model->setData(index, column_info);
        }
        c++;

    }
    model->setRowCount(c);
    ui->l_horas_totales_mes->setText(getHourInSeconds(horas_totales_mes));
}

void Jornada_Operario::setTableView()
{
    if(model!=nullptr){
        ui->tableView->setModel(model);

        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableView->verticalHeader()->setVisible(false);

        ui->tableView->horizontalHeader()->setStretchLastSection(true);
        ui->tableView->horizontalHeader()->setSectionsMovable(true);

        int fields_count_in_table = ui->tableView->horizontalHeader()->count();
        int width_table = ui->tableView->size().width() - 20;
        float medium_width_fileds = (float)width_table/fields_count_in_table;

        for (int i=0; i< fields_count_in_table; i++) {
            ui->tableView->setColumnWidth(i, (int)(medium_width_fileds));
        }

        if(!connected_header_signal){
            connected_header_signal = true;
            connect(ui->tableView->horizontalHeader(), SIGNAL(sectionClicked(int)),
                    this, SLOT(on_sectionClicked(int)));
        }
    }
}

void Jornada_Operario::on_tableView_doubleClicked(const QModelIndex &index)
{

}
void Jornada_Operario::setCurrentMonth(int month){
    QString m = mapMonth.key(month);
    ui->l_month->setText(m);
}
void Jornada_Operario::on_l_up_month_clicked()
{
    QString m = ui->l_month->text();
    int m_int = mapMonth.value(m);
    m_int ++;
    if(m_int > 12){
        m_int = 1;
        on_l_up_year_clicked();
    }
    setCurrentMonth(m_int);
    int y = ui->l_year->text().toInt();
    fixModelForTable(jornada_json, m_int, y);
    setTableView();
}
void Jornada_Operario::on_l_down_month_clicked()
{
    QString m = ui->l_month->text();
    int m_int = mapMonth.value(m);
    m_int --;
    if(m_int < 1){
        m_int = 12;
        on_l_down_year_clicked();
    }
    setCurrentMonth(m_int);
    int y = ui->l_year->text().toInt();
    fixModelForTable(jornada_json, m_int, y);
    setTableView();
}

void Jornada_Operario::on_l_up_year_clicked()
{
    int y = ui->l_year->text().toInt();
    y++;
    ui->l_year->setText(QString::number(y));
    QString m = ui->l_month->text();
    int m_int = mapMonth.value(m);
    fixModelForTable(jornada_json, m_int, y);
    setTableView();
}
void Jornada_Operario::on_l_down_year_clicked()
{
    int y = ui->l_year->text().toInt();
    y--;
    ui->l_year->setText(QString::number(y));
    QString m = ui->l_month->text();
    int m_int = mapMonth.value(m);
    fixModelForTable(jornada_json, m_int, y);
    setTableView();
}
void Jornada_Operario::fillJsonDate(){
    mapMonth.insert("ENERO", 1);
    mapMonth.insert("FEBRERO" , 2);
    mapMonth.insert("MARZO", 3);
    mapMonth.insert("ABRIL", 4);
    mapMonth.insert("MAYO", 5);
    mapMonth.insert("JUNIO", 6);
    mapMonth.insert("JULIO", 7);
    mapMonth.insert("AGOSTO", 8);
    mapMonth.insert("SEPTIEMBRE", 9);
    mapMonth.insert("OCTUBRE", 10);
    mapMonth.insert("NOVIEMBRE", 11);
    mapMonth.insert("DICIEMBRE", 12);
}
