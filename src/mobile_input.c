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
    Vector2 fire_center  = { width - 70.0f * s, height - 95.0f * s };
    Vector2 turbo_center = { width - 145.0f * s, height - 65.0f * s };
    Vector2 dash_center  = { width - 145.0f * s, height - 135.0f * s };
    Vector2 aim_center   = { width - 75.0f * s, height - 160.0f * s };
    Vector2 pause_center = { width - 48.0f * s, 36.0f * s };
    bool a_now = false;
    bool dash_now = false;
    bool pause_now = false;
    Vector2 p = { 0 };

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
                    // Clamp stick origin so it never spawns under the iPhone notch
                    float min_stick_x = 115.0f * s;
                    if (p.x < min_stick_x) p.x = min_stick_x;
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
        if (IsButtonZone(p, turbo_center, 28.0f * s)) g_state.turbo_down = true;
        if (IsButtonZone(p, dash_center, 28.0f * s)) dash_now = true;
        if (IsButtonZone(p, aim_center, 26.0f * s)) g_state.aim_down = true;
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

static void DrawRetroButton(Vector2 center, float radius, const char *label, Color base_col, Color border_col, Color text_col, bool is_down, float s)
{
    int cx = (int)center.x;
    int cy = (int)center.y;
    int r = (int)radius;
    int y_off = is_down ? (int)(2.0f * s) : 0;

    // Sombra proyectada inferior
    DrawRectangle(cx - r + 2, cy - r + 4, (r - 2) * 2, (r - 2) * 2, (Color){ 0, 0, 0, 90 });

    // Fondo del botón con brillo extra si está pulsado
    Color fill_col = is_down ? (Color){ (unsigned char)(base_col.r + 45 > 255 ? 255 : base_col.r + 45),
                                        (unsigned char)(base_col.g + 45 > 255 ? 255 : base_col.g + 45),
                                        (unsigned char)(base_col.b + 45 > 255 ? 255 : base_col.b + 45),
                                        220 } : base_col;
    DrawRectangle(cx - r + 2, cy - r + 2 + y_off, (r - 2) * 2, (r - 2) * 2, fill_col);

    // Borde exterior biselado pixel art
    DrawRectangleLines(cx - r + 2, cy - r + 2 + y_off, (r - 2) * 2, (r - 2) * 2, border_col);

    // Relieve 3D superior e izquierdo estilo 16-bit
    if (!is_down) {
        DrawRectangle(cx - r + 4, cy - r + 4, (r - 4) * 2, 2, (Color){ 255, 255, 255, 120 });
        DrawRectangle(cx - r + 4, cy - r + 4, 2, (r - 4) * 2, (Color){ 255, 255, 255, 120 });
    }

    // Remaches/esquinas pixeladas estilo arcade
    int dot_s = (int)(3.0f * s);
    if (dot_s < 2) dot_s = 2;
    DrawRectangle(cx - r, cy - r + y_off, dot_s, dot_s, border_col);
    DrawRectangle(cx + r - dot_s, cy - r + y_off, dot_s, dot_s, border_col);
    DrawRectangle(cx - r, cy + r - dot_s + y_off, dot_s, dot_s, border_col);
    DrawRectangle(cx + r - dot_s, cy + r - dot_s + y_off, dot_s, dot_s, border_col);

    // Texto de la acción centrado con sombra
    int font_sz = (int)(radius * 0.95f);
    if (font_sz < 14) font_sz = 14;
    int tw = MeasureText(label, font_sz);
    int tx = cx - (tw / 2);
    int ty = cy - (font_sz / 2) + y_off;

    DrawText(label, tx + 1, ty + 1, font_sz, (Color){ 0, 0, 0, 180 });
    DrawText(label, tx, ty, font_sz, text_col);
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

    Vector2 stick_center = g_stick_active ? g_stick_origin : (Vector2){ 120.0f * s, height - 85.0f * s };
    Vector2 stick_knob   = g_stick_active ? g_stick_knob : stick_center;
    Vector2 fire_center  = { width - 70.0f * s, height - 95.0f * s };
    Vector2 turbo_center = { width - 145.0f * s, height - 65.0f * s };
    Vector2 dash_center  = { width - 145.0f * s, height - 135.0f * s };
    Vector2 aim_center   = { width - 75.0f * s, height - 160.0f * s };
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

    // Botones con estética retro pixel-art arcade
    // Botón A (Disparo - Rojo)
    DrawRetroButton(fire_center, button_radius, "A", (Color){ 180, 30, 30, 155 }, (Color){ 255, 110, 110, 230 }, WHITE, g_state.fire_down, s);

    // Botón T (Turbo - Azul, ahora en la posición inferior)
    DrawRetroButton(turbo_center, 26.0f * s, "T", (Color){ 25, 95, 200, 155 }, (Color){ 100, 215, 255, 230 }, WHITE, g_state.turbo_down, s);

    // Botón D (Dash - Verde, ahora en la posición superior izquierda)
    DrawRetroButton(dash_center, 26.0f * s, "D", (Color){ 20, 145, 80, 155 }, (Color){ 110, 255, 175, 230 }, WHITE, g_prev_dash, s);

    // Botón L (Aim Lock - Dorado/Amarillo, ahora en la posición superior derecha)
    DrawRetroButton(aim_center, 24.0f * s, "L", (Color){ 195, 145, 20, 155 }, (Color){ 255, 230, 105, 230 }, WHITE, g_state.aim_down, s);

    // Botón II (Pausa - Pizarra sci-fi)
    DrawRetroButton(pause_center, 18.0f * s, "II", (Color){ 20, 30, 45, 155 }, (Color){ 0, 220, 255, 210 }, WHITE, g_prev_pause, s);

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
