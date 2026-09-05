#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

#define SCREEN_W 240
#define SCREEN_H 160

#define MAX_BULLETS 20
#define MAX_E_BULLETS 15
#define MAX_ENEMIES 14
#define MAX_EXPLO 6
#define MAX_TITLE_STARS 24
#define MAX_MENU_STARS 20
#define MAX_GAME_STARS 18
#define MAX_BLOOD_DROPS 25
#define MAX_TEXTS 15

typedef struct { int x, y, vx, vy, active; } Bullet;
typedef struct { int x, y, hp, active, type, timer, freeze_timer, poison_timer, target_x, target_y; } Enemy;
typedef struct { int x, y, active, type, life_timer; } PowerUp;
typedef struct { int x, y, timer, active; } ExplosionEffect;
typedef struct { float x, y; char text[16]; Color color; int timer; } FloatingText;
typedef struct { int x, y, speed; Color color; } TitleStar;
typedef struct { int x, y, vx, vy; Color color; } MenuStar;
typedef struct { int x, y, speed; Color color; } GameStar;
typedef struct { int x, y, speed; Color color; } BloodDrop;

typedef enum {
    INPUT_KEYBOARD = 0,
    INPUT_GAMEPAD
} InputDeviceType;

typedef struct {
    int key_up;
    int key_down;
    int key_left;
    int key_right;
    int key_shoot;
    int key_turbo;
    int key_aim;
    int key_dash;
} KeyBindings;

typedef struct {
    int btn_up;
    int btn_down;
    int btn_left;
    int btn_right;
    int btn_shoot;
    int btn_turbo;
    int btn_aim;
    int btn_dash;
} PadBindings;

typedef struct {
    int target_fps;
    int vsync;
    int res_index;
    int screen_mode;
    int language;
    int vol_bgm;
    int vol_sfx;
    int crt_filter;
} GameConfig;

#define LANG_COUNT 5

#define NUM_ACHIEVEMENTS 43

typedef enum {
    STR_PLAY = 0,
    STR_OPTIONS,
    STR_RECORDS,
    STR_VOL_BGM,
    STR_VOL_SFX,
    STR_FILTER,
    STR_CONTROLS,
    STR_RESOLUTION,
    STR_SCREEN_MODE,
    STR_FPS,
    STR_VSYNC,
    STR_LANGUAGE,
    STR_DELETE_RECORDS,
    STR_CONFIRM_DELETE_TITLE,
    STR_CONFIRM_DELETE_SUB,
    STR_CANCEL,
    STR_CONFIRM_DELETE_BTN,
    STR_DIFF_TITLE,
    STR_DIFF_EASY,
    STR_DIFF_NORMAL,
    STR_DIFF_HARD,
    STR_DIFF_HELL,
    STR_PAUSE,
    STR_RESUME,
    STR_MAIN_MENU,
    STR_GAME_OVER,
    STR_POINTS,
    STR_RETRY,
    STR_QUIT_PROMPT,
    STR_QUIT_SUB,
    STR_YES,
    STR_NO,
    STR_CTRL_TITLE,
    STR_CTRL_UP,
    STR_CTRL_DOWN,
    STR_CTRL_LEFT,
    STR_CTRL_RIGHT,
    STR_CTRL_SHOOT,
    STR_CTRL_TURBO,
    STR_CTRL_AIM,
    STR_CTRL_DASH,
    STR_REBIND_TITLE,
    STR_PRESS_KEY,
    STR_CANCEL_KEY,
    STR_INVALID_ACTION,
    STR_DUPLICATE_KEY,
    STR_USE_OTHER_KEY,
    STR_PRESS_BACK,
    STR_PRESENTS,
    STR_READY,
    STR_GO,
    STR_WAVE,
    STR_GAMEPAD_CONNECTED,
    STR_KEYBOARD_CONNECTED,
    STR_TOUCH_CONNECTED,
    STR_FREEZE,
    STR_POISON,
    STR_LOGROS,
    STR_ACH_1_TITLE, STR_ACH_1_DESC,
    STR_ACH_2_TITLE, STR_ACH_2_DESC,
    STR_ACH_3_TITLE, STR_ACH_3_DESC,
    STR_ACH_4_TITLE, STR_ACH_4_DESC,
    STR_ACH_5_TITLE, STR_ACH_5_DESC,
    STR_ACH_6_TITLE, STR_ACH_6_DESC,
    STR_ACH_7_TITLE, STR_ACH_7_DESC,
    STR_ACH_8_TITLE, STR_ACH_8_DESC,
    STR_ACH_9_TITLE, STR_ACH_9_DESC,
    STR_ACH_10_TITLE, STR_ACH_10_DESC,
    STR_ACH_11_TITLE, STR_ACH_11_DESC,
    STR_ACH_12_TITLE, STR_ACH_12_DESC,
    STR_ACH_13_TITLE, STR_ACH_13_DESC,
    STR_ACH_14_TITLE, STR_ACH_14_DESC,
    STR_ACH_15_TITLE, STR_ACH_15_DESC,
    STR_ACH_16_TITLE, STR_ACH_16_DESC,
    STR_ACH_17_TITLE, STR_ACH_17_DESC,
    STR_ACH_18_TITLE, STR_ACH_18_DESC,
    STR_ACH_19_TITLE, STR_ACH_19_DESC,
    STR_ACH_20_TITLE, STR_ACH_20_DESC,
    STR_ACH_21_TITLE, STR_ACH_21_DESC,
    STR_ACH_22_TITLE, STR_ACH_22_DESC,
    STR_ACH_23_TITLE, STR_ACH_23_DESC,
    STR_ACH_24_TITLE, STR_ACH_24_DESC,
    STR_ACH_25_TITLE, STR_ACH_25_DESC,
    STR_ACH_26_TITLE, STR_ACH_26_DESC,
    STR_ACH_27_TITLE, STR_ACH_27_DESC,
    STR_ACH_28_TITLE, STR_ACH_28_DESC,
    STR_ACH_29_TITLE, STR_ACH_29_DESC,
    STR_ACH_30_TITLE, STR_ACH_30_DESC,
    STR_ACH_31_TITLE, STR_ACH_31_DESC,
    STR_ACH_32_TITLE, STR_ACH_32_DESC,
    STR_ACH_33_TITLE, STR_ACH_33_DESC,
    STR_ACH_34_TITLE, STR_ACH_34_DESC,
    STR_ACH_35_TITLE, STR_ACH_35_DESC,
    STR_ACH_36_TITLE, STR_ACH_36_DESC,
    STR_ACH_37_TITLE, STR_ACH_37_DESC,
    STR_ACH_38_TITLE, STR_ACH_38_DESC,
    STR_ACH_39_TITLE, STR_ACH_39_DESC,
    STR_ACH_40_TITLE, STR_ACH_40_DESC,
    STR_ACH_41_TITLE, STR_ACH_41_DESC,
    STR_ACH_42_TITLE, STR_ACH_42_DESC,
    STR_ACH_43_TITLE, STR_ACH_43_DESC,
    STR_TOUCH_BACK,
    STR_ACH_HINT,
    STR_ACH_STATUS_UNLOCKED,
    STR_ACH_PROGRESS_LBL,
    STR_CTRL_MOVE,
    STR_TOUCH_INFO_TITLE,
    STR_COUNT
} StringId;

