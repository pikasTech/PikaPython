#include "cJSON_cJSON.h"
#include "cJSON.h"

void cJSON_cJSON_parse(PikaObj* self, char* value) {
    cJSON* item = cJSON_Parse(value);
    obj_setPtr(self, "item", item);
}

char* cJSON_cJSON_print(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    char* res = cJSON_Print(item);
    obj_setStr(self, "_buf", res);
    cJSON_free(res);
    return obj_getStr(self, "_buf");
}

void cJSON_cJSON___del__(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    if (NULL != item) {
        cJSON_Delete(item);
    }
}