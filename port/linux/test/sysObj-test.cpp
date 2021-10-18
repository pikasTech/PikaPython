#include "gtest/gtest.h"
extern "C" {
#include "BaseObj.h"
#include "PikaStdLib_SysObj.h"
#include "TinyObj.h"
}

TEST(sysObj, print) {
  PikaObj* obj = newRootObj((char*)"test", New_PikaStdLib_SysObj);
  Args* res = obj_runDirect(obj, (char*)"print('hello world')");
  char* sysOut = args_getSysOut(res);
  int errCode = args_getErrorCode(res);
  printf("sysout = %s\r\n", sysOut);
  ASSERT_STREQ((char*)"hello world", sysOut);
  ASSERT_EQ(0, errCode);
  args_deinit(res);
  obj_deinit(obj);
  EXPECT_EQ(pikaMemNow(), 0);
}

TEST(sysObj, noMethod) {
  PikaObj* obj = newRootObj((char*)"test", New_PikaStdLib_SysObj);
  Args* res = obj_runDirect(obj, (char*)"printttt('hello world')");
  char* sysOut = args_getSysOut(res);
  int errCode = args_getErrorCode(res);
  printf("sysout = %s\r\n", sysOut);
  ASSERT_EQ(1, strEqu((char*)"[error] runner: method no found.", sysOut));
  ASSERT_EQ(2, errCode);
  args_deinit(res);
  obj_deinit(obj);
  EXPECT_EQ(pikaMemNow(), 0);
}
