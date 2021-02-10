#include "zoompicture.h"
#include "ui_zoompicture.h"
#include <QScrollBar>
#include <QPrintDialog>
#include <QPainter>
#include <QMessageBox>
#include <QTimer>
#include <QDesktopWidget>
#include "global_variables.h"
#include "barcodegenerator.h"
#include "new_table_structure.h"
#include "globalfunctions.h"

ZoomPicture::ZoomPicture(QWidget *parent, QPixmap p) :
    QMainWindow(parent),
    ui(new Ui::ZoomPicture)
{
    ui->setupUi(this);
    this->setWindowTitle("Mi Ruta");
    this->showMaximized();

    pix = p;

    int h = QApplication::desktop()->height()-100;
    //    int w = ui->scrollArea->width();

    float photo_ratio = static_cast<float>(pix.height())/pix.width();

    ui->label->setMaximumSize(static_cast<int>(( static_cast<float>(h)/photo_ratio)) ,h);

    ui->label->setPixmap(pix);
    ui->label->setScaledContents(true);

    createActions();
    createMenus();

    ui->widget_serie->hide();
    ui->widget_serie_DV->hide();

    QTimer::singleShot(50, this, SLOT(getInicialSize()));

    this->setAttribute(Qt::WA_DeleteOnClose);
}

void ZoomPicture::getInicialSize(){
    inicial_size =  ui->label->size();
}

ZoomPicture::~ZoomPicture()
{
    delete ui;
}

void ZoomPicture::setTextInfo(QString info){
    ui->l_info->setText(info); 
}
void ZoomPicture::setJsonInfo(QJsonObject jsonObject){
    if(!jsonObject.isEmpty()){
        if(jsonObject.keys().contains(numero_serie_contador)){
            QString numSerie = jsonObject.value(numero_serie_contador).toString();
            if(checkIfFieldIsValid(numSerie)){
                BarcodeGenerator::printBarcode(numSerie, ui->l_barcode_num_serie, 70);
                ui->l_num_serie->setText(numSerie);
                ui->widget_serie->show();
            }else{
                ui->widget_serie->hide();
            }
        }
        if(jsonObject.keys().contains(numero_serie_contador_devuelto)){
            QString numSerieDV = jsonObject.value(numero_serie_contador_devuelto).toString();
            if(checkIfFieldIsValid(numSerieDV)){
                BarcodeGenerator::printBarcode(numSerieDV, ui->l_barcode_num_serieDV, 70);
                ui->l_num_serieDV->setText(numSerieDV);
                ui->widget_serie_DV->show();
            }else{
                ui->widget_serie_DV->hide();
            }
        }
    }
}
bool ZoomPicture::checkIfFieldIsValid(QString var){//devuelve true si es valido
    if(!var.trimmed().isEmpty() && !var.isNull() && var!="null" && var!="NULL"){
        return true;
    }
    else{
        return false;
    }
}
void ZoomPicture::wheelEvent(QWheelEvent *event)
{
//    zoom_image(event->angleDelta().y());
    event->ignore();
}

void ZoomPicture::zoom_image(int y){

    double ratio;
    if(y < 0){
        ratio = 0.9;
        ui->scrollArea->verticalScrollBar()->move(0,0);
    }else{
        ratio = 1.1;
        ui->scrollArea->verticalScrollBar()->move(0,0);
    }
    QSize image_size = ui->label->size();
    image_size.setWidth(static_cast<int>(image_size.width() * ratio));
    image_size.setHeight(static_cast<int>(image_size.height() * ratio));
    ui->label->setFixedSize(image_size);
    update();
}

void ZoomPicture::print()
{
    Q_ASSERT(ui->label->pixmap());
#ifndef QT_NO_PRINTER
    QPrintDialog dialog(&printer, this);
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = ui->label->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(ui->label->pixmap()->rect());
        painter.drawPixmap(0, 0, *ui->label->pixmap());
    }
#endif
}

void ZoomPicture::zoomIn()
{
    zoom_image(1);
}

void ZoomPicture::zoomOut()
{
    zoom_image(-1);
}

void ZoomPicture::normalSize()
{
    ui->label->setFixedSize(inicial_size);
    ui->label->setScaledContents(true);
}

void ZoomPicture::fitToWindow()
{
//    bool fitToWindow = fitToWindowAct->isChecked();
//    ui->scrollArea->setWidgetResizable(fitToWindow);
//    if (!fitToWindow) {
//        normalSize();
//    }
}

void ZoomPicture::about()
{
    GlobalFunctions::showMessage(this,"Mi Ruta " + versionMiRuta,"Desarrollado Por\n\nMichel Morales Veranes\nCorreo: mraguascontadores@gmail.com"
                                              "\n\nLuis Alejandro Reyes Morales\nCorreo: inglreyesm@gmail.com   ");
}

void ZoomPicture::createActions()
{

    printAct = new QAction(tr("&Imprimir..."), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setEnabled(true);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    exitAct = new QAction(tr("&Salir"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("&Aumentar (25%)"), this);
    zoomInAct->setShortcut(tr("+"));
    zoomInAct->setEnabled(true);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("-"));
    zoomOutAct->setEnabled(true);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Tamaño Normal"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(true);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));


    aboutAct = new QAction(tr("&Sobre MiRuta"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void ZoomPicture::createMenus()
{
    fileMenu = new QMenu(tr("&Archivo"), this);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&Ver"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
//    viewMenu->addSeparator();
//    viewMenu->addAction(fitToWindowAct);

    helpMenu = new QMenu(tr("&Ayuda"), this);
    helpMenu->addAction(aboutAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}


void ZoomPicture::on_pb_aumentar_clicked()
{
    zoomIn();
}

void ZoomPicture::on_pb_disminuir_clicked()
{
    zoomOut();
}

void ZoomPicture::on_pb_rotate_left_clicked()
{
    QSize size = pix.size();
    QImage srcImg(pix.toImage());
    QPoint center = srcImg.rect().center();
    QMatrix matrix;
    matrix.translate(center.x(), center.y());
    matrix.rotate(-90);
    QImage dstImg = srcImg.transformed(matrix);
    QPixmap dstPix = QPixmap::fromImage(dstImg);
    pix = dstPix;
    ui->label->setMaximumSize(size.height(), size.width());
    ui->label->setPixmap(pix);

    emit rotate(pix);
}

void ZoomPicture::on_pb_rotate_right_clicked()
{
    QSize size = pix.size();
    QImage srcImg(pix.toImage());
    QPoint center = srcImg.rect().center();
    QMatrix matrix;
    matrix.translate(center.x(), center.y());
    matrix.rotate(90);
    QImage dstImg = srcImg.transformed(matrix);
    QPixmap dstPix = QPixmap::fromImage(dstImg);
    pix = dstPix;
    ui->label->setMaximumSize(size.height(), size.width());
    ui->label->setPixmap(pix);

    emit rotate(pix);
}
