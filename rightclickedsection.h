#ifndef RIGHTCLICKEDSECTION_H
#define RIGHTCLICKEDSECTION_H

#include <QWidget>

namespace Ui {
class RightClickedSection;
}

class RightClickedSection : public QWidget
{
    Q_OBJECT

public:
    explicit RightClickedSection(QWidget *parent = nullptr, QPoint pos = QPoint());
    ~RightClickedSection();

    enum {FILTRAR,ORDENAR_ASCENDENTE/*normal*/,ORDENAR_DESCENDENTE /*inverso*/};

signals:
    void clickPressed(int);

private slots:
    void on_pb_filtrar_clicked();

    void on_pb_sort_ascending_clicked();

    void on_pb_sort_descending_clicked();

private:
    Ui::RightClickedSection *ui;
};

#endif // RIGHTCLICKEDSECTION_H
