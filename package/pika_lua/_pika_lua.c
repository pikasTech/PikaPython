#include "_pika_lua.h"
#include "dataStrs.h"
#include "lauxlib.h"
#include "lualib.h"

#if !PIKASCRIPT_VERSION_REQUIRE_MINIMUN(1, 12, 3)
#error "PikaScript version require minimum 1.12.3"
#endif

/* if there is already a lua_State, extern pika_L to it */
lua_State* g_pika_L = NULL;

void _pika_lua___init__(PikaObj* self) {
    /* not found exist lua_State, create One */
    if (NULL == g_pika_L) {
        g_pika_L = luaL_newstate(); /* create state */
        luaL_openlibs(g_pika_L);
        obj_setInt(self, "needfree", PIKA_TRUE);
        pika_debug("lua luached!\r\n");
    }
}

Arg* _pika_lua_eval(PikaObj* self, char* cmd) {
    Args buffs = {0};
    int res = luaL_dostring(g_pika_L, cmd);
    Arg* ret = NULL;
    if (LUA_OK != res) {
        obj_setErrorCode(self, PIKA_RES_ERR_OPERATION_FAILED);
        pika_platform_printf("Error: Lua dostring failed: \r\n> %s\r\n",
                             lua_tostring(g_pika_L, -1));
        ret = NULL;
        goto __exit;
    }
    if (!lua_gettop(g_pika_L)) {
        ret = NULL;
        goto __exit;
    }
    if (lua_isinteger(g_pika_L, -1)) {
        ret = arg_newInt(lua_tointeger(g_pika_L, -1));
        goto __exit;
    }
    if (lua_isnumber(g_pika_L, -1)) {
        ret = arg_newFloat(lua_tonumber(g_pika_L, -1));
        goto __exit;
    }
    if (lua_isstring(g_pika_L, -1)) {
        ret = arg_newStr((char*)lua_tostring(g_pika_L, -1));
        goto __exit;
    }
    if (lua_isboolean(g_pika_L, -1)) {
        ret = arg_newBool(lua_toboolean(g_pika_L, -1));
        goto __exit;
    }
    if (lua_isnil(g_pika_L, -1) || lua_isnoneornil(g_pika_L, -1)) {
        ret = arg_newNull();
        goto __exit;
    }
__exit:
    if (NULL != ret) {
        lua_pop(g_pika_L, 1);
    }
    strsDeinit(&buffs);
    return ret;
}

void _pika_lua___exit__(PikaObj* self) {
    /* close the lua_State created */
    if (PIKA_TRUE == obj_getInt(self, "needfree")) {
        lua_close(g_pika_L);
    }
}

Arg* _pika_lua_evalLine(PikaObj* self, char* line) {
    Args buffs = {0};
    char* line_with_end = strsAppend(&buffs, line, "\n");
    Arg* ret = _pika_lua_eval(self, line_with_end);
    goto __exit;
__exit:
    strsDeinit(&buffs);
    return ret;
}

void _pika_lua___del__(PikaObj* self){
    pika_debug("lua close!\r\n");
    lua_close(g_pika_L);  // 关闭 Lua 状态机，释放所有关联的资源
    g_pika_L = NULL;
}
