#include "gtest/gtest.h"
extern "C" {
#include "dataArgs.h"
#include "dataString.h"
}

TEST(bitmap, init) {
    uint8_t* bitmap = bitmap_init(10);
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(0, bitmap_get(bitmap, i));
    }
    bitmap_deinit(bitmap);
}

TEST(bitmap, set_all) {
    uint8_t* bitmap = bitmap_init(10);
    for (int i = 0; i < 10; i++) {
        bitmap_set(bitmap, i, 1);
    }
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(1, bitmap_get(bitmap, i));
    }
    bitmap_deinit(bitmap);
}

TEST(bitmap, set_half) {
    uint8_t* bitmap = bitmap_init(10);
    for (int i = 0; i < 5; i++) {
        bitmap_set(bitmap, i, 1);
    }
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(1, bitmap_get(bitmap, i));
    }
    for (int i = 5; i < 10; i++) {
        EXPECT_EQ(0, bitmap_get(bitmap, i));
    }
    bitmap_deinit(bitmap);
}

TEST(bitmap, set_half_2) {
    uint8_t* bitmap = bitmap_init(10);
    for (int i = 0; i < 10; i += 2) {
        bitmap_set(bitmap, i, 1);
    }
    for (int i = 0; i < 10; i += 2) {
        EXPECT_EQ(1, bitmap_get(bitmap, i));
    }
    for (int i = 1; i < 10; i += 2) {
        EXPECT_EQ(0, bitmap_get(bitmap, i));
    }
    bitmap_deinit(bitmap);
}

TEST(bitmap, reset_half_2) {
    uint8_t* bitmap = bitmap_init(10);
    for (int i = 0; i < 10; i++) {
        bitmap_set(bitmap, i, 1);
    }
    for (int i = 0; i < 10; i += 2) {
        bitmap_set(bitmap, i, 0);
    }
    for (int i = 0; i < 10; i += 2) {
        EXPECT_EQ(0, bitmap_get(bitmap, i));
    }
    for (int i = 1; i < 10; i += 2) {
        EXPECT_EQ(1, bitmap_get(bitmap, i));
    }
    bitmap_deinit(bitmap);
}