#include "cJSON_cJSON.h"
#include "cJSON.h"

void cJSON_cJSON___init__(PikaObj* self) {}
Arg* cJSON_cJSON_parse(PikaObj* self, char* value) {
    cJSON* cJSON_struct = cJSON_Parse(value);
}
char* cJSON_cJSON_print(PikaObj* self) {
    return NULL;
}