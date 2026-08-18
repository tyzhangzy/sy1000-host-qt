#pragma once

#include <QByteArray>
#include <QString>
#include <QVector>

class QSerialPort;

// Minimal Modbus RTU master over QSerialPort (cross-platform).
// Frame building/parsing and CRC16 are pure static functions (unit-testable
// without hardware). Serial I/O uses a simple synchronous request/response.
class ModbusRtuClient
{
public:
    ModbusRtuClient();
    ~ModbusRtuClient();

    bool connectPort(const QString &portName, int baudRate = 19200);
    void disconnectPort();
    bool isConnected() const;

    // Read holding registers (func 0x03).
    bool readHoldingRegisters(int slave, int startAddr, int count, QVector<quint16> &out);
    // Write a single coil (func 0x05).
    bool writeSingleCoil(int slave, int addr, bool on);

    // --- Pure frame helpers (unit-testable) ---
    static QByteArray buildReadHoldingRequest(int slave, int startAddr, int count);
    static QByteArray buildWriteCoilRequest(int slave, int addr, bool on);
    static bool parseReadHoldingResponse(const QByteArray &frame, QVector<quint16> &out);
    static quint16 crc16(const QByteArray &data);

private:
    bool sendFrame(const QByteArray &req, int respLen, QByteArray &resp);

    QSerialPort *m_port = nullptr;
};
