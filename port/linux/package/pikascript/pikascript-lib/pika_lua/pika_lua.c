#include "pika_lua.h"
#include "dataStrs.h"
#include "lauxlib.h"
#include "lualib.h"

/* if there is already a lua_State, extern pika_L to it */
lua_State* pika_L = NULL;

void pika_lua___init__(PikaObj* self) {
    /* not found exist lua_State, create One */
    if (NULL == pika_L) {
        pika_L = luaL_newstate(); /* create state */
        luaL_openlibs(pika_L);
        obj_setInt(self, "needfree", PIKA_TRUE);
        __platform_printf("lua luached!\r\n");
    }
}

void pika_lua_eval(PikaObj* self, char* cmd) {
    int res = luaL_dostring(pika_L, cmd);
    if (LUA_OK != res) {
        obj_setErrorCode(self, PIKA_RES_ERR_OPERATION_FAILED);
        obj_setSysOut(self, "Error: Lua dostring faild.\r\n");
    }
}

void pika_lua___exit__(PikaObj* self) {
    /* close the lua_State created */
    if (PIKA_TRUE == obj_getInt(self, "needfree")) {
        lua_close(pika_L);
    }
}

void pika_lua_evalLine(PikaObj* self, char* line) {
    Args buffs = {0};
    char* line_with_end = strsAppend(&buffs, line, "\n");
    pika_lua_eval(self, line_with_end);
    strsDeinit(&buffs);
}