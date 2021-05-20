#ifndef NEW_TABLE_STRUCTURE_H
#define NEW_TABLE_STRUCTURE_H
#include <QString>
#include <QStringList>

static QString id = "id";                                   //checked
static QString idOrdenCABB = "idOrdenCABB";                 //checked   //new//
static QString FechImportacion = "FechImportacion";         //checked   //new//
static QString numero_interno = "NUMIN";                    //checked
static QString GESTOR = "GESTOR"; //campo para saber la empresa empladora del trabajo a realizar
static QString ANOMALIA = "ANOMALIA";                       //checked   //new//
static QString AREALIZAR = "AREALIZAR";                     //checked   //new//
static QString INTERVENCION = "INTERVENCI";                 //checked   //new//
static QString reparacion = "REPARACION";                   //checked
static QString propiedad = "PROPIEDAD";                     //checked
static QString CONTADOR_Prefijo_anno = "CONTADOR";          //checked
static QString numero_serie_contador = "SERIE";             //checked
static QString marca_contador = "MARCA";                    //checked
static QString calibre_toma = "CALIBRE";                    //checked
static QString ruedas = "RUEDAS";                           //checked
static QString fecha_instalacion = "FECINST"; //fecha inst de contador viejo               //checked   //new//
static QString actividad = "ACTIVI";                        //checked
static QString emplazamiento = "EMPLAZA";                   //checked
static QString acceso = "ACCESO";                           //checked
static QString calle = "CALLE";                             //checked
static QString numero = "NUME";//numero de portal           //checked
static QString BIS = "BIS";                                 //checked   //new//
static QString piso = "PISO";                               //checked
static QString mano = "MANO";                               //checked
static QString poblacion = "MUNICIPIO";                     //checked
static QString nombre_cliente = "NOMBRE_ABONADO";           //checked
static QString numero_abonado = "Numero_de_ABONADO";        //checked
static QString nombre_firmante = "NOMBRE_FIRMANTE";
static QString numero_carnet_firmante = "NUMERO_CARNET_FIRMANTE";
//      static    QString CODLEC = "CODLEC";
static QString FECEMISIO = "FECEMISIO";  //fecha emision, misma que de importacion                   //checked   //new//
static QString FECULTREP = "FECULTREP";   //-------------------------------------------------------------------------------------                 //checked   //new//
static QString OBSERVA = "OBSERVA";///observaciones de informe//-----------------------//checked   //new//
static QString RS = "RS";     //-------------------------------------------------------------------------------------------------                              //checked   //new//
static QString F_INST = "F_INST";  //fecha de instalacion del nuevo contador                           //checked   //new//
static QString INDICE = "INDICE";                           //checked   //new//
static QString emplazamiento_devuelto = "EMPLAZADV";        //checked   //new//
static QString RESTO_EM = "RESTO_EM";  //resto emplazamiento devuelto                      //checked   //new//
//      static    QString LECT_LEV = "LECT_LEV";
static QString lectura_ultima = "CODLEC";//(voy a cambiar CODLEC del CABB para la lectura, ellos lo utilizan para la ruta)
static QString lectura_actual = "LECT_LEV";//-------------  //checked
static QString lectura_contador_nuevo = "LECTURA_CONTADOR_NUEVO";
static QString observaciones_devueltas = "OBSERVADV";//codigo de obs de app excritorio      //checked   //new//
static QString TIPO = "TIPO";   //CLASE DE CONTADOR                             //checked
static QString TIPO_devuelto = "TIPO_DEVUELTO"; //CLASE DE CONTADOR DEVUELTO            //checked   //new// ----------
static QString Estado = "Estado";                           //checked   //new//
static QString marca_devuelta = "MARCADV";                  //checked   //new//
static QString calibre_real = "CALIBREDV";                  //checked
static QString RUEDASDV = "RUEDASDV";                       //checked   //new//
static QString LARGO = "LARGO";                             //checked   //new//
static QString largo_devuelto = "LONGDV";                   //checked   //new//
static QString numero_serie_contador_devuelto = "seriedv";  //checked   //new//
static QString CONTADOR_Prefijo_anno_devuelto = "contadordv";//--------------//checked   //new//
static QString AREALIZAR_devuelta = "AREALIZARDV";//-------------------------//checked   //new//
static QString intervencion_devuelta = "intervencidv";      //checked   //new//
static QString RESTEMPLAZA = "RESTEMPLAZA";  //resto emplazamiento del gestor                //checked   //new//
static QString FECH_CIERRE = "FECH_CIERRE"; //fecha de cerrada tarea                 //checked   //new//
static QString TIPORDEN = "TIPORDEN";                       //checked   //new//
static QString operario = "OPERARIO";                       //checked
static QString observaciones = "observaciones";//obs del gestor (iniciales)            //checked
static QString TIPOFLUIDO = "TIPOFLUIDO";//-----------------------------------//checked   //new//
static QString TIPOFLUIDO_devuelto = "TIPOFLUIDO_DEVUELTO"; //checked   //new//------------------
static QString idexport = "idexport";                       //checked   //new//
static QString fech_facturacion = "fech_facturacion";       //checked   //new//
static QString fech_cierrenew = "fech_cierrenew";           //checked   //new//
static QString fech_informacionnew = "fech_informacionnew"; //checked   //new//
static QString f_instnew = "f_instnew";   //--Variable usada para ver quiene realizao ultima modificacion y si en adrodi o escritorio-----------------------------------------------------------------                 //checked   //new//
static QString tipoRadio = "tipoRadio";                     //checked   //new//
static QString tipoRadio_devuelto = "TIPORADIO_DEVUELTO";
static QString marcaR = "marcaR";                           //checked   //new//
static QString codigo_de_localizacion = "codigo_de_localizacion";//checked //mano en mapa
static QString codigo_de_geolocalizacion = "codigo_de_geolocalizacion";  //1.26.... codigo de emplazamiento
static QString geolocalizacion = "geolocalizacion";              //checked //casa en mapa
static QString url_geolocalizacion = "url_geolocalizacion";
static QString foto_antes_instalacion = "foto_antes_instalacion";
static QString foto_numero_serie = "foto_numero_serie";
static QString foto_lectura = "foto_lectura";
static QString foto_despues_instalacion = "foto_despues_instalacion";
static QString foto_incidencia_1 = "foto_incidencia_1";
static QString foto_incidencia_2 = "foto_incidencia_2";
static QString foto_incidencia_3 = "foto_incidencia_3";
static QString firma_cliente = "firma_cliente";
static QString tipo_tarea = "tipo_tarea";                           //checked   //new//
static QString telefonos_cliente = "telefonos_cliente";             //checked
static QString telefono1 = "telefono1";                             //checked
static QString telefono2 = "telefono2";                             //checked
static QString fechas_tocado_puerta = "fechas_tocado_puerta";       //checked
static QString fechas_nota_aviso = "fechas_nota_aviso";             //checked
static QString resultado = "resultado";                             //checked
static QString nuevo_citas = "nuevo_citas";                         //checked
static QString fecha_hora_cita = "fecha_hora_cita";                 //checked
static QString fecha_de_cambio = "fecha_de_cambio";                 //checked
static QString zona = "zona";                                       //checked
static QString ruta = "ruta";                                       //checked
static QString numero_serie_modulo = "numero_serie_modulo";         //checked
static QString ubicacion_en_bateria = "ubicacion_en_bateria";       //checked
static QString incidencia = "incidencia";                           //checked
static QString ID_FINCA = "ID_FINCA";                               //checked   //new//-----------
static QString COMENTARIOS = "COMENTARIOS";                         //checked   //new//-----------
static QString DNI_CIF_COMUNIDAD = "DNI_CIF_COMUNIDAD";             //checked   //new//-----------
static QString TARIFA = "TARIFA";                                   //checked   //new//-----------
static QString TOTAL_CONTADORES = "TOTAL_CONTADORES";               //checked   //new//-----------
static QString C_CANAL = "C_CANAL";                                 //checked   //new//-----------
static QString C_LYC = "C_LYC";                                     //checked   //new//-----------
static QString C_AGRUPA = "C_AGRUPA";                               //checked   //new//-----------
static QString DNI_CIF_ABONADO = "DNI_CIF_ABONADO";                 //checked   //new//-----------
static QString C_COMUNERO = "C_COMUNERO";                           //checked   //new//-----------
static QString MENSAJE_LIBRE = "MENSAJE_LIBRE";                     //checked   //new//-----------

