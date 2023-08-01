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

void PikaStdData_List_append(PikaObj* self, Arg* arg);
void PikaStdData_Dict_set(PikaObj* self, char* key, Arg* arg);

Arg* _lua_val_to_arg(lua_State* L) {
    if (!lua_gettop(L)) {
        return NULL;
    }
    if (lua_isinteger(L, -1)) {
        return arg_newInt(lua_tointeger(L, -1));
    }
    if (lua_isnumber(L, -1)) {
        return arg_newFloat(lua_tonumber(L, -1));
    }
    if (lua_isstring(L, -1)) {
        return arg_newStr((char*)lua_tostring(L, -1));
    }
    if (lua_isboolean(L, -1)) {
        return arg_newBool(lua_toboolean(L, -1));
    }
    if (lua_isnil(L, -1) || lua_isnoneornil(L, -1)) {
        return arg_newNone();
    }
    if (lua_istable(L, -1)) {
        PikaObj* ret = NULL;
        PIKA_BOOL get_list = PIKA_FALSE;
        PIKA_BOOL get_dict = PIKA_FALSE;
        lua_pushnil(L);  // push the first key
        // PikaObj* dict = objDict_new0();
        while (lua_next(L, -2) != 0) {
            // 'key' is at index -2 and 'value' at index -1
            if (!get_list && !get_dict) {
                if (lua_isinteger(L, -2)) {
                    ret = objList_new(NULL);
                    get_list = PIKA_TRUE;
                } else {
                    ret = objDict_new(NULL);
                    get_dict = PIKA_TRUE;
                }
            }
            Arg* val = _lua_val_to_arg(L);
            if (get_list) {
                PikaStdData_List_append(ret, val);
            } else if (get_dict) {
                char* key = (char*)lua_tostring(L, -2);
                PikaStdData_Dict_set(ret, key, val);
            }
            arg_deinit(val);
            lua_pop(L, 1);  // Remove value, keep key for next iteration
        }
        return arg_newObj(ret);  // You might want to return something else here
    }
    return NULL;
}

Arg* _pika_lua_evals(PikaObj* self, char* cmd) {
    Args buffs = {0};
    int res = luaL_dostring(g_pika_L, cmd);
    if (LUA_OK != res) {
        obj_setErrorCode(self, PIKA_RES_ERR_OPERATION_FAILED);
        pika_platform_printf("Error: Lua dostring failed: \r\n> %s\r\n",
                             lua_tostring(g_pika_L, -1));
        lua_pop(g_pika_L, 1);
        strsDeinit(&buffs);
        return NULL;
    }
    Arg* ret = _lua_val_to_arg(g_pika_L);
    if (ret != NULL) {
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
    Arg* ret = _pika_lua_evals(self, line_with_end);
    goto __exit;
__exit:
    strsDeinit(&buffs);
    return ret;
}

void _pika_lua___del__(PikaObj* self) {
    pika_debug("lua close!\r\n");
    lua_close(g_pika_L);
    g_pika_L = NULL;
}
