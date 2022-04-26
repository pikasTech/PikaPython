#include "gtest/gtest.h"
extern "C" {
#include "PikaMain.h"
#include "PikaParser.h"
#include "PikaStdLib_MemChecker.h"
#include "PikaVM.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataStrs.h"
#include "pikaScript.h"
#include "pika_config_gtest.h"
}

TEST(gc, root) {
    PikaObj* root = newRootObj((char*)"root", New_PikaMain);
    EXPECT_EQ(obj_refcntNow(root), 1);
    Arg* refArg = arg_setRefObj(NULL, (char*)"", root);
    EXPECT_EQ(obj_refcntNow(root), 2);
    arg_deinit(refArg);
    EXPECT_EQ(obj_refcntNow(root), 1);
    obj_deinit(root);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(gc, ref1) {
    /* init */
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, (char*)"mem = PikaStdLib.MemChecker()");
    PikaObj* mem = (PikaObj*)obj_getPtr(pikaMain, (char*)"mem");
    EXPECT_EQ(obj_refcntNow(mem), 1);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(gc, ref121) {
    /* init */
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, (char*)"mem = PikaStdLib.MemChecker()");
    PikaObj* mem = (PikaObj*)obj_getPtr(pikaMain, (char*)"mem");
    EXPECT_EQ(obj_refcntNow(mem), 1);
    obj_run(pikaMain, (char*)"mem2 = mem");
    EXPECT_EQ(obj_refcntNow(mem), 2);
    obj_removeArg(pikaMain, (char*)"mem2");
    EXPECT_EQ(obj_refcntNow(mem), 1);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(gc, ref12) {
    /* init */
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, (char*)"mem = PikaStdLib.MemChecker()");
    PikaObj* mem = (PikaObj*)obj_getPtr(pikaMain, (char*)"mem");
    EXPECT_EQ(obj_refcntNow(mem), 1);
    obj_run(pikaMain, (char*)"mem2 = mem");
    EXPECT_EQ(obj_refcntNow(mem), 2);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(gc, ref1210) {
    /* init */
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, (char*)"mem = PikaStdLib.MemChecker()");
    PikaObj* mem = (PikaObj*)obj_getPtr(pikaMain, (char*)"mem");
    EXPECT_EQ(obj_refcntNow(mem), 1);
    obj_run(pikaMain, (char*)"mem2 = mem");
    EXPECT_EQ(obj_refcntNow(mem), 2);
    obj_removeArg(pikaMain, (char*)"mem2");
    EXPECT_EQ(obj_refcntNow(mem), 1);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(gc, ref_move) {
    /* init */
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, (char*)"mem = PikaStdLib.MemChecker()");
    PikaObj* mem = (PikaObj*)obj_getPtr(pikaMain, (char*)"mem");
    EXPECT_EQ(obj_refcntNow(mem), 1);
    obj_run(pikaMain, (char*)"mem2 = mem");
    EXPECT_EQ(obj_refcntNow(mem), 2);
    obj_removeArg(pikaMain, (char*)"mem");
    EXPECT_EQ(obj_refcntNow(mem), 1);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(gc, factory) {
    /* init */
    PikaObj* pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
    /* run */
    obj_run(pikaMain, (char*)
    "def factory():\n"
    "    _mem = PikaStdLib.MemChecker()\n"
    "    return _mem\n"
    "mem = factory()\n"
    );
    /* collect */
    PikaObj* mem = obj_getObj(pikaMain, (char*)"mem");
    EXPECT_EQ(obj_refcntNow(mem), 1);
    /* deinit */
    obj_deinit(pikaMain);
    EXPECT_EQ(pikaMemNow(), 0);
}
