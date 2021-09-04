#include "gtest/gtest.h"
extern "C"
{
#include "PikaBlock.h"
#include "dataStrs.h"
}

TEST(block, init)
{
    PikaObj *block = block_init();
    block_deinit(block);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(block, push)
{
    PikaObj *block = block_init();
    block_pushLine(block, (char *)"line1");
    block_pushLine(block, (char *)"line2");
    block_deinit(block);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(block, pop)
{
    PikaObj *block = block_init();
    block_pushLine(block, (char *)"line1");
    block_pushLine(block, (char *)"line2");
    char *line1 = block_popLine(block);
    char *line2 = block_popLine(block);
    char *line3 = block_popLine(block);
    ASSERT_STREQ(line1, (char *)"line1");
    ASSERT_STREQ(line2, (char *)"line2");
    ASSERT_STREQ(line3, (char *)"");
    block_deinit(block);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(block, assert1)
{
    PikaObj *block = block_init();
    block_setAssert(block, (char *)"1");
    int res = block_checkAssert(block);
    ASSERT_EQ(1, res);
    block_deinit(block);
}

void block_pushLine(PikaObj *self, char *line);
