#include "Arm2D_Tile.h"
#include "Arm2D_Location.h"
#include "Arm2D_Star.h"
#include "Arm2D_Region.h"
#include "Arm2D_common.h"

void Arm2D_Tile___init__(PikaObj* self) {
    arm_2d_tile_t _self = {0};
    if (NULL == obj_getStruct(self, "_self")) {
        obj_setStruct(self, "_self", _self);
    }
}

PikaObj* Arm2D_Tile_generate_child(PikaObj* self,
                                   PikaObj* reg,
                                   int clipRegion) {
    arm_2d_tile_t* _self = obj_getStruct(self, "_self");
    arm_2d_region_t* _reg = obj_getStruct(reg, "_self");
    arm_2d_tile_t _child = {0};
    arm_2d_tile_generate_child(_self, _reg, &_child, clipRegion);
    PikaObj* child = newNormalObj(New_Arm2D_Tile);
    obj_setStruct(child, "_self", _child);
    return child;
}
PikaObj* Arm2D_Tile_get_root(PikaObj* self,
                             PikaObj* validRegion,
                             PikaObj* offset) {
    arm_2d_tile_t* _self = obj_getStruct(self, "_self");
    arm_2d_region_t* _validRegion = obj_getStruct(validRegion, "_self");
    arm_2d_location_t* _offset = obj_getStruct(offset, "_self");
    const arm_2d_tile_t* _root =
        arm_2d_tile_get_root(_self, _validRegion, _offset);
    PikaObj* root = newNormalObj(New_Arm2D_Tile);
    obj_setStruct(root, "_self", _root);
    return root;
}

int Arm2D_Tile_height_compare(PikaObj* self, PikaObj* reference) {
    arm_2d_tile_t* _self = obj_getStruct(self, "_self");
    arm_2d_tile_t* _reference = obj_getStruct(reference, "_self");
    return arm_2d_tile_height_compare(_self, _reference);
}

PikaObj* Arm2D_Tile_region_diff(PikaObj* self, PikaObj* tile) {
    arm_2d_tile_t* _self = obj_getStruct(self, "_self");
    arm_2d_tile_t* _tile = obj_getStruct(tile, "_self");
    arm_2d_region_t _region = {0};
    arm_2d_tile_region_diff(_self, _tile, &_region);
    PikaObj* region = newNormalObj(New_Arm2D_Region);
    obj_setStruct(region, "_self", _region);
    return region;
}

int Arm2D_Tile_shape_compare(PikaObj* self, PikaObj* reference) {
    arm_2d_tile_t* _self = obj_getStruct(self, "_self");
    arm_2d_tile_t* _reference = obj_getStruct(reference, "_self");
    return arm_2d_tile_shape_compare(_self, _reference);
}

int Arm2D_Tile_width_compare(PikaObj* self, PikaObj* reference) {
    arm_2d_tile_t* _self = obj_getStruct(self, "_self");
    arm_2d_tile_t* _reference = obj_getStruct(reference, "_self");
    return arm_2d_tile_width_compare(_self, _reference);
}

int Arm2D_Tile_transform(PikaObj *self, PikaObj* reg, PikaObj* centre){
    arm_2d_tile_t* _self = obj_getStruct(self, "_self");
    arm_2d_region_t* _reg = obj_getStruct(reg, "_self");
    arm_2d_location_t* _centre = obj_getStruct(centre, "_self");
    return arm_2d_tile_transform(_self, _reg, _centre);
}

int Arm2D_Tile_is_root_tile(PikaObj *self){
    arm_2d_tile_t* _self = obj_getStruct(self, "_self");
    return arm_2d_is_root_tile(_self);
}

PikaObj* Arm2D_Tile_get_absolute_location(PikaObj *self){
    arm_2d_tile_t* _self = obj_getStruct(self, "_self");
    arm_2d_location_t _location = {0};
    arm_2d_get_absolute_location(_self, &_location);
    PikaObj* location = newNormalObj(New_Arm2D_Location);
    obj_setStruct(location, "_self", _location);
    return location;
}

void Arm2D_Star___init__(PikaObj *self){
    extern arm_2d_tile_t c_tilePictureSunRGB565;
    obj_setStruct(self, "_self", c_tilePictureSunRGB565);
}
