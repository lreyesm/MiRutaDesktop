#include "mylabelphoto.h"
#include <QtCore>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include "global_variables.h"

MyLabelPhoto::MyLabelPhoto(QWidget *parent) : QLabel(parent)
{

}

void MyLabelPhoto::setDefaultPhoto(bool defaultActive){
    defaultPhoto = defaultActive;
    if(defaultPhoto){
        QPixmap default_foto = QPixmap(":/icons/add_photo.png");
        QLabel::setPixmap(default_foto);
    }
}
void MyLabelPhoto::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    openPhotoToLoad();
}


void MyLabelPhoto::openPhotoToLoad(){
    const QString DEFAULT_DIR_KEY("C:/Mi_Ruta/Empresas"); //cambiar esto

    QSettings MySettings; // Will be using application informations
    // for correct location of your settings
    QString direccion =  MySettings.value(DEFAULT_DIR_KEY).toString();
    if(direccion.isEmpty()){
        direccion = DEFAULT_DIR_KEY;
    }
    QString ruta = QFileDialog::getOpenFileName(this,"Seleccione la foto", direccion/*QDir::current().path()+"/Logos de empresas"*/, "Imagenes (*.png *.xpm *.jpg)");
    if(ruta.isEmpty() || ruta.isNull()){
        return;
    }
    QDir CurrentDir;
    MySettings.setValue(DEFAULT_DIR_KEY,
                        CurrentDir.absoluteFilePath(ruta));
    QPixmap pixmap = QPixmap::fromImage(QImage(ruta));
    emit selectedPhotoName(this->objectName());
    emit selectedPhoto(pixmap);
    this->setScaledContents(true);
    this->setPixmap(pixmap);
    this->changedPhoto = true;
}

void MyLabelPhoto::photoRotated(QPixmap pix){
    this->setPixmap(pix);
    this->changedPhoto = true;
    emit changedFoto();
    emit selectedPhotoName(this->objectName());
    emit selectedPhoto(pix);
}
void MyLabelPhoto::setPixmap(const QPixmap &p)
{
    QLabel::setPixmap(p);
    QPixmap default_foto = QPixmap(":/icons/add_photo.png");
    if(p.toImage() != default_foto.toImage()){
        setDefaultPhoto(false);
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
        effect->setBlurRadius(20);
        effect->setColor(color_blue_app);
        this->setGraphicsEffect(effect);
    }
}
