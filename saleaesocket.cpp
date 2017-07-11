#include "saleaesocket.h"

saleaesocket::saleaesocket(QObject *parent) :
    QObject(parent)
{
}

saleaesocket::~saleaesocket(){
    this->socket->disconnectFromHost();
    qDebug() << "Disconnected...";
}

void saleaesocket::Connect()
{
    socket = new QTcpSocket(this);
    socket->connectToHost(this->host, this->port);
    if(!socket->waitForConnected(1000)) {
        qDebug() << "Could Not Connect!";
    }
    qDebug() << "Connected...";
    this->GetActiveDevices();
}


void saleaesocket::GetActiveDevices()
{
    QStringList Response = this->sendCommand(get_connected_devices_cmd).split('\n');
    devices.clear();
    foreach (QString line, Response) {

        QStringList elements = line.split(",",QString::SkipEmptyParts);
        if(!(elements.count()==4 || elements.count()==5)){
            qDebug() << "Device Not Valid!" << elements.join(" | ");
            continue;
        }

        int type = TypeStings.indexOf(elements.at(2).simplified());
        if(type == -1){
            qDebug() << "Device Type Not Valid!" << elements.at(2).simplified();
            return;
        }

        QString IdStr=elements.at(3).mid(2,16).simplified();

        ConnectedDevice device;
        device.Index = elements.at(0).toInt();
        device.Name = elements.at(1).simplified();
        device.Type = DeviceType(type);
        device.Id = IdStr.toULongLong();
        if(elements.count()==5 && elements.at(5)==QString("ACTIVE"))device.IsActive = true;
        else device.IsActive = false;
        devices.append(device);
    }
}

void saleaesocket::disconnected()
{
    qDebug() << "disconnected...";
}

void saleaesocket::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}


QString saleaesocket::sendCommand(QString Command)
{
    QByteArray Bytes = QString(Command).toUtf8() + '\0';
    socket->write(Bytes);
    if(!socket->waitForReadyRead(1000)) {
        qDebug() << "No Response!";
        return QString();
    }
    else return QString::fromUtf8(socket->readAll());
}


void saleaesocket::SelectActiveDevice(int device_number){
    QString CMD = select_active_device_cmd + ", " + QString::number(device_number);
    last_response = sendCommand(CMD);
}

void saleaesocket::GetActiveChannels(QList<int> digital_channels, QList<int> analog_channels){

}

void saleaesocket::SetActiveChannels(QList<int> digital_channels, QList<int> analog_channels){

}
