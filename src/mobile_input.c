#include "mobile_input.h"
#include <math.h>

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)

#define BASE_STICK_RADIUS 54.0f
#define BASE_STICK_KNOB 24.0f
#define BASE_BUTTON_RADIUS 28.0f
#define MOBILE_DEADZONE 0.22f

static Vector2 g_stick_origin = { 0.0f, 0.0f };
static Vector2 g_stick_knob = { 0.0f, 0.0f };
static int g_stick_touch_id = -1;
static bool g_stick_active = false;
static bool g_prev_a = false;
static bool g_prev_b = false;
static bool g_prev_dash = false;
static bool g_prev_pause = false;
static MobileInputState g_state = {0};

static float GetMobileScale(int height)
{
    float s = (float)height / 360.0f;
    if (s < 1.0f) s = 1.0f;
    if (s > 2.8f) s = 2.8f;
    return s;
}

static bool TouchExistsId(int id, Vector2 *out_pos)
{
    int count = GetTouchPointCount();
    int i;
    for (i = 0; i < count; i++) {
        if (GetTouchPointId(i) == id) {
            if (out_pos) *out_pos = GetTouchPosition(i);
            return true;
        }
    }
    return false;
}

static bool TouchIsFresh(int id)
{
    (void)id;
    return true;
}

static bool PointInCircle(Vector2 p, Vector2 c, float r)
{
    float dx = p.x - c.x;
    float dy = p.y - c.y;
    return dx*dx + dy*dy <= r*r;
}

static bool IsButtonZone(Vector2 p, Vector2 center, float radius)
{
    return PointInCircle(p, center, radius + 12.0f);
}

void MobileInput_Init(void)
{
    g_stick_active = false;
    g_stick_touch_id = -1;
    g_state = (MobileInputState){0};
}

void MobileInput_Update(bool gameplay_mode)
{
    if (!gameplay_mode) {
        g_stick_active = false;
        g_stick_touch_id = -1;
        g_state = (MobileInputState){0};
        g_prev_a = false;
        g_prev_b = false;
        g_prev_dash = false;
        g_prev_pause = false;
        return;
    }

    int width = GetScreenWidth();
    int height = GetScreenHeight();
    float s = GetMobileScale(height);
    float stick_radius = BASE_STICK_RADIUS * s;
    float button_radius = BASE_BUTTON_RADIUS * s;

    int count = GetTouchPointCount();
    int i;
    Vector2 p;
    Vector2 fire_center  = { width - 76.0f * s, height - 120.0f * s };
    Vector2 turbo_center = { width - 76.0f * s, height - 42.0f * s };
    Vector2 dash_center  = { width - 162.0f * s, height - 60.0f * s };
    Vector2 aim_center   = { width - 162.0f * s, height - 138.0f * s };
    Vector2 pause_center = { width - 48.0f * s, 36.0f * s };
    bool a_now = false;
    bool dash_now = false;
    bool pause_now = false;

    g_state.up_down = false;
    g_state.down_down = false;
    g_state.left_down = false;
    g_state.right_down = false;
    g_state.fire_down = false;
    g_state.turbo_down = false;
    g_state.aim_down = false;

    /* Maintain the floating stick if its touch still exists. */
    if (g_stick_active) {
        if (!TouchExistsId(g_stick_touch_id, &p)) {
            g_stick_active = false;
            g_stick_touch_id = -1;
            g_stick_knob = g_stick_origin;
        } else {
            Vector2 delta = { p.x - g_stick_origin.x, p.y - g_stick_origin.y };
            float len = sqrtf(delta.x*delta.x + delta.y*delta.y);
            if (len > stick_radius && len > 0.0f) {
                delta.x = delta.x * stick_radius / len;
                delta.y = delta.y * stick_radius / len;
            }
            g_stick_knob.x = g_stick_origin.x + delta.x;
            g_stick_knob.y = g_stick_origin.y + delta.y;

            float nx = delta.x / stick_radius;
            float ny = delta.y / stick_radius;
            g_state.left_down = nx < -MOBILE_DEADZONE;
            g_state.right_down = nx > MOBILE_DEADZONE;
            g_state.up_down = ny < -MOBILE_DEADZONE;
            g_state.down_down = ny > MOBILE_DEADZONE;
        }
    }

    /* Claim the first suitable new touch as a floating stick in the left half. */
    if (!g_stick_active) {
        for (i = 0; i < count; i++) {
            int id = GetTouchPointId(i);
            p = GetTouchPosition(i);
            if (p.x < (float)width * 0.50f && p.y > (float)height * 0.35f) {
                if (TouchIsFresh(id)) {
                    g_stick_active = true;
                    g_stick_touch_id = id;
                    g_stick_origin = p;
                    g_stick_knob = p;
                    break;
                }
            }
        }
    }

    /* Any remaining touch can operate action zones. */
    for (i = 0; i < count; i++) {
        int id = GetTouchPointId(i);
        p = GetTouchPosition(i);
        if (id == g_stick_touch_id) continue;
        if (IsButtonZone(p, fire_center, button_radius)) a_now = true;
        if (IsButtonZone(p, dash_center, button_radius - 3.0f * s)) dash_now = true;
        if (IsButtonZone(p, turbo_center, 24.0f * s)) g_state.turbo_down = true;
        if (IsButtonZone(p, aim_center, 24.0f * s)) g_state.aim_down = true;
        if (IsButtonZone(p, pause_center, 22.0f * s)) pause_now = true;
    }

    g_state.fire_down = a_now;
    g_state.dash_pressed = dash_now && !g_prev_dash;
    g_state.a_pressed = a_now && !g_prev_a;
    g_state.b_pressed = false;
    g_state.pause_pressed = pause_now && !g_prev_pause;

    g_state.up_pressed = false;
    g_state.down_pressed = false;
    g_state.left_pressed = false;
    g_state.right_pressed = false;

    g_prev_a = a_now;
    g_prev_b = false;
    g_prev_dash = dash_now;
    g_prev_pause = pause_now;
}

