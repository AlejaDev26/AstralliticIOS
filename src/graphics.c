#include "graphics.h"
#include "types.h"
#include <stdio.h>
#include <string.h>

const unsigned char font[36][5] = {
    {7,5,7,5,5}, {6,5,6,5,6}, {7,4,4,4,7}, {6,5,5,5,6},
    {7,4,6,4,7}, {7,4,6,4,4}, {7,4,5,5,7}, {5,5,7,5,5},
    {7,2,2,2,7}, {3,1,1,5,7}, {5,5,6,5,5}, {4,4,4,4,7},
    {5,7,5,5,5}, {7,5,5,5,5}, {7,5,5,5,7}, {7,5,7,4,4},
    {7,5,5,7,3}, {7,5,7,5,5}, {7,4,7,1,7}, {7,2,2,2,2},
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
    "void main() {\n"
    "    vec2 uv = fragTexCoord;\n"
    "    if (filterType == 2) {\n"
    "        vec2 dc = uv - vec2(0.5, 0.5);\n"
    "        uv = vec2(0.5, 0.5) + dc * (1.0 + 0.16 * dot(dc, dc));\n"
    "        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {\n"
    "            finalColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "            return;\n"
    "        }\n"
    "    }\n"
    "    vec4 col = texture(texture0, uv);\n"
    "    if (filterType >= 1) {\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159) * 0.18;\n"
    "        col.rgb -= scanline;\n"
    "    }\n"
    "    if (filterType == 2) {\n"
    "        float vig = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);\n"
    "        col.rgb *= clamp(pow(16.0 * vig, 0.25), 0.0, 1.0);\n"
    "        col.rgb += col.rgb * 0.08;\n"
    "    }\n"
    "    finalColor = col * fragColor;\n"
    "}\n";
#elif defined(GRAPHICS_API_OPENGL_ES2) || defined(PLATFORM_ANDROID)
const char* crt_vertex_shader_code = 0;
const char* crt_shader_code = 
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec2 fragTexCoord;\n"
    "varying vec4 fragColor;\n"
    "uniform sampler2D texture0;\n"
    "uniform int filterType;\n"
    "void main() {\n"
    "    vec2 uv = fragTexCoord;\n"
    "    if (filterType == 2) {\n"
    "        vec2 dc = uv - vec2(0.5, 0.5);\n"
    "        uv = vec2(0.5, 0.5) + dc * (1.0 + 0.15 * dot(dc, dc));\n"
    "        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {\n"
    "            gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "            return;\n"
    "        }\n"
    "    }\n"
    "    vec4 col = texture2D(texture0, uv);\n"
    "    if (filterType >= 1) {\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159) * 0.18;\n"
    "        col.rgb -= scanline;\n"
    "    }\n"
    "    if (filterType == 2) {\n"
    "        float vig = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);\n"
    "        col.rgb *= clamp(pow(16.0 * vig, 0.25), 0.0, 1.0);\n"
    "        col.rgb += col.rgb * 0.08;\n"
    "    }\n"
    "    gl_FragColor = col * fragColor;\n"
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
    "void main() {\n"
    "    vec2 uv = fragTexCoord;\n"
    "    if (filterType == 2) {\n"
    "        vec2 dc = uv - vec2(0.5, 0.5);\n"
    "        uv = vec2(0.5, 0.5) + dc * (1.0 + 0.15 * dot(dc, dc));\n"
    "        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {\n"
    "            finalColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "            return;\n"
    "        }\n"
    "    }\n"
    "    vec4 col = texture(texture0, uv);\n"
    "    if (filterType >= 1) {\n"
    "        float scanline = sin(uv.y * 320.0 * 3.14159) * 0.12;\n"
    "        col.rgb -= scanline;\n"
    "    }\n"
    "    if (filterType == 2) {\n"
    "        float vig = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);\n"
    "        col.rgb *= clamp(pow(16.0 * vig, 0.25), 0.0, 1.0);\n"
    "        col.rgb += col.rgb * 0.08;\n"
    "    }\n"
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

void DrawCharCustom(char c, int x, int y, Color color, int scale) {
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
        DrawCharCustom(str[i], x, y, color, scale);
        x += (3 * scale) + scale;
        i++;
    }
}

int MeasureStringCustom(const char* str, int scale) {
    int len = 0; while(str[len] != '\0') len++;
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
    DrawHeaderCenteredStringCustom(title, 18, title_color, shadow_color, 2);
    DrawRectangle(25, 42, 190, 1, line_color);

    char back_msg[48];
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
    if (g_last_input_device == INPUT_GAMEPAD) {
        snprintf(back_msg, sizeof(back_msg), T(STR_PRESS_BACK), "B");
    } else {
        snprintf(back_msg, sizeof(back_msg), "%s", T(STR_TOUCH_BACK));
    }
#else
    const char* back_key_txt = (g_last_input_device == INPUT_GAMEPAD) ? "B" : GetKeyNameCustom(g_keys.key_turbo);
    snprintf(back_msg, sizeof(back_msg), T(STR_PRESS_BACK), back_key_txt);
#endif
    DrawCenteredStringCustom(back_msg, 145, GBA_COLOR(14, 18, 22), 1);
}

