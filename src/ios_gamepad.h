#ifndef IOS_GAMEPAD_H
#define IOS_GAMEPAD_H

#include <stdbool.h>
#include "raylib.h"

#if defined(__cplusplus)
extern "C" {
#endif

void IOSGamepad_Init(void);
void IOSGamepad_Update(void);
bool IOSGamepad_IsAvailable(int gamepad);
float IOSGamepad_GetAxisMovement(int gamepad, int axis);
bool IOSGamepad_IsButtonDown(int gamepad, int button);
bool IOSGamepad_IsButtonPressed(int gamepad, int button);
bool IOSGamepad_IsButtonReleased(int gamepad, int button);

#if defined(__cplusplus)
}
#endif

#endif // IOS_GAMEPAD_H
