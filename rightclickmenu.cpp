#include "rightclickmenu.h"
#include "ui_rightclickmenu.h"

#include "global_variables.h"
#include "other_task_screen.h"
#include <QScreen>
#include "globalfunctions.h"

RightClickMenu::RightClickMenu(QWidget *parent, QPoint pos, int from) :
    QWidget(parent),
    ui(new Ui::RightClickMenu)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint);

    QRect rect = QGuiApplication::screens().first()->geometry();

    if(from == FROM_ITACS){
        ui->pb_resumen->hide();
        this->setFixedHeight(this->height()-ui->pb_resumen->height());
    }
    if(from == FROM_CONTADORES){
        ui->pb_mostrar_en_mapa->hide();
        ui->pb_resumen->hide();
        ui->pb_descargar_fotos->hide();
        this->setFixedHeight(this->height()-ui->pb_mostrar_en_mapa->height()*3);
    }

    if((this->width() + pos.x()) >= rect.width()){
        pos.setX(pos.x() - this->width());
    }
    if((this->height() + pos.y()) >= rect.height()){
        pos.setY(pos.y() - this->height());
    }
    this->move(pos);
}

RightClickMenu::~RightClickMenu()
{
    delete ui;
}

void RightClickMenu::on_pb_asignar_campos_comunes_clicked()
{
    this->close();
    emit clickPressed(ASIGNAR_COMUNES);
}

void RightClickMenu::on_pb_asignar_a_un_operario_clicked()
{
    if(!other_task_screen::conexion_activa){
        GlobalFunctions::showWarning(this, "Deshabilitado", "No se puede asignar sin conexión a tabla");
        this->close();
        return;
    }
    this->close();
    emit clickPressed(ASIGNAR_A_OPERARIO);
}

void RightClickMenu::on_pb_eliminar_tarea_clicked()
{
    if(!other_task_screen::conexion_activa){
        GlobalFunctions::showWarning(this, "Deshabilitado", "No se puede eliminar sin conexión a tabla");
        this->close();
        return;
    }
    this->close();
    emit clickPressed(ELIMINAR);

}

void RightClickMenu::on_pb_abrir_tarea_clicked()
{
    this->close();
    emit clickPressed(ABRIR);
}

void RightClickMenu::on_pb_descargar_fotos_clicked()
{
    if(!other_task_screen::conexion_activa){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this, "Deshabilitado", "No se puede descargar fotos sin conexión a servidor");
        this->close();
        return;
    }
    this->close();
    emit clickPressed(DESCARGAR_FOTOS);
}

void RightClickMenu::on_pb_mostrar_en_mapa_clicked()
{
    this->close();
    emit clickPressed(MOSTRAR_EN_MAPA);
}

void RightClickMenu::on_pb_asignar_a_un_equipo_clicked()
{
    if(!other_task_screen::conexion_activa){
        GlobalFunctions gf(this);
        GlobalFunctions::showWarning(this, "Deshabilitado", "No se puede asignar sin conexión a tabla");
        this->close();
        return;
    }
    this->close();
    emit clickPressed(ASIGNAR_A_EQUIPO);
}

void RightClickMenu::on_pb_resumen_clicked()
{
    this->close();
    emit clickPressed(RESUMEN_TAREAS);
}
