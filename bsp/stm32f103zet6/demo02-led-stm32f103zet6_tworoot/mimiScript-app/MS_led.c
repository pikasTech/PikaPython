#include "main.h"
#include "baseObj.h"

static void led1onFun(MimiObj *self, Args *args)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
}

static void led1offFun(MimiObj *self, Args *args)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
}

static void led2onFun(MimiObj *self, Args *args)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
}

static void led2offFun(MimiObj *self, Args *args)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);
}

MimiObj *New_LED1(Args *args)
{
  /*  Derive from the tiny object class.
    Tiny object can not import sub object.  
    Tiny object is the smallest object. */  
    MimiObj   *self  = New_TinyObj(args);
    class_defineMethod(self, "led1on()", led1onFun);
    class_defineMethod(self, "led1off()", led1offFun);
    return self;
}

MimiObj *New_LED2(Args *args)
{
  /*  Derive from the tiny object class.
    Tiny object can not import sub object.  
    Tiny object is the smallest object. */  
    MimiObj   *self  = New_TinyObj(args);
    class_defineMethod(self, "led2on()", led2onFun);
    class_defineMethod(self, "led2off()", led2offFun);
    return self;
}
