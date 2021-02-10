#ifndef ZOOMPICTURE_H
#define ZOOMPICTURE_H

#include <QMainWindow>
#include <QPrinter>

#include <QAction>
#include <QLabel>
#include <QMenu>
#include <QScrollArea>
#include <QWheelEvent>

namespace Ui {
class ZoomPicture;
}

class ZoomPicture : public QMainWindow
{
    Q_OBJECT

public:
    explicit ZoomPicture(QWidget *parent = nullptr, QPixmap p = QPixmap());
    ~ZoomPicture();
    void setTextInfo(QString info);

    void setJsonInfo(QJsonObject jsonObject);
signals:
    void rotate(QPixmap);
public slots:

protected:
    void wheelEvent(QWheelEvent *event);
private slots:
    void print();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();

private slots:
    void zoom_image(int y);
    void getInicialSize();
    void on_pb_aumentar_clicked();

    void on_pb_disminuir_clicked();

    void on_pb_rotate_left_clicked();

    void on_pb_rotate_right_clicked();

private:
    Ui::ZoomPicture *ui;
    QPixmap pix;
    QSize inicial_size;

    void createActions();
        void createMenus();
        void updateActions();
        void adjustScrollBar(QScrollBar *scrollBar, double factor);

    #ifndef QT_NO_PRINTER
        QPrinter printer;
    #endif

        QAction *printAct;
        QAction *exitAct;
        QAction *zoomInAct;
        QAction *zoomOutAct;
        QAction *normalSizeAct;
//        QAction *fitToWindowAct;
        QAction *aboutAct;

        QMenu *fileMenu;
        QMenu *viewMenu;
        QMenu *helpMenu;
        bool checkIfFieldIsValid(QString var);
};

#endif // ZOOMPICTURE_H
