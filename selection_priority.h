#ifndef SELECTION_PRIORITY_H
#define SELECTION_PRIORITY_H

#include <QDialog>

namespace Ui {
class Selection_Priority;
}

class Selection_Priority : public QDialog
{
    Q_OBJECT

public:
    explicit Selection_Priority(QWidget *parent = nullptr);
    ~Selection_Priority();

signals:
    void priorityselected(QString);
private slots:
    void on_pb_aceptar_clicked();

    void on_pb_cancelar_clicked();

    void on_pb_close_clicked();
private:
    Ui::Selection_Priority *ui;
};

#endif // SELECTION_PRIORITY_H
