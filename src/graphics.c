#include "graphics.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "logo_pixelart.h"

const unsigned char font[36][5] = {
    {7,5,7,5,5}, {6,5,6,5,6}, {7,4,4,4,7}, {6,5,5,5,6},
    {7,4,6,4,7}, {7,4,6,4,4}, {7,4,5,5,7}, {5,5,7,5,5},
    {7,2,2,2,7}, {3,1,1,5,7}, {5,5,6,5,5}, {4,4,4,4,7},
    {5,7,5,5,5}, {7,5,5,5,5}, {7,5,5,5,7}, {7,5,7,4,4},
    {7,5,5,7,3}, {6,5,6,5,5}, {7,4,7,1,7}, {7,2,2,2,2},
    {5,5,5,5,7}, {5,5,5,5,2}, {5,5,5,7,5}, {5,5,2,5,5},
    {5,5,2,2,2}, {7,1,2,4,7},
    {7,5,5,5,7}, {2,6,2,2,7}, {7,1,7,4,7}, {7,1,7,1,7},
    {5,5,7,1,1}, {7,4,7,1,7}, {7,4,7,5,7}, {7,1,1,1,1},
    {7,5,7,5,7}, {7,5,7,1,7}
};

const unsigned char header_font[26][7] = {
    { 0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11 }, // A
    { 0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E }, // B
    { 0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E }, // C
    { 0x1E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1E }, // D
    { 0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F }, // E
    { 0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10 }, // F
    { 0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F }, // G
    { 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11 }, // H
    { 0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E }, // I
    { 0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C }, // J
    { 0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11 }, // K
    { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F }, // L
    { 0x11, 0x1B, 0x15, 0x11, 0x11, 0x11, 0x11 }, // M
    { 0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11 }, // N
    { 0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E }, // O
    { 0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10 }, // P
    { 0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D }, // Q
    { 0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11 }, // R
    { 0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E }, // S
    { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 }, // T
    { 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E }, // U
    { 0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04 }, // V
    { 0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11 }, // W
    { 0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11 }, // X
    { 0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04 }, // Y
    { 0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F }  // Z
};

const char* const g_filter_names[LANG_COUNT][FILTER_COUNT] = {
    // 0: ESPAÑOL
    { "OFF", "SCANLINES", "CRT ARCADE", "VERDE RETRO", "FOSFORO AMBAR", "SYNTHWAVE", "MONOCROMO", "CHICO VIRTUAL" },
    // 1: ENGLISH
    { "OFF", "SCANLINES", "CRT ARCADE", "RETRO GREEN", "AMBER PHOSPHOR", "SYNTHWAVE", "MONOCHROME", "VIRTUAL GUY" },
    // 2: FRANÇAIS
    { "OFF", "SCANLINES", "CRT ARCADE", "VERT RETRO", "PHOSPHORE AMBRE", "SYNTHWAVE", "MONOCHROME", "GARCON VIRTUEL" },
    // 3: ITALIANO
    { "OFF", "SCANLINES", "CRT ARCADE", "VERDE RETRO", "FOSFORO AMBRA", "SYNTHWAVE", "MONOCROMO", "RAGAZZO VIRTUALE" },
    // 4: DEUTSCH
    { "OFF", "SCANLINES", "CRT ARCADE", "RETRO GRUEN", "BERNSTEIN", "SYNTHWAVE", "MONOCHROM", "VIRTUELLER TYP" }
};

const char* GetFilterName(int filter_idx, int lang) {
    if (filter_idx < 0 || filter_idx >= FILTER_COUNT) filter_idx = 0;
    if (lang < 0 || lang >= LANG_COUNT) lang = 0;
    return g_filter_names[lang][filter_idx];
}

#if defined(GRAPHICS_API_OPENGL_ES3)
const char* crt_vertex_shader_code = 
    "#version 300 es\n"
    "in vec3 vertexPosition;\n"
    "in vec2 vertexTexCoord;\n"
    "in vec4 vertexColor;\n"
    "out vec2 fragTexCoord;\n"
    "out vec4 fragColor;\n"
    "uniform mat4 mvp;\n"
    "void main() {\n"
    "    fragTexCoord = vertexTexCoord;\n"
    "    fragColor = vertexColor;\n"
    "    gl_Position = mvp * vec4(vertexPosition, 1.0);\n"
    "}\n";

const char* crt_shader_code = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 fragTexCoord;\n"
    "in vec4 fragColor;\n"
    "out vec4 finalColor;\n"
    "uniform sampler2D texture0;\n"
    "uniform int filterType;\n"
    "uniform vec2 renderSize;\n"
    "void main() {\n"
    "    vec2 uv = fragTexCoord;\n"
    "    vec2 res = (renderSize.x > 0.0 && renderSize.y > 0.0) ? renderSize : vec2(1280.0, 720.0);\n"
    "    float aspect = res.x / res.y;\n"
    "\n"
    "    if (filterType == 2) {\n"
    "        vec2 dc = uv - vec2(0.5, 0.5);\n"
    "        vec2 dc_aspect = vec2(dc.x * (aspect / 1.5), dc.y);\n"
    "        float dist = dot(dc_aspect, dc_aspect);\n"
    "        uv = vec2(0.5, 0.5) + dc * (1.0 + 0.12 * dist);\n"
    "        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {\n"
    "            finalColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "            return;\n"
    "        }\n"
    "    }\n"
    "\n"
    "    vec4 col = texture(texture0, uv);\n"
    "    float lum = dot(col.rgb, vec3(0.299, 0.587, 0.114));\n"
    "\n"
    "    if (filterType == 1) {\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.13;\n"
    "        col.rgb -= scanline;\n"
    "        float subpixel = sin(uv.x * res.x * 3.14159265) * 0.025;\n"
    "        col.rgb += subpixel;\n"
    "    } else if (filterType == 2) {\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.15;\n"
    "        col.rgb -= scanline;\n"
    "        float vig = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);\n"
    "        col.rgb *= clamp(pow(16.0 * vig, 0.28), 0.0, 1.0);\n"
    "        col.rgb += col.rgb * 0.06;\n"
    "    } else if (filterType == 3) {\n"
    "        vec3 c0 = vec3(0.059, 0.220, 0.059);\n"
    "        vec3 c1 = vec3(0.188, 0.384, 0.188);\n"
    "        vec3 c2 = vec3(0.545, 0.675, 0.059);\n"
    "        vec3 c3 = vec3(0.608, 0.737, 0.059);\n"
    "        vec3 mapped;\n"
    "        if (lum < 0.25) mapped = mix(c0, c1, lum / 0.25);\n"
    "        else if (lum < 0.60) mapped = mix(c1, c2, (lum - 0.25) / 0.35);\n"
    "        else mapped = mix(c2, c3, (lum - 0.60) / 0.40);\n"
    "        float grid_x = sin(uv.x * 480.0 * 3.14159265);\n"
    "        float grid_y = sin(uv.y * 320.0 * 3.14159265);\n"
    "        mapped -= (grid_x * grid_x + grid_y * grid_y) * 0.035;\n"
    "        col.rgb = clamp(mapped, 0.0, 1.0);\n"
    "    } else if (filterType == 4) {\n"
    "        float l_adj = pow(lum, 0.95);\n"
    "        vec3 ambar_dark = vec3(0.04, 0.015, 0.0);\n"
    "        vec3 ambar_bright = vec3(1.0, 0.72, 0.12);\n"
    "        vec3 ambar_hot = vec3(1.0, 0.95, 0.55);\n"
    "        vec3 mapped;\n"
    "        if (l_adj < 0.75) mapped = mix(ambar_dark, ambar_bright, l_adj / 0.75);\n"
    "        else mapped = mix(ambar_bright, ambar_hot, (l_adj - 0.75) / 0.25);\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.10;\n"
    "        mapped -= scanline;\n"
    "        col.rgb = clamp(mapped, 0.0, 1.0);\n"
    "    } else if (filterType == 5) {\n"
    "        vec3 dark = vec3(0.06, 0.01, 0.18);\n"
    "        vec3 mid = vec3(0.92, 0.10, 0.60);\n"
    "        vec3 bright = vec3(0.05, 0.95, 1.0);\n"
    "        vec3 neon;\n"
    "        if (lum < 0.45) neon = mix(dark, mid, lum / 0.45);\n"
    "        else neon = mix(mid, bright, (lum - 0.45) / 0.55);\n"
    "        col.rgb = mix(neon, col.rgb * vec3(1.2, 0.85, 1.3), 0.35);\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.08;\n"
    "        col.rgb -= scanline;\n"
    "    } else if (filterType == 6) {\n"
    "        float bw_lum = clamp(pow(lum, 1.1) * 1.15, 0.0, 1.0);\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.08;\n"
    "        col.rgb = clamp(vec3(bw_lum) - scanline, 0.0, 1.0);\n"
    "    } else if (filterType == 7) {\n"
    "        vec3 red_dark = vec3(0.02, 0.0, 0.0);\n"
    "        vec3 red_mid = vec3(0.92, 0.06, 0.04);\n"
    "        vec3 red_glow = vec3(1.0, 0.40, 0.25);\n"
    "        vec3 ciber;\n"
    "        if (lum < 0.60) ciber = mix(red_dark, red_mid, lum / 0.60);\n"
    "        else ciber = mix(red_mid, red_glow, (lum - 0.60) / 0.40);\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.12;\n"
    "        ciber -= scanline;\n"
    "        col.rgb = clamp(ciber, 0.0, 1.0);\n"
    "    }\n"
    "\n"
    "    finalColor = col * fragColor;\n"
    "}\n";
#else
const char* crt_vertex_shader_code = 0;
const char* crt_shader_code = 
    "#version 330\n"
    "in vec2 fragTexCoord;\n"
    "in vec4 fragColor;\n"
    "out vec4 finalColor;\n"
    "uniform sampler2D texture0;\n"
    "uniform int filterType;\n"
    "uniform vec2 renderSize;\n"
    "void main() {\n"
    "    vec2 uv = fragTexCoord;\n"
    "    vec2 res = (renderSize.x > 0.0 && renderSize.y > 0.0) ? renderSize : vec2(1280.0, 720.0);\n"
    "    float aspect = res.x / res.y;\n"
    "\n"
    "    if (filterType == 2) {\n"
    "        vec2 dc = uv - vec2(0.5, 0.5);\n"
    "        vec2 dc_aspect = vec2(dc.x * (aspect / 1.5), dc.y);\n"
    "        float dist = dot(dc_aspect, dc_aspect);\n"
    "        uv = vec2(0.5, 0.5) + dc * (1.0 + 0.12 * dist);\n"
    "        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {\n"
    "            finalColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "            return;\n"
    "        }\n"
    "    }\n"
    "\n"
    "    vec4 col = texture(texture0, uv);\n"
    "    float lum = dot(col.rgb, vec3(0.299, 0.587, 0.114));\n"
    "\n"
    "    if (filterType == 1) {\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.13;\n"
    "        col.rgb -= scanline;\n"
    "        float subpixel = sin(uv.x * res.x * 3.14159265) * 0.025;\n"
    "        col.rgb += subpixel;\n"
    "    } else if (filterType == 2) {\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.15;\n"
    "        col.rgb -= scanline;\n"
    "        float vig = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);\n"
    "        col.rgb *= clamp(pow(16.0 * vig, 0.28), 0.0, 1.0);\n"
    "        col.rgb += col.rgb * 0.06;\n"
    "    } else if (filterType == 3) {\n"
    "        vec3 c0 = vec3(0.059, 0.220, 0.059);\n"
    "        vec3 c1 = vec3(0.188, 0.384, 0.188);\n"
    "        vec3 c2 = vec3(0.545, 0.675, 0.059);\n"
    "        vec3 c3 = vec3(0.608, 0.737, 0.059);\n"
    "        vec3 mapped;\n"
    "        if (lum < 0.25) mapped = mix(c0, c1, lum / 0.25);\n"
    "        else if (lum < 0.60) mapped = mix(c1, c2, (lum - 0.25) / 0.35);\n"
    "        else mapped = mix(c2, c3, (lum - 0.60) / 0.40);\n"
    "        float grid_x = sin(uv.x * 480.0 * 3.14159265);\n"
    "        float grid_y = sin(uv.y * 320.0 * 3.14159265);\n"
    "        mapped -= (grid_x * grid_x + grid_y * grid_y) * 0.035;\n"
    "        col.rgb = clamp(mapped, 0.0, 1.0);\n"
    "    } else if (filterType == 4) {\n"
    "        float l_adj = pow(lum, 0.95);\n"
    "        vec3 ambar_dark = vec3(0.04, 0.015, 0.0);\n"
    "        vec3 ambar_bright = vec3(1.0, 0.72, 0.12);\n"
    "        vec3 ambar_hot = vec3(1.0, 0.95, 0.55);\n"
    "        vec3 mapped;\n"
    "        if (l_adj < 0.75) mapped = mix(ambar_dark, ambar_bright, l_adj / 0.75);\n"
    "        else mapped = mix(ambar_bright, ambar_hot, (l_adj - 0.75) / 0.25);\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.10;\n"
    "        mapped -= scanline;\n"
    "        col.rgb = clamp(mapped, 0.0, 1.0);\n"
    "    } else if (filterType == 5) {\n"
    "        vec3 dark = vec3(0.06, 0.01, 0.18);\n"
    "        vec3 mid = vec3(0.92, 0.10, 0.60);\n"
    "        vec3 bright = vec3(0.05, 0.95, 1.0);\n"
    "        vec3 neon;\n"
    "        if (lum < 0.45) neon = mix(dark, mid, lum / 0.45);\n"
    "        else neon = mix(mid, bright, (lum - 0.45) / 0.55);\n"
    "        col.rgb = mix(neon, col.rgb * vec3(1.2, 0.85, 1.3), 0.35);\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.08;\n"
    "        col.rgb -= scanline;\n"
    "    } else if (filterType == 6) {\n"
    "        float bw_lum = clamp(pow(lum, 1.1) * 1.15, 0.0, 1.0);\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.08;\n"
    "        col.rgb = clamp(vec3(bw_lum) - scanline, 0.0, 1.0);\n"
    "    } else if (filterType == 7) {\n"
    "        vec3 red_dark = vec3(0.02, 0.0, 0.0);\n"
    "        vec3 red_mid = vec3(0.92, 0.06, 0.04);\n"
    "        vec3 red_glow = vec3(1.0, 0.40, 0.25);\n"
    "        vec3 ciber;\n"
    "        if (lum < 0.60) ciber = mix(red_dark, red_mid, lum / 0.60);\n"
    "        else ciber = mix(red_mid, red_glow, (lum - 0.60) / 0.40);\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159265) * 0.12;\n"
    "        ciber -= scanline;\n"
    "        col.rgb = clamp(ciber, 0.0, 1.0);\n"
    "    }\n"
    "\n"
    "    finalColor = col * fragColor;\n"
    "}\n";
#endif

const char* GetKeyNameCustom(int key) {
    if (key == KEY_SPACE) return "SPACE";
    if (key == KEY_LEFT_SHIFT) return "LSHIFT";
    if (key == KEY_RIGHT_SHIFT) return "RSHIFT";
    if (key == KEY_LEFT_CONTROL) return "LCTRL";
    if (key == KEY_RIGHT_CONTROL) return "RCTRL";
    if (key == KEY_LEFT_ALT) return "LALT";
    if (key == KEY_RIGHT_ALT) return "RALT";
    if (key == KEY_UP) return "UP";
    if (key == KEY_DOWN) return "DOWN";
    if (key == KEY_LEFT) return "LEFT";
    if (key == KEY_RIGHT) return "RIGHT";
    if (key == KEY_ENTER) return "ENTER";
    if (key == KEY_TAB) return "TAB";
    if (key == KEY_BACKSPACE) return "BKSP";
    if (key == KEY_ESCAPE) return "ESC";
    static char buf[4] = {0};
    if (key >= 32 && key <= 126) {
        buf[0] = (char)key;
        buf[1] = '\0';
        return buf;
    }
    return "KEY";
}

