from PikaObj import *


class cJSON(TinyObj):
    cJSON_Invalid = 0
    cJSON_False = 1
    cJSON_True = 2
    cJSON_NULL = 3
    cJSON_Number = 4
    cJSON_String = 5
    cJSON_Array = 6
    cJSON_Object = 7
    cJSON_Raw = 8
    cJSON_IsReference = 9
    cJSON_StringIsConst = 10
    def print(self) -> str: ...
    def parse(self, value: str): ...
    def __del__(self): ...
    def __init__(self): ...
    def getObjectItem(self, string: str) -> cJSON: ...