void DrawDeviceNotificationToast(int timer, InputDeviceType dev) {
    if (timer <= 0) return;
    int anim_offset = 0;
    if (timer > 100) anim_offset = (timer - 100) / 2;
    else if (timer < 20) anim_offset = (20 - timer) / 2;

    int box_w = 114;
    int box_h = 16;
    int box_x = SCREEN_W - box_w - 4 + anim_offset;
    int box_y = SCREEN_H - box_h - 4;

    DrawRectangle(box_x, box_y, box_w, box_h, GBA_COLOR(2, 6, 12));
    DrawRectangleLines(box_x, box_y, box_w, box_h, C_CYAN);

    if (dev == INPUT_GAMEPAD) {
        DrawGamepadIcon16Bit(box_x + 3, box_y + 3, false);
        DrawStringCustom(T(STR_GAMEPAD_CONNECTED), box_x + 18, box_y + 5, C_YELLOW, 1);
    } else {
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
        // Icono retro pixel-art de móvil táctil
        DrawRectangle(box_x + 4, box_y + 2, 8, 12, GBA_COLOR(3, 8, 16));
        DrawRectangleLines(box_x + 4, box_y + 2, 8, 12, C_CYAN);
        DrawRectangle(box_x + 7, box_y + 11, 2, 1, WHITE);
        DrawStringCustom(T(STR_TOUCH_CONNECTED), box_x + 16, box_y + 5, C_CYAN, 1);
#else
        DrawKeyboardIcon16Bit(box_x + 2, box_y + 3, false);
        DrawStringCustom(T(STR_KEYBOARD_CONNECTED), box_x + 18, box_y + 5, C_CYAN, 1);
#endif
    }
}

