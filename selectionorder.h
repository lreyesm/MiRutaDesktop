#ifndef SELECTIONORDER_H
#define SELECTIONORDER_H

#include <QDialog>

namespace Ui {
class SelectionOrder;
}

class SelectionOrder : public QWidget
{
    Q_OBJECT

public:
    explicit SelectionOrder(QWidget *parent = nullptr);
    ~SelectionOrder();

signals:
    void order_selected(QString);

private slots:

    void on_pb_cancelar_clicked();

    void on_pb_aceptar_clicked();

private:
    Ui::SelectionOrder *ui;
};

#endif // SELECTIONORDER_H
