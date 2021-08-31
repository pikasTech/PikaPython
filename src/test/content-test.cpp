#include "gtest/gtest.h"
extern "C"
{
#include "dataArg.h"
#include "dataString.h"
#include "dataString.h"
}
static int mem;

TEST(content, get)
{
    uint8_t *self = (uint8_t *)pikaMalloc(16);
    self[0] = 'n';
    self[1] = 'a';
    self[2] = 'm';
    self[3] = 'e';
    self[4] = '\0';
    self[5] = 't';
    self[6] = 'y';
    self[7] = 'p';
    self[8] = 'e';
    self[9] = '\0';
    self[10] = 4;
    self[11] = 0;
    self[12] = 1;
    self[13] = 2;
    self[14] = 3;
    self[15] = 4;

    uint16_t contentOffset = content_contentOffset(self);
    uint16_t typeOffset = content_typeOffset(self);
    uint16_t sizeOffset = content_sizeOffset(self);

    char *name = content_getName(self);
    char *type = content_getType(self);
    uint16_t size = content_getSize(self);
    uint8_t *content = content_getContent(self);
    uint16_t totleSize = content_totleSize(self);

    ASSERT_EQ(contentOffset, 12);
    ASSERT_EQ(typeOffset, 5);
    ASSERT_EQ(sizeOffset, 10);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(content[0], 1);
    ASSERT_EQ(content[1], 2);
    ASSERT_EQ(content[2], 3);
    ASSERT_EQ(content[3], 4);
    ASSERT_EQ(totleSize, 16);

    ASSERT_STREQ((char *)"name", name);
    ASSERT_STREQ((char *)"type", type);

    content_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(content, init)
{
    uint8_t contentIn[4] = {0};
    contentIn[0] = 1;
    contentIn[1] = 2;
    contentIn[2] = 3;
    contentIn[3] = 4;
    uint16_t sizeIn = 4;
    uint8_t *self = content_init((char *)"name", (char *)"type", contentIn, 4);

    uint16_t typeOffset = content_typeOffset(self);
    uint16_t sizeOffset = content_sizeOffset(self);
    uint16_t contentOffset = content_contentOffset(self);
    uint16_t totleSize = content_totleSize(self);

    char *name = content_getName(self);
    char *type = content_getType(self);
    uint16_t size = content_getSize(self);
    uint8_t *content = content_getContent(self);

    ASSERT_EQ(contentOffset, 12);
    ASSERT_EQ(typeOffset, 5);
    ASSERT_EQ(sizeOffset, 10);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(content[0], 1);
    ASSERT_EQ(content[1], 2);
    ASSERT_EQ(content[2], 3);
    ASSERT_EQ(content[3], 4);
    ASSERT_EQ(totleSize, 16);

    ASSERT_STREQ((char *)"name", name);
    ASSERT_STREQ((char *)"type", type);

    content_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}

TEST(content, set)
{
    uint8_t contentIn[4] = {0};
    contentIn[0] = 1;
    contentIn[1] = 2;
    contentIn[2] = 3;
    contentIn[3] = 4;
    uint8_t *self = content_init((char *)"", (char *)"", NULL, 0);
    self = content_setName(self, (char *)"name");
    self = content_setType(self, (char *)"type");
    self = content_setContent(self, contentIn, 4);

    uint16_t typeOffset = content_typeOffset(self);
    uint16_t sizeOffset = content_sizeOffset(self);
    uint16_t contentOffset = content_contentOffset(self);
    uint16_t totleSize = content_totleSize(self);

    char *name = content_getName(self);
    char *type = content_getType(self);
    uint16_t size = content_getSize(self);
    uint8_t *content = content_getContent(self);

    ASSERT_EQ(contentOffset, 12);
    ASSERT_EQ(typeOffset, 5);
    ASSERT_EQ(sizeOffset, 10);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(content[0], 1);
    ASSERT_EQ(content[1], 2);
    ASSERT_EQ(content[2], 3);
    ASSERT_EQ(content[3], 4);
    ASSERT_EQ(totleSize, 16);

    ASSERT_STREQ("name", name);
    ASSERT_STREQ("type", type);

    content_deinit(self);
    EXPECT_EQ(pikaMemNow(), 0);
}