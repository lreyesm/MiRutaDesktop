#ifndef VIDEOPLAYBACK_H
#define VIDEOPLAYBACK_H

#include <QMainWindow>
//#include <QMediaPlayer>
//#include <QVideoWidget>

namespace Ui {
class VideoPlayback;
}

class VideoPlayback : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoPlayback(QWidget *parent = nullptr, QString title = "Video", QString file_name = "videos/video.mp4");
    ~VideoPlayback();
protected slots:
    void closeEvent(QCloseEvent *event)
    {
//        mplayer->stop();
//        mplayer->deleteLater();
//        vid_widget->deleteLater();

        QWidget::closeEvent(event);
    }
    void mouseDoubleClickEvent(QMouseEvent *event){
        if(this->isMaximized()){
            this->showNormal();
        }else{
            this->showMaximized();
        }
        QWidget::mouseDoubleClickEvent(event);
    }
    void mousePressEvent(QMouseEvent *event){
//        if(mplayer!=NULL){
//            if(playing){
//                mplayer->pause();
//                playing =false;
//            }else{
//                mplayer->play();
//                playing =true;
//            }
//        }
        QWidget::mousePressEvent(event);
    }

private:
    Ui::VideoPlayback *ui;
    bool playing = true;
//    QMediaPlayer* mplayer = NULL;
//    QVideoWidget* vid_widget = NULL;
};

#endif // VIDEOPLAYBACK_H
