#include "equipo_selection_screen.h"
#include "ui_equipo_selection_screen.h"

#include <QDesktopWidget>
#include <QGraphicsDropShadowEffect>


Equipo_Selection_Screen::Equipo_Selection_Screen(QWidget *parent, QString empresa) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::Equipo_Selection_Screen),
    database_com()
{
    setWindowFlags(Qt::CustomizeWindowHint);
    ui->setupUi(this);

    this->empresa = empresa;

    //    connect(this,SIGNAL(mouse_pressed()),this,SLOT(on_drag_screen()));
    //    connect(this,SIGNAL(mouse_Release()),this,SLOT(on_drag_screen_released()));
    //    connect(&start_moving_screen,SIGNAL(timeout()),this,SLOT(on_start_moving_screen_timeout()));

}

Equipo_Selection_Screen::~Equipo_Selection_Screen()
{
    delete ui;
}

void Equipo_Selection_Screen::moveCenter()
{
    QRect rect = parentWidget()->geometry();
    this->move(rect.width()/2 - this->width()/2,
               rect.height()/2 - this->height()/2 - 20);

}

void Equipo_Selection_Screen::getEquipo_OperariosFromServer()
{
    QStringList keys, values;
    keys << "empresa";
    values << empresa.toLower();
    connect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
    connect(this,SIGNAL(equiposFilled(database_comunication::serverRequestType)),this,SLOT(populateComboBox(database_comunication::serverRequestType)));
    database_com.serverRequest(database_comunication::serverRequestType::GET_EQUIPO_OPERARIOS,keys,values);
}

void Equipo_Selection_Screen::serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo)
{
    if(tipo == database_comunication::GET_EQUIPO_OPERARIOS)
    {
        disconnect(&database_com, SIGNAL(alredyAnswered(QByteArray, database_comunication::serverRequestType)),
                   this, SLOT(serverAnswer(QByteArray, database_comunication::serverRequestType)));
        ba.remove(0,2);
        ba.chop(2);

        jsonArray = database_comunication::getJsonArray(ba);
        emit equiposFilled(tipo);
    }
}

void Equipo_Selection_Screen::populateComboBox(database_comunication::serverRequestType tipo)
{
    Q_UNUSED(tipo);
    connect(this,&Equipo_Selection_Screen::equiposFilled
            ,this,&Equipo_Selection_Screen::populateComboBox);
    int rows = jsonArray.count();
    ui->comboBox->clear();
    QStringList equipos;
    for(int i = 0; i < rows; i++)
    {
        equipos << (jsonArray[i].toObject().value(equipo_operario_equipo_operarios).toString());
    }
    ui->comboBox->addItems(equipos);
}

void Equipo_Selection_Screen::on_buttonBox_accepted()
{
    QString equipo_selected = ui->comboBox->currentText();
    emit equipoSelected(equipo_selected);
}

void Equipo_Selection_Screen::on_pb_close_clicked()
{
    this->close();
}

void Equipo_Selection_Screen::on_drag_screen()
{
    if(isFullScreen()){
        if(QApplication::mouseButtons()==Qt::RightButton){


        }
        return;
    }
    //ui->statusBar->showMessage("Moviendo");
    if(QApplication::mouseButtons()==Qt::LeftButton){

        start_moving_screen.start(10);
        init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
        init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
    }
    else if(QApplication::mouseButtons()==Qt::RightButton){

    }
}

void Equipo_Selection_Screen::on_drag_screen_released()
{
    if(isFullScreen()){

        return;
    }
    start_moving_screen.stop();
    init_pos_x = 0;
    init_pos_y = 0;
    //current_win_Pos = QPoint(this->pos().x()-200,this->pos().y()-200);
}

void Equipo_Selection_Screen::on_start_moving_screen_timeout()
{
    int x_pos = (int)this->pos().x()+((QWidget::mapFromGlobal(QCursor::pos())).x() - init_pos_x);
    int y_pos = (int)this->pos().y()+((QWidget::mapFromGlobal(QCursor::pos())).y() - init_pos_y);
    x_pos = (x_pos < 0)?0:x_pos;
    y_pos = (y_pos < 0)?0:y_pos;

    x_pos = (x_pos > QApplication::desktop()->width()-100)?QApplication::desktop()->width()-100:x_pos;
    y_pos = (y_pos > QApplication::desktop()->height()-180)?QApplication::desktop()->height()-180:y_pos;

    this->move(x_pos,y_pos);

    init_pos_x = (QWidget::mapFromGlobal(QCursor::pos())).x();
    init_pos_y = (QWidget::mapFromGlobal(QCursor::pos())).y();
}

void Equipo_Selection_Screen::on_pb_aceptar_clicked()
{
    QString equipo_selected = ui->comboBox->currentText();
    emit equipoSelected(equipo_selected);
    emit accept();
    this->close();
}

void Equipo_Selection_Screen::on_pb_cancelar_clicked()
{
    emit reject();
    this->close();
}
