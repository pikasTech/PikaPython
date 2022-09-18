#include "_modbus__ModBus.h"
#include "_modbus__ModBusRTU.h"
#include "_modbus__ModBusTCP.h"
#include "agile_modbus.h"

void _modbus__ModBusRTU___init__(PikaObj* self,
                                 int sendBUffSize,
                                 int readBuffSize) {
    agile_modbus_rtu_t ctx_rtu = {0};
    agile_modbus_t* ctx = &ctx_rtu._ctx;
    obj_setBytes(self, "sendBuff", NULL, sendBUffSize);
    obj_setBytes(self, "readBuff", NULL, readBuffSize);
    agile_modbus_rtu_init(&ctx_rtu, obj_getBytes(self, "sendBuff"),
                          sendBUffSize, obj_getBytes(self, "readBuff"),
                          readBuffSize);
    obj_setStruct(self, "ctx_rtu", ctx_rtu);
    obj_setPtr(self, "ctx", ctx);
}

void _modbus__ModBus_setSlave(PikaObj* self, int slave) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    agile_modbus_set_slave(ctx, slave);
}

void _modbus__ModBusTCP___init__(PikaObj* self,
                                 int sendBuffSize,
                                 int readBuffSize) {
    agile_modbus_tcp_t ctx_tcp = {0};
    agile_modbus_t* ctx = &ctx_tcp._ctx;
    obj_setBytes(self, "sendBuff", NULL, sendBuffSize);
    obj_setBytes(self, "readBuff", NULL, readBuffSize);
    agile_modbus_tcp_init(&ctx_tcp, obj_getBytes(self, "sendBuff"),
                          sendBuffSize, obj_getBytes(self, "readBuff"),
                          readBuffSize);
    obj_setStruct(self, "ctx_tcp", ctx_tcp);
    obj_setPtr(self, "ctx", ctx);
}

int _modbus__ModBus_deserializeMaskWriteRegister(PikaObj* self, int msgLength) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_mask_write_register(ctx, msgLength);
}

int _modbus__ModBus_deserializeReadBits(PikaObj* self,
                                        int msgLength,
                                        uint8_t* dest) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_read_bits(ctx, msgLength, dest);
}

int _modbus__ModBus_deserializeReadInputBits(PikaObj* self,
                                             int msgLength,
                                             uint8_t* dest) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_read_input_bits(ctx, msgLength, dest);
}

int _modbus__ModBus_deserializeReadInputRegisters(PikaObj* self,
                                                  int msgLength,
                                                  uint8_t* dest) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_read_input_registers(ctx, msgLength,
                                                         (uint16_t*)dest);
}

int _modbus__ModBus_deserializeReadRegisters(PikaObj* self,
                                             int msgLength,
                                             uint8_t* dest) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_read_registers(ctx, msgLength,
                                                   (uint16_t*)dest);
}

int _modbus__ModBus_deserializeReportSlaveId(PikaObj* self,
                                             int msgLength,
                                             int maxDest,
                                             uint8_t* dest) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_report_slave_id(ctx, msgLength, maxDest,
                                                    dest);
}

int _modbus__ModBus_deserializeWriteAndReadRegisters(PikaObj* self,
                                                     int msgLength,
                                                     uint8_t* dest) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_write_and_read_registers(ctx, msgLength,
                                                             (uint16_t*)dest);
}

int _modbus__ModBus_deserializeWriteBit(PikaObj* self, int msgLength) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_write_bit(ctx, msgLength);
}

int _modbus__ModBus_deserializeWriteBits(PikaObj* self, int msgLength) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_write_bits(ctx, msgLength);
}

int _modbus__ModBus_deserializeWriteRegister(PikaObj* self, int msgLength) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_write_register(ctx, msgLength);
}

int _modbus__ModBus_deserializeWriteRegisters(PikaObj* self, int msgLength) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_write_registers(ctx, msgLength);
}

int _modbus__ModBus_serializeMaskWriteRegister(PikaObj* self,
                                               int addr,
                                               int andMask,
                                               int orMask) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_mask_write_register(ctx, addr, andMask,
                                                      orMask);
}

int _modbus__ModBus_serializeReadBits(PikaObj* self, int addr, int nb) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_read_bits(ctx, addr, nb);
}

int _modbus__ModBus_serializeReadInputBits(PikaObj* self, int addr, int nb) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_read_input_bits(ctx, addr, nb);
}

int _modbus__ModBus_serializeReadInputRegisters(PikaObj* self,
                                                int addr,
                                                int nb) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_read_input_registers(ctx, addr, nb);
}

int _modbus__ModBus_serializeReadRegisters(PikaObj* self, int addr, int nb) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_read_registers(ctx, addr, nb);
}

int _modbus__ModBus_serializeReportSlaveId(PikaObj* self) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_report_slave_id(ctx);
}

int _modbus__ModBus_serializeWriteAndReadRegisters(PikaObj* self,
                                                   int writeAddr,
                                                   int writeNb,
                                                   uint8_t* src,
                                                   int readAddr,
                                                   int readNb) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_write_and_read_registers(
        ctx, writeAddr, writeNb, (uint16_t*)src, readAddr, readNb);
}

int _modbus__ModBus_serializeWriteBit(PikaObj* self, int addr, int status) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_write_bit(ctx, addr, status);
}

int _modbus__ModBus_serializeWriteBits(PikaObj* self,
                                       int addr,
                                       int nb,
                                       uint8_t* src) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_write_bits(ctx, addr, nb, src);
}

int _modbus__ModBus_serializeWriteRegister(PikaObj* self, int addr, int value) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_write_register(ctx, addr, value);
}

int _modbus__ModBus_serializeWriteRegisters(PikaObj* self,
                                            int addr,
                                            int nb,
                                            uint8_t* src) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_serialize_write_registers(ctx, addr, nb,
                                                  (uint16_t*)src);
}
