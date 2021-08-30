#include "dataTest.h"
#include "dataMemory.h"
#include "dataArgs.h"

static void deinit(dataTest_t *self)
{
    pikaFree(self, sizeof(dataTest_t));
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
    dataTest_t *self = pikaMalloc(sizeof(dataTest_t));
    self->init = init;
    self->init(self, args);
    return self;
}
