#include "Arm2D_common.h"

uint16_t getColorCode(char* colorName) {
    if (strEqu("white", colorName)) {
        return GLCD_COLOR_WHITE;
    }
    if (strEqu("black", colorName)) {
        return GLCD_COLOR_BLACK;
    }
    if (strEqu("red", colorName)) {
        return GLCD_COLOR_RED;
    }
    if (strEqu("blue", colorName)) {
        return GLCD_COLOR_BLUE;
    }
    if (strEqu("green", colorName)) {
        return GLCD_COLOR_GREEN;
    }
    return 0x00;
}