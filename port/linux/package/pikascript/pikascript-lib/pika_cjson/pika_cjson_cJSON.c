#include "pika_cjson_cJSON.h"
#include "cJSON.h"

void pika_cjson_cJSON_parse(PikaObj* self, char* value) {
    cJSON* item = cJSON_Parse(value);
    obj_setPtr(self, "item", item);
    obj_setInt(self, "needfree", 1);
}

char* pika_cjson_cJSON_print(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    char* res = cJSON_Print(item);
    obj_setStr(self, "_buf", res);
    cJSON_free(res);
    return obj_getStr(self, "_buf");
}

void pika_cjson_cJSON___del__(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    if (obj_getInt(self, "needfree") == 1) {
        cJSON_Delete(item);
    }
}

PikaObj* pika_cjson_cJSON_getObjectItem(PikaObj* self, char* string) {
    cJSON* item = obj_getPtr(self, "item");
    cJSON* subItem = cJSON_GetObjectItem(item, string);

    /* create subCJSON */
    PikaObj* subCJSON = newNormalObj(New_pika_cjson_cJSON);

    /* init the subCJSON */
    obj_setPtr(subCJSON, "item", subItem);
    obj_setInt(subCJSON, "needfree", 0);

    return subCJSON;
}

void pika_cjson_cJSON___init__(PikaObj* self) {
    obj_setInt(self, "cJSON_Invalid", 0);
    obj_setInt(self, "cJSON_False", 1);
    obj_setInt(self, "cJSON_True", 2);
    obj_setInt(self, "cJSON_NULL", 3);
    obj_setInt(self, "cJSON_Number", 4);
    obj_setInt(self, "cJSON_String", 5);
    obj_setInt(self, "cJSON_Array", 6);
    obj_setInt(self, "cJSON_Object", 7);
    obj_setInt(self, "cJSON_Raw", 8);
    obj_setInt(self, "cJSON_IsReference", 9);
    obj_setInt(self, "cJSON_StringIsConst", 10);
}

PikaObj* pika_cjson_cJSON_getChild(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    cJSON* resItem = item->child;

    /* create subCJSON */
    PikaObj* resCJSON = newNormalObj(New_pika_cjson_cJSON);

    /* init the subCJSON */
    obj_setPtr(resCJSON, "item", resItem);
    obj_setInt(resCJSON, "needfree", 0);

    return resCJSON;
}

PikaObj* pika_cjson_cJSON_getNext(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    cJSON* resItem = item->next;

    /* create subCJSON */
    PikaObj* resCJSON = newNormalObj(New_pika_cjson_cJSON);

    /* init the subCJSON */
    obj_setPtr(resCJSON, "item", resItem);
    obj_setInt(resCJSON, "needfree", 0);

    return resCJSON;
}

PikaObj* pika_cjson_cJSON_getPrev(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    cJSON* resItem = item->prev;

    /* create subCJSON */
    PikaObj* resCJSON = newNormalObj(New_pika_cjson_cJSON);

    /* init the subCJSON */
    obj_setPtr(resCJSON, "item", resItem);
    obj_setInt(resCJSON, "needfree", 0);

    return resCJSON;
}

char* pika_cjson_cJSON_getString(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return item->string;
}

int pika_cjson_cJSON_getType(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return item->type;
}

float pika_cjson_cJSON_getValueDouble(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return item->valuedouble;
}

int pika_cjson_cJSON_getValueInt(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return item->valueint;
}

char* pika_cjson_cJSON_getValueString(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return item->valuestring;
}
