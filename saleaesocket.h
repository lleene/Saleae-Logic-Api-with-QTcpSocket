#ifndef SALEAESOCKET_H
#define SALEAESOCKET_H

#include <QTcpSocket>

//Command strings

const QString set_trigger_cmd = "SET_TRIGGER";
const QString set_num_samples_cmd = "SET_NUM_SAMPLES";
const QString get_sample_rate_cmd = "GET_SAMPLE_RATE";
const QString set_sample_rate_cmd = "SET_SAMPLE_RATE";

const QString set_capture_seconds_cmd = "SET_CAPTURE_SECONDS";
const QString capture_to_file_cmd = "CAPTURE_TO_FILE";
const QString save_to_file_cmd = "SAVE_TO_FILE";
const QString load_from_file_cmd = "LOAD_FROM_FILE";
const QString export_data_cmd = "EXPORT_DATA";
const QString export_data2_cmd = "EXPORT_DATA2";

const QString get_all_sample_rates_cmd = "GET_ALL_SAMPLE_RATES";
const QString get_analyzers_cmd = "GET_ANALYZERS";
const QString export_analyzer_cmd = "EXPORT_ANALYZER";
const QString get_inputs_cmd = "GET_INPUTS";
const QString capture_cmd = "CAPTURE";
const QString stop_capture_cmd = "STOP_CAPTURE";
const QString get_capture_pretrigger_buffer_size_cmd = "GET_CAPTURE_PRETRIGGER_BUFFER_SIZE";
const QString set_capture_pretrigger_buffer_size_cmd = "SET_CAPTURE_PRETRIGGER_BUFFER_SIZE";
const QString get_connected_devices_cmd = "GET_CONNECTED_DEVICES";
const QString select_active_device_cmd = "SELECT_ACTIVE_DEVICE";

const QString get_active_channels_cmd = "GET_ACTIVE_CHANNELS";
const QString set_active_channels_cmd = "SET_ACTIVE_CHANNELS";
const QString reset_active_channels_cmd = "RESET_ACTIVE_CHANNELS";

const QString set_performance_cmd = "SET_PERFORMANCE";
const QString get_performance_cmd = "GET_PERFORMANCE";
const QString is_processing_complete_cmd = "IS_PROCESSING_COMPLETE";
const QString is_analyzer_complete_cmd = "IS_ANALYZER_COMPLETE";

const QString set_digital_voltage_option_cmd = "SET_DIGITAL_VOLTAGE_OPTION";
const QString get_digital_voltage_options_cmd = "GET_DIGITAL_VOLTAGE_OPTIONS";

const QString close_all_tabs_cmd = "CLOSE_ALL_TABS";
const QStringList TypeStings = {"Logic","Logic8","Logic16","LogicPro8","LogicPro16"};

enum DeviceType {
    Logic,
    Logic8,
    Logic16,
    LogicPro8,
    LogicPro16
};

struct SampleRate {
public:
    int AnalogSampleRate;
    int DigitalSampleRate;
};

struct DigitalVoltageOption {
public:
    int Index;
    QString Description;
    bool IsActive;
};

struct ConnectedDevice {
public:
    DeviceType Type;
    QString Name;
    quint64 Id;
    int Index;
    bool IsActive;
};

class saleaesocket : public QObject
{
    Q_OBJECT

public:
    explicit saleaesocket(QObject *parent = nullptr);
    ~saleaesocket();

    int port;
    QString host;
    QString last_response;
    QList<ConnectedDevice> devices;
    QList<int> digital_channels;
    QList<int> analog_channels;

    void Connect();
    void Disconnect();
    void Capture();
    void StopCapture();
    void CloseAllTabs();
    void SetNumSamples(int num_samples);
    void SetCaptureSeconds(double num_seconds);

    QList<DigitalVoltageOption> GetDigitalVoltageOption();
    void SetDigitalVoltageOption(DigitalVoltageOption option);
    SampleRate GetSampleRate();
    void SetSampleRate(SampleRate rate);


    void GetActiveDevices();
    void SelectActiveDevice(int device_number);
    void ResetActiveChannels();
    void GetActiveChannels();
    void SetActiveChannels(QList<int> digital_channels, QList<int> analog_channels);

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readReady();

private:
    QTcpSocket *socket;
    QString sendCommand(QString Command);
};

#endif // SALEAESOCKET_H
