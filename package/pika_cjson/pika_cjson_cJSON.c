#include "pika_cjson_cJSON.h"
#include "cJSON.h"

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
    if (NULL == subItem) {
        return NULL;
    }

    /* create subCJSON */
    PikaObj* subCJSON = newNormalObj(New_pika_cjson_cJSON);

    /* init the subCJSON */
    obj_setPtr(subCJSON, "item", subItem);
    obj_setInt(subCJSON, "needfree", 0);

    return subCJSON;
}

void pika_cjson_cJSON___init__(PikaObj* self) {
    /* const value */
    obj_setInt(self, "cJSON_Invalid", cJSON_Invalid);
    obj_setInt(self, "cJSON_False", cJSON_False);
    obj_setInt(self, "cJSON_True", cJSON_True);
    obj_setInt(self, "cJSON_NULL", cJSON_NULL);
    obj_setInt(self, "cJSON_Number", cJSON_Number);
    obj_setInt(self, "cJSON_String", cJSON_String);
    obj_setInt(self, "cJSON_Array", cJSON_Array);
    obj_setInt(self, "cJSON_Object", cJSON_Object);
    obj_setInt(self, "cJSON_Raw", cJSON_Raw);
}

PikaObj* pika_cjson_cJSON_getChild(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    cJSON* resItem = item->child;
    if (NULL == resItem) {
        return NULL;
    }

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
    if (NULL == resItem) {
        return NULL;
    }

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
    if (NULL == resItem) {
        return NULL;
    }

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

pika_float pika_cjson_cJSON_getValueDouble(PikaObj* self) {
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

Arg* pika_cjson_cJSON_getValue(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    int type = item->type;
    if (type == cJSON_Invalid) {
        return arg_newNull();
    }
    if (type == cJSON_False) {
        return arg_newInt(0);
    }
    if (type == cJSON_True) {
        return arg_newInt(1);
    }
    if (type == cJSON_NULL) {
        return arg_newNull();
    }
    if (type == cJSON_Number) {
        return arg_newFloat(item->valuedouble);
    }
    if (type == cJSON_String) {
        return arg_newStr(item->valuestring);
    }
    return arg_newNull();
}

PikaObj* pika_cjson_cJSON_getArrayItem(PikaObj* self, int index) {
    cJSON* item = obj_getPtr(self, "item");
    cJSON* subItem = cJSON_GetArrayItem(item, index);
    if (NULL == subItem) {
        return NULL;
    }

    /* create subCJSON */
    PikaObj* subCJSON = newNormalObj(New_pika_cjson_cJSON);

    /* init the subCJSON */
    obj_setPtr(subCJSON, "item", subItem);
    obj_setInt(subCJSON, "needfree", 0);

    return subCJSON;
}

int pika_cjson_cJSON_getArraySize(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_GetArraySize(item);
}

int pika_cjson_cJSON_isArray(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_IsArray(item);
}

int pika_cjson_cJSON_isBool(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_IsBool(item);
}

int pika_cjson_cJSON_isFalse(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_IsFalse(item);
}

int pika_cjson_cJSON_isInvalid(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_IsInvalid(item);
}

int pika_cjson_cJSON_isNull(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_IsNull(item);
}

int pika_cjson_cJSON_isNumber(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_IsNumber(item);
}

int pika_cjson_cJSON_isObject(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_IsObject(item);
}

int pika_cjson_cJSON_isRaw(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_IsRaw(item);
}

int pika_cjson_cJSON_isString(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_IsString(item);
}

int pika_cjson_cJSON_isTrue(PikaObj* self) {
    cJSON* item = obj_getPtr(self, "item");
    return cJSON_IsTrue(item);
}

void pika_cjson_cJSON_addItemToArray(PikaObj* self, PikaObj* item) {
    cJSON* self_item = obj_getPtr(self, "item");
    cJSON* item_item = obj_getPtr(item, "item");
    cJSON_AddItemToArray(self_item, item_item);
    obj_setInt(item, "needfree", 0);
}

void pika_cjson_cJSON_addItemToObject(PikaObj* self,
                                      char* string,
                                      PikaObj* item) {
    cJSON* self_item = obj_getPtr(self, "item");
    cJSON* item_item = obj_getPtr(item, "item");
    cJSON_AddItemToObject(self_item, string, item_item);
    obj_setInt(item, "needfree", 0);
}
