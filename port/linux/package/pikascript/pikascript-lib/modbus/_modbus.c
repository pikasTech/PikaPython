#include "_modbus__ModBus.h"
#include "agile_modbus.h"

#if !PIKASCRIPT_VERSION_REQUIRE_MINIMUN(1, 11, 1)
#error "pikascript version must be greater than 1.11.1"
#endif

void _modbus__ModBus___init__rtu(PikaObj* self,
                                 int sendBUffSize,
                                 int readBuffSize) {
    agile_modbus_rtu_t ctx_rtu = {0};
    obj_setBytes(self, "sendBuff", NULL, sendBUffSize);
    obj_setBytes(self, "readBuff", NULL, readBuffSize);
    agile_modbus_rtu_init(&ctx_rtu, obj_getBytes(self, "sendBuff"),
                          sendBUffSize, obj_getBytes(self, "readBuff"),
                          readBuffSize);
    obj_setStruct(self, "ctx_rtu", ctx_rtu);
    agile_modbus_rtu_t* ctx_rtu_heap = obj_getStruct(self, "ctx_rtu");
    obj_setPtr(self, "ctx", &ctx_rtu_heap->_ctx);
}

void _modbus__ModBus_setSlave(PikaObj* self, int slave) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    agile_modbus_set_slave(ctx, slave);
}

void _modbus__ModBus___init__tcp(PikaObj* self,
                                 int sendBuffSize,
                                 int readBuffSize) {
    agile_modbus_tcp_t ctx_tcp = {0};
    obj_setBytes(self, "sendBuff", NULL, sendBuffSize);
    obj_setBytes(self, "readBuff", NULL, readBuffSize);
    agile_modbus_tcp_init(&ctx_tcp, obj_getBytes(self, "sendBuff"),
                          sendBuffSize, obj_getBytes(self, "readBuff"),
                          readBuffSize);
    obj_setStruct(self, "ctx_tcp", ctx_tcp);
    agile_modbus_tcp_t* ctx_tcp_heap = obj_getStruct(self, "ctx_tcp");
    obj_setPtr(self, "ctx", &ctx_tcp_heap->_ctx);
}

int _modbus__ModBus_deserializeMaskWriteRegister(PikaObj* self, int msgLength) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return agile_modbus_deserialize_mask_write_register(ctx, msgLength);
}

Arg* _modbus__ModBus_deserializeReadRegisters(PikaObj* self, int msgLength) {
    uint16_t buff[128] = {0};
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    int len = agile_modbus_deserialize_read_registers(ctx, msgLength,
                                                      (uint16_t*)buff);
    if (len < 0) {
        return NULL;
    }
    return arg_newBytes((uint8_t*)buff, len * 2);
}

Arg* _modbus__ModBus_deserializeReadBits(PikaObj* self, int msgLength) {
    uint8_t buff[128] = {0};
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    int len = agile_modbus_deserialize_read_bits(ctx, msgLength, buff);
    if (len < 0) {
        return NULL;
    }
    return arg_newBytes(buff, len);
}

Arg* _modbus__ModBus_deserializeReadInputBits(PikaObj* self, int msgLength) {
    uint8_t buff[128] = {0};
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    int len = agile_modbus_deserialize_read_input_bits(ctx, msgLength, buff);
    if (len < 0) {
        return NULL;
    }
    return arg_newBytes(buff, len);
}

Arg* _modbus__ModBus_deserializeReadInputRegisters(PikaObj* self,
                                                   int msgLength) {
    uint16_t buff[128] = {0};
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    int len = agile_modbus_deserialize_read_input_registers(ctx, msgLength,
                                                            (uint16_t*)buff);
    if (len < 0) {
        return NULL;
    }
    return arg_newBytes((uint8_t*)buff, len * 2);
}

Arg* _modbus__ModBus_deserializeReportSlaveId(PikaObj* self,
                                              int msgLength,
                                              int maxDest) {
    uint8_t buff[128] = {0};
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    int len = agile_modbus_deserialize_report_slave_id(ctx, msgLength, maxDest,
                                                       (uint8_t*)buff);
    if (len < 0) {
        return NULL;
    }
    return arg_newBytes(buff, len);
}

Arg* _modbus__ModBus_deserializeWriteAndReadRegisters(PikaObj* self,
                                                      int msgLength) {
    uint16_t buff[128] = {0};
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    int len = agile_modbus_deserialize_write_and_read_registers(
        ctx, msgLength, (uint16_t*)buff);
    if (len < 0) {
        return NULL;
    }
    return arg_newBytes((uint8_t*)buff, len * 2);
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

Arg* _modbus__ModBus_getSendBuff(PikaObj* self) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return arg_newBytes(ctx->send_buf, ctx->send_bufsz);
}

Arg* _modbus__ModBus_getReadBuff(PikaObj* self) {
    agile_modbus_t* ctx = obj_getPtr(self, "ctx");
    return arg_newBytes(ctx->read_buf, ctx->read_bufsz);
}