void DrawAstralliticLogoPC(int start_x, int y) {
    const unsigned char logo_chars[11][7] = {
        { 0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11 },
        { 0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E },
        { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 },
        { 0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11 },
        { 0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11 },
        { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F },
        { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F },
        { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F },
        { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04 },
        { 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F },
        { 0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E }
    };

    Color c_shadow_deep = GBA_COLOR(0, 4, 10);
    Color c_shadow_edge = GBA_COLOR(0, 10, 20);
    Color c_glow_top    = GBA_COLOR(24, 31, 31);
    Color c_body_cyan   = GBA_COLOR(0, 28, 31);
    Color c_body_deep   = GBA_COLOR(0, 18, 26);
    Color c_gold_accent = GBA_COLOR(31, 28, 0);

    for(int ch = 0; ch < 11; ch++) {
        int cx = start_x + (ch * 19);
        for(int r = 0; r < 7; r++) {
            for(int c = 0; c < 5; c++) {
                if((logo_chars[ch][r] >> (4 - c)) & 1) {
                    DrawRectangle(cx + (c * 3) + 3, y + (r * 4) + 4, 3, 4, c_shadow_deep);
                    DrawRectangle(cx + (c * 3) + 2, y + (r * 4) + 2, 3, 4, c_shadow_edge);
                    Color col = (r == 0) ? c_glow_top : ((r < 3) ? c_body_cyan : ((r == 3) ? c_gold_accent : c_body_deep));
                    DrawRectangle(cx + (c * 3), y + (r * 4), 3, 4, col);
                    if(r == 0 && c < 4) {
                        DrawRectangle(cx + (c * 3), y + (r * 4), 1, 1, WHITE);
                    }
                }
            }
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

void DrawPlayerShipPC(int x, int y, int dx, int dy, Color base_color) {
    Color c_cockpit = GBA_COLOR(0, 28, 31);
    Color c_glow    = WHITE;
    Color c_flame   = GBA_COLOR(31, 16, 0);
    Color c_shadow  = GBA_COLOR(8, 2, 2);

    if (dy < 0 && dx == 0) {
        DrawRectangle(x + 4, y, 2, 3, c_glow);
        DrawRectangle(x + 3, y + 2, 4, 4, base_color);
        DrawRectangle(x + 4, y + 3, 2, 2, c_cockpit);
        DrawRectangle(x + 1, y + 6, 8, 3, base_color);
        DrawRectangle(x, y + 7, 2, 3, c_shadow);
        DrawRectangle(x + 8, y + 7, 2, 3, c_shadow);
        DrawRectangle(x + 4, y + 9, 2, 1, c_flame);
    } else if (dy > 0 && dx == 0) {
        DrawRectangle(x + 4, y + 7, 2, 3, c_glow);
        DrawRectangle(x + 3, y + 4, 4, 4, base_color);
        DrawRectangle(x + 4, y + 5, 2, 2, c_cockpit);
        DrawRectangle(x + 1, y + 1, 8, 3, base_color);
        DrawRectangle(x, y, 2, 3, c_shadow);
        DrawRectangle(x + 8, y, 2, 3, c_shadow);
        DrawRectangle(x + 4, y + 2, 1, 1, c_flame);
    } else if (dx > 0 && dy == 0) {
        DrawRectangle(x + 7, y + 4, 3, 2, c_glow);
        DrawRectangle(x + 4, y + 3, 4, 4, base_color);
        DrawRectangle(x + 5, y + 4, 2, 2, c_cockpit);
        DrawRectangle(x + 1, y + 1, 3, 8, base_color);
        DrawRectangle(x, y, 3, 2, c_shadow);
        DrawRectangle(x, y + 8, 3, 2, c_shadow);
        DrawRectangle(x, y + 4, 1, 2, c_flame);
    } else if (dx < 0 && dy == 0) {
        DrawRectangle(x, y + 4, 3, 2, c_glow);
        DrawRectangle(x + 2, y + 3, 4, 4, base_color);
        DrawRectangle(x + 3, y + 4, 2, 2, c_cockpit);
        DrawRectangle(x + 6, y + 1, 3, 8, base_color);
        DrawRectangle(x + 7, y, 3, 2, c_shadow);
        DrawRectangle(x + 7, y + 8, 3, 2, c_shadow);
        DrawRectangle(x + 9, y + 4, 1, 2, c_flame);
    } else if (dx > 0 && dy < 0) {
        DrawRectangle(x + 7, y, 2, 2, c_glow);
        DrawRectangle(x + 4, y + 2, 4, 4, base_color);
        DrawRectangle(x + 5, y + 3, 2, 2, c_cockpit);
        DrawRectangle(x, y + 6, 6, 4, base_color);
        DrawRectangle(x, y + 8, 2, 2, c_flame);
    } else if (dx < 0 && dy < 0) {
        DrawRectangle(x + 1, y, 2, 2, c_glow);
        DrawRectangle(x + 2, y + 2, 4, 4, base_color);
        DrawRectangle(x + 3, y + 3, 2, 2, c_cockpit);
        DrawRectangle(x + 4, y + 6, 6, 4, base_color);
        DrawRectangle(x + 8, y + 8, 2, 2, c_flame);
    } else if (dx > 0 && dy > 0) {
        DrawRectangle(x + 7, y + 8, 2, 2, c_glow);
        DrawRectangle(x + 4, y + 4, 4, 4, base_color);
        DrawRectangle(x + 5, y + 4, 2, 2, c_cockpit);
        DrawRectangle(x, y, 6, 4, base_color);
        DrawRectangle(x, y, 2, 2, c_flame);
    } else if (dx < 0 && dy > 0) {
        DrawRectangle(x + 1, y + 8, 2, 2, c_glow);
        DrawRectangle(x + 2, y + 4, 4, 4, base_color);
        DrawRectangle(x + 3, y + 4, 2, 2, c_cockpit);
        DrawRectangle(x + 4, y, 6, 4, base_color);
        DrawRectangle(x + 8, y, 2, 2, c_flame);
    } else {
        DrawRectangle(x + 2, y + 2, 6, 6, base_color);
        DrawRectangle(x + 4, y + 4, 2, 2, c_cockpit);
        DrawRectangle(x + (dx > 0 ? 6 : 0), y + (dy > 0 ? 6 : 0), 4, 4, c_glow);
        DrawRectangle(x + (dx > 0 ? 0 : 6), y + (dy > 0 ? 0 : 6), 4, 4, c_shadow);
    }
}

void DrawEnemyPC(Enemy *e, int frame) {
    Color color;
    if (e->freeze_timer > 0) color = C_ICE;
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

    if (e->type == 0) {
        int anim = (frame / 8) % 2;
        DrawRectangle(x + 4, y, 2, 3, color);
        DrawRectangle(x + 2, y + 3, 6, 3, color);
        DrawRectangle(x, y + 5, 10, 3, color);
        DrawRectangle(x + 4, y + 4, 2, 2, WHITE);
        if (anim == 0) {
            DrawRectangle(x + 1, y + 8, 2, 2, GBA_COLOR(31, 16, 0));
            DrawRectangle(x + 7, y + 8, 2, 2, GBA_COLOR(31, 16, 0));
        } else {
            DrawRectangle(x + 1, y + 8, 2, 1, GBA_COLOR(31, 31, 0));
            DrawRectangle(x + 7, y + 8, 2, 1, GBA_COLOR(31, 31, 0));
        }
    } else if (e->type == 1) {
        int anim = (frame / 6) % 2;
        DrawRectangle(x + 4, y, 2, 2, color);
        DrawRectangle(x + 3, y + 2, 4, 3, color);
        DrawRectangle(x + 1, y + 5, 8, 3, color);
        DrawRectangle(x + 4, y + 3, 2, 2, C_CYAN);
        if (anim == 0) {
            DrawRectangle(x + 2, y + 8, 2, 1, C_YELLOW);
            DrawRectangle(x + 6, y + 8, 2, 1, C_YELLOW);
        } else {
            DrawRectangle(x + 3, y + 8, 2, 2, C_RED);
        }
    } else if (e->type == 2) {
        int pulse = (frame / 12) % 2;
        DrawRectangle(x + 2, y, 6, 2, color);
        DrawRectangle(x, y + 2, 10, 6, color);
        DrawRectangle(x + 2, y + 3, 6, 4, pulse ? C_RED : C_YELLOW);
        DrawRectangle(x + 1, y + 8, 2, 2, color);
        DrawRectangle(x + 7, y + 8, 2, 2, color);
    } else if (e->type == 4) {
        int glow = (frame / 15) % 2;
        DrawRectangle(x + 4, y, 2, 10, color);
        DrawRectangle(x, y + 4, 10, 2, color);
        DrawRectangle(x + 3, y + 3, 4, 4, color);
        DrawRectangle(x + 4, y + 4, 2, 2, glow ? WHITE : C_CYAN);
    } else if (e->type == 5) {
        int anim = (frame / 10) % 2;
        DrawRectangle(x + 2, y + 1, 6, 8, color);
        DrawRectangle(x + 1, y + 3, 8, 4, color);
        DrawRectangle(x + 3, y + 3, 4, 4, anim ? WHITE : C_YELLOW);
        DrawRectangle(x, y + 4, 2, 2, color);
        DrawRectangle(x + 8, y + 4, 2, 2, color);
    } else if (e->type == 6) {
        DrawRectangle(x + 2, y + 2, 6, 6, color);
        DrawRectangle(x + 3, y + 3, 4, 4, C_RED);
        if((frame / 4) % 2 == 0) DrawRectangle(x + 4, y + 4, 2, 2, WHITE);
    } else if (e->type == 7) {
        int trail = (frame / 3) % 2;
        DrawRectangle(x + 4, y, 2, 3, WHITE);
        DrawRectangle(x + 3, y + 3, 4, 4, color);
        DrawRectangle(x + 2, y + 7, 6, 2, C_RED);
        if (trail == 0) {
            DrawRectangle(x + 4, y + 9, 2, 2, C_YELLOW);
        } else {
            DrawRectangle(x + 3, y + 9, 4, 1, C_YELLOW);
        }
    } else if (e->type == 8) {
        int wing_anim = (frame / 7) % 2;
        DrawRectangle(x + 3, y + 3, 4, 4, color);
        if (wing_anim == 0) {
            DrawRectangle(x, y + 1, 3, 3, C_CYAN);
            DrawRectangle(x + 7, y + 1, 3, 3, C_CYAN);
        } else {
            DrawRectangle(x, y + 5, 3, 3, C_CYAN);
            DrawRectangle(x + 7, y + 5, 3, 3, C_CYAN);
        }
        DrawRectangle(x + 4, y + 4, 2, 2, WHITE);
    } else if (e->type == 9) {
        // Teletransportador: Cristal cuántico con pulso energético suave
        int pulse = (frame / 15) % 2;
        DrawRectangle(x + 2, y, 6, 10, color);
        DrawRectangle(x, y + 2, 10, 6, color);
        DrawRectangle(x + 3, y + 3, 4, 4, pulse ? WHITE : C_CYAN);
        // Pequeños destellos en los extremos
        DrawRectangle(x + 4, y, 2, 1, WHITE);
        DrawRectangle(x + 4, y + 9, 2, 1, WHITE);
    } else {
        int core_pulse = (frame / 10) % 2;
        DrawRectangle(x, y + 2, 10, 6, color);
        DrawRectangle(x + 2, y, 6, 8, color);
        DrawRectangle(x + 1, y + 8, 8, 2, color);
        DrawRectangle(x - 2, y + 4, 2, 4, C_RED);
        DrawRectangle(x + 10, y + 4, 2, 4, C_RED);
        DrawRectangle(x + 3, y + 3, 4, 4, core_pulse ? WHITE : C_YELLOW);
    }
}

void DrawPowerUpPC(PowerUp *p) {
    int x = p->x, y = p->y;
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