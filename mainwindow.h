#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardItemModel>
#include "database_comunication.h"
#include "screen_tabla_tareas.h"
#include "screen_table_operarios.h"
#include <operator_selection_screen.h>
#include <counter.h>
#include <QTimer>
#include <QDesktopWidget>
#include "screen_tabla_contadores.h"
#include <QAxWidget>
#include "tabla.h"
#include "screen_table_marcas.h"
#include "screen_table_piezas.h"
#include "screen_table_zonas.h"
#include "screen_table_causas_intervenciones.h"
#include "screen_table_resultado_intervenciones.h"
#include "screen_table_emplazamientos.h"
#include "screen_table_clases.h"
#include "screen_table_tipos.h"
#include "screen_table_observaciones.h"
#include "screen_table_calibres.h"
#include "screen_table_longitudes.h"
#include "screen_table_ruedas.h"
#include "screen_table_rutas.h"
#include "screen_table_infos.h"
#include "screen_table_gestores.h"
#include "screen_table_empresas.h"
#include "screen_table_administradores.h"
#include "screen_table_itacs.h"
#include "screen_table_equipo_operarios.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //        static int lastIDSAT;

    static void readVariablesInDB(QDataStream &in);

    void insertTareasInSQLite(QJsonArray jsonArray);
    bool compareJsonObjectByDateModified(QJsonObject jsonObject_old, QJsonObject jsonObject_new);
public slots:

    void login_request();

    void on_pb_cargar_xls_clicked(QString order);
    void on_pb_tabla_marcas_clicked();
    void on_pb_tabla_piezas_clicked();
    void on_pb_tabla_zonas_clicked();
    void on_pb_tabla_causas_clicked();
    void on_pb_tabla_gestores_clicked();
    void on_pb_tabla_resultados_clicked();
    void on_pb_tabla_observaciones_clicked();
    void on_pb_tabla_tipos_clicked();
    void on_pb_tabla_clases_clicked();
    void on_pb_tabla_equipo_operarios_clicked();
    void on_pb_tabla_emplazamientos_clicked();
    void on_pb_tabla_calibres_clicked();
    void on_pb_tabla_longitudes_clicked();
    void on_pb_tabla_ruedas_clicked();
    void on_pb_tabla_administradores_clicked();
    void on_pb_tabla_empresas_clicked();
    void on_pb_tabla_operarios_clicked();
    void on_pb_tabla_contadores_clicked();
    void on_pb_tabla_contadores_clicked(bool showTable);
    void on_pb_tabla_itacs_clicked();

    void on_pb_tabla_rutas_clicked();



signals:
    void sendData(QJsonObject);
    void download_user_image_signal();
    void download_administrator_image_signal();
    void script_excecution_result(int);
    void upload_finished();
    void mouse_pressed();
    void mouse_Release();
    void hidingLoading();

protected slots:
    void closeEvent(QCloseEvent *event);
private slots:
    void hide_loading();
    void show_loading();
    void serverAnswer(QByteArray, database_comunication::serverRequestType);

    void on_pb_lupa_clicked();
    void on_pb_punta_flecha_clicked();
    void on_pb_cruz_clicked();
    void on_pb_cargar_dat_clicked(QString order);

    void get_user_selected(QString);
    void on_pb_login_clicked();
    void download_user_image();
    void conection_timeout()
    {
        Script_excecution_result = database_comunication::script_result::timeout;
    }
    void on_drag_screen();
    void on_drag_screen_released();
    void on_start_moving_screen_timeout();

    void mousePressEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
    {Q_UNUSED(e);
        emit mouse_pressed();
    }

    void mouseReleaseEvent(QMouseEvent *e) ///al reimplementar esta funcion deja de funcionar el evento pressed
    {Q_UNUSED(e);
        emit mouse_Release();
    }
    void on_pb_cargar_xls_clicked();
    void on_pb_web_browser_clicked();

    void get_order_selected(QString o);

    void updateTableInfoWithServer(database_comunication::serverRequestType type);
    void jsonArrayToEraseInServer(QJsonArray jsonArray);

    void on_pb_cargar_txt_clicked(QString);

    void on_pb_tabla_infos_clicked();
    void getFilesFromServer();

    void empresasDescargadas(database_comunication::serverRequestType tipo);
    void download_administrator_image();
    void descargarTablas();

    void autocompleteCredentials(QString user);
    bool get_tareas_informadas();

    void get_tareas_informadas_request();
    void save_login_request();
    bool save_login();
    void openTableTareas();
    void enableLoginButton();
private:
    Ui::MainWindow *ui;

    QString lastEmpresaLogin;
    database_comunication database_com;

    Screen_tabla_contadores *tabla_contadores = nullptr;
    Screen_Table_Infos *tablaInfosScreen = nullptr;
    Screen_Table_Marcas *tablaMarcasScreen = nullptr;
    Screen_Table_Piezas *tablaPiezasScreen = nullptr;
    Screen_Table_Zonas *tablaZonasScreen = nullptr;
    Screen_Table_Emplazamientos *tablaEmplazamientosScreen = nullptr;
    Screen_Table_Clases *tablaClasesScreen = nullptr;
    Screen_Table_Equipo_Operarios *tablaEquipo_OperariosScreen = nullptr;
    Screen_Table_Administradores *tablaAdministradoresScreen = nullptr;
    Screen_Table_Empresas *tablaEmpresasScreen = nullptr;
    Screen_Table_Tipos *tablaTiposScreen = nullptr;
    Screen_Table_Calibres *tablaCalibresScreen = nullptr;
    Screen_Table_Longitudes *tablaLongitudesScreen = nullptr;
    Screen_Table_Gestores *tablaGestoresScreen = nullptr;
    Screen_Table_ITACs *tablaITACsScreen = nullptr;
    Screen_Table_Ruedas *tablaRuedasScreen = nullptr;
    Screen_Table_Rutas *tablaRutasScreen = nullptr;
    Screen_Table_Causas_Intervenciones *tablaCausasScreen = nullptr;
    Screen_Table_Resultado_Intervenciones *tablaResultadosScreen = nullptr;
    Screen_Table_Observaciones *tablaObservacionesScreen = nullptr;
    screen_tabla_tareas *tablaTareasScreen = nullptr;
    Screen_Table_Operarios *tablaOperariosScreen = nullptr;
    Tabla *myTable = nullptr;
    Operator_Selection_Screen *seleccionOperarioScreen = nullptr;

    int Script_excecution_result;

    QJsonArray jsonArray, jsonArrayDoneTasks;

    bool serverAlredyAnswered;
    QString selected_user;
    QString selected_order;

    QTimer start_moving_screen;
    int init_pos_x;
    int init_pos_y;
    bool first_move = true;
    Navegador *web_browser;

    QString empresa = "";
    QStringList keys, values;


    void conectarSignalsDeTabla(bool conexion = true);
    void iniciateFiles();
    void callUpdater();
    bool checkVersions(QString serverVersion);
    QJsonArray loadCredentials();
    void saveCredentials(QJsonObject credentials_info);

    QJsonArray jsonArrayInformadas;


    void show_loading(QWidget *parent, QPoint pos, QColor color = color_blue_app,
                      int w = 24, int h = 24, bool show_white_background = false);
};

#endif // MAINWINDOW_H
