#include "PikaStdData_Utils.h"
#include "dataStrs.h"

Arg* PikaStdData_Utils_int_to_bytes(PikaObj* self, int val) {
    if (val > 0xFF) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self,
                      "OverflowError: cannot convert value larger than 0xFF to "
                      "bytes");
        return arg_newNone();
    }
    uint8_t val_bytes = (uint8_t)val;
    return arg_newBytes(&val_bytes, 1);
}
