#include "saleaesocket.h"

saleaesocket::saleaesocket(QObject *parent) :
    QObject(parent)
{
    port=10429;
    host=QString("localhost");
    last_response = QString();
    devices = QList<ConnectedDevice>();
    digital_channels = QList<int>();
    analog_channels =QList<int>();
}

saleaesocket::~saleaesocket(){
    if(socket->isOpen())socket->disconnectFromHost();
    qDebug() << "Stopped & Disconnected...";
}

void saleaesocket::connected(){
    qDebug() << "connected...";
}
void saleaesocket::disconnected(){
    qDebug() << "disconnected...";
}
void saleaesocket::readReady(){
    qDebug() << "bytes recieved...";
}
void saleaesocket::bytesWritten(qint64 bytes){
    qDebug() << bytes << "bytes written...";
}

void saleaesocket::Connect()
{
    socket = new QTcpSocket(this);
    socket->connectToHost(host, port);
    if(!socket->waitForConnected(1000)) {
        qDebug() << "Could Not Connect!";
    }
    qDebug() << "Connected...";
    GetActiveDevices();
}

void saleaesocket::Disconnect()
{
    socket->disconnectFromHost();
    qDebug() << "Disconnected...";
}

QString saleaesocket::sendCommand(QString Command){
    QByteArray Bytes = QString(Command).toUtf8() + '\0';
    socket->write(Bytes);
    if(!socket->waitForReadyRead(1000)) {
        qDebug() << "No Response!";
        return QString();
    }
    else {
        last_response = QString::fromUtf8(socket->readAll());
        if(last_response.endsWith("NAK")) qDebug() << "Error";
        return last_response;
    }
}

void saleaesocket::Capture(){
    sendCommand(capture_cmd);
}

void saleaesocket::StopCapture(){
    sendCommand(stop_capture_cmd);
}

void saleaesocket::CloseAllTabs(){
    sendCommand(close_all_tabs_cmd);
}

void saleaesocket::SetNumSamples(int num_samples){
    QString CMD = capture_cmd + ", " + QString::number(num_samples);
    sendCommand(CMD);
}

void saleaesocket::SetCaptureSeconds(double num_seconds){
    QString CMD = capture_cmd + ", " + QString::number(num_seconds);
    sendCommand(CMD);
}

QList<DigitalVoltageOption> saleaesocket::GetDigitalVoltageOption(){
    QStringList Response = sendCommand(get_digital_voltage_options_cmd).simplified().split('\n');
    QList<DigitalVoltageOption> options={};
    foreach(QString line,Response){
        QStringList elements = line.simplified().split(',');
        DigitalVoltageOption option;
        option.Index = elements.at(0).toInt();
        option.Description = elements.at(1);
        if(elements.at(2)==QString("SELECTED")) option.IsActive=true;
        else option.IsActive=false;
        options.append(option);
    }
    return options;
}

void saleaesocket::SetDigitalVoltageOption(DigitalVoltageOption option){
    QString CMD = set_digital_voltage_option_cmd + ", " + QString::number(option.Index);
    sendCommand(CMD);
}

SampleRate saleaesocket::GetSampleRate(){
    QStringList Response = sendCommand(get_sample_rate_cmd).simplified().split('\n');
    Response.removeLast();
    if(Response.count() != 2) qDebug() << "Unexpected Value";
    SampleRate rate;
    rate.DigitalSampleRate = Response.at(0).toInt();
    rate.AnalogSampleRate = Response.at(1).toInt();
    return rate;
}

void saleaesocket::SetSampleRate(SampleRate rate){
    QString CMD = set_sample_rate_cmd  + ", " + QString::number(rate.DigitalSampleRate);
    CMD += ", " + QString::number(rate.AnalogSampleRate);
    sendCommand(CMD);
}

void saleaesocket::GetActiveDevices(){
    QStringList Response = sendCommand(get_connected_devices_cmd).split('\n');
    Response.removeLast();
    devices.clear();

    foreach (QString line, Response) {
        QStringList elements = line.split(",",QString::SkipEmptyParts);
        if(!(elements.count()==4 || elements.count()==5)){
            qDebug() << "Device Not Valid!";
            continue;
        }

        int type = TypeStings.indexOf(elements.at(1).simplified());
        if(type == -1){
            qDebug() << "Device Type Not Valid!";
            return;
        }

        QString IdStr=elements.at(3).mid(2,16).simplified();
        ConnectedDevice device;
        device.Index = elements.at(0).toInt();
        device.Name = elements.at(1).simplified();
        device.Type = DeviceType(type);
        device.Id = IdStr.toULongLong();

        if(elements.count()==5)device.IsActive = true;
        else device.IsActive = false;
        devices.append(device);
    }
}

void saleaesocket::SelectActiveDevice(int device_number){
    QString CMD = select_active_device_cmd + ", " + QString::number(device_number);
    sendCommand(CMD);
}

void saleaesocket::ResetActiveChannels(){
    sendCommand(reset_active_channels_cmd);
}

void saleaesocket::GetActiveChannels(){
    QStringList Response = sendCommand(get_active_channels_cmd).split('\n');
    Response = Response.at(0).simplified().split(',');
    digital_channels = {};
    analog_channels = {};
    int DC = 0;
    foreach(QString term, Response){
        if(term==QString("digital_channels")) {
            DC=1;
            continue;
        }
        if(term==QString("analog_channels")) {
            DC=-1;
            continue;
        }
        switch (DC){
            case 1:
                digital_channels.append(term.toInt());
                break;
            case -1:
                analog_channels.append(term.toInt());
                break;
            default:
                qDebug() << "Channel Has No Identifier!";
                break;
        }
    }
}

void saleaesocket::SetActiveChannels(QList<int> dig_channels, QList<int> alg_channels){
    QString CMD = set_active_channels_cmd;
    if(!dig_channels.empty()){
        CMD.append(", digital_channels");
        foreach(int channel, digital_channels){
            CMD.append(", " + QString::number(channel));
        }
    }
    if(!alg_channels.empty()){
        CMD.append(", analog_channels");
        foreach(int channel, analog_channels){
            CMD.append(", " + QString::number(channel));
        }
    }

    QString Response = sendCommand(CMD);
    if(Response.simplified()== QString("ACK")){
        digital_channels=dig_channels;
        analog_channels=alg_channels;
    }
}
