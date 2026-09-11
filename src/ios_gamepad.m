#include "ios_gamepad.h"

#if defined(__APPLE__)
#import <TargetConditionals.h>
#if TARGET_OS_IOS

#import <Foundation/Foundation.h>
#import <GameController/GameController.h>
#include <string.h>

#define MAX_GAMEPADS 4
#define MAX_BUTTONS 32

static bool s_initialized = false;
static bool s_currentButtonStates[MAX_GAMEPADS][MAX_BUTTONS] = {{0}};
static bool s_previousButtonStates[MAX_GAMEPADS][MAX_BUTTONS] = {{0}};
static float s_axisValues[MAX_GAMEPADS][8] = {{0}};

void IOSGamepad_Init(void)
{
    if (s_initialized) return;
    s_initialized = true;

    [GCController startWirelessControllerDiscoveryWithCompletionHandler:nil];

    [[NSNotificationCenter defaultCenter] addObserverForName:GCControllerDidConnectNotification
                                                      object:nil
                                                       queue:[NSOperationQueue mainQueue]
                                                  usingBlock:^(NSNotification *note) {
        GCController *controller = note.object;
        NSLog(@"[Astrallitic Gamepad] Connected: %@", controller.vendorName);
    }];

    [[NSNotificationCenter defaultCenter] addObserverForName:GCControllerDidDisconnectNotification
                                                      object:nil
                                                       queue:[NSOperationQueue mainQueue]
                                                  usingBlock:^(NSNotification *note) {
        GCController *controller = note.object;
        NSLog(@"[Astrallitic Gamepad] Disconnected: %@", controller.vendorName);
    }];
}

static GCController* GetControllerAtIndex(int index)
{
    NSArray<GCController *> *controllers = [GCController controllers];
    if (index < 0 || index >= (int)[controllers count]) {
        if (index == 0 && [GCController current] != nil) {
            return [GCController current];
        }
        return nil;
    }
    return controllers[index];
}

void IOSGamepad_Update(void)
{
    if (!s_initialized) {
        IOSGamepad_Init();
    }

    memcpy(s_previousButtonStates, s_currentButtonStates, sizeof(s_currentButtonStates));
    memset(s_currentButtonStates, 0, sizeof(s_currentButtonStates));
    memset(s_axisValues, 0, sizeof(s_axisValues));

    NSArray<GCController *> *controllers = [GCController controllers];
    int count = (int)[controllers count];
    if (count == 0 && [GCController current] != nil) {
        controllers = @[[GCController current]];
        count = 1;
    }

    for (int i = 0; i < count && i < MAX_GAMEPADS; i++) {
        GCController *controller = controllers[i];
        GCExtendedGamepad *extended = controller.extendedGamepad;
        GCMicroGamepad *micro = controller.microGamepad;

        if (extended != nil) {
            // D-Pad
            s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_FACE_UP]    = extended.dpad.up.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_FACE_DOWN]  = extended.dpad.down.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_FACE_LEFT]  = extended.dpad.left.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_FACE_RIGHT] = extended.dpad.right.isPressed;

            // Action Buttons
            s_currentButtonStates[i][GAMEPAD_BUTTON_RIGHT_FACE_DOWN]  = extended.buttonA.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_RIGHT_FACE_RIGHT] = extended.buttonB.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_RIGHT_FACE_LEFT]  = extended.buttonX.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_RIGHT_FACE_UP]    = extended.buttonY.isPressed;

            // Bumpers & Triggers
            s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_TRIGGER_1]  = extended.leftShoulder.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_RIGHT_TRIGGER_1] = extended.rightShoulder.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_TRIGGER_2]  = (extended.leftTrigger.value > 0.3f);
            s_currentButtonStates[i][GAMEPAD_BUTTON_RIGHT_TRIGGER_2] = (extended.rightTrigger.value > 0.3f);

            // Thumbsticks (L3 / R3)
            if (@available(iOS 12.1, *)) {
                if (extended.leftThumbstickButton != nil) {
                    s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_THUMB] = extended.leftThumbstickButton.isPressed;
                }
                if (extended.rightThumbstickButton != nil) {
                    s_currentButtonStates[i][GAMEPAD_BUTTON_RIGHT_THUMB] = extended.rightThumbstickButton.isPressed;
                }
            }

            // Menu / Options
            if (@available(iOS 13.0, *)) {
                if (extended.buttonMenu.isPressed) {
                    s_currentButtonStates[i][GAMEPAD_BUTTON_MIDDLE_RIGHT] = true;
                }
                if (extended.buttonOptions != nil && extended.buttonOptions.isPressed) {
                    s_currentButtonStates[i][GAMEPAD_BUTTON_MIDDLE_LEFT] = true;
                }
            }

            // Left Stick: invert Y because iOS +Y is up and Raylib +Y is down
            s_axisValues[i][GAMEPAD_AXIS_LEFT_X] = extended.leftThumbstick.xAxis.value;
            s_axisValues[i][GAMEPAD_AXIS_LEFT_Y] = -extended.leftThumbstick.yAxis.value;

            // Right Stick: invert Y
            s_axisValues[i][GAMEPAD_AXIS_RIGHT_X] = extended.rightThumbstick.xAxis.value;
            s_axisValues[i][GAMEPAD_AXIS_RIGHT_Y] = -extended.rightThumbstick.yAxis.value;

            s_axisValues[i][GAMEPAD_AXIS_LEFT_TRIGGER] = extended.leftTrigger.value;
            s_axisValues[i][GAMEPAD_AXIS_RIGHT_TRIGGER] = extended.rightTrigger.value;
        } else if (micro != nil) {
            s_currentButtonStates[i][GAMEPAD_BUTTON_RIGHT_FACE_DOWN]  = micro.buttonA.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_RIGHT_FACE_RIGHT] = micro.buttonX.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_FACE_UP]    = micro.dpad.up.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_FACE_DOWN]  = micro.dpad.down.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_FACE_LEFT]  = micro.dpad.left.isPressed;
            s_currentButtonStates[i][GAMEPAD_BUTTON_LEFT_FACE_RIGHT] = micro.dpad.right.isPressed;
        }
    }
}

