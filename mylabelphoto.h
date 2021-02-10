#ifndef MYLABELPHOTO_H
#define MYLABELPHOTO_H

#include <QLabel>

class MyLabelPhoto : public QLabel
{
    Q_OBJECT

public:
    explicit MyLabelPhoto(QWidget *parent = nullptr);
    bool changedPhoto = false;
    bool isDefaultPhoto(){ return defaultPhoto;}
    void setDefaultPhoto(bool defaultActive);

public slots:
    void photoRotated(QPixmap pix);
    void setPixmap(const QPixmap &p);
protected slots:
    void mousePressEvent(QMouseEvent *ev);

private:
    bool editable = false;
    void openPhotoToLoad();
    bool defaultPhoto = true;

signals:
    void selectedPhoto(QPixmap);
    void selectedPhotoName(QString);
    void changedFoto();

};

#endif // MYLABELPHOTO_H
