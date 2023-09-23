#include "pika_cjson.h"
#include "cJSON.h"
#include "pika_cjson_cJSON.h"

PikaObj* pika_cjson_Parse(PikaObj* self, char* value) {
    cJSON* item = cJSON_Parse(value);
    if (NULL == item) {
        return NULL;
    }
    PikaObj* cjson_obj = newNormalObj(New_pika_cjson_cJSON);
    obj_setPtr(cjson_obj, "item", item);
    obj_setInt(cjson_obj, "needfree", 1);
    return cjson_obj;
}
