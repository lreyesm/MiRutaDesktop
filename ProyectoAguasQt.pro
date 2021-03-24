#-------------------------------------------------
#
# Project created by QtCreator 2019-09-09T19:40:28
#
#-------------------------------------------------

QT       += core gui network xlsx printsupport axcontainer quickwidgets positioning location sql
#QT += webkit webkitwidgets printsupport #impresion

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProyectoAguasQt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    MarkerModel.cpp \
    QProgressIndicator.cpp \
    administrador.cpp \
    animationsclass.cpp \
    barcodegenerator.cpp \
    calendardialog.cpp \
    calibre.cpp \
    cantidad_piezas.cpp \
    causa.cpp \
    clase.cpp \
    clickablelabel.cpp \
    cliente.cpp \
    combobox.cpp \
    counter.cpp \
    databaseoptions.cpp \
    daterangeselection.cpp \
    datetimewidget.cpp \
    dbtareascontroller.cpp \
    delegate.cpp \
    emplazamiento.cpp \
    empresa.cpp \
    equipo_operario.cpp \
    equipo_selection_screen.cpp \
    facturacion.cpp \
    fields_to_assign.cpp \
    fields_to_assing_counters.cpp \
    fields_to_assing_itacs.cpp \
    gestor.cpp \
    globalfunctions.cpp \
    idordenassign.cpp \
    info.cpp \
    informe_instalacion_servicios.cpp \
    itac.cpp \
    jornada_operario.cpp \
    longitud.cpp \
    maildialog.cpp \
        main.cpp \
        mainwindow.cpp \
    mapa_zonas.cpp \
    mapas.cpp \
    mapas_cercania.cpp \
    mapas_cercania_itacs.cpp \
    marca.cpp \
    markermodelcercania.cpp \
    mycheckbox.cpp \
    mycomboboxshine.cpp \
    mylabelanimated.cpp \
    mylabelmenuanimated.cpp \
    mylabelmenuchildanimated.cpp \
    mylabelphoto.cpp \
    mylabelshine.cpp \
    mylabelspinner.cpp \
    mylabelstateanimated.cpp \
    mylineeditautocomplete.cpp \
    mylineeditshine.cpp \
    myplaintextshine.cpp \
    mypointanimated.cpp \
    myradiobuttonanimated.cpp \
    mytableview.cpp \
    mywidgetanimated.cpp \
    mywidgetmenuanimated.cpp \
    mywidgetshine.cpp \
    observacion.cpp \
    observationseletionscreen.cpp \
    obtener_geolocalizacion.cpp \
    operario.cpp \
    other_task_screen.cpp \
    pieza.cpp \
    processesclass.cpp \
    qlabel_button.cpp \
    qlabeldate.cpp \
    qpersonalizebutton.cpp \
    qwidgetdate.cpp \
    restriction.cpp \
    restrictions.cpp \
    resultado.cpp \
    resumen_estado_instalacion.cpp \
    resumen_tareas.cpp \
    rightclickedsection.cpp \
    rightclickmenu.cpp \
    rueda.cpp \
    database_comunication.cpp \
    my_label.cpp \
    ruta.cpp \
    screen_tabla_contadores.cpp \
    screen_tabla_tareas.cpp \
    screen_table_administradores.cpp \
    screen_table_calibres.cpp \
    screen_table_causas_intervenciones.cpp \
    screen_table_clases.cpp \
    screen_table_clientes.cpp \
    screen_table_emplazamientos.cpp \
    screen_table_empresas.cpp \
    screen_table_equipo_operarios.cpp \
    screen_table_gestores.cpp \
    screen_table_infos.cpp \
    screen_table_itacs.cpp \
    screen_table_longitudes.cpp \
    screen_table_marcas.cpp \
    screen_table_observaciones.cpp \
    screen_table_operarios.cpp \
    screen_table_piezas.cpp \
    screen_table_resultado_intervenciones.cpp \
    screen_table_ruedas.cpp \
    screen_table_rutas.cpp \
    screen_table_tipos.cpp \
    screen_table_zonas.cpp \
    screen_upload_contadores.cpp \
    screen_upload_itacs.cpp \
    seccion_acceso_y_ubicacion.cpp \
    seccion_estado.cpp \
    seccion_estado_de_las_valvulas.cpp \
    seccion_estado_de_tuberias.cpp \
    seccion_llaves_de_puertas.cpp \
    seleccion_gestor.cpp \
    select_file_to_download.cpp \
    selection_priority.cpp \
    selectionorder.cpp \
    smtp.cpp \
    tabla.cpp \
    operator_selection_screen.cpp \
    navegador.cpp \
    tipo.cpp \
    tutoriales.cpp \
    zona.cpp \
    zoompicture.cpp

