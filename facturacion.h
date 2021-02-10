#ifndef FACTURACION_H
#define FACTURACION_H

#include <QDialog>

namespace Ui {
class Facturacion;
}

class Facturacion : public QDialog
{
    Q_OBJECT

public:
    explicit Facturacion(QWidget *parent = nullptr);
    ~Facturacion();
    static QStringList getFilesList(const QString &filter = "xlsx");
    static QStringList getFilesListIdExp(const QString &filter  = "xlsx");
    static QString getFecha(const QString &filename);

signals:
    void selectedFacturation(QStringList);

private slots:
    void on_cb_desde_currentIndexChanged(const QString &arg1);

    void on_cb_hasta_currentIndexChanged(const QString &arg1);

    void on_buttonBox_accepted();

private:
    Ui::Facturacion *ui;

};

#endif // FACTURACION_H
