#include "Arm2D_common.h"
#include "Arm2D_Box.h"

void Arm2D_Box_init(PikaObj *self) {
	obj_setInt(self, "posX", 0);
	obj_setInt(self, "posY", 0);
	obj_setInt(self, "sizeX", 50);
	obj_setInt(self, "sizeY", 50);
	obj_setStr(self, "color", "blue");
}

void Arm2D_Box_update(PikaObj *self) {
	void *target_tile = pika_arm2d_window.pfb_tile_now;

	int posX = obj_getInt(self, "posX");
	int posY = obj_getInt(self, "posY");
	int sizeX = obj_getInt(self, "sizeX");
	int sizeY = obj_getInt(self, "sizeY");
	char *color = obj_getStr(self, "color");
	arm_2d_region_t tBox = { .tLocation = { posX, posY }, .tSize = { sizeX,
			sizeY }, };
	arm_2d_rgb16_fill_colour(target_tile, &tBox, getColorCode(color));
}

void Arm2D_Box_setColor(PikaObj *self, char *color) {
	obj_setStr(self, "color", color);
}

void Arm2D_Box_setSize(PikaObj *self, int x, int y) {
	obj_setInt(self, "sizeX", x);
	obj_setInt(self, "sizeY", y);
}
