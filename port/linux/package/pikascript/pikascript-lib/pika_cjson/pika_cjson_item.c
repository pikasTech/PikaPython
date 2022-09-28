#include "cJSON.h"
#include "pika_cjson_Array.h"
#include "pika_cjson_ArrayReference.h"
#include "pika_cjson_Bool.h"
#include "pika_cjson_False_.h"
#include "pika_cjson_Null.h"
#include "pika_cjson_Number.h"
#include "pika_cjson_Object.h"
#include "pika_cjson_ObjectReference.h"
#include "pika_cjson_Raw.h"
#include "pika_cjson_String.h"
#include "pika_cjson_StringReference.h"
#include "pika_cjson_True_.h"
#include "pika_cjson_cJSON.h"

void pika_cjson_False____init__(PikaObj* self) {
    pika_cjson_cJSON___init__(self);
    cJSON* item = cJSON_CreateFalse();
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_Null___init__(PikaObj* self) {
    pika_cjson_cJSON___init__(self);
    cJSON* item = cJSON_CreateNull();
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_True____init__(PikaObj* self) {
    pika_cjson_cJSON___init__(self);
    cJSON* item = cJSON_CreateTrue();
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_Bool___init__(PikaObj* self, int bolean) {
    pika_cjson_cJSON___init__(self);
    cJSON* item = cJSON_CreateBool(bolean);
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_Number___init__(PikaObj* self, pika_float num) {
    pika_cjson_cJSON___init__(self);
    cJSON* item = cJSON_CreateNumber(num);
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_String___init__(PikaObj* self, char* string) {
    pika_cjson_cJSON___init__(self);
    cJSON* item = cJSON_CreateString(string);
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_Raw___init__(PikaObj* self, char* raw) {
    pika_cjson_cJSON___init__(self);
    cJSON* item = cJSON_CreateRaw(raw);
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_Array___init__(PikaObj* self) {
    pika_cjson_cJSON___init__(self);
    cJSON* item = cJSON_CreateArray();
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_Object___init__(PikaObj* self) {
    pika_cjson_cJSON___init__(self);
    cJSON* item = cJSON_CreateObject();
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_StringReference___init__(PikaObj* self, char* string) {
    pika_cjson_cJSON___init__(self);
    cJSON* item = cJSON_CreateStringReference(string);
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_ObjectReference___init__(PikaObj* self, PikaObj* child) {
    pika_cjson_cJSON___init__(self);
    cJSON* child_item = obj_getPtr(child, "item");
    cJSON* item = cJSON_CreateObjectReference(child_item);
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

void pika_cjson_ArrayReference___init__(PikaObj* self, PikaObj* child) {
    pika_cjson_cJSON___init__(self);
    cJSON* child_item = obj_getPtr(child, "item");
    cJSON* item = cJSON_CreateArrayReference(child_item);
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}
