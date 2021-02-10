#include "calendardialog.h"
#include "ui_calendardialog.h"
#include "operator_selection_screen.h"
#include "equipo_selection_screen.h"
#include <QGraphicsDropShadowEffect>

CalendarDialog::CalendarDialog(QWidget *parent, bool show_horas, QString empresa, bool show_init_time) :
    QDialog(parent),
    ui(new Ui::CalendarDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Seleccione Fecha y Hora");
    this->empresa = empresa;
    if(!show_init_time){
        ui->widget_hora_inicio->hide();
    }
    if(!show_horas){
        this->setWindowTitle("Seleccione Fecha");
        ui->widget_hora_cita->hide();
        ui->widget_asign->hide();
        this->setFixedHeight(this->height() - ui->widget_hora_cita->height()
                             - ui->widget_asign->height());
    }
    dt = new DateTimeWidget(ui->widget_date);
    dt->showCenter();

    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(2);
    ui->pb_asign_operators->setGraphicsEffect(effect);
    QGraphicsDropShadowEffect* eff = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
    eff->setBlurRadius(20);
    eff->setOffset(2);
    ui->pb_asign_team->setGraphicsEffect(eff);

    this->setAttribute(Qt::WA_DeleteOnClose);
}

CalendarDialog::~CalendarDialog()
{
    delete ui;
}

void CalendarDialog::get_user_selected(const QString &user)
{
    if(!user.isEmpty()){
        operario_selected=user;
    }
}
void CalendarDialog::get_team_selected(const QString &team)
{
    if(!team.isEmpty()){
        team_selected=team;
    }
}

void CalendarDialog::on_te_hora_inicio_userTimeChanged(const QTime &time)
{
    if(ui->te_hora_fin->time() < time){
        ui->te_hora_fin->setTime(time);
    }
}
void CalendarDialog::on_pb_asign_team_clicked()
{
    Equipo_Selection_Screen *seleccionEquipoScreen = new Equipo_Selection_Screen(this, empresa);
    seleccionEquipoScreen->move(0, 0);
    seleccionEquipoScreen->getEquipo_OperariosFromServer();
    connect(seleccionEquipoScreen,&Equipo_Selection_Screen::equipoSelected,this,&CalendarDialog::get_team_selected);

    if(seleccionEquipoScreen->exec())
    {
        ui->l_team->setText(team_selected);
    }
}
void CalendarDialog::on_pb_asign_operators_clicked()
{
    Operator_Selection_Screen *seleccionOperarioScreen = new Operator_Selection_Screen(this, empresa);
    seleccionOperarioScreen->move(0, 0);
    seleccionOperarioScreen->getOperariosFromServer();
    connect(seleccionOperarioScreen,&Operator_Selection_Screen::user,this,&CalendarDialog::get_user_selected);

    if(seleccionOperarioScreen->exec())
    {
        ui->l_operators->setText(operario_selected);
    }
}

void CalendarDialog::on_pb_aceptar_clicked()
{
    QDateTime date = dt->getFechaHora();
    emit date_selected(date.date());
    if(!ui->widget_hora_cita->isHidden()){
        QTime time = ui->te_hora_inicio->time();
        time = ui->te_hora_fin->time();
        emit time_selected_init(ui->te_hora_inicio->time());
        emit time_selected_end(ui->te_hora_fin->time());
        emit operator_selected(operario_selected);
        emit equipo_selected(team_selected);
    }
    emit accept();
}

void CalendarDialog::on_pb_cancelar_clicked()
{
    emit reject();
    this->close();
}
