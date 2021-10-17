#ifndef TABLA_H
#define TABLA_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QCompleter>
#include "database_comunication.h"
#include "other_task_screen.h"
#include "operator_selection_screen.h"
#include "screen_tabla_tareas.h"
#include <QMouseEvent>
#include "delegate.h"
#include <QAbstractItemView>
#include <QHeaderView>
#include <QtXlsx>
#include <QtXlsx/xlsxformat.h>

#include "mylineeditshine.h"

#define MAX_JSONARRAY_HISTORY_SIZE 5


namespace Ui {
class Tabla;
}

class Tabla : public QMainWindow
{
    Q_OBJECT

    enum{F_SERIE = 1, F_SERIE_DEVUELTO, F_SIN_REVISAR, F_RESULTADO, F_POR_OPERARIO, F_POR_EQUIPO, F_GESTOR, F_SECTION, F_DIRECCION,
         F_CAUSA_ORIGEN, F_GEOLOCALIZACION, F_TITULAR, F_N_ABONADO, F_ZONA, F_NO_EN_BATERIA,
         F_EN_BATERIA};

public:
    explicit Tabla(QWidget *parent = nullptr, QString empresa = "");
    ~Tabla();
    QString get_current_date_in_format(QDate date = QDate::currentDate(),
                                       QTime time_init = QTime::currentTime(),
                                       QTime time_end = QTime::currentTime());
    void resultado_Eliminacion_Tareas(int result);
    QJsonArray getJsonArrayAll(){
        return jsonArrayAll;
    }

    static bool checkIfFieldIsValid(QString var);
    int adjuntarFotosParaComprimir(QJsonArray jsonArray, QString dir, int iteration, QString &path);
    void copyAllFilesFromDirToDir(QString source, const QString destiny);
    void copyFileToNewDirAndName(QString file_source, QString destiny_dir, QString new_fileName);
    QString comprimirArchivos(QString string_dir, QString current_gestor);
    bool uploadFileToServer(QString filename);
    bool cargarInformacionNueva();
    bool buscarInformacionNueva();
    bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath);
    QString abrirSeleccionGestores(QJsonArray jsonArray, bool todos = false);

    QJsonArray filtroInicialDeGestores(QJsonArray jsonArray, bool abrir = true);
    bool filtro_inicial_seleccionado = false;
    static QJsonObject fillCausaData(QJsonObject jsonObjectTarea, QString anomaly);
    QString getViewOfTarea(QJsonObject jsonObject);
    static QString getDirection(QJsonObject jsonObject);
    static bool checkIfBatteryTask(QJsonObject jsonObject);
    static QString getValidField(QJsonObject jsonObject, QStringList fields);
    int dir_size(const QString _wantedDirPath);
    void setInformadas(QJsonArray jsonArray);
    bool getTareasFromServer(QString empresa, QString query = "NUMIN <> 'NULL'", int limit = LIMIT, int id_start = 0);
    bool getTareasAmountFromServer(QString empresa, QString query = "NUMIN <> 'NULL'", int limit = LIMIT);


public slots:
    void getTareas();
    void setTableView(bool dalegate = false);
    void setJsonArrayAll(QJsonArray);

    void updateTableWithServerInfo(int result =0);
    void informarTareas(QString order);
    void conection_timeout()
    {
        Script_excecution_result = database_comunication::script_result::timeout;
    }
    void salvarTrabajoEnServidor();
    void get_gestor_selected(QString g);
    bool delete_file_in_server(QString file);
    void on_posicionBorrado(int pos, int total);
    void hideMenu(const QString from = "");
    void updateTareasInTable();
signals:
    void setLoadingTextSignal(QString);
    void sectionPressed();
    void tablePressed();
    void closing();
    void script_excecution_result(int);
    void abrirTablaOperarios();
    void abrirTablaContadores(bool);
    void openPiezasTable();
    void openZonasTable();
    void openCausasTable();
    void openResultadosTable();
    void openEmplazamientosTable();
    void openClasesTable();
    void openEquipo_OperariosTable();
    void openTiposTable();
    void openCalibresTable();
    void openInfosTable();
    void openGestoresTable();
    void openEmpresasTable();
    void openAdministradoresTable();
    void openLongitudesTable();
    void openRuedasTable();
    void openRutasTable();
    void openMarcasTable();
    void openObservacionesTable();
    void openITACsTable();
    void crearNuevaTarea();
    void sendData(QJsonObject);
    void importarTXT(QString);
    void importarExcel(QString);
    void importarDAT(QString);
    void updateTableInfo();
    void updateITACsInfo();
    void exportarExcelyDAT();
    void eraseJsonArrayInServer(QJsonArray);
    void hideMenuFast(QString);
    void mouse_pressed();
    void mouse_Release();
    void gestorSelected(QString);
    void file_downloaded_result(int);
    void file_downloaded_dir(QString);
    void hidingLoading();

