#include "datesschedule.h"
#include "ui_datesschedule.h"
#include "new_table_structure.h"
#include "global_variables.h"
#include "globalfunctions.h"
#include <QJsonObject>
#include <QTimer>
#include "QProgressIndicator.h"
#include "calendardialog.h"
#include "mylabelshine.h"

DatesSchedule::DatesSchedule(QWidget *parent, QString empresa) :
    QWidget(parent),
    ui(new Ui::DatesSchedule)
{
    ui->setupUi(this);

    this->empresa = empresa;
    on_pb_update_clicked();
}

DatesSchedule::~DatesSchedule()
{
    delete ui;
}
void DatesSchedule::selectDate()
{
    CalendarDialog *calendarDialog = new CalendarDialog(nullptr, false);
    connect(calendarDialog,SIGNAL(date_selected(QDate)),this,SLOT(get_date_selected(QDate)));
    calendarDialog->show();
}

void DatesSchedule::get_date_selected(QDate d)
{
    dateSelected = d;
    on_pb_update_clicked();
}

QJsonArray DatesSchedule::requestData(){
    QDate date = dateSelected;
    this->setWindowTitle(
                "Citas del " + date.toString(formato_fecha_hora_new_view_sin_hora));
    QString dateString = date.toString(formato_fecha);
    QString query = " ( " + fecha_hora_cita + " LIKE '" + dateString + "%' ) ORDER BY " + fecha_hora_cita;

    QStringList fields;
    fields << fecha_hora_cita << nuevo_citas << numero_abonado << nombre_cliente << telefono1 << telefono2;
    GlobalFunctions gf(this, empresa);
    QJsonArray jsonArray = gf.getTareasFields(fields, query);
    return jsonArray;
}

void DatesSchedule::fixModelForTable(QJsonArray jsonArray)
{
    int rows = jsonArray.count();
    //comprobando que no exista un modelo anterior
    if(model!=nullptr)
        delete model;

    QMap <QString,QString> mapa;
    mapa.insert("Cita", fecha_hora_cita);
    mapa.insert("Teléfono 1", telefono1);
    mapa.insert("Teléfono 2", telefono2);
    mapa.insert("Abonado", numero_abonado);
    mapa.insert("Nombre", nombre_cliente);
    mapa.insert("Nuevo citas", nuevo_citas);

    QStringList listHeaders;
    listHeaders <<"Cita" << "Teléfono 1" << "Teléfono 2" << "Abonado" << "Nombre" << "Nuevo citas";

    model = new QStandardItemModel(rows, listHeaders.size());
    model->setHorizontalHeaderLabels(listHeaders);

    //insertando los datos
    QString column_info;
    for(int i = 0; i < rows; i++)
    {
        for (int n = 0; n < listHeaders.size(); n++) {
            QString header = listHeaders.at(n);
            column_info = jsonArray[i].toObject().value(mapa.value(header)).toString();
            if(header == "Cita"){
                QStringList split = column_info.split(" ");
                if(split.size() > 1){
                    column_info = split.at(1);
                }
            }
            //            item->setData(column_info,Qt::EditRole);
            QModelIndex index = model->index(i, n, QModelIndex());
            model->setData(index, column_info);
        }
    }
}

void DatesSchedule::setTableView()
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
        float medium_width_fields = (float)width_table/fields_count_in_table;

        for (int i=0; i< fields_count_in_table; i++) {
            ui->tableView->setColumnWidth(i, (int)(medium_width_fields));
        }
    }
    hide_loading();
}

void DatesSchedule::on_pb_update_clicked()
{
    show_loading("Buscando citas");
    QJsonArray jsonArray = requestData();

    fixModelForTable(jsonArray);
    QTimer::singleShot(300, this, &DatesSchedule::setTableView);
}

void DatesSchedule::show_loading(QString mess){
    emit hidingLoading();

    QWidget *widget_blur = new QWidget(this);
    QSize size = this->size();
    size += QSize(0,30);
    widget_blur->move(0,0);
    widget_blur->setFixedSize(size);
    widget_blur->setStyleSheet("background-color: rgba(100, 100, 100, 100);");
    widget_blur->show();
    widget_blur->raise();
    connect(this, &DatesSchedule::hidingLoading, widget_blur, &QWidget::hide);
    connect(this, &DatesSchedule::hidingLoading, widget_blur, &QWidget::deleteLater);

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
    connect(this, &DatesSchedule::hidingLoading, label_loading_text, &MyLabelShine::hide);
    connect(this, &DatesSchedule::hidingLoading, label_loading_text, &MyLabelShine::deleteLater);
    connect(this, &DatesSchedule::setLoadingTextSignal, label_loading_text, &MyLabelShine::setText);

    QProgressIndicator *pi = new QProgressIndicator(widget_blur);
    connect(this, &DatesSchedule::hidingLoading, pi, &QProgressIndicator::stopAnimation);
    connect(this, &DatesSchedule::hidingLoading, pi, &QProgressIndicator::deleteLater);
    pi->setColor(color_blue_app);
    pi->setFixedSize(50, 50);
    pi->startAnimation();
    pi->move(rect.width()/2
             - pi->size().width()/2,
             rect.height()/2);
    pi->raise();
    pi->show();
}

void DatesSchedule::setLoadingText(QString mess){
    emit setLoadingTextSignal(mess);
}
void DatesSchedule::hide_loading(){
    emit hidingLoading();
}

void DatesSchedule::on_pb_select_day_clicked()
{
    selectDate();
}
