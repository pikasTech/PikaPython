#include "dataTest.h"
#include "dataMemory.h"
#include "dataArgs.h"

static void deinit(dataTest_t *self)
{
    DynMemPut(self->mem);
}

static void init(dataTest_t *self, Args *args)
{
    /* attribute */
    self->context = self;
    self->a = 0;
    self->b = 0;

    /* operation */
    self->deinit = deinit;

    /* object */

    /* override */
}

dataTest_t *New_dataTest(Args *args)
{
    DMEM *mem = DynMemGet(sizeof(dataTest_t));
    dataTest_t *self = (void *)(mem->addr);
    self->mem = mem;
    self->init = init;
    self->init(self, args);
    return self;
}