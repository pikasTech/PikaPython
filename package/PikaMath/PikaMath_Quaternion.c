#include "PikaMath_Quaternion.h"
#include "PikaStdData_List.h"
#include "math.h"

void PikaMath_Quaternion___init__(PikaObj* self) {
    obj_setFloat(self, "x", 0.0);
    obj_setFloat(self, "y", 0.0);
    obj_setFloat(self, "z", 0.0);
    obj_setFloat(self, "w", 1.0);
}

void PikaMath_Quaternion_set(PikaObj* self,
                             pika_float x,
                             pika_float y,
                             pika_float z,
                             pika_float w) {
    obj_setFloat(self, "x", x);
    obj_setFloat(self, "y", y);
    obj_setFloat(self, "z", z);
    obj_setFloat(self, "w", w);
}

pika_float PikaMath_Quaternion_get(PikaObj* self, int key) {
    if (key == 0) {
        return obj_getFloat(self, "x");
    }
    if (key == 1) {
        return obj_getFloat(self, "y");
    }
    if (key == 2) {
        return obj_getFloat(self, "z");
    }
    if (key == 3) {
        return obj_getFloat(self, "w");
    } else {
        obj_setErrorCode(self, 1);
        return 0.0;
    }
}

void PikaMath_Quaternion_add(PikaObj* self, PikaObj* quat) {
    float x_ = obj_getFloat(quat, "x");
    float y_ = obj_getFloat(quat, "y");
    float z_ = obj_getFloat(quat, "z");
    float w_ = obj_getFloat(quat, "w");

    float x = obj_getFloat(self, "x");
    float y = obj_getFloat(self, "y");
    float z = obj_getFloat(self, "z");
    float w = obj_getFloat(self, "w");

    float x_sum = x + x_;
    float y_sum = y + y_;
    float z_sum = z + z_;
    float w_sum = w + w_;

    obj_setFloat(self, "x", x_sum);
    obj_setFloat(self, "y", y_sum);
    obj_setFloat(self, "z", z_sum);
    obj_setFloat(self, "w", w_sum);
}

void PikaMath_Quaternion_sub(PikaObj* self, PikaObj* quat) {
    float x_ = obj_getFloat(quat, "x");
    float y_ = obj_getFloat(quat, "y");
    float z_ = obj_getFloat(quat, "z");
    float w_ = obj_getFloat(quat, "w");

    float x = obj_getFloat(self, "x");
    float y = obj_getFloat(self, "y");
    float z = obj_getFloat(self, "z");
    float w = obj_getFloat(self, "w");

    float x_sub = x - x_;
    float y_sub = y - y_;
    float z_sub = z - z_;
    float w_sub = w - w_;

    obj_setFloat(self, "x", x_sub);
    obj_setFloat(self, "y", y_sub);
    obj_setFloat(self, "z", z_sub);
    obj_setFloat(self, "w", w_sub);
}

void PikaMath_Quaternion_mul(PikaObj* self, PikaObj* quat) {
    float x_ = obj_getFloat(quat, "x");
    float y_ = obj_getFloat(quat, "y");
    float z_ = obj_getFloat(quat, "z");
    float w_ = obj_getFloat(quat, "w");

    float x = obj_getFloat(self, "x");
    float y = obj_getFloat(self, "y");
    float z = obj_getFloat(self, "z");
    float w = obj_getFloat(self, "w");

    float x_mul = w * x_ + x * w_ - y * z_ + z * y_;
    float y_mul = w * y_ + y * w_ + x * z_ - z * x_;
    float z_mul = w * z_ + z * w_ - x * y_ + y * x_;
    float w_mul = w * w_ - x * x_ - y * y_ - z * z_;

    obj_setFloat(self, "x", x_mul);
    obj_setFloat(self, "y", y_mul);
    obj_setFloat(self, "z", z_mul);
    obj_setFloat(self, "w", w_mul);
}
pika_float PikaMath_Quaternion_magnituded(PikaObj* self) {
    float magnituded = PikaMath_Quaternion_magnitudedsquare(self);

    return pow(magnituded, 0.5);
}
pika_float PikaMath_Quaternion_magnitudedsquare(PikaObj* self) {
    float x = obj_getFloat(self, "x");
    float y = obj_getFloat(self, "y");
    float z = obj_getFloat(self, "z");
    float w = obj_getFloat(self, "w");

    float magnitudedsquare = pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(w, 2);

    return magnitudedsquare;
}

void PikaMath_Quaternion_reverse(PikaObj* self) {
    float x = obj_getFloat(self, "x");
    float y = obj_getFloat(self, "y");
    float z = obj_getFloat(self, "z");
    float w = obj_getFloat(self, "w");

    obj_setFloat(self, "x", -x);
    obj_setFloat(self, "y", -y);
    obj_setFloat(self, "z", -z);
    obj_setFloat(self, "w", w);
}