HEADERS += \
    MarkerModel.h \
    QProgressIndicator.h \
    administrador.h \
    animationsclass.h \
    barcodegenerator.h \
    calendardialog.h \
    calibre.h \
    cantidad_piezas.h \
    causa.h \
    clase.h \
    clickablelabel.h \
    cliente.h \
    combobox.h \
    counter.h \
    databaseoptions.h \
    daterangeselection.h \
    datetimewidget.h \
    dbtareascontroller.h \
    delegate.h \
    emplazamiento.h \
    empresa.h \
    equipo_operario.h \
    equipo_selection_screen.h \
    facturacion.h \
    fields_to_assign.h \
    fields_to_assing_counters.h \
    fields_to_assing_itacs.h \
	gestor.h \
    global_variables.h \
    globalfunctions.h \
    idordenassign.h \
    info.h \
    informe_instalacion_servicios.h \
    itac.h \
    jornada_operario.h \
    longitud.h \
    maildialog.h \
        mainwindow.h \
    mapa_zonas.h \
    mapas.h \
    mapas_cercania.h \
    mapas_cercania_itacs.h \
    mapas_contadores.h \
    marca.h \
    markermodelcercania.h \
    mycheckbox.h \
    mycomboboxshine.h \
    mylabelanimated.h \
    mylabelmenuanimated.h \
    mylabelmenuchildanimated.h \
    mylabelphoto.h \
    mylabelshine.h \
    mylabelspinner.h \
    mylabelstateanimated.h \
    mylineeditautocomplete.h \
    mylineeditshine.h \
    myplaintextshine.h \
    mypointanimated.h \
    myradiobuttonanimated.h \
    mytableview.h \
    mywidgetanimated.h \
    mywidgetmenuanimated.h \
    mywidgetshine.h \
    new_table_structure.h \
    observacion.h \
    observationseletionscreen.h \
    obtener_geolocalizacion.h \
    operario.h \
    other_task_screen.h \
    pieza.h \
    processesclass.h \
    qlabel_button.h \
    qlabeldate.h \
    qpersonalizebutton.h \
    qwidgetdate.h \
    restriction.h \
    restrictions.h \
    resultado.h \
    resumen_estado_instalacion.h \
    resumen_tareas.h \
    rightclickedsection.h \
    rightclickmenu.h \
    rueda.h \
    database_comunication.h \
    my_label.h \
    ruta.h \
    screen_tabla_contadores.h \
    screen_tabla_tareas.h \
    screen_table_administradores.h \
    screen_table_calibres.h \
    screen_table_causas_intervenciones.h \
    screen_table_clases.h \
    screen_table_clientes.h \
    screen_table_emplazamientos.h \
    screen_table_empresas.h \
    screen_table_equipo_operarios.h \
    screen_table_gestores.h \
    screen_table_infos.h \
    screen_table_itacs.h \
    screen_table_longitudes.h \
    screen_table_marcas.h \
    screen_table_observaciones.h \
    screen_table_operarios.h \
    screen_table_piezas.h \
    screen_table_resultado_intervenciones.h \
    screen_table_ruedas.h \
    screen_table_rutas.h \
    screen_table_tipos.h \
    screen_table_zonas.h \
    screen_upload_contadores.h \
    screen_upload_itacs.h \
    seccion_acceso_y_ubicacion.h \
    seccion_estado.h \
    seccion_estado_de_las_valvulas.h \
    seccion_estado_de_tuberias.h \
    seccion_llaves_de_puertas.h \
    seleccion_gestor.h \
    select_file_to_download.h \
    selection_priority.h \
    selectionorder.h \
    smtp.h \
    structure_administrador.h \
    structure_calibre.h \
    structure_causa.h \
    structure_clase.h \
    structure_cliente.h \
    structure_contador.h \
    structure_emplazamiento.h \
    structure_empresa.h \
    structure_equipo_operario.h \
    structure_gestor.h \
    structure_info.h \
    structure_itac.h \
    structure_login.h \
    structure_longitud.h \
    structure_marca.h \
    structure_observacion.h \
    structure_operario.h \
    structure_pieza.h \
    structure_resultado.h \
    structure_rueda.h \
    structure_ruta.h \
    structure_tipo.h \
    structure_zona.h \
    tabla.h \
    operator_selection_screen.h \
    navegador.h \
    tipo.h \
    tutoriales.h \
    zona.h \
    zoompicture.h

