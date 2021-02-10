#ifndef NAVEGADOR_H
#define NAVEGADOR_H

#include <QWidget>

namespace Ui {
class Navegador;
}

class Navegador : public QWidget
{
    Q_OBJECT

public:
    explicit Navegador(QWidget *parent = 0);
    ~Navegador();

private slots:
    void on_pb_go_to_googleMaps_clicked();

    void on_pb_go_clicked();

private:
    Ui::Navegador *ui;

};

#endif // NAVEGADOR_H