Color BlendColorAlpha(Color c, int alpha) {
    if (alpha <= 0) return (Color){ 0, 0, 0, 255 };
    if (alpha >= 16) return c;
    return (Color){
        (unsigned char)((c.r * alpha) / 16),
        (unsigned char)((c.g * alpha) / 16),
        (unsigned char)((c.b * alpha) / 16),
        255
    };
}

void DrawEnneCustom(int x, int y, Color color, int scale) {
    // Virgulilla / Tilde superior sobre la 'N'
    int t_offset = (scale > 1) ? scale : 1;
    DrawRectangle(x, y - scale, scale, scale, color);
    DrawRectangle(x + scale, y - scale - t_offset, scale, scale, color);
    DrawRectangle(x + 2 * scale, y - scale, scale, scale, color);

    // Cuerpo de la letra 'N' (matriz de font[13])
    for(int row = 0; row < 5; row++) {
        for(int col = 0; col < 3; col++) {
            if((font[13][row] >> (2 - col)) & 1)
                DrawRectangle(x + col * scale, y + row * scale, scale, scale, color);
        }
    }
}

void DrawCharCustom(char c, int x, int y, Color color, int scale) {
    unsigned char uc = (unsigned char)c;
    if (uc == 0xD1 || uc == 0xF1) {
        DrawEnneCustom(x, y, color, scale);
        return;
    }
    int idx = -1;
    if(c >= 'A' && c <= 'Z') idx = c - 'A';
    else if(c >= 'a' && c <= 'z') idx = c - 'a';
    else if(c >= '0' && c <= '9') idx = c - '0' + 26;
    else if(c == '!') idx = 35;
    else if(c == '/') {
        DrawRectangle(x + 2 * scale, y, scale, scale, color);
        DrawRectangle(x + scale, y + scale, scale, scale, color);
        DrawRectangle(x + scale, y + 2 * scale, scale, scale, color);
        DrawRectangle(x, y + 3 * scale, scale, scale, color);
        DrawRectangle(x, y + 4 * scale, scale, scale, color);
        return;
    } else if(c == ':') {
        DrawRectangle(x + scale, y + scale, scale, scale, color);
        DrawRectangle(x + scale, y + 3 * scale, scale, scale, color);
        return;
    } else if(c == '.') {
        DrawRectangle(x + scale, y + 4 * scale, scale, scale, color);
        return;
    } else if(c == '%') {
        DrawRectangle(x, y, scale, scale, color);
        DrawRectangle(x + 2 * scale, y, scale, scale, color);
        DrawRectangle(x + scale, y + scale, scale, scale, color);
        DrawRectangle(x, y + 2 * scale, scale, scale, color);
        DrawRectangle(x + 2 * scale, y + 2 * scale, scale, scale, color);
        return;
    } else if(c == '?') {
        DrawRectangle(x, y, 3 * scale, scale, color);
        DrawRectangle(x + 2 * scale, y + scale, scale, scale, color);
        DrawRectangle(x + scale, y + 2 * scale, scale, scale, color);
        DrawRectangle(x + scale, y + 4 * scale, scale, scale, color);
        return;
    } else if(c == '>') {
        DrawRectangle(x, y, scale, 5 * scale, color);
        DrawRectangle(x + scale, y + scale, scale, 3 * scale, color);
        DrawRectangle(x + 2 * scale, y + 2 * scale, scale, scale, color);
        return;
    } else if(c == '<') {
        DrawRectangle(x + 2 * scale, y, scale, 5 * scale, color);
        DrawRectangle(x + scale, y + scale, scale, 3 * scale, color);
        DrawRectangle(x, y + 2 * scale, scale, scale, color);
        return;
    } else if(c == '-') {
        DrawRectangle(x, y + 2 * scale, 3 * scale, scale, color);
        return;
    } else if(c == ',') {
        DrawRectangle(x + scale, y + 3 * scale, scale, scale, color);
        DrawRectangle(x, y + 4 * scale, scale, scale, color);
        return;
    } else if(c == '+') {
        // Corrección limpia del símbolo '+' para que no pinte símbolos raros
        DrawRectangle(x + scale, y, scale, 3 * scale, color);
        DrawRectangle(x, y + scale, 3 * scale, scale, color);
        return;
    }
    if(idx == -1) return;

    for(int row = 0; row < 5; row++) {
        for(int col = 0; col < 3; col++) {
            if(c == '!') {
                if((row < 3 && col == 1) || (row == 4 && col == 1))
                    DrawRectangle(x + col * scale, y + row * scale, scale, scale, color);
            } else {
                if((font[idx][row] >> (2 - col)) & 1)
                    DrawRectangle(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

void DrawStringCustom(const char* str, int x, int y, Color color, int scale) {
    int i = 0;
    while(str[i] != '\0') {
        unsigned char uc = (unsigned char)str[i];
        if (uc == 0xC3 && ((unsigned char)str[i+1] == 0x91 || (unsigned char)str[i+1] == 0xB1)) {
            DrawEnneCustom(x, y, color, scale);
            x += (3 * scale) + scale;
            i += 2;
            continue;
        } else if (uc == 0xD1 || uc == 0xF1) {
            DrawEnneCustom(x, y, color, scale);
            x += (3 * scale) + scale;
            i += 1;
            continue;
        }
        DrawCharCustom(str[i], x, y, color, scale);
        x += (3 * scale) + scale;
        i++;
    }
}

int MeasureStringCustom(const char* str, int scale) {
    int len = 0;
    int i = 0;
    while(str[i] != '\0') {
        unsigned char uc = (unsigned char)str[i];
        if (uc == 0xC3 && ((unsigned char)str[i+1] == 0x91 || (unsigned char)str[i+1] == 0xB1)) {
            i += 2;
        } else {
            i += 1;
        }
        len++;
    }
    if (len == 0) return 0;
    return len * (3 * scale) + (len - 1) * scale;
}

void DrawCenteredStringCustom(const char* str, int y, Color color, int scale) {
    int width = MeasureStringCustom(str, scale);
    int x = (SCREEN_W - width) / 2;
    DrawStringCustom(str, x, y, color, scale);
}

void DrawHeaderCharCustom(char c, int x, int y, Color main_color, Color shadow_color, int scale) {
    if(c < 'A' || c > 'Z') return;
    int idx = c - 'A';
    for(int r = 0; r < 7; r++) {
        for(int col = 0; col < 5; col++) {
            if((header_font[idx][r] >> (4 - col)) & 1) {
                DrawRectangle(x + (col * scale) + scale, y + (r * scale) + scale, scale, scale, shadow_color);
                DrawRectangle(x + (col * scale), y + (r * scale), scale, scale, main_color);
            }
        }
    }
}

void DrawHeaderCenteredStringCustom(const char* str, int y, Color main_color, Color shadow_color, int scale) {
    int len = 0; while(str[len] != '\0') len++;
    int char_w = 5 * scale;
    int space_w = 2 * scale;
    int total_w = (len * char_w) + ((len - 1) * space_w);
    int x = (SCREEN_W - total_w) / 2;
    for(int i = 0; i < len; i++) {
        if(str[i] == ' ') { x += char_w + space_w; continue; }
        DrawHeaderCharCustom(str[i], x, y, main_color, shadow_color, scale);
        x += char_w + space_w;
    }
}

void DrawKeyboardIcon16Bit(int x, int y, bool is_hovered) {
    Color bg = is_hovered ? GBA_COLOR(31, 28, 0) : GBA_COLOR(6, 12, 18);
    Color border = is_hovered ? WHITE : GBA_COLOR(0, 31, 31);
    Color key_color = is_hovered ? GBA_COLOR(0, 0, 0) : WHITE;

    DrawRectangle(x, y, 14, 9, bg);
    DrawRectangleLines(x, y, 14, 9, border);

    DrawRectangle(x + 2, y + 2, 2, 2, key_color);
    DrawRectangle(x + 5, y + 2, 2, 2, key_color);
    DrawRectangle(x + 8, y + 2, 2, 2, key_color);
    DrawRectangle(x + 10, y + 2, 2, 2, key_color);
    DrawRectangle(x + 3, y + 5, 8, 2, key_color);
}

void DrawGamepadIcon16Bit(int x, int y, bool is_hovered) {
    Color bg = is_hovered ? GBA_COLOR(31, 28, 0) : GBA_COLOR(6, 12, 18);
    Color border = is_hovered ? WHITE : GBA_COLOR(0, 31, 31);
    Color btn_color = is_hovered ? GBA_COLOR(0, 0, 0) : WHITE;

    DrawRectangle(x + 2, y, 10, 9, bg);
    DrawRectangleLines(x + 2, y, 10, 9, border);

    DrawRectangle(x + 4, y + 4, 2, 2, btn_color);
    DrawRectangle(x + 8, y + 4, 2, 2, btn_color);
}

void DrawActionIcon16Bit(int type, int x, int y, bool is_sel) {
    Color col_main = is_sel ? C_YELLOW : C_CYAN;
    Color col_sec  = is_sel ? WHITE : GBA_COLOR(31, 28, 0);

    if (type == 0) {
        DrawRectangle(x + 3, y, 2, 2, col_main);
        DrawRectangle(x + 2, y + 2, 4, 2, col_main);
        DrawRectangle(x + 1, y + 4, 6, 2, col_main);
        DrawRectangle(x + 3, y + 6, 2, 3, col_sec);
    } else if (type == 1) {
        DrawRectangle(x + 3, y, 2, 3, col_sec);
        DrawRectangle(x + 1, y + 3, 6, 2, col_main);
        DrawRectangle(x + 2, y + 5, 4, 2, col_main);
        DrawRectangle(x + 3, y + 7, 2, 2, col_main);
    } else if (type == 2) {
        DrawRectangle(x, y + 3, 2, 2, col_main);
        DrawRectangle(x + 2, y + 2, 2, 4, col_main);
        DrawRectangle(x + 4, y + 1, 2, 6, col_main);
        DrawRectangle(x + 6, y + 3, 3, 2, col_sec);
    } else if (type == 3) {
        DrawRectangle(x, y + 3, 3, 2, col_sec);
        DrawRectangle(x + 3, y + 1, 2, 6, col_main);
        DrawRectangle(x + 5, y + 2, 2, 4, col_main);
        DrawRectangle(x + 7, y + 3, 2, 2, col_sec);
    } else if (type == 4) {
        DrawRectangle(x + 2, y + 3, 5, 2, GBA_COLOR(31, 31, 0));
        DrawRectangle(x + 4, y + 1, 1, 6, WHITE);
        DrawRectangle(x + 7, y + 2, 2, 4, GBA_COLOR(31, 15, 0));
    } else if (type == 5) {
        DrawRectangle(x + 2, y + 2, 4, 6, GBA_COLOR(31, 10, 0));
        DrawRectangle(x + 3, y + 3, 2, 4, GBA_COLOR(31, 28, 0));
        DrawRectangle(x + 1, y + 7, 6, 2, GBA_COLOR(31, 4, 0));
    } else if (type == 6) {
        DrawRectangle(x + 1, y + 1, 7, 7, col_main);
        DrawRectangle(x + 2, y + 2, 5, 5, GBA_COLOR(1, 4, 2));
        DrawRectangle(x + 4, y + 3, 1, 3, WHITE);
        DrawRectangle(x + 3, y + 4, 3, 1, WHITE);
    } else if (type == 7) {
        DrawRectangle(x, y + 1, 2, 6, col_main);
        DrawRectangle(x + 2, y + 3, 2, 2, col_main);
        DrawRectangle(x + 4, y + 1, 2, 6, col_sec);
        DrawRectangle(x + 6, y + 3, 2, 2, col_sec);
    }
}

void DrawLanguageFlag16Bit(int lang, int x, int y) {
    DrawRectangle(x, y, 12, 8, GBA_COLOR(0, 0, 0));
    if (lang == 0) {
        DrawRectangle(x + 1, y + 1, 10, 2, GBA_COLOR(31, 2, 2));
        DrawRectangle(x + 1, y + 3, 10, 2, GBA_COLOR(31, 28, 0));
        DrawRectangle(x + 1, y + 5, 10, 2, GBA_COLOR(31, 2, 2));
        DrawRectangle(x + 3, y + 3, 2, 2, GBA_COLOR(22, 10, 2));
    } else if (lang == 1) {
        DrawRectangle(x + 1, y + 1, 10, 6, GBA_COLOR(2, 6, 20));
        DrawRectangle(x + 1, y + 3, 10, 2, WHITE);
        DrawRectangle(x + 5, y + 1, 2, 6, WHITE);
        DrawRectangle(x + 1, y + 4, 10, 1, GBA_COLOR(31, 2, 2));
        DrawRectangle(x + 5, y + 1, 1, 6, GBA_COLOR(31, 2, 2));
    } else if (lang == 2) {
        DrawRectangle(x + 1, y + 1, 3, 6, GBA_COLOR(2, 8, 26));
        DrawRectangle(x + 4, y + 1, 4, 6, WHITE);
        DrawRectangle(x + 8, y + 1, 3, 6, GBA_COLOR(31, 2, 2));
    } else if (lang == 3) {
        DrawRectangle(x + 1, y + 1, 3, 6, GBA_COLOR(0, 22, 4));
        DrawRectangle(x + 4, y + 1, 4, 6, WHITE);
        DrawRectangle(x + 8, y + 1, 3, 6, GBA_COLOR(31, 2, 2));
    } else if (lang == 4) {
        DrawRectangle(x + 1, y + 1, 10, 2, GBA_COLOR(2, 2, 2));
        DrawRectangle(x + 1, y + 3, 10, 2, GBA_COLOR(31, 2, 2));
        DrawRectangle(x + 1, y + 5, 10, 2, GBA_COLOR(31, 24, 0));
    }
}

void DrawSpaceShipCursor16Bit(int x, int y, bool is_clicking) {
    int sz = 3;
    Color c_border  = GBA_COLOR(0, 4, 10);
    Color c_hull    = is_clicking ? GBA_COLOR(31, 28, 0) : GBA_COLOR(0, 24, 28);
    Color c_wing    = is_clicking ? GBA_COLOR(24, 20, 0) : GBA_COLOR(0, 16, 22);
    Color c_cockpit = is_clicking ? WHITE : GBA_COLOR(0, 31, 31);
    Color c_flame   = is_clicking ? GBA_COLOR(31, 4, 0) : GBA_COLOR(31, 18, 0);
    Color c_glow    = is_clicking ? GBA_COLOR(31, 28, 0) : WHITE;

    DrawRectangle(x, y, 2 * sz, 2 * sz, c_glow);
    DrawRectangle(x + (2 * sz), y, 1 * sz, 1 * sz, c_border);
    DrawRectangle(x, y + (2 * sz), 1 * sz, 1 * sz, c_border);

    DrawRectangle(x + (1 * sz), y + (1 * sz), 3 * sz, 3 * sz, c_hull);
    DrawRectangle(x + (2 * sz), y + (2 * sz), 2 * sz, 2 * sz, c_cockpit);

    DrawRectangle(x + (4 * sz), y + (2 * sz), 3 * sz, 2 * sz, c_wing);
    DrawRectangle(x + (2 * sz), y + (4 * sz), 2 * sz, 3 * sz, c_wing);
    DrawRectangle(x + (5 * sz), y + (3 * sz), 2 * sz, 3 * sz, c_border);
    DrawRectangle(x + (3 * sz), y + (5 * sz), 3 * sz, 2 * sz, c_border);

    DrawRectangle(x + (3 * sz), y + (3 * sz), 2 * sz, 2 * sz, c_hull);
    DrawRectangle(x + (4 * sz), y + (4 * sz), 2 * sz, 2 * sz, c_flame);
    
    if (is_clicking) {
        DrawRectangle(x + (5 * sz), y + (5 * sz), 3 * sz, 3 * sz, GBA_COLOR(31, 10, 0));
        DrawRectangle(x + (6 * sz), y + (6 * sz), 2 * sz, 2 * sz, C_YELLOW);
    } else {
        DrawRectangle(x + (5 * sz), y + (5 * sz), 2 * sz, 2 * sz, C_YELLOW);
    }
}

void DrawMenuSpaceFramePC(Color bg_color, const char* title, Color title_color, Color shadow_color, Color line_color) {
    DrawRectangle(0, 0, SCREEN_W, SCREEN_H, bg_color);
    
    // Marco exterior con esquinas biseladas estilo terminal sci-fi
    DrawBevelledBoxPC(4, 4, SCREEN_W - 8, SCREEN_H - 8, (Color){ 0, 0, 0, 0 }, line_color, true);
    
    // Brackets angulares en esquinas con micro-LEDs cian
    DrawRectangle(6, 6, 4, 1, C_CYAN);
    DrawRectangle(6, 6, 1, 4, C_CYAN);
    DrawRectangle(SCREEN_W - 10, 6, 4, 1, C_CYAN);
    DrawRectangle(SCREEN_W - 7, 6, 1, 4, C_CYAN);
    DrawRectangle(6, SCREEN_H - 7, 4, 1, C_CYAN);
    DrawRectangle(6, SCREEN_H - 10, 1, 4, C_CYAN);
    DrawRectangle(SCREEN_W - 10, SCREEN_H - 7, 4, 1, C_CYAN);
    DrawRectangle(SCREEN_W - 7, SCREEN_H - 10, 1, 4, C_CYAN);

    // Título y línea segmentada (solo si hay título)
    if (title != NULL && title[0] != '\0') {
        DrawHeaderCenteredStringCustom(title, 15, title_color, shadow_color, 2);
        int div_w = 190;
        int div_x = (SCREEN_W - div_w) / 2;
        DrawRectangle(div_x, 37, div_w, 1, line_color);
        // Marcador central
        DrawRectangle(div_x + div_w / 2 - 1, 36, 3, 3, title_color);
    }

    // Cápsula biselada para el mensaje de volver atrás (elevada y centrada, separada del borde)
    char back_msg[48];
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
    if (g_last_input_device != INPUT_GAMEPAD) {
        snprintf(back_msg, sizeof(back_msg), "%s", T(STR_TOUCH_BACK));
    } else {
        const char* back_key_txt = "B";
        snprintf(back_msg, sizeof(back_msg), T(STR_PRESS_BACK), back_key_txt);
    }
#else
    const char* back_key_txt = (g_last_input_device == INPUT_GAMEPAD) ? "B" : GetKeyNameCustom(g_keys.key_turbo);
    snprintf(back_msg, sizeof(back_msg), T(STR_PRESS_BACK), back_key_txt);
#endif
    
    int msg_len = MeasureStringCustom(back_msg, 1);
    int cap_w = msg_len + 16;
    int cap_x = (SCREEN_W - cap_w) / 2;
    int cap_y = SCREEN_H - 19;
    DrawBevelledBoxPC(cap_x, cap_y, cap_w, 11, GBA_COLOR(2, 6, 12), GBA_COLOR(6, 14, 20), true);
    DrawCenteredStringCustom(back_msg, cap_y + 2, GBA_COLOR(18, 22, 26), 1);
}


void DrawDeviceNotificationToast(int timer, InputDeviceType dev) {
    if (timer <= 0) return;
    int anim_offset = 0;
    if (timer > 100) anim_offset = (timer - 100) / 2;
    else if (timer < 20) anim_offset = (20 - timer) / 2;

    int box_w = 110;
    int box_h = 16;
    int box_x = SCREEN_W - box_w - 4 + anim_offset;
    int box_y = SCREEN_H - box_h - 4;

    DrawRectangle(box_x, box_y, box_w, box_h, GBA_COLOR(2, 6, 12));
    DrawRectangleLines(box_x, box_y, box_w, box_h, C_CYAN);

    if (dev == INPUT_GAMEPAD) {
        DrawGamepadIcon16Bit(box_x + 3, box_y + 3, false);
        DrawStringCustom(T(STR_GAMEPAD_CONNECTED), box_x + 18, box_y + 5, C_YELLOW, 1);
    } else {
        DrawKeyboardIcon16Bit(box_x + 2, box_y + 3, false);
        DrawStringCustom(T(STR_KEYBOARD_CONNECTED), box_x + 18, box_y + 5, C_CYAN, 1);
    }
}

void DrawAstralliticLogoPC(int start_x, int y, int frame_count) {
    // 1. Flotación suave estilo arcade (rango de -1 a +1 px para evitar colisiones con elementos vecinos)
    int hover_y = (int)roundf(sinf((float)frame_count * 0.045f) * 1.0f);
    int final_y = y + hover_y;

    // 2. Cálculo del barrido de brillo que aparece cada 4.5 segundos (270 frames a 60fps)
    int shine_period = 270;
    int shine_duration = 55;
    int shine_cycle = frame_count % shine_period;
    bool is_shining = (shine_cycle < shine_duration);
    float sweep_center = -50.0f;
    if (is_shining) {
        float t = (float)shine_cycle / (float)shine_duration;
        // Barrido diagonal de izquierda a derecha (coordenada u = x + y * 0.55)
        sweep_center = -30.0f + t * ((float)LOGO_WIDTH + 55.0f);
    }

    // 3. Sombra 3D proyectada en pixel art nativo (1px derecha, 2px abajo)
    Color c_shadow = (Color){ 2, 4, 10, 255 };
    for (int i = 0; i < LOGO_SPAN_COUNT; i++) {
        DrawRectangle(start_x + s_logo_spans[i].x + 1, final_y + s_logo_spans[i].y + 2, s_logo_spans[i].w, 1, c_shadow);
    }

    // 4. Logo oficial ASTRALLITIC cromado recreado en Pixel Art puro con destello
    for (int i = 0; i < LOGO_SPAN_COUNT; i++) {
        int sx = s_logo_spans[i].x;
        int sy = s_logo_spans[i].y;
        int sw = s_logo_spans[i].w;
        unsigned char col_idx = s_logo_spans[i].c;
        Color base_color = s_logo_palette[col_idx];

        if (!is_shining || col_idx == 1 || col_idx == 0) {
            // Sin brillo o borde exterior oscuro
            DrawRectangle(start_x + sx, final_y + sy, sw, 1, base_color);
            continue;
        }

        // Evaluar paso del brillo por el span
        float u = (float)sx + (float)sy * 0.55f;
        float dist = fabsf(u - sweep_center);

        if (dist < 4.0f) {
            // Núcleo del destello: blanco puro resplandeciente
            DrawRectangle(start_x + sx, final_y + sy, sw, 1, WHITE);
        } else if (dist < 10.0f) {
            // Halo de luz brillante
            Color shine_color = (Color){
                (unsigned char)((base_color.r + 255) / 2),
                (unsigned char)((base_color.g + 255) / 2),
                (unsigned char)((base_color.b + 255) / 2),
                255
            };
            DrawRectangle(start_x + sx, final_y + sy, sw, 1, shine_color);
        } else if (dist < 16.0f) {
            // Borde exterior suave del destello
            Color edge_color = (Color){
                (unsigned char)((base_color.r * 2 + 255) / 3),
                (unsigned char)((base_color.g * 2 + 255) / 3),
                (unsigned char)((base_color.b * 2 + 255) / 3),
                255
            };
            DrawRectangle(start_x + sx, final_y + sy, sw, 1, edge_color);
        } else {
            DrawRectangle(start_x + sx, final_y + sy, sw, 1, base_color);
        }
    }

    // 5. Destello estrellado 16-bit (sparkle) en la esquina superior derecha al terminar el barrido
    if (is_shining && shine_cycle >= 35 && shine_cycle <= 52) {
        int spk_age = shine_cycle - 35;
        int spk_x = start_x + 185;
        int spk_y = final_y + 8;
        
        int size = (spk_age < 5) ? 1 : ((spk_age < 12) ? 2 : 1);
        if (size == 1) {
            DrawRectangle(spk_x - 1, spk_y, 3, 1, WHITE);
            DrawRectangle(spk_x, spk_y - 1, 1, 3, WHITE);
        } else {
            DrawRectangle(spk_x - 2, spk_y, 5, 1, WHITE);
            DrawRectangle(spk_x, spk_y - 2, 1, 5, WHITE);
            DrawRectangle(spk_x - 1, spk_y - 1, 3, 3, WHITE);
            DrawRectangle(spk_x - 1, spk_y - 1, 1, 1, C_CYAN);
            DrawRectangle(spk_x + 1, spk_y - 1, 1, 1, C_CYAN);
            DrawRectangle(spk_x - 1, spk_y + 1, 1, 1, C_CYAN);
            DrawRectangle(spk_x + 1, spk_y + 1, 1, 1, C_CYAN);
        }
    }
}

void DrawScoreCustom(int score, int x, int y, Color color) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%d", score);
    DrawStringCustom(buf, x, y, color, 2);
}

void DrawHeartCustom(int x, int y, Color color) {
    DrawRectangle(x + 1, y, 2, 2, GBA_COLOR(31, 20, 20));
    DrawRectangle(x + 5, y, 2, 2, color);
    DrawRectangle(x, y + 2, 10, 4, color);
    DrawRectangle(x + 2, y + 6, 6, 2, GBA_COLOR(18, 0, 0));
    DrawRectangle(x + 4, y + 8, 2, 2, GBA_COLOR(12, 0, 0));
}

void DrawSkullIconPC(int x, int y) {
    DrawRectangle(x + 2, y, 8, 2, WHITE);
    DrawRectangle(x, y + 2, 12, 6, WHITE);
    DrawRectangle(x + 2, y + 8, 8, 3, WHITE);
    DrawRectangle(x + 2, y + 4, 2, 2, GBA_COLOR(14, 1, 2));
    DrawRectangle(x + 8, y + 4, 2, 2, GBA_COLOR(14, 1, 2));
    DrawRectangle(x + 4, y + 9, 1, 2, GBA_COLOR(14, 1, 2));
    DrawRectangle(x + 7, y + 9, 1, 2, GBA_COLOR(14, 1, 2));
}

static inline void DrawRectScaled(int x, int y, float rx, float ry, float rw, float rh, float s, Color col) {
    int dx = (int)roundf((float)x + rx * s);
    int dy = (int)roundf((float)y + ry * s);
    int dw = (int)roundf(rw * s);
    int dh = (int)roundf(rh * s);
    if (dw < 1) dw = 1;
    if (dh < 1) dh = 1;
    DrawRectangle(dx, dy, dw, dh, col);
}

void DrawPlayerShipScaledPC(int x, int y, int dx, int dy, Color base_color, float scale) {
    if (scale <= 0.0f) scale = 1.0f;
    float s = scale;
    Color c_cockpit = GBA_COLOR(0, 28, 31);
    Color c_glow    = WHITE;
    Color c_flame   = GBA_COLOR(31, 16, 0);
    Color c_shadow  = GBA_COLOR(8, 2, 2);

    if (dy < 0 && dx == 0) {
        DrawRectScaled(x, y, 4, 0, 2, 3, s, c_glow);
        DrawRectScaled(x, y, 3, 2, 4, 4, s, base_color);
        DrawRectScaled(x, y, 4, 3, 2, 2, s, c_cockpit);
        DrawRectScaled(x, y, 1, 6, 8, 3, s, base_color);
        DrawRectScaled(x, y, 0, 7, 2, 3, s, c_shadow);
        DrawRectScaled(x, y, 8, 7, 2, 3, s, c_shadow);
        DrawRectScaled(x, y, 4, 9, 2, 1, s, c_flame);
    } else if (dy > 0 && dx == 0) {
        DrawRectScaled(x, y, 4, 7, 2, 3, s, c_glow);
        DrawRectScaled(x, y, 3, 4, 4, 4, s, base_color);
        DrawRectScaled(x, y, 4, 5, 2, 2, s, c_cockpit);
        DrawRectScaled(x, y, 1, 1, 8, 3, s, base_color);
        DrawRectScaled(x, y, 0, 0, 2, 3, s, c_shadow);
        DrawRectScaled(x, y, 8, 0, 2, 3, s, c_shadow);
        DrawRectScaled(x, y, 4, 2, 2, 1, s, c_flame);
    } else if (dx > 0 && dy == 0) {
        DrawRectScaled(x, y, 7, 4, 3, 2, s, c_glow);
        DrawRectScaled(x, y, 4, 3, 4, 4, s, base_color);
        DrawRectScaled(x, y, 5, 4, 2, 2, s, c_cockpit);
        DrawRectScaled(x, y, 1, 1, 3, 8, s, base_color);
        DrawRectScaled(x, y, 0, 0, 3, 2, s, c_shadow);
        DrawRectScaled(x, y, 0, 8, 3, 2, s, c_shadow);
        DrawRectScaled(x, y, 0, 4, 1, 2, s, c_flame);
    } else if (dx < 0 && dy == 0) {
        DrawRectScaled(x, y, 0, 4, 3, 2, s, c_glow);
        DrawRectScaled(x, y, 2, 3, 4, 4, s, base_color);
        DrawRectScaled(x, y, 3, 4, 2, 2, s, c_cockpit);
        DrawRectScaled(x, y, 6, 1, 3, 8, s, base_color);
        DrawRectScaled(x, y, 7, 0, 3, 2, s, c_shadow);
        DrawRectScaled(x, y, 7, 8, 3, 2, s, c_shadow);
        DrawRectScaled(x, y, 9, 4, 1, 2, s, c_flame);
    } else if (dx > 0 && dy < 0) {
        DrawRectScaled(x, y, 7, 0, 2, 2, s, c_glow);
        DrawRectScaled(x, y, 4, 2, 4, 4, s, base_color);
        DrawRectScaled(x, y, 5, 3, 2, 2, s, c_cockpit);
        DrawRectScaled(x, y, 0, 6, 6, 4, s, base_color);
        DrawRectScaled(x, y, 0, 8, 2, 2, s, c_flame);
    } else if (dx < 0 && dy < 0) {
        DrawRectScaled(x, y, 1, 0, 2, 2, s, c_glow);
        DrawRectScaled(x, y, 2, 2, 4, 4, s, base_color);
        DrawRectScaled(x, y, 3, 3, 2, 2, s, c_cockpit);
        DrawRectScaled(x, y, 4, 6, 6, 4, s, base_color);
        DrawRectScaled(x, y, 8, 8, 2, 2, s, c_flame);
    } else if (dx > 0 && dy > 0) {
        DrawRectScaled(x, y, 7, 8, 2, 2, s, c_glow);
        DrawRectScaled(x, y, 4, 4, 4, 4, s, base_color);
        DrawRectScaled(x, y, 5, 4, 2, 2, s, c_cockpit);
        DrawRectScaled(x, y, 0, 0, 6, 4, s, base_color);
        DrawRectScaled(x, y, 0, 2, 2, 2, s, c_flame);
    } else if (dx < 0 && dy > 0) {
        DrawRectScaled(x, y, 1, 8, 2, 2, s, c_glow);
        DrawRectScaled(x, y, 2, 4, 4, 4, s, base_color);
        DrawRectScaled(x, y, 3, 4, 2, 2, s, c_cockpit);
        DrawRectScaled(x, y, 4, 0, 6, 4, s, base_color);
        DrawRectScaled(x, y, 8, 2, 2, 2, s, c_flame);
    } else {
        DrawRectScaled(x, y, 2, 2, 6, 6, s, base_color);
        DrawRectScaled(x, y, 4, 4, 2, 2, s, c_cockpit);
        DrawRectScaled(x, y, (dx > 0 ? 6 : 0), (dy > 0 ? 6 : 0), 4, 4, s, c_glow);
        DrawRectScaled(x, y, (dx > 0 ? 0 : 6), (dy > 0 ? 0 : 6), 4, 4, s, c_shadow);
    }
}

void DrawPlayerShipPC(int x, int y, int dx, int dy, Color base_color) {
    DrawPlayerShipScaledPC(x, y, dx, dy, base_color, 1.0f);
}

void DrawEnemyScaledPC(Enemy *e, int frame, int scale) {
    if (scale <= 0) scale = 1;
    Color color;
    if (e->hit_flash_timer > 0) color = WHITE;
    else if (e->freeze_timer > 0) color = C_ICE;
    else if (e->wet_timer > 0) color = C_NAVY_BLUE;
    else if (e->poison_timer > 0) color = C_POISON_TINT;
    else {
        if(e->type == 0) color = C_ENEMY_N;
        else if(e->type == 1) color = C_ENEMY_F;
        else if(e->type == 2) color = C_ENEMY_T;
        else if(e->type == 4) color = C_SNIPER;
        else if(e->type == 5 || e->type == 6) color = C_DIVISOR;
        else if(e->type == 7) color = C_KAMIKAZE;
        else if(e->type == 8) color = C_PATROLLER;
        else if(e->type == 9) color = C_TELEPORT;
        else color = C_BOSS;
    }

    int x = e->x, y = e->y;
    int s = scale;

    if (e->type == 0) {
        int anim = (frame / 8) % 2;
        DrawRectangle(x + 4*s, y, 2*s, 3*s, color);
        DrawRectangle(x + 2*s, y + 3*s, 6*s, 3*s, color);
        DrawRectangle(x, y + 5*s, 10*s, 3*s, color);
        DrawRectangle(x + 4*s, y + 4*s, 2*s, 2*s, WHITE);
        if (anim == 0) {
            DrawRectangle(x + 1*s, y + 8*s, 2*s, 2*s, GBA_COLOR(31, 16, 0));
            DrawRectangle(x + 7*s, y + 8*s, 2*s, 2*s, GBA_COLOR(31, 16, 0));
        } else {
            DrawRectangle(x + 1*s, y + 8*s, 2*s, 1*s, GBA_COLOR(31, 31, 0));
            DrawRectangle(x + 7*s, y + 8*s, 2*s, 1*s, GBA_COLOR(31, 31, 0));
        }
    } else if (e->type == 1) {
        int anim = (frame / 6) % 2;
        DrawRectangle(x + 4*s, y, 2*s, 2*s, color);
        DrawRectangle(x + 3*s, y + 2*s, 4*s, 3*s, color);
        DrawRectangle(x + 1*s, y + 5*s, 8*s, 3*s, color);
        DrawRectangle(x + 4*s, y + 3*s, 2*s, 2*s, C_CYAN);
        if (anim == 0) {
            DrawRectangle(x + 2*s, y + 8*s, 2*s, 1*s, C_YELLOW);
            DrawRectangle(x + 6*s, y + 8*s, 2*s, 1*s, C_YELLOW);
        } else {
            DrawRectangle(x + 3*s, y + 8*s, 2*s, 2*s, C_RED);
        }
    } else if (e->type == 2) {
        int pulse = (frame / 12) % 2;
        DrawRectangle(x + 2*s, y, 6*s, 2*s, color);
        DrawRectangle(x, y + 2*s, 10*s, 6*s, color);
        DrawRectangle(x + 2*s, y + 3*s, 6*s, 4*s, pulse ? C_RED : C_YELLOW);
        DrawRectangle(x + 1*s, y + 8*s, 2*s, 2*s, color);
        DrawRectangle(x + 7*s, y + 8*s, 2*s, 2*s, color);
    } else if (e->type == 4) {
        int glow = (frame / 15) % 2;
        DrawRectangle(x + 4*s, y, 2*s, 10*s, color);
        DrawRectangle(x, y + 4*s, 10*s, 2*s, color);
        DrawRectangle(x + 3*s, y + 3*s, 4*s, 4*s, color);
        DrawRectangle(x + 4*s, y + 4*s, 2*s, 2*s, glow ? WHITE : C_CYAN);
    } else if (e->type == 5) {
        int anim = (frame / 10) % 2;
        DrawRectangle(x + 2*s, y + 1*s, 6*s, 8*s, color);
        DrawRectangle(x + 1*s, y + 3*s, 8*s, 4*s, color);
        DrawRectangle(x + 3*s, y + 3*s, 4*s, 4*s, anim ? WHITE : C_YELLOW);
        DrawRectangle(x, y + 4*s, 2*s, 2*s, color);
        DrawRectangle(x + 8*s, y + 4*s, 2*s, 2*s, color);
    } else if (e->type == 6) {
        DrawRectangle(x + 2*s, y + 2*s, 6*s, 6*s, color);
        DrawRectangle(x + 3*s, y + 3*s, 4*s, 4*s, C_RED);
        if((frame / 4) % 2 == 0) DrawRectangle(x + 4*s, y + 4*s, 2*s, 2*s, WHITE);
    } else if (e->type == 7) {
        int trail = (frame / 3) % 2;
        DrawRectangle(x + 4*s, y, 2*s, 3*s, WHITE);
        DrawRectangle(x + 3*s, y + 3*s, 4*s, 4*s, color);
        DrawRectangle(x + 2*s, y + 7*s, 6*s, 2*s, C_RED);
        if (trail == 0) {
            DrawRectangle(x + 4*s, y + 9*s, 2*s, 2*s, C_YELLOW);
        } else {
            DrawRectangle(x + 3*s, y + 9*s, 4*s, 1*s, C_YELLOW);
        }
    } else if (e->type == 8) {
        int wing_anim = (frame / 7) % 2;
        DrawRectangle(x + 3*s, y + 3*s, 4*s, 4*s, color);
        if (wing_anim == 0) {
            DrawRectangle(x, y + 1*s, 3*s, 3*s, C_CYAN);
            DrawRectangle(x + 7*s, y + 1*s, 3*s, 3*s, C_CYAN);
        } else {
            DrawRectangle(x, y + 5*s, 3*s, 3*s, C_CYAN);
            DrawRectangle(x + 7*s, y + 5*s, 3*s, 3*s, C_CYAN);
        }
        DrawRectangle(x + 4*s, y + 4*s, 2*s, 2*s, WHITE);
    } else if (e->type == 9) {
        // Teletransportador: Cristal cuántico con pulso energético suave
        int pulse = (frame / 15) % 2;
        DrawRectangle(x + 2*s, y, 6*s, 10*s, color);
        DrawRectangle(x, y + 2*s, 10*s, 6*s, color);
        DrawRectangle(x + 3*s, y + 3*s, 4*s, 4*s, pulse ? WHITE : C_CYAN);
        // Pequeños destellos en los extremos
        DrawRectangle(x + 4*s, y, 2*s, 1*s, WHITE);
        DrawRectangle(x + 4*s, y + 9*s, 2*s, 1*s, WHITE);
    } else {
        int core_pulse = (frame / 10) % 2;
        DrawRectangle(x, y + 2*s, 10*s, 6*s, color);
        DrawRectangle(x + 2*s, y, 6*s, 8*s, color);
        DrawRectangle(x + 1*s, y + 8*s, 8*s, 2*s, color);
        DrawRectangle(x - 2*s, y + 4*s, 2*s, 4*s, C_RED);
        DrawRectangle(x + 10*s, y + 4*s, 2*s, 4*s, C_RED);
        DrawRectangle(x + 3*s, y + 3*s, 4*s, 4*s, core_pulse ? WHITE : C_YELLOW);
    }
}

void DrawEnemyPC(Enemy *e, int frame) {
    DrawEnemyScaledPC(e, frame, 1);
}

void DrawPadlockPC(int cx, int cy) {
    int x = cx - 7;
    int y = cy - 8;

    Color iron = GBA_COLOR(16, 20, 24);
    Color iron_hi = GBA_COLOR(25, 28, 31);
    Color iron_shadow = GBA_COLOR(8, 10, 14);

    Color brass = GBA_COLOR(24, 18, 6);
    Color brass_hi = GBA_COLOR(31, 26, 12);
    Color brass_shadow = GBA_COLOR(14, 10, 2);

    // Arco metálico con relieve e iluminación limpia (el fondo se ve a través del hueco)
    DrawRectangle(x + 4, y, 6, 2, iron_hi);
    DrawRectangle(x + 3, y + 1, 1, 2, iron);
    DrawRectangle(x + 10, y + 1, 1, 2, iron_shadow);
    DrawRectangle(x + 3, y + 2, 2, 4, iron);
    DrawRectangle(x + 3, y + 2, 1, 4, iron_hi);
    DrawRectangle(x + 9, y + 2, 2, 4, iron);
    DrawRectangle(x + 10, y + 2, 1, 4, iron_shadow);

    // Cuerpo dorado del candado
    DrawRectangle(x + 1, y + 6, 12, 10, brass);
    DrawRectangle(x + 2, y + 6, 10, 1, brass_hi);
    DrawRectangle(x + 1, y + 7, 1, 8, brass_hi);
    DrawRectangle(x + 12, y + 7, 1, 8, brass_shadow);
    DrawRectangle(x + 2, y + 15, 10, 1, brass_shadow);

    // Ojo de la cerradura (Keyhole)
    DrawRectangle(x + 6, y + 9, 2, 3, BLACK);
    DrawRectangle(x + 5, y + 12, 4, 2, BLACK);
}

Color GetPowerUpThemeColor(int type) {
    switch (type) {
        case 1:  return C_SHIELD;
        case 2:  return C_RAPID;
        case 3:  return C_GREEN;
        case 4:  return C_HEART;
        case 5:  return GBA_COLOR(31, 15, 0);
        case 6:  return C_BOSS;
        case 7:  return C_CYAN;
        case 8:  return GBA_COLOR(31, 28, 0);
        case 9:  return C_ICE;
        case 10: return C_BOTTLE;
        case 11: return C_NUKE;
        case 12: return GBA_COLOR(20, 0, 31);
        case 13: return GBA_COLOR(31, 4, 4);
        case 14: return C_NAVY_BLUE;
        default: return C_YELLOW;
    }
}

void DrawPowerUpPC(PowerUp *p) {
    if (!p || !p->active) return;
    int life = p->life_timer;
    int age = 1800 - life;
    if (age < 0) age = 0;

    // Despawn blink warning (last 300 frames = 5 seconds)
    if (life < 300) {
        int blink_rate = (life < 60) ? 6 : ((life < 150) ? 10 : 16);
        if ((life % blink_rate) < (blink_rate / 2)) {
            return; // In blink phase, don't draw
        }
    }

    // Subtle gentle floating bob (smooth ±1.5px)
    // Helps players distinguish collectibles from enemies immediately
    int bob = (int)roundf(sinf((float)age * 0.08f) * 1.5f);
    int x = p->x;
    int y = p->y + bob;

    Color theme_col = GetPowerUpThemeColor(p->type);

    // 1. Spawn effect: expanding diamond ring and center flash (first 12 frames)
    if (age < 12) {
        int r = age;
        int cx = x + 5;
        int cy = y + 5;
        DrawLine(cx - r, cy, cx, cy - r, BlendColorAlpha(WHITE, (12 - age) * 20));
        DrawLine(cx, cy - r, cx + r, cy, BlendColorAlpha(WHITE, (12 - age) * 20));
        DrawLine(cx + r, cy, cx, cy + r, BlendColorAlpha(WHITE, (12 - age) * 20));
        DrawLine(cx, cy + r, cx - r, cy, BlendColorAlpha(WHITE, (12 - age) * 20));

        int r2 = r / 2;
        DrawLine(cx - r2, cy, cx, cy - r2, BlendColorAlpha(theme_col, 200));
        DrawLine(cx, cy - r2, cx + r2, cy, BlendColorAlpha(theme_col, 200));
        DrawLine(cx + r2, cy, cx, cy + r2, BlendColorAlpha(theme_col, 200));
        DrawLine(cx, cy + r2, cx - r2, cy, BlendColorAlpha(theme_col, 200));
    }

    // 2. Ambient pulsing corner halo (brackets framing the power-up)
    int halo_alpha = 75 + (int)(sinf((float)age * 0.12f) * 45.0f);
    Color halo_col = BlendColorAlpha(theme_col, halo_alpha);
    DrawRectangle(x - 1, y - 1, 2, 1, halo_col);
    DrawRectangle(x - 1, y - 1, 1, 2, halo_col);
    DrawRectangle(x + 9, y - 1, 2, 1, halo_col);
    DrawRectangle(x + 10, y - 1, 1, 2, halo_col);
    DrawRectangle(x - 1, y + 10, 2, 1, halo_col);
    DrawRectangle(x - 1, y + 9, 1, 2, halo_col);
    DrawRectangle(x + 9, y + 10, 2, 1, halo_col);
    DrawRectangle(x + 10, y + 9, 1, 2, halo_col);

    // 3. Base PowerUp icon
    if (p->type == 1) {
        DrawRectangle(x + 2, y, 6, 2, GBA_COLOR(18, 26, 31));
        DrawRectangle(x + 1, y + 2, 8, 5, C_SHIELD);
        DrawRectangle(x + 2, y + 7, 6, 2, GBA_COLOR(8, 14, 26));
        DrawRectangle(x + 4, y + 9, 2, 1, GBA_COLOR(4, 8, 16));
        DrawRectangle(x + 4, y + 2, 2, 5, WHITE);
        DrawRectangle(x + 2, y + 4, 6, 2, WHITE);
    } else if (p->type == 2) {
        DrawRectangle(x + 5, y, 3, 2, WHITE);
        DrawRectangle(x + 4, y + 2, 3, 3, C_RAPID);
        DrawRectangle(x + 2, y + 4, 6, 2, WHITE);
        DrawRectangle(x + 3, y + 6, 3, 2, C_RAPID);
        DrawRectangle(x + 2, y + 8, 2, 2, GBA_COLOR(31, 20, 0));
    } else if (p->type == 3) {
        DrawRectangle(x + 4, y, 2, 3, WHITE);
        DrawRectangle(x + 1, y + 2, 2, 3, GBA_COLOR(20, 31, 20));
        DrawRectangle(x + 7, y + 2, 2, 3, GBA_COLOR(20, 31, 20));
        DrawRectangle(x + 2, y + 5, 6, 3, C_GREEN);
        DrawRectangle(x + 3, y + 8, 4, 2, GBA_COLOR(0, 16, 4));
    } else if (p->type == 4) {
        DrawHeartCustom(x, y, C_HEART);
    } else if (p->type == 5) {
        DrawRectangle(x + 3, y, 4, 10, GBA_COLOR(31, 15, 0));
        DrawRectangle(x + 1, y + 2, 8, 2, GBA_COLOR(31, 15, 0));
        DrawRectangle(x + 4, y + 2, 2, 6, WHITE);
    } else if (p->type == 6) {
        DrawRectangle(x + 4, y, 2, 10, WHITE);
        DrawRectangle(x, y + 4, 10, 2, WHITE);
        DrawRectangle(x + 2, y + 2, 6, 6, C_BOSS);
        DrawRectangle(x + 3, y + 3, 4, 4, GBA_COLOR(31, 16, 31));
    } else if (p->type == 7) {
        DrawRectangle(x + 2, y, 6, 2, GBA_COLOR(20, 31, 31));
        DrawRectangle(x, y + 2, 10, 6, C_CYAN);
        DrawRectangle(x + 2, y + 8, 6, 2, GBA_COLOR(0, 16, 20));
        DrawRectangle(x + 2, y + 2, 6, 6, GBA_COLOR(2, 6, 12));
        DrawRectangle(x + 4, y + 3, 2, 3, WHITE);
        DrawRectangle(x + 4, y + 5, 3, 1, WHITE);
    } else if (p->type == 8) {
        DrawRectangle(x + 2, y, 6, 10, GBA_COLOR(31, 28, 0));
        DrawRectangle(x, y + 2, 10, 6, GBA_COLOR(31, 28, 0));
        DrawRectangle(x + 1, y + 1, 8, 8, GBA_COLOR(31, 28, 6));
        DrawRectangle(x + 3, y + 3, 2, 2, GBA_COLOR(10, 8, 0));
        DrawRectangle(x + 6, y + 3, 2, 2, GBA_COLOR(10, 8, 0));
        DrawRectangle(x + 4, y + 4, 2, 2, GBA_COLOR(10, 8, 0));
        DrawRectangle(x + 3, y + 6, 2, 2, GBA_COLOR(10, 8, 0));
        DrawRectangle(x + 6, y + 6, 2, 2, GBA_COLOR(10, 8, 0));
    } else if (p->type == 9) {
        DrawRectangle(x + 4, y, 2, 10, WHITE);
        DrawRectangle(x, y + 4, 10, 2, WHITE);
        DrawRectangle(x + 2, y + 2, 2, 2, C_ICE);
        DrawRectangle(x + 6, y + 2, 2, 2, C_ICE);
        DrawRectangle(x + 2, y + 6, 2, 2, C_ICE);
        DrawRectangle(x + 6, y + 6, 2, 2, C_ICE);
    } else if (p->type == 10) {
        DrawRectangle(x + 4, y, 2, 2, GBA_COLOR(20, 14, 8));
        DrawRectangle(x + 3, y + 2, 4, 2, GBA_COLOR(18, 28, 20));
        DrawRectangle(x + 1, y + 4, 8, 6, C_BOTTLE);
        DrawRectangle(x + 3, y + 6, 4, 3, GBA_COLOR(16, 31, 16));
    } else if (p->type == 11) {
        DrawRectangle(x + 3, y, 4, 2, GBA_COLOR(26, 26, 26));
        DrawRectangle(x + 1, y + 2, 8, 8, C_NUKE);
        DrawRectangle(x + 4, y + 3, 2, 2, GBA_COLOR(31, 24, 0));
        DrawRectangle(x + 2, y + 6, 2, 2, GBA_COLOR(31, 0, 0));
        DrawRectangle(x + 6, y + 6, 2, 2, GBA_COLOR(31, 0, 0));
    } else if (p->type == 12) {
        DrawRectangle(x + 4, y, 2, 2, GBA_COLOR(24, 18, 31));
        DrawRectangle(x + 3, y + 2, 4, 2, GBA_COLOR(18, 12, 24));
        DrawRectangle(x + 1, y + 4, 8, 6, GBA_COLOR(20, 0, 31));
        DrawRectangle(x + 3, y + 6, 4, 3, WHITE);
    } else if (p->type == 13) {
        DrawRectangle(x, y, 10, 10, GBA_COLOR(31, 0, 0));
        DrawRectangle(x + 1, y + 1, 8, 8, GBA_COLOR(16, 0, 0));
        DrawRectangle(x + 2, y + 2, 2, 6, WHITE);
        DrawRectangle(x + 6, y + 2, 2, 6, WHITE);
        DrawRectangle(x + 6, y + 2, 2, 2, WHITE);
        DrawRectangle(x + 6, y + 6, 2, 2, WHITE);
    } else if (p->type == 14) {
        DrawRectangle(x + 4, y, 2, 2, C_NAVY_BLUE);
        DrawRectangle(x + 3, y + 2, 4, 2, C_NAVY_BLUE);
        DrawRectangle(x + 2, y + 4, 6, 2, C_NAVY_BLUE);
        DrawRectangle(x + 1, y + 6, 8, 3, C_NAVY_BLUE);
        DrawRectangle(x + 2, y + 9, 6, 1, C_NAVY_BLUE);
        DrawRectangle(x + 3, y + 3, 2, 2, WHITE);
        DrawRectangle(x + 2, y + 5, 2, 2, C_SHIELD);
    }

    // 4. Periodic Glint / Star Sparkle effect (every ~2 seconds = 120 frames)
    int glint_cycle = (age + 30) % 120;
    if (glint_cycle < 16) {
        int t = glint_cycle;
        bool alt_corner = (((age + 30) / 120) % 2) == 1;
        int gx = alt_corner ? (x + 2) : (x + 7);
        int gy = alt_corner ? (y + 7) : (y + 2);

        if (t < 3 || t >= 13) {
            DrawRectangle(gx, gy, 1, 1, WHITE);
        } else if ((t >= 3 && t < 6) || (t >= 10 && t < 13)) {
            DrawRectangle(gx, gy, 1, 1, WHITE);
            DrawRectangle(gx - 1, gy, 1, 1, BlendColorAlpha(WHITE, 200));
            DrawRectangle(gx + 1, gy, 1, 1, BlendColorAlpha(WHITE, 200));
            DrawRectangle(gx, gy - 1, 1, 1, BlendColorAlpha(WHITE, 200));
            DrawRectangle(gx, gy + 1, 1, 1, BlendColorAlpha(WHITE, 200));
        } else {
            // Peak brilliance (t == 6..9): 4-pointed sparkle star with theme color halo
            DrawRectangle(gx - 1, gy, 3, 1, WHITE);
            DrawRectangle(gx, gy - 1, 1, 3, WHITE);
            DrawRectangle(gx - 2, gy, 1, 1, theme_col);
            DrawRectangle(gx + 2, gy, 1, 1, theme_col);
            DrawRectangle(gx, gy - 2, 1, 1, theme_col);
            DrawRectangle(gx + 2, gy, 1, 1, theme_col);
            DrawRectangle(gx - 1, gy - 1, 1, 1, BlendColorAlpha(WHITE, 180));
            DrawRectangle(gx + 1, gy - 1, 1, 1, BlendColorAlpha(WHITE, 180));
            DrawRectangle(gx - 1, gy + 1, 1, 1, BlendColorAlpha(WHITE, 180));
            DrawRectangle(gx + 1, gy + 1, 1, 1, BlendColorAlpha(WHITE, 180));
        }
    }
}

const char* GetGamepadButtonNameCustom(int btn) {
    if (btn == GAMEPAD_BUTTON_LEFT_FACE_UP) return "DPAD UP";
    if (btn == GAMEPAD_BUTTON_LEFT_FACE_DOWN) return "DPAD DOWN";
    if (btn == GAMEPAD_BUTTON_LEFT_FACE_LEFT) return "DPAD LEFT";
    if (btn == GAMEPAD_BUTTON_LEFT_FACE_RIGHT) return "DPAD RIGHT";
    if (btn == GAMEPAD_BUTTON_RIGHT_FACE_UP) return "Y";
    if (btn == GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) return "B";
    if (btn == GAMEPAD_BUTTON_RIGHT_FACE_DOWN) return "A";
    if (btn == GAMEPAD_BUTTON_RIGHT_FACE_LEFT) return "X";
    if (btn == GAMEPAD_BUTTON_LEFT_TRIGGER_1) return "LB";
    if (btn == GAMEPAD_BUTTON_LEFT_TRIGGER_2) return "LT";
    if (btn == GAMEPAD_BUTTON_RIGHT_TRIGGER_1) return "RB";
    if (btn == GAMEPAD_BUTTON_RIGHT_TRIGGER_2) return "RT";
    if (btn == GAMEPAD_BUTTON_MIDDLE_RIGHT) return "START";
    if (btn == GAMEPAD_BUTTON_MIDDLE_LEFT) return "BACK";
    return "BTN";
}

Color GetGamepadButtonColorCustom(int btn) {
    if (btn == GAMEPAD_BUTTON_RIGHT_FACE_DOWN) return C_XBOX_A;
    if (btn == GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) return C_XBOX_B;
    if (btn == GAMEPAD_BUTTON_RIGHT_FACE_LEFT)  return C_XBOX_X;
    if (btn == GAMEPAD_BUTTON_RIGHT_FACE_UP)    return C_XBOX_Y;
    if (btn == GAMEPAD_BUTTON_LEFT_TRIGGER_1 || btn == GAMEPAD_BUTTON_RIGHT_TRIGGER_1 ||
        btn == GAMEPAD_BUTTON_LEFT_TRIGGER_2 || btn == GAMEPAD_BUTTON_RIGHT_TRIGGER_2) {
        return GBA_COLOR(22, 24, 26);
    }
    return C_CYAN;
}

void DrawGhostTrailsPC(GhostTrail *trails, int count) {
    for (int i = 0; i < count; i++) {
        if (trails[i].timer <= 0) continue;
        float alpha = (float)trails[i].timer / (float)trails[i].max_timer;
        Color c = ColorAlpha(trails[i].color, alpha * 0.55f);
        DrawPlayerShipPC((int)roundf(trails[i].x), (int)roundf(trails[i].y), trails[i].dx, trails[i].dy, c);
    }
}

void DrawParticlesPC(Particle *particles, int count) {
    for (int i = 0; i < count; i++) {
        if (particles[i].life <= 0) continue;
        float alpha = (float)particles[i].life / (float)particles[i].max_life;
        Color c = ColorAlpha(particles[i].color, alpha);
        DrawRectangle((int)roundf(particles[i].x), (int)roundf(particles[i].y), particles[i].size, particles[i].size, c);
    }
}

void DrawDashGaugePC(int ship_x, int ship_y, int dash_cd, int max_cd, int ready_flash) {
    int bar_w = 10;
    int bar_h = 1;
    int bar_x = ship_x;
    int bar_y = ship_y + 11;

    if (dash_cd > 0) {
        float pct = 1.0f - ((float)dash_cd / (float)max_cd);
        int fill_w = (int)(pct * (float)bar_w);
        DrawRectangle(bar_x, bar_y, bar_w, bar_h, GBA_COLOR(1, 4, 8));
        if (fill_w > 0) {
            DrawRectangle(bar_x, bar_y, fill_w, bar_h, C_CYAN);
        }
    } else if (ready_flash > 0) {
        Color flash_col = (ready_flash % 4 < 2) ? WHITE : C_YELLOW;
        DrawRectangle(bar_x, bar_y, bar_w, bar_h, flash_col);
    }
}

void DrawDamageFlashPC(int flash_timer) {
    if (flash_timer <= 0) return;
    unsigned char a = (unsigned char)(flash_timer * 30);
    if (a > 110) a = 110;
    Color border_c = (Color){ 255, 20, 20, a };
    DrawRectangleLinesEx((Rectangle){ 0, 16, (float)SCREEN_W, (float)(SCREEN_H - 16) }, 2, border_c);
}

void DrawBevelledBoxPC(int x, int y, int w, int h, Color bg_color, Color border_color, bool bevel_corners) {
    if (w <= 4 || h <= 4) {
        if (bg_color.a > 0) DrawRectangle(x, y, w, h, bg_color);
        if (border_color.a > 0) DrawRectangleLines(x, y, w, h, border_color);
        return;
    }

    if (bevel_corners) {
        if (bg_color.a > 0) {
            DrawRectangle(x + 2, y, w - 4, h, bg_color);
            DrawRectangle(x, y + 2, 2, h - 4, bg_color);
            DrawRectangle(x + w - 2, y + 2, 2, h - 4, bg_color);
            DrawRectangle(x + 1, y + 1, 1, 1, bg_color);
            DrawRectangle(x + w - 2, y + 1, 1, 1, bg_color);
            DrawRectangle(x + 1, y + h - 2, 1, 1, bg_color);
            DrawRectangle(x + w - 2, y + h - 2, 1, 1, bg_color);
        }
        if (border_color.a > 0) {
            DrawRectangle(x + 2, y, w - 4, 1, border_color);
            DrawRectangle(x + 2, y + h - 1, w - 4, 1, border_color);
            DrawRectangle(x, y + 2, 1, h - 4, border_color);
            DrawRectangle(x + w - 1, y + 2, 1, h - 4, border_color);
            // 4 biseles a 45 grados
            DrawRectangle(x + 1, y + 1, 1, 1, border_color);
            DrawRectangle(x + w - 2, y + 1, 1, 1, border_color);
            DrawRectangle(x + 1, y + h - 2, 1, 1, border_color);
            DrawRectangle(x + w - 2, y + h - 2, 1, 1, border_color);
        }
    } else {
        if (bg_color.a > 0) DrawRectangle(x, y, w, h, bg_color);
        if (border_color.a > 0) DrawRectangleLines(x, y, w, h, border_color);
    }
}

void DrawMenuIconPC(int icon_type, int x, int y, Color theme_color) {
    switch (icon_type) {
        case MENU_ICON_PLAY: {
            DrawRectangle(x + 4, y + 1, 2, 5, theme_color);
            DrawRectangle(x + 3, y + 2, 4, 4, theme_color);
            DrawRectangle(x + 1, y + 4, 8, 3, theme_color);
            DrawRectangle(x, y + 6, 10, 2, theme_color);
            DrawRectangle(x + 4, y + 3, 2, 2, C_CYAN);
            DrawRectangle(x + 2, y + 8, 2, 2, GBA_COLOR(31, 14, 0));
            DrawRectangle(x + 6, y + 8, 2, 2, GBA_COLOR(31, 14, 0));
            DrawRectangle(x + 3, y + 8, 1, 1, C_YELLOW);
            DrawRectangle(x + 7, y + 8, 1, 1, C_YELLOW);
            break;
        }
        case MENU_ICON_OPTIONS: {
            DrawRectangle(x + 4, y, 2, 10, theme_color);
            DrawRectangle(x, y + 4, 10, 2, theme_color);
            DrawRectangle(x + 2, y + 2, 6, 6, theme_color);
            DrawRectangle(x + 1, y + 1, 2, 2, theme_color);
            DrawRectangle(x + 7, y + 1, 2, 2, theme_color);
            DrawRectangle(x + 1, y + 7, 2, 2, theme_color);
            DrawRectangle(x + 7, y + 7, 2, 2, theme_color);
            DrawRectangle(x + 4, y + 4, 2, 2, GBA_COLOR(2, 6, 14));
            break;
        }
        case MENU_ICON_RECORDS: {
            Color gold = GBA_COLOR(31, 26, 2);
            Color gold_dark = GBA_COLOR(20, 16, 0);
            DrawRectangle(x + 1, y + 1, 8, 4, gold);
            DrawRectangle(x + 2, y + 5, 6, 2, gold);
            DrawRectangle(x + 4, y + 7, 2, 2, gold_dark);
            DrawRectangle(x + 2, y + 9, 6, 1, gold);
            DrawRectangle(x, y + 1, 1, 3, gold_dark);
            DrawRectangle(x + 9, y + 1, 1, 3, gold_dark);
            DrawRectangle(x + 2, y + 2, 1, 2, WHITE);
            break;
        }
        case MENU_ICON_DIFF_EASY: {
            Color emerald = GBA_COLOR(0, 31, 14);
            DrawRectangle(x + 2, y + 1, 6, 2, emerald);
            DrawRectangle(x + 1, y + 3, 8, 3, emerald);
            DrawRectangle(x + 2, y + 6, 6, 2, emerald);
            DrawRectangle(x + 3, y + 8, 4, 1, emerald);
            DrawRectangle(x + 4, y + 9, 2, 1, emerald);
            DrawRectangle(x + 3, y + 3, 4, 3, GBA_COLOR(14, 31, 20));
            DrawRectangle(x + 4, y + 4, 2, 2, WHITE);
            break;
        }
        case MENU_ICON_DIFF_NORMAL: {
            Color cy = C_CYAN;
            DrawRectangle(x + 4, y, 2, 2, cy);
            DrawRectangle(x + 3, y + 2, 4, 2, cy);
            DrawRectangle(x + 1, y + 4, 8, 2, cy);
            DrawRectangle(x + 3, y + 6, 4, 2, cy);
            DrawRectangle(x + 4, y + 8, 2, 2, cy);
            DrawRectangle(x + 4, y + 4, 2, 2, WHITE);
            break;
        }
        case MENU_ICON_DIFF_HARD: {
            Color amb = GBA_COLOR(31, 18, 0);
            DrawRectangle(x + 2, y + 1, 6, 5, amb);
            DrawRectangle(x + 1, y + 2, 8, 3, amb);
            DrawRectangle(x + 3, y + 6, 4, 3, amb);
            DrawRectangle(x + 2, y + 3, 2, 2, GBA_COLOR(2, 4, 8));
            DrawRectangle(x + 6, y + 3, 2, 2, GBA_COLOR(2, 4, 8));
            DrawRectangle(x + 4, y + 7, 2, 1, GBA_COLOR(2, 4, 8));
            break;
        }
        case MENU_ICON_DIFF_HELL: {
            Color red = GBA_COLOR(31, 4, 4);
            DrawRectangle(x + 1, y, 1, 2, GBA_COLOR(31, 16, 0));
            DrawRectangle(x + 8, y, 1, 2, GBA_COLOR(31, 16, 0));
            DrawRectangle(x + 2, y + 2, 6, 5, red);
            DrawRectangle(x + 1, y + 3, 8, 3, red);
            DrawRectangle(x + 3, y + 7, 4, 2, red);
            DrawRectangle(x + 2, y + 4, 2, 1, C_YELLOW);
            DrawRectangle(x + 6, y + 4, 2, 1, C_YELLOW);
            break;
        }
        case MENU_ICON_PADLOCK: {
            Color iron = GBA_COLOR(14, 18, 22);
            DrawRectangle(x + 3, y + 1, 4, 1, iron);
            DrawRectangle(x + 3, y + 2, 1, 3, iron);
            DrawRectangle(x + 6, y + 2, 1, 3, iron);
            DrawRectangle(x + 2, y + 4, 6, 5, GBA_COLOR(20, 16, 8));
            DrawRectangle(x + 4, y + 5, 2, 2, BLACK);
            DrawRectangle(x + 4, y + 7, 2, 1, BLACK);
            break;
        }
        case MENU_ICON_MEDAL: {
            DrawRectangle(x + 2, y, 2, 4, C_RED);
            DrawRectangle(x + 6, y, 2, 4, C_CYAN);
            DrawRectangle(x + 2, y + 4, 6, 5, GBA_COLOR(31, 26, 2));
            DrawRectangle(x + 1, y + 5, 8, 3, GBA_COLOR(31, 26, 2));
            DrawRectangle(x + 4, y + 5, 2, 2, WHITE);
            break;
        }
        case MENU_ICON_AUDIO: {
            DrawRectangle(x + 1, y + 3, 3, 4, theme_color);
            DrawRectangle(x + 4, y + 2, 2, 6, theme_color);
            DrawRectangle(x + 6, y + 1, 1, 8, theme_color);
            DrawRectangle(x + 8, y + 3, 1, 4, theme_color);
            DrawRectangle(x + 9, y + 2, 1, 6, theme_color);
            break;
        }
        case MENU_ICON_VIDEO: {
            DrawRectangle(x + 1, y + 1, 8, 6, theme_color);
            DrawRectangle(x + 2, y + 2, 6, 4, GBA_COLOR(2, 6, 12));
            DrawRectangle(x + 3, y + 3, 4, 1, C_CYAN);
            DrawRectangle(x + 4, y + 7, 2, 2, theme_color);
            DrawRectangle(x + 2, y + 9, 6, 1, theme_color);
            break;
        }
        case MENU_ICON_CONTROLS: {
            DrawRectangle(x + 1, y + 3, 8, 5, theme_color);
            DrawRectangle(x + 2, y + 4, 2, 2, GBA_COLOR(2, 6, 12));
            DrawRectangle(x + 6, y + 4, 1, 1, C_YELLOW);
            DrawRectangle(x + 7, y + 5, 1, 1, C_RED);
            break;
        }
        case MENU_ICON_EXIT: {
            DrawRectangle(x + 1, y + 1, 5, 8, theme_color);
            DrawRectangle(x + 2, y + 2, 3, 6, GBA_COLOR(2, 6, 12));
            DrawRectangle(x + 5, y + 4, 4, 2, C_RED);
            DrawRectangle(x + 7, y + 3, 1, 4, C_RED);
            DrawRectangle(x + 8, y + 4, 1, 2, C_RED);
            break;
        }
        default:
            break;
    }
}

void DrawSciFiButtonPC(int x, int y, int w, int h, const char* text, int icon_type, bool is_selected, Color theme_color, int anim_frame) {
    if (!is_selected) {
        Color bg_col = GBA_COLOR(1, 3, 7);
        Color border_col = GBA_COLOR(5, 10, 16);
        DrawBevelledBoxPC(x, y, w, h, bg_col, border_col, true);
        
        DrawRectangle(x + 2, y + 2, 1, 1, GBA_COLOR(8, 14, 20));
        DrawRectangle(x + w - 3, y + 2, 1, 1, GBA_COLOR(8, 14, 20));
        DrawRectangle(x + 2, y + h - 3, 1, 1, GBA_COLOR(8, 14, 20));
        DrawRectangle(x + w - 3, y + h - 3, 1, 1, GBA_COLOR(8, 14, 20));

        if (icon_type != MENU_ICON_NONE) {
            DrawMenuIconPC(icon_type, x + 6, y + (h - 10) / 2, GBA_COLOR(12, 16, 20));
        }

        int text_y = y + (h - 7) / 2;
        DrawCenteredStringCustom(text, text_y, GBA_COLOR(16, 20, 24), 1);
    } else {
        float pulse = (sinf((float)anim_frame * 0.14f) + 1.0f) * 0.5f;
        Color pulse_border = ColorLerp(theme_color, WHITE, pulse * 0.4f);
        Color pulse_bg = ColorLerp(GBA_COLOR(2, 6, 14), GBA_COLOR(4, 12, 22), pulse * 0.35f);

        DrawBevelledBoxPC(x, y, w, h, pulse_bg, pulse_border, true);

        for (int ly = y + 2; ly < y + h - 2; ly += 2) {
            DrawRectangle(x + 2, ly, w - 4, 1, ColorAlpha(theme_color, 0.12f));
        }

        DrawRectangle(x + 2, y + 2, 1, 1, C_CYAN);
        DrawRectangle(x + w - 3, y + 2, 1, 1, C_CYAN);
        DrawRectangle(x + 2, y + h - 3, 1, 1, C_CYAN);
        DrawRectangle(x + w - 3, y + h - 3, 1, 1, C_CYAN);

        int bounce = (int)(sinf((float)anim_frame * 0.20f) * 2.0f);
        int ch_y = y + (h - 7) / 2;
        
        DrawCharCustom('>', x + 4 + bounce, ch_y, theme_color, 1);
        DrawCharCustom('>', x + 9 + bounce, ch_y, pulse_border, 1);

        DrawCharCustom('<', x + w - 14 - bounce, ch_y, pulse_border, 1);
        DrawCharCustom('<', x + w - 9 - bounce, ch_y, theme_color, 1);

        if (icon_type != MENU_ICON_NONE) {
            DrawMenuIconPC(icon_type, x + 18, y + (h - 10) / 2, theme_color);
        }

        int text_y = y + (h - 7) / 2;
        DrawCenteredStringCustom(text, text_y + 1, GBA_COLOR(1, 4, 10), 1);
        DrawCenteredStringCustom(text, text_y, WHITE, 1);
    }
}

void DrawNebulaBackgroundPC(int screen_w, int screen_h, int frame_count) {
    float t = (float)frame_count * 0.01f;
    int cx1 = (int)(sinf(t * 0.5f) * 30.0f) + screen_w / 3;
    int cy1 = (int)(cosf(t * 0.4f) * 20.0f) + screen_h / 2;
    int cx2 = (int)(cosf(t * 0.6f) * 40.0f) + (screen_w * 2) / 3;
    int cy2 = (int)(sinf(t * 0.5f) * 25.0f) + screen_h / 3;

    Color c_nebula1 = (Color){ 35, 12, 55, 30 };
    Color c_nebula2 = (Color){ 10, 35, 55, 26 };

    DrawCircleGradient(cx1, cy1, 90, c_nebula1, (Color){ 0, 0, 0, 0 });
    DrawCircleGradient(cx2, cy2, 100, c_nebula2, (Color){ 0, 0, 0, 0 });
}

void DrawAchievementBadgePC(int x, int y, bool unlocked, int tier) {
    int card_w = 16;
    int card_h = 16;
    if (!unlocked) {
        DrawBevelledBoxPC(x, y, card_w, card_h, GBA_COLOR(1, 3, 7), GBA_COLOR(6, 10, 16), true);
        DrawMenuIconPC(MENU_ICON_PADLOCK, x + 3, y + 3, GBA_COLOR(10, 14, 18));
    } else {
        Color tier_col = GBA_COLOR(24, 14, 4);
        if (tier == 1) tier_col = GBA_COLOR(22, 24, 26);
        else if (tier == 2) tier_col = GBA_COLOR(31, 26, 2);
        else if (tier >= 3) tier_col = C_CYAN;

        DrawBevelledBoxPC(x, y, card_w, card_h, GBA_COLOR(2, 6, 14), tier_col, true);
        DrawMenuIconPC(MENU_ICON_MEDAL, x + 3, y + 3, tier_col);
        DrawRectangle(x + 2, y + 2, 1, 1, WHITE);
    }
}

void DrawClassicCardArtworkPC(int x, int y, int w, int h, int frame) {
    int cx = x + w / 2;

    // Mini estrellas tácticas de fondo
    DrawRectangle(x + 10, y + 10, 1, 1, (Color){ 160, 240, 200, 160 });
    DrawRectangle(x + 76, y + 14, 1, 1, (Color){ 160, 240, 200, 140 });
    DrawRectangle(x + 12, y + 48, 1, 1, (Color){ 160, 240, 200, 120 });
    DrawRectangle(x + 76, y + 46, 1, 1, (Color){ 160, 240, 200, 110 });

    // Movimiento orgánico de vaivén de los flancos enemigos
    int sway_x = (int)roundf(sinf(frame * 0.045f) * 3.0f);
    int bob0 = (int)roundf(sinf(frame * 0.08f) * 2.0f);
    int bob1 = (int)roundf(sinf(frame * 0.08f + 3.14f) * 2.0f);

    // 1. Enemigos de los flancos (vigilando el combate)
    // Enemigo normal (verde, tipo 0) en el flanco izquierdo
    Enemy e0 = { .x = cx - 30 + sway_x, .y = y + 7 + bob0, .type = 0, .active = 1, .hp = 1 };
    DrawEnemyScaledPC(&e0, frame, 1);

    // Enemigo rápido (rojo/amarillo, tipo 1) en el flanco derecho
    Enemy e1 = { .x = cx + 20 + sway_x, .y = y + 7 + bob1, .type = 1, .active = 1, .hp = 1 };
    DrawEnemyScaledPC(&e1, frame, 1);

    // 2. Bucle cinemático del francotirador central (Ciclo continuo de 130 frames ~ 2.1 seg)
    int cycle = 130;
    int t = frame % cycle;

    // Posición y estado del francotirador
    int sniper_x = cx - 5;
    int sniper_y = y + 8;
    bool sniper_alive = false;
    bool sniper_flash = false;

    if (t < 52) {
        // Vivo y en combate
        sniper_alive = true;
        sniper_y = y + 8 + (int)roundf(sinf(frame * 0.08f) * 1.5f);
        if (t >= 44 && t < 52) {
            sniper_flash = (t % 2 == 0); // Parpadeo de impacto blanco
        }
    } else if (t >= 105) {
        // Reaparición descendiendo desde arriba de la pantalla
        sniper_alive = true;
        float enter_prog = (float)(t - 105) / 25.0f;
        int target_top = (h <= 48) ? (y + 4) : (y + 8);
        sniper_y = (int)roundf((y - 12) + (target_top - (y - 12)) * enter_prog);
    }

    if (sniper_alive) {
        if (h <= 48) sniper_y = y + 4 + (int)roundf(sinf(frame * 0.08f) * 1.0f);
        Enemy e4 = { .x = sniper_x, .y = sniper_y, .type = 4, .active = 1, .hp = 1, .hit_flash_timer = sniper_flash ? 1 : 0 };
        DrawEnemyScaledPC(&e4, frame, 1);
    }

    // 3. Explosión auténtica del juego y puntuación flotante (+100) al morir
    if (t >= 50 && t < 86) {
        int ex_t = t - 50;
        Color spark_color = (ex_t % 4 < 2) ? GBA_COLOR(31, 31, 0) : GBA_COLOR(31, 15, 0);
        float dist = (float)ex_t * 0.45f;
        int explo_y = (h <= 48) ? (y + 4) : (y + 8);

        // Chispas de la explosión expandiéndose en cruz/diagonal
        DrawRectangle((int)(cx - 3 - dist), (int)(explo_y - 1 - dist), 2, 2, spark_color);
        DrawRectangle((int)(cx + 3 + dist), (int)(explo_y - 1 - dist), 2, 2, spark_color);
        DrawRectangle((int)(cx - 3 - dist), (int)(explo_y + 5 + dist), 2, 2, spark_color);
        DrawRectangle((int)(cx + 3 + dist), (int)(explo_y + 5 + dist), 2, 2, spark_color);

        // Destello central en el momento del impacto
        if (ex_t < 6) {
            DrawRectangle(cx - 3, explo_y, 6, 6, WHITE);
            DrawRectangle(cx - 1, explo_y - 2, 2, 10, C_YELLOW);
            DrawRectangle(cx - 5, explo_y + 2, 10, 2, C_YELLOW);
        }

        // Texto de puntuación flotante "+100" en amarillo idéntico al gameplay
        if (ex_t >= 4 && ex_t < 34) {
            int fty = explo_y + 1 - (ex_t - 4) / 3;
            DrawStringCustom("+100", cx - 8, fty, C_YELLOW, 1);
        }
    }

    // 4. Nave del jugador con leve flotación
    int ship_float_y = (int)roundf(sinf(frame * 0.09f) * 1.2f);
    int ship_scale = (h <= 48) ? 1 : 2;
    int px = (h <= 48) ? (cx - 5) : (cx - 10);
    int py = (h <= 40) ? (y + 22 + ship_float_y) : ((h <= 48) ? (y + 25 + ship_float_y) : (y + 36 + ship_float_y));

    // 5. Balas reales del juego disparadas hacia el sniper (2x4 amarillo con punta blanca C_BULLET)
    int shot1_t = t - 22;
    int shot2_t = t - 32;
    int hit_target_y = (h <= 48) ? (y + 11) : (y + 15);

    // Primer disparo (viaja entre t=22 y t=45)
    if (shot1_t >= 0 && shot1_t < 23) {
        int by = py - 4 - (int)(shot1_t * 1.35f);
        if (by >= hit_target_y) {
            DrawRectangle(cx - 1, by, 2, 4, C_BULLET);
            DrawRectangle(cx - 1, by, 2, 1, WHITE);
        }
    }

    // Segundo disparo (viaja entre t=32 y t=52)
    if (shot2_t >= 0 && shot2_t < 20) {
        int by = py - 4 - (int)(shot2_t * 1.35f);
        if (by >= hit_target_y) {
            DrawRectangle(cx - 1, by, 2, 4, C_BULLET);
            DrawRectangle(cx - 1, by, 2, 1, WHITE);
        }
    }

    // Destello en el cañón central al disparar
    if ((shot1_t >= 0 && shot1_t <= 2) || (shot2_t >= 0 && shot2_t <= 2)) {
        DrawRectangle(cx - 1, py - 1, 2, 2, WHITE);
    }

    // 6. Nave del jugador auténtica
    DrawPlayerShipScaledPC(px, py, 0, -1, C_PLAYER, ship_scale);

    // Fuego animado de los propulsores del jugador
    if (ship_scale == 2) {
        int flm_h = 3 + (frame % 3) * 2;
        DrawRectangle(px + 6, py + 20, 3, flm_h, GBA_COLOR(31, 14, 0));
        DrawRectangle(px + 7, py + 20, 1, flm_h - 1, C_YELLOW);
        DrawRectangle(px + 11, py + 20, 3, flm_h, GBA_COLOR(31, 14, 0));
        DrawRectangle(px + 12, py + 20, 1, flm_h - 1, C_YELLOW);
    } else {
        int flm_h = 2 + (frame % 2);
        DrawRectangle(px + 3, py + 10, 1, flm_h, GBA_COLOR(31, 14, 0));
        DrawRectangle(px + 6, py + 10, 1, flm_h, GBA_COLOR(31, 14, 0));
    }
}

void DrawRushCardArtworkPC(int x, int y, int w, int h, int frame) {
    int cx = x + w / 2;

    // Estelas cinéticas de peligro / velocidad en rojo
    for (int i = 0; i < 5; i++) {
        int lx = x + 10 + i * 15;
        int ly = y + 5 + ((frame * 3 + i * 13) % (h - 14));
        DrawRectangle(lx, ly, 1, 4, (Color){ 255, 60, 40, 70 });
    }

    // Flancos de escolta del Boss: Kamikaze (tipo 7) a la izquierda y Tanque (tipo 2) a la derecha
    int k_x = cx - 30;
    int k_y = y + 7 + (int)roundf(sinf(frame * 0.08f) * 1.5f);
    Enemy e_k = { .x = k_x, .y = k_y, .type = 7, .active = 1, .hp = 1 };
    DrawEnemyScaledPC(&e_k, frame, 1);

    int t_x = cx + 20;
    int t_y = y + 7 + (int)roundf(cosf(frame * 0.08f) * 1.5f);
    Enemy e_t = { .x = t_x, .y = t_y, .type = 2, .active = 1, .hp = 1 };
    DrawEnemyScaledPC(&e_t, frame, 1);

    // 1. JEFE REAL DEL JUEGO (tipo 3 = Boss) a escala 1x adaptado al tamaño de la tarjeta
    int boss_bob = (int)roundf(sinf(frame * 0.05f) * 1.0f);
    int t = frame % 100;
    bool boss_hit_flash = (t >= 28 && t <= 34) || (t >= 68 && t <= 74);
    int boss_top = y + 4 + boss_bob;
    Enemy boss = { .x = cx - 5, .y = boss_top, .type = 3, .active = 1, .hp = 50, .hit_flash_timer = boss_hit_flash ? 4 : 0 };
    DrawEnemyScaledPC(&boss, frame, 1);

    // 2. Mini nave del jugador (escala 1x: 10x10) esquivando y disparando
    int dodge_x = (int)roundf(sinf(frame * 0.08f) * 5.0f);
    int px = cx - 5 + dodge_x;
    int py = y + 22;
    DrawPlayerShipScaledPC(px, py, 0, -1, C_PLAYER, 1);
    int flm_h = 2 + (frame % 2);
    DrawRectangle(px + 3, py + 10, 1, flm_h, GBA_COLOR(31, 14, 0));
    DrawRectangle(px + 6, py + 10, 1, flm_h, GBA_COLOR(31, 14, 0));

    int boss_bottom = boss_top + 10;

    // 4. Balas auténticas de la nave disparando contra el Boss
    int s1 = t - 16;
    if (s1 >= 0 && s1 < 12) {
        int bx1 = cx - 1 + (int)roundf(sinf((frame - s1) * 0.08f) * 5.0f);
        int by1 = (py - 3) - (s1 * (py - 3 - boss_bottom)) / 12;
        if (by1 >= boss_bottom) {
            DrawRectangle(bx1, by1, 2, 3, C_BULLET);
            DrawRectangle(bx1, by1, 2, 1, WHITE);
        }
    }
    if (s1 == 0 || s1 == 1) {
        DrawRectangle(px + 4, py - 1, 2, 2, WHITE);
    }
    if (t >= 28 && t <= 32) {
        int bx1 = cx - 1 + (int)roundf(sinf((frame - (t - 16)) * 0.08f) * 5.0f);
        DrawRectangle(bx1 - 1, boss_bottom - 1, 4, 2, WHITE);
        DrawRectangle(bx1, boss_bottom + 1, 2, 1, C_YELLOW);
    }

    int s2 = t - 56;
    if (s2 >= 0 && s2 < 12) {
        int bx2 = cx - 1 + (int)roundf(sinf((frame - s2) * 0.08f) * 5.0f);
        int by2 = (py - 3) - (s2 * (py - 3 - boss_bottom)) / 12;
        if (by2 >= boss_bottom) {
            DrawRectangle(bx2, by2, 2, 3, C_BULLET);
            DrawRectangle(bx2, by2, 2, 1, WHITE);
        }
    }
    if (s2 == 0 || s2 == 1) {
        DrawRectangle(px + 4, py - 1, 2, 2, WHITE);
    }
    if (t >= 68 && t <= 72) {
        int bx2 = cx - 1 + (int)roundf(sinf((frame - (t - 56)) * 0.08f) * 5.0f);
        DrawRectangle(bx2 - 1, boss_bottom - 1, 4, 2, WHITE);
        DrawRectangle(bx2, boss_bottom + 1, 2, 1, C_YELLOW);
    }
}

void DrawTimeAttackCardArtworkPC(int x, int y, int w, int h, int frame) {
    // Fondo: estelas cian de alta velocidad horizontal
    for (int i = 0; i < 4; i++) {
        int lx = x + 6 + ((frame * 3 + i * 22) % (w - 14));
        int ly = y + 4 + i * 8;
        DrawRectangle(lx, ly, 5, 1, (Color){ 40, 180, 255, 65 });
    }

    // Cronómetro analógico pixel-art a la derecha
    int clock_cx = x + w - 16;
    int clock_cy = y + h / 2;
    int pulse = (int)roundf(sinf(frame * 0.1f) * 1.5f);
    
    // Anillo exterior del reloj
    DrawCircleLines(clock_cx, clock_cy, 9 + pulse, GBA_COLOR(0, 24, 31));
    DrawCircle(clock_cx, clock_cy, 8, GBA_COLOR(0, 4, 8));
    DrawCircleLines(clock_cx, clock_cy, 8, C_CYAN);
    
    // Botón superior del cronómetro
    DrawRectangle(clock_cx - 1, clock_cy - 11, 3, 2, C_YELLOW);
    // Marcas de horas (12, 3, 6, 9)
    DrawRectangle(clock_cx, clock_cy - 6, 1, 2, WHITE);
    DrawRectangle(clock_cx + 5, clock_cy, 2, 1, WHITE);
    DrawRectangle(clock_cx, clock_cy + 5, 1, 2, WHITE);
    DrawRectangle(clock_cx - 6, clock_cy, 2, 1, WHITE);

    // Aguja del segundero giratoria
    float ang = frame * 0.12f;
    int hx = clock_cx + (int)roundf(cosf(ang) * 5.0f);
    int hy = clock_cy + (int)roundf(sinf(ang) * 5.0f);
    DrawLine(clock_cx, clock_cy, hx, hy, C_YELLOW);
    DrawRectangle(clock_cx, clock_cy, 1, 1, WHITE);

    // Ciclo de movimiento vertical y disparos horizontales (70 frames)
    int t = frame % 70;

    // Posición Y de la nave: se desplaza entre e1 (arriba) y e2 (abajo)
    int py = y + 5;
    if (t >= 25 && t < 35) {
        float prog = (float)(t - 25) / 10.0f;
        float smooth = 0.5f - 0.5f * cosf(prog * 3.14159f);
        py = y + 5 + (int)roundf(16.0f * smooth);
    } else if (t >= 35 && t < 60) {
        py = y + 21;
    } else if (t >= 60) {
        float prog = (float)(t - 60) / 10.0f;
        float smooth = 0.5f - 0.5f * cosf(prog * 3.14159f);
        py = y + 21 - (int)roundf(16.0f * smooth);
    }

    // Nave del jugador mirando a la DERECHA (dx = 1, dy = 0)
    int px = x + 7;
    DrawPlayerShipScaledPC(px, py, 1, 0, C_PLAYER, 1);

    // Dos enemigos fijos en formación
    // Enemigo 1: Drone veloz (tipo 1, cian) arriba
    int e1_x = x + 44;
    int e1_y = y + 5;
    int shot1_t = t - 3;
    bool hit1 = (shot1_t >= 11 && shot1_t <= 16);
    Enemy e1 = { .x = e1_x, .y = e1_y, .type = 1, .active = 1, .hp = 1, .hit_flash_timer = hit1 ? 2 : 0 };
    DrawEnemyScaledPC(&e1, frame, 1);

    // Enemigo 2: Explorador normal (tipo 0, verde) abajo
    int e2_x = x + 44;
    int e2_y = y + 21;
    int shot2_t = t - 38;
    bool hit2 = (shot2_t >= 11 && shot2_t <= 16);
    Enemy e2 = { .x = e2_x, .y = e2_y, .type = 0, .active = 1, .hp = 1, .hit_flash_timer = hit2 ? 2 : 0 };
    DrawEnemyScaledPC(&e2, frame, 1);

    // Disparo 1 (completamente horizontal hacia Enemigo 1)
    if (shot1_t >= 0 && shot1_t < 13) {
        int bx1 = px + 10 + shot1_t * 3;
        if (bx1 < e1_x + 5) {
            DrawRectangle(bx1, e1_y + 4, 3, 2, C_BULLET);
            DrawRectangle(bx1 + 1, e1_y + 4, 2, 1, WHITE);
        }
    }
    if (shot1_t >= 0 && shot1_t <= 2) {
        DrawRectangle(px + 10, e1_y + 4, 2, 2, WHITE);
    }
    if (hit1) {
        // Chispas limpias en el punto frontal de impacto
        DrawRectangle(e1_x - 1, e1_y + 4, 2, 2, WHITE);
        DrawRectangle(e1_x - 2, e1_y + 2, 1, 1, C_CYAN);
        DrawRectangle(e1_x - 2, e1_y + 6, 1, 1, C_CYAN);
    }

    // Disparo 2 (completamente horizontal hacia Enemigo 2)
    if (shot2_t >= 0 && shot2_t < 13) {
        int bx2 = px + 10 + shot2_t * 3;
        if (bx2 < e2_x + 5) {
            DrawRectangle(bx2, e2_y + 4, 3, 2, C_BULLET);
            DrawRectangle(bx2 + 1, e2_y + 4, 2, 1, WHITE);
        }
    }
    if (shot2_t >= 0 && shot2_t <= 2) {
        DrawRectangle(px + 10, e2_y + 4, 2, 2, WHITE);
    }
    if (hit2) {
        // Chispas limpias en el punto frontal de impacto (sin barras sobre el sprite)
        DrawRectangle(e2_x - 1, e2_y + 4, 2, 2, WHITE);
        DrawRectangle(e2_x - 2, e2_y + 2, 1, 1, C_YELLOW);
        DrawRectangle(e2_x - 2, e2_y + 6, 1, 1, C_YELLOW);
    }
}

void DrawAssaultCardArtworkPC(int x, int y, int w, int h, int frame) {
    // 1. Fondo táctico de asalto militar espacial (estelas y rejilla sutil ámbar/naranja)
    for (int i = 0; i < 4; i++) {
        int lx = x + 6 + ((frame * 2 + i * 24) % (w - 14));
        int ly = y + 5 + i * 8;
        DrawRectangle(lx, ly, 4, 1, (Color){ 255, 140, 40, 55 });
    }

    // 2. Colores y armas de la nave (ciclo de armas cada 45 frames ~ 0.75s)
    int pu_cycle = (frame / 45) % 6;
    Color ship_colors[6] = {
        C_PLAYER,            // 0: Rojo clásico
        C_RAPID,             // 1: Amarillo disparo rápido
        C_GREEN,             // 2: Verde triple disparo
        C_ICE,               // 3: Azul hielo
        GBA_COLOR(31, 15, 0),// 4: Naranja fuego pesado
        C_NAVY_BLUE          // 5: Azul marino agua
    };
    Color cur_ship_col = ship_colors[pu_cycle];

    // 3. Posición de la nave: a la DERECHA, mirando a la IZQUIERDA (dx = -1, dy = 0)
    int px = x + w - 20;
    int py = y + (h / 2) - 5;
    DrawPlayerShipScaledPC(px, py, -1, 0, cur_ship_col, 1);

    // 4. Formación enemiga a la IZQUIERDA (sprites oficiales inéditos en miniaturas)
    // Enemigo Central: Tipo 5 (Divisor, púrpura tecnológico)
    int e_center_x = x + 10;
    int e_center_y = y + (h / 2) - 5;
    int bob = (int)roundf(sinf(frame * 0.08f) * 1.0f);

    // Flancos de escolta: Tipo 9 (Teletransportadores) arriba y abajo (desplazados a la derecha)
    int sway_t = (int)roundf(sinf(frame * 0.07f) * 1.5f);
    int sway_b = (int)roundf(cosf(frame * 0.07f) * 1.5f);
    Enemy e_top = { .x = x + 28 + sway_t, .y = y + 3, .type = 9, .active = 1, .hp = 1 };
    DrawEnemyScaledPC(&e_top, frame, 1);

    Enemy e_bot = { .x = x + 28 + sway_b, .y = y + h - 13, .type = 9, .active = 1, .hp = 1 };
    DrawEnemyScaledPC(&e_bot, frame, 1);

    // 5. Bucle de disparo continuo hacia el enemigo central
    int shot_cycle = 22;
    int st = frame % shot_cycle;
    bool hit_flash = (st >= 10 && st <= 14);

    // Dibujar enemigo central (nunca muere, recibe los impactos con hit flash)
    Enemy e_center = { .x = e_center_x, .y = e_center_y + bob, .type = 5, .active = 1, .hp = 50, .hit_flash_timer = hit_flash ? 2 : 0 };
    DrawEnemyScaledPC(&e_center, frame, 1);

    // 6. Proyectiles viajando de derecha a izquierda
    if (st >= 0 && st < 12) {
        int bx = px - 2 - st * 4;
        int target_x = e_center_x + 9;
        if (bx >= target_x) {
            int by = py + 4;
            if (pu_cycle == 0) { // Normal
                DrawRectangle(bx - 3, by, 3, 2, C_BULLET);
                DrawRectangle(bx - 3, by, 1, 2, WHITE);
            } else if (pu_cycle == 1) { // Rápido
                DrawRectangle(bx - 3, by, 3, 2, C_RAPID);
                DrawRectangle(bx - 3, by, 1, 2, WHITE);
            } else if (pu_cycle == 2) { // Triple
                DrawRectangle(bx - 3, by - 2, 3, 1, C_GREEN);
                DrawRectangle(bx - 4, by, 4, 2, C_GREEN);
                DrawRectangle(bx - 3, by + 3, 3, 1, C_GREEN);
            } else if (pu_cycle == 3) { // Hielo
                DrawRectangle(bx - 3, by, 3, 2, C_ICE);
                DrawRectangle(bx - 2, by - 1, 1, 4, WHITE);
            } else if (pu_cycle == 4) { // Pesado
                DrawRectangle(bx - 4, by - 1, 4, 4, GBA_COLOR(31, 15, 0));
                DrawRectangle(bx - 3, by, 2, 2, C_YELLOW);
            } else { // Agua
                DrawRectangle(bx - 3, by, 3, 2, C_NAVY_BLUE);
                DrawRectangle(bx - 3, by, 1, 1, C_SHIELD);
            }
        }
    }

    // Fogonazo en la punta del cañón al disparar
    if (st <= 2) {
        DrawRectangle(px - 3, py + 4, 2, 2, WHITE);
    }

    // Chispas de impacto sobre el enemigo central
    if (hit_flash) {
        DrawRectangle(e_center_x + 10, e_center_y + 4 + bob, 2, 2, WHITE);
        DrawRectangle(e_center_x + 11, e_center_y + 2 + bob, 1, 1, cur_ship_col);
        DrawRectangle(e_center_x + 11, e_center_y + 6 + bob, 1, 1, cur_ship_col);
    }

    // Brackets sci-fi en esquinas de la tarjeta
    Color bracket_col = (frame % 30 < 15) ? GBA_COLOR(31, 18, 0) : GBA_COLOR(24, 10, 0);
    DrawRectangle(x + 4, y + 4, 4, 1, bracket_col);
    DrawRectangle(x + 4, y + 4, 1, 4, bracket_col);
    DrawRectangle(x + w - 5, y + 4, 4, 1, bracket_col);
    DrawRectangle(x + w - 2, y + 4, 1, 4, bracket_col);
    DrawRectangle(x + 4, y + h - 5, 4, 1, bracket_col);
    DrawRectangle(x + 4, y + h - 5, 1, 4, bracket_col);
    DrawRectangle(x + w - 5, y + h - 5, 4, 1, bracket_col);
    DrawRectangle(x + w - 2, y + h - 5, 1, 4, bracket_col);
}

void DrawComingSoonCardArtworkPC(int x, int y, int w, int h, int frame) {
    int cx = x + w / 2;
    int cy = y + h / 2;

    // Fondo con nebulosa púrpura oscura y estrellas titilantes
    for (int i = 0; i < 5; i++) {
        int sx = x + 12 + i * 16;
        int sy = y + 8 + ((i * 17) % (h - 16));
        int alpha = 100 + (int)roundf(sinf(frame * 0.05f + i) * 60.0f);
        DrawRectangle(sx, sy, 1, 1, (Color){ 180, 120, 255, (unsigned char)alpha });
    }

    // Candado arcade / silueta misteriosa en el centro
    int bob = (int)roundf(sinf(frame * 0.06f) * 1.5f);
    int lock_y = cy - 6 + bob;

    // Arco metálico del candado (shackle)
    DrawRectangle(cx - 5, lock_y - 5, 2, 5, GBA_COLOR(20, 20, 24));
    DrawRectangle(cx + 4, lock_y - 5, 2, 5, GBA_COLOR(20, 20, 24));
    DrawRectangle(cx - 4, lock_y - 7, 9, 2, GBA_COLOR(24, 24, 28));

    // Cuerpo del candado (dorado/bronce metálico con bisel)
    Color lock_bg = GBA_COLOR(22, 16, 4);
    Color lock_border = GBA_COLOR(31, 26, 8);
    DrawBevelledBoxPC(cx - 8, lock_y, 17, 13, lock_bg, lock_border, true);

    // Ojo de cerradura
    DrawRectangle(cx, lock_y + 3, 2, 3, GBA_COLOR(4, 2, 0));
    DrawRectangle(cx, lock_y + 6, 2, 4, GBA_COLOR(4, 2, 0));

    // Brackets sci-fi en esquinas de la tarjeta
    Color bracket_col = (frame % 30 < 15) ? GBA_COLOR(14, 8, 24) : GBA_COLOR(20, 12, 31);
    DrawRectangle(x + 4, y + 4, 4, 1, bracket_col);
    DrawRectangle(x + 4, y + 4, 1, 4, bracket_col);
    DrawRectangle(x + w - 5, y + 4, 4, 1, bracket_col);
    DrawRectangle(x + w - 2, y + 4, 1, 4, bracket_col);
    DrawRectangle(x + 4, y + h - 5, 4, 1, bracket_col);
    DrawRectangle(x + 4, y + h - 5, 1, 4, bracket_col);
    DrawRectangle(x + w - 5, y + h - 5, 4, 1, bracket_col);
    DrawRectangle(x + w - 2, y + h - 5, 1, 4, bracket_col);
}

void DrawRecordArrowPC(int x, int y, bool points_right, Color color) {
    for (int i = 0; i < 7; i++) {
        int col = points_right ? (x + i) : (x + 6 - i);
        int row_start = y + i;
        int h = 13 - 2 * i;
        DrawRectangle(col, row_start, 1, h, color);
    }
}

// Sleek 4x5 / 5x5 proportional typography for footer
typedef struct {
    char c;
    int w;
    unsigned char rows[5];
} SleekGlyph;

static const SleekGlyph sleek_glyphs[] = {
    { '0', 4, { 0x6, 0x9, 0x9, 0x9, 0x6 } },
    { '1', 3, { 0x2, 0x6, 0x2, 0x2, 0x7 } },
    { '2', 4, { 0x6, 0x9, 0x2, 0x4, 0xF } },
    { '3', 4, { 0xE, 0x1, 0x6, 0x1, 0xE } },
    { '4', 4, { 0x9, 0x9, 0xF, 0x1, 0x1 } },
    { '5', 4, { 0xF, 0x8, 0xE, 0x1, 0xE } },
    { '6', 4, { 0x6, 0x8, 0xE, 0x9, 0x6 } },
    { '7', 4, { 0xF, 0x1, 0x2, 0x4, 0x4 } },
    { '8', 4, { 0x6, 0x9, 0x6, 0x9, 0x6 } },
    { '9', 4, { 0x6, 0x9, 0x7, 0x1, 0x6 } },
    { 'A', 4, { 0x6, 0x9, 0xF, 0x9, 0x9 } },
    { 'B', 4, { 0xE, 0x9, 0xE, 0x9, 0xE } },
    { 'C', 4, { 0x7, 0x8, 0x8, 0x8, 0x7 } },
    { 'D', 4, { 0xE, 0x9, 0x9, 0x9, 0xE } },
    { 'E', 4, { 0xF, 0x8, 0xE, 0x8, 0xF } },
    { 'F', 4, { 0xF, 0x8, 0xE, 0x8, 0x8 } },
    { 'G', 4, { 0x7, 0x8, 0xB, 0x9, 0x7 } },
    { 'H', 4, { 0x9, 0x9, 0xF, 0x9, 0x9 } },
    { 'I', 3, { 0x7, 0x2, 0x2, 0x2, 0x7 } },
    { 'J', 4, { 0x1, 0x1, 0x1, 0x9, 0x6 } },
    { 'K', 4, { 0x9, 0xA, 0xC, 0xA, 0x9 } },
    { 'L', 3, { 0x4, 0x4, 0x4, 0x4, 0x7 } },
    { 'M', 5, { 0x11, 0x1B, 0x15, 0x11, 0x11 } },
    { 'N', 4, { 0x9, 0xD, 0xB, 0x9, 0x9 } },
    { 'O', 4, { 0x6, 0x9, 0x9, 0x9, 0x6 } },
    { 'P', 4, { 0xE, 0x9, 0xE, 0x8, 0x8 } },
    { 'Q', 4, { 0x6, 0x9, 0x9, 0xA, 0x5 } },
    { 'R', 4, { 0xE, 0x9, 0xE, 0xA, 0x9 } },
    { 'S', 4, { 0x7, 0x8, 0x6, 0x1, 0xE } },
    { 'T', 5, { 0x1F, 0x4, 0x4, 0x4, 0x4 } },
    { 'U', 4, { 0x9, 0x9, 0x9, 0x9, 0x6 } },
    { 'V', 5, { 0x11, 0x11, 0x0A, 0x0A, 0x04 } },
    { 'W', 5, { 0x11, 0x11, 0x15, 0x1B, 0x11 } },
    { 'X', 4, { 0x9, 0x6, 0x6, 0x9, 0x9 } },
    { 'Y', 3, { 0x5, 0x5, 0x2, 0x2, 0x2 } },
    { 'Z', 4, { 0xF, 0x2, 0x4, 0x8, 0xF } },
    { '.', 2, { 0x0, 0x0, 0x0, 0x0, 0x3 } },
    { '-', 3, { 0x0, 0x0, 0x7, 0x0, 0x0 } },
    { '/', 3, { 0x1, 0x2, 0x2, 0x4, 0x4 } },
    { ':', 2, { 0x0, 0x3, 0x0, 0x3, 0x0 } },
    { ' ', 3, { 0x0, 0x0, 0x0, 0x0, 0x0 } },
    { 0, 0, {0} }
};

static void DrawSleekCharPC(char c, int x, int y, Color color) {
    if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
    const SleekGlyph *g = NULL;
    for (int i = 0; sleek_glyphs[i].c != 0; i++) {
        if (sleek_glyphs[i].c == c) { g = &sleek_glyphs[i]; break; }
    }
    if (!g) return;
    for (int r = 0; r < 5; r++) {
        unsigned char row_bits = g->rows[r];
        for (int col = 0; col < g->w; col++) {
            if ((row_bits >> (g->w - 1 - col)) & 1) {
                DrawRectangle(x + col, y + r, 1, 1, color);
            }
        }
    }
}

static void DrawSleekStringPC(const char* str, int x, int y, Color color) {
    int cur_x = x;
    for (int i = 0; str[i] != '\0'; i++) {
        char c = str[i];
        if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
        const SleekGlyph *g = NULL;
        for (int j = 0; sleek_glyphs[j].c != 0; j++) {
            if (sleek_glyphs[j].c == c) { g = &sleek_glyphs[j]; break; }
        }
        if (g) {
            DrawSleekCharPC(c, cur_x, y, color);
            cur_x += g->w + 1;
        } else {
            cur_x += 4;
        }
    }
}

static void DrawCopyrightIconPC(int x, int y, Color color) {
    DrawRectangle(x + 1, y, 3, 1, color);
    DrawRectangle(x, y + 1, 1, 3, color);
    DrawRectangle(x + 4, y + 1, 1, 3, color);
    DrawRectangle(x + 1, y + 4, 3, 1, color);
    DrawRectangle(x + 2, y + 1, 2, 1, color);
    DrawRectangle(x + 2, y + 2, 1, 1, color);
    DrawRectangle(x + 2, y + 3, 2, 1, color);
}

void DrawTitleFooterPC(int frame_count) {
    int py = SCREEN_H - 13;
    
    // Left Badge: (C) 2026 ALEJADEV
    int left_w = 88;
    int left_x = 7;
    DrawBevelledBoxPC(left_x, py - 2, left_w, 11, GBA_COLOR(1, 3, 6), GBA_COLOR(3, 8, 14), true);
    DrawRectangle(left_x + 1, py - 1, 1, 1, C_CYAN);
    DrawRectangle(left_x + left_w - 2, py - 1, 1, 1, C_CYAN);
    
    DrawCopyrightIconPC(left_x + 4, py + 1, GBA_COLOR(14, 20, 26));
    DrawSleekStringPC("2026", left_x + 12, py + 1, GBA_COLOR(16, 20, 24));
    DrawRectangle(left_x + 36, py + 3, 1, 1, C_CYAN);
    DrawSleekStringPC("ALEJADEV", left_x + 40, py + 1, GBA_COLOR(24, 30, 31));

    // Right Badge: * V1.0 // PC
    int right_w = 56;
    int right_x = SCREEN_W - right_w - 7;
    DrawBevelledBoxPC(right_x, py - 2, right_w, 11, GBA_COLOR(1, 3, 6), GBA_COLOR(3, 8, 14), true);
    DrawRectangle(right_x + 1, py - 1, 1, 1, C_CYAN);
    DrawRectangle(right_x + right_w - 2, py - 1, 1, 1, C_CYAN);

    // Blinking / pulsing neon green online dot
    int led_pulse = (frame_count / 30) % 2;
    Color led_col = (led_pulse == 0) ? GBA_COLOR(0, 31, 12) : GBA_COLOR(0, 22, 8);
    DrawRectangle(right_x + 4, py + 3, 2, 2, led_col);
    if (led_pulse == 0) DrawRectangle(right_x + 4, py + 3, 1, 1, WHITE);

    DrawSleekStringPC("V1.0", right_x + 9, py + 1, GBA_COLOR(22, 26, 30));
    DrawSleekStringPC("//", right_x + 29, py + 1, GBA_COLOR(8, 14, 20));
    DrawSleekStringPC("PC", right_x + 39, py + 1, C_YELLOW);
}

void DrawTitleDividerPC(int center_x, int y, int width, int frame_count) {
    int half_w = width / 2;
    int x1 = center_x - half_w;
    int x2 = center_x + half_w;

    // 1. Sombra inferior proyectada (1px sutil en navy profundo)
    DrawRectangle(x1 + 6, y + 1, width - 12, 1, GBA_COLOR(0, 4, 8));

    // 2. Extremos cónicos 16-bit retro
    // Puntas exteriores (1px suave)
    DrawRectangle(x1, y, 4, 1, GBA_COLOR(0, 10, 16));
    DrawRectangle(x2 - 4, y, 4, 1, GBA_COLOR(0, 10, 16));
    // Tramo intermedio cónico
    DrawRectangle(x1 + 4, y, 5, 1, GBA_COLOR(0, 18, 24));
    DrawRectangle(x2 - 9, y, 5, 1, GBA_COLOR(0, 18, 24));
    // Marcador vertical de finial
    DrawRectangle(x1 + 9, y - 1, 1, 3, GBA_COLOR(0, 22, 28));
    DrawRectangle(x2 - 10, y - 1, 1, 3, GBA_COLOR(0, 22, 28));

    // 3. Riel principal de energía cian (núcleo 16-bit nítido)
    int inner_x1 = x1 + 10;
    int inner_w = width - 20;
    DrawRectangle(inner_x1, y, inner_w, 1, C_CYAN);

    // 4. Bisel superior metálico (destello cian claro / blanco hielo)
    DrawRectangle(inner_x1 + 4, y - 1, inner_w - 8, 1, GBA_COLOR(14, 28, 31));

    // 5. Marcadores secundarios de sección (estilo calibración arcade)
    int notch_offset = 32;
    DrawRectangle(center_x - notch_offset, y - 1, 1, 3, GBA_COLOR(18, 30, 31));
    DrawRectangle(center_x + notch_offset, y - 1, 1, 3, GBA_COLOR(18, 30, 31));

    // 6. Nodo central: Rombo compacto 16-bit (3px de altura para máxima pulcritud)
    // Silueta exterior cian vivo
    DrawRectangle(center_x - 2, y, 5, 1, C_CYAN);
    DrawRectangle(center_x - 1, y - 1, 3, 3, C_CYAN);
    DrawRectangle(center_x, y - 2, 1, 5, C_CYAN);
    // Núcleo blanco brillante
    DrawRectangle(center_x - 1, y, 3, 1, WHITE);
    DrawRectangle(center_x, y - 1, 1, 3, WHITE);

    // 7. Destello especular dinámico que recorre la línea suavemente cada 3.5 segundos
    int gleam_cycle = frame_count % 180;
    if (gleam_cycle < 100) {
        float progress = (float)gleam_cycle / 100.0f;
        int gleam_x = inner_x1 + (int)(progress * (float)inner_w);
        if (gleam_x >= inner_x1 && gleam_x <= inner_x1 + inner_w - 4) {
            DrawRectangle(gleam_x, y - 1, 4, 1, WHITE);
            DrawRectangle(gleam_x + 1, y, 2, 1, WHITE);
        }
    }
}


