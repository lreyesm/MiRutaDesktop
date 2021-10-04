#include "mywidgetmenuanimated.h"
#include <QDebug>
#include <QTimer>
#include "mylabelmenuanimated.h"
#include "mylabelmenuchildanimated.h"
#include <QGraphicsDropShadowEffect>

MyWidgetMenuAnimated::MyWidgetMenuAnimated(QWidget *parent)
    :QWidget(parent)
{
    //    this->move(0,0);
    //    this->setFixedSize(0, 0);///poner alto del padre (Toda la pantalla) y ancho 0
    //    setScaledContents(true);
    //    this->setMouseTracking(true);
    //    QTimer::singleShot(500, this, SLOT(setSizeInit()));
    //    initializeGL();
    this->setStyleSheet("background-color: #EFEFEF;"
                        "border-radius: 10px;");
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect();//dar sombra a borde del widget
    effect->setBlurRadius(20);
    effect->setOffset(10);
    this->setGraphicsEffect(effect);
}
MyWidgetMenuAnimated::~MyWidgetMenuAnimated()
{
    //    delete animation;
}

void MyWidgetMenuAnimated::emitActionToOwner(QString action){
    emit actionPressed(action);
}

void MyWidgetMenuAnimated::setMyLabelAnimatedProperties(MyLabelAnimated *button, QString name, int x, int y)
{
    button->setObjectName("l_" + name);
    button->setStyleSheet(styleHover);
//    button->setBackgroundImage("fondo_transparente");
    button->setPixmap(QPixmap(":/icons/"+name+".png"));
    button->setScaledContents(true);
    connect(button,SIGNAL(mouseLeftClicked(QString)),this,SLOT(emitActionToOwner(QString)));
    button->move(x,  y);
    button->setFixedSize(buttons_witdh, buttons_height);
    button->show();
}

