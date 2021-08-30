#include "gtest/gtest.h"
extern "C"
{
#include "dataLink.h"
#include "dataTest.h"
#include <stdio.h>
}

TEST(link_test, test1)
{
    Link *link = New_link(NULL);
    dataTest_t *data1 = New_dataTest(NULL);
    dataTest_t *data2 = New_dataTest(NULL);

    data1->a = 2;
    data1->b = 5;
    link_addNode(link, data1, (contentDeinitFun)data1->deinit);
    data2->a = 4;
    data2->b = 21;
    link_addNode(link, data2, (contentDeinitFun)data2->deinit);

    EXPECT_EQ(2, link_getSize(link));
    link_deinit(link);
}
TEST(link_test, mem)
{
    EXPECT_EQ(pikaMemNow(), 0);
}
