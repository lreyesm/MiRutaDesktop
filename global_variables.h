#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <QString>
#include <QDir>
#include <QColor>


static QString versionMiRuta = "V3.1.1";
static QColor color_blue_app = QColor(54,141,206);
static QString limite_archivos_en_servidor = "256Mb";
static QString formato_fecha_hora = "yyyy-MM-dd HH:mm:ss";
static QString formato_fecha = "yyyy-MM-dd";
static QString formato_fecha_sin_hora = "yyyy_MM_dd";
static QString formato_fecha_hora_for_filename = "yyyy_MM_dd_HH_mm_ss";
static QString formato_fecha_hora_new_view = "dd/MM/yyyy  HH:mm:ss";
static QString formato_fecha_hora_new_view_sin_hora = "dd/MM/yyyy";
static QString formato_fecha_hora_to_file_upload = "d MMMM yyyy HH_mm_ss";
static QString formato_fecha_hora_to_file_upload_ms = "d MMMM yyyy HH_mm_ss_zzz";
//ultimo trabajo antes de cerrar la app
static QString db_tareas_path = "TareasDataBase.db";
static QString tareas_sincronizadas = "BD/tareas_sincronizadas.dat";
static QString tareas_descargadas = "BD/tareas_descargadas.dat";
static QString tareas_modificadas = "BD/tareas_modificadas.dat";
static QString datos = "BD/datos.dat";
static QString trabajos_cargados = "BD/trabajos_cargados.dat";
static QString contadores_descargados = "BD/contadores_descargados.dat";
static QString numeros_serie_descargados = "BD/numeros_serie_descargados.dat";
static QString marcas_descargadas = "BD/marcas_descargadas.dat";
static QString zonas_descargadas = "BD/zonas_descargadas.dat";
static QString calibres_descargadas = "BD/calibres_descargadas.dat";
static QString longitudes_descargadas = "BD/longitudes_descargadas.dat";
static QString gestores_descargados = "BD/gestores_descargados.dat";
static QString ruedas_descargadas = "BD/ruedas_descargadas.dat";
static QString rutas_descargadas = "BD/rutas_descargadas.dat";
static QString causas_descargadas = "BD/causas_descargadas.dat";
static QString resultados_descargadas = "BD/resultados_descargadas.dat";
static QString emplazamientos_descargadas = "BD/emplazamientos_descargadas.dat";
static QString equipo_operarios_descargadas = "BD/equipo_operarios_descargadas.dat";
static QString clases_descargadas = "BD/clases_descargadas.dat";
static QString tipos_descargadas = "BD/tipos_descargadas.dat";
static QString observaciones_descargadas = "BD/observaciones_descargadas.dat";
static QString piezas_descargadas = "BD/piezas_descargadas.dat";
static QString ficheros_comprimidos_descargados = "BD/ficheros_comprimidos_descargados.dat";
static QString infos_descargadas = "BD/infos_descargadas.dat";
static QString empresas_descargadas = "BD/empresas_descargadas.dat";
static QString administradores_descargadas = "BD/administradores_descargadas.dat";
static QString itacs_descargadas = "BD/itacs_descargadas.dat";
static QString operarios_descargadas = "BD/operarios_descargadas.dat";
static QString clientes_descargados = "BD/clientes_descargados.dat";
static QString info_pagination = "BD/info_pagination.dat";

static QString informacion_local = "BD/informacion_local.dat";
static QString credentials = "BD/credentials.dat";
static QString url_server = "https://lreyesm.000webhostapp.com/Mi_Ruta/";
//ultimo trabajo antes de que se caiga la conexion
static QString ultimo_respaldo = QDir::currentPath()+"/Respaldos/ultimo_trabajo_salvado.txt";

enum {ABRIR, ENVIAR_MENSAJE, MOSTRAR_EN_MAPA,DESCARGAR_FOTOS, RESUMEN_TAREAS,ASIGNAR_COMUNES, ASIGNAR_A_EQUIPO, ASIGNAR_A_OPERARIO, ELIMINAR};

enum {TODAS,ABIERTAS,AUSENTE,CITA,EJECUTADA,CERRADA,INFORMADA,REQUERIDA, NINGUNO};



#endif // GLOBAL_VARIABLES_H
