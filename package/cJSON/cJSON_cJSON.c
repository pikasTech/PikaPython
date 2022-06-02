#include "cJSON_cJSON.h"
#include "cJSON.h"

void cJSON_cJSON_parse(PikaObj* self, char* value) {
    cJSON* item = cJSON_Parse(value);
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
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
    if (obj_getInt(self, "needfree") == 1) {
        cJSON_Delete(item);
    }
}

Arg* cJSON_cJSON_getObjectItem(PikaObj* self, char* string) {
    cJSON* item = obj_getPtr(self, "item");
    cJSON* subItem = cJSON_GetObjectItem(item, string);
    Arg* subItem_arg = obj_newObjInPackage(New_cJSON_cJSON);
    PikaObj* subCJSON = arg_getPtr(subItem_arg);
    obj_setPtr(subCJSON, "item", subItem);
    obj_setInt(subCJSON, "needfree", 0);
    return subItem_arg;
}
