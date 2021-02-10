#ifndef MYLABELSPINNER_H
#define MYLABELSPINNER_H

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QPushButton>
#include "clickablelabel.h"

#define ITEMHEIGHT 33
#define ANIM_TIME 300


class MyLabelSpinner : public ClickableLabel
{
    Q_OBJECT
public:
    explicit MyLabelSpinner(QWidget *parent = nullptr);

    QString getSpinnerItem(int pos);
    QStringList getSpinnerList();
    bool hasSpinnerItem(QString item);
    bool addSpinnerItem(QString item, bool show = false);
    void setSpinnerList(QStringList list, bool show = false);
    void addSpinnerList(QStringList list, bool show = false);
    void addItems(QStringList list, bool show = false);
    void appendItems(QStringList list, bool show = false);
    QString currentText();
    QStringList items();

    bool removeItem(QString item);
    void removeItems(QStringList items);
    void removeItems();
    void set_Enabled(bool enable);
signals:
    void itemSelected(QString);
    void currentTextChanged(QString);
    void currentIndexChanged(QString);
    void currentTextChangedGetPrevious(QString, QString);
    void width_Changed(int);
    void hiding();
    void showing();
public slots:
    bool addItem(QString item);
    void showSpinnerList();
    void hideSpinnerList();
protected slots:
    void resizeEvent(QResizeEvent *event);
    void moveEvent(QMoveEvent *event);

    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
private slots:
    void clearSpinnerWidget();
    void fillSpinnerWidget();
    void emitTextSelected(QString item);
    void setSpinnerBackground(QString styleSheet = "");
    void finalizadaAnimacion();
    void showHideSpinner();
private:
    QRect previousGeometry;
    QStringList spinnerList;
    QWidget *widgetSpinnerList = nullptr;
    QLabel *backgroundLabel = nullptr;
    QTimer timerChangeThreshold;
    QString textToEmit = "";
    ClickableLabel *button_expand = nullptr;
//    QList< ClickableLabel *> buttons;

    void setButtons();
    void animateUp(QWidget *widget);
    void animateDown(QWidget *widget);


    bool shown = false;
    QString getScrollBarStyle();
};

#endif // MYLABELSPINNER_H
