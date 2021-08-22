#include "gtest/gtest.h"
extern "C"
{
#include "SysObj.h"
#include "TinyObj.h"
#include "BaseObj.h"
}

void testFloat(MimiObj *obj, Args *args)
{
    float val1 = args_getFloat(args, (char *)"val1");
    float val2 = args_getFloat(args, (char *)"val2");
    int32_t isShow = args_getInt(args, (char *)"isShow");
    if (isShow)
    {
        printf("the float val1 is: %f\r\n", val1);
        printf("the float val2 is: %f\r\n", val2);
    }
    method_returnFloat(args, val1 + val2);
}

void hello2(MimiObj *obj, Args *args)
{
    char *name1 = args_getStr(args, (char *)"name1");
    char *name2 = args_getStr(args, (char *)"name2");
    char *name3 = args_getStr(args, (char *)"name3");
    char *myName = obj->name;
    int32_t isShow = args_getInt(args, (char *)"isShow");
    if (isShow)
    {
        printf("hello, %s, %s and %s!\r\n", name1, name2, name3);
        printf("my name is %s.\r\n", myName);
    }
}

void hello(MimiObj *obj, Args *args)
{
    char *name = args_getStr(args, (char *)"name");
    int32_t isShow = args_getInt(args, (char *)"isShow");
    if (isShow)
    {
        printf("hello, %s!\r\n", name);
    }
}

void add(MimiObj *obj, Args *args)
{
    int32_t val1 = args_getInt(args, (char *)"val1");
    int32_t val2 = args_getInt(args, (char *)"val2");
    method_returnInt(args, val1 + val2);
}

MimiObj *New_MimiObj_test(Args *args)
{
    MimiObj *self = New_SysObj(args);
    class_defineMethod(self, (char *)"hello(name:str, isShow:int)", hello);
    class_defineMethod(self, (char *)"hello2(name1:str, name2:str, name3:str, isShow:int)", hello2);
    class_defineMethod(self, (char *)"testFloat(val1:float, val2:float, isShow:int)->float", testFloat);
    class_defineMethod(self, (char *)"add(val1:int, val2:int)->int", add);
    return self;
}

void sendMethod(MimiObj *self, Args *args)
{
    char *data = args_getStr(args, (char *)"data");
    /* send to com1 */
    printf("[com1]: %s\r\n", data);
}

MimiObj *New_USART(Args *args)
{
    /*	Derive from the tiny object class.
		Tiny object can not import sub object.
		Tiny object is the smallest object. */
    MimiObj *self = New_TinyObj(args);

    /* bind the method */
    class_defineMethod(self, (char *)"send(data:str)", sendMethod);

    /* return the object */
    return self;
}

MimiObj *New_MYROOT1(Args *args)
{
    /*	Derive from the base object class .
		BaseObj is the smallest object that can
		import sub object.		*/
    MimiObj *self = New_BaseObj(args);

    /* import LED class */
    obj_import(self, (char *)"USART", New_USART);

    /* new led object bellow root object */
    obj_newObj(self, (char *)"usart", (char *)"USART");

    /* return the object */
    return self;
}

extern DMEM_STATE DMEMS;
TEST(object_test, test1)
{
    MimiObj *process = newRootObj((char *)"sys", New_SysObj);
    float floatTest = 12.231;
    obj_bindFloat(process, (char *)"testFloatBind", &floatTest);
    EXPECT_TRUE(strEqu((char *)"12.231000", obj_print(process, (char *)"testFloatBind")));
    obj_deinit(process);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, test2)
{
    int isShow = 1;
    MimiObj *obj = newRootObj((char *)"test", New_MimiObj_test);
    obj_setInt(obj, (char *)"isShow", isShow);
    obj_run(obj, (char *)"hello(name = 'world', isShow = isShow)");
    obj_deinit(obj);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, test3)
{
    int isShow = 1;
    MimiObj *obj = newRootObj((char *)"test", New_MimiObj_test);
    obj_setInt(obj, (char *)"isShow", isShow);
    obj_run(obj, (char *)"hello2(name2='tom', name1='john', name3='cat', isShow=isShow) ");
    obj_deinit(obj);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, test4)
{
    int isShow = 1;
    MimiObj *obj = newRootObj((char *)"test", New_MimiObj_test);
    obj_setInt(obj, (char *)"isShow", isShow);
    obj_setFloat(obj, (char *)"val2", 3.11);
    obj_run(obj, (char *)"res = testFloat(val1 = 3.22,val2 = val2,isShow = isShow)");
    float res = obj_getFloat(obj, (char *)"res");
    EXPECT_TRUE((res - 6.33) * (res - 6.33) < 0.00001);
    obj_deinit(obj);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, test5)
{
    MimiObj *obj = newRootObj((char *)"test", New_MimiObj_test);
    obj_run(obj, (char *)"res = add(val1 = 1, val2 = 2)");
    int32_t res = obj_getInt(obj, (char *)"res");
    EXPECT_EQ(3, res);
    obj_deinit(obj);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, test6)
{
    MimiObj *obj = newRootObj((char *)"test", New_MimiObj_test);
    obj_run(obj, (char *)"res = add(1, 2)");
    int32_t res = obj_getInt(obj, (char *)"res");
    EXPECT_EQ(3, res);
    obj_deinit(obj);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, test7)
{
    MimiObj *sys = newRootObj((char *)"sys", New_SysObj);
    int32_t a = 0;
    obj_bind(sys, (char *)"int", (char *)"a", &a);
    obj_run(sys, (char *)"set('a', 1)");
    obj_deinit(sys);
    EXPECT_EQ(1, a);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, test8)
{
    MimiObj *sys = newRootObj((char *)"sys", New_SysObj);
    obj_run(sys, (char *)"set('a', 1)");
    obj_run(sys, (char *)"del('a')");
    obj_deinit(sys);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, test9)
{
    MimiObj *sys = newRootObj((char *)"sys", New_SysObj);
    obj_run(sys, (char *)"ls()");
    obj_setPtr(sys, (char *)"baseClass", (void *)New_TinyObj);
    obj_run(sys, (char *)"ls()");
    obj_deinit(sys);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, test10)
{
    MimiObj *root = newRootObj((char *)"root", New_MYROOT1);
    obj_run(root, (char *)"res = usart.send('hello world')");
    obj_deinit(root);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, newObject)
{
    MimiObj *root = newRootObj((char *)"root", New_MYROOT1);
    obj_newObj(root, (char *)"newUart", (char *)"USART");
    obj_deinit(root);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, newObjectAndSetStr)
{
    MimiObj *root = newRootObj((char *)"root", New_MYROOT1);
    obj_newObj(root, (char *)"newUart", (char *)"USART");
    obj_setStr(root, (char *)"newUart.name", (char *)"testName");
    char *name = obj_getStr(root, (char *)"newUart.name");
    printf("the name is %s\r\n", name);
    EXPECT_TRUE(strEqu((char *)"testName", name));
    obj_deinit(root);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

TEST(object_test, noMethod)
{
    MimiObj *root = newRootObj((char *)"root", New_MYROOT1);
    obj_runNoRes(root, (char *)"noDefindMethod()");
    obj_deinit(root);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}

extern DMEM_STATE DMEMS;
TEST(object_test, mem)
{
    EXPECT_EQ(DMEMS.heapUsed, 0);
    EXPECT_EQ(DMEMS.heapUsed, 0);
}