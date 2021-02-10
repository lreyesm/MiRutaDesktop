#include "tutoriales.h"
#include "ui_tutoriales.h"
#include "processesclass.h"

Tutoriales::Tutoriales(QWidget *parent, QString type) :
    QDialog(parent),
    ui(new Ui::Tutoriales)
{
    ui->setupUi(this);
    this->setWindowTitle("Tutoriales");
}

Tutoriales::~Tutoriales()
{
    delete ui;
}

void Tutoriales::on_pb_salvar_trabajo_en_servidor_clicked()
{
    QString file_name = "subir_trabajo_a_servidor.mp4";
    QString pr = QDir::currentPath()+"/videos/"+file_name;
    ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 500, true);
}

void Tutoriales::on_pb_cargar_desde_servidor_pc_clicked()
{
    QString file_name = "cargar_trabajo_de_respaldo_servidor.mp4";
    QString pr = QDir::currentPath()+"/videos/"+file_name;
    ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 500, true);
}

void Tutoriales::on_pb_respaldor_en_txt_clicked()
{
    QString file_name = "respaldar_trabajo.mp4";
    QString pr = QDir::currentPath()+"/videos/"+file_name;
    ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 500, true);
}

void Tutoriales::on_pb_cargar_respaldo_txt_clicked()
{
    QString file_name = "cargar_trabajo_de_respaldo_txt.mp4";
    QString pr = QDir::currentPath()+"/videos/"+file_name;
    ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 500, true);
}

void Tutoriales::on_pb_cargar_trabajo_desde_android_clicked()
{
    QString file_name = "cargar_del_servidor_android.mp4";
    QString pr = QDir::currentPath()+"/videos/"+file_name;
    ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 500, true);
}

void Tutoriales::on_pb_salvar_trabajo_desde_android_clicked()
{
    QString file_name = "salvar_en_servidor_desde_android.mp4";
    QString pr = QDir::currentPath()+"/videos/"+file_name;
    ProcessesClass::executeProcess(this, pr, ProcessesClass::WINDOWS, 500, true);
}
