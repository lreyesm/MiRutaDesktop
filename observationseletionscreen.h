#ifndef OBSERVATIONSELETIONSCREEN_H
#define OBSERVATIONSELETIONSCREEN_H

#include <QDialog>

namespace Ui {
class ObservationSeletionScreen;
}

class ObservationSeletionScreen : public QDialog
{
    Q_OBJECT

public:
    explicit ObservationSeletionScreen(QWidget *parent = nullptr);
    ~ObservationSeletionScreen();

    void moveCenter();
signals:
    void selected_observation(QString);

public slots:


private slots:
    void on_pb_close_clicked();
    void on_pb_aceptar_clicked();
    void on_pb_cancelar_clicked();

private:
    Ui::ObservationSeletionScreen *ui;
};

#endif // OBSERVATIONSELETIONSCREEN_H