static QString ID_SAT = "ID_SAT";                 //checked   //new//-----------//campos nuevos
static QString fecha_realizacion = "fecha_realizacion";                           //checked   //new//-----------//campos nuevos
static QString suministros = "suministros";                     //checked   //new//-----------//campos nuevos
static QString servicios = "servicios";
static QString equipo = "equipo";
static QString fecha_informe_servicios = "fecha_informe_servicios";
static QString piezas = "piezas";
static QString prioridad = "prioridad";

static QString causa_origen = "causa_origen"; //anomalia mas intervencion
static QString accion_ordenada = "accion_ordenada";
static QString hibernacion = "hibernacion";

static QString audio_detalle = "audio_detalle";
static QString numero_precinto = "numero_precinto";
static QString dia_predeterminado = "dia_predeterminado";

static QString date_time_modified = "date_time_modified";           //checked
static QString status_tarea = "status_tarea";                       //checked

static QString principal_variable = numero_interno;
static QString ultima_modificacion = f_instnew;
static QString fech_revisado = fech_cierrenew;
static QString fech_exportacion = FECEMISIO;
static QString OPERARIO = operario;

//estados de tarea
static QString state_abierta = "IDLE";
static QString state_exportada = "IDLE EXPORT";
static QString state_ausente = "IDLE CITA";
static QString state_cita = "IDLE CITA";
static QString state_ejecutada = "DONE";
static QString state_cerrada = "CLOSED";
static QString state_informada = "INFORMADA";
static QString state_revisada = "REVISADA";
static QString state_requerida = "REQUERIDA";
//OJO SE QUITO letra_del_edificio, numero_edificio

static QString default_anomaly = "Z21";

#endif // NEW_TABLE_STRUCTURE_H
