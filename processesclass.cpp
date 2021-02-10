#include "processesclass.h"

ProcessesClass::ProcessesClass()
{

}

QString ProcessesClass::executeProcess(QObject *parent, QString process, OPtype so,
                                       int waitTime, bool close_on_finished)
{
    QString response="";

    if(so == LINUX){
        QProcess *proc_ovpn = new QProcess(parent);
        proc_ovpn->setProcessChannelMode(QProcess::MergedChannels);

        proc_ovpn->start("sh",QStringList() << "-c" << process);

        if(!proc_ovpn->waitForStarted()) //default wait time 30 sec
            qDebug() << " cannot start process ";

        if (!proc_ovpn->waitForFinished(waitTime))
            qDebug() << "timeout .. ";

        response = proc_ovpn->readAllStandardOutput();

        if(close_on_finished){
            proc_ovpn->close();
        }

    }else if(so == WINDOWS){

        QProcess *proc_ovpn = new QProcess(parent);
        proc_ovpn->setProcessChannelMode(QProcess::MergedChannels);
        proc_ovpn->start("C:/windows/system32/cmd.exe",QStringList()<<"/C"<<process);
        if(!proc_ovpn->waitForStarted()) //default wait time 30 sec
            qDebug() << " cannot start process ";

        if (!proc_ovpn->waitForFinished(waitTime))
            qDebug() << "timeout .. ";

        response = proc_ovpn->readAllStandardOutput();
        if(close_on_finished){
            proc_ovpn->close();
        }
    }
    return response;
}
