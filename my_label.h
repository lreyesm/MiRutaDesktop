#ifndef MY_LABEL_H
#define MY_LABEL_H
#include <QLabel>
#include <QModelIndex>
#include <QJsonObject>

class my_label : public QLabel
{
    Q_OBJECT
public:
    my_label(QWidget*);

    void setEditable(bool);

    void setModelIndex(QModelIndex i);
    void setTextInfo(QString information);
    void setJsonInfo(QJsonObject jsonObject);
    bool isDefaultPhoto(){ return defaultPhoto;}
    void setDefaultPhoto(bool defaultActive);

signals:
    void doubleClickedLabel(QModelIndex);
    void changedFoto();
    
public slots:
    void setPixmap(const QPixmap &);
protected:
    void mousePressEvent(QMouseEvent *ev);

private slots:
    void photoRotated(QPixmap pix);
    
private:
    bool editable = false;
    QModelIndex index;
    void openPhotoToLoad();
    QString info ="";
    QJsonObject jsonObject;
    bool defaultPhoto = true;
    
};

#endif // MY_LABEL_H