void MyWidgetMenuAnimated::setOwner(const QString owner_name)
{

    if(owner_name == "l_archivo"){
        int x_pos = 50;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelMenuChildAnimated *importar = new MyLabelMenuChildAnimated(this);
        importar->setObjectName("l_importar");
        importar->setStyleSheet(styleHover);
//        importar->setBackgroundImage("fondo_transparente");
        importar->setPixmap(QPixmap(":/icons/importar.png"));
        importar->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),importar,SLOT(hideChilds()));
        connect(importar,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        importar->move(x_pos, y_pos);
        importar->show();
        y_pos+= space;

        MyLabelMenuChildAnimated *informar = new MyLabelMenuChildAnimated(this);
        informar->setObjectName("l_informar");
        informar->setStyleSheet(styleHover);
//        informar->setBackgroundImage("fondo_transparente");
        informar->setPixmap(QPixmap(":/icons/informar.png"));
        informar->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),informar,SLOT(hideChilds()));
        connect(informar,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        informar->move(x_pos, y_pos);
        informar->show();
        y_pos+= space;

        MyLabelAnimated *facturar = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(facturar, "facturar", x_pos,  y_pos);
        y_pos+= space;

        MyLabelMenuChildAnimated *cargar_trabajo = new MyLabelMenuChildAnimated(this);
        cargar_trabajo->setObjectName("l_cargar_trabajo");
        cargar_trabajo->setStyleSheet(styleHover);
//        cargar_trabajo->setBackgroundImage("fondo_transparente");
        cargar_trabajo->setPixmap(QPixmap(":/icons/cargar_trabajo.png"));
        cargar_trabajo->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),cargar_trabajo,SLOT(hideChilds()));
        connect(cargar_trabajo,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        cargar_trabajo->move(x_pos, y_pos);
        cargar_trabajo->show();
        y_pos+= space;

        MyLabelAnimated *subir_trabajo = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(subir_trabajo, "subir_trabajo", x_pos, y_pos);
        y_pos+= space;

        MyLabelAnimated *respaldar = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(respaldar, "respaldar", x_pos, y_pos);
        y_pos+= space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_cargar_trabajo") {
        int x_pos = 50;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *cargar_de_servidor = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(cargar_de_servidor, "cargar_de_servidor", x_pos, y_pos);
        cargar_de_servidor->setToolTip("Cargar tareas guardadas en servidor, previamente salvadas");
        y_pos+= space;

        MyLabelAnimated *cargar_txt = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(cargar_txt, "cargar_txt", x_pos, y_pos);
        cargar_txt->setToolTip("Cargar tareas guardadas en formato TXT");
        y_pos+= space;

        MyLabelAnimated *cargar_dat = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(cargar_dat, "cargar_dat", x_pos, y_pos);
        cargar_dat->setToolTip("Cargar tareas guardadas en formato DAT");
        y_pos+= space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_importar") {
        int x_pos = 50;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelMenuChildAnimated *importar_diarias = new MyLabelMenuChildAnimated(this);
        importar_diarias->setObjectName("l_importar_diarias");
        importar_diarias->setStyleSheet(styleHover);
//        importar_diarias->setBackgroundImage("fondo_transparente");
        importar_diarias->setPixmap(QPixmap(":/icons/importar_diarias.png"));
        importar_diarias->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),importar_diarias,SLOT(hideChilds()));
        connect(importar_diarias,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        importar_diarias->move(x_pos, y_pos);
        importar_diarias->show();
        y_pos+= space;

        MyLabelMenuChildAnimated *importar_masivas = new MyLabelMenuChildAnimated(this);
        importar_masivas->setObjectName("l_importar_masivas");
        importar_masivas->setStyleSheet(styleHover);
        importar_masivas->setBackgroundImage("fondo_transparente");
        importar_masivas->setPixmap(QPixmap(":/icons/importar_masivas.png"));
        importar_masivas->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),importar_masivas,SLOT(hideChilds()));
        connect(importar_masivas,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        importar_masivas->move(x_pos, y_pos);
        importar_masivas->show();
        y_pos+= space;

        MyLabelMenuChildAnimated *importar_especiales = new MyLabelMenuChildAnimated(this);
        importar_especiales->setObjectName("l_importar_especiales");
        importar_especiales->setStyleSheet(styleHover);
        importar_especiales->setBackgroundImage("fondo_transparente");
        importar_especiales->setPixmap(QPixmap(":/icons/importar_especiales.png"));
        importar_especiales->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),importar_especiales,SLOT(hideChilds()));
        connect(importar_especiales,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        importar_especiales->move(x_pos, y_pos);
        importar_especiales->show();
        y_pos+= space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_importar_diarias"){
        int x_pos = 50;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *importar_excel_diarias = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(importar_excel_diarias, "importar_excel_diarias", x_pos, y_pos);
        importar_excel_diarias->setToolTip("Importar tareas diarias desde fichero excel");
        y_pos+= space;

        MyLabelAnimated *importar_dat_diarias = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(importar_dat_diarias, "importar_dat_diarias", x_pos, y_pos);
        importar_dat_diarias->setToolTip("Importar tareas diarias desde fichero DAT");
        y_pos+= space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_importar_masivas"){
        int x_pos = 50;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *importar_excel_masivas = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(importar_excel_masivas, "importar_excel_masivas", x_pos, y_pos);
        importar_excel_masivas->setToolTip("Importar tareas masivas desde fichero excel");
        y_pos+= space;

        MyLabelAnimated *importar_dat_masivas = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(importar_dat_masivas, "importar_dat_masivas", x_pos, y_pos);
        importar_dat_masivas->setToolTip("Importar tareas masivas desde fichero DAT");
        y_pos+= space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_importar_especiales"){
        int x_pos = 50;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *importar_excel_especiales = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(importar_excel_especiales, "importar_excel_especiales", x_pos, y_pos);
        importar_excel_especiales->setToolTip("Importar tareas especiales desde fichero excel");
        y_pos+= space;

        MyLabelAnimated *importar_dat_especiales = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(importar_dat_especiales, "importar_dat_especiales", x_pos, y_pos);
        importar_dat_especiales->setToolTip("Importar tareas especiales desde fichero DAT");
        y_pos+= space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_informar") {
        int x_pos = 50;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *informar_diarias = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(informar_diarias, "informar_diarias", x_pos, y_pos);
        informar_diarias->setToolTip("Informar a excel las tareas diarias");
        y_pos+= space;

        MyLabelAnimated *informar_masivas = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(informar_masivas, "informar_masivas", x_pos, y_pos);
        informar_masivas->setToolTip("Informar a excel las tareas masivas");
        y_pos+= space;

        MyLabelAnimated *informar_especiales = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(informar_especiales, "informar_especiales", x_pos, y_pos);
        informar_especiales->setToolTip("Informar a excel las tareas especiales");
        y_pos+= space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_tablas") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelMenuChildAnimated *contador = new MyLabelMenuChildAnimated(this);
        contador->setObjectName("l_contador");
        contador->setStyleSheet(styleHover);
//        contador->setBackgroundImage("fondo_transparente");
        contador->setPixmap(QPixmap(":/icons/contador.png"));
        contador->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),contador,SLOT(hideChilds()));
        connect(contador,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        contador->setFixedSize(180, 50);
        contador->move(x_pos,  y_pos);
        contador->show();
        y_pos+=space;
        MyLabelAnimated *itacs = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(itacs, "itacs", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *emplazamientos = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(emplazamientos, "emplazamiento", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *zonas = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(zonas, "zonas", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *observaciones = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(observaciones, "observaciones", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *resultados = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(resultados, "resultados", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *causas = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(causas, "causas", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *piezas = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(piezas, "piezas", x_pos, y_pos);
        y_pos+=space;
        MyLabelMenuChildAnimated *administrador = new MyLabelMenuChildAnimated(this);
        administrador->setObjectName("l_administrador");
        administrador->setStyleSheet(styleHover);
//        administrador->setBackgroundImage("fondo_transparente");
        administrador->setPixmap(QPixmap(":/icons/administrador.png"));
        administrador->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),administrador,SLOT(hideChilds()));
        connect(administrador,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        administrador->move(x_pos,  y_pos);
        administrador->show();
        y_pos+=space;
        MyLabelAnimated *infos = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(infos, "infos", x_pos, y_pos);
        y_pos+=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_administrador") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        //        MyLabelAnimated *l_empresas = new MyLabelAnimated(this); //Importane no usar esto, esto es para el control de usuarios
        //        setMyLabelAnimatedProperties(l_empresas, "empresas", x_pos, y_pos);
        //        y_pos+=space;
        MyLabelAnimated *l_administradores = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(l_administradores, "administradores", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *l_gestores = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(l_gestores, "gestores", x_pos, y_pos);
        y_pos+=space;

        MyLabelAnimated *l_clientes = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(l_clientes, "clientes", x_pos, y_pos);
        y_pos+=space;

        MyLabelAnimated *equipo_operarios = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(equipo_operarios, "equipo_operarios", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *operarios = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(operarios, "operarios", x_pos, y_pos);
        y_pos+=space;

        MyLabelAnimated *rutas = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(rutas, "rutas", x_pos, y_pos);
        y_pos+=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_contador") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *l_contadores = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(l_contadores, "contadores", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *marcas = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(marcas, "marcas", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *clases = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(clases, "clases", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *tipos = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(tipos, "tipos", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *calibres = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(calibres, "calibres", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *longitudes = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(longitudes, "longitudes", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *ruedas = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(ruedas, "ruedas", x_pos, y_pos);
        y_pos+=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_ordenar") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelMenuChildAnimated *ordenar_fecha = new MyLabelMenuChildAnimated(this);
        ordenar_fecha->setObjectName("l_ordenar_fecha");
        ordenar_fecha->setStyleSheet(styleHover);
//        ordenar_fecha->setBackgroundImage("fondo_transparente");
        ordenar_fecha->setPixmap(QPixmap(":/icons/ordenar_fecha.png"));
        ordenar_fecha->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),ordenar_fecha,SLOT(hideChilds()));
        connect(ordenar_fecha,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        ordenar_fecha->move(x_pos, y_pos);
        ordenar_fecha->show();
        y_pos+=space;

        MyLabelAnimated *ubicacion_bateria = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(ubicacion_bateria, "ubicacion_bateria", x_pos, y_pos);
        y_pos+=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_ordenar_fecha") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *ordenar_fecha_importacion = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(ordenar_fecha_importacion, "ordenar_fecha_importacion", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *ordenar_fecha_cita = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(ordenar_fecha_cita, "ordenar_fecha_cita", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *ordenar_fecha_ejecucion = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(ordenar_fecha_ejecucion, "ordenar_fecha_ejecucion", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *ordenar_fecha_cierre = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(ordenar_fecha_cierre, "ordenar_fecha_cierre", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *ordenar_fecha_informe = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(ordenar_fecha_informe, "ordenar_fecha_informe", x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *ordenar_fecha_modificacion = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(ordenar_fecha_modificacion, "ordenar_fecha_modificacion", x_pos, y_pos);
        y_pos+=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_tareas") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *asignar_campos = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(asignar_campos, "asignar_campos", x_pos, y_pos);
        y_pos+=space;

        MyLabelAnimated *nueva_tarea = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(nueva_tarea, "nueva_tarea", x_pos, y_pos);
        y_pos+=space;

        MyLabelAnimated *asignar_FNT = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(asignar_FNT, "asignar_FNT", x_pos, y_pos);
        y_pos+=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_ayuda") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *contactar = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(contactar, "contactar", x_pos, y_pos);
        y_pos+=space;

        MyLabelAnimated *acercade = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(acercade, "acercade", x_pos, y_pos);
        y_pos+=space;

//        MyLabelMenuChildAnimated *tutoriales = new MyLabelMenuChildAnimated(this);
//        tutoriales->setObjectName("l_tutoriales");
//        tutoriales->setStyleSheet(styleHover);
//        tutoriales->setPixmap(QPixmap(":/icons/tutoriales.png"));
//        tutoriales->setScaledContents(true);
//        connect(this, SIGNAL(onHidden()),tutoriales,SLOT(hideChilds()));
//        connect(tutoriales,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
//        tutoriales->move(x_pos, y_pos);
//        tutoriales->show();
//        y_pos+=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if(owner_name == "l_tutoriales"){
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        //        MyLabelAnimated *tuto_sin_conexion = new MyLabelAnimated(this);
        //        setMyLabelAnimatedProperties(tuto_sin_conexion, "tuto_sin_conexion", x_pos, y_pos);
        //        y_pos+=space;

        MyLabelAnimated *tuto_asignar_campos = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(tuto_asignar_campos, "tuto_asignar_campos", x_pos, y_pos);
        y_pos+=space;

        //        MyLabelAnimated *tuto_google_map = new MyLabelAnimated(this);
        //        setMyLabelAnimatedProperties(tuto_google_map, "tuto_google_map", x_pos, y_pos);
        //        y_pos+=space;

        MyLabelAnimated *tuto_asignar_FNT = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(tuto_asignar_FNT, "tuto_asignar_FNT", x_pos, y_pos);
        y_pos+=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "pb_buscar") {//boton de filtrar que desplega menu
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;

        MyLabelAnimated *sin_revisar = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(sin_revisar, "sin_revisar", x_pos, y_pos);
        y_pos +=space;

        MyLabelMenuChildAnimated *filtrar_fecha = new MyLabelMenuChildAnimated(this);
        filtrar_fecha->setObjectName("l_filtrar_fecha");
        filtrar_fecha->setStyleSheet(styleHover);
//        filtrar_fecha->setBackgroundImage("fondo_transparente");
        filtrar_fecha->setPixmap(QPixmap(":/icons/filtrar_fecha.png"));
        filtrar_fecha->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),filtrar_fecha,SLOT(hideChilds()));
        connect(filtrar_fecha,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        filtrar_fecha->move(x_pos,  y_pos);
        filtrar_fecha->show();

        y_pos +=space;

        MyLabelAnimated *direccion = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(direccion, "direccion", x_pos, y_pos);
        y_pos +=space;
        MyLabelAnimated *tipo_orden = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(tipo_orden, "tipo_orden", x_pos, y_pos);
        y_pos +=space;
        MyLabelAnimated *tipo_tarea = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(tipo_tarea, "tipo_tarea", x_pos, y_pos);
        y_pos +=space;
        MyLabelAnimated *numero_abonado = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(numero_abonado, "numero_abonado", x_pos, y_pos);
        y_pos +=space;
        MyLabelAnimated *titular = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(titular, "titular", 60, y_pos);
        y_pos +=space;
        MyLabelAnimated *numero_serie = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(numero_serie, "numero_serie", x_pos, y_pos);
        y_pos +=space;

        MyLabelMenuChildAnimated *ubicacion = new MyLabelMenuChildAnimated(this);
        ubicacion->setObjectName("l_ubicacion");
        ubicacion->setStyleSheet(styleHover);
//        ubicacion->setBackgroundImage("fondo_transparente");
        ubicacion->setPixmap(QPixmap(":/icons/ubicacion.png"));
        ubicacion->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),ubicacion,SLOT(hideChilds()));
        connect(ubicacion,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        ubicacion->move(x_pos,  y_pos);
        ubicacion->show();
        y_pos +=space;

        MyLabelAnimated *fontanero = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(fontanero, "fontanero", x_pos, y_pos);
        y_pos +=space;

        MyLabelMenuChildAnimated *mas_filtros = new MyLabelMenuChildAnimated(this);
        mas_filtros->setObjectName("l_mas_filtros");
        mas_filtros->setStyleSheet(styleHover);
//        mas_filtros->setBackgroundImage("fondo_transparente");
        mas_filtros->setPixmap(QPixmap(":/icons/mas_filtros.png"));
        mas_filtros->setScaledContents(true);
        connect(this, SIGNAL(onHidden()),mas_filtros,SLOT(hideChilds()));
        connect(mas_filtros,SIGNAL(actionPress(QString)),this,SLOT(emitActionToOwner(QString)));
        mas_filtros->move(x_pos,  y_pos);
        mas_filtros->show();
        y_pos+=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);

    }
    else if (owner_name == "l_mas_filtros") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;

        MyLabelAnimated *equipo = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(equipo, "equipo", x_pos, y_pos);
        y_pos +=space;


        MyLabelAnimated *numero_serie_devuelto = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(numero_serie_devuelto, "numero_serie_devuelto", x_pos, y_pos);
        y_pos +=space;

        MyLabelAnimated *zona = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(zona, "zona", x_pos, y_pos);
        y_pos +=space;

        MyLabelAnimated *geolocalizacion = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(geolocalizacion, "geolocalizacion", x_pos, y_pos);
        y_pos +=space;

        MyLabelAnimated *resultado = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(resultado, "resultado", x_pos, y_pos);
        y_pos +=space;

        MyLabelAnimated *prioridad = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(prioridad, "prioridad", x_pos, y_pos);
        y_pos +=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_ubicacion") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *en_bateria = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(en_bateria, "en_bateria",  x_pos, y_pos);
        y_pos+=space;

        MyLabelAnimated *unitarios = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(unitarios, "unitarios",  x_pos, y_pos);
        y_pos+=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);
    }
    else if (owner_name == "l_filtrar_fecha") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *filtrar_fecha_importacion = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(filtrar_fecha_importacion, "filtrar_fecha_importacion",  x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *filtrar_fecha_cita = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(filtrar_fecha_cita, "filtrar_fecha_cita",  x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *filtrar_fecha_ejecucion = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(filtrar_fecha_ejecucion, "filtrar_fecha_ejecucion",  x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *filtrar_fecha_cierre = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(filtrar_fecha_cierre, "filtrar_fecha_cierre",  x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *filtrar_fecha_informe = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(filtrar_fecha_informe, "filtrar_fecha_informe",  x_pos, y_pos);
        y_pos+=space;
        MyLabelAnimated *filtrar_fecha_modificacion = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(filtrar_fecha_modificacion, "filtrar_fecha_modificacion",  x_pos, y_pos);
        y_pos+=space;
        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y(), 280, y_pos + y_pos_init);

    }
    else if (owner_name == "l_opciones_pdf") {
        int x_pos = 60;
        int space = 50;
        int y_pos_init = 20;
        int y_pos=y_pos_init;
        MyLabelAnimated *opciones_pdf_label_text = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(opciones_pdf_label_text, "opciones_pdf_label_text",  x_pos, y_pos);
        opciones_pdf_label_text->setEnabled(false);
        y_pos +=space;
        MyLabelAnimated *ver_pdf = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(ver_pdf, "ver_pdf", x_pos, y_pos);
        y_pos +=space;
        MyLabelAnimated *crear_pdf = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(crear_pdf, "crear_pdf", x_pos, y_pos);
        y_pos +=space;
        MyLabelAnimated *enviar_pdf = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(enviar_pdf, "enviar_pdf", x_pos, y_pos);
        y_pos +=space;
        MyLabelAnimated *imprimir_pdf = new MyLabelAnimated(this);
        setMyLabelAnimatedProperties(imprimir_pdf, "imprimir_pdf", x_pos, y_pos);
        y_pos +=space;

        QRect geometry = this->geometry();
        this->setGeometry(geometry.x(), geometry.y() - y_pos_init, 300, y_pos + y_pos_init);
    }

}

void MyWidgetMenuAnimated::setBackgroundImage(const QString image_filename)
{
    if(QFile::exists("icons/"+ image_filename +".png")){
        //this->setStyleSheet("border-image: url(:/icons/"+ image_filename +".png);");
        //backgroundImage = ":/icons/"+ image_filename +".png";
    }else {
        qDebug()<<"La imagen seteada no existe";
    }
}

void MyWidgetMenuAnimated::setAnimationParameters(int delay, int scale, int bounce, int time, QRect start, QRect end)
{
    Q_UNUSED(delay);
    anim_scale = scale;
    bounce_scale = bounce;
    anim_time = time;
    startValue = start;
    endValue = end;
    parametersSeted = true;
}

void MyWidgetMenuAnimated::hideChilds()
{
    this->hide();
    emit onHidden();
}


void MyWidgetMenuAnimated::setSizeInit()
{
    this->setGeometry(QRect(0,0, 10, 0));
}

void MyWidgetMenuAnimated::showWithAnimation()
{
    if(!animationOnGoing){
        if(this->isHidden()){
            this->setFixedSize(0,0);
            this->show();
        }
        this->raise();
        setChecked(true);
        if(parametersSeted){
            startAnimation(0, anim_scale, anim_time, startValue, endValue);
        }else {
            startAnimation(0);
        }
    }
}

void MyWidgetMenuAnimated::startAnimation(int delay, int scale, int anim_tim, QRect start, QRect end)
{
    Q_UNUSED(delay);
    //hacer un single shot con delay
    anim_time = anim_tim;
    anim_scale = scale;
    setAnimationInit(start, end);
    animationOnGoing = true;
    connect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    //    connect(animation, &QPropertyAnimation::finished, this, &MyWidgetMenuAnimated::setAnimationEnd);
}

void MyWidgetMenuAnimated::setAnimationInit(QRect start, QRect end)
{
    QString parentName = parentWidget()->objectName();
    if(start.isEmpty()){
        startValue = QRect(0,0, 0, parentWidget()->height());
    }else {
        startValue = start;
    }
    if(end.isEmpty()){
        endValue= QRect(startValue.x(),startValue.y() //posicion sinmodificar
                        ,startValue.width() + anim_scale + bounce_scale /*Ancho de icono de menu +50 (efecto rebote)*/
                        ,startValue.height()+ 0 /*El alto lo dejo como esta*/);
    }else {
        endValue = end;
    }
    midValue = QRect(startValue.x(),startValue.y() //posicion sinmodificar
                     ,startValue.width() + anim_scale /*Ancho de icono de menu*/
                     ,startValue.height()+ 0 /*El alto lo dejo como esta*/);

    this->setMaximumSize(startValue.width() + anim_scale + bounce_scale, startValue.height());
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(anim_time);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
}
void MyWidgetMenuAnimated::setAnimationEnd()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(setAnimationEnd()));
    delete animation;
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration((int)((float)anim_time/2));
    animation->setStartValue(endValue);
    animation->setEndValue(midValue);
    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}
