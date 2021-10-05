#include "Arm2D_common.h"

void Arm2D_Screen_newBox(PikaObj *self, char *name)
{
    Args *buffs = New_strBuff();
    PikaObj *pikaMain = obj_getPtr(self, "__context");
    int res = obj_newObj(pikaMain, strsFormat(buffs, "screen.elems.%s", name), "Arm2D_Box");
    {
        if (res == 1)
        {
            obj_setErrorCode(self, 1);
            obj_setSysOut(self, "[error]: new Box faild! class no found!");
            goto exit;
        }
        if (res == 2)
        {
            obj_setErrorCode(self, 2);
            obj_setSysOut(self, "[error]: new Box faild! object host no found!");
            goto exit;
        }
    }
    obj_run(pikaMain, strsFormat(buffs, "screen.elems.%s.init()", name));
exit:
    args_deinit(buffs);
}

void Arm2D_Screen_newStar(PikaObj *self, char *name)
{
    Args *buffs = New_strBuff();
    PikaObj *pikaMain = obj_getPtr(self, "__context");
    obj_newObj(pikaMain, strsFormat(buffs, "screen.elems.%s", name), "Arm2D_Star");
    obj_run(pikaMain, strsFormat(buffs, "screen.elems.%s.init()", name));
    args_deinit(buffs);
}

void Arm2D_Screen_init(PikaObj *self)
{
    obj_run(self, "background.init()");
}

void Arm2D_Screen_update(PikaObj *self, int bIsNewFrame, void *ptTile)
{
    obj_setPtr(self, "ptTile", (void *)ptTile);
    obj_setInt(self, "bIsNewFrame", bIsNewFrame);
    obj_run(self, "background.update(ptTile, bIsNewFrame)");
    obj_run(self, "elems.update(ptTile, bIsNewFrame)");
}