protected slots:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void get_fields_selected(QMap<QString, QString> map_received);
    bool checkIfNewInfoSync();
    void fixModelforTable(QJsonArray, bool save_history = true);
    void on_rb_todas_clicked();
    void on_rb_abierta_clicked();
    QJsonArray fillFilterOrdenABIERTAS(QJsonArray);
    void on_rb_ejecutada_clicked();
    QJsonArray fillFilterOrdenEJECUTADAS(QJsonArray);
    void on_rb_cerrada_clicked();
    QJsonArray fillFilterOrdenCERRADAS(QJsonArray);
    void on_rb_informada_clicked();
    QJsonArray fillFilterOrdenINFORMADAS(QJsonArray);


    void on_rb_requerida_clicked();
    QJsonArray fillFilterOrdenREQUERIDAS(QJsonArray);

    QJsonArray fillFilterOrdenCITAS(QJsonArray);
    QJsonArray fillFilterOrdenAUSENTES(QJsonArray);

    void on_actionDireccion_triggered();

    void on_pb_filtrar_clicked();

    void on_actionTipo_de_Tarea_triggered();

    //    void on_cb_tipoTarea_currentIndexChanged(const QString &arg1);

    void on_actionC_Geolocalizaci_n_triggered();

    void on_actionTitular_triggered();

    void on_actionN_Abonado_triggered();

    void on_actionN_Serie_triggered();

    void on_actionContadores_triggered();

    void on_actionOperarios_triggered();

    void on_actionNueva_Tarea_triggered();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_actionExcel_4_triggered();

    void on_actionFichero_DAT_triggered();

    void on_actionExcel_3_triggered();

    void on_actionFichero_DAT_2_triggered();

    void on_actionExcel_5_triggered();

    void on_actionFichero_DAT_3_triggered();

    void on_actionEspeciales_triggered();

    void on_actionMasivas_triggered();

    void on_actionDiarias_triggered();

    void get_date_selected(QDate d);
    void get_dates_selected(QStringList dates);
    void exportExcelAndDat(QJsonArray jsonArray);

    void on_actionImportacion_triggered();

    void on_actionEjecucion_triggered();

    void on_actionCierre_triggered();

    void on_actionInformada_triggered();

    void on_actionAsignar_campos_comunes_triggered();

    void on_actionAsignar_a_un_operario_triggered();

    void get_user_selected(QString u);
    void on_actionN_Serie_2_triggered();

    void on_actionN_Abonado_2_triggered();

    void on_actionPor_Titular_triggered();

    void on_actionPor_BIS_triggered();

    void on_actionDe_Importaci_n_triggered();

    void on_actionDe_Cierre_triggered();

    void on_actionDe_Informe_triggered();

    void on_actionDe_Ejecuci_n_triggered();

    void on_actionPor_Piso_triggered();

    void on_pb_eliminar_clicked();

    void on_actionDe_Modificacion_triggered();

    void on_pb_nuevaTarea_clicked();

    void getContadoresToCompleter();
    void on_pb_hide_filters_clicked();

    void on_actionVer_Diarias_triggered();

    void on_actionVer_Masivas_triggered();

    void on_actionVer_Especiales_triggered();

    void on_actionDe_Modificacion_2_triggered();


    
    void on_actionPor_Portal_triggered();

    void on_tableView_pressed(const QModelIndex &index);

    void on_rb_citas_clicked();

    void on_rb_ausente_clicked();

    void getMenuClickedItem(int selected);
    void on_pb_atras_clicked();

    void on_pb_new_info_clicked();

    void on_actionCambios_sin_Revisar_triggered();

    void on_tarea_revisada(QString princ_var);
    void on_pb_crear_excel_clicked();

    void on_actionDe_Cita_triggered();

    void on_actionDe_Cita_2_triggered();

    void on_pb_logotipo_clicked();

    void on_actionPor_Operario_triggered();

    void on_actionCargar_Trabajo_Salvado_triggered();

    void getFileDownloadDir(QString dir);
    void getFileDirSelected(QString dir);
    void on_actionDesdde_Fichero_TXT_triggered();

    void on_actionSubir_Trabajo_a_Servidor_triggered();

    void on_pb_respaldar_clicked();

    void on_actionContadores_Unitarios_triggered();

    void on_actionContadores_en_Bater_a_triggered();

    void on_actionPor_Ubicacion_en_Bater_a_triggered();

    void on_actionContactar_triggered();

    void on_actionTrabajar_sin_conexion_a_Tabla_triggered();

    void on_actionGeolocalizar_una_tarea_triggered();

    void on_actionAsignar_campos_comunes_2_triggered();

    void on_actionAsignar_a_un_operario_2_triggered();

    void on_sectionClicked(int logicalIndex);
    void on_actionDesde_Fichero_DAT_triggered();

    void on_pb_buscar_trabajo_clicked();
    void getFilesDirSelected(QStringList dirs);

    QString descargarArchivo(QString file);
    void on_pb_gestor_inicial_seleccionado_clicked();

    void on_actionZona_triggered();

    void on_actionMarcas_triggered();

    void on_actionPiezas_triggered();

    void on_actionZonas_triggered();

    void on_actionObservaciones_triggered();

    void on_actionCausas_triggered();

    void on_actionResultados_triggered();

    void on_actionEmplazamientos_triggered();

    void on_actionClases_triggered();
    void on_actionEquipo_Operarios_triggered();
    void on_actionTipo_triggered();

    void on_actionRuedas_triggered();

    void on_actionLongitudes_triggered();

    void on_actionCalibres_triggered();

    void on_pb_agregar_tipoTarea_clicked();

    void on_actionInfos_triggered();

    void on_actionFacturar_triggered();

    void facturarTrabajo(QStringList seleccionadas);
    void on_pb_open_menu_clicked();
    void on_pb_close_clicked();
    void on_pb_minimize_clicked();
    void on_pb_maximize_clicked();
    void on_drag_screen();
    void on_drag_screen_released();
    void on_start_moving_screen_timeout();
    void onActionPress(QString action);
    void seleccionarInicial();
    void rightClikedTable(QMouseEvent *event);
    void leftClikedTable(QMouseEvent *event);
    void on_le_a_filtrar_returnPressed();

    void on_pb_buscar_clicked();

    void on_cb_portal_currentIndexChanged(const QString &arg1);

    void on_actionGestores_triggered();
    void on_actionEmpresas_triggered();
    void on_actionAdministradores_triggered();
    void on_pb_gestor_inicial_seleccionado_2_clicked();

    void on_actionDescargar_Fotos();
    void on_pb_cercania_clicked();

    void on_actionITACs_triggered();
    void on_actionTipo_Orden_triggered();
    void filtrarPorOrden(QString orden);

    void on_actionPor_Resultado_triggered();
    void openTareaX(QString numin);
    void setJsonArrayFilterbyPerimeter(QStringList princ_vars);

    void on_actionAsignar_a_un_equipo_triggered();
    void get_equipo_selected(QString u);
    void serverAnswer(QByteArray ba, database_comunication::serverRequestType tipo);
    void update_tareas_fields_request();
    void on_actionResumen_Tareas_triggered();
    void updateITACsServerInfo();
    void on_actionFiltrarPrioridad_triggered();
    void filtrarPrioridad(QString prioridad_selected);
    void on_actionRutas_triggered();
    void get_tareas_informadas_request();
    void on_actionEquipo_triggered();
    void getMenuSectionClickedItem(int selection);
    void filterColumnField();
    void addRemoveFilterList(QString value);
    void upload_save_work_file_request();
    void send_client_work_file_request();
    void delete_file_request();
    void download_save_work_file_request();
    bool sendClientWorkFileToServer(QString filename);
    void download_client_work_file_request();
    QString descargarArchivoCliente(QString file);
    void getFileDirSelectedAndDownload(QString file_download_dir_selected);
    void filtrarEnTabla(bool checked);
    void on_pb_idOrdenFix_clicked();

    void get_tareas_amount_request();
    void get_tareas_request();
    void moveToPage(QString page);

    void on_pb_next_pagination_clicked();

    void checkPaginationButtons();
    void on_pb_previous_pagination_clicked();

    void get_all_column_values_request();
    bool getTareasValuesFieldServer(QString empresa, QString column);
    void fillValuesInLineEditToFilter();
    bool getTareasValuesFieldCustomQueryServer(QString empresa, QString column, QString query);
    bool getTareasValuesFieldCustomQueryServer(QString empresa, QString column, QString query, QString limit);
    void get_all_column_values_custom_query_request();
    void on_le_poblacion_editingFinished();

    void on_le_calle_editingFinished();
    void show_loading(QString mess);
    void hide_loading();

    void add_causas_to_select();
    void add_calibres_to_select(const QString &anomalia);
    void update_itacs_fields_request();
    void on_pb_database_config_clicked();

    void setTareasPorPagina(int cant);
    void on_pushButton_clicked();

    void on_actionSendMessage_triggered();
    void countSelectedTasks();
    void on_actionAsignOrderID_triggered();
    void updateIDOrdenes(QString id_orden);
    void on_actionN_SerieDevuelto_triggered();


    void on_le_a_filtrar_textEdited(const QString &arg1);

    void triggerGetColumnValues();
    void addCheckBoxes(QStringList values);
    void updateCheckBoxes(QString value);
    void triggerGetCheckBoxesValues();
