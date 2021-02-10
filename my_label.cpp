#include "my_label.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QDir>
#include <zoompicture.h>
#include <zoompicture.h>
#include <other_task_screen.h>
#include <QGraphicsDropShadowEffect>
#include "global_variables.h"

my_label::my_label(QWidget *parent)
    :QLabel(parent)
{
    editable = false;
}

void my_label::setEditable(bool e)
{
    editable = e;
}

void my_label::setModelIndex(QModelIndex i){
    index = i;
}

void my_label::setTextInfo(QString information){
    info = information;
}

void my_label::setJsonInfo(QJsonObject jsonObject)
{
    this->jsonObject = jsonObject;
}

void my_label::setDefaultPhoto(bool defaultActive){
    defaultPhoto = defaultActive;
    if(defaultPhoto){
        QPixmap default_foto = QPixmap(":/icons/add_photo.png");
        QLabel::setPixmap(default_foto);
    }
}

void my_label::setPixmap(const QPixmap &p)
{
    QLabel::setPixmap(p);
    QPixmap default_foto = QPixmap(":/icons/add_photo.png");
    if(p.toImage() != default_foto.toImage()){
        setDefaultPhoto(false);
        QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect(this);//dar sombra a borde del widget
        effect->setBlurRadius(20);
        effect->setColor(color_blue_app);
        this->setGraphicsEffect(effect);
    }else{
        defaultPhoto = true;
    }
}

void my_label::openPhotoToLoad(){
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
    emit changedFoto();
    this->setMinimumWidth(0);
    this->setPixmap(QPixmap::fromImage(QImage(ruta)));
    this->setScaledContents(true);
}

void my_label::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    if(editable)
    {
        openPhotoToLoad();
    }
    else
    {
        if(this->pixmap() != nullptr)
        {
            QPixmap foto = *(this->pixmap());
            QPixmap default_foto = QPixmap(":/icons/add_photo.png");
            if(foto.toImage() == default_foto.toImage()){
                openPhotoToLoad();
            }else{
                ZoomPicture *zoom = new ZoomPicture(nullptr, foto);
                zoom->setTextInfo(info);
                zoom->setJsonInfo(jsonObject);
                connect(zoom, &ZoomPicture::rotate, this, &my_label::photoRotated);
                zoom->showMaximized();
            }
        }else{

        }
    }
}
void my_label::photoRotated(QPixmap pix){
    this->setPixmap(pix);
    emit changedFoto();
}