extern InputDeviceType g_last_input_device;
extern int g_device_toast_timer;
extern InputDeviceType g_toast_device;
extern KeyBindings g_keys;
extern PadBindings g_pad;
extern GameConfig g_config;
extern const int g_resolutions[4][2];
extern const char* const g_lang_names[LANG_COUNT];

const char* T(StringId id);

#define GBA_COLOR(r, g, b) (Color){ (unsigned char)((r) * 255 / 31), (unsigned char)((g) * 255 / 31), (unsigned char)((b) * 255 / 31), 255 }

#define C_BG            GBA_COLOR(0, 0, 0)
#define C_RED           GBA_COLOR(14, 1, 2)
#define C_PLAYER        GBA_COLOR(31, 4, 4)
#define C_SHIELD        GBA_COLOR(2, 16, 31)
#define C_RAPID         GBA_COLOR(31, 28, 0)
#define C_BULLET        GBA_COLOR(31, 31, 0)
#define C_E_BULLET      GBA_COLOR(31, 2, 2)
#define C_ENEMY_N       GBA_COLOR(0, 28, 4)
#define C_ENEMY_F       GBA_COLOR(0, 26, 31)
#define C_ENEMY_T       GBA_COLOR(24, 20, 0)
#define C_BOSS          GBA_COLOR(28, 0, 28)
#define C_SNIPER        GBA_COLOR(18, 18, 28)
#define C_DIVISOR       GBA_COLOR(24, 4, 24)
#define C_KAMIKAZE      GBA_COLOR(31, 10, 0)
#define C_PATROLLER     GBA_COLOR(0, 14, 4)
#define C_TELEPORT      GBA_COLOR(20, 0, 31)
#define C_POISON_BULLET GBA_COLOR(20, 0, 31)
#define C_POISON_TINT   GBA_COLOR(18, 2, 26)
#define C_HEART         GBA_COLOR(31, 2, 4)
#define C_CYAN          GBA_COLOR(0, 31, 31)
#define C_YELLOW        GBA_COLOR(31, 31, 0)
#define C_GREEN         GBA_COLOR(0, 31, 0)
#define C_ICE           GBA_COLOR(15, 25, 31)
#define C_BOTTLE        GBA_COLOR(0, 24, 6)
#define C_NUKE          GBA_COLOR(6, 6, 6)
#define C_TITLE_BG      GBA_COLOR(1, 2, 6)
#define C_DIFF_BG       GBA_COLOR(1, 2, 8)
#define C_REC_BG        GBA_COLOR(0, 4, 8)
#define C_OPT_BG        GBA_COLOR(3, 2, 6)
#define C_PAUSE_OPT_BG  GBA_COLOR(2, 5, 10)
#define C_CTRL_BG       GBA_COLOR(2, 6, 4)

#define C_XBOX_A        GBA_COLOR(2, 28, 6)
#define C_XBOX_B        GBA_COLOR(31, 4, 4)
#define C_XBOX_X        GBA_COLOR(4, 14, 31)
#define C_XBOX_Y        GBA_COLOR(31, 26, 0)

#endif