private:
    Ui::Tabla *ui;
    QThread thread;
    bool solo_tabla_actual = true;
    database_comunication database_com;
    QStringList keys, values;
    QString equipoName="";
    QString empresa = "";
    QTimer start_moving_screen;
    QTimer selectedTaskTimer;
    int init_pos_x;
    int init_pos_y;
    bool first_move = true;

    bool connected_header_signal = false;
    QStandardItemModel* model=nullptr;
    QList<int> rows_red;
    int fields_count_in_table = 21;
    QJsonArray jsonArrayContadores;
    QStringList numeros_serie_contadores, modificadas_princ_var, files_download_dir_selected;
    QJsonArray jsonArrayAll; //todas menos las informadas
    QJsonArray jsonArrayAllInformadas;
    QJsonArray jsonArrayInTable;
    QJsonArray jsonArrayInTableFiltered;

    int filter_type = 0;
    QString filter_column_field_selected = "";
    QMap <int,QString> map_months;
    QMap <QString,QString> map_days_week;
    QCompleter *completer;
    bool filtering = false, lastOrderString = false;
    QDate selection_date;
    QStringList selected_dates;
    QString selected_status, fecha_to_filter ="";
    QString selection_Order;
    Operator_Selection_Screen *seleccionOperarioScreen = nullptr;
    QString operatorName, file_download_dir_selected, file_download_dir_string, lastSectionCliked ="", lastSectionField = "";

    QMap<QString, QString> fields_selected, mapaEstados;

    void fillFilterPoblacion();

    void fillMapaTiposTareas();
    QString get_date_from_status(QJsonObject object, QString status);
    QJsonObject set_date_from_status(QJsonObject object, QString status, QString date, QTime time = QTime::currentTime());
    void export_to_dat_IDExp(QJsonArray jsonArray);
    QString composeNumExp(int num);
    QString setDirToExplorer();
    QString fill_with_spaces(QString str, int size, bool blank = true);
    QString export_to_excel_IDExp(QJsonArray jsonArray);
    int Script_excecution_result;
    QString setDirExpToExplorer();
    void filtrarPorFecha(QString tipoFecha);

    void hideAllFilters();
    QJsonArray ordenarPor(QJsonArray jsonArray, QStringList fields, QString type = "");
    void ordenarPor(QString field, QString type,  bool salvarHistoria = true);
    void ordenarPor(QString field, int type, bool salvarHistoria = true);
    QJsonArray fixJsonToLastModel(QJsonArray jsonArray);

    void uncheckAllRadioButtons();
    QJsonArray getCurrentRadioButton(QJsonArray);
    int lastSelectedRow = -1;

    QList<QJsonArray> jsonArrayHistoryList;
    QList<int> radioButtonsHistory;
    QList<int> countTareasHistory;    
    QList<QString> showingTextHistory;
    QList<QString> paginationHistory;
    QList<QStringList> paginationItemsHistory;
    QList<QString> queryHistory;
    QList<int> id_startHistory;

    void abrirTareaX(int index);
    int getCurrentRadioButton();
    void setRadiobutton(int type);
    void habilitarBotonAtras(bool enable_disable);
    void filtrarPorCambiosSinRevisar(int id_start = 0);
    void export_tasks_in_table_to_excel();
    QJsonObject get_JObject_from_JArray(QJsonArray jsonArray, QString principal_var);

    QJsonArray fillFilterPorOperario(QJsonArray jsonArray, QString operarioSelected);
    QString crearFicheroTxtConTabla(QJsonArray jsonArray, QString ruta_y_nombre_file);
    QString crearFicheroDATConTabla(QJsonArray jsonArray,QString ruta_y_nombre_file);
    QJsonArray getCurrentJsonArrayInTable();
    QByteArray builUploadString(QString filename_and_dir);
    QJsonArray cargarJsonArrayDeTxt(QString ruta = "");
    QTimer timer;

    void buscarTrabajoDevueltoEnLinea();
    QCompleter *completer_calles;
    QCompleter *completer_poblaciones;

    bool matchMultipleFields(QMap<QString, QString> fields, QJsonObject jsonObject);
    QString eliminarNumerosAlFinal(QString string);
    QJsonArray loadExportedDatFile(QString filename);
    QMap<QString, QString> fillMapaExpCABB();
    QMap<QString, QString> fillMapaExp();
    QMap<QString, QString> fillMapaExpAlternativo();
    QString fillSheetPartes(QJsonArray jsonArray, QXlsx::Document &xlsx);
    QString fillSheetResumenMaterial(QJsonArray jsonArray, QXlsx::Document &xlsx);
    QString cantidadDeCalibreMM(QString cal, QJsonArray jsonArray);
    QString cantidadDeCalibreWOLTMAN(QString cal, QJsonArray jsonArray);
    QString cantidadDeCalibreTipoRadio(QString radio, QJsonArray jsonArray);
    QString cantidadDeREED(QJsonArray jsonArray);
    QString cantidadDeDP(QJsonArray jsonArray);
    QString cantidadDePulsar(QJsonArray jsonArray);
    QString fillSheetResumenServicios(QJsonArray jsonArray, QXlsx::Document &xlsx);
    QString cantidadCamposResumenServicios(QString cals, QJsonArray jsonArray);
    QString fillSheetCalibres(QJsonArray jsonArray, QXlsx::Document &xlsx);
    QString fillSheetExcepciones(QJsonArray jsonArray, QXlsx::Document &xlsx);
    QString fillSheetCausas(QJsonArray jsonArray, QXlsx::Document &xlsx);
    int ultimoIDExportacion = 0;
    int ultimoIDSAT = 0;
    int ultimoIDOrden = 0;
    QJsonArray readJsonArrayFromFile(QString filename);
    QMap<QString, QString> fillMapaFacturacionCABB();
    QString fillSheetFacturacion(QJsonArray jsonArray, QStringList seleccionadas);
    QString eliminarCharacteresAlFinal(QString string);
    QString returnIfSerieOrPrefijo(QJsonArray jsonArray, int pos, QString field, QString previous_value);
    void uncheckAllRadioButtons(int state_marked);
    QMap<QString, QString> fillMapForFixModel(QStringList &listHeaders);
    void setLoadingText(QString mess);
    void on_actionMostrarEnMapa();
    bool checkGestor(QString gestor);
    bool update_fields(QStringList numeros_internos_list, QJsonObject campos);


    bool get_tareas_informadas();
    QJsonArray fillFilterPorEquipo(QJsonArray jsonArray, QString equipoSelected);
    void scalePhoto(QPixmap pixmap);
    QStringList getFieldValues(QString field, QString searchValue);
    void showFilterWidgetOptions(bool offset = true);//offset - desplazar ubicacion o no
    QStringList filterColumnList;
    QPoint lastCursorPos;
    QString getScrollBarStyle();
    QStringList campos_de_fechas;
    QJsonArray ordenarPorBateria(QJsonArray jsonArray);
    QJsonArray loadIDOrdenesExcel();
    QMap<QString, QString> mapExcelImport(QStringList listHeaders);
    bool checkValidJsonObjectFields(QJsonObject jsonObject);
    void cargarDesdeExcel();
    int containsField(QJsonArray jsonArray, QString field, QString fieldValue);
    void addItemsToPaginationInfo(int sizeShowing = LIMIT);
    QString selectGestor();
    QString getQueyStatus();

    QString defaultQuery = "NUMIN <> 'NULL'";
    QString lastQuery = defaultQuery;
    int last_id_start = 0;
    QJsonObject jsonInfoTareasAmount = QJsonObject();
    int countTareas = 0; //showing
    int currentPages = 0;
    int currentPage = 1;
    QString currentGestor = "";
    int limit_pagination = LIMIT;
    QJsonObject jsonObjectValues;

    bool getTareasCustomQuery(QString query, int id_start = 0);
    bool filter_enabled = false;
    bool updateITACs(QStringList lista_cod_emplazamientos, QJsonObject campos);
    QJsonArray importarExtraExcel();
    void migrateExceltoExcel();

    QJsonObject getJsonObjectInJsonArray(QJsonArray jsonArray, QString field, QString value);
    QMap<QString, QString> mapExcelExtraImport(QStringList listHeaders);
    void export_jsonArray_to_excel(QJsonArray jsonArray, QMap<QString, QString> mapa_exportacion, QStringList listHeaders);
    void fixPortals();
    void updateTask(QJsonObject jsonObject);
    QTimer timerAutocomplete;
    QTimer timerAutocompleteCheckBoxes;
    QString searchString = "";
    QWidget *widgetValues;
    QCheckBox *cb_todos = nullptr;
    MyLineEditShine *lineEdit = nullptr;
};

#endif // TABLA_H
