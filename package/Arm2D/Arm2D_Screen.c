#include "Arm2D_common.h"

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
