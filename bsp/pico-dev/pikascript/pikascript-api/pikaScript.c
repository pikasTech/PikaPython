/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include <stdio.h>
#include <stdlib.h>

PikaObj *__pikaMain;
PikaObj *pikaScriptInit(void){
    __platform_printf("======[pikascript packages installed]======\r\n");
    __platform_printf("===========================================\r\n");
    __pikaMain = newRootObj("pikaMain", New_PikaMain);
    extern unsigned char pikaModules_py_a[];
    obj_linkLibrary(__pikaMain, pikaModules_py_a);
#if PIKA_INIT_STRING_ENABLE
    obj_run(__pikaMain,
            "import PikaStdLib\n"
            "import PikaStdData\n"
            "mem = PikaStdLib.MemChecker()\n"
            "mem.resetMax()\n"
            "def EXPECT_EQ(test_name, input, expected):\n"
            "    print('-----TEST-----')\n"
            "    print(test_name)\n"
            "    if input != expected:\n"
            "        print('[Error]')\n"
            "        print(test_name)\n"
            "        print('[info] Expected:')\n"
            "        print(expected)\n"
            "        print('[info] Input:')\n"
            "        print(input)\n"
            "        # error\n"
            "        while True:\n"
            "            is_error = 1\n"
            "    else:\n"
            "        print('[ OK  ]')\n"
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    if i == 5:\n"
            "        continue\n"
            "    a = a + i\n"
            "EXPECT_EQ('for_if_continue', a, 40)\n"
            "         \n"
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    if i == 5:\n"
            "        break\n"
            "    a = a + i\n"
            "EXPECT_EQ('for_if_break', a, 10)\n"
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    for k in range(0, 3):\n"
            "        # print(k)\n"
            "        a = a + k\n"
            "EXPECT_EQ('for_for_in_range', a, 30)\n"
            "a = 0\n"
            "for i in range(0, 10):\n"
            "    # print(i)\n"
            "    a = a + i\n"
            "EXPECT_EQ('for_in_range', a, 45)\n"
            "list = PikaStdData.List()\n"
            "list.append(7)\n"
            "list.append('eee')\n"
            "len = list.len()\n"
            "for item in list:\n"
            "    # print(item)\n"
            "    a = item\n"
            "EXPECT_EQ('list_for_loop', a, 'eee')\n"
            "list = PikaStdData.List()\n"
            "list.append(7)\n"
            "list.append('eee')\n"
            "len = list.len()\n"
            "a1 = list.get(0)\n"
            "a2 = list.get(1)\n"
            "res = iter(list)\n"
            "EXPECT_EQ('iter', res, list)\n"
            "dict = PikaStdData.Dict()\n"
            "dict.set('a', 1)\n"
            "dict.set('b', 2)\n"
            "dict.remove('b')\n"
            "a = dict.get('a')\n"
            "b = dict.get('b')\n"
            "EXPECT_EQ('PikaStdData', a, 1)\n"
            "EXPECT_EQ('PikaStdData_2', b, 0)\n"
            "num = 0\n"
            "i = 2\n"
            "for i in range(2,30):\n"
            "    j=2\n"
            "    is_prime = 1\n"
            "    for j in range(2,i):\n"
            "        if i%j==0 :\n"
            "            is_prime = 0\n"
            "            break\n"
            "    if is_prime:\n"
            "        num = num + i\n"
            "EXPECT_EQ('prime_number_30', num, 129)\n"
            "print('====================')\n"
            "print('[Unit Tests Succeed]')\n"
            "print('====================')\n"
            "print('mem used max:')\n"
            "mem.max()\n"
            "print(\"test\\r\\n\\r\\n\")\n"
            "print(\"end\\r\\n\")\n"
            "\n");
#else 
    obj_runModule(__pikaMain, "main");
#endif
    return __pikaMain;
}

