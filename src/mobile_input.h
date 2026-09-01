#ifndef MOBILE_INPUT_H
#define MOBILE_INPUT_H

#include "raylib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MobileInputState {
    bool up_down;
    bool down_down;
    bool left_down;
    bool right_down;
    bool up_pressed;
    bool down_pressed;
    bool left_pressed;
    bool right_pressed;
    bool fire_down;
    bool turbo_down;
    bool aim_down;
    bool dash_pressed;
    bool a_pressed;
    bool b_pressed;
    bool pause_pressed;
} MobileInputState;

void MobileInput_Init(void);
void MobileInput_Update(bool gameplay_mode);
MobileInputState MobileInput_GetState(void);
void MobileInput_Draw(bool gameplay_mode);
void MobileInput_Shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
