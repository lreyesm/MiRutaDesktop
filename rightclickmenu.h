#ifndef RIGHTCLICKMENU_H
#define RIGHTCLICKMENU_H

#include <QWidget>

namespace Ui {
class RightClickMenu;
}

class RightClickMenu : public QWidget
{
    Q_OBJECT


public:
    explicit RightClickMenu(QWidget *parent = nullptr,
                            QPoint pos = QPoint(), int from_tareas = FROM_TAREAS);
    ~RightClickMenu();

    void setPositionMenu(QPoint pos){this->move(pos);}

    enum {FROM_TAREAS, FROM_ITACS, FROM_CONTADORES};

signals:
    void clickPressed(int);

private slots:
    void on_pb_asignar_campos_comunes_clicked();

    void on_pb_asignar_a_un_operario_clicked();

    void on_pb_eliminar_tarea_clicked();

    void on_pb_abrir_tarea_clicked();

    void on_pb_descargar_fotos_clicked();

    void on_pb_mostrar_en_mapa_clicked();

    void on_pb_asignar_a_un_equipo_clicked();

    void on_pb_resumen_clicked();

private:
    Ui::RightClickMenu *ui;
};

#endif // RIGHTCLICKMENU_H