bool IOSGamepad_IsAvailable(int gamepad)
{
    if (!s_initialized) IOSGamepad_Init();
    if (gamepad < 0 || gamepad >= MAX_GAMEPADS) return false;
    return (GetControllerAtIndex(gamepad) != nil);
}

float IOSGamepad_GetAxisMovement(int gamepad, int axis)
{
    if (gamepad < 0 || gamepad >= MAX_GAMEPADS) return 0.0f;
    if (axis < 0 || axis >= 8) return 0.0f;
    return s_axisValues[gamepad][axis];
}

bool IOSGamepad_IsButtonDown(int gamepad, int button)
{
    if (gamepad < 0 || gamepad >= MAX_GAMEPADS) return false;
    if (button < 0 || button >= MAX_BUTTONS) return false;
    return s_currentButtonStates[gamepad][button];
}

bool IOSGamepad_IsButtonPressed(int gamepad, int button)
{
    if (gamepad < 0 || gamepad >= MAX_GAMEPADS) return false;
    if (button < 0 || button >= MAX_BUTTONS) return false;
    return (s_currentButtonStates[gamepad][button] && !s_previousButtonStates[gamepad][button]);
}

bool IOSGamepad_IsButtonReleased(int gamepad, int button)
{
    if (gamepad < 0 || gamepad >= MAX_GAMEPADS) return false;
    if (button < 0 || button >= MAX_BUTTONS) return false;
    return (!s_currentButtonStates[gamepad][button] && s_previousButtonStates[gamepad][button]);
}

#endif // TARGET_OS_IOS
#else
// Non-Apple fallback / local verification stubs
void IOSGamepad_Init(void) {}
void IOSGamepad_Update(void) {}
bool IOSGamepad_IsAvailable(int gamepad) { (void)gamepad; return false; }
float IOSGamepad_GetAxisMovement(int gamepad, int axis) { (void)gamepad; (void)axis; return 0.0f; }
bool IOSGamepad_IsButtonDown(int gamepad, int button) { (void)gamepad; (void)button; return false; }
bool IOSGamepad_IsButtonPressed(int gamepad, int button) { (void)gamepad; (void)button; return false; }
bool IOSGamepad_IsButtonReleased(int gamepad, int button) { (void)gamepad; (void)button; return false; }
#endif