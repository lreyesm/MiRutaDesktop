#include "videoplayback.h"
#include "ui_videoplayback.h"

#include <QFile>
#include <QMessageBox>
#include <QDir>
#include "processesclass.h"

VideoPlayback::VideoPlayback(QWidget *parent, QString title, QString file_name) :
    QMainWindow(parent),
    ui(new Ui::VideoPlayback)
{
    ui->setupUi(this);
    this->setWindowTitle(title);

    ProcessesClass::executeProcess(parent, QDir::currentPath()+"/videos/"+file_name, ProcessesClass::WINDOWS, 500, true);
    this->close();

//Para mediaplayer hasta ahora no me fucniona desde que formatee en el Trabajo
//    mplayer = new QMediaPlayer;
//    vid_widget = new QVideoWidget(this);
//    vid_widget->setAspectRatioMode(Qt::KeepAspectRatio);

//    this->setCentralWidget(vid_widget);

//    mplayer->setVideoOutput(vid_widget);
//    mplayer->setMedia(QUrl::fromLocalFile(QDir::currentPath()+"/videos/"+file_name));

//    mplayer->setVolume(0);
//    mplayer->setPlaybackRate(1);
//    mplayer->play();

//    ui->statusbar->showMessage("Reproduciendo");

//    this->showMaximized();
}

VideoPlayback::~VideoPlayback()
{
    delete ui;
}
