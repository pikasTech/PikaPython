#include "MimiObj.h"
#include "TinyObj.h"
#include "dataMemory.h"
#include "dataStrs.h"
#include "dataString.h"

static void init_class(MimiObj *self, Args *args)
{
    /* attribute */

    /* operation */

    /* object */

    /* override */
}

MimiObj *New_MimiObj_class(Args *args)
{
    MimiObj *self = New_TinyObj(args);
    init_class(self, args);
    return self;
}