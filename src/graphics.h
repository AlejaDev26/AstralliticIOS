#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "raylib.h"
#include "types.h"

Color GetGamepadButtonColorCustom(int btn);
const char* GetGamepadButtonNameCustom(int btn);
const char* GetKeyNameCustom(int key);

#define FILTER_COUNT 8
extern const char* const g_filter_names[LANG_COUNT][FILTER_COUNT];
const char* GetFilterName(int filter_idx, int lang);
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
void DrawTouchIcon16Bit(int x, int y, bool is_hovered);
void DrawActionIcon16Bit(int type, int x, int y, bool is_sel);
void DrawLanguageFlag16Bit(int lang, int x, int y);
void DrawSpaceShipCursor16Bit(int x, int y, bool is_clicking);
void DrawMenuSpaceFramePC(Color bg_color, const char* title, Color title_color, Color shadow_color, Color line_color);
void DrawDeviceNotificationToast(int timer, InputDeviceType dev);
void DrawAstralliticLogoPC(int start_x, int y, int frame_count);
void DrawScoreCustom(int score, int x, int y, Color color);
void DrawHeartCustom(int x, int y, Color color);
void DrawSkullIconPC(int x, int y);
void DrawPlayerShipPC(int x, int y, int dx, int dy, Color base_color);
void DrawPlayerShipScaledPC(int x, int y, int dx, int dy, Color base_color, float scale);
void DrawEnemyPC(Enemy *e, int frame);
void DrawEnemyScaledPC(Enemy *e, int frame, int scale);
void DrawPadlockPC(int cx, int cy);
void DrawPowerUpPC(PowerUp *p);
Color GetPowerUpThemeColor(int type);
void DrawGhostTrailsPC(GhostTrail *trails, int count);
void DrawParticlesPC(Particle *particles, int count);
void DrawDashGaugePC(int ship_x, int ship_y, int dash_cd, int max_cd, int ready_flash);
void DrawDamageFlashPC(int flash_timer);

typedef enum {
    MENU_ICON_NONE = 0,
    MENU_ICON_PLAY,
    MENU_ICON_OPTIONS,
    MENU_ICON_RECORDS,
    MENU_ICON_DIFF_EASY,
    MENU_ICON_DIFF_NORMAL,
    MENU_ICON_DIFF_HARD,
    MENU_ICON_DIFF_HELL,
    MENU_ICON_PADLOCK,
    MENU_ICON_MEDAL,
    MENU_ICON_AUDIO,
    MENU_ICON_VIDEO,
    MENU_ICON_CONTROLS,
    MENU_ICON_EXIT
} MenuIconType;

void DrawBevelledBoxPC(int x, int y, int w, int h, Color bg_color, Color border_color, bool bevel_corners);
void DrawMenuIconPC(int icon_type, int x, int y, Color theme_color);
void DrawSciFiButtonPC(int x, int y, int w, int h, const char* text, int icon_type, bool is_selected, Color theme_color, int anim_frame);
void DrawNebulaBackgroundPC(int screen_w, int screen_h, int frame_count);
void DrawAchievementBadgePC(int x, int y, bool unlocked, int tier);
void DrawClassicCardArtworkPC(int x, int y, int w, int h, int frame);
void DrawRushCardArtworkPC(int x, int y, int w, int h, int frame);
void DrawTimeAttackCardArtworkPC(int x, int y, int w, int h, int frame);
void DrawAssaultCardArtworkPC(int x, int y, int w, int h, int frame);
void DrawComingSoonCardArtworkPC(int x, int y, int w, int h, int frame);
void DrawRecordArrowPC(int x, int y, bool points_right, Color color);
void DrawTitleFooterPC(int frame_count);
void DrawTitleDividerPC(int center_x, int y, int width, int frame_count);

#endif