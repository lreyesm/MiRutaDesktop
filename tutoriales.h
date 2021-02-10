#ifndef TUTORIALES_H
#define TUTORIALES_H

#include <QDialog>

namespace Ui {
class Tutoriales;
}

class Tutoriales : public QDialog
{
    Q_OBJECT

public:
    explicit Tutoriales(QWidget *parent = nullptr, QString type = "None");
    ~Tutoriales();

private slots:
    void on_pb_salvar_trabajo_en_servidor_clicked();

    void on_pb_cargar_desde_servidor_pc_clicked();

    void on_pb_respaldor_en_txt_clicked();

    void on_pb_cargar_respaldo_txt_clicked();

    void on_pb_cargar_trabajo_desde_android_clicked();

    void on_pb_salvar_trabajo_desde_android_clicked();

private:
    Ui::Tutoriales *ui;
};

#endif // TUTORIALES_H
