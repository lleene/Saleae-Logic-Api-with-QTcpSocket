#ifndef SALEAESOCKET_H
#define SALEAESOCKET_H

#include <QTcpSocket>

//#include <QObject>
//#include <QString>
//#include <QStringList>

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

//channels
const QString all_channels_option = "ALL_CHANNELS";
const QString specific_channels_option = "SPECIFIC_CHANNELS";
const QString digital_channels_option = "DIGITAL_CHANNELS";
const QString analog_channels_option = "ANALOG_CHANNELS";

//time span
const QString all_time_option = "ALL_TIME";
const QString time_span_option = "TIME_SPAN";

const QString csv_option = "CSV";
const QStringList CsvHeadersTypeStrings = {"HEADERS","NO_HEADERS"};
const QStringList CsvDelimiterTypeStrings = {"TAB","COMMA"};
const QStringList CsvTimestampTypeStrings = {"SAMPLE_NUMBER","TIME_STAMP"};
const QStringList CsvOutputModeStrings = {"COMBINED","SEPARATE"};
const QStringList CsvDensityStrings = {"ROW_PER_CHANGE","ROW_PER_SAMPLE"};
const QStringList CsvBaseStrings = {"DEC","HEX","BIN","ASCII"};

const QString binary_option = "BINARY";
const QStringList BinaryOutputModeStrings  = {"EACH_SAMPLE","ON_CHANGE"};
const QStringList BinaryOutputWordSizeStrings   = {"8","16","32","64"};

const QStringList AnalogFormatStrings = {"VOLTAGE","ADC"};
const QString vcd_option = "VCD";
const QString matlab_option = "MATLAB";

const QStringList TypeStings = {"Logic","Logic8","Logic16","LogicPro8","LogicPro16"};
enum DeviceType {
    Logic,
    Logic8,
    Logic16,
    LogicPro8,
    LogicPro16
};

const QStringList PerfStings = {"100","80","60","40","20"};
enum PerformanceOption {
    OneHundredPercent,
    EightyPercent,
    SixtyPercent,
    FortyPercent,
    TwentyPercent
};

const QStringList TriggerStings = {"None","High","Low","FallingEdge","RisingEdge","NegativePulse","PositivePulse"};
enum Trigger {
        None,
        High,
        Low,
        FallingEdge,
        RisingEdge,
        NegativePulse,
        PositivePulse
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

enum DataExportChannelSelection{
    AllChannels,
    SpecificChannels
};

const QStringList ExportModeStrings = {"DIGITAL_ONLY", "ANALOG_ONLY", "ANALOG_AND_DIGITAL"};
enum DataExportMixedModeExportType{
    DigitalOnly,
    AnalogOnly,
    AnalogAndDigital
};

enum DataExportSampleRangeType {
    RangeAll,
    RangeTimes
};

enum DataExportType {
    ExportBinary,
    ExportCsv,
    ExportVcd,
    ExportMatlab
};

enum CsvHeadersType {
    CsvIncludesHeaders,
    CsvNoHeaders
};

enum CsvDelimiterType {
    CsvComma,
    CsvTab
};

enum CsvOutputMode {
    CsvSingleNumber,
    CsvOneColumnPerBit
};

enum CsvTimestampType {
    CsvTime,
    CsvSample
};

enum CsvBase {
    CsvBinary,
    CsvDecimal,
    CsvHexadecimal,
    CsvAscii
};

enum CsvDensity {
    CsvTransition,
    CsvComplete
};

enum BinaryOutputMode {
    BinaryEverySample,
    BinaryEveryChange
};

const QStringList BinaryBitShiftingStrings = {"NO_SHIFT", "RIGHT_SHIFT"};
enum BinaryBitShifting {
    BinaryOriginalBitPositions,
    BinaryShiftRight
};

enum BinaryOutputWordSize {
    Binary8Bit,
    Binary16Bit,
    Binary32Bit,
    Binary64Bit
};

enum AnalogOutputFormat {
    Voltage,
    ADC
};

struct ExportDataStruct {
public:
    QString FileName;
    DataExportMixedModeExportType DataExportMixedExportMode;
    DataExportChannelSelection ExportChannelSelection;
    QList<int> DigitalChannelsToExport;
    QList<int> AnalogChannelsToExport;
    DataExportSampleRangeType SamplesRangeType;
    double StartingTime;
    double EndingTime;
    DataExportType DataExportType;
    CsvHeadersType CsvIncludeHeaders;
    CsvDelimiterType CsvDelimiterType;
    CsvOutputMode CsvOutputMode;
    CsvTimestampType CsvTimestampType;
    CsvBase CsvDisplayBase;
    CsvDensity CsvDensity;
    BinaryOutputMode BinaryOutputMode;
    BinaryBitShifting BinaryBitShifting;
    BinaryOutputWordSize BinaryOutputWordSize;
    AnalogOutputFormat AnalogFormat;
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
    int GetCapturePretriggerBufferSize();
    void SetCapturePretriggerBufferSize(int buffer_size);
    void CloseAllTabs();
    void SetNumSamples(int num_samples);
    void SetCaptureSeconds(double num_seconds);
    bool IsProcessingComplete();

    void SetTrigger(QList<Trigger> triggers, double minimum_pulse_width, double maximum_pulse_width);
    QList<DigitalVoltageOption> GetDigitalVoltageOption();
    void SetDigitalVoltageOption(DigitalVoltageOption option);
    SampleRate GetSampleRate();
    void SetSampleRate(SampleRate rate);
    void CaptureToFile(QString file);
    void SaveToFile(QString file);
    void LoadFromFile(QString file);
    void SetPerformanceOption(PerformanceOption performance);
    PerformanceOption GetPerformanceOption();

    void GetActiveDevices();
    void SelectActiveDevice(int device_number);
    void ResetActiveChannels();
    void GetActiveChannels();
    void SetActiveChannels(QList<int> digital_channels, QList<int> analog_channels);

    void ExportData(ExportDataStruct export_data_struct);
    void ExportData2(ExportDataStruct export_data_struct, bool capture_digital, bool capture_analog);

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
