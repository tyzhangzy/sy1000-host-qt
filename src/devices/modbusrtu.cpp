#include "devices/modbusrtu.h"

#include <QSerialPort>
#include <QtEndian>

namespace {
quint16 crc16Loop(const QByteArray &data)
{
    quint16 crc = 0xFFFF;
    for (const char c : data) {
        crc ^= static_cast<quint8>(c);
        for (int i = 0; i < 8; ++i) {
            if (crc & 0x0001)
                crc = static_cast<quint16>((crc >> 1) ^ 0xA001);
            else
                crc >>= 1;
        }
    }
    return crc;
}
} // namespace

ModbusRtuClient::ModbusRtuClient()
    : m_port(new QSerialPort)
{
}

ModbusRtuClient::~ModbusRtuClient()
{
    disconnectPort();
    delete m_port;
}

quint16 ModbusRtuClient::crc16(const QByteArray &data)
{
    return crc16Loop(data);
}

QByteArray ModbusRtuClient::buildReadHoldingRequest(int slave, int startAddr, int count)
{
    QByteArray frame;
    frame.append(static_cast<char>(slave & 0xFF));
    frame.append(static_cast<char>(0x03));
    frame.append(static_cast<char>((startAddr >> 8) & 0xFF));
    frame.append(static_cast<char>(startAddr & 0xFF));
    frame.append(static_cast<char>((count >> 8) & 0xFF));
    frame.append(static_cast<char>(count & 0xFF));
    const quint16 crc = crc16(frame);
    frame.append(static_cast<char>(crc & 0xFF));       // low byte first
    frame.append(static_cast<char>((crc >> 8) & 0xFF));
    return frame;
}

QByteArray ModbusRtuClient::buildWriteCoilRequest(int slave, int addr, bool on)
{
    QByteArray frame;
    frame.append(static_cast<char>(slave & 0xFF));
    frame.append(static_cast<char>(0x05));
    frame.append(static_cast<char>((addr >> 8) & 0xFF));
    frame.append(static_cast<char>(addr & 0xFF));
    frame.append(static_cast<char>(on ? 0xFF : 0x00));
    frame.append(static_cast<char>(0x00));
    const quint16 crc = crc16(frame);
    frame.append(static_cast<char>(crc & 0xFF));
    frame.append(static_cast<char>((crc >> 8) & 0xFF));
    return frame;
}

bool ModbusRtuClient::parseReadHoldingResponse(const QByteArray &frame, QVector<quint16> &out)
{
    out.clear();
    // frame: slave(1) func(1) byteCount(1) data(n) crc(2)
    if (frame.size() < 5)
        return false;
    const int byteCount = static_cast<quint8>(frame.at(2));
    if (frame.size() != 3 + byteCount + 2)
        return false;

    // verify crc
    const QByteArray payload = frame.left(frame.size() - 2);
    const quint16 crc = crc16(payload);
    const quint16 recv = static_cast<quint8>(frame.at(frame.size() - 2)) |
                         (static_cast<quint8>(frame.at(frame.size() - 1)) << 8);
    if (crc != recv)
        return false;

    for (int i = 0; i < byteCount; i += 2) {
        const quint16 val = static_cast<quint8>(frame.at(3 + i)) << 8 |
                            static_cast<quint8>(frame.at(4 + i));
        out.append(val);
    }
    return true;
}

bool ModbusRtuClient::connectPort(const QString &portName, int baudRate)
{
    disconnectPort();
    m_port->setPortName(portName);
    m_port->setBaudRate(baudRate);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);
    m_port->setReadBufferSize(0);
    return m_port->open(QIODevice::ReadWrite);
}

void ModbusRtuClient::disconnectPort()
{
    if (m_port->isOpen())
        m_port->close();
}

bool ModbusRtuClient::isConnected() const
{
    return m_port->isOpen();
}

bool ModbusRtuClient::sendFrame(const QByteArray &req, int respLen, QByteArray &resp)
{
    if (!m_port->isOpen())
        return false;
    m_port->clear();
    if (m_port->write(req) != req.size())
        return false;
    if (!m_port->waitForBytesWritten(200))
        return false;

    resp.clear();
    while (resp.size() < respLen) {
        if (!m_port->waitForReadyRead(200))
            return false;
        resp += m_port->read(respLen - resp.size());
    }
    return true;
}

bool ModbusRtuClient::readHoldingRegisters(int slave, int startAddr, int count, QVector<quint16> &out)
{
    const QByteArray req = buildReadHoldingRequest(slave, startAddr, count);
    QByteArray resp;
    const int respLen = 3 + count * 2 + 2;
    if (!sendFrame(req, respLen, resp))
        return false;
    return parseReadHoldingResponse(resp, out);
}

bool ModbusRtuClient::writeSingleCoil(int slave, int addr, bool on)
{
    const QByteArray req = buildWriteCoilRequest(slave, addr, on);
    QByteArray resp;
    if (!sendFrame(req, 8, resp))
        return false;
    // echo should match the request
    return resp == req;
}
