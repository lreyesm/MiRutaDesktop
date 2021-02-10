#ifndef IDORDENASSIGN_H
#define IDORDENASSIGN_H

#include <QWidget>

namespace Ui {
class IDOrdenAssign;
}

class IDOrdenAssign : public QWidget
{
    Q_OBJECT

public:
    explicit IDOrdenAssign(QWidget *parent = nullptr);
    ~IDOrdenAssign();

    void moveCenter();
signals:
    void id_selected(QString);
    void accepted();
    void rejected();
private slots:
    void on_pb_cancelar_clicked();
    void on_pb_aceptar_clicked();
    void on_pb_close_clicked();
private:
    Ui::IDOrdenAssign *ui;
};

#endif // IDORDENASSIGN_H
