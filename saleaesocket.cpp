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

int saleaesocket::GetCapturePretriggerBufferSize(){
    QStringList Response = sendCommand(get_sample_rate_cmd).simplified().split('\n');
    int buffer_size = Response.at(0).toInt();
    return buffer_size;
}

void saleaesocket::SetCapturePretriggerBufferSize(int buffer_size){
    QString CMD = capture_cmd + ", " + QString::number(buffer_size);
    sendCommand(CMD);
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

bool saleaesocket::IsProcessingComplete(){
    QStringList Response = sendCommand(is_processing_complete_cmd).split('\n');
    bool complete = (Response.at(0) == QString("1"));
    return complete;
}

void saleaesocket::SetTrigger(QList<Trigger> triggers, double minimum_pulse_width, double maximum_pulse_width){
    int events = triggers.count(Trigger::FallingEdge) + triggers.count(Trigger::RisingEdge);
    events += triggers.count(Trigger::NegativePulse) + triggers.count(Trigger::PositivePulse);
    if( events > 1){
        qDebug() << "Invalid Trigger";
        return;
    }
    if( triggers.count() != digital_channels.count()){
        qDebug() << "Invalid Number of Triggers";
        return;
    }
    QString CMD = set_trigger_cmd;
    foreach (Trigger specification, triggers) {
        CMD += ", " + TriggerStings.at(specification);
        if(specification == Trigger::NegativePulse || specification == Trigger::PositivePulse){
            CMD += ", " + QString::number(minimum_pulse_width);
            if(maximum_pulse_width > 0) CMD += ", " + QString::number(maximum_pulse_width);
        }
    }
    sendCommand(CMD);
}

void saleaesocket::ExportData(ExportDataStruct export_data_struct){
    QString CMD = export_data_cmd + ", " + export_data_struct.FileName;
    if(export_data_struct.ExportChannelSelection == DataExportChannelSelection::AllChannels){
        CMD += ", " + all_channels_option;
        if(!export_data_struct.AnalogChannelsToExport.isEmpty()){
            CMD += ", " + AnalogFormatStrings.at(export_data_struct.AnalogFormat);
        }

    }
    else {
        CMD += ", " + specific_channels_option;
        if(!export_data_struct.DigitalChannelsToExport.isEmpty()){
            CMD += ", " + digital_channels_option;
            foreach(int channel, export_data_struct.DigitalChannelsToExport)
                CMD += ", " + QString::number(channel);
        }
        if(!export_data_struct.AnalogChannelsToExport.isEmpty()){
            CMD += ", " + analog_channels_option;
            foreach(int channel, export_data_struct.AnalogChannelsToExport)
                CMD += ", " + QString::number(channel);
            CMD += ", " + AnalogFormatStrings.at(export_data_struct.AnalogFormat);
        }
    }
    if(export_data_struct.SamplesRangeType == DataExportSampleRangeType::RangeAll)
        CMD += ", " + all_time_option;
    else if ( export_data_struct.SamplesRangeType == DataExportSampleRangeType::RangeTimes ) {
        CMD += ", " + time_span_option + ", " + QString::number(export_data_struct.StartingTime);
        CMD += ", " + QString::number(export_data_struct.EndingTime);
    }
    if( export_data_struct.DataExportType == DataExportType::ExportCsv){
        CMD += ", " + csv_option;
        CMD += ", " + CsvHeadersTypeStrings.at(export_data_struct.CsvIncludeHeaders);
        CMD += ", " + CsvDelimiterTypeStrings.at(export_data_struct.CsvDelimiterType);
        CMD += ", " + CsvTimestampTypeStrings.at(export_data_struct.CsvTimestampType);
        CMD += ", " + CsvOutputModeStrings.at(export_data_struct.CsvOutputMode);
        CMD += ", " + CsvDensityStrings.at(export_data_struct.CsvDensity);
        CMD += ", " + CsvBaseStrings.at(export_data_struct.CsvDisplayBase);
    }
    else if( export_data_struct.DataExportType == DataExportType::ExportBinary ){
        CMD += ", " + binary_option;
        CMD += ", " + BinaryOutputModeStrings.at(export_data_struct.BinaryOutputMode);
        CMD += ", " + BinaryOutputWordSizeStrings.at(export_data_struct.BinaryOutputWordSize);
    }
    else if( export_data_struct.DataExportType == DataExportType::ExportVcd )
        CMD += ", " + vcd_option;
    else if( export_data_struct.DataExportType == DataExportType::ExportMatlab )
        CMD += ", " + matlab_option;

    sendCommand(CMD);
}

void saleaesocket::ExportData2(ExportDataStruct export_data_struct, bool capture_digital, bool capture_analog){
    bool is_mixed_mode_capture = true;
    QString CMD = export_data2_cmd + ", " + export_data_struct.FileName;

    if(export_data_struct.ExportChannelSelection == DataExportChannelSelection::AllChannels){
        CMD += ", " + all_channels_option;
        if(!export_data_struct.AnalogChannelsToExport.isEmpty()){
            CMD += ", " + AnalogFormatStrings.at(export_data_struct.AnalogFormat);
        }
    }
    else {
        CMD += ", " + specific_channels_option;
        if(is_mixed_mode_capture){
            CMD += ", " + ExportModeStrings.at(export_data_struct.DataExportMixedExportMode);
        }
        if(!export_data_struct.DigitalChannelsToExport.isEmpty()){
            CMD += ", " + digital_channels_option;
            foreach(int channel, export_data_struct.DigitalChannelsToExport)
                CMD += ", " + QString::number(channel) + " " + QString("DIGITAL") ;
        }
        if(!export_data_struct.AnalogChannelsToExport.isEmpty()){
            CMD += ", " + analog_channels_option;
            foreach(int channel, export_data_struct.AnalogChannelsToExport)
                CMD += ", " + QString::number(channel) + " " + QString("ANALOG");
            CMD += ", " + AnalogFormatStrings.at(export_data_struct.AnalogFormat);
        }
    }

    if(export_data_struct.SamplesRangeType == DataExportSampleRangeType::RangeAll)
        CMD += ", " + all_time_option;
    else if ( export_data_struct.SamplesRangeType == DataExportSampleRangeType::RangeTimes ) {
        CMD += ", " + time_span_option + ", " + QString::number(export_data_struct.StartingTime);
        CMD += ", " + QString::number(export_data_struct.EndingTime);
    }

    if( export_data_struct.DataExportType == DataExportType::ExportCsv){
        CMD += ", " + csv_option;
        if(capture_digital && !is_mixed_mode_capture){
            CMD += ", " + CsvHeadersTypeStrings.at(export_data_struct.CsvIncludeHeaders);
            CMD += ", " + CsvDelimiterTypeStrings.at(export_data_struct.CsvDelimiterType);
            CMD += ", " + CsvTimestampTypeStrings.at(export_data_struct.CsvTimestampType);
            CMD += ", " + CsvOutputModeStrings.at(export_data_struct.CsvOutputMode);
            if(export_data_struct.CsvOutputMode == CsvOutputMode::CsvSingleNumber)
                CMD += ", " + CsvBaseStrings.at(export_data_struct.CsvDisplayBase);
            CMD += ", " + CsvDensityStrings.at(export_data_struct.CsvDensity);
        }
        else {
            CMD += ", " + CsvHeadersTypeStrings.at(export_data_struct.CsvIncludeHeaders);
            CMD += ", " + CsvDelimiterTypeStrings.at(export_data_struct.CsvDelimiterType);
            CMD += ", " + CsvBaseStrings.at(export_data_struct.CsvDisplayBase);
            CMD += ", " + AnalogFormatStrings.at(export_data_struct.AnalogFormat);
        }
    }
    else if( export_data_struct.DataExportType == DataExportType::ExportBinary ){
        if(capture_digital && !is_mixed_mode_capture){
            CMD += ", " + binary_option;
            CMD += ", " + BinaryOutputModeStrings.at(export_data_struct.BinaryOutputMode);
            CMD += ", " + BinaryBitShiftingStrings.at(export_data_struct.BinaryBitShifting);
            CMD += ", " + BinaryOutputWordSizeStrings.at(export_data_struct.BinaryOutputWordSize);
        }
        else {
            CMD += ", " + binary_option;
            CMD += ", " + AnalogFormatStrings.at(export_data_struct.AnalogFormat);
        }
    }
    else if( export_data_struct.DataExportType == DataExportType::ExportVcd )
        CMD += ", " + vcd_option;
    else if( export_data_struct.DataExportType == DataExportType::ExportMatlab ){
        CMD += ", " + matlab_option;
        if( capture_analog || is_mixed_mode_capture)
            CMD += ", " + AnalogFormatStrings.at(export_data_struct.AnalogFormat);
    }

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

void saleaesocket::CaptureToFile(QString file){
    QString CMD = capture_to_file_cmd + ", " + file;
    sendCommand(CMD);
}

void saleaesocket::SaveToFile(QString file){
    QString CMD = save_to_file_cmd + ", " + file;
    sendCommand(CMD);
}

void saleaesocket::LoadFromFile(QString file){
    QString CMD = load_from_file_cmd + ", " + file;
    sendCommand(CMD);
}

void saleaesocket::SetPerformanceOption(PerformanceOption performance){
    QString CMD = set_performance_cmd + ", " + PerfStings.at(performance);
    sendCommand(CMD);
}

PerformanceOption saleaesocket::GetPerformanceOption(){
    QStringList Response = sendCommand(get_performance_cmd).split('\n');
    int type = PerfStings.indexOf(Response.at(0).simplified());
    if(type == -1){
        qDebug() << "Performance Type Not Valid!";
        return PerformanceOption(0);
    }
    return PerformanceOption(type);
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
        if(term==digital_channels_option) { // change here from string
            DC=1;
            continue;
        }
        if(term==analog_channels_option) {
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
        CMD += ", " + digital_channels_option; // change here from string
        foreach(int channel, digital_channels){
            CMD += ", " + QString::number(channel);
        }
    }
    if(!alg_channels.empty()){
        CMD += ", " + analog_channels_option;
        foreach(int channel, analog_channels){
            CMD += ", " + QString::number(channel);
        }
    }

    QString Response = sendCommand(CMD);
    if(Response.simplified()== QString("ACK")){
        digital_channels=dig_channels;
        analog_channels=alg_channels;
    }
}
