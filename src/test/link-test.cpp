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
    link_addNode(link, data1, (contantDeinitFun)data1->deinit);
    data2->a = 4;
    data2->b = 21;
    link_addNode(link, data2, (contantDeinitFun)data2->deinit);

    dataTest_t *data1InLink = (dataTest_t *)link_getNode(link, 0)->contant;
    dataTest_t *data2InLink = (dataTest_t *)link_getNode(link, 1)->contant;
    EXPECT_EQ(2, data1InLink->a);
    EXPECT_EQ(5, data1InLink->b);
    EXPECT_EQ(4, data2InLink->a);
    EXPECT_EQ(21, data2InLink->b);
    EXPECT_EQ(2, link_getSize(link));
    link_removeNode(link, data1InLink);
    link_deinit(link);
}
TEST(link_test, mem)
{
    EXPECT_EQ(pikaMemNow(), 0);
}