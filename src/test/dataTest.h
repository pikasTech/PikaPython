#ifndef _dataTest__H
#define _dataTest__H
#include "dataMemory.h"
#include "dataArgs.h"
typedef struct Class_dataTest dataTest_t;
struct Class_dataTest
{
    /* attribute */
    void *context;
    int32_t a;
    int32_t b;
    uint32_t memSize;

    /* operation */
    void (*deinit)(dataTest_t *self);
    void (*init)(dataTest_t *self, Args *args);

    /* virtual operation */

    /* object */
};

dataTest_t *New_dataTest(Args *args);
#endif