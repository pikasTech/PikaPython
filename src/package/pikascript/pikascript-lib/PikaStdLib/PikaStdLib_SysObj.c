#include "dataStrs.h"
#include "BaseObj.h"

static int32_t __foreach_listEachArg(Arg *argEach, Args *handleArgs)
{
    Args *buffs = handleArgs;
    if (NULL == handleArgs)
    {
        /* error: not handleArgs input */
        return 1;
    }

    char *argName = strsCopy(buffs, arg_getName(argEach));
    if (strIsStartWith(argName, "["))
    {
        /* skip */
        return 0;
    }

    char *stringOut = args_getStr(handleArgs, "stringOut");
    if (NULL == stringOut)
    {
        // stringOut no found
        return 1;
    }

    stringOut = strsAppend(buffs, stringOut, argName);
    stringOut = strsAppend(buffs, stringOut, " ");
    args_setStr(handleArgs, "stringOut", stringOut);
    return 0;
}

void PikaStdLib_SysObj_ls(PikaObj *self, char *objPath)
{
    obj_setErrorCode(self, 0);
    Args *args = New_args(NULL);
    args_setStr(args, "stringOut", "");
    obj_setSysOut(self, "");
    if (NULL == objPath)
    {
        /* no input obj path, use current obj */
        args_foreach(self->attributeList, __foreach_listEachArg, args);
        obj_setSysOut(self, args_getStr(args, "stringOut"));
        goto exit;
    }
    PikaObj *obj = obj_getObj(self, objPath, 0);
    if (NULL == obj)
    {
        /* do not find obj */
        obj_setSysOut(self, "[error] list: object no found.");
        obj_setErrorCode(self, 1);
        goto exit;
    }
    /* list args */
    args_foreach(obj->attributeList, __foreach_listEachArg, args);
    obj_setSysOut(self, args_getStr(args, "stringOut"));
exit:
    args_deinit(args);
}

void PikaStdLib_SysObj_new(PikaObj *self, char *classPath, char *objPath)
{
    int32_t res = obj_newObj(self, objPath, classPath);
    if (1 == res)
    {
        obj_setSysOut(self, "[error] new: class not found .");
        obj_setErrorCode(self, 1);
        return;
    }
}

void PikaStdLib_SysObj_remove(PikaObj *self, char *argPath)
{
    obj_setErrorCode(self, 0);
    int32_t res = obj_removeArg(self, argPath);
    if (1 == res)
    {
        obj_setSysOut(self, "[error] del: object no found.");
        obj_setErrorCode(self, 1);
        return;
    }
    if (2 == res)
    {
        obj_setSysOut(self, "[error] del: arg not match.");
        obj_setErrorCode(self, 2);
        return;
    }
}

void PikaStdLib_SysObj_type(PikaObj *self, char *argPath)
{
    if (NULL == argPath)
    {
        /* no input obj path, use current obj */
        PikaObj *objHost = obj_getPtr(self, "__context");
        Arg *objArg = obj_getArg(objHost, self->name);
        if (NULL == objArg)
        {
            obj_setSysOut(self, "[error] type: arg no found.");
            obj_setErrorCode(self, 1);
            return;
        }
        obj_setSysOut(self, arg_getType(objArg));
        return;
    }
    Arg *arg = obj_getArg(self, argPath);
    if (NULL == arg)
    {
        obj_setSysOut(self, "[error] type: arg no found.");
        obj_setErrorCode(self, 1);
        return;
    }
    obj_setSysOut(self, arg_getType(arg));
}
