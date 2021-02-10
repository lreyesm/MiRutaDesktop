#ifndef SCREEN_TABLE_INFO_H
#define SCREEN_TABLE_INFO_H

#include <QMainWindow>

namespace Ui {
class Screen_Table_Info;
}

class Screen_Table_Info : public QMainWindow
{
    Q_OBJECT

public:
    explicit Screen_Table_Info(QWidget *parent = nullptr);
    ~Screen_Table_Info();

private:
    Ui::Screen_Table_Info *ui;
};

#endif // SCREEN_TABLE_INFO_H
