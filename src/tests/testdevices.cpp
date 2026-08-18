// Headless smoke test for the devices layer: verifies Modbus RTU frame
// construction and CRC16 against known vectors (no hardware needed).
#include <QCoreApplication>
#include <cstdio>

#include "devices/modbusrtu.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Known vector: read 2 holding registers from slave 1 at addr 0.
    // Frame: 01 03 00 00 00 02 C4 0B
    const QByteArray req = ModbusRtuClient::buildReadHoldingRequest(1, 0, 2);
    const QByteArray expect = QByteArray::fromHex("010300000002c40b");
    const bool frameOk = (req == expect);

    // CRC of the 6-byte PDU (01 03 00 00 00 02) = 0x0BC4
    const quint16 crc = ModbusRtuClient::crc16(QByteArray::fromHex("010300000002"));
    const bool crcOk = (crc == 0x0BC4);

    // write coil frame: slave1 coil addr5 on -> 01 05 00 05 FF 00 9C 3B
    const QByteArray coil = ModbusRtuClient::buildWriteCoilRequest(1, 5, true);
    const bool coilOk = (coil == QByteArray::fromHex("01050005ff009c3b"));

    // parse a read-holding response: 01 03 04 00 0B 00 0C ... (registers 0x000B, 0x000C)
    // build a valid response with correct CRC
    QByteArray resp = QByteArray::fromHex("010304000b000c");
    quint16 rc = ModbusRtuClient::crc16(resp);
    resp.append(static_cast<char>(rc & 0xFF));
    resp.append(static_cast<char>((rc >> 8) & 0xFF));
    QVector<quint16> regs;
    const bool parseOk = ModbusRtuClient::parseReadHoldingResponse(resp, regs) &&
                         regs.size() == 2 && regs.at(0) == 11 && regs.at(1) == 12;

    std::printf("read frame: %s (expect %s)\n", frameOk ? "OK" : "FAIL", req.toHex().constData());
    std::printf("crc16: %s (0x%04X)\n", crcOk ? "OK" : "FAIL", crc);
    std::printf("coil frame: %s\n", coilOk ? "OK" : "FAIL");
    std::printf("parse response: %s (regs=%d,%d)\n", parseOk ? "OK" : "FAIL", regs.value(0), regs.value(1));

    const bool allOk = frameOk && crcOk && coilOk && parseOk;
    std::printf(allOk ? "DEVICES SMOKE PASS\n" : "DEVICES SMOKE FAIL\n");
    return allOk ? 0 : 1;
}