void PikaMath_Quaternion_inverse(PikaObj* self) {
    float x = obj_getFloat(self, "x");
    float y = obj_getFloat(self, "y");
    float z = obj_getFloat(self, "z");
    float w = obj_getFloat(self, "w");

    float mag = PikaMath_Quaternion_magnitudedsquare(self);
    if (mag > 0.0001) {
        obj_setFloat(self, "x", -x / mag);
        obj_setFloat(self, "y", -y / mag);
        obj_setFloat(self, "z", -z / mag);
        obj_setFloat(self, "w", w / mag);
    } else {
        obj_setFloat(self, "x", 0.0);
        obj_setFloat(self, "y", 0.0);
        obj_setFloat(self, "z", 0.0);
        obj_setFloat(self, "w", 1.0);
    }
}

void PikaMath_Quaternion_crossproduct(PikaObj* self, PikaObj* quat) {
    float x_ = obj_getFloat(quat, "x");
    float y_ = obj_getFloat(quat, "y");
    float z_ = obj_getFloat(quat, "z");

    float x = obj_getFloat(self, "x");
    float y = obj_getFloat(self, "y");
    float z = obj_getFloat(self, "z");

    float x_cross = y * z_ - z * y_;
    float y_cross = z * x_ - x * z_;
    float z_cross = x * y_ - y * x_;

    obj_setFloat(self, "x", x_cross);
    obj_setFloat(self, "y", y_cross);
    obj_setFloat(self, "z", z_cross);
    obj_setFloat(self, "w", 0.0);
}

pika_float PikaMath_Quaternion_dot(PikaObj* self, PikaObj* quat) {
    float x_ = obj_getFloat(quat, "x");
    float y_ = obj_getFloat(quat, "y");
    float z_ = obj_getFloat(quat, "z");
    float w_ = obj_getFloat(quat, "w");

    float x = obj_getFloat(self, "x");
    float y = obj_getFloat(self, "y");
    float z = obj_getFloat(self, "z");
    float w = obj_getFloat(self, "w");

    return (x_ * x + y_ * y + z_ * z + w_ * w);
}

void PikaMath_Quaternion_normalize(PikaObj* self) {
    float x = obj_getFloat(self, "x");
    float y = obj_getFloat(self, "y");
    float z = obj_getFloat(self, "z");
    float w = obj_getFloat(self, "w");

    float mag = PikaMath_Quaternion_magnituded(self);
    if (mag > 0.0001) {
        obj_setFloat(self, "x", x / mag);
        obj_setFloat(self, "y", y / mag);
        obj_setFloat(self, "z", z / mag);
        obj_setFloat(self, "w", w / mag);
    } else {
        obj_setFloat(self, "x", 0.0);
        obj_setFloat(self, "y", 0.0);
        obj_setFloat(self, "z", 0.0);
        obj_setFloat(self, "w", 1.0);
    }
}

int PikaMath_Quaternion_isnormalize(PikaObj* self) {
    float mag = PikaMath_Quaternion_magnituded(self);

    if (1.0 == mag) {
        return 1;
    } else {
        return 0;
    }
}

#define PI_DIV_180 (0.017453292519943296)
#define DegToRad(x) ((x)*PI_DIV_180)

void PikaMath_Quaternion_fromEuler(PikaObj* self,
                                   pika_float yaw,
                                   pika_float pitch,
                                   pika_float roll,
                                   int mode) {
    if (mode) {
        pitch = DegToRad(pitch);
        roll = DegToRad(roll);
        yaw = DegToRad(yaw);
    }

    float sinp = sin(pitch / 2);
    float siny = sin(yaw / 2);
    float sinr = sin(roll / 2);

    float cosp = cos(pitch / 2);
    float cosy = cos(yaw / 2);
    float cosr = cos(roll / 2);

    float x = sinr * cosp * cosy - cosr * sinp * siny;
    float y = cosr * sinp * cosy + sinr * cosp * siny;
    float z = cosr * cosp * siny - sinr * sinp * cosy;
    float w = cosr * cosp * cosy + sinr * sinp * siny;

    obj_setFloat(self, "x", x);
    obj_setFloat(self, "y", y);
    obj_setFloat(self, "z", z);
    obj_setFloat(self, "w", w);
}

PikaObj* PikaMath_Quaternion_toEuler(PikaObj* self) {
    float x = obj_getFloat(self, "x");
    float y = obj_getFloat(self, "y");
    float z = obj_getFloat(self, "z");
    float w = obj_getFloat(self, "w");

    float roll = atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
    float pitch = asin(2 * (w * y - z * x));
    float yaw = atan2(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));

    float rpy[3] = {roll, pitch, yaw};

    PikaObj* list = newNormalObj(New_PikaStdData_List);
    PikaStdData_List___init__(list);

    for (int i = 0; i < 3; i++) {
        Arg* token_arg = arg_newFloat(rpy[i]);
        /* 添加到 list 对象 */
        PikaStdData_List_append(list, token_arg);
        /* 销毁 arg */
        arg_deinit(token_arg);
    }

    return list;
}