MobileInputState MobileInput_GetState(void)
{
    return g_state;
}

void MobileInput_Draw(bool gameplay_mode)
{
    if (!gameplay_mode) return;

    int width = GetScreenWidth();
    int height = GetScreenHeight();
    float s = GetMobileScale(height);
    float stick_radius = BASE_STICK_RADIUS * s;
    float knob_radius = BASE_STICK_KNOB * s;
    float button_radius = BASE_BUTTON_RADIUS * s;

    Vector2 stick_center = g_stick_active ? g_stick_origin : (Vector2){ 82.0f * s, height - 85.0f * s };
    Vector2 stick_knob   = g_stick_active ? g_stick_knob : stick_center;
    Vector2 fire_center  = { width - 76.0f * s, height - 120.0f * s };
    Vector2 turbo_center = { width - 76.0f * s, height - 42.0f * s };
    Vector2 dash_center  = { width - 162.0f * s, height - 60.0f * s };
    Vector2 aim_center   = { width - 162.0f * s, height - 138.0f * s };
    Vector2 pause_center = { width - 48.0f * s, 36.0f * s };

    BeginBlendMode(BLEND_ALPHA);

    // Base exterior cuadrada estilo pixel-art 16-bit
    float box_half = stick_radius;
    int bx = (int)(stick_center.x - box_half);
    int by = (int)(stick_center.y - box_half);
    int bw = (int)(box_half * 2.0f);
    int bh = (int)(box_half * 2.0f);

    // Fondo y bordes del marco cuadrado
    DrawRectangle(bx, by, bw, bh, (Color){ 10, 16, 26, 85 });
    DrawRectangleLines(bx - 1, by - 1, bw + 2, bh + 2, (Color){ 255, 255, 255, 30 });
    DrawRectangleLines(bx, by, bw, bh, (Color){ 0, 220, 255, 110 });

    // Puntas direccionales pixeladas en los 4 ejes
    int arrow_thick = (int)(3.0f * s);
    if (arrow_thick < 2) arrow_thick = 2;
    int arrow_len = (int)(6.0f * s);
    DrawRectangle((int)stick_center.x - arrow_thick, by + (int)(3.0f * s), arrow_thick * 2, arrow_len, (Color){ 255, 255, 255, 120 });
    DrawRectangle((int)stick_center.x - arrow_thick, by + bh - (int)(3.0f * s) - arrow_len, arrow_thick * 2, arrow_len, (Color){ 255, 255, 255, 120 });
    DrawRectangle(bx + (int)(3.0f * s), (int)stick_center.y - arrow_thick, arrow_len, arrow_thick * 2, (Color){ 255, 255, 255, 120 });
    DrawRectangle(bx + bw - (int)(3.0f * s) - arrow_len, (int)stick_center.y - arrow_thick, arrow_len, arrow_thick * 2, (Color){ 255, 255, 255, 120 });

    // Pomo central cuadrado con relieve pixel art
    float k_half = knob_radius;
    int kx = (int)(stick_knob.x - k_half);
    int ky = (int)(stick_knob.y - k_half);
    int kw = (int)(k_half * 2.0f);
    int kh = (int)(k_half * 2.0f);

    DrawRectangle(kx, ky, kw, kh, (Color){ 0, 180, 230, 150 });
    DrawRectangleLines(kx, ky, kw, kh, (Color){ 255, 255, 255, 220 });
    int c_pad = (int)(k_half * 0.45f);
    DrawRectangle(kx + c_pad, ky + c_pad, kw - c_pad * 2, kh - c_pad * 2, (Color){ 255, 255, 255, 140 });

    int font_large = (int)(20.0f * s);
    int font_med   = (int)(18.0f * s);
    int font_small = (int)(15.0f * s);

    // Botón A (Disparo)
    DrawCircleV(fire_center, button_radius, (Color){ 190, 40, 40, 125 });
    DrawCircleLines((int)fire_center.x, (int)fire_center.y, button_radius, (Color){ 255, 255, 255, 115 });
    DrawText("A", (int)fire_center.x - (int)(6 * s), (int)fire_center.y - (int)(9 * s), font_large, (Color){255,255,255,210});

    // Botón D (Dash)
    DrawCircleV(dash_center, button_radius - 3.0f * s, (Color){ 55, 120, 220, 105 });
    DrawText("D", (int)dash_center.x - (int)(6 * s), (int)dash_center.y - (int)(9 * s), font_med, (Color){255,255,255,190});

    // Botón T (Turbo)
    DrawCircleV(turbo_center, 24.0f * s, (Color){ 210, 175, 30, 100 });
    DrawText("T", (int)turbo_center.x - (int)(5 * s), (int)turbo_center.y - (int)(8 * s), font_med, (Color){255,255,255,185});

    // Botón L (Aim Lock)
    DrawCircleV(aim_center, 24.0f * s, (Color){ 50, 180, 150, 100 });
    DrawText("L", (int)aim_center.x - (int)(5 * s), (int)aim_center.y - (int)(8 * s), font_med, (Color){255,255,255,185});

    // Botón II (Pausa)
    DrawCircleV(pause_center, 18.0f * s, (Color){ 35, 35, 40, 125 });
    DrawText("II", (int)pause_center.x - (int)(7 * s), (int)pause_center.y - (int)(7 * s), font_small, (Color){255,255,255,190});

    EndBlendMode();
}

void MobileInput_Shutdown(void)
{
    g_stick_active = false;
    g_stick_touch_id = -1;
}

#else

void MobileInput_Init(void) {}
void MobileInput_Update(bool gameplay_mode) { (void)gameplay_mode; }
MobileInputState MobileInput_GetState(void) { return (MobileInputState){0}; }
void MobileInput_Draw(bool gameplay_mode) { (void)gameplay_mode; }
void MobileInput_Shutdown(void) {}

#endif
