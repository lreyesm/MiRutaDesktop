#ifndef SELECCION_GESTOR_H
#define SELECCION_GESTOR_H

#include <QDialog>

namespace Ui {
class Seleccion_Gestor;
}

class Seleccion_Gestor : public QDialog
{
    Q_OBJECT

public:
    explicit Seleccion_Gestor(QWidget *parent = nullptr, QStringList list = QStringList(), bool todos = false);
    ~Seleccion_Gestor();
    void setGestors(QStringList list);

public slots:
    void moveCenter();
signals:
    void selected_gestor(QString);

private slots:
    void on_pb_close_clicked();
    void on_pb_aceptar_clicked();
    void on_pb_cancelar_clicked();

private:
    Ui::Seleccion_Gestor *ui;
};

#endif // SELECCION_GESTOR_H