void MyWidgetMenuAnimated::finalizadaAnimacion()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->stop();
    animationOnGoing = false;
    delete animation;
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(16777215);
    animation->setStartValue(midValue);
    animation->setEndValue(midValue);
    //    connect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacion()));
    animation->start(QPropertyAnimation::DeleteWhenStopped);
    //    this->setFixedSize(midValue.size());
}

void MyWidgetMenuAnimated::hideMenu(){
    if(animation != nullptr){
        animation->stop();
        delete animation;
        animation = nullptr;
        animationOnGoing = false;
    }
    this->hide();
    //    for (int i=0; i < children().size();i++) { //Esto no funciono
    //      //  if(children().at(i)->isWidgetType()){
    //            connect(this, SIGNAL(hideAll()), (MyLabelAnimated*)children().at(i), SLOT(hideChilds()));
    //       // }
    //    }
    //    emit hideAll();
}

void MyWidgetMenuAnimated::finalizadaAnimacionEnd()
{
    disconnect(animation, SIGNAL(finished()), this, SLOT(finalizadaAnimacionEnd()));
    animation->stop();
    animationOnGoing = false;
    delete animation;
    //Aqui puedo emitir que se mostro el menu, (con una demora singleshot por supuesto)
}

void MyWidgetMenuAnimated::resizeWidget(){

    qDebug()<< parent()->objectName() <<" Size After:  "<< QString::number(parentWidget()->size().width()) << ", " << QString::number(parentWidget()->size().height());
    float width_ratio = (float)parentWidget()->size().width()/1920;
    float height_ratio = (float)parentWidget()->size().height()/1080;
    int width_icon = 72;
    int height_icon = 1078;
    int width_resize = width_icon * width_ratio;
    int height_resize = height_icon * height_ratio;
    this->setFixedSize(width_resize, height_resize);
    //   this->setFixedSize(72, 1000);
    this->move(0,0);
}


void MyWidgetMenuAnimated::mousePressEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    emit mouseLeftClicked();
    if(!animationOnGoing){

    }
}

void MyWidgetMenuAnimated::setOnIcon()
{
    if(!isChecked()){
        setChecked(true);
    }else {
        setChecked(false);
    }
}

void MyWidgetMenuAnimated::setChecked(bool checked){
    if(checked){
        activated_state = true;
    }else{
        activated_state = false;
    }
}
bool MyWidgetMenuAnimated::isChecked(){
    return activated_state;
}
//void MyWidgetMenuAnimated::mouseMoveEvent(QMouseEvent *ev)
//{
//    emit mouseEntered(ev->pos());
//}