FORMS += \
    administrador.ui \
    calendardialog.ui \
    calibre.ui \
    cantidad_piezas.ui \
    causa.ui \
    clase.ui \
    cliente.ui \
    counter.ui \
    databaseoptions.ui \
    daterangeselection.ui \
    datetimewidget.ui \
    emplazamiento.ui \
    empresa.ui \
    equipo_operario.ui \
    equipo_selection_screen.ui \
    facturacion.ui \
    fields_to_assign.ui \
    fields_to_assing_counters.ui \
    fields_to_assing_itacs.ui \
    gestor.ui \
    idordenassign.ui \
    info.ui \
    informe_instalacion_servicios.ui \
    itac.ui \
    jornada_operario.ui \
    longitud.ui \
    maildialog.ui \
        mainwindow.ui \
    mapa_zonas.ui \
    mapas.ui \
    mapas_cercania.ui \
    mapas_cercania_itacs.ui \
    marca.ui \
    observacion.ui \
    observationseletionscreen.ui \
    obtener_geolocalizacion.ui \
    operario.ui \
    other_task_screen.ui \
    pieza.ui \
    restriction.ui \
    restrictions.ui \
    resultado.ui \
    resumen_estado_instalacion.ui \
    resumen_tareas.ui \
    rightclickedsection.ui \
    rightclickmenu.ui \
    rueda.ui \
    ruta.ui \
    screen_tabla_contadores.ui \
    screen_tabla_tareas.ui \
    screen_table_administradores.ui \
    screen_table_calibres.ui \
    screen_table_causas_intervenciones.ui \
    screen_table_clases.ui \
    screen_table_clientes.ui \
    screen_table_emplazamientos.ui \
    screen_table_empresas.ui \
    screen_table_equipo_operarios.ui \
    screen_table_gestores.ui \
    screen_table_infos.ui \
    screen_table_itacs.ui \
    screen_table_longitudes.ui \
    screen_table_marcas.ui \
    screen_table_observaciones.ui \
    screen_table_operarios.ui \
    screen_table_piezas.ui \
    screen_table_resultado_intervenciones.ui \
    screen_table_ruedas.ui \
    screen_table_rutas.ui \
    screen_table_tipos.ui \
    screen_table_zonas.ui \
    screen_upload_contadores.ui \
    screen_upload_itacs.ui \
    seccion_acceso_y_ubicacion.ui \
    seccion_estado.ui \
    seccion_estado_de_las_valvulas.ui \
    seccion_estado_de_tuberias.ui \
    seccion_llaves_de_puertas.ui \
    seleccion_gestor.ui \
    select_file_to_download.ui \
    selection_priority.ui \
    selectionorder.ui \
    tabla.ui \
    operator_selection_screen.ui \
    navegador.ui \
    tipo.ui \
    tutoriales.ui \
    zona.ui \
    zoompicture.ui

RESOURCES += \
    assets.qrc

RC_ICONS = /icons/icono.ico

DISTFILES +=

INCLUDEPATH += "ZipFilesLibrary/include"
INCLUDEPATH += "geoservices"
LIBS += -L"ZipFilesLibrary/lib"
#LIBS += -L"geoservices"
#INCLUDEPATH += "C:/Users/luis.reyes/Desktop/ZipFilesLibrary/include"
#LIBS += -L"C:/Users/luis.reyes/Desktop/ZipFilesLibrary/lib"
LIBS += -lquazip -lzlib
#LIBS += -lqtgeoservices_googlemaps
