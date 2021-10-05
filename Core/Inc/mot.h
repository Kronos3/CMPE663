//
// Created by tumbar on 9/22/21.
//

#ifndef CMPE663_MOT_H
#define CMPE663_MOT_H


#include "types.h"

typedef enum
{
    MOT_INVALID = -1,
    MOT_SERVO_1 = 0,
    MOT_SERVO_2,
    MOT_N
} MotorId;

typedef I32 MotPos;

MotPos mot_get_position(MotorId mid);
I32 mot_set_position(MotorId mid, MotPos pos);

#endif //CMPE663_MOT_H
