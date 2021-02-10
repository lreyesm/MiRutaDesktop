#ifndef MYLINEEDITAUTOCOMPLETE_H
#define MYLINEEDITAUTOCOMPLETE_H

#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QPushButton>
#include "clickablelabel.h"

#define CHANGETHRESHOLD 1000
#define ITEMHEIGHT 40

class MyLineEditAutoComplete : public QLineEdit
{
    Q_OBJECT
public:
    explicit MyLineEditAutoComplete(QWidget *parent = nullptr);
    void setAutoCompleteBackground(QString styleSheet = ""); 
    void stopChangeThresholdAndHide();

signals:
    void itemSelected(QString);
    void textModified(QString);

protected slots:
    void moveEvent(QMoveEvent *event);

    void resizeEvent(QResizeEvent *e);
public slots:
    void addAutoCompleteList(QStringList list, bool show = true);
    void setAutoCompleteList(QStringList list, bool show = true);
    bool addAutoCompleteItem(QString item, bool show = true);

    bool hasAutoComplteItem(QString item);

    QStringList getAutoCompleteList();
    QString getAutoCompleteItem(int pos);

    void showAutoCompleteList();
    void hideAutoCompleteList();

    void text_Edited(QString text);
    void emitTextEdited();

    void setButtons();

private slots:
    void fillAutoCompleteWidget();
    void clearAutoCompleteWidget();

    void emitTextSelected(QString item);

private:
    QStringList autoCompleteList;
    QWidget *widgetAutoComplete = nullptr;
    QLabel *backgroundLabel = nullptr;
    QTimer timerChangeThreshold;
    QString textToEmit = "";
    ClickableLabel *button_clear = nullptr;
};

#endif // MYLINEEDITAUTOCOMPLETE_H
