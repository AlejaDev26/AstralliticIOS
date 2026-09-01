#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "raylib.h"
#include "types.h"

Color GetGamepadButtonColorCustom(int btn);
const char* GetGamepadButtonNameCustom(int btn);
const char* GetKeyNameCustom(int key);
extern const char* crt_shader_code;
extern const char* crt_vertex_shader_code;

Color BlendColorAlpha(Color c, int alpha);
void DrawCharCustom(char c, int x, int y, Color color, int scale);
void DrawStringCustom(const char* str, int x, int y, Color color, int scale);
int MeasureStringCustom(const char* str, int scale);
void DrawCenteredStringCustom(const char* str, int y, Color color, int scale);
void DrawHeaderCharCustom(char c, int x, int y, Color main_color, Color shadow_color, int scale);
void DrawHeaderCenteredStringCustom(const char* str, int y, Color main_color, Color shadow_color, int scale);
void DrawKeyboardIcon16Bit(int x, int y, bool is_hovered);
void DrawGamepadIcon16Bit(int x, int y, bool is_hovered);
void DrawActionIcon16Bit(int type, int x, int y, bool is_sel);
void DrawLanguageFlag16Bit(int lang, int x, int y);
void DrawSpaceShipCursor16Bit(int x, int y, bool is_clicking);
void DrawMenuSpaceFramePC(Color bg_color, const char* title, Color title_color, Color shadow_color, Color line_color);
void DrawDeviceNotificationToast(int timer, InputDeviceType dev);
void DrawAstralliticLogoPC(int start_x, int y);
void DrawScoreCustom(int score, int x, int y, Color color);
void DrawHeartCustom(int x, int y, Color color);
void DrawSkullIconPC(int x, int y);
void DrawPlayerShipPC(int x, int y, int dx, int dy, Color base_color);
void DrawEnemyPC(Enemy *e, int frame);
void DrawPowerUpPC(PowerUp *p);

#endif