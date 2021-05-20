#ifndef DATABASEOPTIONS_H
#define DATABASEOPTIONS_H

#include <QWidget>

namespace Ui {
class DatabaseOptions;
}

class DatabaseOptions : public QWidget
{
    Q_OBJECT

public:
    explicit DatabaseOptions(QWidget *parent = nullptr, int cantActualConfigurada = 500);
    ~DatabaseOptions();

signals:
    void sendTareasPorPagina(int);

private slots:
    void on_pb_close_clicked();

    void on_pb_aceptar_clicked();

    void on_pb_cancelar_clicked();

    void on_le_tareas_por_pagina_textChanged(const QString &arg1);

private:
    Ui::DatabaseOptions *ui;
};

#endif // DATABASEOPTIONS_H
