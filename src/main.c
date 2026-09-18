#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "types.h"
#include "audio.h"
#include "graphics.h"
#include "platform_paths.h"
#include "mobile_input.h"

#if defined(PLATFORM_IOS)
#include "ios_gamepad.h"
#define IsGamepadAvailable(pad) IOSGamepad_IsAvailable(pad)
#define GetGamepadAxisMovement(pad, axis) IOSGamepad_GetAxisMovement(pad, axis)
#define IsGamepadButtonDown(pad, btn) IOSGamepad_IsButtonDown(pad, btn)
#define IsGamepadButtonPressed(pad, btn) IOSGamepad_IsButtonPressed(pad, btn)
#define IsGamepadButtonReleased(pad, btn) IOSGamepad_IsButtonReleased(pad, btn)
extern int IOS_GetMaxRefreshRate(void);
extern void IOS_SetDisplayFPS(int fps);
#endif

InputDeviceType g_last_input_device = INPUT_KEYBOARD;
int g_device_toast_timer = 0;
static float g_device_toast_timer_f = 0.0f;
InputDeviceType g_toast_device = INPUT_KEYBOARD;

#define ACH_NOVATO_FACIL 0
#define ACH_NOVATO_NORMAL 1
#define ACH_NOVATO_DIFICIL 2
#define ACH_NOVATO_INFIERNO 3
#define ACH_VETERANO_FACIL 4
#define ACH_VETERANO_NORMAL 5
#define ACH_VETERANO_DIFICIL 6
#define ACH_VETERANO_INFIERNO 7
#define ACH_MASTER_FACIL 8
#define ACH_MASTER_NORMAL 9
#define ACH_MASTER_DIFICIL 10
#define ACH_MASTER_INFIERNO 11
#define ACH_CALIENTE 12
#define ACH_INTACTO 13
#define ACH_BOOM 14
#define ACH_FRIO_FRIO 15
#define ACH_TOXICIDAD 16
#define ACH_TRIADA_ELEMENTAL 17
#define ACH_TIEMPO_MUERTO 18
#define ACH_EMPODERATE 19
#define ACH_NO_TOCARME 20
#define ACH_CAZADOR_JEFES 21
#define ACH_TELETRANSP 22
#define ACH_ULTIMO_ALIENTO 23
#define ACH_FULL_ARSENAL 24
#define ACH_MODO_DIOS 25
#define ACH_DEMASIADO_RAPIDO 26
#define ACH_ESQUIVE_PERFECTO 27
#define ACH_LIMPIEZA_TOTAL 28
#define ACH_VOY_CIEGO 29
#define ACH_SIN_RASGUNOS 30
#define ACH_ULTIMO_SUPERVIVIENTE 31
#define ACH_A_CONTRACORRIENTE 32
#define ACH_PISALO 33
#define ACH_NI_UN_PASO_ATRAS 34
#define ACH_BAILANDO_ENTRE_BALAS 35
#define ACH_DIVIDE_Y_VENCERAS 36
#define ACH_KABOOM 37
#define ACH_DONDE_ESTA 38
#define ACH_NO_ERA_TAN_DIFICIL 39
#define ACH_INFIERNO_PURO 40
#define ACH_A_UNA_VIDA 41
#define ACH_SIN_AYUDA 42
#define ACH_RUSH_NOVATO 43
#define ACH_RUSH_MAESTRO 44
#define ACH_RUSH_EXPERTO 45
#define ACH_RUSH_QUE_HACES_AQUI 46
#define ACH_RUSH_ENTERITO 47
#define ACH_RUSH_VENENO_ATOPE 48
#define ACH_RUSH_HIELO_ATOPE 49
#define ACH_RUSH_MOJADO_ATOPE 50
#define ACH_RUSH_HASTA_ARRIBA 51
#define ACH_RUSH_PODEROSO 52
#define ACH_RUSH_MUERTE_SUBITA 53
#define ACH_TA_1MIN 54
#define ACH_TA_3MIN 55
#define ACH_TA_5MIN 56
#define ACH_TA_RELOJ_LLENO 57
#define ACH_TA_CAZADOR 58
#define ACH_TA_BOSS 59
#define ACH_ASSAULT_1MIN 60
#define ACH_ASSAULT_3MIN 61
#define ACH_ASSAULT_5MIN 62
#define ACH_ASSAULT_CAZADOR 63
#define ACH_ASSAULT_BOSS 64
#define ACH_ASSAULT_RULETA 65
#define ACH_LEYENDA 66

#define ACHIEVEMENT_QUEUE_SIZE 67
#define ARSENAL_NORMAL_MASK 1
#define ARSENAL_RAPID_MASK 2
#define ARSENAL_TRIPLE_MASK 4
#define ARSENAL_HEAVY_MASK 8
#define ARSENAL_FREEZE_MASK 16
#define ARSENAL_POISON_MASK 32
#define ARSENAL_WATER_MASK 64
#define ARSENAL_ALL_MASK 127

bool g_achievements_unlocked[NUM_ACHIEVEMENTS] = { false };
int g_achievement_progress[NUM_ACHIEVEMENTS] = { 0 };
int g_enemies_killed_per_diff[4] = { 0, 0, 0, 0 };
int g_achievement_queue[ACHIEVEMENT_QUEUE_SIZE] = { 0 };
int g_achievement_queue_count = 0;
int selected_ach_index = 0;
bool show_ach_details = false;
int g_achievement_toast_timer = 0;
static float g_achievement_toast_timer_f = 0.0f;
int g_latest_unlocked_ach = 0;
bool player_took_hit_this_wave = false;

// Variables de seguimiento para los nuevos logros
int g_waves_without_damage_streak = 0;
bool g_boss_dash_used = false;
int g_consecutive_bullets_dodged = 0;
bool g_divisor_combo_active = false;
int g_divisor_fragments_left = 0;
int g_divisor_combo_timer = 0;
int g_powerups_picked_this_wave = 0;
int g_powerups_picked_this_run = 0;
static int g_assault_weapons_killed_mask = 0;

// Variables para Código Konami
static bool g_konami_redeemed = false;
static bool g_konami_hell_bonus = false;
static bool show_konami_popup = false;
static int g_konami_step = 0;

// Variables para el Menú de Trucos
static bool g_show_cheat_popup = false;
static char g_cheat_input[32] = {0};
static int g_cheat_input_len = 0;
static bool g_cheat_error = false;
static bool g_cheat_already_used_err = false;
static int g_cheat_error_timer = 0;
static bool g_cheat_success = false;
static int g_cheat_success_timer = 0;
static char g_cheat_success_msg[48] = {0};

// Canjes únicos por sesión (como el código Konami)
static bool g_cheat_rainbow_redeemed = false;
static bool g_cheat_barrelroll_redeemed = false;
static bool g_cheat_tiny_redeemed = false;
static bool g_cheat_holycow_redeemed = false;
static bool g_cheat_matrix_redeemed = false;
static bool g_cheat_chonky_redeemed = false;
static bool g_cheat_flip_redeemed = false;
static bool g_cheat_onehit_redeemed = false;
static bool g_cheat_pewpew_redeemed = false;

// Trucos encolados para la SIGUIENTE partida
static bool g_cheat_rainbow_queued = false;
static int g_cheat_size_queued = 0; // 0 = normal, 1 = tiny, 2 = holycow
static bool g_cheat_chonky_queued = false;
static bool g_cheat_flip_queued = false;
static bool g_cheat_onehit_queued = false;
static bool g_cheat_pewpew_queued = false;

// Trucos activos en la partida en curso
static bool g_cheat_rainbow_active = false;
static int g_cheat_size_active = 0; // 0 = normal, 1 = tiny, 2 = holycow
static bool g_cheat_matrix_active = false;
static bool g_cheat_chonky_active = false;
static bool g_cheat_flip_active = false;
static bool g_cheat_onehit_active = false;
bool g_cheat_pewpew_active = false;

// Animación de barril para la pantalla de inicio
static bool g_cheat_barrelroll_pending = false;
static float g_barrel_roll_timer = 0.0f;

int game_mode_selection = 0;
bool show_rush_popup = false;
int current_game_mode = MODE_CLASSIC;

int rush_kills_in_wave = 0;
int rush_target_kills = 20;
int rush_cycle = 0;
int rush_wave_banner_timer = 0;
int rush_run_kills = 0;
int records_mode_tab = 0;

int ta_time_left_frames = 30 * 60;
int ta_time_survived_frames = 0;
int ta_run_kills = 0;
int ta_spawn_rate = 90;
int ta_spawn_timer = 0;
bool ta_time_up = false;

int assault_timer_frames = 15 * 60;
int assault_time_survived_frames = 0;
int assault_run_kills = 0;
int assault_banner_timer = 0;
int assault_cur_pu_name_id = 0;
Color assault_cur_pu_color = (Color){ 255, 255, 255, 255 };

static const int ach_enemy_types[NUM_ACHIEVEMENTS] = {
    0, 1, 2, 3, // 0-3: Novato Facil, Normal, Dificil, Infierno
    4, 5, 7, 8, // 4-7: Veterano Facil, Normal, Dificil, Infierno
    9, 2, 4, 3, // 8-11: Master Facil, Normal, Dificil, Infierno
    2, // 12: Caliente (Tanque)
    0, // 13: Intacto (Normal)
    7, // 14: Boom (Kamikaze)
    1, // 15: Frio Frio (Rapido)
    2, // 16: Toxicidad (Tanque)
    0, // 17: Triada Elemental (Normal)
    4, // 18: Tiempo Muerto (Sniper)
    8, // 19: Empoderate (Patrullero)
    1, // 20: No Tocarme (Rapido)
    3, // 21: Cazador Jefes (Boss)
    9, // 22: Teletransp (Teleport)
    0, // 23: Ultimo Aliento (Normal)
    4, // 24: Full Arsenal (Sniper)
    3, // 25: Modo Dios (Boss)
    1, // 26: Demasiado Rap (Rapido)
    4, // 27: Esquive Perfecto (Sniper)
    7, // 28: Limpieza Total (Kamikaze)
    8, // 29: Voy Ciego (Patrullero)
    0, // 30: Sin Rasgunos (Normal)
    2, // 31: Ultimo Superviviente (Tanque)
    1, // 32: A Contracorriente (Rapido)
    7, // 33: Pisalo (Kamikaze)
    3, // 34: Ni Un Paso Atras (Boss)
    4, // 35: Bailando entre Balas (Sniper)
    5, // 36: Divide y Venceras (Divisor)
    7, // 37: Kaboom (Kamikaze)
    9, // 38: Donde Esta? (Teleport)
    8, // 39: No Era Tan Dificil (Patrullero)
    1, // 40: Infierno Puro (Rapido)
    3, // 41: A Una Vida (Boss)
    2, // 42: Sin Ayuda (Tanque)
    0, // 43: Novato Rush (Normal)
    4, // 44: Maestro Rush (Sniper)
    3, // 45: Experto Rush (Boss)
    3, // 46: Que Haces Aqui? (Boss)
    8, // 47: Enterito (Patrullero)
    2, // 48: Veneno Atope (Tanque)
    1, // 49: Hielo Atope (Rapido)
    4, // 50: Mojado Atope (Sniper)
    0, // 51: Hasta Arriba (Normal)
    5, // 52: Poderoso (Divisor)
    7, // 53: Muerte Subita (Kamikaze)
    0, // 54: 1 Minuto de Gloria (Normal)
    1, // 55: Superviviente del Tiempo (Rapido)
    4, // 56: Maestro del Cronos (Sniper)
    8, // 57: Reloj Lleno (Patrullero)
    7, // 58: Cazador Temporal (Kamikaze)
    3, // 59: Destrucción Veloz (Boss)
    0, // 60: Recluta de Asalto (Normal)
    1, // 61: Veterano de Asalto (Rapido)
    4, // 62: Maestro de Asalto (Sniper)
    7, // 63: Armero Imparable (Kamikaze)
    3, // 64: Duelo de Asalto (Boss)
    5, // 65: Ruleta Letal (Divisor)
    3  // 66: Leyenda (Boss - Platino)
};

static inline StringId GetAchTitleId(int index) {
    if (index < 0 || index >= NUM_ACHIEVEMENTS) return STR_ACH_1_TITLE;
    return (StringId)(STR_ACH_1_TITLE + index * 2);
}

static inline StringId GetAchDescId(int index) {
    if (index < 0 || index >= NUM_ACHIEVEMENTS) return STR_ACH_1_DESC;
    return (StringId)(STR_ACH_1_DESC + index * 2);
}

KeyBindings g_keys = {
    .key_up = KEY_W,
    .key_down = KEY_S,
    .key_left = KEY_A,
    .key_right = KEY_D,
    .key_shoot = KEY_SPACE,
    .key_turbo = KEY_K,
    .key_aim = KEY_LEFT_SHIFT,
    .key_dash = KEY_C
};

PadBindings g_pad = {
    .btn_up = GAMEPAD_BUTTON_LEFT_FACE_UP,
    .btn_down = GAMEPAD_BUTTON_LEFT_FACE_DOWN,
    .btn_left = GAMEPAD_BUTTON_LEFT_FACE_LEFT,
    .btn_right = GAMEPAD_BUTTON_LEFT_FACE_RIGHT,
    .btn_shoot = GAMEPAD_BUTTON_RIGHT_FACE_DOWN,
    .btn_turbo = GAMEPAD_BUTTON_RIGHT_FACE_LEFT,
    .btn_aim = GAMEPAD_BUTTON_LEFT_TRIGGER_1,
    .btn_dash = GAMEPAD_BUTTON_RIGHT_TRIGGER_1
};

GameConfig g_config = {
    .target_fps = 1,
    .vsync = 1,
    .res_index = 2,
    .screen_mode = 0,
    .language = 0,
    .vol_bgm = 10,
    .vol_sfx = 10,
    .crt_filter = 0
};

const int g_resolutions[RESOLUTION_COUNT][2] = {
    { 960, 640 },    // 3:2
    { 1280, 720 },   // 16:9
    { 1280, 800 },   // 16:10
    { 1440, 900 },   // 16:10
    { 1440, 960 },   // 3:2
    { 1600, 900 },   // 16:9
    { 1680, 1050 },  // 16:10
    { 1920, 1080 },  // 16:9
    { 1920, 1200 },  // 16:10
    { 2560, 1440 },  // 16:9
    { 2560, 1600 },  // 16:10
    { 3440, 1440 },  // 21:9 Ultrawide
    { 3840, 2160 }   // 16:9 4K
};

const char* const g_res_names[RESOLUTION_COUNT] = {
    "960X640 (3:2)",
    "1280X720 (16:9)",
    "1280X800 (16:10)",
    "1440X900 (16:10)",
    "1440X960 (3:2)",
    "1600X900 (16:9)",
    "1680X1050 (16:10)",
    "1920X1080 (16:9)",
    "1920X1200 (16:10)",
    "2560X1440 (16:9)",
    "2560X1600 (16:10)",
    "3440X1440 (21:9)",
    "3840X2160 (16:9)"
};


const char* const g_screen_mode_names[3] = { "COMPLETA", "16:9 WIDE", "3:2 RETRO" };
const char* const g_lang_names[LANG_COUNT] = { "ES", "EN", "FR", "IT", "DE" };

const char* const g_lang_strings[LANG_COUNT][STR_COUNT] = {
    [0] = { // ESPAÑOL
        [STR_PLAY] = "JUGAR", [STR_OPTIONS] = "OPCIONES", [STR_RECORDS] = "RECORDS", [STR_VOL_BGM] = "VOL. MUSICA",
        [STR_VOL_SFX] = "VOL. EFECTOS", [STR_FILTER] = "FILTRO", [STR_CONTROLS] = "CONTROLES", [STR_RESOLUTION] = "RES",
        [STR_SCREEN_MODE] = "MODO", [STR_FPS] = "FPS", [STR_VSYNC] = "VSYNC", [STR_LANGUAGE] = "IDIOMA",
        [STR_DELETE_RECORDS] = "BORRAR RECORDS", [STR_CONFIRM_DELETE_TITLE] = "BORRAR RECORDS?", [STR_CONFIRM_DELETE_SUB] = "ACCION IRREVERSIBLE",
        [STR_CANCEL] = "NO, CANCELAR", [STR_CONFIRM_DELETE_BTN] = "SI, BORRAR", [STR_DIFF_TITLE] = "DIFICULTAD",
        [STR_DIFF_EASY] = "FACIL", [STR_DIFF_NORMAL] = "NORMAL", [STR_DIFF_HARD] = "DIFICIL", [STR_DIFF_HELL] = "INFIERNO",
        [STR_PAUSE] = "PAUSA", [STR_RESUME] = "REANUDAR", [STR_MAIN_MENU] = "MENU PRINCIPAL", [STR_GAME_OVER] = "GAME OVER",
        [STR_POINTS] = "PUNTOS", [STR_RETRY] = "REINTENTAR", [STR_QUIT_PROMPT] = "SALIR DEL JUEGO?",
        [STR_QUIT_SUB] = "QUIERES SALIR AHORA?", [STR_YES] = "SI", [STR_NO] = "NO", [STR_CTRL_TITLE] = "CONTROLES",
        [STR_CTRL_UP] = "ARRIBA", [STR_CTRL_DOWN] = "ABAJO", [STR_CTRL_LEFT] = "IZQUIERDA", [STR_CTRL_RIGHT] = "DERECHA",
        [STR_CTRL_SHOOT] = "DISPARO", [STR_CTRL_TURBO] = "TURBO", [STR_CTRL_AIM] = "APUNTAR",
        [STR_CTRL_DASH] = "DASH", [STR_REBIND_TITLE] = "REASIGNAR ENTRADA", [STR_PRESS_KEY] = "PULSA TECLA O BOTON...",
        [STR_CANCEL_KEY] = "BKSP / ESC CANCELAR", [STR_INVALID_ACTION] = "ACCION INVALIDA", [STR_DUPLICATE_KEY] = "ENTRADA YA ASIGNADA",
        [STR_USE_OTHER_KEY] = "USA OTRA DIFERENTE", [STR_PRESS_BACK] = "PULSA %s PARA VOLVER", [STR_PRESENTS] = "PRESENTA...",
        [STR_READY] = "LISTO!", [STR_GO] = "YA!", [STR_WAVE] = "OLEADA",
        [STR_GAMEPAD_CONNECTED] = "MANDO CONECTADO", [STR_KEYBOARD_CONNECTED] = "TECLADO CONECTADO",
        [STR_FREEZE] = "CONGELADO!", [STR_POISON] = "VENENO!",
        [STR_LOGROS] = "LOGROS",
        [STR_ACH_1_TITLE] = "NOVATO FACIL", [STR_ACH_1_DESC] = "ELIMINA 10 ENEMIGOS EN FACIL",
        [STR_ACH_2_TITLE] = "NOVATO NORMAL", [STR_ACH_2_DESC] = "ELIMINA 10 ENEMIGOS EN NORMAL",
        [STR_ACH_3_TITLE] = "NOVATO DIFICIL", [STR_ACH_3_DESC] = "ELIMINA 10 ENEMIGOS EN DIFICIL",
        [STR_ACH_4_TITLE] = "NOVATO INFIERNO", [STR_ACH_4_DESC] = "ELIMINA 10 ENEMIGOS EN INFIERNO",
        [STR_ACH_5_TITLE] = "VETERANO FACIL", [STR_ACH_5_DESC] = "ELIMINA 100 ENEMIGOS EN FACIL",
        [STR_ACH_6_TITLE] = "VETERANO NORMAL", [STR_ACH_6_DESC] = "ELIMINA 100 ENEMIGOS EN NORMAL",
        [STR_ACH_7_TITLE] = "VETERANO DIFICIL", [STR_ACH_7_DESC] = "ELIMINA 100 ENEMIGOS EN DIFICIL",
        [STR_ACH_8_TITLE] = "VETERANO INFIERNO", [STR_ACH_8_DESC] = "ELIMINA 100 ENEMIGOS EN INFIERNO",
        [STR_ACH_9_TITLE] = "MASTER FACIL", [STR_ACH_9_DESC] = "ELIMINA 500 ENEMIGOS EN FACIL",
        [STR_ACH_10_TITLE] = "MASTER NORMAL", [STR_ACH_10_DESC] = "ELIMINA 500 ENEMIGOS EN NORMAL",
        [STR_ACH_11_TITLE] = "MASTER DIFICIL", [STR_ACH_11_DESC] = "ELIMINA 500 ENEMIGOS EN DIFICIL",
        [STR_ACH_12_TITLE] = "MASTER INFIERNO", [STR_ACH_12_DESC] = "ELIMINA 500 ENEMIGOS EN INFIERNO",
        [STR_ACH_13_TITLE] = "CALIENTE", [STR_ACH_13_DESC] = "ELIMINA 25 CON DISPARO PESADO",
        [STR_ACH_14_TITLE] = "INTACTO", [STR_ACH_14_DESC] = "COMPLETA UNA OLEADA SIN DAÑO",
        [STR_ACH_15_TITLE] = "!BOOM!", [STR_ACH_15_DESC] = "ELIMINA 5 CON UNA NUKE",
        [STR_ACH_16_TITLE] = "FRIO, FRIO", [STR_ACH_16_DESC] = "CONGELA 10 ENEMIGOS",
        [STR_ACH_17_TITLE] = "TOXICIDAD", [STR_ACH_17_DESC] = "ENVENENA 10 ENEMIGOS",
        [STR_ACH_18_TITLE] = "TRIADA ELEMENTAL", [STR_ACH_18_DESC] = "AFECTA A 300 CON HIELO, VENENO O AGUA EN CLASICO",
        [STR_ACH_19_TITLE] = "TIEMPO MUERTO", [STR_ACH_19_DESC] = "ELIMINA 5 ENEMIGOS DURANTE SLOW-MO",
        [STR_ACH_20_TITLE] = "EMPODERATE", [STR_ACH_20_DESC] = "COGE UN POWER-UP POR PRIMERA VEZ",
        [STR_ACH_21_TITLE] = "NO TOCARME", [STR_ACH_21_DESC] = "30S EN INFIERNO SIN DAÑO",
        [STR_ACH_22_TITLE] = "CAZADOR JEFES", [STR_ACH_22_DESC] = "DESTRUYE 5 JEFES",
        [STR_ACH_23_TITLE] = "TELETRANSP.", [STR_ACH_23_DESC] = "DESTRUYE 10 TELEPORTS",
        [STR_ACH_24_TITLE] = "ULTIMO ALIENTO", [STR_ACH_24_DESC] = "1000 PTOS CON 1 CORAZON",
        [STR_ACH_25_TITLE] = "FULL ARSENAL", [STR_ACH_25_DESC] = "USA TODOS LOS DISPAROS",
        [STR_ACH_26_TITLE] = "MODO DIOS", [STR_ACH_26_DESC] = "COMBO X10 CON POWER-UP",
        [STR_ACH_27_TITLE] = "DEMASIADO RAP.", [STR_ACH_27_DESC] = "10 KILLS EN RAPID FIRE",
        [STR_ACH_28_TITLE] = "ESQUIVE PERFE.", [STR_ACH_28_DESC] = "EVITA 10 PROYECTILES DASH",
        [STR_ACH_29_TITLE] = "LIMPIEZA TOT.", [STR_ACH_29_DESC] = "NUKE CON 10+ ENEMIGOS",
        [STR_ACH_30_TITLE] = "VOY CIEGO", [STR_ACH_30_DESC] = "EMBORRACHATE TOMANDO LA BOTELLA",
        [STR_ACH_31_TITLE] = "SIN DAÑOS", [STR_ACH_31_DESC] = "SOBREVIVE 3 OLEADAS SIN DAÑO",
        [STR_ACH_32_TITLE] = "ULTIMO SUPERVIVIENTE", [STR_ACH_32_DESC] = "10000 PTOS CON 1 CORAZON",
        [STR_ACH_33_TITLE] = "A CONTRACORRIENTE", [STR_ACH_33_DESC] = "MATA 10 CON CONTROL INVERTIDO",
        [STR_ACH_34_TITLE] = "PISALO", [STR_ACH_34_DESC] = "MATA KAMIKAZE A QUEMARROPA",
        [STR_ACH_35_TITLE] = "NI UN PASO ATRAS", [STR_ACH_35_DESC] = "DERROTA UN BOSS SIN USAR DASH",
        [STR_ACH_36_TITLE] = "BAILANDO ENTRE BALAS", [STR_ACH_36_DESC] = "ESQUIVA 25 BALAS SIN DAÑO",
        [STR_ACH_37_TITLE] = "DIVIDE Y VENCERAS", [STR_ACH_37_DESC] = "MATA UN DIVISOR Y SUS 2 MINIS",
        [STR_ACH_38_TITLE] = "KABOOM", [STR_ACH_38_DESC] = "KAMIKAZE DESTRUYE A OTRO ENEMIGO",
        [STR_ACH_39_TITLE] = "DONDE ESTA?", [STR_ACH_39_DESC] = "MATA TELEPORT TRAS TELETRANSPORTE",
        [STR_ACH_40_TITLE] = "NO ERA TAN DIFICIL", [STR_ACH_40_DESC] = "PASAR OLEADA DIFICIL SIN POWERUPS",
        [STR_ACH_41_TITLE] = "INFIERNO PURO", [STR_ACH_41_DESC] = "LLEGA A OLEADA 15 EN INFIERNO",
        [STR_ACH_42_TITLE] = "A UNA VIDA", [STR_ACH_42_DESC] = "MATA BOSS EN INFIERNO CON 1 HP",
        [STR_ACH_43_TITLE] = "SIN AYUDA", [STR_ACH_43_DESC] = "5000 PTOS EN INFIERNO SIN POWERUPS",
        [STR_ACH_44_TITLE] = "NOVATO RUSH", [STR_ACH_44_DESC] = "SUPERA LA PRIMERA OLEADA EN RUSH",
        [STR_ACH_45_TITLE] = "MAESTRO RUSH", [STR_ACH_45_DESC] = "SUPERA LA CUARTA OLEADA EN RUSH",
        [STR_ACH_46_TITLE] = "EXPERTO RUSH", [STR_ACH_46_DESC] = "SUPERA LA PRIMERA OLEADA DE JEFES",
        [STR_ACH_47_TITLE] = "QUE HACES AQUI?", [STR_ACH_47_DESC] = "SUPERA LA SEGUNDA OLEADA DE JEFES",
        [STR_ACH_48_TITLE] = "ENTERITO", [STR_ACH_48_DESC] = "SUPERA UNA OLEADA SIN DAÑO EN RUSH",
        [STR_ACH_49_TITLE] = "VENENO ATOPE", [STR_ACH_49_DESC] = "ENVENENA A 500 ENEMIGOS EN RUSH",
        [STR_ACH_50_TITLE] = "HIELO ATOPE", [STR_ACH_50_DESC] = "CONGELA A 500 ENEMIGOS EN RUSH",
        [STR_ACH_51_TITLE] = "MOJADO ATOPE", [STR_ACH_51_DESC] = "MOJA A 500 ENEMIGOS EN RUSH",
        [STR_ACH_52_TITLE] = "HASTA ARRIBA", [STR_ACH_52_DESC] = "ACUMULA 5 CORAZONES EN MODO RUSH",
        [STR_ACH_53_TITLE] = "PODEROSO", [STR_ACH_53_DESC] = "UTILIZA 60 POWER-UPS EN MODO RUSH",
        [STR_ACH_54_TITLE] = "MUERTE SUBITA", [STR_ACH_54_DESC] = "ACUMULA 50 MUERTES EN MODO RUSH",
        [STR_ACH_55_TITLE] = "UN MINUTO DE GLORIA", [STR_ACH_55_DESC] = "SOBREVIVE 1 MINUTO EN CONTRARRELOJ",
        [STR_ACH_56_TITLE] = "SUPERVIVIENTE DEL TIEMPO", [STR_ACH_56_DESC] = "SOBREVIVE 3 MINUTOS EN CONTRARRELOJ",
        [STR_ACH_57_TITLE] = "MAESTRO DEL CRONOS", [STR_ACH_57_DESC] = "SOBREVIVE 5 MINUTOS EN CONTRARRELOJ",
        [STR_ACH_58_TITLE] = "RELOJ LLENO", [STR_ACH_58_DESC] = "ACUMULA 90 SEGUNDOS EN EL RELOJ",
        [STR_ACH_59_TITLE] = "CAZADOR TEMPORAL", [STR_ACH_59_DESC] = "ELIMINA 100 ENEMIGOS EN CONTRARRELOJ",
        [STR_ACH_60_TITLE] = "DESTRUCCION VELOZ", [STR_ACH_60_DESC] = "ELIMINA UN BOSS EN CONTRARRELOJ",
        [STR_ACH_61_TITLE] = "RECLUTA DE ASALTO", [STR_ACH_61_DESC] = "SOBREVIVE 1 MINUTO EN MODO ASALTO",
        [STR_ACH_62_TITLE] = "VETERANO DE ASALTO", [STR_ACH_62_DESC] = "SOBREVIVE 3 MINUTOS EN MODO ASALTO",
        [STR_ACH_63_TITLE] = "MAESTRO DE ASALTO", [STR_ACH_63_DESC] = "SOBREVIVE 5 MINUTOS EN MODO ASALTO",
        [STR_ACH_64_TITLE] = "ARMERO IMPARABLE", [STR_ACH_64_DESC] = "ELIMINA 100 ENEMIGOS EN ASALTO",
        [STR_ACH_65_TITLE] = "DUELO DE ASALTO", [STR_ACH_65_DESC] = "ELIMINA UN BOSS EN MODO ASALTO",
        [STR_ACH_66_TITLE] = "RULETA LETAL", [STR_ACH_66_DESC] = "ELIMINA CON 5 ARMAS EN ASALTO",
        [STR_ACH_67_TITLE] = "LEYENDA", [STR_ACH_67_DESC] = "DESBLOQUEA TODOS LOS DEMAS LOGROS",
        [STR_NEW_ACHIEVEMENT] = "!LOGRO NUEVO!",
        [STR_ACH_DETAILS_HINT] = "PULSA UN LOGRO O ENTER PARA VER DETALLES",
        [STR_STATUS_UNLOCKED] = "ESTADO: DESBLOQUEADO (OK)",
        [STR_PROGRESS] = "PROGRESO",
        [STR_MODE_WINDOWED] = "VENTANA",
        [STR_MODE_BORDERLESS] = "SIN BORDES",
        [STR_GAME_MODE_TITLE] = "MODOS DE JUEGO",
        [STR_MODE_CLASSIC] = "CLASICO",
        [STR_MODE_RUSH] = "RUSH",
        [STR_MODE_TIME_ATTACK] = "CONTRARRELOJ",
        [STR_MODE_COMING_SOON] = "PROXIMAMENTE",
        [STR_RUSH_UNAVAILABLE_TITLE] = "MODO EN DESARROLLO",
        [STR_RUSH_UNAVAILABLE_DESC] = "ESTE MODO ESTARA DISPONIBLE PROXIMAMENTE",
        [STR_CLOSE_HINT] = "PULSA CUALQUIER TECLA PARA CERRAR",
        [STR_MAX_SCORE] = "PUNTOS MAX",
        [STR_MAX_WAVE] = "OLEADA MAX",
        [STR_TOTAL_KILLS] = "ENEMIGOS K.O.",
        [STR_MAX_TIME] = "TIEMPO MAX",
        [STR_TIME_UP] = "!TIEMPO!",
        [STR_TIME_LEFT] = "TIEMPO",
        [STR_WET] = "!MOJADO!",
        [STR_KONAMI_TITLE] = "ENHORABUENA MAESTRO!",
        [STR_KONAMI_SUB1] = "ERES UN VERDADERO FAN DEL GAMING",
        [STR_KONAMI_SUB2] = "TANTO MODERNO COMO RETRO!",
        [STR_KONAMI_SUB3] = "+2 VIDAS EXTRAS EN TU PROXIMO INFIERNO!",
        [STR_MODE_ASSAULT] = "ASALTO",
        [STR_ASSAULT_CHANGE] = "!CAMBIO DE ARMA!",
        [STR_ASSAULT_NEXT] = "CAMBIO",
        [STR_PU_SHIELD_NAME] = "ESCUDO ACTIVO",
        [STR_PU_RAPID_NAME] = "DISPARO RAPIDO",
        [STR_PU_TRIPLE_NAME] = "TRIPLE DISPARO",
        [STR_PU_HEAVY_NAME] = "DISPARO PESADO",
        [STR_PU_STAR_NAME] = "INVENCIBILIDAD",
        [STR_PU_SLOWMO_NAME] = "RALENTIZACION",
        [STR_PU_FREEZE_NAME] = "DISPARO HELADO",
        [STR_PU_POISON_NAME] = "DISPARO VENENOSO",
        [STR_PU_WATER_NAME] = "DISPARO ACUATICO",
        [STR_PU_NUKE_NAME] = "BOMBA NUCLEAR",
        [STR_CHEAT_TITLE] = "MENU SECRETO",
        [STR_CHEAT_SUBTITLE] = "ESTAS EN EL MENU SECRETO",
        [STR_CHEAT_SUBTITLE2] = "SI TIENES UN CODIGO SECRETO PONLO AQUI:",
        [STR_CHEAT_PLACEHOLDER] = "ESCRIBE AQUI...",
        [STR_CHEAT_INVALID] = "ESTE CODIGO NO ES VALIDO",
        [STR_CHEAT_ALREADY_USED] = "ESTE CODIGO YA HA SIDO CANJEADO",
        [STR_CHEAT_SUCCESS] = "!CODIGO ACTIVADO!",
        [STR_MENU_SECRETS] = "SECRETOS",
        [STR_TOUCH_CONNECTED] = "TACTIL CONECTADO",
        [STR_TOUCH_BACK] = "< TOCA AQUI PARA VOLVER >",
        [STR_ACH_HINT] = "TOCA UN LOGRO PARA VER DETALLES",
        [STR_ACH_STATUS_UNLOCKED] = "ESTADO: DESBLOQUEADO (OK)",
        [STR_ACH_PROGRESS_LBL] = "PROGRESO",
        [STR_CTRL_MOVE] = "MOVER NAVE",
        [STR_TOUCH_INFO_TITLE] = "CONTROLES TACTILES"
    },
    [1] = { // ENGLISH
        [STR_PLAY] = "PLAY", [STR_OPTIONS] = "OPTIONS", [STR_RECORDS] = "HIGHSCORES", [STR_VOL_BGM] = "BGM VOLUME",
        [STR_VOL_SFX] = "SFX VOLUME", [STR_FILTER] = "FILTER", [STR_CONTROLS] = "CONTROLS", [STR_RESOLUTION] = "RES",
        [STR_SCREEN_MODE] = "MODE", [STR_FPS] = "FPS", [STR_VSYNC] = "VSYNC", [STR_LANGUAGE] = "LANGUAGE",
        [STR_DELETE_RECORDS] = "CLEAR RECORDS", [STR_CONFIRM_DELETE_TITLE] = "CLEAR RECORDS?", [STR_CONFIRM_DELETE_SUB] = "IRREVERSIBLE ACTION",
        [STR_CANCEL] = "NO, CANCEL", [STR_CONFIRM_DELETE_BTN] = "YES, CLEAR", [STR_DIFF_TITLE] = "DIFFICULTY",
        [STR_DIFF_EASY] = "EASY", [STR_DIFF_NORMAL] = "NORMAL", [STR_DIFF_HARD] = "HARD", [STR_DIFF_HELL] = "HELL",
        [STR_PAUSE] = "PAUSE", [STR_RESUME] = "RESUME", [STR_MAIN_MENU] = "MAIN MENU", [STR_GAME_OVER] = "GAME OVER",
        [STR_POINTS] = "POINTS", [STR_RETRY] = "RETRY", [STR_QUIT_PROMPT] = "QUIT GAME?",
        [STR_QUIT_SUB] = "DO YOU WANT TO QUIT?", [STR_YES] = "YES", [STR_NO] = "NO", [STR_CTRL_TITLE] = "CONTROLS",
        [STR_CTRL_UP] = "UP", [STR_CTRL_DOWN] = "DOWN", [STR_CTRL_LEFT] = "LEFT", [STR_CTRL_RIGHT] = "RIGHT",
        [STR_CTRL_SHOOT] = "SHOOT", [STR_CTRL_TURBO] = "TURBO", [STR_CTRL_AIM] = "AIM",
        [STR_CTRL_DASH] = "DASH", [STR_REBIND_TITLE] = "REBIND KEY", [STR_PRESS_KEY] = "PRESS KEY/BUTTON...",
        [STR_CANCEL_KEY] = "BKSP / ESC CANCEL", [STR_INVALID_ACTION] = "INVALID ACTION", [STR_DUPLICATE_KEY] = "BINDING ALREADY IN USE",
        [STR_USE_OTHER_KEY] = "USE A DIFFERENT INPUT", [STR_PRESS_BACK] = "PRESS %s TO RETURN", [STR_PRESENTS] = "PRESENTS...",
        [STR_READY] = "READY!", [STR_GO] = "GO!", [STR_WAVE] = "WAVE",
        [STR_GAMEPAD_CONNECTED] = "CONTROLLER CONNECTED", [STR_KEYBOARD_CONNECTED] = "KEYBOARD CONNECTED",
        [STR_FREEZE] = "FROZEN!", [STR_POISON] = "POISON!",
        [STR_LOGROS] = "ACHIEVEMENTS",
        [STR_ACH_1_TITLE] = "EASY RECRUIT", [STR_ACH_1_DESC] = "KILL 10 ENEMIES ON EASY",
        [STR_ACH_2_TITLE] = "NORMAL RECRUIT", [STR_ACH_2_DESC] = "KILL 10 ENEMIES ON NORMAL",
        [STR_ACH_3_TITLE] = "HARD RECRUIT", [STR_ACH_3_DESC] = "KILL 10 ENEMIES ON HARD",
        [STR_ACH_4_TITLE] = "HELL RECRUIT", [STR_ACH_4_DESC] = "KILL 10 ENEMIES ON HELL",
        [STR_ACH_5_TITLE] = "EASY VETERAN", [STR_ACH_5_DESC] = "KILL 100 ENEMIES ON EASY",
        [STR_ACH_6_TITLE] = "NORMAL VETERAN", [STR_ACH_6_DESC] = "KILL 100 ENEMIES ON NORMAL",
        [STR_ACH_7_TITLE] = "HARD VETERAN", [STR_ACH_7_DESC] = "KILL 100 ENEMIES ON HARD",
        [STR_ACH_8_TITLE] = "HELL VETERAN", [STR_ACH_8_DESC] = "KILL 100 ENEMIES ON HELL",
        [STR_ACH_9_TITLE] = "EASY MASTER", [STR_ACH_9_DESC] = "KILL 500 ENEMIES ON EASY",
        [STR_ACH_10_TITLE] = "NORMAL MASTER", [STR_ACH_10_DESC] = "KILL 500 ENEMIES ON NORMAL",
        [STR_ACH_11_TITLE] = "HARD MASTER", [STR_ACH_11_DESC] = "KILL 500 ENEMIES ON HARD",
        [STR_ACH_12_TITLE] = "HELL MASTER", [STR_ACH_12_DESC] = "KILL 500 ENEMIES ON HELL",
        [STR_ACH_13_TITLE] = "OVERHEAT", [STR_ACH_13_DESC] = "KILL 25 WITH HEAVY SHOT",
        [STR_ACH_14_TITLE] = "INTACT", [STR_ACH_14_DESC] = "CLEAR WAVE WITHOUT DAMAGE",
        [STR_ACH_15_TITLE] = "!BOOM!", [STR_ACH_15_DESC] = "KILL 5 WITH A NUKE BOMB",
        [STR_ACH_16_TITLE] = "DEEP FREEZE", [STR_ACH_16_DESC] = "FREEZE 10 ENEMIES",
        [STR_ACH_17_TITLE] = "TOXICITY", [STR_ACH_17_DESC] = "POISON 10 ENEMIES",
        [STR_ACH_18_TITLE] = "ELEMENTAL TRIAD", [STR_ACH_18_DESC] = "AFFECT 300 WITH ICE, POISON OR WATER IN CLASSIC",
        [STR_ACH_19_TITLE] = "DEAD TIME", [STR_ACH_19_DESC] = "5 KILLS IN SLOW-MO",
        [STR_ACH_20_TITLE] = "POWER UP", [STR_ACH_20_DESC] = "PICK POWER-UP FOR FIRST TIME",
        [STR_ACH_21_TITLE] = "UNTOUCHABLE", [STR_ACH_21_DESC] = "30S ON HELL NO DAMAGE",
        [STR_ACH_22_TITLE] = "BOSS HUNTER", [STR_ACH_22_DESC] = "DESTROY 5 BOSSES",
        [STR_ACH_23_TITLE] = "TELEPORTATION", [STR_ACH_23_DESC] = "DESTROY 10 TELEPORTERS",
        [STR_ACH_24_TITLE] = "LAST BREATH", [STR_ACH_24_DESC] = "1000 POINTS WITH 1 HEART",
        [STR_ACH_25_TITLE] = "FULL ARSENAL", [STR_ACH_25_DESC] = "USE ALL SHOT TYPES",
        [STR_ACH_26_TITLE] = "GOD MODE", [STR_ACH_26_DESC] = "X10 COMBO WITH POWER-UP",
        [STR_ACH_27_TITLE] = "TOO FAST", [STR_ACH_27_DESC] = "10 KILLS WITH RAPID FIRE",
        [STR_ACH_28_TITLE] = "PERFECT DODGE", [STR_ACH_28_DESC] = "DODGE 10 BULLETS WITH DASH",
        [STR_ACH_29_TITLE] = "TOTAL CLEAR", [STR_ACH_29_DESC] = "NUKE BOMB WITH 10+ ENEMIES",
        [STR_ACH_30_TITLE] = "BLIND DRUNK", [STR_ACH_30_DESC] = "DRINK THE GREEN BOTTLE",
        [STR_ACH_31_TITLE] = "UNSCRATCHED", [STR_ACH_31_DESC] = "SURVIVE 3 WAVES NO DAMAGE",
        [STR_ACH_32_TITLE] = "LAST SURVIVOR", [STR_ACH_32_DESC] = "10000 PTS WITH 1 HEART",
        [STR_ACH_33_TITLE] = "UPSTREAM", [STR_ACH_33_DESC] = "KILL 10 WHILE DRUNK",
        [STR_ACH_34_TITLE] = "STEP ON IT", [STR_ACH_34_DESC] = "KILL KAMIKAZE POINT BLANK",
        [STR_ACH_35_TITLE] = "NOT ONE STEP BACK", [STR_ACH_35_DESC] = "BEAT A BOSS NO DASH",
        [STR_ACH_36_TITLE] = "BULLET DANCER", [STR_ACH_36_DESC] = "DODGE 25 BULLETS IN A ROW",
        [STR_ACH_37_TITLE] = "DIVIDE AND CONQUER", [STR_ACH_37_DESC] = "KILL DIVIDER AND BOTH MINIS",
        [STR_ACH_38_TITLE] = "KABOOM", [STR_ACH_38_DESC] = "KAMIKAZE DESTROYS ENEMY",
        [STR_ACH_39_TITLE] = "WHERE IS HE?", [STR_ACH_39_DESC] = "KILL TELEPORT RIGHT AFTER WARP",
        [STR_ACH_40_TITLE] = "NOT THAT HARD", [STR_ACH_40_DESC] = "CLEAR HARD WAVE WITHOUT POWERUPS",
        [STR_ACH_41_TITLE] = "PURE HELL", [STR_ACH_41_DESC] = "REACH WAVE 15 ON HELL",
        [STR_ACH_42_TITLE] = "ONE LIFE", [STR_ACH_42_DESC] = "KILL HELL BOSS WITH 1 HEART",
        [STR_ACH_43_TITLE] = "NO ASSISTANCE", [STR_ACH_43_DESC] = "5000 PTS ON HELL NO POWERUPS",
        [STR_ACH_44_TITLE] = "RUSH ROOKIE", [STR_ACH_44_DESC] = "BEAT FIRST WAVE IN RUSH MODE",
        [STR_ACH_45_TITLE] = "RUSH MASTER", [STR_ACH_45_DESC] = "BEAT FOURTH WAVE IN RUSH MODE",
        [STR_ACH_46_TITLE] = "RUSH EXPERT", [STR_ACH_46_DESC] = "BEAT FIRST BOSS WAVE IN RUSH",
        [STR_ACH_47_TITLE] = "WHAT ARE YOU DOING HERE?", [STR_ACH_47_DESC] = "BEAT SECOND BOSS WAVE IN RUSH",
        [STR_ACH_48_TITLE] = "UNTOUCHED", [STR_ACH_48_DESC] = "CLEAR A WAVE NO DAMAGE IN RUSH",
        [STR_ACH_49_TITLE] = "MAX POISON", [STR_ACH_49_DESC] = "POISON 500 ENEMIES IN RUSH",
        [STR_ACH_50_TITLE] = "MAX FREEZE", [STR_ACH_50_DESC] = "FREEZE 500 ENEMIES IN RUSH",
        [STR_ACH_51_TITLE] = "MAX SOAKED", [STR_ACH_51_DESC] = "SOAK 500 ENEMIES IN RUSH",
        [STR_ACH_52_TITLE] = "OVERFLOWING", [STR_ACH_52_DESC] = "ACCUMULATE 5 HEARTS IN RUSH",
        [STR_ACH_53_TITLE] = "POWERFUL", [STR_ACH_53_DESC] = "USE 60 POWER-UPS IN RUSH",
        [STR_ACH_54_TITLE] = "SUDDEN DEATH", [STR_ACH_54_DESC] = "ACCUMULATE 50 DEATHS IN RUSH",
        [STR_ACH_55_TITLE] = "ONE MINUTE OF GLORY", [STR_ACH_55_DESC] = "SURVIVE 1 MINUTE IN TIME ATTACK",
        [STR_ACH_56_TITLE] = "TIME SURVIVOR", [STR_ACH_56_DESC] = "SURVIVE 3 MINUTES IN TIME ATTACK",
        [STR_ACH_57_TITLE] = "CHRONOS MASTER", [STR_ACH_57_DESC] = "SURVIVE 5 MINUTES IN TIME ATTACK",
        [STR_ACH_58_TITLE] = "MAXED CLOCK", [STR_ACH_58_DESC] = "ACCUMULATE 90 SECONDS ON CLOCK",
        [STR_ACH_59_TITLE] = "TEMPORAL HUNTER", [STR_ACH_59_DESC] = "KILL 100 ENEMIES IN TIME ATTACK",
        [STR_ACH_60_TITLE] = "SWIFT DESTRUCTION", [STR_ACH_60_DESC] = "DEFEAT A BOSS IN TIME ATTACK",
        [STR_ACH_61_TITLE] = "ASSAULT RECRUIT", [STR_ACH_61_DESC] = "SURVIVE 1 MINUTE IN ASSAULT MODE",
        [STR_ACH_62_TITLE] = "ASSAULT VETERAN", [STR_ACH_62_DESC] = "SURVIVE 3 MINUTES IN ASSAULT MODE",
        [STR_ACH_63_TITLE] = "ASSAULT MASTER", [STR_ACH_63_DESC] = "SURVIVE 5 MINUTES IN ASSAULT MODE",
        [STR_ACH_64_TITLE] = "UNSTOPPABLE GUNNER", [STR_ACH_64_DESC] = "KILL 100 ENEMIES IN ASSAULT MODE",
        [STR_ACH_65_TITLE] = "ASSAULT DUEL", [STR_ACH_65_DESC] = "DEFEAT A BOSS IN ASSAULT MODE",
        [STR_ACH_66_TITLE] = "LETHAL ROULETTE", [STR_ACH_66_DESC] = "KILL ENEMIES WITH 5 WEAPONS IN ASSAULT",
        [STR_ACH_67_TITLE] = "LEGEND", [STR_ACH_67_DESC] = "UNLOCK ALL OTHER ACHIEVEMENTS",
        [STR_NEW_ACHIEVEMENT] = "NEW ACHIEVEMENT!",
        [STR_ACH_DETAILS_HINT] = "SELECT AN ACHIEVEMENT TO VIEW DETAILS",
        [STR_STATUS_UNLOCKED] = "STATUS: UNLOCKED (OK)",
        [STR_PROGRESS] = "PROGRESS",
        [STR_MODE_WINDOWED] = "WINDOWED",
        [STR_MODE_BORDERLESS] = "BORDERLESS",
        [STR_GAME_MODE_TITLE] = "GAME MODES",
        [STR_MODE_CLASSIC] = "CLASSIC",
        [STR_MODE_RUSH] = "RUSH",
        [STR_MODE_TIME_ATTACK] = "TIME ATTACK",
        [STR_MODE_COMING_SOON] = "COMING SOON",
        [STR_RUSH_UNAVAILABLE_TITLE] = "UNDER DEVELOPMENT",
        [STR_RUSH_UNAVAILABLE_DESC] = "THIS MODE WILL BE AVAILABLE SOON",
        [STR_CLOSE_HINT] = "PRESS ANY KEY TO CLOSE",
        [STR_MAX_SCORE] = "MAX SCORE",
        [STR_MAX_WAVE] = "MAX WAVE",
        [STR_TOTAL_KILLS] = "TOTAL KILLS",
        [STR_MAX_TIME] = "BEST TIME",
        [STR_TIME_UP] = "TIME'S UP!",
        [STR_TIME_LEFT] = "TIME",
        [STR_WET] = "!SOAKED!",
        [STR_KONAMI_TITLE] = "CONGRATULATIONS MASTER!",
        [STR_KONAMI_SUB1] = "YOU ARE A TRUE GAMING FAN",
        [STR_KONAMI_SUB2] = "BOTH RETRO AND MODERN!",
        [STR_KONAMI_SUB3] = "+2 EXTRA LIVES ON YOUR NEXT HELL RUN!",
        [STR_MODE_ASSAULT] = "ASSAULT",
        [STR_ASSAULT_CHANGE] = "!WEAPON CHANGE!",
        [STR_ASSAULT_NEXT] = "CHANGE",
        [STR_PU_SHIELD_NAME] = "SHIELD ACTIVE",
        [STR_PU_RAPID_NAME] = "RAPID FIRE",
        [STR_PU_TRIPLE_NAME] = "TRIPLE SHOT",
        [STR_PU_HEAVY_NAME] = "HEAVY SHOT",
        [STR_PU_STAR_NAME] = "INVINCIBILITY",
        [STR_PU_SLOWMO_NAME] = "SLOW-MOTION",
        [STR_PU_FREEZE_NAME] = "FREEZE SHOT",
        [STR_PU_POISON_NAME] = "POISON SHOT",
        [STR_PU_WATER_NAME] = "WATER SHOT",
        [STR_PU_NUKE_NAME] = "NUCLEAR BOMB",
        [STR_CHEAT_TITLE] = "SECRET MENU",
        [STR_CHEAT_SUBTITLE] = "YOU ARE IN THE SECRET MENU",
        [STR_CHEAT_SUBTITLE2] = "IF YOU HAVE A SECRET CODE ENTER IT HERE:",
        [STR_CHEAT_PLACEHOLDER] = "TYPE HERE...",
        [STR_CHEAT_INVALID] = "INVALID CODE",
        [STR_CHEAT_ALREADY_USED] = "CODE ALREADY REDEEMED",
        [STR_CHEAT_SUCCESS] = "CODE ACTIVATED!",
        [STR_MENU_SECRETS] = "SECRETS",
        [STR_TOUCH_CONNECTED] = "TOUCH CONNECTED",
        [STR_TOUCH_BACK] = "< TAP HERE TO GO BACK >",
        [STR_ACH_HINT] = "TAP AN ACHIEVEMENT FOR DETAILS",
        [STR_ACH_STATUS_UNLOCKED] = "STATUS: UNLOCKED (OK)",
        [STR_ACH_PROGRESS_LBL] = "PROGRESS",
        [STR_CTRL_MOVE] = "MOVE SHIP",
        [STR_TOUCH_INFO_TITLE] = "TOUCH CONTROLS"
    },
    [2] = { // FRANÇAIS
        [STR_PLAY] = "JOUER", [STR_OPTIONS] = "OPTIONS", [STR_RECORDS] = "RECORDS", [STR_VOL_BGM] = "VOL. MUSIQUE",
        [STR_VOL_SFX] = "VOL. BRUITS", [STR_FILTER] = "FILTRE", [STR_CONTROLS] = "COMMANDES", [STR_RESOLUTION] = "RES",
        [STR_SCREEN_MODE] = "MODE", [STR_FPS] = "FPS", [STR_VSYNC] = "VSYNC", [STR_LANGUAGE] = "LANGUE",
        [STR_DELETE_RECORDS] = "EFFACER RECORDS", [STR_CONFIRM_DELETE_TITLE] = "EFFACER RECORDS?", [STR_CONFIRM_DELETE_SUB] = "ACTION IRREVERSIBLE",
        [STR_CANCEL] = "NON, ANNULER", [STR_CONFIRM_DELETE_BTN] = "OUI, EFFACER", [STR_DIFF_TITLE] = "DIFFICULTE",
        [STR_DIFF_EASY] = "FACILE", [STR_DIFF_NORMAL] = "NORMAL", [STR_DIFF_HARD] = "DIFFICILE", [STR_DIFF_HELL] = "ENFER",
        [STR_PAUSE] = "PAUSE", [STR_RESUME] = "REPRENDRE", [STR_MAIN_MENU] = "MENU PRINCIPAL", [STR_GAME_OVER] = "GAME OVER",
        [STR_POINTS] = "POINTS", [STR_RETRY] = "REESSAYER", [STR_QUIT_PROMPT] = "QUITTER LE JEU?",
        [STR_QUIT_SUB] = "VOULEZ-VOUS QUITTER?", [STR_YES] = "OUI", [STR_NO] = "NON", [STR_CTRL_TITLE] = "COMMANDES",
        [STR_CTRL_UP] = "HAUT", [STR_CTRL_DOWN] = "BAS", [STR_CTRL_LEFT] = "GAUCHE", [STR_CTRL_RIGHT] = "DROITE",
        [STR_CTRL_SHOOT] = "TIRER", [STR_CTRL_TURBO] = "TURBO", [STR_CTRL_AIM] = "VISER",
        [STR_CTRL_DASH] = "DASH", [STR_REBIND_TITLE] = "CHANGER TOUCHE", [STR_PRESS_KEY] = "APPUYEZ TOUCHE/BOUTON...",
        [STR_CANCEL_KEY] = "BKSP / ESC ANNULER", [STR_INVALID_ACTION] = "ACTION INVALIDE", [STR_DUPLICATE_KEY] = "ENTREE DEJA ASSIGNEE",
        [STR_USE_OTHER_KEY] = "UTILISEZ UNE AUTRE", [STR_PRESS_BACK] = "APPUYEZ %s POUR RETOUR", [STR_PRESENTS] = "PRESENTE...",
        [STR_READY] = "PRET!", [STR_GO] = "PARTEZ!", [STR_WAVE] = "VAGUE",
        [STR_GAMEPAD_CONNECTED] = "MANETTE DETECTEE", [STR_KEYBOARD_CONNECTED] = "CLAVIER DETECTE",
        [STR_FREEZE] = "GEL!", [STR_POISON] = "POISON!",
        [STR_LOGROS] = "SUCCES",
        [STR_ACH_1_TITLE] = "RECRUE FACILE", [STR_ACH_1_DESC] = "ELIMINER 10 ENNEMIS EN FACILE",
        [STR_ACH_2_TITLE] = "RECRUE NORMALE", [STR_ACH_2_DESC] = "ELIMINER 10 ENNEMIS EN NORMAL",
        [STR_ACH_3_TITLE] = "RECRUE DIFFICILE", [STR_ACH_3_DESC] = "ELIMINER 10 ENNEMIS EN DIFFICILE",
        [STR_ACH_4_TITLE] = "RECRUE ENFER", [STR_ACH_4_DESC] = "ELIMINER 10 ENNEMIS EN ENFER",
        [STR_ACH_5_TITLE] = "VETERAN FACILE", [STR_ACH_5_DESC] = "ELIMINER 100 ENNEMIS EN FACILE",
        [STR_ACH_6_TITLE] = "VETERAN NORMAL", [STR_ACH_6_DESC] = "ELIMINER 100 ENNEMIS EN NORMAL",
        [STR_ACH_7_TITLE] = "VETERAN DIFFICILE", [STR_ACH_7_DESC] = "ELIMINER 100 ENNEMIS EN DIFFICILE",
        [STR_ACH_8_TITLE] = "VETERAN ENFER", [STR_ACH_8_DESC] = "ELIMINER 100 ENNEMIS EN ENFER",
        [STR_ACH_9_TITLE] = "MAITRE FACILE", [STR_ACH_9_DESC] = "ELIMINER 500 ENNEMIS EN FACILE",
        [STR_ACH_10_TITLE] = "MAITRE NORMAL", [STR_ACH_10_DESC] = "ELIMINER 500 ENNEMIS EN NORMAL",
        [STR_ACH_11_TITLE] = "MAITRE DIFFICILE", [STR_ACH_11_DESC] = "ELIMINER 500 ENNEMIS EN DIFFICILE",
        [STR_ACH_12_TITLE] = "MAITRE ENFER", [STR_ACH_12_DESC] = "ELIMINER 500 ENNEMIS EN ENFER",
        [STR_ACH_13_TITLE] = "SURCHAUFFE", [STR_ACH_13_DESC] = "ELIMINER 25 AVEC TIR LOURD",
        [STR_ACH_14_TITLE] = "INTACT", [STR_ACH_14_DESC] = "TERMINER UNE VAGUE SANS DEGAT",
        [STR_ACH_15_TITLE] = "!BOOM!", [STR_ACH_15_DESC] = "ELIMINER 5 AVEC UNE NUKE",
        [STR_ACH_16_TITLE] = "GRAND FROID", [STR_ACH_16_DESC] = "GELER 10 ENNEMIS",
        [STR_ACH_17_TITLE] = "TOXICITE", [STR_ACH_17_DESC] = "EMPOISONNER 10 ENNEMIS",
        [STR_ACH_18_TITLE] = "TEMPS MORT", [STR_ACH_18_DESC] = "5 ELIMINATIONS EN SLOW-MO",
        [STR_ACH_19_TITLE] = "SURPUISSANCE", [STR_ACH_19_DESC] = "PRENDRE UN POWER-UP 1ERE FOIS",
        [STR_ACH_20_TITLE] = "INTOUCHABLE", [STR_ACH_20_DESC] = "30S EN ENFER SANS PRENDRE DEGAT",
        [STR_ACH_21_TITLE] = "CHASSEUR BOSS", [STR_ACH_21_DESC] = "DETRUIRE 5 BOSS",
        [STR_ACH_22_TITLE] = "TELEPORTATION", [STR_ACH_22_DESC] = "DETRUIRE 10 TELEPORTEURS",
        [STR_ACH_23_TITLE] = "DERNIER SOUFFLE", [STR_ACH_23_DESC] = "1000 POINTS AVEC 1 COEUR",
        [STR_ACH_24_TITLE] = "FULL ARSENAL", [STR_ACH_24_DESC] = "UTILISER TOUS LES TIRS",
        [STR_ACH_25_TITLE] = "MODE DIEU", [STR_ACH_25_DESC] = "COMBO X10 AVEC POWER-UP",
        [STR_ACH_26_TITLE] = "TROP RAPIDE", [STR_ACH_26_DESC] = "10 KILLS AVEC TIR RAPIDE",
        [STR_ACH_27_TITLE] = "ESQUIVE PARFAITE", [STR_ACH_27_DESC] = "ESQUIVER 10 TIRS AVEC DASH",
        [STR_ACH_28_TITLE] = "NETTOYAGE TOTAL", [STR_ACH_28_DESC] = "NUKE AVEC 10+ ENNEMIS",
        [STR_ACH_29_TITLE] = "COMPLETEMENT IVRE", [STR_ACH_29_DESC] = "BOIRE LA BOUTEILLE VERTE",
        [STR_ACH_30_TITLE] = "SANS RAYURE", [STR_ACH_30_DESC] = "SURVIVRE 3 VAGUES SANS DEGATS",
        [STR_ACH_31_TITLE] = "DERNIER SURVIVANT", [STR_ACH_31_DESC] = "10000 PTS AVEC 1 COEUR",
        [STR_ACH_32_TITLE] = "A CONTRE-COURANT", [STR_ACH_32_DESC] = "TUER 10 ENNEMIS ETANT IVRE",
        [STR_ACH_33_TITLE] = "ECRASE-LE", [STR_ACH_33_DESC] = "TUER KAMIKAZE A BOUT PORTANT",
        [STR_ACH_34_TITLE] = "PAS UN PAS EN ARRIERE", [STR_ACH_34_DESC] = "BATTRE UN BOSS SANS DASH",
        [STR_ACH_35_TITLE] = "DANSEUR DE BALLES", [STR_ACH_35_DESC] = "ESQUIVER 25 TIRS D AFFILEE",
        [STR_ACH_36_TITLE] = "DIVISER ET REGNER", [STR_ACH_36_DESC] = "TUER DIVISEUR ET SES 2 FRAGMENTS",
        [STR_ACH_37_TITLE] = "KABOOM", [STR_ACH_37_DESC] = "KAMIKAZE DETRUIT UN ENNEMI",
        [STR_ACH_38_TITLE] = "OU EST-IL?", [STR_ACH_38_DESC] = "TUER TELEPORT APRES UN WARP",
        [STR_ACH_39_TITLE] = "PAS SI DUR", [STR_ACH_39_DESC] = "PASSER VAGUE DIFFICILE SANS BONUS",
        [STR_ACH_40_TITLE] = "ENFER PUR", [STR_ACH_40_DESC] = "ATTEINDRE VAGUE 15 EN ENFER",
        [STR_ACH_41_TITLE] = "A UNE VIE", [STR_ACH_41_DESC] = "BATTRE BOSS EN ENFER AVEC 1 PV",
        [STR_ACH_42_TITLE] = "SANS AIDE", [STR_ACH_42_DESC] = "5000 PTS EN ENFER SANS BONUS",
        [STR_ACH_43_TITLE] = "RECRUE DU RUSH", [STR_ACH_43_DESC] = "PASSER LA PREMIERE VAGUE EN RUSH",
        [STR_ACH_44_TITLE] = "MAITRE DU RUSH", [STR_ACH_44_DESC] = "PASSER LA QUATRIEME VAGUE EN RUSH",
        [STR_ACH_45_TITLE] = "EXPERT DU RUSH", [STR_ACH_45_DESC] = "BATTRE LA PREMIERE VAGUE BOSS",
        [STR_ACH_46_TITLE] = "QUE FAIS-TU ICI?", [STR_ACH_46_DESC] = "BATTRE LA DEUXIEME VAGUE BOSS",
        [STR_ACH_47_TITLE] = "INTACT", [STR_ACH_47_DESC] = "PASSER VAGUE SANS DEGATS EN RUSH",
        [STR_ACH_48_TITLE] = "POISON MAX", [STR_ACH_48_DESC] = "EMPOISONNER 500 ENNEMIS EN RUSH",
        [STR_ACH_49_TITLE] = "GLACE MAX", [STR_ACH_49_DESC] = "GELER 500 ENNEMIS EN MODE RUSH",
        [STR_ACH_50_TITLE] = "PLEIN A CRAQUER", [STR_ACH_50_DESC] = "ACCUMULER 5 COEURS EN RUSH",
        [STR_ACH_51_TITLE] = "PUISSANT", [STR_ACH_51_DESC] = "UTILISER 60 POWER-UPS EN RUSH",
        [STR_ACH_52_TITLE] = "MORT SUBITE", [STR_ACH_52_DESC] = "ACCUMULER 50 MORTS EN MODE RUSH",
        [STR_ACH_53_TITLE] = "MINUTE DE GLOIRE", [STR_ACH_53_DESC] = "SURVIVRE 1 MIN EN CONTRE-LA-MONTRE",
        [STR_ACH_54_TITLE] = "SURVIVANT DU TEMPS", [STR_ACH_54_DESC] = "SURVIVRE 3 MIN EN CONTRE-LA-MONTRE",
        [STR_ACH_55_TITLE] = "MAITRE DU CHRONO", [STR_ACH_55_DESC] = "SURVIVRE 5 MIN EN CONTRE-LA-MONTRE",
        [STR_ACH_56_TITLE] = "HORLOGE PLEINE", [STR_ACH_56_DESC] = "ACCUMULER 90 SECONDES AU CHRONO",
        [STR_ACH_58_TITLE] = "DESTRUCTION RAPIDE", [STR_ACH_58_DESC] = "BATTRE UN BOSS EN CONTRE-LA-MONTRE",
        [STR_ACH_59_TITLE] = "CHASSEUR TEMPOREL", [STR_ACH_59_DESC] = "TUER 100 ENNEMIS EN CHRONO",
        [STR_ACH_60_TITLE] = "DESTRUCTION RAPIDE", [STR_ACH_60_DESC] = "BATTRE UN BOSS EN CHRONO",
        [STR_ACH_61_TITLE] = "RECRUE DE L'ASSAUT", [STR_ACH_61_DESC] = "SURVIVRE 1 MINUTE EN MODE ASSAUT",
        [STR_ACH_62_TITLE] = "VETERAN DE L'ASSAUT", [STR_ACH_62_DESC] = "SURVIVRE 3 MINUTES EN MODE ASSAUT",
        [STR_ACH_63_TITLE] = "MAITRE DE L'ASSAUT", [STR_ACH_63_DESC] = "SURVIVRE 5 MINUTES EN MODE ASSAUT",
        [STR_ACH_64_TITLE] = "ARTILLEUR IMPARABLE", [STR_ACH_64_DESC] = "TUER 100 ENNEMIS EN MODE ASSAUT",
        [STR_ACH_65_TITLE] = "DUEL D'ASSAUT", [STR_ACH_65_DESC] = "BATTRE UN BOSS EN MODE ASSAUT",
        [STR_ACH_66_TITLE] = "ROULETTE MORTELLE", [STR_ACH_66_DESC] = "TUER AVEC 5 ARMES EN ASSAUT",
        [STR_ACH_67_TITLE] = "LEGENDE", [STR_ACH_67_DESC] = "DEVERROUILLER TOUS LES SUCCES",
        [STR_NEW_ACHIEVEMENT] = "NOUVEAU SUCCES!",
        [STR_ACH_DETAILS_HINT] = "CHOISIR UN SUCCES POUR VOIR DETAILS",
        [STR_STATUS_UNLOCKED] = "STATUT: DEVERROUILLE (OK)",
        [STR_PROGRESS] = "PROGRES",
        [STR_MODE_WINDOWED] = "FENETRE",
        [STR_MODE_BORDERLESS] = "SANS BORD",
        [STR_GAME_MODE_TITLE] = "MODES DE JEU",
        [STR_MODE_CLASSIC] = "CLASSIQUE",
        [STR_MODE_RUSH] = "RUSH",
        [STR_MODE_TIME_ATTACK] = "CHRONO",
        [STR_MODE_COMING_SOON] = "BIENTOT",
        [STR_RUSH_UNAVAILABLE_TITLE] = "EN DEVELOPPEMENT",
        [STR_RUSH_UNAVAILABLE_DESC] = "CE MODE SERA BIENTOT DISPONIBLE",
        [STR_CLOSE_HINT] = "APPUYEZ SUR UNE TOUCHE POUR FERMER",
        [STR_MAX_SCORE] = "SCORE MAX",
        [STR_MAX_WAVE] = "VAGUE MAX",
        [STR_TOTAL_KILLS] = "ENNEMIS K.O.",
        [STR_MAX_TIME] = "TEMPS MAX",
        [STR_TIME_UP] = "TEMPS ECOULE!",
        [STR_TIME_LEFT] = "TEMPS",
        [STR_CHEAT_TITLE] = "MENU SECRET",
        [STR_CHEAT_SUBTITLE] = "VOUS ETES DANS LE MENU SECRET",
        [STR_CHEAT_SUBTITLE2] = "ENTREZ VOTRE CODE SECRET ICI:",
        [STR_CHEAT_PLACEHOLDER] = "ECRIVEZ ICI...",
        [STR_CHEAT_INVALID] = "CODE INVALIDE",
        [STR_CHEAT_ALREADY_USED] = "CODE DEJA UTILISE",
        [STR_CHEAT_SUCCESS] = "CODE ACTIVE!",
        [STR_MENU_SECRETS] = "SECRETS",
        [STR_TOUCH_CONNECTED] = "TACTILE CONNECTE",
        [STR_TOUCH_BACK] = "< TOUCHEZ POUR RETOUR >",
        [STR_ACH_HINT] = "TOUCHEZ UN SUCCES POUR DETAILS",
        [STR_ACH_STATUS_UNLOCKED] = "STATUT: DEVERROUILLE (OK)",
        [STR_ACH_PROGRESS_LBL] = "PROGRES",
        [STR_CTRL_MOVE] = "BOUGER VAISSEAU",
        [STR_TOUCH_INFO_TITLE] = "COMMANDES TACTILES"
    },
    [3] = { // ITALIANO
        [STR_PLAY] = "GIOCA", [STR_OPTIONS] = "OPZIONI", [STR_RECORDS] = "RECORD", [STR_VOL_BGM] = "VOL. MUSICA",
        [STR_VOL_SFX] = "VOL. EFFETTI", [STR_FILTER] = "FILTRO", [STR_CONTROLS] = "COMANDI", [STR_RESOLUTION] = "RES",
        [STR_SCREEN_MODE] = "MODO", [STR_FPS] = "FPS", [STR_VSYNC] = "VSYNC", [STR_LANGUAGE] = "LINGUA",
        [STR_DELETE_RECORDS] = "CANCELLA RECORD", [STR_CONFIRM_DELETE_TITLE] = "CANCELLA RECORD?", [STR_CONFIRM_DELETE_SUB] = "AZIONE IRREVERSIBILE",
        [STR_CANCEL] = "NO, ANNULLA", [STR_CONFIRM_DELETE_BTN] = "SI, CANCELLA", [STR_DIFF_TITLE] = "DIFFICOLTA",
        [STR_DIFF_EASY] = "FACILE", [STR_DIFF_NORMAL] = "NORMALE", [STR_DIFF_HARD] = "DIFFICILE", [STR_DIFF_HELL] = "INFERNO",
        [STR_PAUSE] = "PAUSA", [STR_RESUME] = "RIPRENDI", [STR_MAIN_MENU] = "MENU PRINCIPALE", [STR_GAME_OVER] = "GAME OVER",
        [STR_POINTS] = "PUNTI", [STR_RETRY] = "RIPROVA", [STR_QUIT_PROMPT] = "USCIRE DAL GIOCO?",
        [STR_QUIT_SUB] = "VUOI USCIRE ORA?", [STR_YES] = "SI", [STR_NO] = "NO", [STR_CTRL_TITLE] = "COMANDI",
        [STR_CTRL_UP] = "SU", [STR_CTRL_DOWN] = "GIU", [STR_CTRL_LEFT] = "SINISTRA", [STR_CTRL_RIGHT] = "DESTRA",
        [STR_CTRL_SHOOT] = "SPARA", [STR_CTRL_TURBO] = "TURBO", [STR_CTRL_AIM] = "MIRA",
        [STR_CTRL_DASH] = "SCATTO", [STR_REBIND_TITLE] = "RIASSEGNA TASTO", [STR_PRESS_KEY] = "PREMI TASTO/PULSANTE...",
        [STR_CANCEL_KEY] = "BKSP / ESC ANNULLA", [STR_INVALID_ACTION] = "AZIONE INVALIDA", [STR_DUPLICATE_KEY] = "PULSANTE GIA ASSEGNATO",
        [STR_USE_OTHER_KEY] = "USA UN ALTRO TASTO", [STR_PRESS_BACK] = "PREMI %s PER TORNARE", [STR_PRESENTS] = "PRESENTA...",
        [STR_READY] = "PRONTO!", [STR_GO] = "VIA!", [STR_WAVE] = "ONDATA",
        [STR_GAMEPAD_CONNECTED] = "CONTROLLER RILEVATO", [STR_KEYBOARD_CONNECTED] = "TASTIERA RILEVATA",
        [STR_FREEZE] = "GELATO!", [STR_POISON] = "VELENO!",
        [STR_LOGROS] = "SUCCESSI",
        [STR_ACH_1_TITLE] = "RECLUTA FACILE", [STR_ACH_1_DESC] = "ELIMINA 10 NEMICI A FACILE",
        [STR_ACH_2_TITLE] = "RECLUTA NORMALE", [STR_ACH_2_DESC] = "ELIMINA 10 NEMICI A NORMALE",
        [STR_ACH_3_TITLE] = "RECLUTA DIFFICILE", [STR_ACH_3_DESC] = "ELIMINA 10 NEMICI A DIFFICILE",
        [STR_ACH_4_TITLE] = "RECLUTA INFERNO", [STR_ACH_4_DESC] = "ELIMINA 10 NEMICI AD INFERNO",
        [STR_ACH_5_TITLE] = "VETERANO FACILE", [STR_ACH_5_DESC] = "ELIMINA 100 NEMICI A FACILE",
        [STR_ACH_6_TITLE] = "VETERANO NORMALE", [STR_ACH_6_DESC] = "ELIMINA 100 NEMICI A NORMALE",
        [STR_ACH_7_TITLE] = "VETERANO DIFFICILE", [STR_ACH_7_DESC] = "ELIMINA 100 NEMICI A DIFFICILE",
        [STR_ACH_8_TITLE] = "VETERANO INFERNO", [STR_ACH_8_DESC] = "ELIMINA 100 NEMICI AD INFERNO",
        [STR_ACH_9_TITLE] = "MAESTRO FACILE", [STR_ACH_9_DESC] = "ELIMINA 500 NEMICI A FACILE",
        [STR_ACH_10_TITLE] = "MAESTRO NORMALE", [STR_ACH_10_DESC] = "ELIMINA 500 NEMICI A NORMALE",
        [STR_ACH_11_TITLE] = "MAESTRO DIFFICILE", [STR_ACH_11_DESC] = "ELIMINA 500 NEMICI A DIFFICILE",
        [STR_ACH_12_TITLE] = "MAESTRO INFERNO", [STR_ACH_12_DESC] = "ELIMINA 500 NEMICI AD INFERNO",
        [STR_ACH_13_TITLE] = "FUOCO PESANTE", [STR_ACH_13_DESC] = "ELIMINA 25 CON COLPO PESANTE",
        [STR_ACH_14_TITLE] = "INTATTO", [STR_ACH_14_DESC] = "COMPLETA ONDATA SENZA DANNI",
        [STR_ACH_15_TITLE] = "!BOOM!", [STR_ACH_15_DESC] = "ELIMINA 5 CON UNA BOMBA NUKE",
        [STR_ACH_16_TITLE] = "GELIDO", [STR_ACH_16_DESC] = "CONGELA 10 NEMICI",
        [STR_ACH_17_TITLE] = "TOSSICITA", [STR_ACH_17_DESC] = "AVVELENA 10 NEMICI",
        [STR_ACH_18_TITLE] = "TEMPO MORTO", [STR_ACH_18_DESC] = "ELIMINA 5 IN SLOW-MO",
        [STR_ACH_19_TITLE] = "POTENZIATO", [STR_ACH_19_DESC] = "PRENDI POWER-UP PRIMA VOLTA",
        [STR_ACH_20_TITLE] = "INTOCCABILE", [STR_ACH_20_DESC] = "30S AD INFERNO SENZA DANNI",
        [STR_ACH_21_TITLE] = "CACCIATORE BOSS", [STR_ACH_21_DESC] = "DISTRUGGI 5 BOSS",
        [STR_ACH_22_TITLE] = "TELETRASPORTO", [STR_ACH_22_DESC] = "DISTRUGGI 10 TELETRASPORTI",
        [STR_ACH_23_TITLE] = "ULTIMO RESPIRO", [STR_ACH_23_DESC] = "1000 PUNTI CON 1 CUORE",
        [STR_ACH_24_TITLE] = "ARSENALE TOTALE", [STR_ACH_24_DESC] = "USA TUTTI I TIPI DI SPARO",
        [STR_ACH_25_TITLE] = "MODALITA DIO", [STR_ACH_25_DESC] = "COMBO X10 CON POWER-UP",
        [STR_ACH_26_TITLE] = "TROPPO VELOCE", [STR_ACH_26_DESC] = "10 UCCISIONI CON SPARO RAPIDO",
        [STR_ACH_27_TITLE] = "SCHIVATA PERFETTA", [STR_ACH_27_DESC] = "SCHIVA 10 PROIETTILI CON DASH",
        [STR_ACH_28_TITLE] = "PULIZIA TOTALE", [STR_ACH_28_DESC] = "BOMBA NUKE CON 10+ NEMICI",
        [STR_ACH_29_TITLE] = "UBRIACO PERSO", [STR_ACH_29_DESC] = "BEVI LA BOTTIGLIA VERDE",
        [STR_ACH_30_TITLE] = "SENZA GRAFFI", [STR_ACH_30_DESC] = "SOPRAVVIVI 3 ONDATE SENZA DANNI",
        [STR_ACH_31_TITLE] = "ULTIMO SUPERSTITE", [STR_ACH_31_DESC] = "10000 PUNTI CON 1 CUORE",
        [STR_ACH_32_TITLE] = "CONTROCORRENTE", [STR_ACH_32_DESC] = "UCCIDI 10 NEMICI DA UBRIACO",
        [STR_ACH_33_TITLE] = "SCHIACCIALO", [STR_ACH_33_DESC] = "UCCIDI KAMIKAZE A BRUCIAPELO",
        [STR_ACH_34_TITLE] = "NON UN PASSO INDIETRO", [STR_ACH_34_DESC] = "BATTI UN BOSS SENZA DASH",
        [STR_ACH_35_TITLE] = "DANZA TRA I PROIETTILI", [STR_ACH_35_DESC] = "SCHIVA 25 PROIETTILI DI FILA",
        [STR_ACH_36_TITLE] = "DIVIDI E VINCI", [STR_ACH_36_DESC] = "DISTRUGGI DIVISORE E I 2 FRAMMENTI",
        [STR_ACH_37_TITLE] = "KABOOM", [STR_ACH_37_DESC] = "KAMIKAZE DISTRUGGE UN ALTRO NEMICO",
        [STR_ACH_38_TITLE] = "DOV E?", [STR_ACH_38_DESC] = "UCCIDI TELEPORT SUBITO DOPO WARP",
        [STR_ACH_39_TITLE] = "NON COSI DIFFICILE", [STR_ACH_39_DESC] = "SUPERARE ONDATA DIFFICILE NO BONUS",
        [STR_ACH_40_TITLE] = "INFERNO PURO", [STR_ACH_40_DESC] = "RAGGIUNGI ONDATA 15 A INFERNO",
        [STR_ACH_41_TITLE] = "A UNA VITA", [STR_ACH_41_DESC] = "BATTI BOSS A INFERNO CON 1 CUORE",
        [STR_ACH_42_TITLE] = "SENZA AIUTO", [STR_ACH_42_DESC] = "5000 PUNTI A INFERNO SENZA BONUS",
        [STR_ACH_43_TITLE] = "RECLUTA RUSH", [STR_ACH_43_DESC] = "SUPERA LA PRIMA ONDATA IN RUSH",
        [STR_ACH_44_TITLE] = "MAESTRO RUSH", [STR_ACH_44_DESC] = "SUPERA LA QUARTA ONDATA IN RUSH",
        [STR_ACH_45_TITLE] = "ESPERTO RUSH", [STR_ACH_45_DESC] = "SUPERA LA PRIMA ONDATA DI BOSS",
        [STR_ACH_46_TITLE] = "CHE CI FAI QUI?", [STR_ACH_46_DESC] = "SUPERA LA SECONDA ONDATA DI BOSS",
        [STR_ACH_47_TITLE] = "INTEGRO", [STR_ACH_47_DESC] = "SUPERA ONDATA SENZA DANNI IN RUSH",
        [STR_ACH_48_TITLE] = "VELENO AL TOP", [STR_ACH_48_DESC] = "AVVELENA 500 NEMICI IN RUSH",
        [STR_ACH_49_TITLE] = "GHIACCIO AL TOP", [STR_ACH_49_DESC] = "CONGELA 500 NEMICI IN RUSH",
        [STR_ACH_50_TITLE] = "AL MASSIMO", [STR_ACH_50_DESC] = "ACCUMULA 5 CUORI IN MODALITA RUSH",
        [STR_ACH_51_TITLE] = "POTENTE", [STR_ACH_51_DESC] = "USA 60 POWER-UP IN MODALITA RUSH",
        [STR_ACH_52_TITLE] = "MORTE IMPROVVISA", [STR_ACH_52_DESC] = "ACCUMULA 50 MORTI IN MODALITA RUSH",
        [STR_ACH_53_TITLE] = "MINUTO DI GLORIA", [STR_ACH_53_DESC] = "SOPRAVVIVI 1 MINUTO A TEMPO",
        [STR_ACH_54_TITLE] = "SUPERSTITE DEL TEMPO", [STR_ACH_54_DESC] = "SOPRAVVIVI 3 MINUTI A TEMPO",
        [STR_ACH_55_TITLE] = "MAESTRO DEL TEMPO", [STR_ACH_55_DESC] = "SOPRAVVIVI 5 MINUTI A TEMPO",
        [STR_ACH_56_TITLE] = "OROLOGIO PIENO", [STR_ACH_56_DESC] = "ACCUMULA 90 SECONDI SUL TIMER",
        [STR_ACH_57_TITLE] = "CACCIATORE TEMPORALE", [STR_ACH_57_DESC] = "ELIMINA 100 NEMICI A TEMPO",
        [STR_ACH_58_TITLE] = "DISTRUZIONE VELOCE", [STR_ACH_58_DESC] = "BATTI UN BOSS IN MODALITA A TEMPO",
        [STR_ACH_59_TITLE] = "CACCIATORE TEMPORALE", [STR_ACH_59_DESC] = "ELIMINA 100 NEMICI A TEMPO",
        [STR_ACH_60_TITLE] = "DISTRUZIONE VELOCE", [STR_ACH_60_DESC] = "BATTI UN BOSS IN MODALITA A TEMPO",
        [STR_ACH_61_TITLE] = "RECLUTA DELL'ASSALTO", [STR_ACH_61_DESC] = "SOPRAVVIVI 1 MINUTO IN ASSALTO",
        [STR_ACH_62_TITLE] = "VETERANO DELL'ASSALTO", [STR_ACH_62_DESC] = "SOPRAVVIVI 3 MINUTI IN ASSALTO",
        [STR_ACH_63_TITLE] = "MAESTRO DELL'ASSALTO", [STR_ACH_63_DESC] = "SOPRAVVIVI 5 MINUTI IN ASSALTO",
        [STR_ACH_64_TITLE] = "ARTIGLIERE IMPARABILE", [STR_ACH_64_DESC] = "ELIMINA 100 NEMICI IN ASSALTO",
        [STR_ACH_65_TITLE] = "DUELLO D'ASSALTO", [STR_ACH_65_DESC] = "SCONFIGGI UN BOSS IN ASSALTO",
        [STR_ACH_66_TITLE] = "ROULETTE LETALE", [STR_ACH_66_DESC] = "ELIMINA CON 5 ARMI IN ASSALTO",
        [STR_ACH_67_TITLE] = "LEGGENDA", [STR_ACH_67_DESC] = "SBLOCCA TUTTI GLI ALTRI SUCCESSI",
        [STR_NEW_ACHIEVEMENT] = "NUOVO SUCCESSO!",
        [STR_ACH_DETAILS_HINT] = "SELEZIONA UN SUCCESSO PER I DETTAGLI",
        [STR_STATUS_UNLOCKED] = "STATO: SBLOCCATO (OK)",
        [STR_PROGRESS] = "PROGRESSO",
        [STR_MODE_WINDOWED] = "FINESTRA",
        [STR_MODE_BORDERLESS] = "SENZA BORDI",
        [STR_GAME_MODE_TITLE] = "MODI DI GIOCO",
        [STR_MODE_CLASSIC] = "CLASSICO",
        [STR_MODE_RUSH] = "RUSH",
        [STR_MODE_TIME_ATTACK] = "A TEMPO",
        [STR_MODE_COMING_SOON] = "PROSSIMAMENTE",
        [STR_RUSH_UNAVAILABLE_TITLE] = "IN SVILUPPO",
        [STR_RUSH_UNAVAILABLE_DESC] = "QUESTA MODALITA SARA PRESTO DISPONIBILE",
        [STR_CLOSE_HINT] = "PREMI UN TASTO PER CHIUDERE",
        [STR_MAX_SCORE] = "PUNTI MAX",
        [STR_MAX_WAVE] = "ONDA MAX",
        [STR_TOTAL_KILLS] = "NEMICI K.O.",
        [STR_MAX_TIME] = "TEMPO MAX",
        [STR_TIME_UP] = "TEMPO SCADUTO!",
        [STR_TIME_LEFT] = "TEMPO",
        [STR_CHEAT_TITLE] = "MENU SEGRETO",
        [STR_CHEAT_SUBTITLE] = "SEI NEL MENU SEGRETO",
        [STR_CHEAT_SUBTITLE2] = "SE HAI UN CODICE SEGRETO INSERISCILO QUI:",
        [STR_CHEAT_PLACEHOLDER] = "SCRIVI QUI...",
        [STR_CHEAT_INVALID] = "CODICE NON VALIDO",
        [STR_CHEAT_ALREADY_USED] = "CODICE GIA UTILIZZATO",
        [STR_CHEAT_SUCCESS] = "CODICE ATTIVATO!",
        [STR_MENU_SECRETS] = "SEGRETI",
        [STR_TOUCH_CONNECTED] = "TOUCH CONNESSO",
        [STR_TOUCH_BACK] = "< TOCCA PER TORNARE >",
        [STR_ACH_HINT] = "TOCCA UN SUCCESSO PER DETTAGLI",
        [STR_ACH_STATUS_UNLOCKED] = "STATO: SBLOCCATO (OK)",
        [STR_ACH_PROGRESS_LBL] = "PROGRESSO",
        [STR_CTRL_MOVE] = "MUOVI NAVE",
        [STR_TOUCH_INFO_TITLE] = "COMANDI TOUCH"
    },
    [4] = { // DEUTSCH
        [STR_PLAY] = "SPIELEN", [STR_OPTIONS] = "OPTIONEN", [STR_RECORDS] = "REKORDE", [STR_VOL_BGM] = "MUSIK LAUTST.",
        [STR_VOL_SFX] = "SFX LAUTST.", [STR_FILTER] = "FILTER", [STR_CONTROLS] = "STEUERUNG", [STR_RESOLUTION] = "RES",
        [STR_SCREEN_MODE] = "MODUS", [STR_FPS] = "FPS", [STR_VSYNC] = "VSYNC", [STR_LANGUAGE] = "SPRACHE",
        [STR_DELETE_RECORDS] = "REKORDE LOSCHEN", [STR_CONFIRM_DELETE_TITLE] = "REKORDE LOSCHEN?", [STR_CONFIRM_DELETE_SUB] = "UNWIDERRUFLICH",
        [STR_CANCEL] = "NEIN, ABBRUCH", [STR_CONFIRM_DELETE_BTN] = "JA, LOSCHEN", [STR_DIFF_TITLE] = "SCHWIERIGKEIT",
        [STR_DIFF_EASY] = "LEICHT", [STR_DIFF_NORMAL] = "NORMAL", [STR_DIFF_HARD] = "SCHWER", [STR_DIFF_HELL] = "HOLLE",
        [STR_PAUSE] = "PAUSE", [STR_RESUME] = "WEITER", [STR_MAIN_MENU] = "HAUPTMENU", [STR_GAME_OVER] = "GAME OVER",
        [STR_POINTS] = "PUNKTE", [STR_RETRY] = "NOCHMAL", [STR_QUIT_PROMPT] = "BEENDEN?",
        [STR_QUIT_SUB] = "JETZT BEENDEN?", [STR_YES] = "JA", [STR_NO] = "NEIN", [STR_CTRL_TITLE] = "STEUERUNG",
        [STR_CTRL_UP] = "HOCH", [STR_CTRL_DOWN] = "RUNTER", [STR_CTRL_LEFT] = "LINKS", [STR_CTRL_RIGHT] = "RECHTS",
        [STR_CTRL_SHOOT] = "SCHUSS", [STR_CTRL_TURBO] = "TURBO", [STR_CTRL_AIM] = "ZIELEN",
        [STR_CTRL_DASH] = "SPURT", [STR_REBIND_TITLE] = "TASTE BELEGEN", [STR_PRESS_KEY] = "TASTE DRUCKEN...",
        [STR_CANCEL_KEY] = "BKSP / ESC ABBRUCH", [STR_INVALID_ACTION] = "UNGULTIGE AKTION", [STR_DUPLICATE_KEY] = "BEREITS BELEGT",
        [STR_USE_OTHER_KEY] = "WAHLE ANDERE EINGABE", [STR_PRESS_BACK] = "DRUCKE %s ZUM ZURUCK", [STR_PRESENTS] = "PRASENTIERT...",
        [STR_READY] = "BEREIT!", [STR_GO] = "LOS!", [STR_WAVE] = "WELLE",
        [STR_GAMEPAD_CONNECTED] = "GAMEPAD VERBUNDEN", [STR_KEYBOARD_CONNECTED] = "TASTATUR VERBUNDEN",
        [STR_FREEZE] = "EINGEFROREN!", [STR_POISON] = "VERGIFTET!",
        [STR_LOGROS] = "ERFOLGE",
        [STR_ACH_1_TITLE] = "ANFANGER LEICHT", [STR_ACH_1_DESC] = "10 GEGNER AUF LEICHT BESIEGEN",
        [STR_ACH_2_TITLE] = "ANFANGER NORMAL", [STR_ACH_2_DESC] = "10 GEGNER AUF NORMAL BESIEGEN",
        [STR_ACH_3_TITLE] = "ANFANGER SCHWER", [STR_ACH_3_DESC] = "10 GEGNER AUF SCHWER BESIEGEN",
        [STR_ACH_4_TITLE] = "ANFANGER HOLLE", [STR_ACH_4_DESC] = "10 GEGNER AUF HOLLE BESIEGEN",
        [STR_ACH_5_TITLE] = "VETERAN LEICHT", [STR_ACH_5_DESC] = "100 GEGNER AUF LEICHT BESIEGEN",
        [STR_ACH_6_TITLE] = "VETERAN NORMAL", [STR_ACH_6_DESC] = "100 GEGNER AUF NORMAL BESIEGEN",
        [STR_ACH_7_TITLE] = "VETERAN SCHWER", [STR_ACH_7_DESC] = "BESIEGE 100 FEINDE AUF SCHWER",
        [STR_ACH_8_TITLE] = "VETERAN HOLLE", [STR_ACH_8_DESC] = "BESIEGE 100 FEINDE AUF HOLLE",
        [STR_ACH_9_TITLE] = "MEISTER LEICHT", [STR_ACH_9_DESC] = "BESIEGE 500 FEINDE AUF LEICHT",
        [STR_ACH_10_TITLE] = "MEISTER NORMAL", [STR_ACH_10_DESC] = "BESIEGE 500 FEINDE AUF NORMAL",
        [STR_ACH_11_TITLE] = "MEISTER SCHWER", [STR_ACH_11_DESC] = "BESIEGE 500 FEINDE AUF SCHWER",
        [STR_ACH_12_TITLE] = "MEISTER HOLLE", [STR_ACH_12_DESC] = "BESIEGE 500 FEINDE AUF HOLLE",
        [STR_ACH_13_TITLE] = "HEISSBLUTIG", [STR_ACH_13_DESC] = "25 KILLS MIT SCHWEREM SCHUSS",
        [STR_ACH_14_TITLE] = "UNVERSEHRT", [STR_ACH_14_DESC] = "WELLE OHNE SCHADEN BEENDET",
        [STR_ACH_15_TITLE] = "!BOOM!", [STR_ACH_15_DESC] = "5 KILLS MIT EINER ATOMBOMBE",
        [STR_ACH_16_TITLE] = "EINGEFROREN", [STR_ACH_16_DESC] = "FRIERE 10 FEINDE EIN",
        [STR_ACH_17_TITLE] = "TOXISCH", [STR_ACH_17_DESC] = "VERGIFTE 10 FEINDE",
        [STR_ACH_18_TITLE] = "ZEITLUPE", [STR_ACH_18_DESC] = "5 KILLS IN ZEITLUPE",
        [STR_ACH_19_TITLE] = "POWER-UP", [STR_ACH_19_DESC] = "SAMMLE ERSTES POWER-UP EIN",
        [STR_ACH_20_TITLE] = "UNANTASTBAR", [STR_ACH_20_DESC] = "30S IN HOLLE OHNE SCHADEN",
        [STR_ACH_21_TITLE] = "BOSS-JAGER", [STR_ACH_21_DESC] = "ZERSTORE 5 BOSSE",
        [STR_ACH_22_TITLE] = "TELEPORTER", [STR_ACH_22_DESC] = "ZERSTORE 10 TELEPORTER",
        [STR_ACH_23_TITLE] = "LETZTER ATEMZUG", [STR_ACH_23_DESC] = "1000 PKT MIT 1 LEBENSKRAFT",
        [STR_ACH_24_TITLE] = "VOLLES ARSENAL", [STR_ACH_24_DESC] = "NUTZE ALLE WAFFENARTEN",
        [STR_ACH_25_TITLE] = "GOTT-MODUS", [STR_ACH_25_DESC] = "X10 COMBO MIT POWER-UP",
        [STR_ACH_26_TITLE] = "ZU SCHNELL", [STR_ACH_26_DESC] = "10 KILLS MIT SCHNELLFEUER",
        [STR_ACH_27_TITLE] = "PERFEKTER DASH", [STR_ACH_27_DESC] = "WEICHE 10 SCHUSSEN MIT DASH AUS",
        [STR_ACH_28_TITLE] = "TOTALSAUBERUNG", [STR_ACH_28_DESC] = "ATOMBOMBE BEI 10+ FEINDEN",
        [STR_ACH_29_TITLE] = "VOLLTRUNKEN", [STR_ACH_29_DESC] = "TRINKE DIE GRUNE FLASCHE",
        [STR_ACH_30_TITLE] = "OHNE KRATZER", [STR_ACH_30_DESC] = "3 WELLEN OHNE SCHADEN UBERLEBEN",
        [STR_ACH_31_TITLE] = "LETZTER UBERLEBENDER", [STR_ACH_31_DESC] = "10000 PUNKTE MIT 1 HERZ",
        [STR_ACH_32_TITLE] = "GEGEN DEN STROM", [STR_ACH_32_DESC] = "10 GEGNER BETRUNKEN TOTEN",
        [STR_ACH_33_TITLE] = "ZERTRITT ES", [STR_ACH_33_DESC] = "KAMIKAZE AUS NACHSTER NAHE TOTEN",
        [STR_ACH_34_TITLE] = "KEIN SCHRITT ZURUCK", [STR_ACH_34_DESC] = "BOSS OHNE DASH BESIEGEN",
        [STR_ACH_35_TITLE] = "KUGELTANZEN", [STR_ACH_35_DESC] = "25 KUGELN IN FOLGE AUSWEICHEN",
        [STR_ACH_36_TITLE] = "TEILE UND HERRSCHE", [STR_ACH_36_DESC] = "TEILER UND BEIDE MINIS TOTEN",
        [STR_ACH_37_TITLE] = "KABOOM", [STR_ACH_37_DESC] = "KAMIKAZE ZERSTORT ANDEREN GEGNER",
        [STR_ACH_38_TITLE] = "WO IST ER?", [STR_ACH_38_DESC] = "TELEPORTER DIREKT NACH WARP TOTEN",
        [STR_ACH_39_TITLE] = "NICHT SO SCHWER", [STR_ACH_39_DESC] = "SCHWERE WELLE OHNE POWERUPS",
        [STR_ACH_40_TITLE] = "REINE HOLLE", [STR_ACH_40_DESC] = "ERREICHE WELLE 15 IN HOLLE",
        [STR_ACH_41_TITLE] = "AUF EIN LEBEN", [STR_ACH_41_DESC] = "HOLLE-BOSS MIT 1 HERZ BESIEGEN",
        [STR_ACH_42_TITLE] = "OHNE HILFE", [STR_ACH_42_DESC] = "5000 PUNKTE IN HOLLE OHNE POWERUPS",
        [STR_ACH_43_TITLE] = "RUSH NEULING", [STR_ACH_43_DESC] = "ERSTE RUSH WELLE UEBERSTEHEN",
        [STR_ACH_44_TITLE] = "RUSH MEISTER", [STR_ACH_44_DESC] = "VIERTE RUSH WELLE UEBERSTEHEN",
        [STR_ACH_45_TITLE] = "RUSH EXPERTE", [STR_ACH_45_DESC] = "ERSTE BOSS WELLE UEBERSTEHEN",
        [STR_ACH_46_TITLE] = "WAS MACHST DU HIER?", [STR_ACH_46_DESC] = "ZWEITE BOSS WELLE UEBERSTEHEN",
        [STR_ACH_47_TITLE] = "UNVERSEHRT", [STR_ACH_47_DESC] = "RUSH WELLE OHNE SCHADEN SCHAFFEN",
        [STR_ACH_48_TITLE] = "VOLLES GIFT", [STR_ACH_48_DESC] = "500 GEGNER IN RUSH VERGIFTEN",
        [STR_ACH_49_TITLE] = "VOLLES EIS", [STR_ACH_49_DESC] = "500 GEGNER IN RUSH EINFRIEREN",
        [STR_ACH_50_TITLE] = "VOLL GELADEN", [STR_ACH_50_DESC] = "5 HERZEN IN RUSH SAMMELN",
        [STR_ACH_51_TITLE] = "MAECHTIG", [STR_ACH_51_DESC] = "60 POWER-UPS IN RUSH NUTZEN",
        [STR_ACH_52_TITLE] = "PLOETZLICHER TOD", [STR_ACH_52_DESC] = "50 TODE IM RUSH MODUS ERLEIDEN",
        [STR_ACH_53_TITLE] = "MINUTE DES RUHMS", [STR_ACH_53_DESC] = "UBERLEBE 1 MINUTE IN ZEITANGRIFF",
        [STR_ACH_54_TITLE] = "ZEIT-UBERLEBENDER", [STR_ACH_54_DESC] = "UBERLEBE 3 MINUTEN IN ZEITANGRIFF",
        [STR_ACH_55_TITLE] = "MEISTER DER ZEIT", [STR_ACH_55_DESC] = "UBERLEBE 5 MINUTEN IN ZEITANGRIFF",
        [STR_ACH_56_TITLE] = "VOLLE UHR", [STR_ACH_56_DESC] = "SAMMLE 90 SEKUNDEN AUF DER UHR",
        [STR_ACH_57_TITLE] = "ZEITJAGER", [STR_ACH_57_DESC] = "BESIEGE 100 FEINDE IN ZEITANGRIFF",
        [STR_ACH_58_TITLE] = "SCHNELLE VERNICHTUNG", [STR_ACH_58_DESC] = "BESIEGE EINEN BOSS IN ZEITANGRIFF",
        [STR_ACH_59_TITLE] = "ZEITJAGER", [STR_ACH_59_DESC] = "BESIEGE 100 FEINDE IN ZEITANGRIFF",
        [STR_ACH_60_TITLE] = "SCHNELLE VERNICHTUNG", [STR_ACH_60_DESC] = "BESIEGE EINEN BOSS IN ZEITANGRIFF",
        [STR_ACH_61_TITLE] = "STURM-REKRUT", [STR_ACH_61_DESC] = "UBERLEBE 1 MINUTE IM STURM-MODUS",
        [STR_ACH_62_TITLE] = "STURM-VETERAN", [STR_ACH_62_DESC] = "UBERLEBE 3 MINUTEN IM STURM-MODUS",
        [STR_ACH_63_TITLE] = "STURM-MEISTER", [STR_ACH_63_DESC] = "UBERLEBE 5 MINUTEN IM STURM-MODUS",
        [STR_ACH_64_TITLE] = "UNAUFHALTSAMER SCHUTZE", [STR_ACH_64_DESC] = "BESIEGE 100 FEINDE IM STURM",
        [STR_ACH_65_TITLE] = "STURM-DUELL", [STR_ACH_65_DESC] = "BESIEGE EINEN BOSS IM STURM-MODUS",
        [STR_ACH_66_TITLE] = "TODLICHES ROULETTE", [STR_ACH_66_DESC] = "BESIEGE FEINDE MIT 5 WAFFEN",
        [STR_ACH_67_TITLE] = "LEGENDE", [STR_ACH_67_DESC] = "SCHALTE ALLE ANDEREN ERFOLGE FREI",
        [STR_NEW_ACHIEVEMENT] = "NEUER ERFOLG!",
        [STR_ACH_DETAILS_HINT] = "WAHLE EINEN ERFOLG FUR MEHR DETAILS",
        [STR_STATUS_UNLOCKED] = "STATUS: FREIGESCHALTET (OK)",
        [STR_PROGRESS] = "FORTSCHRITT",
        [STR_MODE_WINDOWED] = "FENSTER",
        [STR_MODE_BORDERLESS] = "RAHMENLOS",
        [STR_GAME_MODE_TITLE] = "SPIELMODI",
        [STR_MODE_CLASSIC] = "KLASSISCH",
        [STR_MODE_RUSH] = "RUSH",
        [STR_MODE_TIME_ATTACK] = "ZEITANGRIFF",
        [STR_MODE_COMING_SOON] = "DEMNAECHST",
        [STR_RUSH_UNAVAILABLE_TITLE] = "IN ENTWICKLUNG",
        [STR_RUSH_UNAVAILABLE_DESC] = "DIESER MODUS WIRD BALD VERFUGBAR SEIN",
        [STR_CLOSE_HINT] = "DRUCKE EINE TASTE ZUM SCHLIESSEN",
        [STR_MAX_SCORE] = "MAX PUNKTE",
        [STR_MAX_WAVE] = "MAX WELLE",
        [STR_TOTAL_KILLS] = "GEGNER K.O.",
        [STR_MAX_TIME] = "BESTZEIT",
        [STR_TIME_UP] = "ZEIT ABGELAUFEN!",
        [STR_TIME_LEFT] = "ZEIT",
        [STR_CHEAT_TITLE] = "GEHEIMMENU",
        [STR_CHEAT_SUBTITLE] = "DU BIST IM GEHEIMMENU",
        [STR_CHEAT_SUBTITLE2] = "GIB DEINEN GEHEIMCODE HIER EIN:",
        [STR_CHEAT_PLACEHOLDER] = "TIPPE HIER...",
        [STR_CHEAT_INVALID] = "UNGULTIGER CODE",
        [STR_CHEAT_ALREADY_USED] = "CODE BEREITS EINGELOST",
        [STR_CHEAT_SUCCESS] = "CODE AKTIVIERT!",
        [STR_MENU_SECRETS] = "GEHEIMNISSE",
        [STR_TOUCH_CONNECTED] = "TOUCH VERBUNDEN",
        [STR_TOUCH_BACK] = "< HIER TIPPEN ZURUECK >",
        [STR_ACH_HINT] = "ERFOLG TIPPEN FUER DETAILS",
        [STR_ACH_STATUS_UNLOCKED] = "STATUS: FREIGESCHALTET (OK)",
        [STR_ACH_PROGRESS_LBL] = "FORTSCHRITT",
        [STR_CTRL_MOVE] = "SCHIFF BEWEGEN",
        [STR_TOUCH_INFO_TITLE] = "TOUCH-STEUERUNG"
    }
};

const char* T(StringId id) {
    int lang = g_config.language;
    if (lang < 0 || lang >= LANG_COUNT) lang = 0;
    const char* str = g_lang_strings[lang][id];
    if (!str || str[0] == '\0') str = g_lang_strings[0][id];
    return str ? str : "";
}

// --- PERSISTENCIA ---
void saveHighScorePC(int diff, int s) {
    int scores[4] = {0};
    FILE *f = fopen(PlatformGetDataPath("records.dat"), "rb");
    if (f) { fread(scores, sizeof(int), 4, f); fclose(f); }
    if (diff >= 0 && diff < 4 && s > scores[diff]) scores[diff] = s;
    f = fopen(PlatformGetDataPath("records.dat"), "wb");
    if (f) { fwrite(scores, sizeof(int), 4, f); fclose(f); }
}

int loadHighScorePC(int diff) {
    int scores[4] = {0};
    FILE *f = fopen(PlatformGetDataPath("records.dat"), "rb");
    if (f) { fread(scores, sizeof(int), 4, f); fclose(f); }
    if (diff >= 0 && diff < 4) return scores[diff];
    return 0;
}

void saveRushHighScorePC(int s, int w, int kills) {
    int data[3] = {0};
    FILE *f = fopen(PlatformGetDataPath("records_rush.dat"), "rb");
    if (f) { fread(data, sizeof(int), 3, f); fclose(f); }
    if (s > data[0]) data[0] = s;
    if (w > data[1]) data[1] = w;
    if (kills > 0) data[2] += kills;
    f = fopen(PlatformGetDataPath("records_rush.dat"), "wb");
    if (f) { fwrite(data, sizeof(int), 3, f); fclose(f); }
}

void loadRushHighScorePC(int *s, int *w, int *k) {
    int data[3] = {0};
    FILE *f = fopen(PlatformGetDataPath("records_rush.dat"), "rb");
    if (f) { fread(data, sizeof(int), 3, f); fclose(f); }
    if (s) *s = data[0];
    if (w) *w = data[1];
    if (k) *k = data[2];
}

void saveTimeAttackHighScorePC(int survival_seconds, int s, int kills) {
    int data[3] = {0};
    FILE *f = fopen(PlatformGetDataPath("records_timeattack.dat"), "rb");
    if (f) { fread(data, sizeof(int), 3, f); fclose(f); }
    if (survival_seconds > data[0]) data[0] = survival_seconds;
    if (s > data[1]) data[1] = s;
    if (kills > 0) data[2] += kills;
    f = fopen(PlatformGetDataPath("records_timeattack.dat"), "wb");
    if (f) { fwrite(data, sizeof(int), 3, f); fclose(f); }
}

void loadTimeAttackHighScorePC(int *surv, int *s, int *k) {
    int data[3] = {0};
    FILE *f = fopen(PlatformGetDataPath("records_timeattack.dat"), "rb");
    if (f) { fread(data, sizeof(int), 3, f); fclose(f); }
    if (surv) *surv = data[0];
    if (s) *s = data[1];
    if (k) *k = data[2];
}

void saveAssaultHighScorePC(int survival_seconds, int s, int kills) {
    int data[3] = {0};
    FILE *f = fopen(PlatformGetDataPath("records_assault.dat"), "rb");
    if (f) { fread(data, sizeof(int), 3, f); fclose(f); }
    if (survival_seconds > data[0]) data[0] = survival_seconds;
    if (s > data[1]) data[1] = s;
    if (kills > 0) data[2] += kills;
    f = fopen(PlatformGetDataPath("records_assault.dat"), "wb");
    if (f) { fwrite(data, sizeof(int), 3, f); fclose(f); }
}

void loadAssaultHighScorePC(int *surv, int *s, int *k) {
    int data[3] = {0};
    FILE *f = fopen(PlatformGetDataPath("records_assault.dat"), "rb");
    if (f) { fread(data, sizeof(int), 3, f); fclose(f); }
    if (surv) *surv = data[0];
    if (s) *s = data[1];
    if (k) *k = data[2];
}

void clearAllHighScoresPC() {
    int scores[4] = {0};
    FILE *f = fopen(PlatformGetDataPath("records.dat"), "wb");
    if (f) { 
        fwrite(scores, sizeof(int), 4, f); 
        fclose(f); 
    }
    int rush_data[3] = {0};
    f = fopen(PlatformGetDataPath("records_rush.dat"), "wb");
    if (f) {
        fwrite(rush_data, sizeof(int), 3, f);
        fclose(f);
    }
    int ta_data[3] = {0};
    f = fopen(PlatformGetDataPath("records_timeattack.dat"), "wb");
    if (f) {
        fwrite(ta_data, sizeof(int), 3, f);
        fclose(f);
    }
    int assault_data[3] = {0};
    f = fopen(PlatformGetDataPath("records_assault.dat"), "wb");
    if (f) {
        fwrite(assault_data, sizeof(int), 3, f);
        fclose(f);
    }
}

void saveAchievementsPC() {
    FILE *f = fopen(PlatformGetDataPath("achievements.dat"), "wb");
    if (f) {
        size_t w1 = fwrite(g_achievements_unlocked, sizeof(bool), NUM_ACHIEVEMENTS, f);
        size_t w2 = fwrite(g_achievement_progress, sizeof(int), NUM_ACHIEVEMENTS, f);
        fclose(f);
        (void)w1;
        (void)w2;
    }
}

void loadAchievementsPC() {
    FILE *f = fopen(PlatformGetDataPath("achievements.dat"), "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        long sz = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (sz == (long)(30 * sizeof(bool) + 30 * sizeof(int))) {
            bool old_unlocked[30] = { false };
            int old_prog[30] = { 0 };
            fread(old_unlocked, sizeof(bool), 30, f);
            fread(old_prog, sizeof(int), 30, f);
            for (int i = 0; i < 29; i++) {
                g_achievements_unlocked[i] = old_unlocked[i];
                g_achievement_progress[i] = old_prog[i];
            }
        } else if (sz == (long)(43 * sizeof(bool) + 43 * sizeof(int))) {
            bool old_unlocked[43] = { false };
            int old_prog[43] = { 0 };
            fread(old_unlocked, sizeof(bool), 43, f);
            fread(old_prog, sizeof(int), 43, f);
            for (int i = 0; i < 42; i++) {
                g_achievements_unlocked[i] = old_unlocked[i];
                g_achievement_progress[i] = old_prog[i];
            }
        } else if (sz == (long)(53 * sizeof(bool) + 53 * sizeof(int))) {
            bool old_unlocked[53] = { false };
            int old_prog[53] = { 0 };
            fread(old_unlocked, sizeof(bool), 53, f);
            fread(old_prog, sizeof(int), 53, f);
            for (int i = 0; i < 52; i++) {
                g_achievements_unlocked[i] = old_unlocked[i];
                g_achievement_progress[i] = old_prog[i];
            }
        } else if (sz == (long)(59 * sizeof(bool) + 59 * sizeof(int))) {
            bool old_unlocked[59] = { false };
            int old_prog[59] = { 0 };
            fread(old_unlocked, sizeof(bool), 59, f);
            fread(old_prog, sizeof(int), 59, f);
            for (int i = 0; i <= 16; i++) {
                g_achievements_unlocked[i] = old_unlocked[i];
                g_achievement_progress[i] = old_prog[i];
            }
            for (int i = 17; i <= 46; i++) {
                g_achievements_unlocked[i + 1] = old_unlocked[i];
                g_achievement_progress[i + 1] = old_prog[i];
            }
            for (int i = 47; i <= 48; i++) {
                g_achievements_unlocked[i + 1] = old_unlocked[i];
                g_achievement_progress[i + 1] = old_prog[i];
            }
            for (int i = 49; i <= 57; i++) {
                g_achievements_unlocked[i + 2] = old_unlocked[i];
                g_achievement_progress[i + 2] = old_prog[i];
            }
            g_achievements_unlocked[ACH_LEYENDA] = old_unlocked[58];
            g_achievement_progress[ACH_LEYENDA] = old_prog[58];
        } else if (sz == (long)(61 * sizeof(bool) + 61 * sizeof(int))) {
            bool old_unlocked[61] = { false };
            int old_prog[61] = { 0 };
            fread(old_unlocked, sizeof(bool), 61, f);
            fread(old_prog, sizeof(int), 61, f);
            for (int i = 0; i < 60; i++) {
                g_achievements_unlocked[i] = old_unlocked[i];
                g_achievement_progress[i] = old_prog[i];
            }
            g_achievements_unlocked[ACH_LEYENDA] = old_unlocked[60];
            g_achievement_progress[ACH_LEYENDA] = old_prog[60];
        } else {
            fread(g_achievements_unlocked, sizeof(bool), NUM_ACHIEVEMENTS, f);
            fread(g_achievement_progress, sizeof(int), NUM_ACHIEVEMENTS, f);
        }
        fclose(f);
    }
}

void saveControlsPC() {
    FILE *f = fopen(PlatformGetDataPath("controls.dat"), "wb");
    if (f) {
        fwrite(&g_keys, sizeof(KeyBindings), 1, f);
        fwrite(&g_pad, sizeof(PadBindings), 1, f);
        fclose(f);
    }
}

static int GetPadBindingPC(int action) {
    switch (action) {
        case 0: return g_pad.btn_up;
        case 1: return g_pad.btn_down;
        case 2: return g_pad.btn_left;
        case 3: return g_pad.btn_right;
        case 4: return g_pad.btn_shoot;
        case 5: return g_pad.btn_turbo;
        case 6: return g_pad.btn_aim;
        case 7: return g_pad.btn_dash;
        default: return -1;
    }
}

static void SetPadBindingPC(int action, int button) {
    switch (action) {
        case 0: g_pad.btn_up = button; break;
        case 1: g_pad.btn_down = button; break;
        case 2: g_pad.btn_left = button; break;
        case 3: g_pad.btn_right = button; break;
        case 4: g_pad.btn_shoot = button; break;
        case 5: g_pad.btn_turbo = button; break;
        case 6: g_pad.btn_aim = button; break;
        case 7: g_pad.btn_dash = button; break;
    }
}

static bool IsPadButtonAssignedPC(int button, int except_action) {
    for (int action = 0; action < 8; action++) {
        if (action == except_action) continue;
        if (GetPadBindingPC(action) == button) return true;
    }
    return false;
}

static void ResetPadBindingToDefaultPC(int action) {
    static const int defaults[8] = {
        GAMEPAD_BUTTON_LEFT_FACE_UP,
        GAMEPAD_BUTTON_LEFT_FACE_DOWN,
        GAMEPAD_BUTTON_LEFT_FACE_LEFT,
        GAMEPAD_BUTTON_LEFT_FACE_RIGHT,
        GAMEPAD_BUTTON_RIGHT_FACE_DOWN,
        GAMEPAD_BUTTON_RIGHT_FACE_LEFT,
        GAMEPAD_BUTTON_LEFT_TRIGGER_1,
        GAMEPAD_BUTTON_RIGHT_TRIGGER_1
    };

    int preferred = defaults[action];
    if (!IsPadButtonAssignedPC(preferred, action)) {
        SetPadBindingPC(action, preferred);
        return;
    }

    for (int button = 1; button <= 17; button++) {
        if (!IsPadButtonAssignedPC(button, action)) {
            SetPadBindingPC(action, button);
            return;
        }
    }
}

static bool ValidatePadBindingsPC() {
    bool changed = false;
    for (int action = 0; action < 8; action++) {
        int button = GetPadBindingPC(action);
        if (button < 1 || button > 17 || IsPadButtonAssignedPC(button, action)) {
            ResetPadBindingToDefaultPC(action);
            changed = true;
        }
    }
    return changed;
}

void loadControlsPC() {
    FILE *f = fopen(PlatformGetDataPath("controls.dat"), "rb");
    if (f) {
        fread(&g_keys, sizeof(KeyBindings), 1, f);
        fread(&g_pad, sizeof(PadBindings), 1, f);
        fclose(f);
    }

    // Corrige configuraciones antiguas que tengan dos acciones en el mismo boton.
    if (ValidatePadBindingsPC()) saveControlsPC();
}

void saveConfigPC() {
    FILE *f = fopen(PlatformGetDataPath("config.dat"), "wb");
    if (f) { fwrite(&g_config, sizeof(GameConfig), 1, f); fclose(f); }
}

static void ValidateConfigPC(void) {
    if (g_config.target_fps < 0 || g_config.target_fps >= FPS_OPTION_COUNT) g_config.target_fps = 1;
    if (g_config.screen_mode < 0 || g_config.screen_mode > 2) g_config.screen_mode = 0;
    if (g_config.language < 0 || g_config.language >= LANG_COUNT) g_config.language = 0;
    if (g_config.vol_bgm < 0 || g_config.vol_bgm > 10) g_config.vol_bgm = 10;
    if (g_config.vol_sfx < 0 || g_config.vol_sfx > 10) g_config.vol_sfx = 10;
    if (g_config.crt_filter < 0 || g_config.crt_filter >= FILTER_COUNT) g_config.crt_filter = 0;
}

static bool g_config_file_found = false;

static int DetectNativeResolutionIndex(int mon_w, int mon_h) {
    for (int i = 0; i < RESOLUTION_COUNT; i++) {
        if (g_resolutions[i][0] == mon_w && g_resolutions[i][1] == mon_h) {
            return i;
        }
    }
    float mon_aspect = (mon_h > 0) ? ((float)mon_w / (float)mon_h) : (16.0f / 9.0f);
    int best_idx = -1;
    float best_aspect_diff = 999.0f;
    int best_pixels = 0;

    for (int i = 0; i < RESOLUTION_COUNT; i++) {
        int rw = g_resolutions[i][0];
        int rh = g_resolutions[i][1];
        if (rw <= mon_w && rh <= mon_h) {
            float res_aspect = (float)rw / (float)rh;
            float diff = fabsf(res_aspect - mon_aspect);
            if (diff < best_aspect_diff - 0.01f) {
                best_aspect_diff = diff;
                best_pixels = rw * rh;
                best_idx = i;
            } else if (fabsf(diff - best_aspect_diff) <= 0.01f) {
                if (rw * rh > best_pixels) {
                    best_pixels = rw * rh;
                    best_idx = i;
                }
            }
        }
    }

    if (best_idx >= 0) return best_idx;
    return (RESOLUTION_COUNT > 1) ? 1 : 0;
}

void loadConfigPC() {
    FILE *f = fopen(PlatformGetDataPath("config.dat"), "rb");
    if (f) {
        GameConfig loaded = g_config;
        size_t r = fread(&loaded, sizeof(GameConfig), 1, f);
        fclose(f);
        if (r == 1) {
            g_config = loaded;
            g_config_file_found = true;
        }
    }
    ValidateConfigPC();
}

int g_screen_w = 240;
int g_screen_h = 160;

static RenderTexture2D target = { 0 };

static TitleStar title_stars[MAX_TITLE_STARS];
static MenuStar menu_stars[MAX_MENU_STARS];
static GameStar game_stars[MAX_GAME_STARS];
static BloodDrop blood_drops[MAX_BLOOD_DROPS];
static bool g_stars_initialized = false;

static void InitTitleStars(void) {
    for(int s = 0; s < MAX_TITLE_STARS; s++) {
        title_stars[s].x = rand() % (SCREEN_W - 8) + 4;
        title_stars[s].y = rand() % (SCREEN_H - 6) + 2;
        title_stars[s].speed = (s % 3) + 1;
        title_stars[s].color = (title_stars[s].speed == 3) ? WHITE : ((title_stars[s].speed == 2) ? GBA_COLOR(16, 26, 31) : GBA_COLOR(8, 14, 22));
    }
}

static void InitMenuStars(void) {
    for(int m = 0; m < MAX_MENU_STARS; m++) {
        menu_stars[m].x = rand() % (SCREEN_W - 12) + 6;
        menu_stars[m].y = rand() % (SCREEN_H - 16) + 6;
        menu_stars[m].vy = (m % 2) + 1;
        menu_stars[m].color = (menu_stars[m].vy == 2) ? WHITE : GBA_COLOR(10, 22, 31);
    }
}

static void InitGameStars(void) {
    for(int g = 0; g < MAX_GAME_STARS; g++) {
        game_stars[g].x = rand() % (SCREEN_W - 8) + 4;
        game_stars[g].y = rand() % (SCREEN_H - 22) + 18;
        game_stars[g].speed = (g % 2) + 1;
        game_stars[g].color = (game_stars[g].speed == 2) ? GBA_COLOR(12, 18, 24) : GBA_COLOR(6, 8, 14);
    }
}

static void InitBloodDrops(void) {
    for(int b = 0; b < MAX_BLOOD_DROPS; b++) {
        blood_drops[b].x = rand() % (SCREEN_W - 8) + 4;
        blood_drops[b].y = rand() % (SCREEN_H - 15) + 10;
        blood_drops[b].speed = (b % 3) + 1;
        blood_drops[b].color = (b % 2 == 0) ? GBA_COLOR(24, 0, 0) : GBA_COLOR(31, 2, 2);
    }
}

static void AdaptStarsOnResolutionChange(void) {
    if (!g_stars_initialized) return;
    InitTitleStars();
    InitMenuStars();
    InitGameStars();
    InitBloodDrops();
}

static const char* GetFpsOptionText(int fps_idx) {
    int max_hz = 60;
#if defined(PLATFORM_IOS)
    max_hz = IOS_GetMaxRefreshRate();
#endif
    if (fps_idx == 0) return "30 FPS";
    if (fps_idx == 1) return "60 FPS";
    if (fps_idx == 2) {
        return (max_hz >= 120) ? "120 FPS" : "120 FPS (60Hz)";
    }
    return "60 FPS";
}

void ApplyFpsSetting(void) {
    int fps_values[FPS_OPTION_COUNT] = { 30, 60, 120 };
    if (g_config.target_fps < 0 || g_config.target_fps >= FPS_OPTION_COUNT) g_config.target_fps = 1;
    int target_fps = fps_values[g_config.target_fps];
    SetTargetFPS(target_fps);
#if defined(PLATFORM_IOS)
    IOS_SetDisplayFPS(target_fps);
#endif
}

void ApplyVSyncSetting(void) {
    // VSync en iOS es gestionado nativamente por UIKit/Metal
}

void ApplyVideoSettings(void) {
    ApplyFpsSetting();

    AdaptStarsOnResolutionChange();

    if (IsWindowReady()) {
        if (target.id <= 0) {
            target = LoadRenderTexture(SCREEN_W, SCREEN_H);
            SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
        }
    }
}

int CountAchievementBits(int value) {
    int count = 0;
    while (value > 0) {
        count += value & 1;
        value >>= 1;
    }
    return count;
}

void UnlockAchievement(int index) {
    if (index < 0 || index >= NUM_ACHIEVEMENTS) return;

    if (!g_achievements_unlocked[index]) {
        g_achievements_unlocked[index] = true;
        saveAchievementsPC();
        PlaySfx(sndPowerUp);

        if (g_achievement_toast_timer > 0 || g_achievement_queue_count > 0) {
            if (g_achievement_queue_count < ACHIEVEMENT_QUEUE_SIZE) {
                g_achievement_queue[g_achievement_queue_count++] = index;
            }
        } else {
            g_latest_unlocked_ach = index;
            g_achievement_toast_timer = 180;
            g_achievement_toast_timer_f = 180.0f;
        }

        if (index != ACH_LEYENDA) {
            int unlocked_count = 0;
            for (int ach_idx = 0; ach_idx < ACH_LEYENDA; ach_idx++) {
                if (g_achievements_unlocked[ach_idx]) unlocked_count++;
            }
            g_achievement_progress[ACH_LEYENDA] = unlocked_count;

            if (unlocked_count >= ACH_LEYENDA) {
                UnlockAchievement(ACH_LEYENDA);
            } else {
                saveAchievementsPC();
            }
        }
    }
}

typedef enum {
    KILL_CAUSE_BULLET,
    KILL_CAUSE_POISON,
    KILL_CAUSE_NUKE,
    KILL_CAUSE_PLAYER_POWER,
    KILL_CAUSE_KAMIKAZE_AOE,
    KILL_CAUSE_KAMIKAZE_CRASH
} KillCause;

typedef struct {
    KillCause cause;
    int weapon_type;
    bool during_slowmo;
} KillContext;

static Enemy enemies[MAX_ENEMIES];
static ExplosionEffect explosions[MAX_EXPLO];
static FloatingText ftexts[MAX_TEXTS];
static int score = 0;
static int combo = 1;
static int combo_timer = 0;
static int player_hp = 3;
static int current_difficulty = 1;
static int p_type = 0;
static int px = 110;
static int py = 125;
static float player_x = 110.0f;
static float player_y = 125.0f;
static float prev_player_x = 110.0f;
static float prev_player_y = 125.0f;
static float enemy_fx[MAX_ENEMIES];
static float enemy_fy[MAX_ENEMIES];
static float prev_enemy_fx[MAX_ENEMIES];
static float prev_enemy_fy[MAX_ENEMIES];
static int screen_shake_timer = 0;
static int slowmo_timer = 0;
static int wave = 1;
static Bullet e_bullets[MAX_E_BULLETS];

static Particle particles[MAX_PARTICLES];
static GhostTrail ghost_trails[MAX_GHOST_TRAILS];
static int g_damage_flash_timer = 0;
static int g_dash_ready_flash = 0;
static int prev_dash_cd = 0;

static void SpawnParticle(float x, float y, float vx, float vy, Color color, int life, int size) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life <= 0) {
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = vx;
            particles[i].vy = vy;
            particles[i].color = color;
            particles[i].life = life;
            particles[i].max_life = life;
            particles[i].size = size;
            break;
        }
    }
}

static void SpawnGhostTrail(float x, float y, int dx, int dy, Color color) {
    for (int i = 0; i < MAX_GHOST_TRAILS; i++) {
        if (ghost_trails[i].timer <= 0) {
            ghost_trails[i].x = x;
            ghost_trails[i].y = y;
            ghost_trails[i].dx = dx;
            ghost_trails[i].dy = dy;
            ghost_trails[i].color = color;
            ghost_trails[i].timer = 12;
            ghost_trails[i].max_timer = 12;
            break;
        }
    }
}

static void SpawnBulletSparks(float x, float y, Color col) {
    for (int s = 0; s < 3; s++) {
        float vx = ((float)(rand() % 40) - 20.0f) * 0.05f;
        float vy = ((float)(rand() % 40) - 20.0f) * 0.05f;
        SpawnParticle(x, y, vx, vy, col, 8 + (rand() % 6), 1);
    }
}

static void KillEnemy(int idx, KillContext ctx);
static void ApplyEnemyDamage(int idx, int damage, KillContext ctx);

static int GetEnemyBasePoints(int type) {
    if (type == 3) return 500;
    if (type == 4) return 200;
    if (type == 7) return 150;
    if (type == 6) return 50;
    return 100;
}

static void SpawnFloatingText(float fx, float fy, const char *fstr, Color fcol) {
    for (int ft_idx = 0; ft_idx < MAX_TEXTS; ft_idx++) {
        if (ftexts[ft_idx].timer <= 0) {
            ftexts[ft_idx].x = fx;
            ftexts[ft_idx].y = fy;
            snprintf(ftexts[ft_idx].text, sizeof(ftexts[ft_idx].text), "%s", fstr);
            ftexts[ft_idx].color = fcol;
            ftexts[ft_idx].timer = 40;
            break;
        }
    }
}

static void SpawnFloatingTextAboveShip(float ship_x, float ship_y, const char *fstr, Color fcol) {
    int txt_w = MeasureStringCustom(fstr, 1);
    float centered_x = (ship_x + 5.0f) - ((float)txt_w / 2.0f);
    if (centered_x < 4.0f) centered_x = 4.0f;
    if (centered_x + (float)txt_w > (float)(SCREEN_W - 4)) centered_x = (float)(SCREEN_W - 4 - txt_w);
    float centered_y = ship_y - 12.0f;
    SpawnFloatingText(centered_x, centered_y, fstr, fcol);
}

static void ActivateQueuedCheatsOnGameStart(int *hp, int start_px, int start_py) {
    g_cheat_rainbow_active = g_cheat_rainbow_queued;
    g_cheat_rainbow_queued = false;
    g_cheat_size_active = g_cheat_size_queued;
    g_cheat_size_queued = 0;
    g_cheat_chonky_active = g_cheat_chonky_queued;
    g_cheat_chonky_queued = false;
    g_cheat_flip_active = g_cheat_flip_queued;
    g_cheat_flip_queued = false;
    g_cheat_onehit_active = g_cheat_onehit_queued;
    g_cheat_onehit_queued = false;
    g_cheat_pewpew_active = g_cheat_pewpew_queued;
    g_cheat_pewpew_queued = false;

    if (g_cheat_onehit_active) {
        *hp = 1;
        SpawnFloatingTextAboveShip((float)start_px, (float)start_py, "!1 HP!", C_RED);
    }
}

static void ValidateSecretCode(void) {
    if (g_cheat_input_len > 0 && !g_cheat_success) {
        char code_upper[32];
        for (int ci = 0; ci < g_cheat_input_len; ci++) {
            code_upper[ci] = (char)toupper((unsigned char)g_cheat_input[ci]);
        }
        code_upper[g_cheat_input_len] = '\0';

        bool code_valid = false;
        bool already_redeemed = false;

        if (strcmp(code_upper, "RAINBOW") == 0) {
            if (g_cheat_rainbow_redeemed) {
                already_redeemed = true;
            } else {
                code_valid = true;
                g_cheat_rainbow_redeemed = true;
                g_cheat_rainbow_queued = true;
                snprintf(g_cheat_success_msg, sizeof(g_cheat_success_msg), "!ARCOIRIS ACTIVADO!");
            }
        } else if (strcmp(code_upper, "DOABARRELROLL") == 0) {
            if (g_cheat_barrelroll_redeemed) {
                already_redeemed = true;
            } else {
                code_valid = true;
                g_cheat_barrelroll_redeemed = true;
                g_cheat_barrelroll_pending = true;
                snprintf(g_cheat_success_msg, sizeof(g_cheat_success_msg), "!DO A BARREL ROLL!");
            }
        } else if (strcmp(code_upper, "TINY") == 0) {
            if (g_cheat_tiny_redeemed) {
                already_redeemed = true;
            } else {
                code_valid = true;
                g_cheat_tiny_redeemed = true;
                g_cheat_size_queued = 1;
                snprintf(g_cheat_success_msg, sizeof(g_cheat_success_msg), "!MODO DIMINUTO!");
            }
        } else if (strcmp(code_upper, "HOLYCOW") == 0) {
            if (g_cheat_holycow_redeemed) {
                already_redeemed = true;
            } else {
                code_valid = true;
                g_cheat_holycow_redeemed = true;
                g_cheat_size_queued = 2;
                snprintf(g_cheat_success_msg, sizeof(g_cheat_success_msg), "!MODO GIGANTE!");
            }
        } else if (strcmp(code_upper, "MATRIX") == 0) {
            if (g_cheat_matrix_redeemed) {
                already_redeemed = true;
            } else {
                code_valid = true;
                g_cheat_matrix_redeemed = true;
                g_cheat_matrix_active = true;
                snprintf(g_cheat_success_msg, sizeof(g_cheat_success_msg), "!MODO MATRIX!");
            }
        } else if (strcmp(code_upper, "CHONKY") == 0) {
            if (g_cheat_chonky_redeemed) {
                already_redeemed = true;
            } else {
                code_valid = true;
                g_cheat_chonky_redeemed = true;
                g_cheat_chonky_queued = true;
                snprintf(g_cheat_success_msg, sizeof(g_cheat_success_msg), "!ENEMIGOS CHONKY!");
            }
        } else if (strcmp(code_upper, "FLIP") == 0) {
            if (g_cheat_flip_redeemed) {
                already_redeemed = true;
            } else {
                code_valid = true;
                g_cheat_flip_redeemed = true;
                g_cheat_flip_queued = true;
                snprintf(g_cheat_success_msg, sizeof(g_cheat_success_msg), "!MODO ESPEJO!");
            }
        } else if (strcmp(code_upper, "ONEHIT") == 0) {
            if (g_cheat_onehit_redeemed) {
                already_redeemed = true;
            } else {
                code_valid = true;
                g_cheat_onehit_redeemed = true;
                g_cheat_onehit_queued = true;
                snprintf(g_cheat_success_msg, sizeof(g_cheat_success_msg), "!MUERTE SUBITA 1 HP!");
            }
        } else if (strcmp(code_upper, "PEWPEW") == 0) {
            if (g_cheat_pewpew_redeemed) {
                already_redeemed = true;
            } else {
                code_valid = true;
                g_cheat_pewpew_redeemed = true;
                g_cheat_pewpew_queued = true;
                snprintf(g_cheat_success_msg, sizeof(g_cheat_success_msg), "!MODO PEW PEW!");
            }
        }

        if (code_valid) {
            g_cheat_success = true;
            g_cheat_success_timer = 90;
            g_cheat_error = false;
            g_cheat_already_used_err = false;
            PlaySfx(sndPowerUp);
        } else if (already_redeemed) {
            g_cheat_error = true;
            g_cheat_already_used_err = true;
            g_cheat_error_timer = 180;
            g_cheat_success = false;
            PlaySfx(sndHit);
        } else {
            g_cheat_error = true;
            g_cheat_already_used_err = false;
            g_cheat_error_timer = 180;
            g_cheat_success = false;
            PlaySfx(sndHit);
        }
    }
}

static void OnSecretCodeSubmitted(const char* code) {
    if (!code) return;
    int len = 0;
    while (code[len] && len < 18) {
        char ch = (char)toupper((unsigned char)code[len]);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '-') {
            g_cheat_input[len] = ch;
            len++;
        }
    }
    g_cheat_input[len] = '\0';
    g_cheat_input_len = len;
    ValidateSecretCode();
}

static void ResetActiveCheatsOnGameEnd(void) {
    g_cheat_rainbow_active = false;
    g_cheat_size_active = 0;
    g_cheat_chonky_active = false;
    g_cheat_flip_active = false;
    g_cheat_onehit_active = false;
    g_cheat_pewpew_active = false;
}

static void SpawnEnemyExplosion(int x, int y) {
    for (int ex_slot = 0; ex_slot < MAX_EXPLO; ex_slot++) {
        if (!explosions[ex_slot].active) {
            explosions[ex_slot].active = 1;
            explosions[ex_slot].x = x;
            explosions[ex_slot].y = y;
            explosions[ex_slot].timer = 30;
            break;
        }
    }
}

static void SpawnDivisorFragments(int x, int y) {
    g_divisor_combo_active = true;
    g_divisor_fragments_left = 2;
    g_divisor_combo_timer = 300;
    int spawned_minis = 0;
    for (int m_slot = 0; m_slot < MAX_ENEMIES && spawned_minis < 2; m_slot++) {
        if (!enemies[m_slot].active) {
            enemies[m_slot].active = 1;
            enemies[m_slot].type = 6;
            enemies[m_slot].hp = 1;
            enemies[m_slot].freeze_timer = 0;
            enemies[m_slot].poison_timer = 0;
            enemies[m_slot].wet_timer = 0;
            enemies[m_slot].timer = 0;
            enemies[m_slot].x = x + (spawned_minis == 0 ? -14 : 14);
            enemies[m_slot].y = y;
            enemy_fx[m_slot] = (float)enemies[m_slot].x;
            enemy_fy[m_slot] = (float)enemies[m_slot].y;
            prev_enemy_fx[m_slot] = enemy_fx[m_slot];
            prev_enemy_fy[m_slot] = enemy_fy[m_slot];
            spawned_minis++;
        }
    }
}

static void ApplyEnemyDamage(int idx, int damage, KillContext ctx) {
    if (idx < 0 || idx >= MAX_ENEMIES || !enemies[idx].active) return;

    enemies[idx].hp -= damage;
    enemies[idx].hit_flash_timer = 4;
    SpawnBulletSparks((float)enemies[idx].x + 5.0f, (float)enemies[idx].y + 5.0f, C_YELLOW);
    if (enemies[idx].hp <= 0) {
        KillEnemy(idx, ctx);
    }
}

static void KillEnemy(int idx, KillContext ctx) {
    if (idx < 0 || idx >= MAX_ENEMIES || !enemies[idx].active) return;

    int type = enemies[idx].type;
    int ex = enemies[idx].x;
    int ey = enemies[idx].y;

    if (type == 8) {
        enemies[idx].active = 0;
        return;
    }

    if (ctx.cause == KILL_CAUSE_BULLET) {
        if (ctx.weapon_type == 2) {
            g_achievement_progress[ACH_DEMASIADO_RAPIDO]++;
            if (g_achievement_progress[ACH_DEMASIADO_RAPIDO] >= 10)
                UnlockAchievement(ACH_DEMASIADO_RAPIDO);
        }
        if (ctx.weapon_type == 0 && type == 7 && abs(px - ex) <= 22 && abs(py - ey) <= 22) {
            UnlockAchievement(ACH_PISALO);
        }
    }

    if (ctx.during_slowmo) {
        g_achievement_progress[ACH_TIEMPO_MUERTO]++;
        if (g_achievement_progress[ACH_TIEMPO_MUERTO] >= 5)
            UnlockAchievement(ACH_TIEMPO_MUERTO);
    }

    if (ctx.cause == KILL_CAUSE_KAMIKAZE_AOE) {
        UnlockAchievement(ACH_KABOOM);
    }

    if (ctx.cause != KILL_CAUSE_KAMIKAZE_CRASH) {
        if (current_game_mode == MODE_CLASSIC && current_difficulty >= 0 && current_difficulty < 4) {
            g_enemies_killed_per_diff[current_difficulty]++;

            int base_idx = current_difficulty;
            g_achievement_progress[base_idx]++;
            if (!g_achievements_unlocked[base_idx] && g_achievement_progress[base_idx] >= 10)
                UnlockAchievement(base_idx);

            int vet_idx = 4 + current_difficulty;
            g_achievement_progress[vet_idx]++;
            if (!g_achievements_unlocked[vet_idx] && g_achievement_progress[vet_idx] >= 100)
                UnlockAchievement(vet_idx);

            int mas_idx = 8 + current_difficulty;
            g_achievement_progress[mas_idx]++;
            if (!g_achievements_unlocked[mas_idx] && g_achievement_progress[mas_idx] >= 500)
                UnlockAchievement(mas_idx);
        }
    }

    if (type == 3) {
        g_achievement_progress[ACH_CAZADOR_JEFES]++;
        if (g_achievement_progress[ACH_CAZADOR_JEFES] >= 5)
            UnlockAchievement(ACH_CAZADOR_JEFES);
        if (!g_boss_dash_used)
            UnlockAchievement(ACH_NI_UN_PASO_ATRAS);
        if (current_game_mode == MODE_CLASSIC && current_difficulty == 3 && player_hp == 1)
            UnlockAchievement(ACH_A_UNA_VIDA);
    }

    if (type == 9) {
        g_achievement_progress[ACH_TELETRANSP]++;
        if (g_achievement_progress[ACH_TELETRANSP] >= 10)
            UnlockAchievement(ACH_TELETRANSP);
        if (enemies[idx].timer < 60)
            UnlockAchievement(ACH_DONDE_ESTA);
    }

    if (type == 6 && g_divisor_combo_active) {
        g_divisor_fragments_left--;
        if (g_divisor_fragments_left <= 0) {
            UnlockAchievement(ACH_DIVIDE_Y_VENCERAS);
            g_divisor_combo_active = false;
        }
    }

    if (p_type == 10 && ctx.cause != KILL_CAUSE_KAMIKAZE_CRASH) {
        g_achievement_progress[ACH_A_CONTRACORRIENTE]++;
        if (g_achievement_progress[ACH_A_CONTRACORRIENTE] >= 10) {
            UnlockAchievement(ACH_A_CONTRACORRIENTE);
        }
    }

    enemies[idx].active = 0;
    SpawnEnemyExplosion(ex, ey);
    PlaySfx(sndExplo);
    if (type == 3) screen_shake_timer = 25;

    if (ctx.cause != KILL_CAUSE_KAMIKAZE_CRASH) {
        int base_pts = GetEnemyBasePoints(type);
        int pts = base_pts * combo;
        score += pts;

        if (player_hp == 1 && score >= 1000) UnlockAchievement(ACH_ULTIMO_ALIENTO);
        if (player_hp == 1 && score >= 10000) UnlockAchievement(ACH_ULTIMO_SUPERVIVIENTE);
        if (current_game_mode == MODE_CLASSIC && current_difficulty == 3 && g_powerups_picked_this_run == 0 && score >= 5000) UnlockAchievement(ACH_SIN_AYUDA);

        char pts_buf[20];
        snprintf(pts_buf, sizeof(pts_buf), "+%d", pts);
        SpawnFloatingText((float)ex, (float)ey - 8, pts_buf, C_YELLOW);

        if (combo < 5 && p_type != 8 && p_type != 13) combo++;
        combo_timer = 300;
    }

    if (current_game_mode == MODE_RUSH) {
        if (type != 8) rush_run_kills++;
        int cycle_wave = ((wave - 1) % 10) + 1;
        bool wave_cleared = false;
        if (cycle_wave == 10) {
            if (type == 3) {
                rush_kills_in_wave++;
                if (rush_kills_in_wave >= 10) wave_cleared = true;
            }
        } else {
            if (type != 8) {
                rush_kills_in_wave++;
                int needed = 20 + (rush_cycle * 10);
                if (rush_kills_in_wave >= needed) wave_cleared = true;
            }
        }

        if (wave_cleared) {
            int cleared_wave = wave;
            if (cleared_wave >= 1) UnlockAchievement(ACH_RUSH_NOVATO);
            if (cleared_wave >= 4) UnlockAchievement(ACH_RUSH_MAESTRO);
            if (cleared_wave >= 10) UnlockAchievement(ACH_RUSH_EXPERTO);
            if (cleared_wave >= 20) UnlockAchievement(ACH_RUSH_QUE_HACES_AQUI);

            if (!player_took_hit_this_wave) {
                UnlockAchievement(ACH_RUSH_ENTERITO);
            }
            player_took_hit_this_wave = false;

            wave++;
            rush_cycle = (wave - 1) / 10;
            rush_kills_in_wave = 0;
            rush_wave_banner_timer = 90;
            for (int eb = 0; eb < MAX_E_BULLETS; eb++) e_bullets[eb].active = 0;
            for (int en = 0; en < MAX_ENEMIES; en++) {
                if (enemies[en].active && enemies[en].type != 8) {
                    enemies[en].active = 0;
                    SpawnEnemyExplosion(enemies[en].x, enemies[en].y);
                }
            }
        }
    }

    if (current_game_mode == MODE_TIME_ATTACK) {
        if (type != 8) {
            ta_run_kills++;
            g_achievement_progress[ACH_TA_CAZADOR] = ta_run_kills;
            if (ta_run_kills >= 100) {
                UnlockAchievement(ACH_TA_CAZADOR);
            }

            int bonus_sec = 1;
            if (type == 3) {
                bonus_sec = 15;
                g_achievement_progress[ACH_TA_BOSS] = 1;
                UnlockAchievement(ACH_TA_BOSS);
            } else if (type == 2 || type == 4 || type == 5 || type == 7 || type == 9) {
                bonus_sec = 3;
            }

            ta_time_left_frames += bonus_sec * 60;
            if (ta_time_left_frames > 99 * 60 * 60) ta_time_left_frames = 99 * 60 * 60;

            int left_sec = ta_time_left_frames / 60;
            if (left_sec > g_achievement_progress[ACH_TA_RELOJ_LLENO]) {
                g_achievement_progress[ACH_TA_RELOJ_LLENO] = left_sec;
            }
            if (ta_time_left_frames >= 90 * 60) {
                UnlockAchievement(ACH_TA_RELOJ_LLENO);
            }

            char time_bonus_buf[16];
            snprintf(time_bonus_buf, sizeof(time_bonus_buf), "+%d SEG", bonus_sec);
            SpawnFloatingText((float)ex, (float)ey - 16, time_bonus_buf, C_CYAN);
        }
    }

    if (current_game_mode == MODE_ASSAULT) {
        if (type != 8) {
            assault_run_kills++;
            g_achievement_progress[ACH_ASSAULT_CAZADOR] = assault_run_kills;
            if (assault_run_kills >= 100) {
                UnlockAchievement(ACH_ASSAULT_CAZADOR);
            }

            if (type == 3) {
                g_achievement_progress[ACH_ASSAULT_BOSS] = 1;
                UnlockAchievement(ACH_ASSAULT_BOSS);
            }

            int pu_bit = -1;
            if (p_type == 2) pu_bit = 0;       // Disparo Rapido
            else if (p_type == 3) pu_bit = 1;  // Triple Disparo
            else if (p_type == 5) pu_bit = 2;  // Disparo Pesado
            else if (p_type == 7) pu_bit = 3;  // Ralentizacion
            else if (p_type == 9) pu_bit = 4;  // Disparo Helado
            else if (p_type == 12) pu_bit = 5; // Disparo Venenoso
            else if (p_type == 14) pu_bit = 6; // Disparo Acuatico
            else if (p_type == 0) pu_bit = 7;  // Cañon Normal

            if (pu_bit >= 0) {
                g_assault_weapons_killed_mask |= (1 << pu_bit);
                int distinct_weapons = CountAchievementBits(g_assault_weapons_killed_mask);
                if (distinct_weapons > g_achievement_progress[ACH_ASSAULT_RULETA]) {
                    g_achievement_progress[ACH_ASSAULT_RULETA] = (distinct_weapons > 5) ? 5 : distinct_weapons;
                }
                if (distinct_weapons >= 5) {
                    UnlockAchievement(ACH_ASSAULT_RULETA);
                }
            }
        }
    }

    if (type == 5 && ctx.cause != KILL_CAUSE_NUKE) {
        SpawnDivisorFragments(ex, ey);
    }

    if (type == 7 && ctx.cause != KILL_CAUSE_NUKE && ctx.cause != KILL_CAUSE_KAMIKAZE_AOE) {
        for (int other_idx = 0; other_idx < MAX_ENEMIES; other_idx++) {
            if (other_idx != idx && enemies[other_idx].active) {
                int dx = enemies[other_idx].x - ex;
                int dy = enemies[other_idx].y - ey;
                if (dx * dx + dy * dy <= 28 * 28) {
                    ApplyEnemyDamage(other_idx, 2, (KillContext){ .cause = KILL_CAUSE_KAMIKAZE_AOE, .weapon_type = -1, .during_slowmo = ctx.during_slowmo });
                }
            }
        }
    }
}


static Shader crtShader = { 0 };
static int filterTypeLoc = -1;
static int renderSizeLoc = -1;
static Texture2D logoTexture = { 0 };

static int state = 14; 
static int intro_timer = 0;
static int frame_count = 0;

static int menu_selection = 0;
static int options_selection = 0;
static int pause_options_selection = 0;
static int exit_confirm_selection = 1;
static int options_scroll_offset = 0;
static int pause_options_scroll_offset = 0;
static float pause_options_enter_timer = 0.0f;
static bool is_dragging_scrollbar = false;
static bool is_dragging_ach_scroll = false;
static bool just_entered_menu = false;
static float pause_resume_timer = 0.0f;
static float pause_toggle_timer = 0.0f;
static float stick_menu_timer = 0.0f;
static int stick_menu_last_dir = -1;
static int controls_origin_state = 7;

static int diff_selection = 1;
static int pause_selection = 0;
static int gameover_selection = 0;
static int confirm_selection = 0;
static int controls_selection = 0;
static int rebinding_action = -1;
static int duplicate_key_warning = 0;
static float duplicate_key_warning_f = 0.0f;

static int dir_x = 0, dir_y = -1;
static int invincibility = 0;
static int shoot_cd = 0;
static int p_timer = 0;
static int nuke_timer = 0, dash_cd = 0, dash_invincibility = 0, death_timer = 0, ready_timer = 0;
static int hell_no_damage_frames = 0;

static Bullet bullets[MAX_BULLETS] = {0};
static PowerUp p_up = {0};

static float time_accumulator = 0.0f;
static const float FIXED_DELTA = 1.0f / 60.0f;
static bool request_exit = false;

#define SPAWN_FTEXT(fx, fy, fstr, fcol) SpawnFloatingText((float)(fx), (float)(fy), (fstr), (fcol))

static void GameInit(void) {
    loadConfigPC();
    loadAchievementsPC();

    g_achievements_unlocked[ACH_LEYENDA] = false;
    int unlocked_count_on_load = 0;
    for (int ach_idx = 0; ach_idx < ACH_LEYENDA; ach_idx++) {
        if (g_achievements_unlocked[ach_idx]) unlocked_count_on_load++;
    }
    g_achievement_progress[ACH_LEYENDA] = unlocked_count_on_load;

    InitWindow(0, 0, "Astrallitic");
    SetExitKey(KEY_NULL);
    if (!g_config_file_found) {
        g_config.screen_mode = 0;
        g_config.target_fps = 1;
        g_config.vol_bgm = 10;
        g_config.vol_sfx = 10;
        saveConfigPC();
    }
    ApplyVideoSettings();

    if (FileExists(PlatformGetAssetPath("icon.png"))) {
        Image app_icon = LoadImage(PlatformGetAssetPath("icon.png"));
        SetWindowIcon(app_icon);
        UnloadImage(app_icon);
    } else if (FileExists(PlatformGetAssetPath("image.png"))) {
        Image app_icon = LoadImage(PlatformGetAssetPath("image.png"));
        SetWindowIcon(app_icon);
        UnloadImage(app_icon);
    }

    InitGameAudio();
    loadControlsPC();

    if (g_achievement_progress[ACH_LEYENDA] >= ACH_LEYENDA) {
        UnlockAchievement(ACH_LEYENDA);
    } else {
        saveAchievementsPC();
    }

    if (target.id <= 0) target = LoadRenderTexture(SCREEN_W, SCREEN_H);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    crtShader = LoadShaderFromMemory(crt_vertex_shader_code, crt_shader_code);
    filterTypeLoc = GetShaderLocation(crtShader, "filterType");
    renderSizeLoc = GetShaderLocation(crtShader, "renderSize");

    if (FileExists(PlatformGetAssetPath("image.png"))) {
        Image img = LoadImage(PlatformGetAssetPath("image.png"));
        ImageResizeNN(&img, 80, 80);
        logoTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }

    MobileInput_Init();
#if defined(PLATFORM_IOS)
    IOSGamepad_Init();
#endif
    memset(e_bullets, 0, sizeof(e_bullets));

    InitTitleStars();
    InitMenuStars();
    InitGameStars();
    InitBloodDrops();
    g_stars_initialized = true;
}

static void GameUpdate(void) {
#if defined(PLATFORM_IOS)
    IOSGamepad_Update();
#endif

        float frame_dt = GetFrameTime();
        if (frame_dt > 0.25f) frame_dt = 0.25f;
        time_accumulator += frame_dt;

        UpdateGameBgm();

        if (g_device_toast_timer_f > 0.0f) {
            g_device_toast_timer_f -= (frame_dt * 60.0f);
            if (g_device_toast_timer_f < 0.0f) g_device_toast_timer_f = 0.0f;
            g_device_toast_timer = (int)ceilf(g_device_toast_timer_f);
        } else {
            g_device_toast_timer = 0;
        }

        if (g_achievement_toast_timer_f > 0.0f) {
            g_achievement_toast_timer_f -= (frame_dt * 60.0f);
            if (g_achievement_toast_timer_f <= 0.0f) {
                if (g_achievement_queue_count > 0) {
                    g_latest_unlocked_ach = g_achievement_queue[0];
                    for (int q_idx = 1; q_idx < g_achievement_queue_count; q_idx++)
                        g_achievement_queue[q_idx - 1] = g_achievement_queue[q_idx];
                    g_achievement_queue_count--;
                    g_achievement_toast_timer_f = 180.0f;
                    g_achievement_toast_timer = 180;
                } else {
                    g_achievement_toast_timer_f = 0.0f;
                    g_achievement_toast_timer = 0;
                }
            } else {
                g_achievement_toast_timer = (int)ceilf(g_achievement_toast_timer_f);
            }
        } else {
            g_achievement_toast_timer = 0;
        }

        if (pause_toggle_timer > 0.0f) pause_toggle_timer -= frame_dt;
        if (pause_resume_timer > 0.0f) pause_resume_timer -= frame_dt;
        if (pause_options_enter_timer > 0.0f) pause_options_enter_timer -= frame_dt;
        if (duplicate_key_warning_f > 0.0f) {
            duplicate_key_warning_f -= (frame_dt * 60.0f);
            if (duplicate_key_warning_f < 0.0f) duplicate_key_warning_f = 0.0f;
            duplicate_key_warning = (int)ceilf(duplicate_key_warning_f);
        } else {
            duplicate_key_warning = 0;
        }

        // --- DETECCIÓN DINÁMICA DE ENTRADAS ---
        int pad_id = 0;
        bool pad_active = IsGamepadAvailable(pad_id);

        float pad_stick_x = pad_active ? GetGamepadAxisMovement(pad_id, GAMEPAD_AXIS_LEFT_X) : 0.0f;
        float pad_stick_y = pad_active ? GetGamepadAxisMovement(pad_id, GAMEPAD_AXIS_LEFT_Y) : 0.0f;
        if (fabsf(pad_stick_x) < 0.25f) pad_stick_x = 0.0f;
        if (fabsf(pad_stick_y) < 0.25f) pad_stick_y = 0.0f;

        bool pad_game_hold_up    = pad_active && (IsGamepadButtonDown(pad_id, g_pad.btn_up) || pad_stick_y < -0.3f);
        bool pad_game_hold_down  = pad_active && (IsGamepadButtonDown(pad_id, g_pad.btn_down) || pad_stick_y > 0.3f);
        bool pad_game_hold_left  = pad_active && (IsGamepadButtonDown(pad_id, g_pad.btn_left) || pad_stick_x < -0.3f);
        bool pad_game_hold_right = pad_active && (IsGamepadButtonDown(pad_id, g_pad.btn_right) || pad_stick_x > 0.3f);
        bool pad_game_dash       = pad_active && IsGamepadButtonPressed(pad_id, g_pad.btn_dash);

        bool pad_stick_menu_up    = (pad_stick_y < -0.5f);
        bool pad_stick_menu_down  = (pad_stick_y > 0.5f);
        bool pad_stick_menu_left  = (pad_stick_x < -0.5f);
        bool pad_stick_menu_right = (pad_stick_x > 0.5f);

        int current_menu_dir = -1;
        if (pad_active) {
            if (IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_UP) || pad_stick_menu_up) current_menu_dir = 0;
            else if (IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_DOWN) || pad_stick_menu_down) current_menu_dir = 1;
            else if (IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || pad_stick_menu_left) current_menu_dir = 2;
            else if (IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || pad_stick_menu_right) current_menu_dir = 3;
        }

        bool stick_menu_trigger = false;
        if (current_menu_dir != -1) {
            if (current_menu_dir != stick_menu_last_dir) {
                stick_menu_trigger = true;
                stick_menu_timer = 0.28f;
                stick_menu_last_dir = current_menu_dir;
            } else {
                stick_menu_timer -= frame_dt;
                if (stick_menu_timer <= 0.0f) {
                    stick_menu_trigger = true;
                    stick_menu_timer = 0.14f;
                }
            }
        } else {
            stick_menu_last_dir = -1;
            stick_menu_timer = 0.0f;
        }

        bool pad_menu_up    = (current_menu_dir == 0 && stick_menu_trigger);
        bool pad_menu_down  = (current_menu_dir == 1 && stick_menu_trigger);
        bool pad_menu_left  = (current_menu_dir == 2 && stick_menu_trigger);
        bool pad_menu_right = (current_menu_dir == 3 && stick_menu_trigger);

        bool pad_lb_pressed = pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_LEFT_TRIGGER_1);
        bool pad_rb_pressed = pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_TRIGGER_1);

        bool pad_menu_accept = pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
        bool pad_menu_back   = pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
        bool pad_btn_start   = pad_active && (IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_MIDDLE_RIGHT) || IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_MIDDLE_LEFT));

        int current_frame_key = GetKeyPressed();

        bool any_pad_event = false;
        if (pad_active) {
            if (fabsf(pad_stick_x) > 0.3f || fabsf(pad_stick_y) > 0.3f) any_pad_event = true;
            for (int b = 1; b <= 17; b++) {
                if (IsGamepadButtonPressed(pad_id, b) || IsGamepadButtonDown(pad_id, b)) { any_pad_event = true; break; }
            }
        }

        bool any_kb_event = (current_frame_key > 0 || IsMouseButtonPressed(MOUSE_BUTTON_LEFT));

        if (rebinding_action == -1 && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER)) {
            g_config.screen_mode = (g_config.screen_mode + 1) % 2;
            ApplyVideoSettings();
            saveConfigPC();
            current_frame_key = 0;
        }

        if (any_pad_event && g_last_input_device != INPUT_GAMEPAD) {
            g_last_input_device = INPUT_GAMEPAD;
            g_toast_device = INPUT_GAMEPAD;
            g_device_toast_timer_f = 120.0f;
            g_device_toast_timer = 120;
        } else if (any_kb_event && g_last_input_device != INPUT_KEYBOARD) {
            g_last_input_device = INPUT_KEYBOARD;
            g_toast_device = INPUT_KEYBOARD;
            g_device_toast_timer_f = 120.0f;
            g_device_toast_timer = 120;
        }

        bool is_gameplay = (state == 1 || state == 9 || state == 8);
        MobileInput_Update(is_gameplay);
        MobileInputState mobile = MobileInput_GetState();

        float screen_render_w = (float)GetScreenWidth();
        float screen_render_h = (float)GetScreenHeight();

        float scale_x = screen_render_w / (float)SCREEN_W;
        float scale_y = screen_render_h / (float)SCREEN_H;
        float scale_val = (scale_x < scale_y) ? scale_x : scale_y;

        float draw_w = (float)SCREEN_W * scale_val;
        float draw_h = (float)SCREEN_H * scale_val;
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
        if (g_config.screen_mode == 0) {
            // Modo 0 (Por defecto): COMPLETA - llena el 100% de la pantalla del iPhone moderno
            draw_w = screen_render_w;
            draw_h = screen_render_h;
        } else if (g_config.screen_mode == 1) {
            // Modo 1: 16:9 WIDE panorámico arcade
            draw_h = screen_render_h;
            draw_w = draw_h * (16.0f / 9.0f);
            if (draw_w > screen_render_w) draw_w = screen_render_w;
        } else if (g_config.screen_mode == 2) {
            // Modo 2: 3:2 RETRO clásico (GBA)
            draw_w = (float)SCREEN_W * scale_val;
            draw_h = (float)SCREEN_H * scale_val;
        }
#endif
        float offset_x = roundf((screen_render_w - draw_w) * 0.5f);
        float offset_y = roundf((screen_render_h - draw_h) * 0.5f);
        float render_w = draw_w;
        float render_h = draw_h;

        Vector2 m_pos = GetMousePosition();
        int mouse_x = (draw_w > 0.001f) ? (int)((m_pos.x - offset_x) * ((float)SCREEN_W / draw_w)) : 0;
        int mouse_y = (draw_h > 0.001f) ? (int)((m_pos.y - offset_y) * ((float)SCREEN_H / draw_h)) : 0;
        bool mouse_clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool mouse_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        Vector2 m_delta = GetMouseDelta();
        bool mouse_moved = (m_delta.x != 0 || m_delta.y != 0);
        float mouse_wheel = GetMouseWheelMove();

        if (just_entered_menu) {
            mouse_clicked = false;
            just_entered_menu = false;
        }

        bool esc_pressed  = IsKeyPressed(KEY_ESCAPE);
        bool m_up    = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || pad_menu_up || mobile.up_pressed;
        bool m_down  = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || pad_menu_down || mobile.down_pressed;
        bool m_left  = IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || pad_menu_left || mobile.left_pressed;
        bool m_right = IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || pad_menu_right || mobile.right_pressed;
        bool m_accept = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || pad_menu_accept || mobile.a_pressed;
        bool m_back   = IsKeyPressed(KEY_BACKSPACE) || esc_pressed || IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT) || pad_menu_back || mobile.b_pressed;

        bool is_menu_with_back_btn = (state == 4 || state == 5 || state == 6 || state == 7 || state == 10 || state == 12 || state == 15 || state == 16);
        if (!is_gameplay && is_menu_with_back_btn && mouse_clicked) {
            if (mouse_y >= SCREEN_H - 24 && mouse_y <= SCREEN_H && mouse_x >= 20 && mouse_x <= SCREEN_W - 20) {
                m_back = true;
                PlaySfx(sndHit);
            }
        }

        bool k_hold_up    = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(g_keys.key_up) || pad_game_hold_up || mobile.up_down;
        bool k_hold_down  = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S) || IsKeyDown(g_keys.key_down) || pad_game_hold_down || mobile.down_down;
        bool k_hold_left  = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || IsKeyDown(g_keys.key_left) || pad_game_hold_left || mobile.left_down;
        bool k_hold_right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) || IsKeyDown(g_keys.key_right) || pad_game_hold_right || mobile.right_down;
        bool k_dash       = IsKeyPressed(KEY_C) || IsKeyPressed(KEY_I) || (current_frame_key == g_keys.key_dash) || pad_game_dash || mobile.dash_pressed;
        bool k_start      = (pause_toggle_timer <= 0.0f) && (esc_pressed || (current_frame_key == KEY_ESCAPE) || pad_btn_start || mobile.pause_pressed);

        // Pausa instantánea ingame en el render frame (cero latencia)
        if (state == 1 || state == 9) {
            if (!IsWindowFocused() && pause_toggle_timer <= 0.0f) {
                state = 3;
                pause_selection = 0;
                pause_toggle_timer = 0.35f;
                m_back = false;
            }
        }
        if (state == 1) {
            if (k_start && pause_toggle_timer <= 0.0f) {
                state = 3;
                pause_selection = 0;
                pause_toggle_timer = 0.35f;
                k_start = false;
                m_back = false;
                pad_btn_start = false;
                PlaySfx(sndHit);
            }
        }

        // --- GESTIÓN DE MENÚS ---
        if (state == 0) {
            PlayGameBgm(0);

            // --- Popup de trucos activo ---
            if (g_show_cheat_popup) {
                int p_w = 224;
                int p_h = 116;
                int p_x = (SCREEN_W - p_w) / 2;
                int p_y = (SCREEN_H - p_h) / 2;
                int field_x = p_x + 16;
                int field_y = p_y + 51;
                int field_w = p_w - 32;
                int field_h = 18;

                // Botón [X] para cerrar en táctil
                if (mouse_clicked && mouse_x >= p_x + p_w - 24 && mouse_x <= p_x + p_w - 2 && mouse_y >= p_y + 2 && mouse_y <= p_y + 20) {
                    g_show_cheat_popup = false;
                    just_entered_menu = true;
                    m_back = false;
                    PlaySfx(sndHit);
                    if (g_cheat_barrelroll_pending) {
                        g_cheat_barrelroll_pending = false;
                        g_barrel_roll_timer = 1.0f;
                    }
                }

                // Tocar campo de texto o botón de teclado para desplegar teclado nativo en iOS
                int kb_btn_w = 90;
                int kb_btn_x = p_x + (p_w - kb_btn_w) / 2;
                int kb_btn_y = p_y + 94;
                if (mouse_clicked && ((mouse_x >= field_x && mouse_x <= field_x + field_w && mouse_y >= field_y && mouse_y <= field_y + field_h) ||
                                     (mouse_x >= kb_btn_x && mouse_x <= kb_btn_x + kb_btn_w && mouse_y >= kb_btn_y && mouse_y <= kb_btn_y + 14))) {
                    IOS_ShowSecretCodeDialog(OnSecretCodeSubmitted);
                }
                // Actualizar timers
                if (g_cheat_error_timer > 0) {
                    g_cheat_error_timer--;
                    if (g_cheat_error_timer == 0) { g_cheat_error = false; g_cheat_already_used_err = false; }
                }
                if (g_cheat_success_timer > 0) {
                    g_cheat_success_timer--;
                    if (g_cheat_success_timer == 0) {
                        g_cheat_success = false;
                        g_show_cheat_popup = false;
                        just_entered_menu = true;
                        if (g_cheat_barrelroll_pending) {
                            g_cheat_barrelroll_pending = false;
                            g_barrel_roll_timer = 1.0f;
                        }
                    }
                }

                // Cerrar con ESC, Gamepad B/Back o clic fuera del popup
                int p_box_popup_w = 224;
                int p_box_popup_h = 116;
                int p_box_popup_x = (SCREEN_W - p_box_popup_w) / 2;
                int p_box_popup_y = (SCREEN_H - p_box_popup_h) / 2;
                bool mouse_outside_click = mouse_clicked && (mouse_x < p_box_popup_x || mouse_x > p_box_popup_x + p_box_popup_w || mouse_y < p_box_popup_y || mouse_y > p_box_popup_y + p_box_popup_h);
                bool close_pressed = IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT) || (pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) || mouse_outside_click;
                if (close_pressed) {
                    g_show_cheat_popup = false;
                    g_cheat_input_len = 0;
                    g_cheat_input[0] = '\0';
                    g_cheat_error = false;
                    g_cheat_already_used_err = false;
                    g_cheat_success = false;
                    just_entered_menu = true;
                    m_back = false;
                    PlaySfx(sndHit);
                    if (g_cheat_barrelroll_pending) {
                        g_cheat_barrelroll_pending = false;
                        g_barrel_roll_timer = 1.0f;
                    }
                }
                // Borrar último carácter con Backspace (sin cerrar el popup si hay caracteres)
                else if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
                    if (g_cheat_input_len > 0) {
                        g_cheat_input_len--;
                        g_cheat_input[g_cheat_input_len] = '\0';
                        g_cheat_error = false;
                        g_cheat_already_used_err = false;
                        g_cheat_success = false;
                    } else {
                        // Si ya está vacío y pulsa Backspace, cerramos
                        g_show_cheat_popup = false;
                        just_entered_menu = true;
                        m_back = false;
                        PlaySfx(sndHit);
                        if (g_cheat_barrelroll_pending) {
                            g_cheat_barrelroll_pending = false;
                            g_barrel_roll_timer = 1.0f;
                        }
                    }
                }
                // Confirmar con Enter / Teclado Numérico Enter / Gamepad A
                else if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || (pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)))) {
                    ValidateSecretCode();
                }
                // Entrada de caracteres
                else if (!g_cheat_success) {
                    bool added_char = false;
                    int c_input = GetCharPressed();
                    while (c_input > 0) {
                        if (g_cheat_input_len < 18) {
                            if ((c_input >= 'a' && c_input <= 'z') || (c_input >= 'A' && c_input <= 'Z') || (c_input >= '0' && c_input <= '9') || c_input == '_' || c_input == '-') {
                                char ch = (char)toupper((unsigned char)c_input);
                                g_cheat_input[g_cheat_input_len++] = ch;
                                g_cheat_input[g_cheat_input_len] = '\0';
                                g_cheat_error = false;
                                added_char = true;
                            }
                        }
                        c_input = GetCharPressed();
                    }
                    if (!added_char && g_cheat_input_len < 18) {
                        if (current_frame_key >= KEY_A && current_frame_key <= KEY_Z) {
                            char ch = (char)('A' + (current_frame_key - KEY_A));
                            g_cheat_input[g_cheat_input_len++] = ch;
                            g_cheat_input[g_cheat_input_len] = '\0';
                            g_cheat_error = false;
                        } else if (current_frame_key >= KEY_ZERO && current_frame_key <= KEY_NINE) {
                            char ch = (char)('0' + (current_frame_key - KEY_ZERO));
                            g_cheat_input[g_cheat_input_len++] = ch;
                            g_cheat_input[g_cheat_input_len] = '\0';
                            g_cheat_error = false;
                        }
                    }
                }

                // Desactivar teclas del menú debajo para evitar activaciones fantasma
                m_up = false;
                m_down = false;
                m_left = false;
                m_right = false;
                m_accept = false;
                m_back = false;
            } else if (show_konami_popup) {
                if (m_accept || m_back || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) ||
                    IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_B) || mouse_clicked ||
                    (pad_active && (IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
                                    IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
                                    IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_MIDDLE_RIGHT)))) {
                    show_konami_popup = false;
                    just_entered_menu = true;
                    m_accept = false;
                    m_back = false;
                }
            } else {
                // Detección del Código Konami: ARRIBA, ARRIBA, ABAJO, ABAJO, IZQUIERDA, DERECHA, IZQUIERDA, DERECHA, B, A
                bool k_up    = m_up;
                bool k_down  = m_down;
                bool k_left  = IsKeyPressed(KEY_LEFT) || (g_konami_step != 9 && IsKeyPressed(KEY_A)) || pad_menu_left;
                bool k_right = m_right;
                bool k_b     = IsKeyPressed(KEY_B) || (pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT));
                bool k_a     = (g_konami_step == 9 && IsKeyPressed(KEY_A)) || (pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));

                if (k_up) {
                    if (g_konami_step == 0) g_konami_step = 1;
                    else if (g_konami_step == 1) g_konami_step = 2;
                    else g_konami_step = 1;
                } else if (k_down) {
                    if (g_konami_step == 2) g_konami_step = 3;
                    else if (g_konami_step == 3) g_konami_step = 4;
                    else g_konami_step = 0;
                } else if (k_left) {
                    if (g_konami_step == 4) g_konami_step = 5;
                    else if (g_konami_step == 6) g_konami_step = 7;
                    else g_konami_step = 0;
                } else if (k_right) {
                    if (g_konami_step == 5) g_konami_step = 6;
                    else if (g_konami_step == 7) g_konami_step = 8;
                    else g_konami_step = 0;
                } else if (k_b) {
                    if (g_konami_step == 8) {
                        g_konami_step = 9;
                        m_back = false;
                    } else {
                        g_konami_step = 0;
                    }
                } else if (k_a) {
                    if (g_konami_step == 9) {
                        g_konami_step = 0;
                        m_accept = false;
                        if (!g_konami_redeemed) {
                            g_konami_redeemed = true;
                            g_konami_hell_bonus = true;
                            show_konami_popup = true;
                            PlaySfx(sndPowerUp);
                        }
                    } else {
                        g_konami_step = 0;
                    }
                }

                if (m_up)   menu_selection = (menu_selection - 1 + 4) % 4;
                if (m_down) menu_selection = (menu_selection + 1) % 4;

                int btn_w = 150;
                int btn_x = (SCREEN_W - btn_w) / 2;
                for (int opt_idx = 0; opt_idx < 4; opt_idx++) {
                    int opt_y = 74 + (opt_idx * 17);
                    if (mouse_x >= btn_x && mouse_x <= btn_x + btn_w && mouse_y >= opt_y - 3 && mouse_y <= opt_y + 12) {
                        if (mouse_moved) menu_selection = opt_idx;
                        if (mouse_clicked) { menu_selection = opt_idx; m_accept = true; }
                    }
                }

                if (m_back || (pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_MIDDLE_LEFT))) {
                    state = 13;
                    exit_confirm_selection = 1;
                    just_entered_menu = true;
                    PlaySfx(sndHit);
                }

                if (m_accept) {
                    just_entered_menu = true;
                    if (menu_selection == 0) { state = 16; PlayGameBgm(3); game_mode_selection = 0; show_rush_popup = false; }
                    else if (menu_selection == 1) { state = 7; PlayGameBgm(1); options_selection = 0; options_scroll_offset = 0; }
                    else if (menu_selection == 2) { state = 6; PlayGameBgm(2); show_ach_details = false; selected_ach_index = 0; records_mode_tab = 0; }
                    else if (menu_selection == 3) {
                        g_show_cheat_popup = true;
                        g_cheat_input_len = 0;
                        g_cheat_input[0] = '\0';
                        g_cheat_error = false;
                        g_cheat_already_used_err = false;
                        g_cheat_success = false;
                        PlaySfx(sndPowerUp);
                    }
                }
            }
        }

        else if (state == 13) {
            if (m_up || m_down) exit_confirm_selection = 1 - exit_confirm_selection;

            int conf_btn_w = 150;
            int conf_btn_x = (SCREEN_W - conf_btn_w) / 2;
            for (int conf_idx = 0; conf_idx < 2; conf_idx++) {
                int opt_y = 86 + (conf_idx * 14);
                if (mouse_x >= conf_btn_x && mouse_x <= conf_btn_x + conf_btn_w && mouse_y >= opt_y - 2 && mouse_y <= opt_y + 10) {
                    if (mouse_moved) exit_confirm_selection = conf_idx;
                    if (mouse_clicked) { exit_confirm_selection = conf_idx; m_accept = true; }
                }
            }

            if (m_accept) {
                if (exit_confirm_selection == 0) { request_exit = true; return; }
                else { state = 0; just_entered_menu = true; PlaySfx(sndHit); }
            }
            if (m_back) { state = 0; just_entered_menu = true; }
        }
        else if (state == 16) {
            PlayGameBgm(3);
            if (show_rush_popup) {
                if (m_accept || m_back || mouse_clicked || (GetKeyPressed() > 0)) {
                    show_rush_popup = false;
                    PlaySfx(sndHit);
                }
            } else {
                if (m_left) {
                    if (game_mode_selection == 1) { game_mode_selection = 0; PlaySfx(sndHit); }
                    else if (game_mode_selection == 3) { game_mode_selection = 2; PlaySfx(sndHit); }
                }
                if (m_right) {
                    if (game_mode_selection == 0) { game_mode_selection = 1; PlaySfx(sndHit); }
                    else if (game_mode_selection == 2) { game_mode_selection = 3; PlaySfx(sndHit); }
                }
                if (m_up) {
                    if (game_mode_selection == 2) { game_mode_selection = 0; PlaySfx(sndHit); }
                    else if (game_mode_selection == 3) { game_mode_selection = 1; PlaySfx(sndHit); }
                }
                if (m_down) {
                    if (game_mode_selection == 0) { game_mode_selection = 2; PlaySfx(sndHit); }
                    else if (game_mode_selection == 1) { game_mode_selection = 3; PlaySfx(sndHit); }
                }

                int card_w = 88;
                int card_h = 36;
                int gap_x = 16;
                int total_w = card_w * 2 + gap_x;
                int start_x = (SCREEN_W - total_w) / 2;
                int bx0 = start_x;
                int bx1 = start_x + card_w + gap_x;

                int top_margin = 28;
                int bottom_cap_y = SCREEN_H - 19;
                int avail_h = bottom_cap_y - top_margin;
                int content_h = (card_h + 9) * 2;
                int gap_v = (avail_h - content_h) / 3;
                if (gap_v < 6) gap_v = 6;
                int by0 = top_margin + gap_v;
                int lbl_y0 = by0 + card_h + 3;
                int by1 = lbl_y0 + 6 + gap_v;

                if (mouse_x >= bx0 && mouse_x <= bx0 + card_w && mouse_y >= by0 && mouse_y <= by0 + card_h + 10) {
                    if (mouse_moved) game_mode_selection = 0;
                    if (mouse_clicked) { game_mode_selection = 0; m_accept = true; }
                }
                if (mouse_x >= bx1 && mouse_x <= bx1 + card_w && mouse_y >= by0 && mouse_y <= by0 + card_h + 10) {
                    if (mouse_moved) game_mode_selection = 1;
                    if (mouse_clicked) { game_mode_selection = 1; m_accept = true; }
                }
                if (mouse_x >= bx0 && mouse_x <= bx0 + card_w && mouse_y >= by1 && mouse_y <= by1 + card_h + 10) {
                    if (mouse_moved) game_mode_selection = 2;
                    if (mouse_clicked) { game_mode_selection = 2; m_accept = true; }
                }
                if (mouse_x >= bx1 && mouse_x <= bx1 + card_w && mouse_y >= by1 && mouse_y <= by1 + card_h + 10) {
                    if (mouse_moved) game_mode_selection = 3;
                    if (mouse_clicked) { game_mode_selection = 3; m_accept = true; }
                }

                if (m_accept) {
                    if (game_mode_selection == 0) {
                        current_game_mode = MODE_CLASSIC;
                        state = 5;
                        diff_selection = current_difficulty;
                        PlaySfx(sndHit);
                    } else if (game_mode_selection == 1) {
                        current_game_mode = MODE_RUSH;
                        StopAllBgm();
                        current_difficulty = 1;
                        player_hp = 3;
                        score = 0; px = (SCREEN_W - 10) / 2; py = SCREEN_H - 35; player_x = (float)px; player_y = (float)py; prev_player_x = player_x; prev_player_y = player_y; dir_x = 0; dir_y = -1; p_type = 0; p_up.active = 0;
                        ActivateQueuedCheatsOnGameStart(&player_hp, px, py);
                        slowmo_timer = 0; nuke_timer = 0; dash_cd = 0; dash_invincibility = 0; death_timer = 0; combo = 1; combo_timer = 0;
                        wave = 1;
                        rush_kills_in_wave = 0;
                        rush_run_kills = 0;
                        rush_target_kills = 20;
                        rush_cycle = 0;
                        rush_wave_banner_timer = 0;
                        hell_no_damage_frames = 0; player_took_hit_this_wave = false;
                        g_waves_without_damage_streak = 0; g_boss_dash_used = false; g_consecutive_bullets_dodged = 0;
                        g_divisor_combo_active = false; g_divisor_fragments_left = 0; g_divisor_combo_timer = 0;
                        g_powerups_picked_this_wave = 0; g_powerups_picked_this_run = 0;
                        for(int e_idx=0; e_idx<MAX_ENEMIES; e_idx++) { enemies[e_idx].active = 0; enemies[e_idx].wet_timer = 0; enemy_fx[e_idx] = 0.0f; enemy_fy[e_idx] = 0.0f; prev_enemy_fx[e_idx] = 0.0f; prev_enemy_fy[e_idx] = 0.0f; }
                        for(int b_idx=0; b_idx<MAX_BULLETS; b_idx++) bullets[b_idx].active = 0;
                        for(int eb_idx=0; eb_idx<MAX_E_BULLETS; eb_idx++) e_bullets[eb_idx].active = 0;
                        for(int ex_idx=0; ex_idx<MAX_EXPLO; ex_idx++) explosions[ex_idx].active = 0;
                        for (int pi = 0; pi < MAX_PARTICLES; pi++) particles[pi].life = 0;
                        for (int gi = 0; gi < MAX_GHOST_TRAILS; gi++) ghost_trails[gi].timer = 0;
                        for (int fti = 0; fti < MAX_TEXTS; fti++) ftexts[fti].timer = 0;
                        g_damage_flash_timer = 0;
                        g_dash_ready_flash = 0;
                        prev_dash_cd = 0;
                        ready_timer = 120;
                        pause_resume_timer = 0.25f;
                        pause_toggle_timer = 0.35f;
                        state = 9;
                        PlaySfx(sndHit);
                    } else if (game_mode_selection == 2) {
                        current_game_mode = MODE_TIME_ATTACK;
                        StopAllBgm();
                        current_difficulty = 1;
                        player_hp = 5;
                        score = 0; px = (SCREEN_W - 10) / 2; py = SCREEN_H - 35; player_x = (float)px; player_y = (float)py; prev_player_x = player_x; prev_player_y = player_y; dir_x = 0; dir_y = -1; p_type = 0; p_up.active = 0;
                        ActivateQueuedCheatsOnGameStart(&player_hp, px, py);
                        slowmo_timer = 0; nuke_timer = 0; dash_cd = 0; dash_invincibility = 0; death_timer = 0; combo = 1; combo_timer = 0;
                        wave = 1;
                        ta_time_left_frames = 30 * 60;
                        ta_time_survived_frames = 0;
                        ta_run_kills = 0;
                        ta_spawn_rate = 75;
                        ta_spawn_timer = 0;
                        ta_time_up = false;
                        hell_no_damage_frames = 0; player_took_hit_this_wave = false;
                        g_waves_without_damage_streak = 0; g_boss_dash_used = false; g_consecutive_bullets_dodged = 0;
                        g_divisor_combo_active = false; g_divisor_fragments_left = 0; g_divisor_combo_timer = 0;
                        g_powerups_picked_this_wave = 0; g_powerups_picked_this_run = 0;
                        for(int e_idx=0; e_idx<MAX_ENEMIES; e_idx++) { enemies[e_idx].active = 0; enemies[e_idx].wet_timer = 0; enemy_fx[e_idx] = 0.0f; enemy_fy[e_idx] = 0.0f; prev_enemy_fx[e_idx] = 0.0f; prev_enemy_fy[e_idx] = 0.0f; }
                        for(int b_idx=0; b_idx<MAX_BULLETS; b_idx++) bullets[b_idx].active = 0;
                        for(int eb_idx=0; eb_idx<MAX_E_BULLETS; eb_idx++) e_bullets[eb_idx].active = 0;
                        for(int ex_idx=0; ex_idx<MAX_EXPLO; ex_idx++) explosions[ex_idx].active = 0;
                        for (int pi = 0; pi < MAX_PARTICLES; pi++) particles[pi].life = 0;
                        for (int gi = 0; gi < MAX_GHOST_TRAILS; gi++) ghost_trails[gi].timer = 0;
                        for (int fti = 0; fti < MAX_TEXTS; fti++) ftexts[fti].timer = 0;
                        g_damage_flash_timer = 0;
                        g_dash_ready_flash = 0;
                        prev_dash_cd = 0;
                        ready_timer = 120;
                        pause_resume_timer = 0.25f;
                        pause_toggle_timer = 0.35f;
                        state = 9;
                        PlaySfx(sndHit);
                    } else if (game_mode_selection == 3) {
                        current_game_mode = MODE_ASSAULT;
                        StopAllBgm();
                        current_difficulty = 1;
                        player_hp = 5;
                        score = 0; px = (SCREEN_W - 10) / 2; py = SCREEN_H - 35; player_x = (float)px; player_y = (float)py; prev_player_x = player_x; prev_player_y = player_y; dir_x = 0; dir_y = -1; p_type = 0; p_up.active = 0;
                        ActivateQueuedCheatsOnGameStart(&player_hp, px, py);
                        slowmo_timer = 0; nuke_timer = 0; dash_cd = 0; dash_invincibility = 0; death_timer = 0; combo = 1; combo_timer = 0;
                        wave = 1;
                        assault_timer_frames = 15 * 60;
                        assault_time_survived_frames = 0;
                        assault_run_kills = 0;
                        assault_banner_timer = 0;
                        g_assault_weapons_killed_mask = 0;
                        hell_no_damage_frames = 0; player_took_hit_this_wave = false;
                        g_waves_without_damage_streak = 0; g_boss_dash_used = false; g_consecutive_bullets_dodged = 0;
                        g_divisor_combo_active = false; g_divisor_fragments_left = 0; g_divisor_combo_timer = 0;
                        g_powerups_picked_this_wave = 0; g_powerups_picked_this_run = 0;
                        for(int e_idx=0; e_idx<MAX_ENEMIES; e_idx++) { enemies[e_idx].active = 0; enemies[e_idx].wet_timer = 0; enemy_fx[e_idx] = 0.0f; enemy_fy[e_idx] = 0.0f; prev_enemy_fx[e_idx] = 0.0f; prev_enemy_fy[e_idx] = 0.0f; }
                        for(int b_idx=0; b_idx<MAX_BULLETS; b_idx++) bullets[b_idx].active = 0;
                        for(int eb_idx=0; eb_idx<MAX_E_BULLETS; eb_idx++) e_bullets[eb_idx].active = 0;
                        for(int ex_idx=0; ex_idx<MAX_EXPLO; ex_idx++) explosions[ex_idx].active = 0;
                        for (int pi = 0; pi < MAX_PARTICLES; pi++) particles[pi].life = 0;
                        for (int gi = 0; gi < MAX_GHOST_TRAILS; gi++) ghost_trails[gi].timer = 0;
                        for (int fti = 0; fti < MAX_TEXTS; fti++) ftexts[fti].timer = 0;
                        g_damage_flash_timer = 0;
                        g_dash_ready_flash = 0;
                        prev_dash_cd = 0;
                        ready_timer = 120;
                        pause_resume_timer = 0.25f;
                        pause_toggle_timer = 0.35f;
                        state = 9;
                        PlaySfx(sndHit);
                    }
                }
                if (m_back) {
                    state = 0;
                    PlayGameBgm(0);
                    just_entered_menu = true;
                    PlaySfx(sndHit);
                }
            }
        }
        else if (state == 5) {
            PlayGameBgm(3);
            if (m_up)   diff_selection = (diff_selection - 1 + 4) % 4;
            if (m_down) diff_selection = (diff_selection + 1) % 4;

            int diff_btn_w = 150;
            int diff_btn_x = (SCREEN_W - diff_btn_w) / 2;
            for (int diff_idx = 0; diff_idx < 4; diff_idx++) {
                int opt_y = 54 + (diff_idx * 20);
                if (mouse_x >= diff_btn_x && mouse_x <= diff_btn_x + diff_btn_w && mouse_y >= opt_y - 3 && mouse_y <= opt_y + 12) {
                    if (mouse_moved) diff_selection = diff_idx;
                    if (mouse_clicked) { diff_selection = diff_idx; m_accept = true; }
                }
            }

            if (m_back) { state = 16; just_entered_menu = true; PlaySfx(sndHit); }
            if (m_accept) {
                StopAllBgm();
                current_game_mode = MODE_CLASSIC;
                current_difficulty = diff_selection;
                player_hp = (current_difficulty == 0) ? 5 : ((current_difficulty == 3) ? 1 : 3);
                score = 0; px = (SCREEN_W - 10) / 2; py = SCREEN_H - 35; player_x = (float)px; player_y = (float)py; prev_player_x = player_x; prev_player_y = player_y; dir_x = 0; dir_y = -1; p_type = 0; p_up.active = 0;
                if (current_difficulty == 3 && g_konami_hell_bonus) {
                    player_hp += 2;
                    g_konami_hell_bonus = false;
                    SpawnFloatingTextAboveShip((float)px, (float)py, "+2 VIDAS!", C_YELLOW);
                }
                ActivateQueuedCheatsOnGameStart(&player_hp, px, py);
                slowmo_timer = 0; nuke_timer = 0; dash_cd = 0; dash_invincibility = 0; death_timer = 0; combo = 1; combo_timer = 0; wave = 1;
                hell_no_damage_frames = 0; player_took_hit_this_wave = false;
                g_waves_without_damage_streak = 0; g_boss_dash_used = false; g_consecutive_bullets_dodged = 0;
                g_divisor_combo_active = false; g_divisor_fragments_left = 0; g_divisor_combo_timer = 0;
                g_powerups_picked_this_wave = 0; g_powerups_picked_this_run = 0;
                for(int e_idx=0; e_idx<MAX_ENEMIES; e_idx++) { enemies[e_idx].active = 0; enemies[e_idx].wet_timer = 0; enemy_fx[e_idx] = 0.0f; enemy_fy[e_idx] = 0.0f; prev_enemy_fx[e_idx] = 0.0f; prev_enemy_fy[e_idx] = 0.0f; }
                for(int b_idx=0; b_idx<MAX_BULLETS; b_idx++) bullets[b_idx].active = 0;
                for(int eb_idx=0; eb_idx<MAX_E_BULLETS; eb_idx++) e_bullets[eb_idx].active = 0;
                for(int ex_idx=0; ex_idx<MAX_EXPLO; ex_idx++) explosions[ex_idx].active = 0;
                for (int pi = 0; pi < MAX_PARTICLES; pi++) particles[pi].life = 0;
                for (int gi = 0; gi < MAX_GHOST_TRAILS; gi++) ghost_trails[gi].timer = 0;
                for (int fti = 0; fti < MAX_TEXTS; fti++) ftexts[fti].timer = 0;
                g_damage_flash_timer = 0;
                g_dash_ready_flash = 0;
                prev_dash_cd = 0;
                ready_timer = 120;
                pause_resume_timer = 0.25f;
                pause_toggle_timer = 0.35f;
                state = 9; 
            }
        }
        else if (state == 7) {
            PlayGameBgm(1);

            const int total_opts = 8;
            const int max_scroll = 5;

            if (mouse_wheel != 0) {
                int scroll_delta = (int)roundf(mouse_wheel);
                if (scroll_delta == 0) scroll_delta = (mouse_wheel > 0.0f) ? 1 : -1;
                options_scroll_offset -= scroll_delta;
                if (options_scroll_offset < 0) options_scroll_offset = 0;
                if (options_scroll_offset > max_scroll) options_scroll_offset = max_scroll;
                if (options_selection < options_scroll_offset) options_selection = options_scroll_offset;
                if (options_selection >= options_scroll_offset + 3) options_selection = options_scroll_offset + 2;
            }

            int opt_top = (SCREEN_H - 130) / 2;
            if (opt_top < 15) opt_top = 15;
            int opt_start_y = opt_top + 37;

            // Botones triangulares en el lateral derecho (x: 200..234)
            if (mouse_clicked && mouse_x >= 200 && mouse_x <= 234) {
                if (mouse_y >= 44 && mouse_y <= 84) {
                    // Triangulo Arriba: sube de opcion si no esta arriba del todo
                    if (options_selection > 0) {
                        options_selection--;
                        PlaySfx(sndHit);
                    }
                } else if (mouse_y >= 94 && mouse_y <= 136) {
                    // Triangulo Abajo: baja de opcion si no esta abajo del todo
                    if (options_selection < total_opts - 1) {
                        options_selection++;
                        PlaySfx(sndHit);
                    }
                }
            }

            if (m_up) {
                if (options_selection > 0) {
                    options_selection--;
                    PlaySfx(sndHit);
                }
            }
            if (m_down) {
                if (options_selection < total_opts - 1) {
                    options_selection++;
                    PlaySfx(sndHit);
                }
            }

            if (options_selection < options_scroll_offset) options_scroll_offset = options_selection;
            if (options_selection >= options_scroll_offset + 3) options_scroll_offset = options_selection - 2;

            char opt_strings[8][64];
            snprintf(opt_strings[0], sizeof(opt_strings[0]), "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(opt_strings[1], sizeof(opt_strings[1]), "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(opt_strings[2], sizeof(opt_strings[2]), "%s: < %s >", T(STR_FILTER), GetFilterName(g_config.crt_filter, g_config.language));
            snprintf(opt_strings[3], sizeof(opt_strings[3]), "%s", T(STR_CONTROLS));
            snprintf(opt_strings[4], sizeof(opt_strings[4]), "%s: < %s >", T(STR_LANGUAGE), g_lang_names[g_config.language]);
            snprintf(opt_strings[5], sizeof(opt_strings[5]), "%s: < %s >", T(STR_SCREEN_MODE), g_screen_mode_names[g_config.screen_mode]);
            snprintf(opt_strings[6], sizeof(opt_strings[6]), "%s: < %s >", T(STR_FPS), GetFpsOptionText(g_config.target_fps));
            snprintf(opt_strings[7], sizeof(opt_strings[7]), "%s", T(STR_DELETE_RECORDS));

            int row_w = 175;
            int row_x = 25;
            for (int slot = 0; slot < 3; slot++) {
                int opt_idx = options_scroll_offset + slot;
                if (opt_idx >= total_opts) break;
                int row_y = opt_start_y + (slot * 30);
                if (mouse_clicked && mouse_x >= row_x && mouse_x <= row_x + row_w && mouse_y >= row_y - 2 && mouse_y <= row_y + 22) {
                    if (options_selection == opt_idx) {
                        const char* str = opt_strings[opt_idx];
                        const char* left_arrow = strchr(str, '<');
                        const char* right_arrow = strrchr(str, '>');

                        if (left_arrow && right_arrow) {
                            if (mouse_x < 100) {
                                m_left = true;
                            } else if (mouse_x > 140) {
                                m_right = true;
                            } else {
                                m_accept = true;
                            }
                        } else {
                            m_accept = true;
                        }
                    } else {
                        options_selection = opt_idx;
                        PlaySfx(sndHit);
                    }
                }
            }

            if (options_selection == 0) { 
                if ((m_right || m_accept) && g_config.vol_bgm < 10) { g_config.vol_bgm++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_bgm > 0) { g_config.vol_bgm--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 1) { 
                if ((m_right || m_accept) && g_config.vol_sfx < 10) { g_config.vol_sfx++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_sfx > 0) { g_config.vol_sfx--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 2) { 
                if (m_right || m_accept) { g_config.crt_filter = (g_config.crt_filter + 1) % FILTER_COUNT; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.crt_filter = (g_config.crt_filter - 1 + FILTER_COUNT) % FILTER_COUNT; saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 3) { 
                if (m_accept) { state = 4; controls_origin_state = 7; controls_selection = 0; rebinding_action = -1; just_entered_menu = true; PlaySfx(sndHit); }
            } else if (options_selection == 4) { 
                if (m_right || m_accept) { g_config.language = (g_config.language + 1) % LANG_COUNT; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.language = (g_config.language - 1 + LANG_COUNT) % LANG_COUNT; saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 5) { 
                if (m_right || m_accept) { g_config.screen_mode = (g_config.screen_mode + 1) % 3; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.screen_mode = (g_config.screen_mode - 1 + 3) % 3; saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 6) { 
                if (m_right || m_accept) { g_config.target_fps = (g_config.target_fps + 1) % FPS_OPTION_COUNT; ApplyFpsSetting(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.target_fps = (g_config.target_fps - 1 + FPS_OPTION_COUNT) % FPS_OPTION_COUNT; ApplyFpsSetting(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 7) { 
                if (m_accept) { state = 10; confirm_selection = 0; just_entered_menu = true; PlaySfx(sndHit); }
            }

            if (m_back) { state = 0; PlayGameBgm(0); just_entered_menu = true; }
        }
        else if (state == 12) {
            bool pause_options_input_locked = (pause_options_enter_timer > 0.0f);
            if (pause_options_input_locked) {
                m_up = false;
                m_down = false;
                m_left = false;
                m_right = false;
                m_accept = false;
                m_back = false;
                mouse_clicked = false;
                mouse_wheel = 0;
            }

            const int total_pause_opts = 6;
            const int max_pause_scroll = 3;

            if (mouse_wheel != 0) {
                int scroll_delta = (int)roundf(mouse_wheel);
                if (scroll_delta == 0) scroll_delta = (mouse_wheel > 0.0f) ? 1 : -1;
                pause_options_scroll_offset -= scroll_delta;
                if (pause_options_scroll_offset < 0) pause_options_scroll_offset = 0;
                if (pause_options_scroll_offset > max_pause_scroll) pause_options_scroll_offset = max_pause_scroll;
                if (pause_options_selection < pause_options_scroll_offset) pause_options_selection = pause_options_scroll_offset;
                if (pause_options_selection >= pause_options_scroll_offset + 3) pause_options_selection = pause_options_scroll_offset + 2;
            }

            int opt_top = (SCREEN_H - 130) / 2;
            if (opt_top < 15) opt_top = 15;
            int opt_start_y = opt_top + 40;

            // Botones triangulares en el lateral derecho (x: 200..234)
            if (mouse_clicked && mouse_x >= 200 && mouse_x <= 234) {
                if (mouse_y >= 44 && mouse_y <= 84) {
                    if (pause_options_selection > 0) {
                        pause_options_selection--;
                        PlaySfx(sndHit);
                    }
                } else if (mouse_y >= 94 && mouse_y <= 136) {
                    if (pause_options_selection < total_pause_opts - 1) {
                        pause_options_selection++;
                        PlaySfx(sndHit);
                    }
                }
            }

            if (m_up) {
                if (pause_options_selection > 0) {
                    pause_options_selection--;
                    PlaySfx(sndHit);
                }
            }
            if (m_down) {
                if (pause_options_selection < total_pause_opts - 1) {
                    pause_options_selection++;
                    PlaySfx(sndHit);
                }
            }

            if (pause_options_selection < pause_options_scroll_offset) pause_options_scroll_offset = pause_options_selection;
            if (pause_options_selection >= pause_options_scroll_offset + 3) pause_options_scroll_offset = pause_options_selection - 2;

            char pause_opt_strings[6][64];
            snprintf(pause_opt_strings[0], sizeof(pause_opt_strings[0]), "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(pause_opt_strings[1], sizeof(pause_opt_strings[1]), "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(pause_opt_strings[2], sizeof(pause_opt_strings[2]), "%s: < %s >", T(STR_FILTER), GetFilterName(g_config.crt_filter, g_config.language));
            snprintf(pause_opt_strings[3], sizeof(pause_opt_strings[3]), "%s", T(STR_CONTROLS));
            snprintf(pause_opt_strings[4], sizeof(pause_opt_strings[4]), "%s: < %s >", T(STR_SCREEN_MODE), g_screen_mode_names[g_config.screen_mode]);
            snprintf(pause_opt_strings[5], sizeof(pause_opt_strings[5]), "%s: < %s >", T(STR_FPS), GetFpsOptionText(g_config.target_fps));

            int p_row_w = 175;
            int p_row_x = 25;
            for (int slot = 0; slot < 3; slot++) {
                int opt_idx = pause_options_scroll_offset + slot;
                if (opt_idx >= total_pause_opts) break;
                int row_y = opt_start_y + (slot * 30);
                if (mouse_clicked && mouse_x >= p_row_x && mouse_x <= p_row_x + p_row_w && mouse_y >= row_y - 2 && mouse_y <= row_y + 22) {
                    if (pause_options_selection == opt_idx) {
                        const char* str = pause_opt_strings[opt_idx];
                        const char* left_arrow = strchr(str, '<');
                        const char* right_arrow = strrchr(str, '>');

                        if (left_arrow && right_arrow) {
                            if (mouse_x < 100) {
                                m_left = true;
                            } else if (mouse_x > 140) {
                                m_right = true;
                            } else {
                                m_accept = true;
                            }
                        } else {
                            m_accept = true;
                        }
                    } else {
                        pause_options_selection = opt_idx;
                        PlaySfx(sndHit);
                    }
                }
            }

            if (pause_options_selection == 0) { 
                if ((m_right || m_accept) && g_config.vol_bgm < 10) { g_config.vol_bgm++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_bgm > 0) { g_config.vol_bgm--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 1) { 
                if ((m_right || m_accept) && g_config.vol_sfx < 10) { g_config.vol_sfx++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_sfx > 0) { g_config.vol_sfx--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 2) { 
                if (m_right || m_accept) { g_config.crt_filter = (g_config.crt_filter + 1) % FILTER_COUNT; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.crt_filter = (g_config.crt_filter - 1 + FILTER_COUNT) % FILTER_COUNT; saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 3) { 
                if (m_accept) { state = 4; controls_origin_state = 12; controls_selection = 0; rebinding_action = -1; just_entered_menu = true; PlaySfx(sndHit); }
            } else if (pause_options_selection == 4) { 
                if (m_right || m_accept) { g_config.screen_mode = (g_config.screen_mode + 1) % 3; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.screen_mode = (g_config.screen_mode - 1 + 3) % 3; saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 5) { 
                if (m_right || m_accept) { g_config.target_fps = (g_config.target_fps + 1) % FPS_OPTION_COUNT; ApplyFpsSetting(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.target_fps = (g_config.target_fps - 1 + FPS_OPTION_COUNT) % FPS_OPTION_COUNT; ApplyFpsSetting(); saveConfigPC(); PlaySfx(sndHit); }
            }

            if (m_back) { state = 3; just_entered_menu = true; }
        }
        else if (state == 10) {
            if (m_up || m_down) confirm_selection = 1 - confirm_selection;

            int d_box_y = (SCREEN_H - 90) / 2;
            int d_btn_w = 150;
            int d_btn_x = (SCREEN_W - d_btn_w) / 2;
            for (int conf_idx = 0; conf_idx < 2; conf_idx++) {
                int opt_y = d_box_y + 51 + (conf_idx * 14);
                if (mouse_x >= d_btn_x && mouse_x <= d_btn_x + d_btn_w && mouse_y >= opt_y - 2 && mouse_y <= opt_y + 10) {
                    if (mouse_moved) confirm_selection = conf_idx;
                    if (mouse_clicked) { confirm_selection = conf_idx; m_accept = true; }
                }
            }

            if (m_accept) {
                if (confirm_selection == 1) {
                    clearAllHighScoresPC();
                }
                state = 7;
                just_entered_menu = true;
                PlaySfx(sndHit);
            }
            if (m_back) {
                state = 7;
                just_entered_menu = true;
            }
        }
        else if (state == 6 || state == 15) {
            PlayGameBgm(2);

            if (state == 6 && pad_rb_pressed) {
                state = 15;
                show_ach_details = false;
                PlaySfx(sndHit);
                just_entered_menu = true;
            } else if (state == 15 && pad_lb_pressed) {
                state = 6;
                show_ach_details = false;
                records_mode_tab = 0;
                PlaySfx(sndHit);
                just_entered_menu = true;
            }

            int rec_top = (SCREEN_H - 130) / 2;
            if (rec_top < 15) rec_top = 15;

            int tabs_center = SCREEN_W / 2;
            if (mouse_clicked && mouse_y >= rec_top - 1 && mouse_y <= rec_top + 17) {
                if (mouse_x >= tabs_center - 95 && mouse_x <= tabs_center - 15) { state = 6; show_ach_details = false; records_mode_tab = 0; PlaySfx(sndHit); }
                if (mouse_x >= tabs_center + 10 && mouse_x <= tabs_center + 90) { state = 15; show_ach_details = false; PlaySfx(sndHit); }
            }

            if (state == 6) {
                int rec_w = 170;
                int rec_x = (SCREEN_W - rec_w) / 2;
                int btn_w = 16;
                int btn_h = 32;
                int left_arrow_x = (rec_x - btn_w) / 2;
                int right_arrow_x = (rec_x + rec_w) + (SCREEN_W - (rec_x + rec_w) - btn_w) / 2;
                int arrow_y = rec_top + 68 - (btn_h / 2);

                if (records_mode_tab == 0) {
                    if (m_right) {
                        records_mode_tab = 1;
                        PlaySfx(sndHit);
                    }
                    if (mouse_clicked && mouse_x >= right_arrow_x - 6 && mouse_x <= right_arrow_x + btn_w + 6 &&
                        mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6) {
                        records_mode_tab = 1;
                        PlaySfx(sndHit);
                    }
                } else if (records_mode_tab == 1) {
                    if (m_left) {
                        records_mode_tab = 0;
                        PlaySfx(sndHit);
                    }
                    if (m_right) {
                        records_mode_tab = 2;
                        PlaySfx(sndHit);
                    }
                    if (mouse_clicked && mouse_x >= left_arrow_x - 6 && mouse_x <= left_arrow_x + btn_w + 6 &&
                        mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6) {
                        records_mode_tab = 0;
                        PlaySfx(sndHit);
                    }
                    if (mouse_clicked && mouse_x >= right_arrow_x - 6 && mouse_x <= right_arrow_x + btn_w + 6 &&
                        mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6) {
                        records_mode_tab = 2;
                        PlaySfx(sndHit);
                    }
                } else if (records_mode_tab == 2) {
                    if (m_left) {
                        records_mode_tab = 1;
                        PlaySfx(sndHit);
                    }
                    if (m_right) {
                        records_mode_tab = 3;
                        PlaySfx(sndHit);
                    }
                    if (mouse_clicked && mouse_x >= left_arrow_x - 6 && mouse_x <= left_arrow_x + btn_w + 6 &&
                        mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6) {
                        records_mode_tab = 1;
                        PlaySfx(sndHit);
                    }
                    if (mouse_clicked && mouse_x >= right_arrow_x - 6 && mouse_x <= right_arrow_x + btn_w + 6 &&
                        mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6) {
                        records_mode_tab = 3;
                        PlaySfx(sndHit);
                    }
                } else if (records_mode_tab == 3) {
                    if (m_left) {
                        records_mode_tab = 2;
                        PlaySfx(sndHit);
                    }
                    if (mouse_clicked && mouse_x >= left_arrow_x - 6 && mouse_x <= left_arrow_x + btn_w + 6 &&
                        mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6) {
                        records_mode_tab = 2;
                        PlaySfx(sndHit);
                    }
                }
            }

            if (state == 15) {
                if (m_left)  { selected_ach_index = (selected_ach_index > 0) ? selected_ach_index - 1 : (NUM_ACHIEVEMENTS - 1); PlaySfx(sndHit); }
                if (m_right) { selected_ach_index = (selected_ach_index < NUM_ACHIEVEMENTS - 1) ? selected_ach_index + 1 : 0; PlaySfx(sndHit); }
                if (m_up)    { selected_ach_index = (selected_ach_index >= 4) ? selected_ach_index - 4 : selected_ach_index; PlaySfx(sndHit); }
                if (m_down)  { selected_ach_index = (selected_ach_index + 4 < NUM_ACHIEVEMENTS) ? selected_ach_index + 4 : selected_ach_index; PlaySfx(sndHit); }

                // Scroll con rueda del ratón
                if (mouse_wheel < 0.0f) {
                    int cur_p = selected_ach_index / 8;
                    int max_p = ((NUM_ACHIEVEMENTS + 7) / 8) - 1;
                    if (cur_p < max_p) {
                        cur_p++;
                        selected_ach_index = cur_p * 8 + (selected_ach_index % 8);
                        if (selected_ach_index >= NUM_ACHIEVEMENTS) selected_ach_index = NUM_ACHIEVEMENTS - 1;
                        show_ach_details = false;
                        PlaySfx(sndHit);
                    }
                } else if (mouse_wheel > 0.0f) {
                    int cur_p = selected_ach_index / 8;
                    if (cur_p > 0) {
                        cur_p--;
                        selected_ach_index = cur_p * 8 + (selected_ach_index % 8);
                        show_ach_details = false;
                        PlaySfx(sndHit);
                    }
                }

                // Botones triangulares en lateral derecho (x: 198..234)
                if (mouse_clicked && mouse_x >= 198 && mouse_x <= 234) {
                    int total_pages = (NUM_ACHIEVEMENTS + 7) / 8;
                    int cur_p = selected_ach_index / 8;
                    if (mouse_y >= 44 && mouse_y <= 84) {
                        // Triangulo Arriba: sube de pagina si no esta en la primera
                        if (cur_p > 0) {
                            selected_ach_index -= 8;
                            if (selected_ach_index < 0) selected_ach_index = 0;
                            show_ach_details = false;
                            PlaySfx(sndHit);
                        }
                    } else if (mouse_y >= 94 && mouse_y <= 136) {
                        // Triangulo Abajo: baja de pagina si no esta en la ultima
                        if (cur_p < total_pages - 1) {
                            selected_ach_index += 8;
                            if (selected_ach_index >= NUM_ACHIEVEMENTS) selected_ach_index = NUM_ACHIEVEMENTS - 1;
                            show_ach_details = false;
                            PlaySfx(sndHit);
                        }
                    }
                }

                if (show_ach_details && mouse_clicked && (mouse_x < 198)) {
                    show_ach_details = false;
                    PlaySfx(sndHit);
                } else if (!show_ach_details && mouse_clicked && (mouse_x < 198)) {
                    int grid_start_x = 16;
                    for (int ach_i = 0; ach_i < NUM_ACHIEVEMENTS; ach_i++) {
                        int page_base = (selected_ach_index / 8) * 8;
                        int i_local = ach_i - page_base;
                        if (i_local < 0 || i_local >= 8) continue;

                        int row = i_local / 4;
                        int col = i_local % 4;
                        int bx = grid_start_x + (col * 45);
                        int by = rec_top + 22 + (row * 35);
                        if (mouse_x >= bx && mouse_x <= bx + 36 && mouse_y >= by && mouse_y <= by + 32) {
                            selected_ach_index = ach_i;
                            show_ach_details = true;
                            PlaySfx(sndHit);
                            break;
                        }
                    }
                }

                if (m_accept) {
                    show_ach_details = !show_ach_details;
                    PlaySfx(sndHit);
                }
            }

            if (m_back) {
                if (state == 15 && show_ach_details) {
                    show_ach_details = false;
                    PlaySfx(sndHit);
                } else {
                    state = 0; PlayGameBgm(0); just_entered_menu = true;
                }
            }
        }
        else if (state == 4) {
            if (just_entered_menu) {
                m_accept = false;
                m_back = false;
                current_frame_key = 0;
                just_entered_menu = false;
            }

            if (g_last_input_device != INPUT_GAMEPAD) {
                if (mouse_clicked || m_back || m_accept) {
                    just_entered_menu = true;
                    state = controls_origin_state;
                    PlaySfx(sndHit);
                }
            } else {
                if (rebinding_action != -1) {
                    for (int b = 1; b <= 17; b++) {
                        if (IsGamepadButtonPressed(pad_id, b)) {
                            if (IsPadButtonAssignedPC(b, rebinding_action)) {
                                duplicate_key_warning = 120;
                                duplicate_key_warning_f = 120.0f;
                                PlaySfx(sndHurt);
                            } else {
                                SetPadBindingPC(rebinding_action, b);
                                saveControlsPC();
                                PlaySfx(sndHit);
                            }
                            rebinding_action = -1;
                            just_entered_menu = true;
                            break;
                        }
                    }
                } else {
                    if (m_up)   controls_selection = (controls_selection - 1 + 8) % 8;
                    if (m_down) controls_selection = (controls_selection + 1) % 8;

                    int c_box_w = 210;
                    int c_box_x = (SCREEN_W - c_box_w) / 2;
                    int c_box_y = 42;
                    for (int act_idx = 0; act_idx < 8; act_idx++) {
                        int row_y = c_box_y + 4 + (act_idx * 11);
                        if (mouse_x >= c_box_x && mouse_x <= c_box_x + c_box_w && mouse_y >= row_y - 2 && mouse_y <= row_y + 9) {
                            if (mouse_moved) controls_selection = act_idx;
                            if (mouse_clicked) {
                                controls_selection = act_idx;
                                if (mouse_x >= c_box_x + 125 && mouse_x <= c_box_x + c_box_w - 2) {
                                    rebinding_action = act_idx;
                                    PlaySfx(sndPowerUp);
                                }
                            }
                        }
                    }

                    if (m_accept && rebinding_action == -1) {
                        rebinding_action = controls_selection;
                        PlaySfx(sndPowerUp);
                    }

                    if (m_back && rebinding_action == -1) {
                        just_entered_menu = true;
                        state = controls_origin_state;
                    }
                }
            }
        }
        else if (state == 2) {
            PlayGameBgm(4);
            if (m_up || m_down) gameover_selection = 1 - gameover_selection;

            int go_top = (SCREEN_H - 128) / 2;
            int go_btn_w = 150;
            int go_btn_x = (SCREEN_W - go_btn_w) / 2;
            for (int go_idx = 0; go_idx < 2; go_idx++) {
                int opt_y = go_top + 96 + (go_idx * 18);
                if (mouse_x >= go_btn_x && mouse_x <= go_btn_x + go_btn_w && mouse_y >= opt_y - 3 && mouse_y <= opt_y + 11) {
                    if (mouse_moved) gameover_selection = go_idx;
                    if (mouse_clicked) { gameover_selection = go_idx; m_accept = true; }
                }
            }

            if (m_accept) {
                StopAllBgm();
                if (gameover_selection == 0) {
                    ResetActiveCheatsOnGameEnd();
                    player_hp = (current_game_mode == MODE_RUSH) ? 3 : ((current_game_mode == MODE_TIME_ATTACK || current_game_mode == MODE_ASSAULT) ? 5 : ((current_difficulty == 0) ? 5 : ((current_difficulty == 3) ? 1 : 3)));
                    score = 0; px = (SCREEN_W - 10) / 2; py = SCREEN_H - 35; player_x = (float)px; player_y = (float)py; prev_player_x = player_x; prev_player_y = player_y; dir_x = 0; dir_y = -1; p_type = 0; p_up.active = 0;
                    slowmo_timer = 0; nuke_timer = 0; dash_cd = 0; dash_invincibility = 0; death_timer = 0; combo = 1; combo_timer = 0; wave = 1;
                    if (current_game_mode == MODE_RUSH) {
                        rush_kills_in_wave = 0;
                        rush_run_kills = 0;
                        rush_target_kills = 20;
                        rush_cycle = 0;
                        rush_wave_banner_timer = 0;
                    } else if (current_game_mode == MODE_TIME_ATTACK) {
                        ta_time_left_frames = 30 * 60;
                        ta_time_survived_frames = 0;
                        ta_run_kills = 0;
                        ta_spawn_rate = 75;
                        ta_spawn_timer = 0;
                        ta_time_up = false;
                    } else if (current_game_mode == MODE_ASSAULT) {
                        assault_timer_frames = 15 * 60;
                        assault_time_survived_frames = 0;
                        assault_run_kills = 0;
                        assault_banner_timer = 0;
                        g_assault_weapons_killed_mask = 0;
                    }
                    hell_no_damage_frames = 0; player_took_hit_this_wave = false;
                    g_waves_without_damage_streak = 0; g_boss_dash_used = false; g_consecutive_bullets_dodged = 0;
                    g_divisor_combo_active = false; g_divisor_fragments_left = 0; g_divisor_combo_timer = 0;
                    g_powerups_picked_this_wave = 0; g_powerups_picked_this_run = 0;
                    for(int e_idx=0; e_idx<MAX_ENEMIES; e_idx++) { enemies[e_idx].active = 0; enemies[e_idx].wet_timer = 0; enemy_fx[e_idx] = 0.0f; enemy_fy[e_idx] = 0.0f; prev_enemy_fx[e_idx] = 0.0f; prev_enemy_fy[e_idx] = 0.0f; }
                    for(int b_idx=0; b_idx<MAX_BULLETS; b_idx++) bullets[b_idx].active = 0;
                    for(int eb_idx=0; eb_idx<MAX_E_BULLETS; eb_idx++) e_bullets[eb_idx].active = 0;
                    for(int ex_idx=0; ex_idx<MAX_EXPLO; ex_idx++) explosions[ex_idx].active = 0;
                    for (int pi = 0; pi < MAX_PARTICLES; pi++) particles[pi].life = 0;
                    for (int gi = 0; gi < MAX_GHOST_TRAILS; gi++) ghost_trails[gi].timer = 0;
                    for (int fti = 0; fti < MAX_TEXTS; fti++) ftexts[fti].timer = 0;
                    g_damage_flash_timer = 0;
                    g_dash_ready_flash = 0;
                    prev_dash_cd = 0;
                    ready_timer = 120; pause_resume_timer = 0.25f; pause_toggle_timer = 0.35f; state = 9; 
                } else {
                    saveAchievementsPC();
                    ResetActiveCheatsOnGameEnd();
                    state = 0; PlayGameBgm(0); just_entered_menu = true;
                }
            }
        }
        else if (state == 3) {
            if (k_start && pause_toggle_timer <= 0.0f) {
                state = 1;
                pause_selection = 0;
                pause_toggle_timer = 0.35f;
                pause_resume_timer = 0.25f;
                k_start = false;
                pad_btn_start = false;
                m_back = false;
                m_accept = false;
                pad_menu_back = false;
                just_entered_menu = true;
            }

            if (m_up) pause_selection = (pause_selection - 1 + 3) % 3;
            if (m_down) pause_selection = (pause_selection + 1) % 3;

            int pause_y = 26;
            int p_btn_w = 156;
            int p_btn_x = (SCREEN_W - p_btn_w) / 2;
            for (int p_idx = 0; p_idx < 3; p_idx++) {
                int py_pos = pause_y + 38 + (p_idx * 28);
                if (mouse_x >= p_btn_x && mouse_x <= p_btn_x + p_btn_w && mouse_y >= py_pos - 3 && mouse_y <= py_pos + 15) {
                    if (mouse_moved) pause_selection = p_idx;
                    if (mouse_clicked) { pause_selection = p_idx; m_accept = true; }
                }
            }

            if (m_accept) {
                if (pause_selection == 0) {
                    state = 1;
                    pause_resume_timer = 0.25f;
                    pause_toggle_timer = 0.35f;
                    k_start = false;
                    pad_btn_start = false;
                    m_back = false;
                    m_accept = false;
                    pad_menu_back = false;
                    just_entered_menu = true;
                }
                else if (pause_selection == 1) {
                    state = 12;
                    pause_options_selection = 0;
                    pause_options_scroll_offset = 0;
                    pause_options_enter_timer = 0.15f;
                    is_dragging_scrollbar = false;
                    StopAllBgm();
                    just_entered_menu = true;
                }
                else if (pause_selection == 2) {
                    saveAchievementsPC();
                    ResetActiveCheatsOnGameEnd();
                    state = 0;
                    PlayGameBgm(0);
                    just_entered_menu = true;
                }
            }
            if (m_back) {
                state = 1;
                pause_resume_timer = 0.25f;
                pause_toggle_timer = 0.35f;
                k_start = false;
                pad_btn_start = false;
                m_back = false;
                m_accept = false;
                pad_menu_back = false;
                just_entered_menu = true;
            }
        }

        // --- BUCLE FIXED UPDATE (LÓGICA FÍSICA A 60 HZ) ---
        while (time_accumulator >= FIXED_DELTA) {
            time_accumulator -= FIXED_DELTA;
            frame_count++;

            prev_player_x = player_x;
            prev_player_y = player_y;
            for (int e_prev = 0; e_prev < MAX_ENEMIES; e_prev++) {
                prev_enemy_fx[e_prev] = enemy_fx[e_prev];
                prev_enemy_fy[e_prev] = enemy_fy[e_prev];
            }

            for (int ft_idx = 0; ft_idx < MAX_TEXTS; ft_idx++) {
                if (ftexts[ft_idx].timer > 0) {
                    ftexts[ft_idx].timer--;
                    ftexts[ft_idx].y -= 0.4f;
                }
            }

            for (int pi = 0; pi < MAX_PARTICLES; pi++) {
                if (particles[pi].life > 0) {
                    particles[pi].life--;
                    particles[pi].x += particles[pi].vx;
                    particles[pi].y += particles[pi].vy;
                }
            }
            for (int gi = 0; gi < MAX_GHOST_TRAILS; gi++) {
                if (ghost_trails[gi].timer > 0) ghost_trails[gi].timer--;
            }
            for (int en = 0; en < MAX_ENEMIES; en++) {
                if (enemies[en].active && enemies[en].hit_flash_timer > 0) enemies[en].hit_flash_timer--;
            }
            if (g_damage_flash_timer > 0) g_damage_flash_timer--;
            if (screen_shake_timer > 0) screen_shake_timer--;

            if (state == 14) {
                intro_timer++;
                if (intro_timer > 250 || m_accept || mouse_clicked || GetTouchPointCount() > 0) {
                    state = 0;
                    PlayGameBgm(0);
                }
            } 
            else if (state == 0 || state == 13) {
                for(int s = 0; s < MAX_TITLE_STARS; s++) {
                    title_stars[s].y += title_stars[s].speed;
                    if (title_stars[s].y >= SCREEN_H - 2) {
                        title_stars[s].y = 2;
                        title_stars[s].x = rand() % (SCREEN_W - 8) + 4;
                    }
                }
            }
            else if (state == 5 || state == 6 || state == 15 || state == 7 || state == 12 || state == 4 || state == 16 || state == 3) {
                for(int m = 0; m < MAX_MENU_STARS; m++) {
                    menu_stars[m].y -= menu_stars[m].vy;
                    if(menu_stars[m].y <= 2) {
                        menu_stars[m].y = SCREEN_H - 4;
                        menu_stars[m].x = rand() % (SCREEN_W - 12) + 6;
                    }
                }
            }
            else if (state == 9) {
                UpdateReadyJinglePC(ready_timer);
                ready_timer--;
                if (ready_timer <= 0) {
                    state = 1; 
                    if (current_game_mode == MODE_RUSH) {
                        rush_wave_banner_timer = 90;
                    }
                }
            }
            else if (state == 8) {
                death_timer--;
                if (death_timer <= 0) {
                    if (current_game_mode == MODE_RUSH) {
                        saveRushHighScorePC(score, wave, rush_run_kills);
                        g_achievement_progress[ACH_RUSH_MUERTE_SUBITA]++;
                        if (g_achievement_progress[ACH_RUSH_MUERTE_SUBITA] >= 50) {
                            UnlockAchievement(ACH_RUSH_MUERTE_SUBITA);
                        }
                    } else if (current_game_mode == MODE_TIME_ATTACK) {
                        int surv_sec = ta_time_survived_frames / 60;
                        saveTimeAttackHighScorePC(surv_sec, score, ta_run_kills);
                    } else if (current_game_mode == MODE_ASSAULT) {
                        int surv_sec = assault_time_survived_frames / 60;
                        saveAssaultHighScorePC(surv_sec, score, assault_run_kills);
                    } else {
                        saveHighScorePC(current_difficulty, score);
                    }
                    saveAchievementsPC();
                    gameover_selection = 0;
                    state = 2;
                    PlayGameBgm(4);
                }
            }
            else if (state == 2) {
                for(int b = 0; b < MAX_BLOOD_DROPS; b++) {
                    blood_drops[b].y -= blood_drops[b].speed;
                    if(blood_drops[b].y <= 2) {
                        blood_drops[b].y = SCREEN_H - 4;
                        blood_drops[b].x = rand() % (SCREEN_W - 8) + 4;
                    }
                }
            }
            else if (state == 1) {
                if (nuke_timer > 0) {
                    nuke_timer--;
                    invincibility = 5;
                    screen_shake_timer = 20;
                    int nuke_kills_count = 0;
                    for(int k = 0; k < MAX_ENEMIES; k++) {
                        if (enemies[k].active) {
                            KillEnemy(k, (KillContext){ .cause = KILL_CAUSE_NUKE, .weapon_type = -1, .during_slowmo = (slowmo_timer > 0) });
                            nuke_kills_count++;
                        }
                    }
                    if (nuke_kills_count >= 5) UnlockAchievement(ACH_BOOM);
                    if (nuke_kills_count >= 10) UnlockAchievement(ACH_LIMPIEZA_TOTAL);
                    for(int k = 0; k < MAX_E_BULLETS; k++) e_bullets[k].active = 0;
                } else if (slowmo_timer > 0) {
                    slowmo_timer--;
                }

                for(int g = 0; g < MAX_GAME_STARS; g++) {
                    game_stars[g].y += game_stars[g].speed;
                    if (game_stars[g].y >= SCREEN_H - 2) {
                        game_stars[g].y = 18;
                        game_stars[g].x = rand() % (SCREEN_W - 8) + 4;
                    }
                }

                for(int ex_i = 0; ex_i < MAX_EXPLO; ex_i++) {
                    if (explosions[ex_i].active) {
                        explosions[ex_i].timer--;
                        if (explosions[ex_i].timer <= 0) explosions[ex_i].active = 0;
                    }
                }

                if (invincibility > 0) invincibility--;
                if (dash_invincibility > 0) dash_invincibility--;
                if (dash_invincibility < 0) dash_invincibility = 0;
                if (dash_cd > 0) dash_cd--;
                if (prev_dash_cd > 0 && dash_cd == 0) g_dash_ready_flash = 12;
                if (g_dash_ready_flash > 0) g_dash_ready_flash--;
                prev_dash_cd = dash_cd;
                if (g_divisor_combo_timer > 0) {
                    g_divisor_combo_timer--;
                    if (g_divisor_combo_timer == 0) g_divisor_combo_active = false;
                }
                if (p_timer > 0) {
                    p_timer--;
                    if (p_timer == 0) { p_type = 0; if (combo > 1) combo = 1; }
                }

                if (current_game_mode == MODE_ASSAULT) {
                    assault_time_survived_frames++;
                    int a_sec = assault_time_survived_frames / 60;
                    if (a_sec > g_achievement_progress[ACH_ASSAULT_1MIN]) {
                        g_achievement_progress[ACH_ASSAULT_1MIN] = (a_sec > 60) ? 60 : a_sec;
                    }
                    if (a_sec > g_achievement_progress[ACH_ASSAULT_3MIN]) {
                        g_achievement_progress[ACH_ASSAULT_3MIN] = (a_sec > 180) ? 180 : a_sec;
                    }
                    if (a_sec > g_achievement_progress[ACH_ASSAULT_5MIN]) {
                        g_achievement_progress[ACH_ASSAULT_5MIN] = (a_sec > 300) ? 300 : a_sec;
                    }

                    if (assault_time_survived_frames >= 60 * 60) UnlockAchievement(ACH_ASSAULT_1MIN);
                    if (assault_time_survived_frames >= 180 * 60) UnlockAchievement(ACH_ASSAULT_3MIN);
                    if (assault_time_survived_frames >= 300 * 60) UnlockAchievement(ACH_ASSAULT_5MIN);

                    if (assault_banner_timer > 0) assault_banner_timer--;
                    assault_timer_frames--;
                    if (assault_timer_frames <= 0) {
                        assault_timer_frames = 15 * 60;
                        const int assault_pool[7] = { 2, 3, 5, 7, 9, 12, 14 };
                        int cur_idx = -1;
                        for (int i = 0; i < 7; i++) {
                            if (assault_pool[i] == p_type) { cur_idx = i; break; }
                        }
                        int next_idx = rand() % 7;
                        if (next_idx == cur_idx) next_idx = (next_idx + 1 + (rand() % 6)) % 7;
                        int chosen_pu = assault_pool[next_idx];

                        if (chosen_pu != 7 && slowmo_timer > 0) slowmo_timer = 0;

                        p_type = chosen_pu;
                        p_timer = 15 * 60;

                        if (chosen_pu == 2) {
                            assault_cur_pu_name_id = STR_PU_RAPID_NAME;
                            assault_cur_pu_color = C_RAPID;
                            PlaySfx(sndPowerUp);
                        } else if (chosen_pu == 3) {
                            assault_cur_pu_name_id = STR_PU_TRIPLE_NAME;
                            assault_cur_pu_color = C_GREEN;
                            PlaySfx(sndPowerUp);
                        } else if (chosen_pu == 5) {
                            assault_cur_pu_name_id = STR_PU_HEAVY_NAME;
                            assault_cur_pu_color = GBA_COLOR(31, 15, 0);
                            PlaySfx(sndPowerUp);
                        } else if (chosen_pu == 7) {
                            assault_cur_pu_name_id = STR_PU_SLOWMO_NAME;
                            assault_cur_pu_color = C_ICE;
                            slowmo_timer = 15 * 60;
                            PlaySfx(sndPowerUp);
                        } else if (chosen_pu == 9) {
                            assault_cur_pu_name_id = STR_PU_FREEZE_NAME;
                            assault_cur_pu_color = C_ICE;
                            PlaySfx(sndPowerUp);
                        } else if (chosen_pu == 12) {
                            assault_cur_pu_name_id = STR_PU_POISON_NAME;
                            assault_cur_pu_color = GBA_COLOR(20, 0, 31);
                            PlaySfx(sndPowerUp);
                        } else if (chosen_pu == 14) {
                            assault_cur_pu_name_id = STR_PU_WATER_NAME;
                            assault_cur_pu_color = C_NAVY_BLUE;
                            PlaySfx(sndPowerUp);
                        }

                        assault_banner_timer = 110;
                        SpawnBulletSparks(player_x + 5.0f, player_y + 5.0f, assault_cur_pu_color);
                    }
                }

                if (p_type == 8) combo = 5;
                else if (p_type == 13) combo = 10;
                else if (combo > 1) {
                    if (combo_timer > 0) combo_timer--;
                    else combo = 1;
                }

                if (combo >= 10 && p_type == 13) {
                    UnlockAchievement(ACH_MODO_DIOS);
                }

                bool is_sprinting = IsKeyDown(g_keys.key_turbo) || (pad_active && IsGamepadButtonDown(pad_id, g_pad.btn_turbo)) || mobile.turbo_down;
                int speed = is_sprinting ? 4 : 2;
                int mx_dir = 0, my_dir = 0;

                if (p_type == 10) { 
                    if (k_hold_left)  mx_dir = 1;
                    if (k_hold_right) mx_dir = -1;
                    if (k_hold_up)    my_dir = 1;
                    if (k_hold_down)  my_dir = -1;
                } else {
                    if (k_hold_left)  mx_dir = -1;
                    if (k_hold_right) mx_dir = 1;
                    if (k_hold_up)    my_dir = -1;
                    if (k_hold_down)  my_dir = 1;
                }

                int dash_dx = mx_dir;
                int dash_dy = my_dir;
                if (dash_dx == 0 && dash_dy == 0) {
                    dash_dx = dir_x;
                    dash_dy = dir_y;
                }

                if (k_dash && dash_cd == 0 && (dash_dx != 0 || dash_dy != 0)) {
                    PlaySfx(sndDash);
                    float d_mult = (dash_dx != 0 && dash_dy != 0) ? 0.70710678f : 1.0f;
                    float dash_dist = 28.0f * d_mult;
                    float start_dash_x = player_x;
                    float start_dash_y = player_y;

                    player_x += (float)dash_dx * dash_dist;
                    player_y += (float)dash_dy * dash_dist;
                    if (player_x < 0.0f) player_x = 0.0f;
                    if (player_x > (float)(SCREEN_W - 10)) player_x = (float)(SCREEN_W - 10);
                    if (player_y < 16.0f) player_y = 16.0f;
                    if (player_y > (float)(SCREEN_H - 10)) player_y = (float)(SCREEN_H - 10);
                    px = (int)roundf(player_x);
                    py = (int)roundf(player_y);
                    dash_cd = 60;
                    invincibility = 15;
                    dash_invincibility = 15;

                    SpawnGhostTrail(start_dash_x, start_dash_y, dir_x, dir_y, C_CYAN);
                    SpawnGhostTrail(start_dash_x + (float)dash_dx * dash_dist * 0.5f, start_dash_y + (float)dash_dy * dash_dist * 0.5f, dir_x, dir_y, C_CYAN);
                    SpawnGhostTrail(player_x, player_y, dir_x, dir_y, WHITE);
                    for (int dp = 0; dp < 6; dp++) {
                        float pvx = -((float)dash_dx) * (0.3f + (float)(rand() % 20) * 0.03f) + ((float)(rand() % 20) - 10.0f) * 0.03f;
                        float pvy = -((float)dash_dy) * (0.3f + (float)(rand() % 20) * 0.03f) + ((float)(rand() % 20) - 10.0f) * 0.03f;
                        SpawnParticle(start_dash_x + 5.0f, start_dash_y + 5.0f, pvx, pvy, C_CYAN, 10 + (rand() % 6), 1);
                    }

                    for (int b_en = 0; b_en < MAX_ENEMIES; b_en++) {
                        if (enemies[b_en].type == 3 && enemies[b_en].active) {
                            g_boss_dash_used = true;
                            break;
                        }
                    }
                }

                bool is_aim_locked = IsKeyDown(g_keys.key_aim) || (pad_active && IsGamepadButtonDown(pad_id, g_pad.btn_aim)) || mobile.aim_down;
                if (is_aim_locked) {
                    if (mx_dir != 0 || my_dir != 0) { dir_x = mx_dir; dir_y = my_dir; }
                    mx_dir = 0;
                    my_dir = 0;
                } else {
                    if (mx_dir != 0 || my_dir != 0) { dir_x = mx_dir; dir_y = my_dir; }
                    float move_x = (float)mx_dir;
                    float move_y = (float)my_dir;
                    if (move_x != 0.0f && move_y != 0.0f) {
                        move_x *= 0.70710678f;
                        move_y *= 0.70710678f;
                    }
                    player_x += move_x * (float)speed;
                    player_y += move_y * (float)speed;

                    if (move_x != 0.0f || move_y != 0.0f) {
                        bool should_spawn = is_sprinting || (frame_count % 2 == 0);
                        if (should_spawn) {
                            float nozz_x = player_x + 4.5f;
                            float nozz_y = player_y + 4.5f;
                            if (dir_y < 0) nozz_y = player_y + 9.5f;
                            else if (dir_y > 0) nozz_y = player_y - 0.5f;
                            if (dir_x > 0) nozz_x = player_x - 0.5f;
                            else if (dir_x < 0) nozz_x = player_x + 9.5f;

                            float pvx = -move_x * (is_sprinting ? 0.7f : 0.4f) + ((float)(rand() % 20) - 10.0f) * 0.02f;
                            float pvy = -move_y * (is_sprinting ? 0.7f : 0.4f) + ((float)(rand() % 20) - 10.0f) * 0.02f;
                            Color p_col;
                            if (is_sprinting) {
                                p_col = (rand() % 3 == 0) ? WHITE : C_CYAN;
                            } else {
                                p_col = (rand() % 2 == 0) ? GBA_COLOR(31, 24, 0) : GBA_COLOR(31, 12, 0);
                            }
                            SpawnParticle(nozz_x, nozz_y, pvx, pvy, p_col, is_sprinting ? 10 : 7, 1);
                        }
                    }
                }

                if (player_x < 0.0f) player_x = 0.0f;
                if (player_x > (float)(SCREEN_W - 10)) player_x = (float)(SCREEN_W - 10);
                if (player_y < 16.0f) player_y = 16.0f;
                if (player_y > (float)(SCREEN_H - 10)) player_y = (float)(SCREEN_H - 10);
                px = (int)roundf(player_x);
                py = (int)roundf(player_y);

                int p_box_x = px, p_box_y = py, p_box_w = 10, p_box_h = 10;
                if (g_cheat_size_active == 1) {
                    p_box_x = px + 3; p_box_y = py + 3; p_box_w = 4; p_box_h = 4;
                } else if (g_cheat_size_active == 2) {
                    p_box_x = px - 5; p_box_y = py - 5; p_box_w = 20; p_box_h = 20;
                }

                if (shoot_cd > 0) shoot_cd--;
                bool is_shooting = IsKeyDown(g_keys.key_shoot) || (current_frame_key == g_keys.key_shoot) || (pad_active && (IsGamepadButtonDown(pad_id, g_pad.btn_shoot) || IsGamepadButtonPressed(pad_id, g_pad.btn_shoot))) || mobile.fire_down;
                if (pause_resume_timer <= 0.0f && is_shooting && shoot_cd == 0) {
                    shoot_cd = (p_type == 2) ? 6 : ((p_type == 5) ? 20 : 15);
                    PlaySfx(sndShoot);

                    if (p_type == 0) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_NORMAL_MASK;
                    else if (p_type == 2) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_RAPID_MASK;
                    else if (p_type == 3) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_TRIPLE_MASK;
                    else if (p_type == 5) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_HEAVY_MASK;
                    else if (p_type == 9) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_FREEZE_MASK;
                    else if (p_type == 12) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_POISON_MASK;
                    else if (p_type == 14) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_WATER_MASK;

                    if (g_achievement_progress[ACH_FULL_ARSENAL] == ARSENAL_ALL_MASK)
                        UnlockAchievement(ACH_FULL_ARSENAL);

                    if (p_type == 3) {
                        int spawned = 0;
                        for(int b_slot = 0; b_slot < MAX_BULLETS && spawned < 3; b_slot++) {
                            if (!bullets[b_slot].active) {
                                bullets[b_slot].active = 1;
                                bullets[b_slot].x = px + 2; bullets[b_slot].y = py + 2;
                                if (spawned == 0) { bullets[b_slot].vx = dir_x * 6; bullets[b_slot].vy = dir_y * 6; }
                                else if (spawned == 1) { bullets[b_slot].vx = (dir_x == 0) ? -4 : dir_x * 5; bullets[b_slot].vy = (dir_y == 0) ? -4 : dir_y * 5; }
                                else { bullets[b_slot].vx = (dir_x == 0) ? 4 : dir_x * 5; bullets[b_slot].vy = (dir_y == 0) ? 4 : dir_y * 5; }
                                spawned++;
                            }
                        }
                    } else {
                        for(int b_slot = 0; b_slot < MAX_BULLETS; b_slot++) {
                            if (!bullets[b_slot].active) {
                                bullets[b_slot].active = 1;
                                bullets[b_slot].x = px + 2; bullets[b_slot].y = py + 2;
                                bullets[b_slot].vx = dir_x * 6; bullets[b_slot].vy = dir_y * 6;
                                break;
                            }
                        }
                    }
                }

                int cycle_w = ((wave - 1) % 10) + 1;
                bool is_rush_w3 = (current_game_mode == MODE_RUSH && cycle_w == 3);
                int p_interval = is_rush_w3 ? 180 : (current_difficulty == 0 ? 500 : 700);

                if (current_game_mode != MODE_ASSAULT && frame_count % p_interval == 0 && !p_up.active && p_type == 0 && slowmo_timer == 0 && nuke_timer == 0) {
                    p_up.active = 1;
                    if (is_rush_w3) {
                        int r = rand() % 100;
                        if (r < 40) p_up.type = 12;         // 40% Veneno
                        else if (r < 80) p_up.type = 5;     // 40% Disparo Naranja Pesado
                        else if (r < 95) p_up.type = 6;     // 15% Invencibilidad
                        else p_up.type = (rand() % 14) + 1; // 5% otros
                    } else {
                        p_up.type = (rand() % 14) + 1;
                    }
                    p_up.x = (rand() % (SCREEN_W - 60)) + 20;
                    p_up.y = (rand() % (SCREEN_H - 50)) + 30;
                    p_up.life_timer = 1800;
                    Color p_theme = GetPowerUpThemeColor(p_up.type);
                    for (int s = 0; s < 8; s++) {
                        float ang = (float)s * (2.0f * PI / 8.0f);
                        SpawnParticle((float)p_up.x + 5.0f, (float)p_up.y + 5.0f, cosf(ang) * 1.0f, sinf(ang) * 1.0f, WHITE, 14, 1);
                        SpawnParticle((float)p_up.x + 5.0f, (float)p_up.y + 5.0f, cosf(ang) * 0.6f, sinf(ang) * 0.6f, p_theme, 18, 1);
                    }
                }

                if (p_up.active) {
                    p_up.life_timer--;
                    if (p_up.life_timer <= 0) {
                        p_up.active = 0;
                        Color p_theme = GetPowerUpThemeColor(p_up.type);
                        for (int s = 0; s < 6; s++) {
                            float ang = (float)s * (2.0f * PI / 6.0f);
                            SpawnParticle((float)p_up.x + 5.0f, (float)p_up.y + 5.0f, cosf(ang) * 0.5f, sinf(ang) * 0.5f, p_theme, 12, 1);
                            SpawnParticle((float)p_up.x + 5.0f, (float)p_up.y + 5.0f, cosf(ang) * 0.3f, sinf(ang) * 0.3f, GBA_COLOR(14, 14, 14), 16, 1);
                        }
                    } else {
                        if (p_box_x < p_up.x + 8 && p_box_x + p_box_w > p_up.x && p_box_y < p_up.y + 8 && p_box_y + p_box_h > p_up.y) {
                            p_up.active = 0;
                            Color p_theme = GetPowerUpThemeColor(p_up.type);
                            for (int s = 0; s < 8; s++) {
                                float ang = (float)s * (2.0f * PI / 8.0f);
                                SpawnParticle((float)p_up.x + 5.0f, (float)p_up.y + 5.0f, cosf(ang) * 1.3f, sinf(ang) * 1.3f, WHITE, 12, 1);
                                SpawnParticle((float)p_up.x + 5.0f, (float)p_up.y + 5.0f, cosf(ang) * 0.8f, sinf(ang) * 0.8f, p_theme, 16, 1);
                            }
                            g_powerups_picked_this_wave++;
                            g_powerups_picked_this_run++;

                            // EMPODERATE: primer power-up recogido
                            if (!g_achievements_unlocked[ACH_EMPODERATE]) {
                                g_achievement_progress[ACH_EMPODERATE] = 1;
                                UnlockAchievement(ACH_EMPODERATE);
                            }
                            
                            // VOY CIEGO (Bebida verde / controles invertidos, p_up.type == 10)
                            if (p_up.type == 10) { 
                                UnlockAchievement(ACH_VOY_CIEGO);
                            }

                            if (p_up.type == 11) {
                                nuke_timer = 45;
                                PlaySfx(sndNukePickup);
                            } else {
                                PlaySfx(sndPowerUp);
                                if (p_up.type == 4) {
                                    if (!g_cheat_onehit_active) {
                                        if (player_hp < (current_difficulty == 3 ? 3 : 5)) player_hp++;
                                        if (current_game_mode == MODE_RUSH) {
                                            if (player_hp > g_achievement_progress[ACH_RUSH_HASTA_ARRIBA]) {
                                                g_achievement_progress[ACH_RUSH_HASTA_ARRIBA] = player_hp;
                                            }
                                            if (player_hp >= 5) {
                                                UnlockAchievement(ACH_RUSH_HASTA_ARRIBA);
                                            }
                                        }
                                    }
                                } 
                                else if (p_up.type == 7) { slowmo_timer = 600; } 
                                else if (p_up.type == 8) { combo = 5; p_type = 8; p_timer = 600; } 
                                else if (p_up.type == 13) { combo = 10; p_type = 13; p_timer = 600; } 
                                else { p_type = p_up.type; p_timer = 600; }
                            }

                            if (current_game_mode == MODE_RUSH) {
                                g_achievement_progress[ACH_RUSH_PODEROSO]++;
                                if (g_achievement_progress[ACH_RUSH_PODEROSO] >= 60) {
                                    UnlockAchievement(ACH_RUSH_PODEROSO);
                                }
                            }
                        }
                    }
                }

                for(int b_idx = 0; b_idx < MAX_BULLETS; b_idx++) {
                    if(bullets[b_idx].active) {
                        bullets[b_idx].x += bullets[b_idx].vx;
                        bullets[b_idx].y += bullets[b_idx].vy;
                        if(bullets[b_idx].x > SCREEN_W || bullets[b_idx].x < 0 || bullets[b_idx].y < 16 || bullets[b_idx].y > SCREEN_H) {
                            bullets[b_idx].active = 0;
                        } else {
                            for(int en_idx = 0; en_idx < MAX_ENEMIES; en_idx++) {
                                int bw = (p_type == 5) ? 6 : 4;
                                int bh = (p_type == 5) ? 6 : 4;
                                int bx_hit = bullets[b_idx].x;
                                int by_hit = bullets[b_idx].y;
                                if (g_cheat_size_active == 1) {
                                    bw = (p_type == 5) ? 3 : 2;
                                    bh = (p_type == 5) ? 3 : 2;
                                } else if (g_cheat_size_active == 2) {
                                    bw = (p_type == 5) ? 14 : 10;
                                    bh = (p_type == 5) ? 14 : 10;
                                    bx_hit = bullets[b_idx].x - ((p_type == 5) ? 4 : 3);
                                    by_hit = bullets[b_idx].y - ((p_type == 5) ? 4 : 3);
                                }
                                if(enemies[en_idx].active && enemies[en_idx].type != 8 &&
                                   bx_hit < enemies[en_idx].x + 10 && bx_hit + bw > enemies[en_idx].x &&
                                   by_hit < enemies[en_idx].y + 10 && by_hit + bh > enemies[en_idx].y) {
                                    bullets[b_idx].active = 0;
                                    int dmg = (p_type == 5) ? 5 : 1;
                                    
                                    if (p_type == 5 && enemies[en_idx].hp <= dmg) {
                                        g_achievement_progress[ACH_CALIENTE]++;
                                        if (g_achievement_progress[ACH_CALIENTE] >= 25) UnlockAchievement(ACH_CALIENTE);
                                    }

                                    if (p_type == 9) {
                                        if (enemies[en_idx].freeze_timer <= 0) {
                                            SPAWN_FTEXT((float)enemies[en_idx].x, (float)enemies[en_idx].y - 6, T(STR_FREEZE), C_ICE);
                                        }
                                        enemies[en_idx].freeze_timer = 90;
                                        g_achievement_progress[ACH_FRIO_FRIO]++;
                                        if (g_achievement_progress[ACH_FRIO_FRIO] >= 10) UnlockAchievement(ACH_FRIO_FRIO); 
                                        if (current_game_mode == MODE_RUSH) {
                                            g_achievement_progress[ACH_RUSH_HIELO_ATOPE]++;
                                            if (g_achievement_progress[ACH_RUSH_HIELO_ATOPE] >= 500) {
                                                UnlockAchievement(ACH_RUSH_HIELO_ATOPE);
                                            }
                                        } else if (current_game_mode == MODE_CLASSIC) {
                                            g_achievement_progress[ACH_TRIADA_ELEMENTAL]++;
                                            if (g_achievement_progress[ACH_TRIADA_ELEMENTAL] >= 300) {
                                                UnlockAchievement(ACH_TRIADA_ELEMENTAL);
                                            }
                                        }
                                    } else if (p_type == 12) {
                                        if (enemies[en_idx].poison_timer <= 0) {
                                            SPAWN_FTEXT((float)enemies[en_idx].x, (float)enemies[en_idx].y - 6, T(STR_POISON), C_POISON_BULLET);
                                        }
                                        enemies[en_idx].poison_timer = 150;
                                        g_achievement_progress[ACH_TOXICIDAD]++;
                                        if (g_achievement_progress[ACH_TOXICIDAD] >= 10) UnlockAchievement(ACH_TOXICIDAD); 
                                        if (current_game_mode == MODE_RUSH) {
                                            g_achievement_progress[ACH_RUSH_VENENO_ATOPE]++;
                                            if (g_achievement_progress[ACH_RUSH_VENENO_ATOPE] >= 500) {
                                                UnlockAchievement(ACH_RUSH_VENENO_ATOPE);
                                            }
                                        } else if (current_game_mode == MODE_CLASSIC) {
                                            g_achievement_progress[ACH_TRIADA_ELEMENTAL]++;
                                            if (g_achievement_progress[ACH_TRIADA_ELEMENTAL] >= 300) {
                                                UnlockAchievement(ACH_TRIADA_ELEMENTAL);
                                            }
                                        }
                                    } else if (p_type == 14) {
                                        if (enemies[en_idx].wet_timer <= 0) {
                                            SPAWN_FTEXT((float)enemies[en_idx].x, (float)enemies[en_idx].y - 6, T(STR_WET), C_SHIELD);
                                        }
                                        enemies[en_idx].wet_timer = 150;
                                        if (current_game_mode == MODE_RUSH) {
                                            g_achievement_progress[ACH_RUSH_MOJADO_ATOPE]++;
                                            if (g_achievement_progress[ACH_RUSH_MOJADO_ATOPE] >= 500) {
                                                UnlockAchievement(ACH_RUSH_MOJADO_ATOPE);
                                            }
                                        } else if (current_game_mode == MODE_CLASSIC) {
                                            g_achievement_progress[ACH_TRIADA_ELEMENTAL]++;
                                            if (g_achievement_progress[ACH_TRIADA_ELEMENTAL] >= 300) {
                                                UnlockAchievement(ACH_TRIADA_ELEMENTAL);
                                            }
                                        }
                                    }

                                    PlaySfx(sndHit);

                                    ApplyEnemyDamage(en_idx, dmg, (KillContext){ .cause = KILL_CAUSE_BULLET, .weapon_type = p_type, .during_slowmo = (slowmo_timer > 0) });
                                    break;
                                }
                            }
                        }
                    }
                }

                for(int eb_idx = 0; eb_idx < MAX_E_BULLETS; eb_idx++) {
                    if (e_bullets[eb_idx].active) {
                        if (slowmo_timer == 0 || frame_count % 2 == 0) {
                            e_bullets[eb_idx].x += e_bullets[eb_idx].vx;
                            e_bullets[eb_idx].y += e_bullets[eb_idx].vy;
                        }
                        if (e_bullets[eb_idx].x > SCREEN_W || e_bullets[eb_idx].x < 0 || e_bullets[eb_idx].y < 16 || e_bullets[eb_idx].y > SCREEN_H) {
                            e_bullets[eb_idx].active = 0;
                            g_consecutive_bullets_dodged++;
                            if (g_consecutive_bullets_dodged >= 25) {
                                UnlockAchievement(ACH_BAILANDO_ENTRE_BALAS);
                            }
                        } else {
                            if (p_type == 6) {
                                e_bullets[eb_idx].active = 0;
                            } else if (dash_invincibility > 0 && p_box_x < e_bullets[eb_idx].x + 4 && p_box_x + p_box_w > e_bullets[eb_idx].x && p_box_y < e_bullets[eb_idx].y + 4 && p_box_y + p_box_h > e_bullets[eb_idx].y) {
                                e_bullets[eb_idx].active = 0;
                                g_achievement_progress[ACH_ESQUIVE_PERFECTO]++;
                                if (g_achievement_progress[ACH_ESQUIVE_PERFECTO] >= 10)
                                    UnlockAchievement(ACH_ESQUIVE_PERFECTO);
                                g_consecutive_bullets_dodged++;
                                if (g_consecutive_bullets_dodged >= 25) {
                                    UnlockAchievement(ACH_BAILANDO_ENTRE_BALAS);
                                }
                            } else if (invincibility == 0 && p_box_x < e_bullets[eb_idx].x + 4 && p_box_x + p_box_w > e_bullets[eb_idx].x && p_box_y < e_bullets[eb_idx].y + 4 && p_box_y + p_box_h > e_bullets[eb_idx].y) {
                                e_bullets[eb_idx].active = 0;
                                player_took_hit_this_wave = true;
                                g_waves_without_damage_streak = 0;
                                g_consecutive_bullets_dodged = 0;
                                hell_no_damage_frames = 0;
                                if (p_type == 1) {
                                    PlaySfx(sndHurt);
                                    p_type = 0; p_timer = 0;
                                    invincibility = 90;
                                    for (int sp = 0; sp < 6; sp++) {
                                        float pvx = ((float)(rand() % 40) - 20.0f) * 0.05f;
                                        float pvy = ((float)(rand() % 40) - 20.0f) * 0.05f;
                                        SpawnParticle(player_x + 5.0f, player_y + 5.0f, pvx, pvy, C_CYAN, 10, 1);
                                    }
                                } else {
                                    player_hp--;
                                    screen_shake_timer = 15;
                                    g_damage_flash_timer = 6;
                                    if (p_type != 13) { combo = 1; combo_timer = 0; if (p_type == 8) { p_type = 0; p_timer = 0; } }
                                    if (current_game_mode == MODE_TIME_ATTACK) {
                                        ta_time_left_frames -= 300;
                                        SpawnFloatingText(player_x, player_y - 12, "-5 SEG", C_RED);
                                        if (ta_time_left_frames < 0) ta_time_left_frames = 0;
                                    }
                                    if (player_hp <= 0 || (current_game_mode == MODE_TIME_ATTACK && ta_time_left_frames <= 0)) { PlaySfx(sndDeath); state = 8; death_timer = 60; } 
                                    else { PlaySfx(sndHurt); invincibility = 90; }
                                }
                            }
                        }
                    }
                }

                if (current_game_mode == MODE_CLASSIC) {
                    int old_wave = wave;
                    wave = 1 + (score / (current_difficulty == 0 ? 4000 : (current_difficulty == 1 ? 3000 : (current_difficulty == 2 ? 2000 : 1200))));
                    if (wave > old_wave) {
                        if (!player_took_hit_this_wave) {
                            UnlockAchievement(ACH_INTACTO);
                            g_waves_without_damage_streak++;
                            if (g_waves_without_damage_streak >= 3) {
                                UnlockAchievement(ACH_SIN_RASGUNOS);
                            }
                        } else {
                            g_waves_without_damage_streak = 0;
                        }

                        if (current_difficulty == 2 && g_powerups_picked_this_wave == 0) {
                            UnlockAchievement(ACH_NO_ERA_TAN_DIFICIL);
                        }

                        player_took_hit_this_wave = false;
                        g_powerups_picked_this_wave = 0;
                    }
                } else if (current_game_mode == MODE_RUSH) {
                    if (rush_wave_banner_timer > 0) {
                        UpdateWaveJinglePC(rush_wave_banner_timer);
                        rush_wave_banner_timer--;
                    }
                } else if (current_game_mode == MODE_TIME_ATTACK) {
                    ta_time_survived_frames++;
                    ta_time_left_frames--;

                    int s_sec = ta_time_survived_frames / 60;
                    if (s_sec > g_achievement_progress[ACH_TA_1MIN]) g_achievement_progress[ACH_TA_1MIN] = (s_sec > 60) ? 60 : s_sec;
                    if (s_sec > g_achievement_progress[ACH_TA_3MIN]) g_achievement_progress[ACH_TA_3MIN] = (s_sec > 180) ? 180 : s_sec;
                    if (s_sec > g_achievement_progress[ACH_TA_5MIN]) g_achievement_progress[ACH_TA_5MIN] = (s_sec > 300) ? 300 : s_sec;

                    if (ta_time_survived_frames >= 60 * 60) UnlockAchievement(ACH_TA_1MIN);
                    if (ta_time_survived_frames >= 180 * 60) UnlockAchievement(ACH_TA_3MIN);
                    if (ta_time_survived_frames >= 300 * 60) UnlockAchievement(ACH_TA_5MIN);

                    if (ta_time_left_frames <= 0) {
                        ta_time_left_frames = 0;
                        ta_time_up = true;
                        PlaySfx(sndDeath);
                        state = 8;
                        death_timer = 60;
                    }
                }

                if (current_difficulty == 3 && current_game_mode == MODE_CLASSIC) {
                    g_achievement_progress[ACH_INFIERNO_PURO] = wave;
                    if (wave >= 15) {
                        UnlockAchievement(ACH_INFIERNO_PURO);
                    }
                    hell_no_damage_frames++;
                    g_achievement_progress[ACH_NO_TOCARME] = hell_no_damage_frames;
                    if (hell_no_damage_frames >= 1800) UnlockAchievement(ACH_NO_TOCARME);
                } else {
                    hell_no_damage_frames = 0;
                    g_achievement_progress[ACH_NO_TOCARME] = 0;
                }

                if (current_game_mode == MODE_RUSH) {
                    if (rush_wave_banner_timer == 0) {
                        int cycle_wave = ((wave - 1) % 10) + 1;
                        rush_cycle = (wave - 1) / 10;
                        int rush_spawn_rate;
                        if (cycle_wave == 10) {
                            rush_spawn_rate = 110 - (rush_cycle * 8);
                            if (rush_spawn_rate < 50) rush_spawn_rate = 50;
                        } else {
                            rush_spawn_rate = 95 - (cycle_wave * 4) - (rush_cycle * 8);
                            if (rush_spawn_rate < 24) rush_spawn_rate = 24;
                        }

                        if (frame_count % rush_spawn_rate == 0) {
                            if (cycle_wave == 10) {
                                int active_bosses = 0;
                                for (int b_en = 0; b_en < MAX_ENEMIES; b_en++) {
                                    if (enemies[b_en].active && enemies[b_en].type == 3) active_bosses++;
                                }
                                if (active_bosses < 3 && (rush_kills_in_wave + active_bosses) < 10) {
                                    for (int e_slot = 0; e_slot < MAX_ENEMIES; e_slot++) {
                                        if (!enemies[e_slot].active) {
                                            enemies[e_slot].active = 1;
                                            enemies[e_slot].type = 3;
                                            enemies[e_slot].hp = 18 + (rush_cycle * 5);
                                            enemies[e_slot].freeze_timer = 0;
                                            enemies[e_slot].poison_timer = 0;
                                            enemies[e_slot].wet_timer = 0;
                                            enemies[e_slot].timer = 0;
                                            enemies[e_slot].hit_flash_timer = 0;
                                            int b_attempts = 0;
                                            do {
                                                int side = rand() % 4;
                                                if (side == 0) {
                                                    enemies[e_slot].x = (rand() % (SCREEN_W - 40)) + 20;
                                                    enemies[e_slot].y = 16;
                                                } else if (side == 1) {
                                                    enemies[e_slot].x = (rand() % (SCREEN_W - 40)) + 20;
                                                    enemies[e_slot].y = SCREEN_H - 24;
                                                } else if (side == 2) {
                                                    enemies[e_slot].x = 4;
                                                    enemies[e_slot].y = (rand() % (SCREEN_H - 50)) + 25;
                                                } else {
                                                    enemies[e_slot].x = SCREEN_W - 24;
                                                    enemies[e_slot].y = (rand() % (SCREEN_H - 50)) + 25;
                                                }
                                                b_attempts++;
                                            } while (b_attempts < 10 && ((enemies[e_slot].x - px)*(enemies[e_slot].x - px) + (enemies[e_slot].y - py)*(enemies[e_slot].y - py) < 3000));
                                            enemies[e_slot].target_x = enemies[e_slot].x;
                                            enemies[e_slot].target_y = enemies[e_slot].y;
                                            enemy_fx[e_slot] = (float)enemies[e_slot].x;
                                            enemy_fy[e_slot] = (float)enemies[e_slot].y;
                                            prev_enemy_fx[e_slot] = enemy_fx[e_slot];
                                            prev_enemy_fy[e_slot] = enemy_fy[e_slot];
                                            break;
                                        }
                                    }
                                }
                            } else {
                                int patroller_exists = 0;
                                for(int p_en = 0; p_en < MAX_ENEMIES; p_en++) {
                                    if (enemies[p_en].active && enemies[p_en].type == 8) patroller_exists = 1;
                                }

                                for (int e_slot = 0; e_slot < MAX_ENEMIES; e_slot++) {
                                    if (!enemies[e_slot].active) {
                                        enemies[e_slot].active = 1;
                                        enemies[e_slot].freeze_timer = 0;
                                        enemies[e_slot].poison_timer = 0;
                                        enemies[e_slot].wet_timer = 0;
                                        enemies[e_slot].timer = 0;
                                        enemies[e_slot].hit_flash_timer = 0;

                                        int enemy_type = 0;
                                        if (cycle_wave == 1) enemy_type = 0;
                                        else if (cycle_wave == 2) enemy_type = 1;
                                        else if (cycle_wave == 3) enemy_type = 2;
                                        else if (cycle_wave == 4) enemy_type = 4;
                                        else if (cycle_wave == 5) enemy_type = 5;
                                        else if (cycle_wave == 6) enemy_type = 7;
                                        else if (cycle_wave == 7) enemy_type = 9;
                                        else if (cycle_wave == 8) {
                                            if (!patroller_exists && (rand() % 100 < 35)) enemy_type = 8;
                                            else enemy_type = 2;
                                        } else if (cycle_wave == 9) {
                                            int pool[4] = { 7, 4, 2, 9 };
                                            enemy_type = pool[rand() % 4];
                                        }

                                        enemies[e_slot].type = enemy_type;
                                        if (enemies[e_slot].type == 2) enemies[e_slot].hp = 5 + (rush_cycle * 2);
                                        else if (enemies[e_slot].type == 4 || enemies[e_slot].type == 5) enemies[e_slot].hp = 2 + rush_cycle;
                                        else if (enemies[e_slot].type == 7) enemies[e_slot].hp = 2;
                                        else if (enemies[e_slot].type == 8) enemies[e_slot].hp = 999;
                                        else if (enemies[e_slot].type == 9) enemies[e_slot].hp = 3 + rush_cycle;
                                        else enemies[e_slot].hp = 2 + rush_cycle;

                                        enemies[e_slot].target_x = (rand() % (SCREEN_W - 60)) + 30;
                                        enemies[e_slot].target_y = (rand() % (SCREEN_H - 55)) + 25;

                                        if (enemies[e_slot].type == 8) {
                                            int p_attempts = 0;
                                            do {
                                                int edge = rand() % 4;
                                                if (edge == 0) {
                                                    enemies[e_slot].x = 0; enemies[e_slot].y = (rand() % (SCREEN_H - 40)) + 20;
                                                    enemies[e_slot].target_x = SCREEN_W; enemies[e_slot].target_y = enemies[e_slot].y;
                                                } else if (edge == 1) {
                                                    enemies[e_slot].x = SCREEN_W - 10; enemies[e_slot].y = (rand() % (SCREEN_H - 40)) + 20;
                                                    enemies[e_slot].target_x = 0; enemies[e_slot].target_y = enemies[e_slot].y;
                                                } else if (edge == 2) {
                                                    enemies[e_slot].x = (rand() % (SCREEN_W - 40)) + 20; enemies[e_slot].y = 16;
                                                    enemies[e_slot].target_x = enemies[e_slot].x; enemies[e_slot].target_y = SCREEN_H - 10;
                                                } else {
                                                    enemies[e_slot].x = (rand() % (SCREEN_W - 40)) + 20; enemies[e_slot].y = SCREEN_H - 10;
                                                    enemies[e_slot].target_x = enemies[e_slot].x; enemies[e_slot].target_y = 16;
                                                }
                                                p_attempts++;
                                            } while (p_attempts < 10 && ((enemies[e_slot].x - px)*(enemies[e_slot].x - px) + (enemies[e_slot].y - py)*(enemies[e_slot].y - py) < 2500));
                                        } else {
                                            do {
                                                int side = rand() % 4;
                                                if (side == 0) { enemies[e_slot].x = rand() % (SCREEN_W - 10); enemies[e_slot].y = 16; }
                                                else if (side == 1) { enemies[e_slot].x = SCREEN_W - 10; enemies[e_slot].y = (rand() % (SCREEN_H - 26)) + 16; }
                                                else if (side == 2) { enemies[e_slot].x = rand() % (SCREEN_W - 10); enemies[e_slot].y = SCREEN_H - 10; }
                                                else { enemies[e_slot].x = 0; enemies[e_slot].y = (rand() % (SCREEN_H - 26)) + 16; }
                                            } while((enemies[e_slot].x - px)*(enemies[e_slot].x - px) + (enemies[e_slot].y - py)*(enemies[e_slot].y - py) < 3000);
                                        }

                                        enemy_fx[e_slot] = (float)enemies[e_slot].x;
                                        enemy_fy[e_slot] = (float)enemies[e_slot].y;
                                        prev_enemy_fx[e_slot] = enemy_fx[e_slot];
                                        prev_enemy_fy[e_slot] = enemy_fy[e_slot];
                                        break;
                                    }
                                }
                            }
                        }
                    }
                } else if (current_game_mode == MODE_TIME_ATTACK || current_game_mode == MODE_ASSAULT) {
                    int mode_sec = (current_game_mode == MODE_ASSAULT) ? (assault_time_survived_frames / 60) : (ta_time_survived_frames / 60);
                    int mode_kills = (current_game_mode == MODE_ASSAULT) ? assault_run_kills : ta_run_kills;
                    int spawn_rate = 65 - (mode_sec / 15) * 4;
                    if (spawn_rate < 20) spawn_rate = 20;

                    if (frame_count % spawn_rate == 0) {
                        int patroller_exists = 0;
                        int boss_exists = 0;
                        int teleporter_count = 0;
                        for(int p_en = 0; p_en < MAX_ENEMIES; p_en++) {
                            if (enemies[p_en].active && enemies[p_en].type == 8) patroller_exists = 1;
                            if (enemies[p_en].active && enemies[p_en].type == 3) boss_exists = 1;
                            if (enemies[p_en].active && enemies[p_en].type == 9) teleporter_count++;
                        }

                        bool spawn_mode_boss = (!boss_exists && ((mode_kills > 0 && mode_kills % 25 == 0) || (mode_sec > 0 && mode_sec % 50 == 0 && rand() % 100 < 35)));

                        for(int e_slot = 0; e_slot < MAX_ENEMIES; e_slot++) {
                            if(!enemies[e_slot].active) {
                                enemies[e_slot].active = 1;
                                enemies[e_slot].freeze_timer = 0;
                                enemies[e_slot].poison_timer = 0;
                                enemies[e_slot].wet_timer = 0;
                                enemies[e_slot].hit_flash_timer = 0;
                                enemies[e_slot].timer = 0;

                                if (spawn_mode_boss) {
                                    enemies[e_slot].type = 3;
                                    enemies[e_slot].hp = 18 + (mode_sec / 30) * 4;
                                    enemies[e_slot].x = (SCREEN_W - 20) / 2;
                                    enemies[e_slot].y = 16;
                                    g_boss_dash_used = false;
                                    spawn_mode_boss = false;
                                } else {
                                    int r;
                                    do {
                                        if (teleporter_count < 1 && (rand() % 100 < 5)) r = 9;
                                        else if (!patroller_exists && rand() % 100 < 10) r = 8;
                                        else r = rand() % 7;
                                    } while(r == 6);

                                    if (r == 3) enemies[e_slot].type = 4;
                                    else if (r == 4) enemies[e_slot].type = 5;
                                    else if (r == 5) enemies[e_slot].type = 7;
                                    else enemies[e_slot].type = r;

                                    if (enemies[e_slot].type == 2) enemies[e_slot].hp = 5;
                                    else if (enemies[e_slot].type == 4 || enemies[e_slot].type == 5) enemies[e_slot].hp = 2;
                                    else if (enemies[e_slot].type == 7) enemies[e_slot].hp = 2;
                                    else if (enemies[e_slot].type == 8) enemies[e_slot].hp = 999;
                                    else if (enemies[e_slot].type == 9) enemies[e_slot].hp = 3;
                                    else enemies[e_slot].hp = 2;

                                    enemies[e_slot].target_x = (rand() % (SCREEN_W - 60)) + 30;
                                    enemies[e_slot].target_y = (rand() % (SCREEN_H - 55)) + 25;

                                    if (enemies[e_slot].type == 8) {
                                        int p_attempts = 0;
                                        do {
                                            int edge = rand() % 4;
                                            if (edge == 0) {
                                                enemies[e_slot].x = 0; enemies[e_slot].y = (rand() % (SCREEN_H - 40)) + 20;
                                                enemies[e_slot].target_x = SCREEN_W; enemies[e_slot].target_y = enemies[e_slot].y;
                                            } else if (edge == 1) {
                                                enemies[e_slot].x = SCREEN_W - 10; enemies[e_slot].y = (rand() % (SCREEN_H - 40)) + 20;
                                                enemies[e_slot].target_x = 0; enemies[e_slot].target_y = enemies[e_slot].y;
                                            } else if (edge == 2) {
                                                enemies[e_slot].x = (rand() % (SCREEN_W - 40)) + 20; enemies[e_slot].y = 16;
                                                enemies[e_slot].target_x = enemies[e_slot].x; enemies[e_slot].target_y = SCREEN_H - 10;
                                            } else {
                                                enemies[e_slot].x = (rand() % (SCREEN_W - 40)) + 20; enemies[e_slot].y = SCREEN_H - 10;
                                                enemies[e_slot].target_x = enemies[e_slot].x; enemies[e_slot].target_y = 16;
                                            }
                                            p_attempts++;
                                        } while (p_attempts < 10 && ((enemies[e_slot].x - px)*(enemies[e_slot].x - px) + (enemies[e_slot].y - py)*(enemies[e_slot].y - py) < 2500));
                                    } else {
                                        do {
                                            int side = rand() % 4;
                                            if (side == 0) { enemies[e_slot].x = rand() % (SCREEN_W - 10); enemies[e_slot].y = 16; }
                                            else if (side == 1) { enemies[e_slot].x = SCREEN_W - 10; enemies[e_slot].y = (rand() % (SCREEN_H - 26)) + 16; }
                                            else if (side == 2) { enemies[e_slot].x = rand() % (SCREEN_W - 10); enemies[e_slot].y = SCREEN_H - 10; }
                                            else { enemies[e_slot].x = 0; enemies[e_slot].y = (rand() % (SCREEN_H - 26)) + 16; }
                                        } while((enemies[e_slot].x - px)*(enemies[e_slot].x - px) + (enemies[e_slot].y - py)*(enemies[e_slot].y - py) < 3000);
                                    }
                                }

                                enemy_fx[e_slot] = (float)enemies[e_slot].x;
                                enemy_fy[e_slot] = (float)enemies[e_slot].y;
                                prev_enemy_fx[e_slot] = enemy_fx[e_slot];
                                prev_enemy_fy[e_slot] = enemy_fy[e_slot];
                                break;
                            }
                        }
                    }
                } else {
                    int base_spawn = (current_difficulty == 0) ? 140 : (current_difficulty == 1 ? 110 : (current_difficulty == 2 ? 80 : 50));
                    int spawn_rate = base_spawn - (wave * (current_difficulty == 3 ? 8 : 4));
                    if (spawn_rate < (current_difficulty == 3 ? 20 : 35)) spawn_rate = (current_difficulty == 3 ? 20 : 35);

                    if (frame_count % spawn_rate == 0) {
                        int patroller_exists = 0;
                        for(int p_en = 0; p_en < MAX_ENEMIES; p_en++) {
                            if (enemies[p_en].active && enemies[p_en].type == 8) patroller_exists = 1;
                        }

                        for(int e_slot = 0; e_slot < MAX_ENEMIES; e_slot++) {
                            if(!enemies[e_slot].active) {
                                enemies[e_slot].active = 1;
                                enemies[e_slot].freeze_timer = 0;
                                enemies[e_slot].poison_timer = 0;
                                enemies[e_slot].wet_timer = 0;

                                int is_boss = (wave % 3 == 0) && (rand() % 100 > (current_difficulty == 3 ? 60 : 80));
                                int boss_exists = 0;
                                for(int b_en = 0; b_en < MAX_ENEMIES; b_en++) if (enemies[b_en].type == 3 && enemies[b_en].active) boss_exists = 1;

                                if (is_boss && !boss_exists) {
                                    enemies[e_slot].type = 3; enemies[e_slot].hp = (current_difficulty == 3 ? 25 : 15) + wave;
                                    enemies[e_slot].x = (SCREEN_W - 20) / 2; enemies[e_slot].y = 16;
                                    g_boss_dash_used = false;
                                } else {
                                    int r;
                                    do {
                                        if (current_difficulty >= 2 && rand() % 100 < 15) r = 9;
                                        else if (current_difficulty >= 2 && !patroller_exists && rand() % 100 < 10) r = 8;
                                        else r = rand() % 7;
                                    } while(r == 6);

                                    if (r == 3) enemies[e_slot].type = 4;
                                    else if (r == 4) enemies[e_slot].type = 5;
                                    else if (r == 5) enemies[e_slot].type = 7;
                                    else enemies[e_slot].type = r;

                                    if (enemies[e_slot].type == 2) enemies[e_slot].hp = (current_difficulty == 3 ? 8 : 5);
                                    else if (enemies[e_slot].type == 4 || enemies[e_slot].type == 5) enemies[e_slot].hp = (current_difficulty == 3 ? 3 : 2);
                                    else if (enemies[e_slot].type == 7) enemies[e_slot].hp = 2;
                                    else if (enemies[e_slot].type == 8) enemies[e_slot].hp = 999;
                                    else if (enemies[e_slot].type == 9) enemies[e_slot].hp = (current_difficulty == 3 ? 4 : 3);
                                    else enemies[e_slot].hp = (current_difficulty == 3 ? 4 : 2);

                                    enemies[e_slot].timer = 0;
                                    enemies[e_slot].target_x = (rand() % (SCREEN_W - 60)) + 30;
                                    enemies[e_slot].target_y = (rand() % (SCREEN_H - 55)) + 25;

                                    if (enemies[e_slot].type == 8) {
                                        int p_attempts = 0;
                                        do {
                                            int edge = rand() % 4;
                                            if (edge == 0) {
                                                enemies[e_slot].x = 0; enemies[e_slot].y = (rand() % (SCREEN_H - 40)) + 20;
                                                enemies[e_slot].target_x = SCREEN_W; enemies[e_slot].target_y = enemies[e_slot].y;
                                            } else if (edge == 1) {
                                                enemies[e_slot].x = SCREEN_W - 10; enemies[e_slot].y = (rand() % (SCREEN_H - 40)) + 20;
                                                enemies[e_slot].target_x = 0; enemies[e_slot].target_y = enemies[e_slot].y;
                                            } else if (edge == 2) {
                                                enemies[e_slot].x = (rand() % (SCREEN_W - 40)) + 20; enemies[e_slot].y = 16;
                                                enemies[e_slot].target_x = enemies[e_slot].x; enemies[e_slot].target_y = SCREEN_H - 10;
                                            } else {
                                                enemies[e_slot].x = (rand() % (SCREEN_W - 40)) + 20; enemies[e_slot].y = SCREEN_H - 10;
                                                enemies[e_slot].target_x = enemies[e_slot].x; enemies[e_slot].target_y = 16;
                                            }
                                            p_attempts++;
                                        } while (p_attempts < 10 && ((enemies[e_slot].x - px)*(enemies[e_slot].x - px) + (enemies[e_slot].y - py)*(enemies[e_slot].y - py) < 2500));
                                    } else {
                                        do {
                                            int side = rand() % 4;
                                            if (side == 0) { enemies[e_slot].x = rand() % (SCREEN_W - 10); enemies[e_slot].y = 16; }
                                            else if (side == 1) { enemies[e_slot].x = SCREEN_W - 10; enemies[e_slot].y = (rand() % (SCREEN_H - 26)) + 16; }
                                            else if (side == 2) { enemies[e_slot].x = rand() % (SCREEN_W - 10); enemies[e_slot].y = SCREEN_H - 10; }
                                            else { enemies[e_slot].x = 0; enemies[e_slot].y = (rand() % (SCREEN_H - 26)) + 16; }
                                        } while((enemies[e_slot].x - px)*(enemies[e_slot].x - px) + (enemies[e_slot].y - py)*(enemies[e_slot].y - py) < 3000);
                                    }
                                }
                                enemy_fx[e_slot] = (float)enemies[e_slot].x;
                                enemy_fy[e_slot] = (float)enemies[e_slot].y;
                                prev_enemy_fx[e_slot] = enemy_fx[e_slot];
                                prev_enemy_fy[e_slot] = enemy_fy[e_slot];
                                break;
                            }
                        }
                    }
                }

                for(int en_idx = 0; en_idx < MAX_ENEMIES; en_idx++) {
                    if(enemies[en_idx].active) {
                        if (enemies[en_idx].poison_timer > 0) {
                            enemies[en_idx].poison_timer--;
                            if (frame_count % 30 == 0) {
                                PlaySfx(sndHit);
                                ApplyEnemyDamage(en_idx, 1, (KillContext){ .cause = KILL_CAUSE_POISON, .weapon_type = -1, .during_slowmo = (slowmo_timer > 0) });
                            }
                        }
                        if (enemies[en_idx].wet_timer > 0) {
                            enemies[en_idx].wet_timer--;
                        }

                        if (!enemies[en_idx].active) continue;

                        if (enemies[en_idx].freeze_timer > 0) {
                            enemies[en_idx].freeze_timer--;
                        } else {
                            float step_spd = 0.5f;
                            int is_fast = (enemies[en_idx].type == 1 || enemies[en_idx].type == 6);
                            if (is_fast) step_spd = 1.0f;
                            if (slowmo_timer > 0 || enemies[en_idx].wet_timer > 0) step_spd *= 0.5f;

                            if (enemies[en_idx].type == 8) {
                                if (fabsf(enemy_fx[en_idx] - (float)enemies[en_idx].target_x) <= step_spd) enemy_fx[en_idx] = (float)enemies[en_idx].target_x;
                                else if (enemy_fx[en_idx] < (float)enemies[en_idx].target_x) enemy_fx[en_idx] += step_spd;
                                else enemy_fx[en_idx] -= step_spd;

                                if (fabsf(enemy_fy[en_idx] - (float)enemies[en_idx].target_y) <= step_spd) enemy_fy[en_idx] = (float)enemies[en_idx].target_y;
                                else if (enemy_fy[en_idx] < (float)enemies[en_idx].target_y) enemy_fy[en_idx] += step_spd;
                                else enemy_fy[en_idx] -= step_spd;

                                if (fabsf(enemy_fx[en_idx] - (float)enemies[en_idx].target_x) <= 1.0f && fabsf(enemy_fy[en_idx] - (float)enemies[en_idx].target_y) <= 1.0f) {
                                    enemies[en_idx].active = 0;
                                    continue;
                                }
                            } else if (enemies[en_idx].type == 9) {
                                enemies[en_idx].timer++;
                                if (fabsf(enemy_fx[en_idx] - player_x) <= step_spd) enemy_fx[en_idx] = player_x;
                                else if (enemy_fx[en_idx] < player_x) enemy_fx[en_idx] += step_spd;
                                else enemy_fx[en_idx] -= step_spd;

                                if (fabsf(enemy_fy[en_idx] - player_y) <= step_spd) enemy_fy[en_idx] = player_y;
                                else if (enemy_fy[en_idx] < player_y) enemy_fy[en_idx] += step_spd;
                                else enemy_fy[en_idx] -= step_spd;

                                if (enemies[en_idx].timer >= 120) {
                                    enemies[en_idx].timer = 0;
                                    int new_x = px;
                                    int new_y = py;
                                    int tp_attempts = 0;
                                    do {
                                        float angle = ((float)(rand() % 360)) * (3.14159265f / 180.0f);
                                        float dist = 40.0f + (float)(rand() % 35); // 40 a 75 px (cerca pero sin aparecer en la cara)
                                        new_x = (int)roundf((float)px + cosf(angle) * dist);
                                        new_y = (int)roundf((float)py + sinf(angle) * dist);
                                        if (new_x < 6) new_x = 6;
                                        if (new_x > SCREEN_W - 14) new_x = SCREEN_W - 14;
                                        if (new_y < 18) new_y = 18;
                                        if (new_y > SCREEN_H - 14) new_y = SCREEN_H - 14;
                                        tp_attempts++;
                                    } while (tp_attempts < 15 && ((new_x - px)*(new_x - px) + (new_y - py)*(new_y - py) < (35 * 35)));

                                    enemies[en_idx].x = new_x;
                                    enemies[en_idx].y = new_y;
                                    enemy_fx[en_idx] = (float)enemies[en_idx].x;
                                    enemy_fy[en_idx] = (float)enemies[en_idx].y;
                                    prev_enemy_fx[en_idx] = enemy_fx[en_idx];
                                    prev_enemy_fy[en_idx] = enemy_fy[en_idx];
                                }
                            } else if (enemies[en_idx].type == 3) {
                                if (current_game_mode == MODE_RUSH) {
                                    float b_spd = step_spd * 0.7f;
                                    if (fabsf(enemy_fx[en_idx] - player_x) > 4.0f) {
                                        enemy_fx[en_idx] += (enemy_fx[en_idx] < player_x) ? b_spd : -b_spd;
                                    }
                                    if (fabsf(enemy_fy[en_idx] - player_y) > 4.0f) {
                                        enemy_fy[en_idx] += (enemy_fy[en_idx] < player_y) ? b_spd : -b_spd;
                                    }
                                    enemy_fx[en_idx] += (frame_count / 30 % 2 == 0) ? (step_spd * 0.4f) : -(step_spd * 0.4f);
                                } else {
                                    enemy_fx[en_idx] += (frame_count / 30 % 2 == 0) ? step_spd : -step_spd;
                                    if (enemy_fy[en_idx] < player_y) enemy_fy[en_idx] += step_spd;
                                }
                            } else if (enemies[en_idx].type == 4) {
                                if (fabsf(enemy_fx[en_idx] - (float)enemies[en_idx].target_x) <= step_spd) enemy_fx[en_idx] = (float)enemies[en_idx].target_x;
                                else if (enemy_fx[en_idx] < (float)enemies[en_idx].target_x) enemy_fx[en_idx] += step_spd;
                                else enemy_fx[en_idx] -= step_spd;

                                if (fabsf(enemy_fy[en_idx] - (float)enemies[en_idx].target_y) <= step_spd) enemy_fy[en_idx] = (float)enemies[en_idx].target_y;
                                else if (enemy_fy[en_idx] < (float)enemies[en_idx].target_y) enemy_fy[en_idx] += step_spd;
                                else enemy_fy[en_idx] -= step_spd;
                            } else {
                                if (fabsf(enemy_fx[en_idx] - player_x) <= step_spd) enemy_fx[en_idx] = player_x;
                                else if (enemy_fx[en_idx] < player_x) enemy_fx[en_idx] += step_spd;
                                else enemy_fx[en_idx] -= step_spd;

                                if (fabsf(enemy_fy[en_idx] - player_y) <= step_spd) enemy_fy[en_idx] = player_y;
                                else if (enemy_fy[en_idx] < player_y) enemy_fy[en_idx] += step_spd;
                                else enemy_fy[en_idx] -= step_spd;
                            }

                            enemies[en_idx].x = (int)roundf(enemy_fx[en_idx]);
                            enemies[en_idx].y = (int)roundf(enemy_fy[en_idx]);

                            if (enemies[en_idx].type != 9) enemies[en_idx].timer++;

                            int fire_rate;
                            if (enemies[en_idx].type == 8) fire_rate = (current_difficulty == 3) ? 35 : 55;
                            else if (enemies[en_idx].type == 4) fire_rate = (current_difficulty == 3) ? 70 : 110;
                            else if (enemies[en_idx].type == 3) fire_rate = 60;
                            else if (enemies[en_idx].type == 9) fire_rate = (current_difficulty == 3 ? 70 : 100);
                            else if (enemies[en_idx].type == 6 || enemies[en_idx].type == 7) fire_rate = 9999;
                            else fire_rate = (current_difficulty == 3) ? 40 : 180;

                            if (slowmo_timer > 0) fire_rate *= 2;

                            if (enemies[en_idx].type == 9 || enemies[en_idx].timer > fire_rate) {
                                if (enemies[en_idx].type != 9) enemies[en_idx].timer = 0;

                                if (enemies[en_idx].type != 9 || (frame_count % (current_difficulty == 3 ? 60 : 90) == 0)) {
                                    for(int eb_slot = 0; eb_slot < MAX_E_BULLETS; eb_slot++) {
                                        if (!e_bullets[eb_slot].active) {
                                            e_bullets[eb_slot].active = 1;
                                            e_bullets[eb_slot].x = enemies[en_idx].x + 3;
                                            e_bullets[eb_slot].y = enemies[en_idx].y + 3;

                                            if (enemies[en_idx].type == 4 || enemies[en_idx].type == 8 || enemies[en_idx].type == 9) {
                                                int dx = (px + 3) - e_bullets[eb_slot].x;
                                                int dy = (py + 3) - e_bullets[eb_slot].y;
                                                int dist = abs(dx) + abs(dy);
                                                if (dist == 0) dist = 1;
                                                int b_spd = (current_difficulty == 3) ? 5 : 4;
                                                e_bullets[eb_slot].vx = (dx * b_spd) / dist;
                                                e_bullets[eb_slot].vy = (dy * b_spd) / dist;
                                                if (e_bullets[eb_slot].vx == 0 && dx != 0) e_bullets[eb_slot].vx = (dx > 0) ? 1 : -1;
                                                if (e_bullets[eb_slot].vy == 0 && dy != 0) e_bullets[eb_slot].vy = (dy > 0) ? 1 : -1;
                                            } else {
                                                e_bullets[eb_slot].vx = (px > enemies[en_idx].x) ? (current_difficulty == 3 ? 4 : 3) : (current_difficulty == 3 ? -4 : -3);
                                                e_bullets[eb_slot].vy = (py > enemies[en_idx].y) ? (current_difficulty == 3 ? 4 : 3) : (current_difficulty == 3 ? -4 : -3);
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        if ((invincibility == 0 || p_type == 6) && p_box_x < enemies[en_idx].x + 10 && p_box_x + p_box_w > enemies[en_idx].x && p_box_y < enemies[en_idx].y + 10 && p_box_y + p_box_h > enemies[en_idx].y) {
                            player_took_hit_this_wave = true;
                            hell_no_damage_frames = 0;
                            if (p_type == 6) {
                                KillEnemy(en_idx, (KillContext){ .cause = KILL_CAUSE_PLAYER_POWER, .weapon_type = -1, .during_slowmo = (slowmo_timer > 0) });
                            } else {
                                if (enemies[en_idx].type == 7) {
                                    KillEnemy(en_idx, (KillContext){ .cause = KILL_CAUSE_KAMIKAZE_CRASH, .weapon_type = -1, .during_slowmo = (slowmo_timer > 0) });
                                    if (p_type == 1) {
                                        PlaySfx(sndHurt);
                                        p_type = 0; p_timer = 0;
                                        invincibility = 90;
                                    } else {
                                        player_hp -= 2;
                                        screen_shake_timer = 18;
                                        if (p_type != 13) {
                                            combo = 1;
                                            combo_timer = 0;
                                            if(p_type == 8) { p_type = 0; p_timer = 0; }
                                        }
                                        if (player_hp <= 0) {
                                            PlaySfx(sndDeath);
                                            state = 8;
                                            death_timer = 60;
                                        } else {
                                            PlaySfx(sndHurt);
                                            invincibility = 90;
                                        }
                                    }
                                } else {
                                    if (p_type == 1) {
                                        PlaySfx(sndHurt);
                                        p_type = 0; p_timer = 0;
                                        invincibility = 90;
                                        for (int sp = 0; sp < 6; sp++) {
                                            float pvx = ((float)(rand() % 40) - 20.0f) * 0.05f;
                                            float pvy = ((float)(rand() % 40) - 20.0f) * 0.05f;
                                            SpawnParticle(player_x + 5.0f, player_y + 5.0f, pvx, pvy, C_CYAN, 10, 1);
                                        }
                                    } else {
                                        player_hp--;
                                        screen_shake_timer = 15;
                                        g_damage_flash_timer = 6;
                                        if (p_type != 13) {
                                            combo = 1;
                                            combo_timer = 0;
                                            if(p_type == 8) { p_type = 0; p_timer = 0; }
                                        }
                                        if (current_game_mode == MODE_TIME_ATTACK) {
                                            ta_time_left_frames -= 300;
                                            SpawnFloatingText(player_x, player_y - 12, "-5 SEG", C_RED);
                                            if (ta_time_left_frames < 0) ta_time_left_frames = 0;
                                        }
                                        if (player_hp <= 0 || (current_game_mode == MODE_TIME_ATTACK && ta_time_left_frames <= 0)) {
                                            PlaySfx(sndDeath);
                                            state = 8;
                                            death_timer = 60;
                                        } else {
                                            PlaySfx(sndHurt);
                                            invincibility = 90;
                                        }
                                    }
                                    enemies[en_idx].x -= mx_dir * 15;
                                    enemies[en_idx].y -= my_dir * 15;
                                    if(enemies[en_idx].x < 0) enemies[en_idx].x = 0;
                                    if(enemies[en_idx].x > SCREEN_W - 10) enemies[en_idx].x = SCREEN_W - 10;
                                    if(enemies[en_idx].y < 16) enemies[en_idx].y = 16;
                                    if(enemies[en_idx].y > SCREEN_H - 10) enemies[en_idx].y = SCREEN_H - 10;
                                }
                            }
                        }
                    }
                }
            }
        }

        // --- RENDERIZADO AL VIRTUAL BUFFER (240x160) ---
        float alpha_interp = time_accumulator / FIXED_DELTA;
        if (alpha_interp < 0.0f) alpha_interp = 0.0f;
        if (alpha_interp > 1.0f) alpha_interp = 1.0f;

        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (state == 14) {
            int alpha = 16;
            if (intro_timer <= 48) alpha = (intro_timer * 16) / 48;
            else if (intro_timer > 170 && intro_timer <= 218) alpha = 16 - (((intro_timer - 170) * 16) / 48);
            else if (intro_timer > 218) alpha = 0;

            if (alpha > 0) {
                int intro_logo_y = (SCREEN_H - 120) / 2;
                if (intro_logo_y < 15) intro_logo_y = 15;
                int intro_txt_y = intro_logo_y + 98;
                if (logoTexture.id > 0) {
                    DrawTexture(logoTexture, (SCREEN_W - 80) / 2, intro_logo_y, (Color){ 255, 255, 255, (unsigned char)(alpha * 15.9f) });
                }
                Color c_brand = BlendColorAlpha(GBA_COLOR(31, 28, 0), alpha);
                Color c_text  = BlendColorAlpha(GBA_COLOR(24, 24, 24), alpha);
                int brand_w = MeasureStringCustom("ALEJADEV ", 1);
                int pres_w = MeasureStringCustom(T(STR_PRESENTS), 1);
                int intro_txt_x = (SCREEN_W - (brand_w + pres_w)) / 2;
                DrawStringCustom("ALEJADEV", intro_txt_x, intro_txt_y, c_brand, 1);
                DrawStringCustom(T(STR_PRESENTS), intro_txt_x + brand_w, intro_txt_y, c_text, 1);
            }
        }
        else if (state == 0) {
            float t_glitch = GetTitleBgmGlitchIntensity();
            int logo_gx = 0, logo_gy = 0;
            if (t_glitch > 0.15f && (rand() % 100 < (int)(t_glitch * 70))) {
                logo_gx = (rand() % 7) - 3;
                if (t_glitch > 0.5f) logo_gy = (rand() % 3) - 1;
            }

            int title_top = (SCREEN_H - 130) / 2;
            if (title_top < 12) title_top = 12;

            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, C_TITLE_BG);
            if (g_cheat_matrix_active) {
                // Lluvia digital de código Matrix estilizada, sutil y compacta en el menú principal
                int num_cols = SCREEN_W / 6;
                for (int c = 0; c < num_cols; c++) {
                    int col_x = c * 6 + 1;
                    int col_spd = 1 + (c * 7) % 3;
                    int head_y = ((frame_count * col_spd + c * 23) % (SCREEN_H + 28)) - 12;
                    if (head_y >= 0 && head_y < SCREEN_H) {
                        DrawRectangle(col_x, head_y, 2, 3, GBA_COLOR(26, 31, 26));
                        DrawRectangle(col_x, head_y + 1, 2, 1, WHITE);
                    }
                    for (int t = 1; t <= 8; t++) {
                        int ty = head_y - (t * 4);
                        if (ty >= 0 && ty < SCREEN_H) {
                            Color tc = (t < 2) ? GBA_COLOR(0, 31, 10) : ((t < 5) ? GBA_COLOR(0, 22, 6) : ((t < 7) ? GBA_COLOR(0, 14, 3) : GBA_COLOR(0, 7, 1)));
                            int seed = (frame_count / 8 + c * 11 + t) % 3;
                            if (seed == 0) {
                                DrawRectangle(col_x, ty, 2, 3, tc);
                            } else if (seed == 1) {
                                DrawRectangle(col_x, ty, 2, 1, tc);
                                DrawRectangle(col_x + 1, ty + 2, 1, 1, tc);
                            } else {
                                DrawRectangle(col_x + 1, ty, 1, 2, tc);
                                DrawRectangle(col_x, ty + 1, 2, 1, tc);
                            }
                        }
                    }
                }
            } else {
                DrawNebulaBackgroundPC(SCREEN_W, SCREEN_H, frame_count);
                for(int s = 0; s < MAX_TITLE_STARS; s++) {
                    if (!(t_glitch > 0.3f && (rand() % 10 < 3))) {
                        int sy = (int)roundf((float)title_stars[s].y - (float)title_stars[s].speed * (1.0f - alpha_interp));
                        if (sy < 2) sy += (SCREEN_H - 4);
                        DrawRectangle(title_stars[s].x, sy, 1, 1, title_stars[s].color);
                    }
                }
            }
            DrawAstralliticLogoPC((SCREEN_W - 208) / 2 + logo_gx, 8 + logo_gy, frame_count);
            DrawTitleDividerPC(SCREEN_W / 2 + (logo_gx / 2), 65 + logo_gy, 180, frame_count);
            DrawTitleFooterPC(frame_count);

            const char* m_options[4] = { T(STR_PLAY), T(STR_OPTIONS), T(STR_RECORDS), T(STR_MENU_SECRETS) };
            int m_icons[4] = { MENU_ICON_PLAY, MENU_ICON_OPTIONS, MENU_ICON_RECORDS, MENU_ICON_PADLOCK };
            Color m_colors[4] = { C_YELLOW, C_CYAN, GBA_COLOR(31, 26, 2), GBA_COLOR(25, 8, 30) };
            int btn_w = 150;
            int btn_x = (SCREEN_W - btn_w) / 2;
            for(int opt_idx = 0; opt_idx < 4; opt_idx++) {
                int opt_y = 74 + (opt_idx * 17);
                DrawSciFiButtonPC(btn_x, opt_y - 3, btn_w, 15, m_options[opt_idx], m_icons[opt_idx], (opt_idx == menu_selection), m_colors[opt_idx], frame_count);
            }

            if (show_konami_popup) {
                DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, 180 });
                int p_w = 216;
                int p_h = 100;
                int p_x = (SCREEN_W - p_w) / 2;
                int p_y = (SCREEN_H - p_h) / 2;

                DrawBevelledBoxPC(p_x, p_y, p_w, p_h, GBA_COLOR(1, 3, 7), C_YELLOW, true);
                DrawRectangle(p_x + 3, p_y + 3, 1, 1, C_YELLOW);
                DrawRectangle(p_x + p_w - 4, p_y + 3, 1, 1, C_YELLOW);
                DrawRectangle(p_x + 3, p_y + p_h - 4, 1, 1, C_YELLOW);
                DrawRectangle(p_x + p_w - 4, p_y + p_h - 4, 1, 1, C_YELLOW);

                DrawMenuIconPC(MENU_ICON_MEDAL, p_x + 8, p_y + 7, C_YELLOW);
                DrawMenuIconPC(MENU_ICON_MEDAL, p_x + p_w - 18, p_y + 7, C_YELLOW);

                DrawCenteredStringCustom(T(STR_KONAMI_TITLE), p_y + 8, C_YELLOW, 1);
                DrawRectangle(p_x + 12, p_y + 21, p_w - 24, 1, GBA_COLOR(10, 16, 22));

                DrawCenteredStringCustom(T(STR_KONAMI_SUB1), p_y + 29, WHITE, 1);
                DrawCenteredStringCustom(T(STR_KONAMI_SUB2), p_y + 42, WHITE, 1);

                DrawCenteredStringCustom(T(STR_KONAMI_SUB3), p_y + 60, C_CYAN, 1);

                Color c_hint = ((frame_count / 20) % 2 == 0) ? GBA_COLOR(20, 20, 20) : GBA_COLOR(12, 14, 16);
                DrawCenteredStringCustom(T(STR_CLOSE_HINT), p_y + 82, c_hint, 1);
            }

            if (g_show_cheat_popup) {
                DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){ 0, 0, 0, 205 });
                int p_w = 224;
                int p_h = 116;
                int p_x = (SCREEN_W - p_w) / 2;
                int p_y = (SCREEN_H - p_h) / 2;

                // Caja con borde verde neón y esquinas decoradas
                Color border_col = ((frame_count / 15) % 2 == 0) ? GBA_COLOR(0, 28, 14) : GBA_COLOR(0, 22, 10);
                DrawBevelledBoxPC(p_x, p_y, p_w, p_h, GBA_COLOR(0, 2, 4), border_col, true);
                DrawRectangle(p_x + 3, p_y + 3, 1, 1, border_col);
                DrawRectangle(p_x + p_w - 4, p_y + 3, 1, 1, border_col);
                DrawRectangle(p_x + 3, p_y + p_h - 4, 1, 1, border_col);
                DrawRectangle(p_x + p_w - 4, p_y + p_h - 4, 1, 1, border_col);

                // Título
                DrawCenteredStringCustom(T(STR_CHEAT_TITLE), p_y + 8, GBA_COLOR(0, 31, 16), 1);
                DrawRectangle(p_x + 12, p_y + 20, p_w - 24, 1, GBA_COLOR(0, 14, 8));

                // Subtítulos
                DrawCenteredStringCustom(T(STR_CHEAT_SUBTITLE), p_y + 26, WHITE, 1);
                DrawCenteredStringCustom(T(STR_CHEAT_SUBTITLE2), p_y + 38, GBA_COLOR(16, 24, 20), 1);

                // Campo de texto
                int field_x = p_x + 16;
                int field_y = p_y + 51;
                int field_w = p_w - 32;
                int field_h = 18;
                DrawBevelledBoxPC(field_x, field_y, field_w, field_h, GBA_COLOR(0, 4, 2), GBA_COLOR(0, 20, 10), true);

                if (g_cheat_input_len > 0) {
                    // Texto tecleado centrado
                    int txt_w = MeasureStringCustom(g_cheat_input, 1);
                    int txt_x = field_x + (field_w - txt_w) / 2;
                    DrawStringCustom(g_cheat_input, txt_x, field_y + 5, WHITE, 1);
                    // Cursor parpadeante
                    if ((frame_count / 20) % 2 == 0) {
                        DrawRectangle(txt_x + txt_w + 2, field_y + 5, 4, 7, GBA_COLOR(0, 31, 16));
                    }
                } else {
                    // Placeholder
                    int ph_w = MeasureStringCustom(T(STR_CHEAT_PLACEHOLDER), 1);
                    DrawStringCustom(T(STR_CHEAT_PLACEHOLDER), field_x + (field_w - ph_w) / 2, field_y + 5, GBA_COLOR(8, 14, 10), 1);
                }

                // Mensaje de estado: error o éxito
                if (g_cheat_error) {
                    DrawCenteredStringCustom(T(g_cheat_already_used_err ? STR_CHEAT_ALREADY_USED : STR_CHEAT_INVALID), p_y + 75, GBA_COLOR(31, 4, 4), 1);
                } else if (g_cheat_success) {
                    DrawCenteredStringCustom(g_cheat_success_msg[0] ? g_cheat_success_msg : T(STR_CHEAT_SUCCESS), p_y + 75, GBA_COLOR(4, 31, 12), 1);
                }

                // Hint de ayuda
                // Botón [X] táctil
                DrawRectangle(p_x + p_w - 18, p_y + 4, 14, 12, GBA_COLOR(14, 2, 2));
                DrawCharCustom('X', p_x + p_w - 13, p_y + 6, WHITE, 1);

                // Botón táctil para abrir teclado en iOS
                int kb_btn_w = 90;
                int kb_btn_x = p_x + (p_w - kb_btn_w) / 2;
                int kb_btn_y = p_y + 94;
                DrawBevelledBoxPC(kb_btn_w ? kb_btn_x : 0, kb_btn_y, kb_btn_w, 14, GBA_COLOR(0, 8, 4), GBA_COLOR(0, 24, 12), true);
                DrawCenteredStringCustom("ABRIR TECLADO", kb_btn_y + 3, C_YELLOW, 1);
            }
        }
        else if (state == 13) { 
            float t_glitch = GetTitleBgmGlitchIntensity();
            int logo_gx = 0, logo_gy = 0;
            if (t_glitch > 0.15f && (rand() % 100 < (int)(t_glitch * 70))) {
                logo_gx = (rand() % 7) - 3;
                if (t_glitch > 0.5f) logo_gy = (rand() % 3) - 1;
            }

            int title_top = (SCREEN_H - 130) / 2;
            if (title_top < 12) title_top = 12;

            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, C_TITLE_BG);
            DrawNebulaBackgroundPC(SCREEN_W, SCREEN_H, frame_count);
            for(int s = 0; s < MAX_TITLE_STARS; s++) {
                int sy = (int)roundf((float)title_stars[s].y - (float)title_stars[s].speed * (1.0f - alpha_interp));
                if (sy < 2) sy += (SCREEN_H - 4);
                DrawRectangle(title_stars[s].x, sy, 1, 1, title_stars[s].color);
            }
            DrawAstralliticLogoPC((SCREEN_W - 208) / 2 + logo_gx, 8 + logo_gy, frame_count);
            DrawTitleDividerPC(SCREEN_W / 2 + (logo_gx / 2), 65 + logo_gy, 180, frame_count);
            DrawTitleFooterPC(frame_count);

            int box_y = (SCREEN_H - 90) / 2;
            int exit_box_w = 180;
            int exit_box_x = (SCREEN_W - exit_box_w) / 2;
            DrawBevelledBoxPC(exit_box_x, box_y, exit_box_w, 90, GBA_COLOR(1, 3, 7), C_CYAN, true);
            DrawRectangle(exit_box_x + 3, box_y + 3, 1, 1, C_CYAN);
            DrawRectangle(exit_box_x + exit_box_w - 4, box_y + 3, 1, 1, C_CYAN);
            DrawRectangle(exit_box_x + 3, box_y + 86, 1, 1, C_CYAN);
            DrawRectangle(exit_box_x + exit_box_w - 4, box_y + 86, 1, 1, C_CYAN);

            DrawCenteredStringCustom(T(STR_QUIT_PROMPT), box_y + 10, GBA_COLOR(31, 28, 0), 1);
            DrawCenteredStringCustom(T(STR_QUIT_SUB), box_y + 25, GBA_COLOR(20, 20, 20), 1);
            DrawRectangle((SCREEN_W - 150) / 2, box_y + 39, 150, 1, GBA_COLOR(10, 16, 22));

            const char* exit_opts[2] = { T(STR_YES), T(STR_NO) };
            Color exit_cols[2] = { GBA_COLOR(31, 6, 6), GBA_COLOR(0, 31, 10) };
            int exit_icons[2] = { MENU_ICON_EXIT, MENU_ICON_NONE };

            int conf_btn_w = 150;
            int conf_btn_x = (SCREEN_W - conf_btn_w) / 2;
            for(int conf_idx = 0; conf_idx < 2; conf_idx++) {
                int opt_y = box_y + 51 + (conf_idx * 16);
                DrawSciFiButtonPC(conf_btn_x, opt_y - 2, conf_btn_w, 14, exit_opts[conf_idx], exit_icons[conf_idx], (conf_idx == exit_confirm_selection), exit_cols[conf_idx], frame_count);
            }
        }
        else if (state == 16) {
            Color c_lime_bg = GBA_COLOR(1, 4, 2);
            Color c_lime_title = GBA_COLOR(14, 31, 10);
            Color c_lime_shadow = GBA_COLOR(2, 10, 2);
            Color c_lime_line = GBA_COLOR(8, 26, 6);

            DrawMenuSpaceFramePC(c_lime_bg, "", c_lime_title, c_lime_shadow, c_lime_line);
            DrawNebulaBackgroundPC(SCREEN_W, SCREEN_H, frame_count);
            for(int m = 0; m < MAX_MENU_STARS; m++) {
                int my = (int)roundf((float)menu_stars[m].y + (float)menu_stars[m].vy * (1.0f - alpha_interp));
                if (my > SCREEN_H - 4) my -= (SCREEN_H - 6);
                DrawRectangle(menu_stars[m].x, my, 1, 1, GBA_COLOR(20, 31, 16));
            }

            // Cabecera compacta para Modos de Juego (no solapa con las tarjetas)
            DrawHeaderCenteredStringCustom(T(STR_GAME_MODE_TITLE), 9, c_lime_title, c_lime_shadow, 2);
            int div_w = 190;
            int div_x = (SCREEN_W - div_w) / 2;
            DrawRectangle(div_x, 27, div_w, 1, c_lime_line);
            DrawRectangle(div_x + div_w / 2 - 1, 26, 3, 3, c_lime_title);

            int card_w = 88;
            int card_h = 36;
            int gap_x = 16;
            int total_w = card_w * 2 + gap_x;
            int start_x = (SCREEN_W - total_w) / 2;
            int bx0 = start_x;
            int bx1 = start_x + card_w + gap_x;

            int top_margin = 28;
            int bottom_cap_y = SCREEN_H - 19;
            int avail_h = bottom_cap_y - top_margin;
            int content_h = (card_h + 9) * 2;
            int gap_v = (avail_h - content_h) / 3;
            if (gap_v < 6) gap_v = 6;
            int by0 = top_margin + gap_v;
            int by1 = by0 + card_h + 9 + gap_v;

            // Tarjeta 0: Modo Clásico
            bool sel0 = (game_mode_selection == 0);
            Color border0 = sel0 ? WHITE : c_lime_line;
            Color bg0 = sel0 ? GBA_COLOR(2, 8, 5) : GBA_COLOR(1, 4, 3);
            DrawBevelledBoxPC(bx0, by0, card_w, card_h, bg0, border0, true);
            if (sel0) {
                DrawRectangle(bx0 + 2, by0 + 2, 1, 1, C_CYAN);
                DrawRectangle(bx0 + card_w - 3, by0 + 2, 1, 1, C_CYAN);
                DrawRectangle(bx0 + 2, by0 + card_h - 3, 1, 1, C_CYAN);
                DrawRectangle(bx0 + card_w - 3, by0 + card_h - 3, 1, 1, C_CYAN);
            }
            DrawClassicCardArtworkPC(bx0, by0, card_w, card_h, frame_count);

            // Tarjeta 1: Modo Rush
            bool sel1 = (game_mode_selection == 1);
            Color border1 = sel1 ? WHITE : GBA_COLOR(14, 6, 6);
            Color bg1 = sel1 ? GBA_COLOR(6, 2, 2) : GBA_COLOR(3, 1, 1);
            DrawBevelledBoxPC(bx1, by0, card_w, card_h, bg1, border1, true);
            if (sel1) {
                DrawRectangle(bx1 + 2, by0 + 2, 1, 1, C_CYAN);
                DrawRectangle(bx1 + card_w - 3, by0 + 2, 1, 1, C_CYAN);
                DrawRectangle(bx1 + 2, by0 + card_h - 3, 1, 1, C_CYAN);
                DrawRectangle(bx1 + card_w - 3, by0 + card_h - 3, 1, 1, C_CYAN);
            }
            DrawRushCardArtworkPC(bx1, by0, card_w, card_h, frame_count);

            // Tarjeta 2: Modo Contrarreloj
            bool sel2 = (game_mode_selection == 2);
            Color border2 = sel2 ? WHITE : GBA_COLOR(0, 20, 26);
            Color bg2 = sel2 ? GBA_COLOR(1, 6, 12) : GBA_COLOR(0, 3, 6);
            DrawBevelledBoxPC(bx0, by1, card_w, card_h, bg2, border2, true);
            if (sel2) {
                DrawRectangle(bx0 + 2, by1 + 2, 1, 1, C_CYAN);
                DrawRectangle(bx0 + card_w - 3, by1 + 2, 1, 1, C_CYAN);
                DrawRectangle(bx0 + 2, by1 + card_h - 3, 1, 1, C_CYAN);
                DrawRectangle(bx0 + card_w - 3, by1 + card_h - 3, 1, 1, C_CYAN);
            }
            DrawTimeAttackCardArtworkPC(bx0, by1, card_w, card_h, frame_count);

            // Tarjeta 3: Modo Asalto
            bool sel3 = (game_mode_selection == 3);
            Color border3 = sel3 ? WHITE : GBA_COLOR(31, 14, 0);
            Color bg3 = sel3 ? GBA_COLOR(8, 4, 1) : GBA_COLOR(4, 2, 0);
            DrawBevelledBoxPC(bx1, by1, card_w, card_h, bg3, border3, true);
            if (sel3) {
                DrawRectangle(bx1 + 2, by1 + 2, 1, 1, C_CYAN);
                DrawRectangle(bx1 + card_w - 3, by1 + 2, 1, 1, C_CYAN);
                DrawRectangle(bx1 + 2, by1 + card_h - 3, 1, 1, C_CYAN);
                DrawRectangle(bx1 + card_w - 3, by1 + card_h - 3, 1, 1, C_CYAN);
            }
            DrawAssaultCardArtworkPC(bx1, by1, card_w, card_h, frame_count);

            // Etiquetas bajo las tarjetas
            int lbl_y0 = by0 + card_h + 3;
            int lbl_y1 = by1 + card_h + 3;
            int cx0 = bx0 + card_w / 2;
            int cx1 = bx1 + card_w / 2;

            const char* str_c = T(STR_MODE_CLASSIC);
            int len_c = MeasureStringCustom(str_c, 1);
            DrawStringCustom(str_c, cx0 - len_c / 2, lbl_y0, sel0 ? C_YELLOW : GBA_COLOR(20, 31, 16), 1);
            if (sel0) {
                DrawStringCustom(">", cx0 - len_c / 2 - 8, lbl_y0, C_CYAN, 1);
                DrawStringCustom("<", cx0 + len_c / 2 + 3, lbl_y0, C_CYAN, 1);
            }

            const char* str_r = T(STR_MODE_RUSH);
            int len_r = MeasureStringCustom(str_r, 1);
            DrawStringCustom(str_r, cx1 - len_r / 2, lbl_y0, sel1 ? C_YELLOW : GBA_COLOR(26, 12, 12), 1);
            if (sel1) {
                DrawStringCustom(">", cx1 - len_r / 2 - 8, lbl_y0, C_CYAN, 1);
                DrawStringCustom("<", cx1 + len_r / 2 + 3, lbl_y0, C_CYAN, 1);
            }

            const char* str_ta = T(STR_MODE_TIME_ATTACK);
            int len_ta = MeasureStringCustom(str_ta, 1);
            DrawStringCustom(str_ta, cx0 - len_ta / 2, lbl_y1, sel2 ? C_YELLOW : C_CYAN, 1);
            if (sel2) {
                DrawStringCustom(">", cx0 - len_ta / 2 - 8, lbl_y1, C_CYAN, 1);
                DrawStringCustom("<", cx0 + len_ta / 2 + 3, lbl_y1, C_CYAN, 1);
            }

            const char* str_as = T(STR_MODE_ASSAULT);
            int len_as = MeasureStringCustom(str_as, 1);
            DrawStringCustom(str_as, cx1 - len_as / 2, lbl_y1, sel3 ? C_YELLOW : GBA_COLOR(31, 14, 0), 1);
            if (sel3) {
                DrawStringCustom(">", cx1 - len_as / 2 - 8, lbl_y1, C_CYAN, 1);
                DrawStringCustom("<", cx1 + len_as / 2 + 3, lbl_y1, C_CYAN, 1);
            }

            // Modal emergente de Modo Rush
            if (show_rush_popup) {
                int pop_w = 210;
                int pop_h = 56;
                int pop_x = (SCREEN_W - pop_w) / 2;
                int pop_y = (SCREEN_H - pop_h) / 2;
                DrawBevelledBoxPC(pop_x, pop_y, pop_w, pop_h, GBA_COLOR(2, 3, 5), GBA_COLOR(31, 8, 8), true);
                DrawRectangle(pop_x + 2, pop_y + 2, 1, 1, C_YELLOW);
                DrawRectangle(pop_x + pop_w - 3, pop_y + 2, 1, 1, C_YELLOW);
                DrawRectangle(pop_x + 2, pop_y + pop_h - 3, 1, 1, C_YELLOW);
                DrawRectangle(pop_x + pop_w - 3, pop_y + pop_h - 3, 1, 1, C_YELLOW);

                DrawCenteredStringCustom(T(STR_RUSH_UNAVAILABLE_TITLE), pop_y + 8, C_YELLOW, 1);
                DrawRectangle(pop_x + 12, pop_y + 20, pop_w - 24, 1, GBA_COLOR(16, 6, 6));
                DrawCenteredStringCustom(T(STR_RUSH_UNAVAILABLE_DESC), pop_y + 26, WHITE, 1);
                DrawCenteredStringCustom(T(STR_CLOSE_HINT), pop_y + 40, C_CYAN, 1);
            }
        }
        else if (state == 5) {
            DrawMenuSpaceFramePC(C_DIFF_BG, T(STR_DIFF_TITLE), C_CYAN, GBA_COLOR(0, 10, 14), GBA_COLOR(0, 24, 31));
            DrawNebulaBackgroundPC(SCREEN_W, SCREEN_H, frame_count);
            for(int m = 0; m < MAX_MENU_STARS; m++) {
                int my = (int)roundf((float)menu_stars[m].y + (float)menu_stars[m].vy * (1.0f - alpha_interp));
                if (my > SCREEN_H - 4) my -= (SCREEN_H - 6);
                DrawRectangle(menu_stars[m].x, my, 1, 1, menu_stars[m].color);
            }
            int diff_top = (SCREEN_H - 120) / 2;
            if (diff_top < 15) diff_top = 15;
            const char* diff_names[4] = { T(STR_DIFF_EASY), T(STR_DIFF_NORMAL), T(STR_DIFF_HARD), T(STR_DIFF_HELL) };
            int diff_icons[4] = { MENU_ICON_DIFF_EASY, MENU_ICON_DIFF_NORMAL, MENU_ICON_DIFF_HARD, MENU_ICON_DIFF_HELL };
            Color diff_colors[4] = { GBA_COLOR(0, 31, 10), C_CYAN, GBA_COLOR(31, 18, 0), GBA_COLOR(31, 5, 5) };
            int diff_btn_w = 150;
            int diff_btn_x = (SCREEN_W - diff_btn_w) / 2;
            for(int diff_idx = 0; diff_idx < 4; diff_idx++) {
                int opt_y = diff_top + 34 + (diff_idx * 20);
                DrawSciFiButtonPC(diff_btn_x, opt_y - 3, diff_btn_w, 16, diff_names[diff_idx], diff_icons[diff_idx], (diff_idx == diff_selection), diff_colors[diff_idx], frame_count);
            }
        }
        else if (state == 6 || state == 15) {
            DrawMenuSpaceFramePC(C_REC_BG, "", GBA_COLOR(31, 28, 0), GBA_COLOR(10, 8, 0), GBA_COLOR(0, 28, 31));
            DrawNebulaBackgroundPC(SCREEN_W, SCREEN_H, frame_count);
            for(int m = 0; m < MAX_MENU_STARS; m++) {
                int my = (int)roundf((float)menu_stars[m].y + (float)menu_stars[m].vy * (1.0f - alpha_interp));
                if (my > SCREEN_H - 4) my -= (SCREEN_H - 6);
                DrawRectangle(menu_stars[m].x, my, 1, 1, menu_stars[m].color);
            }

            int rec_top = (SCREEN_H - 130) / 2;
            if (rec_top < 15) rec_top = 15;

            bool is_rec = (state == 6);
            
            int tabs_center = SCREEN_W / 2;
            int tab1_x = tabs_center - 95;
            int tab2_x = tabs_center + 10;
            DrawBevelledBoxPC(tab1_x, rec_top - 1, 85, 14, is_rec ? GBA_COLOR(3, 8, 16) : GBA_COLOR(1, 3, 6), is_rec ? C_YELLOW : GBA_COLOR(6, 10, 16), true);
            DrawMenuIconPC(MENU_ICON_RECORDS, tab1_x + 6, rec_top + 1, is_rec ? C_YELLOW : GBA_COLOR(12, 16, 20));
            DrawStringCustom(T(STR_RECORDS), tab1_x + 20, rec_top + 2, is_rec ? C_YELLOW : GBA_COLOR(12, 16, 20), 1);

            DrawBevelledBoxPC(tab2_x, rec_top - 1, 85, 14, !is_rec ? GBA_COLOR(3, 8, 16) : GBA_COLOR(1, 3, 6), !is_rec ? C_GREEN : GBA_COLOR(6, 10, 16), true);
            DrawMenuIconPC(MENU_ICON_MEDAL, tab2_x + 6, rec_top + 1, !is_rec ? C_GREEN : GBA_COLOR(12, 16, 20));
            DrawStringCustom(T(STR_LOGROS), tab2_x + 20, rec_top + 2, !is_rec ? C_GREEN : GBA_COLOR(12, 16, 20), 1);

            DrawRectangle((SCREEN_W - 210) / 2, rec_top + 16, 210, 1, GBA_COLOR(0, 24, 31));

            if (state == 6) {
                int rec_w = 170;
                int rec_x = (SCREEN_W - rec_w) / 2;
                int btn_w = 16;
                int btn_h = 32;
                int left_arrow_x = (rec_x - btn_w) / 2;
                int right_arrow_x = (rec_x + rec_w) + (SCREEN_W - (rec_x + rec_w) - btn_w) / 2;
                int arrow_y = rec_top + 68 - (btn_h / 2);

                if (records_mode_tab == 0) {
                    // Header Sub-etiqueta Modo Clásico
                    const char* mode_lbl = T(STR_MODE_CLASSIC);
                    int mlen = MeasureStringCustom(mode_lbl, 1);
                    int mcx = SCREEN_W / 2;
                    DrawStringCustom(mode_lbl, mcx - mlen / 2, rec_top + 19, C_YELLOW, 1);
                    DrawRectangle(mcx - mlen / 2 - 18, rec_top + 22, 12, 1, GBA_COLOR(0, 20, 26));
                    DrawRectangle(mcx + mlen / 2 + 6, rec_top + 22, 12, 1, GBA_COLOR(0, 20, 26));

                    const char* diff_labels[4] = { T(STR_DIFF_EASY), T(STR_DIFF_NORMAL), T(STR_DIFF_HARD), T(STR_DIFF_HELL) };
                    int diff_icons[4] = { MENU_ICON_DIFF_EASY, MENU_ICON_DIFF_NORMAL, MENU_ICON_DIFF_HARD, MENU_ICON_DIFF_HELL };
                    Color rank_colors[4] = { GBA_COLOR(0, 31, 10), C_CYAN, GBA_COLOR(31, 18, 0), GBA_COLOR(31, 6, 6) };
                    for(int r_idx = 0; r_idx < 4; r_idx++) {
                        int row_y = rec_top + 29 + (r_idx * 23);
                        DrawBevelledBoxPC(rec_x, row_y - 2, rec_w, 19, GBA_COLOR(1, 5, 12), GBA_COLOR(0, 20, 26), true);
                        DrawMenuIconPC(diff_icons[r_idx], rec_x + 8, row_y + 2, rank_colors[r_idx]);
                        DrawStringCustom(diff_labels[r_idx], rec_x + 24, row_y + 3, rank_colors[r_idx], 1);
                        DrawStringCustom(":", rec_x + 72, row_y + 3, GBA_COLOR(16, 20, 24), 1);
                        DrawScoreCustom(loadHighScorePC(r_idx), rec_x + 95, row_y + 1, GBA_COLOR(31, 28, 0));
                    }

                    // Botón Flecha Derecha (►) para ir a Modo Rush
                    bool r_hover = (mouse_x >= right_arrow_x - 6 && mouse_x <= right_arrow_x + btn_w + 6 &&
                                    mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6);
                    int bounce = r_hover ? (int)(sinf((float)frame_count * 0.2f) * 2.0f) : 0;
                    Color btn_bg = r_hover ? GBA_COLOR(3, 8, 16) : GBA_COLOR(1, 4, 10);
                    Color btn_border = r_hover ? C_YELLOW : GBA_COLOR(0, 20, 26);
                    DrawBevelledBoxPC(right_arrow_x, arrow_y, btn_w, btn_h, btn_bg, btn_border, true);
                    
                    Color arrow_col = r_hover ? WHITE : C_CYAN;
                    DrawRecordArrowPC(right_arrow_x + 5 + bounce, arrow_y + 10, true, arrow_col);
                } else if (records_mode_tab == 1) {
                    // Header Sub-etiqueta Modo Rush
                    const char* mode_lbl = T(STR_MODE_RUSH);
                    int mlen = MeasureStringCustom(mode_lbl, 1);
                    int mcx = SCREEN_W / 2;
                    DrawStringCustom(mode_lbl, mcx - mlen / 2, rec_top + 19, GBA_COLOR(31, 14, 0), 1);
                    DrawRectangle(mcx - mlen / 2 - 18, rec_top + 22, 12, 1, GBA_COLOR(24, 8, 0));
                    DrawRectangle(mcx + mlen / 2 + 6, rec_top + 22, 12, 1, GBA_COLOR(24, 8, 0));

                    int r_score = 0, r_wave = 0, r_kills = 0;
                    loadRushHighScorePC(&r_score, &r_wave, &r_kills);

                    // Fila 0: PUNTOS MAX
                    int row0_y = rec_top + 31;
                    DrawBevelledBoxPC(rec_x, row0_y - 2, rec_w, 21, GBA_COLOR(4, 2, 1), GBA_COLOR(28, 18, 0), true);
                    DrawMenuIconPC(MENU_ICON_MEDAL, rec_x + 8, row0_y + 3, C_YELLOW);
                    DrawStringCustom(T(STR_MAX_SCORE), rec_x + 24, row0_y + 4, C_YELLOW, 1);
                    DrawStringCustom(":", rec_x + 82, row0_y + 4, GBA_COLOR(20, 16, 12), 1);
                    DrawScoreCustom(r_score, rec_x + 95, row0_y + 2, GBA_COLOR(31, 28, 0));

                    // Fila 1: OLEADA MAX
                    int row1_y = rec_top + 59;
                    DrawBevelledBoxPC(rec_x, row1_y - 2, rec_w, 21, GBA_COLOR(4, 1, 1), GBA_COLOR(26, 8, 2), true);
                    DrawMenuIconPC(MENU_ICON_DIFF_HARD, rec_x + 8, row1_y + 3, GBA_COLOR(31, 14, 0));
                    DrawStringCustom(T(STR_MAX_WAVE), rec_x + 24, row1_y + 4, GBA_COLOR(31, 14, 0), 1);
                    DrawStringCustom(":", rec_x + 82, row1_y + 4, GBA_COLOR(20, 14, 12), 1);
                    DrawScoreCustom(r_wave, rec_x + 95, row1_y + 2, C_YELLOW);

                    // Fila 2: TOTAL ENEMIGOS K.O.
                    int row2_y = rec_top + 87;
                    DrawBevelledBoxPC(rec_x, row2_y - 2, rec_w, 21, GBA_COLOR(3, 1, 3), GBA_COLOR(20, 6, 22), true);
                    DrawSkullIconPC(rec_x + 7, row2_y + 4);
                    DrawStringCustom(T(STR_TOTAL_KILLS), rec_x + 24, row2_y + 4, GBA_COLOR(31, 8, 16), 1);
                    DrawStringCustom(":", rec_x + 82, row2_y + 4, GBA_COLOR(20, 14, 18), 1);
                    DrawScoreCustom(r_kills, rec_x + 95, row2_y + 2, WHITE);

                    // Botón Flecha Izquierda (◄) para volver a Modo Clásico
                    bool l_hover = (mouse_x >= left_arrow_x - 6 && mouse_x <= left_arrow_x + btn_w + 6 &&
                                    mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6);
                    int l_bounce = l_hover ? (int)(sinf((float)frame_count * 0.2f) * 2.0f) : 0;
                    Color l_btn_bg = l_hover ? GBA_COLOR(3, 8, 16) : GBA_COLOR(1, 4, 10);
                    Color l_btn_border = l_hover ? C_YELLOW : GBA_COLOR(0, 20, 26);
                    DrawBevelledBoxPC(left_arrow_x, arrow_y, btn_w, btn_h, l_btn_bg, l_btn_border, true);

                    Color l_arrow_col = l_hover ? WHITE : C_CYAN;
                    DrawRecordArrowPC(left_arrow_x + 5 - l_bounce, arrow_y + 10, false, l_arrow_col);

                    // Botón Flecha Derecha (►) para ir a Modo Contrarreloj
                    bool r_hover = (mouse_x >= right_arrow_x - 6 && mouse_x <= right_arrow_x + btn_w + 6 &&
                                    mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6);
                    int r_bounce = r_hover ? (int)(sinf((float)frame_count * 0.2f) * 2.0f) : 0;
                    Color r_btn_bg = r_hover ? GBA_COLOR(3, 8, 16) : GBA_COLOR(1, 4, 10);
                    Color r_btn_border = r_hover ? C_YELLOW : GBA_COLOR(0, 20, 26);
                    DrawBevelledBoxPC(right_arrow_x, arrow_y, btn_w, btn_h, r_btn_bg, r_btn_border, true);

                    Color r_arrow_col = r_hover ? WHITE : C_CYAN;
                    DrawRecordArrowPC(right_arrow_x + 5 + r_bounce, arrow_y + 10, true, r_arrow_col);
                } else if (records_mode_tab == 2) {
                    // Header Sub-etiqueta Modo Contrarreloj
                    const char* mode_lbl = T(STR_MODE_TIME_ATTACK);
                    int mlen = MeasureStringCustom(mode_lbl, 1);
                    int mcx = SCREEN_W / 2;
                    DrawStringCustom(mode_lbl, mcx - mlen / 2, rec_top + 19, C_CYAN, 1);
                    DrawRectangle(mcx - mlen / 2 - 18, rec_top + 22, 12, 1, GBA_COLOR(0, 20, 26));
                    DrawRectangle(mcx + mlen / 2 + 6, rec_top + 22, 12, 1, GBA_COLOR(0, 20, 26));

                    int ta_surv = 0, ta_score = 0, ta_kills = 0;
                    loadTimeAttackHighScorePC(&ta_surv, &ta_score, &ta_kills);

                    // Fila 0: TIEMPO MAX (MM:SS)
                    int row0_y = rec_top + 31;
                    DrawBevelledBoxPC(rec_x, row0_y - 2, rec_w, 21, GBA_COLOR(1, 4, 8), GBA_COLOR(0, 24, 31), true);
                    DrawMenuIconPC(MENU_ICON_MEDAL, rec_x + 8, row0_y + 3, C_CYAN);
                    DrawStringCustom(T(STR_MAX_TIME), rec_x + 24, row0_y + 4, C_CYAN, 1);
                    DrawStringCustom(":", rec_x + 82, row0_y + 4, GBA_COLOR(14, 20, 26), 1);
                    char time_buf[16];
                    snprintf(time_buf, sizeof(time_buf), "%02d:%02d", ta_surv / 60, ta_surv % 60);
                    DrawStringCustom(time_buf, rec_x + 95, row0_y + 2, C_YELLOW, 2);

                    // Fila 1: PUNTOS MAX
                    int row1_y = rec_top + 59;
                    DrawBevelledBoxPC(rec_x, row1_y - 2, rec_w, 21, GBA_COLOR(3, 3, 1), GBA_COLOR(24, 20, 0), true);
                    DrawMenuIconPC(MENU_ICON_DIFF_NORMAL, rec_x + 8, row1_y + 3, C_YELLOW);
                    DrawStringCustom(T(STR_MAX_SCORE), rec_x + 24, row1_y + 4, C_YELLOW, 1);
                    DrawStringCustom(":", rec_x + 82, row1_y + 4, GBA_COLOR(20, 16, 12), 1);
                    DrawScoreCustom(ta_score, rec_x + 95, row1_y + 2, GBA_COLOR(31, 28, 0));

                    // Fila 2: TOTAL ENEMIGOS K.O.
                    int row2_y = rec_top + 87;
                    DrawBevelledBoxPC(rec_x, row2_y - 2, rec_w, 21, GBA_COLOR(3, 1, 3), GBA_COLOR(20, 6, 22), true);
                    DrawSkullIconPC(rec_x + 7, row2_y + 4);
                    DrawStringCustom(T(STR_TOTAL_KILLS), rec_x + 24, row2_y + 4, GBA_COLOR(31, 8, 16), 1);
                    DrawStringCustom(":", rec_x + 82, row2_y + 4, GBA_COLOR(20, 14, 18), 1);
                    DrawScoreCustom(ta_kills, rec_x + 95, row2_y + 2, WHITE);

                    // Botón Flecha Izquierda (◄) para volver a Modo Rush
                    bool l_hover = (mouse_x >= left_arrow_x - 6 && mouse_x <= left_arrow_x + btn_w + 6 &&
                                    mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6);
                    int bounce = l_hover ? (int)(sinf((float)frame_count * 0.2f) * 2.0f) : 0;
                    Color btn_bg = l_hover ? GBA_COLOR(3, 8, 16) : GBA_COLOR(1, 4, 10);
                    Color btn_border = l_hover ? C_YELLOW : GBA_COLOR(0, 20, 26);
                    DrawBevelledBoxPC(left_arrow_x, arrow_y, btn_w, btn_h, btn_bg, btn_border, true);

                    Color arrow_col = l_hover ? WHITE : C_CYAN;
                    DrawRecordArrowPC(left_arrow_x + 5 - bounce, arrow_y + 10, false, arrow_col);

                    // Botón Flecha Derecha (►) para ir a Modo Asalto
                    bool r_hover = (mouse_x >= right_arrow_x - 6 && mouse_x <= right_arrow_x + btn_w + 6 &&
                                    mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6);
                    int r_bounce = r_hover ? (int)(sinf((float)frame_count * 0.2f) * 2.0f) : 0;
                    Color r_btn_bg = r_hover ? GBA_COLOR(3, 8, 16) : GBA_COLOR(1, 4, 10);
                    Color r_btn_border = r_hover ? C_YELLOW : GBA_COLOR(0, 20, 26);
                    DrawBevelledBoxPC(right_arrow_x, arrow_y, btn_w, btn_h, r_btn_bg, r_btn_border, true);

                    Color r_arrow_col = r_hover ? WHITE : C_CYAN;
                    DrawRecordArrowPC(right_arrow_x + 5 + r_bounce, arrow_y + 10, true, r_arrow_col);
                } else if (records_mode_tab == 3) {
                    // Header Sub-etiqueta Modo Asalto
                    const char* mode_lbl = T(STR_MODE_ASSAULT);
                    int mlen = MeasureStringCustom(mode_lbl, 1);
                    int mcx = SCREEN_W / 2;
                    DrawStringCustom(mode_lbl, mcx - mlen / 2, rec_top + 19, GBA_COLOR(31, 14, 0), 1);
                    DrawRectangle(mcx - mlen / 2 - 18, rec_top + 22, 12, 1, GBA_COLOR(24, 8, 0));
                    DrawRectangle(mcx + mlen / 2 + 6, rec_top + 22, 12, 1, GBA_COLOR(24, 8, 0));

                    int as_surv = 0, as_score = 0, as_kills = 0;
                    loadAssaultHighScorePC(&as_surv, &as_score, &as_kills);

                    // Fila 0: TIEMPO MAX (MM:SS)
                    int row0_y = rec_top + 31;
                    DrawBevelledBoxPC(rec_x, row0_y - 2, rec_w, 21, GBA_COLOR(4, 2, 0), GBA_COLOR(31, 14, 0), true);
                    DrawMenuIconPC(MENU_ICON_MEDAL, rec_x + 8, row0_y + 3, GBA_COLOR(31, 18, 0));
                    DrawStringCustom(T(STR_MAX_TIME), rec_x + 24, row0_y + 4, GBA_COLOR(31, 18, 0), 1);
                    DrawStringCustom(":", rec_x + 82, row0_y + 4, GBA_COLOR(20, 14, 12), 1);
                    char time_buf[16];
                    snprintf(time_buf, sizeof(time_buf), "%02d:%02d", as_surv / 60, as_surv % 60);
                    DrawStringCustom(time_buf, rec_x + 95, row0_y + 2, C_YELLOW, 2);

                    // Fila 1: PUNTOS MAX
                    int row1_y = rec_top + 59;
                    DrawBevelledBoxPC(rec_x, row1_y - 2, rec_w, 21, GBA_COLOR(3, 3, 1), GBA_COLOR(24, 20, 0), true);
                    DrawMenuIconPC(MENU_ICON_DIFF_NORMAL, rec_x + 8, row1_y + 3, C_YELLOW);
                    DrawStringCustom(T(STR_MAX_SCORE), rec_x + 24, row1_y + 4, C_YELLOW, 1);
                    DrawStringCustom(":", rec_x + 82, row1_y + 4, GBA_COLOR(20, 16, 12), 1);
                    DrawScoreCustom(as_score, rec_x + 95, row1_y + 2, GBA_COLOR(31, 28, 0));

                    // Fila 2: TOTAL ENEMIGOS K.O.
                    int row2_y = rec_top + 87;
                    DrawBevelledBoxPC(rec_x, row2_y - 2, rec_w, 21, GBA_COLOR(3, 1, 3), GBA_COLOR(20, 6, 22), true);
                    DrawSkullIconPC(rec_x + 7, row2_y + 4);
                    DrawStringCustom(T(STR_TOTAL_KILLS), rec_x + 24, row2_y + 4, GBA_COLOR(31, 8, 16), 1);
                    DrawStringCustom(":", rec_x + 82, row2_y + 4, GBA_COLOR(20, 14, 18), 1);
                    DrawScoreCustom(as_kills, rec_x + 95, row2_y + 2, WHITE);

                    // Botón Flecha Izquierda (◄) para volver a Modo Contrarreloj
                    bool l_hover = (mouse_x >= left_arrow_x - 6 && mouse_x <= left_arrow_x + btn_w + 6 &&
                                    mouse_y >= arrow_y - 6 && mouse_y <= arrow_y + btn_h + 6);
                    int bounce = l_hover ? (int)(sinf((float)frame_count * 0.2f) * 2.0f) : 0;
                    Color btn_bg = l_hover ? GBA_COLOR(3, 8, 16) : GBA_COLOR(1, 4, 10);
                    Color btn_border = l_hover ? C_YELLOW : GBA_COLOR(0, 20, 26);
                    DrawBevelledBoxPC(left_arrow_x, arrow_y, btn_w, btn_h, btn_bg, btn_border, true);

                    Color arrow_col = l_hover ? WHITE : C_CYAN;
                    DrawRecordArrowPC(left_arrow_x + 5 - bounce, arrow_y + 10, false, arrow_col);
                }
            } else {
                int start_index = (selected_ach_index / 8) * 8;
                int grid_start_x = 16;

                for (int i = 0; i < 8; i++) {
                    int ach_idx = start_index + i;
                    if (ach_idx >= NUM_ACHIEVEMENTS) break;

                    int row = i / 4;
                    int col = i % 4;
                    int bx = grid_start_x + (col * 45);
                    int by = rec_top + 22 + (row * 35);
                    int bw = 36;
                    int bh = 32;
                    bool is_unlocked = g_achievements_unlocked[ach_idx];
                    bool is_selected = (selected_ach_index == ach_idx);

                    Color card_border = is_selected ? (is_unlocked ? WHITE : C_YELLOW) : (is_unlocked ? C_GREEN : GBA_COLOR(6, 10, 16));
                    DrawBevelledBoxPC(bx, by, bw, bh, GBA_COLOR(1, 4, 10), card_border, true);
                    
                    if (is_selected) {
                        DrawRectangle(bx + 2, by + 2, 1, 1, C_CYAN);
                        DrawRectangle(bx + bw - 3, by + 2, 1, 1, C_CYAN);
                        DrawRectangle(bx + 2, by + bh - 3, 1, 1, C_CYAN);
                        DrawRectangle(bx + bw - 3, by + bh - 3, 1, 1, C_CYAN);
                    }

                    int center_x = bx + bw / 2;
                    int center_y = by + bh / 2;

                    if (!is_unlocked) {
                        DrawPadlockPC(center_x, center_y);
                    } else {
                        Enemy preview_enemy = { .x = center_x - 10, .y = center_y - 10, .type = ach_enemy_types[ach_idx], .active = 1, .hp = 1 };
                        DrawEnemyScaledPC(&preview_enemy, frame_count, 2);
                    }
                }

                // Triangulos de navegacion de Logros en lateral derecho
                int total_pages = (NUM_ACHIEVEMENTS + 7) / 8;
                int cur_page = selected_ach_index / 8;

                bool can_ach_up = (cur_page > 0);
                Color ach_up_col = can_ach_up ? C_YELLOW : GBA_COLOR(6, 8, 12);
                Color ach_up_border = can_ach_up ? WHITE : GBA_COLOR(10, 14, 18);
                Vector2 a1_up = { 214, 52 };
                Vector2 a2_up = { 204, 72 };
                Vector2 a3_up = { 224, 72 };
                DrawTriangle(a1_up, a2_up, a3_up, ach_up_col);
                DrawTriangleLines(a1_up, a2_up, a3_up, ach_up_border);

                char page_buf[16];
                snprintf(page_buf, sizeof(page_buf), "%d/%d", cur_page + 1, total_pages);
                int page_w = MeasureStringCustom(page_buf, 1);
                DrawStringCustom(page_buf, 214 - (page_w / 2), 87, C_CYAN, 1);

                bool can_ach_dn = (cur_page < total_pages - 1);
                Color ach_dn_col = can_ach_dn ? C_YELLOW : GBA_COLOR(6, 8, 12);
                Color ach_dn_border = can_ach_dn ? WHITE : GBA_COLOR(10, 14, 18);
                Vector2 a1_dn = { 204, 106 };
                Vector2 a2_dn = { 224, 106 };
                Vector2 a3_dn = { 214, 126 };
                DrawTriangle(a1_dn, a2_dn, a3_dn, ach_dn_col);
                DrawTriangleLines(a1_dn, a2_dn, a3_dn, ach_dn_border);

                if (show_ach_details) {
                    int pop_w = 200;
                    int pop_x = (SCREEN_W - pop_w) / 2;
                    int pop_y = rec_top + 53;
                    Color pop_border = g_achievements_unlocked[selected_ach_index] ? C_GREEN : C_CYAN;
                    DrawBevelledBoxPC(pop_x, pop_y, pop_w, 56, GBA_COLOR(1, 4, 8), pop_border, true);

                    DrawStringCustom(T(GetAchTitleId(selected_ach_index)), pop_x + 9, pop_y + 6, g_achievements_unlocked[selected_ach_index] ? C_GREEN : C_YELLOW, 1);
                    DrawStringCustom(T(GetAchDescId(selected_ach_index)), pop_x + 9, pop_y + 19, WHITE, 1);

                    char prog_buf[48];
                    if (g_achievements_unlocked[selected_ach_index]) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s", T(STR_STATUS_UNLOCKED));
                    } else if (selected_ach_index == ACH_FULL_ARSENAL) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/6", T(STR_PROGRESS), CountAchievementBits(g_achievement_progress[ACH_FULL_ARSENAL]));
                    } else if (selected_ach_index == ACH_LEYENDA) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/%d", T(STR_PROGRESS), g_achievement_progress[ACH_LEYENDA], ACH_LEYENDA);
                    } else if (selected_ach_index == ACH_RUSH_VENENO_ATOPE) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/500", T(STR_PROGRESS), g_achievement_progress[ACH_RUSH_VENENO_ATOPE]);
                    } else if (selected_ach_index == ACH_RUSH_HIELO_ATOPE) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/500", T(STR_PROGRESS), g_achievement_progress[ACH_RUSH_HIELO_ATOPE]);
                    } else if (selected_ach_index == ACH_RUSH_HASTA_ARRIBA) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/5", T(STR_PROGRESS), g_achievement_progress[ACH_RUSH_HASTA_ARRIBA]);
                    } else if (selected_ach_index == ACH_RUSH_PODEROSO) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/60", T(STR_PROGRESS), g_achievement_progress[ACH_RUSH_PODEROSO]);
                    } else if (selected_ach_index == ACH_RUSH_MUERTE_SUBITA) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/50", T(STR_PROGRESS), g_achievement_progress[ACH_RUSH_MUERTE_SUBITA]);
                    } else if (selected_ach_index == ACH_TA_1MIN) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/60s", T(STR_PROGRESS), g_achievement_progress[ACH_TA_1MIN]);
                    } else if (selected_ach_index == ACH_TA_3MIN) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/180s", T(STR_PROGRESS), g_achievement_progress[ACH_TA_3MIN]);
                    } else if (selected_ach_index == ACH_TA_5MIN) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/300s", T(STR_PROGRESS), g_achievement_progress[ACH_TA_5MIN]);
                    } else if (selected_ach_index == ACH_TA_RELOJ_LLENO) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/90s", T(STR_PROGRESS), g_achievement_progress[ACH_TA_RELOJ_LLENO]);
                    } else if (selected_ach_index == ACH_TA_CAZADOR) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/100", T(STR_PROGRESS), g_achievement_progress[ACH_TA_CAZADOR]);
                    } else if (selected_ach_index == ACH_TA_BOSS) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/1", T(STR_PROGRESS), g_achievement_progress[ACH_TA_BOSS]);
                    } else if (selected_ach_index == ACH_ASSAULT_1MIN) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/60s", T(STR_PROGRESS), g_achievement_progress[ACH_ASSAULT_1MIN]);
                    } else if (selected_ach_index == ACH_ASSAULT_3MIN) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/180s", T(STR_PROGRESS), g_achievement_progress[ACH_ASSAULT_3MIN]);
                    } else if (selected_ach_index == ACH_ASSAULT_5MIN) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/300s", T(STR_PROGRESS), g_achievement_progress[ACH_ASSAULT_5MIN]);
                    } else if (selected_ach_index == ACH_ASSAULT_CAZADOR) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/100", T(STR_PROGRESS), g_achievement_progress[ACH_ASSAULT_CAZADOR]);
                    } else if (selected_ach_index == ACH_ASSAULT_BOSS) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/1", T(STR_PROGRESS), g_achievement_progress[ACH_ASSAULT_BOSS]);
                    } else if (selected_ach_index == ACH_ASSAULT_RULETA) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/5", T(STR_PROGRESS), g_achievement_progress[ACH_ASSAULT_RULETA]);
                    } else {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d", T(STR_PROGRESS), g_achievement_progress[selected_ach_index]);
                    }
                    DrawStringCustom(prog_buf, pop_x + 9, pop_y + 35, g_achievements_unlocked[selected_ach_index] ? C_GREEN : GBA_COLOR(31, 10, 10), 1);
                    DrawCenteredStringCustom(T(STR_TOUCH_BACK), pop_y + 46, GBA_COLOR(10, 18, 24), 1);
                } else {
                    DrawCenteredStringCustom(T(STR_ACH_DETAILS_HINT), 125, GBA_COLOR(12, 18, 24), 1);
                }
            }
        }
        else if (state == 7) {
            DrawMenuSpaceFramePC(C_OPT_BG, T(STR_OPTIONS), GBA_COLOR(31, 28, 0), GBA_COLOR(12, 10, 0), GBA_COLOR(20, 10, 31));
            DrawNebulaBackgroundPC(SCREEN_W, SCREEN_H, frame_count);
            for(int m = 0; m < MAX_MENU_STARS; m++) {
                int my = (int)roundf((float)menu_stars[m].y + (float)menu_stars[m].vy * (1.0f - alpha_interp));
                if (my > SCREEN_H - 4) my -= (SCREEN_H - 6);
                DrawRectangle(menu_stars[m].x, my, 1, 1, menu_stars[m].color);
            }

            int opt_top = (SCREEN_H - 130) / 2;
            if (opt_top < 15) opt_top = 15;
            int opt_start_y = opt_top + 37;

            char opt_strings[8][64];
            snprintf(opt_strings[0], sizeof(opt_strings[0]), "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(opt_strings[1], sizeof(opt_strings[1]), "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(opt_strings[2], sizeof(opt_strings[2]), "%s: < %s >", T(STR_FILTER), GetFilterName(g_config.crt_filter, g_config.language));
            snprintf(opt_strings[3], sizeof(opt_strings[3]), "%s", T(STR_CONTROLS));
            snprintf(opt_strings[4], sizeof(opt_strings[4]), "%s: < %s >", T(STR_LANGUAGE), g_lang_names[g_config.language]);
            snprintf(opt_strings[5], sizeof(opt_strings[5]), "%s: < %s >", T(STR_SCREEN_MODE), g_screen_mode_names[g_config.screen_mode]);
            snprintf(opt_strings[6], sizeof(opt_strings[6]), "%s: < %s >", T(STR_FPS), GetFpsOptionText(g_config.target_fps));
            snprintf(opt_strings[7], sizeof(opt_strings[7]), "%s", T(STR_DELETE_RECORDS));

            int opt_icons[8] = {
                MENU_ICON_AUDIO, MENU_ICON_AUDIO, MENU_ICON_VIDEO, MENU_ICON_CONTROLS,
                MENU_ICON_NONE, MENU_ICON_VIDEO, MENU_ICON_VIDEO, MENU_ICON_EXIT
            };

            int total_opts = 8;
            int row_w = 175;
            int row_x = 25;
            for (int slot = 0; slot < 3; slot++) {
                int opt_idx = options_scroll_offset + slot;
                if (opt_idx >= total_opts) break;
                int row_y = opt_start_y + (slot * 30);
                bool is_sel = (options_selection == opt_idx);

                Color opt_border = is_sel ? GBA_COLOR(31, 24, 0) : GBA_COLOR(6, 12, 18);
                Color opt_bg = is_sel ? GBA_COLOR(4, 8, 16) : GBA_COLOR(1, 3, 7);
                DrawBevelledBoxPC(row_x, row_y - 3, row_w, 22, opt_bg, opt_border, true);

                if (is_sel) {
                    float pulse = (sinf((float)frame_count * 0.14f) + 1.0f) * 0.5f;
                    int bounce = (int)(sinf((float)frame_count * 0.20f) * 2.0f);
                    DrawCharCustom('>', row_x + 4 + bounce, row_y + 4, C_YELLOW, 1);
                    DrawCharCustom('>', row_x + 9 + bounce, row_y + 4, ColorLerp(C_YELLOW, WHITE, pulse), 1);
                    DrawCharCustom('<', row_x + row_w - 14 - bounce, row_y + 4, ColorLerp(C_YELLOW, WHITE, pulse), 1);
                    DrawCharCustom('<', row_x + row_w - 9 - bounce, row_y + 4, C_YELLOW, 1);
                    DrawCenteredStringCustom(opt_strings[opt_idx], row_y + 4, C_YELLOW, 1);
                } else {
                    DrawCenteredStringCustom(opt_strings[opt_idx], row_y + 4, GBA_COLOR(18, 16, 24), 1);
                }

                if (opt_idx == 4) {
                    DrawLanguageFlag16Bit(g_config.language, is_sel ? row_x + 18 : row_x + 6, row_y + 4);
                } else if (opt_icons[opt_idx] != MENU_ICON_NONE) {
                    DrawMenuIconPC(opt_icons[opt_idx], is_sel ? row_x + 18 : row_x + 6, row_y + 3, is_sel ? C_YELLOW : GBA_COLOR(10, 14, 18));
                }
            }

            // Botones triangulares arriba y abajo en el lateral derecho
            bool can_scroll_up = (options_selection > 0);
            Color up_col = can_scroll_up ? C_YELLOW : GBA_COLOR(6, 8, 12);
            Color up_border = can_scroll_up ? WHITE : GBA_COLOR(10, 14, 18);
            Vector2 p1_up = { 216, 52 };
            Vector2 p2_up = { 206, 72 };
            Vector2 p3_up = { 226, 72 };
            DrawTriangle(p1_up, p2_up, p3_up, up_col);
            DrawTriangleLines(p1_up, p2_up, p3_up, up_border);

            bool can_scroll_down = (options_selection < total_opts - 1);
            Color down_col = can_scroll_down ? C_YELLOW : GBA_COLOR(6, 8, 12);
            Color down_border = can_scroll_down ? WHITE : GBA_COLOR(10, 14, 18);
            Vector2 p1_dn = { 206, 106 };
            Vector2 p2_dn = { 226, 106 };
            Vector2 p3_dn = { 216, 126 };
            DrawTriangle(p1_dn, p2_dn, p3_dn, down_col);
            DrawTriangleLines(p1_dn, p2_dn, p3_dn, down_border);
        }
        else if (state == 12) {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, C_PAUSE_OPT_BG);
            DrawNebulaBackgroundPC(SCREEN_W, SCREEN_H, frame_count);

            for (int m = 0; m < MAX_MENU_STARS; m++) {
                int my = (int)roundf((float)menu_stars[m].y + (float)menu_stars[m].vy * (1.0f - alpha_interp));
                if (my > SCREEN_H - 4) my -= (SCREEN_H - 6);
                DrawRectangle(menu_stars[m].x, my, 1, 1, menu_stars[m].color);
            }

            int opt_top = (SCREEN_H - 130) / 2;
            if (opt_top < 15) opt_top = 15;
            int header_y = opt_top + 1;

            DrawHeaderCenteredStringCustom(T(STR_OPTIONS), header_y, C_CYAN, GBA_COLOR(0, 10, 18), 2);
            DrawRectangle((SCREEN_W - 184) / 2, header_y + 31, 184, 1, GBA_COLOR(0, 24, 31));

            char pause_opt_strings[6][64];
            snprintf(pause_opt_strings[0], sizeof(pause_opt_strings[0]), "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(pause_opt_strings[1], sizeof(pause_opt_strings[1]), "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(pause_opt_strings[2], sizeof(pause_opt_strings[2]), "%s: < %s >", T(STR_FILTER), GetFilterName(g_config.crt_filter, g_config.language));
            snprintf(pause_opt_strings[3], sizeof(pause_opt_strings[3]), "%s", T(STR_CONTROLS));
            snprintf(pause_opt_strings[4], sizeof(pause_opt_strings[4]), "%s: < %s >", T(STR_SCREEN_MODE), g_screen_mode_names[g_config.screen_mode]);
            snprintf(pause_opt_strings[5], sizeof(pause_opt_strings[5]), "%s: < %s >", T(STR_FPS), GetFpsOptionText(g_config.target_fps));

            int p_icons[6] = {
                MENU_ICON_AUDIO, MENU_ICON_AUDIO, MENU_ICON_VIDEO, MENU_ICON_CONTROLS,
                MENU_ICON_VIDEO, MENU_ICON_VIDEO
            };

            int total_pause_opts = 6;
            int p_row_w = 175;
            int p_row_x = 25;
            int opt_start_y = opt_top + 40;
            for (int slot = 0; slot < 3; slot++) {
                int opt_idx = pause_options_scroll_offset + slot;
                if (opt_idx >= total_pause_opts) break;

                int row_y = opt_start_y + (slot * 30);
                bool is_sel = (pause_options_selection == opt_idx);

                Color p_bg = is_sel ? GBA_COLOR(2, 10, 18) : GBA_COLOR(1, 4, 10);
                Color p_border = is_sel ? C_CYAN : GBA_COLOR(0, 18, 26);
                DrawBevelledBoxPC(p_row_x, row_y - 4, p_row_w, 22, p_bg, p_border, true);

                if (is_sel) {
                    float pulse = (sinf((float)frame_count * 0.14f) + 1.0f) * 0.5f;
                    int bounce = (int)(sinf((float)frame_count * 0.20f) * 2.0f);
                    DrawCharCustom('>', p_row_x + 4 + bounce, row_y + 4, C_YELLOW, 1);
                    DrawCharCustom('>', p_row_x + 9 + bounce, row_y + 4, ColorLerp(C_YELLOW, WHITE, pulse), 1);
                    if (opt_idx != 3) {
                        DrawCharCustom('<', p_row_x + p_row_w - 14 - bounce, row_y + 4, ColorLerp(C_YELLOW, WHITE, pulse), 1);
                        DrawCharCustom('<', p_row_x + p_row_w - 9 - bounce, row_y + 4, C_YELLOW, 1);
                    }
                    DrawCenteredStringCustom(pause_opt_strings[opt_idx], row_y + 4, C_YELLOW, 1);
                } else {
                    DrawCenteredStringCustom(pause_opt_strings[opt_idx], row_y + 4, GBA_COLOR(14, 22, 28), 1);
                }

                DrawMenuIconPC(p_icons[opt_idx], is_sel ? p_row_x + 18 : p_row_x + 6, row_y + 3, is_sel ? C_CYAN : GBA_COLOR(0, 18, 24));
            }

            // Botones triangulares lateral derecho en pausa
            bool can_p_scroll_up = (pause_options_selection > 0);
            Color p_up_col = can_p_scroll_up ? C_YELLOW : GBA_COLOR(6, 8, 12);
            Color p_up_border = can_p_scroll_up ? WHITE : GBA_COLOR(10, 14, 18);
            Vector2 p1_pup = { 216, 52 };
            Vector2 p2_pup = { 206, 72 };
            Vector2 p3_pup = { 226, 72 };
            DrawTriangle(p1_pup, p2_pup, p3_pup, p_up_col);
            DrawTriangleLines(p1_pup, p2_pup, p3_pup, p_up_border);

            bool can_p_scroll_down = (pause_options_selection < total_pause_opts - 1);
            Color p_down_col = can_p_scroll_down ? C_YELLOW : GBA_COLOR(6, 8, 12);
            Color p_down_border = can_p_scroll_down ? WHITE : GBA_COLOR(10, 14, 18);
            Vector2 p1_pdn = { 206, 106 };
            Vector2 p2_pdn = { 226, 106 };
            Vector2 p3_pdn = { 216, 126 };
            DrawTriangle(p1_pdn, p2_pdn, p3_pdn, p_down_col);
            DrawTriangleLines(p1_pdn, p2_pdn, p3_pdn, p_down_border);

            char pause_back_msg[48];
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            if (g_last_input_device != INPUT_GAMEPAD) {
                snprintf(pause_back_msg, sizeof(pause_back_msg), "%s", T(STR_TOUCH_BACK));
            } else {
                const char* p_back_key = "B";
                snprintf(pause_back_msg, sizeof(pause_back_msg), T(STR_PRESS_BACK), p_back_key);
            }
#else
            const char* p_back_key = (g_last_input_device == INPUT_GAMEPAD) ? "B" : GetKeyNameCustom(g_keys.key_turbo);
            snprintf(pause_back_msg, sizeof(pause_back_msg), T(STR_PRESS_BACK), p_back_key);
#endif
            int p_msg_len = MeasureStringCustom(pause_back_msg, 1);
            int p_cap_w = p_msg_len + 16;
            int p_cap_x = (SCREEN_W - p_cap_w) / 2;
            int p_cap_y = SCREEN_H - 19;
            DrawBevelledBoxPC(p_cap_x, p_cap_y, p_cap_w, 11, GBA_COLOR(2, 6, 12), GBA_COLOR(0, 20, 28), true);
            DrawCenteredStringCustom(pause_back_msg, p_cap_y + 2, GBA_COLOR(14, 22, 28), 1);
        }
        else if (state == 10) {
            DrawMenuSpaceFramePC(C_OPT_BG, T(STR_OPTIONS), GBA_COLOR(31, 28, 0), GBA_COLOR(12, 10, 0), GBA_COLOR(20, 10, 31));
            int d_box_w = 180;
            int d_box_x = (SCREEN_W - d_box_w) / 2;
            int d_box_y = (SCREEN_H - 90) / 2;
            DrawBevelledBoxPC(d_box_x, d_box_y, d_box_w, 90, GBA_COLOR(2, 3, 6), C_CYAN, true);
            DrawRectangle(d_box_x + 3, d_box_y + 3, 1, 1, C_CYAN);
            DrawRectangle(d_box_x + d_box_w - 4, d_box_y + 3, 1, 1, C_CYAN);
            DrawRectangle(d_box_x + 3, d_box_y + 86, 1, 1, C_CYAN);
            DrawRectangle(d_box_x + d_box_w - 4, d_box_y + 86, 1, 1, C_CYAN);

            DrawCenteredStringCustom(T(STR_CONFIRM_DELETE_TITLE), d_box_y + 10, GBA_COLOR(31, 28, 0), 1);
            DrawCenteredStringCustom(T(STR_CONFIRM_DELETE_SUB), d_box_y + 25, GBA_COLOR(20, 10, 10), 1);
            DrawRectangle((SCREEN_W - 150) / 2, d_box_y + 39, 150, 1, GBA_COLOR(10, 16, 22));
            const char* conf_opts[2] = { T(STR_CANCEL), T(STR_CONFIRM_DELETE_BTN) };
            Color conf_cols[2] = { GBA_COLOR(0, 31, 10), GBA_COLOR(31, 6, 6) };
            int d_btn_w = 150;
            int d_btn_x = (SCREEN_W - d_btn_w) / 2;
            for(int c_idx = 0; c_idx < 2; c_idx++) {
                int opt_y = d_box_y + 51 + (c_idx * 16);
                DrawSciFiButtonPC(d_btn_x, opt_y - 2, d_btn_w, 14, conf_opts[c_idx], (c_idx == 1 ? MENU_ICON_EXIT : MENU_ICON_NONE), (c_idx == confirm_selection), conf_cols[c_idx], frame_count);
            }
        }
        else if (state == 4) {
            if (g_last_input_device != INPUT_GAMEPAD) {
                DrawMenuSpaceFramePC(C_CTRL_BG, T(STR_CONTROLS), GBA_COLOR(0, 31, 10), GBA_COLOR(0, 10, 4), GBA_COLOR(0, 26, 10));
                DrawNebulaBackgroundPC(SCREEN_W, SCREEN_H, frame_count);
                for(int m = 0; m < MAX_MENU_STARS; m++) {
                    int my = (int)roundf((float)menu_stars[m].y + (float)menu_stars[m].vy * (1.0f - alpha_interp));
                    if (my > SCREEN_H - 4) my -= (SCREEN_H - 6);
                    DrawRectangle(menu_stars[m].x, my, 1, 1, menu_stars[m].color);
                }

                // Chasis exterior del teléfono móvil en horizontal
                DrawRectangle(22, 38, 196, 98, GBA_COLOR(2, 4, 8));
                DrawRectangleLines(22, 38, 196, 98, GBA_COLOR(0, 26, 31));
                // Pantalla interior
                DrawRectangle(34, 42, 172, 90, GBA_COLOR(1, 2, 5));
                DrawRectangleLines(34, 42, 172, 90, GBA_COLOR(0, 16, 22));

                // Cámara frontal (izq) y altavoz (der)
                DrawCircle(28, 87, 2, GBA_COLOR(8, 12, 16));
                DrawRectangle(212, 81, 2, 12, GBA_COLOR(8, 12, 16));

                // --- LADO IZQUIERDO: JOYSTICK VIRTUAL FLOTANTE (ESTILO RETRO 16-BIT) ---
                DrawRectangle(44, 62, 38, 38, GBA_COLOR(1, 4, 10));
                DrawRectangleLines(44, 62, 38, 38, GBA_COLOR(0, 24, 31));
                // Flechas cardinales
                DrawRectangle(61, 64, 4, 3, WHITE);
                DrawRectangle(61, 95, 4, 3, WHITE);
                DrawRectangle(46, 79, 3, 4, WHITE);
                DrawRectangle(77, 79, 3, 4, WHITE);
                // Pomo central
                DrawRectangle(55, 73, 16, 16, C_CYAN);
                DrawRectangleLines(55, 73, 16, 16, WHITE);
                DrawRectangle(60, 78, 6, 6, WHITE);
                int move_w = MeasureStringCustom(T(STR_CTRL_MOVE), 1);
                DrawStringCustom(T(STR_CTRL_MOVE), 63 - (move_w / 2), 108, C_CYAN, 1);

                // --- LADO DERECHO: BOTONES TÁCTILES RETRO (NUEVA DISTRIBUCIÓN) ---
                // Botón Pausa [II] (esquina superior derecha de la pantalla interior)
                DrawRectangle(188, 46, 12, 10, GBA_COLOR(4, 8, 12));
                DrawRectangleLines(188, 46, 12, 10, C_CYAN);
                DrawRectangle(189, 47, 10, 1, (Color){ 255, 255, 255, 100 });
                DrawStringCustom("II", 192, 48, WHITE, 1);
                int p_w = MeasureStringCustom(T(STR_PAUSE), 1);
                DrawStringCustom(T(STR_PAUSE), 184 - p_w, 48, GBA_COLOR(18, 22, 26), 1);

                // Botón Dash [D] (Verde esmeralda, posición superior izquierda)
                DrawRectangle(138, 66, 14, 14, GBA_COLOR(2, 18, 8));
                DrawRectangleLines(138, 66, 14, 14, C_GREEN);
                DrawRectangle(139, 67, 12, 1, (Color){ 255, 255, 255, 120 });
                DrawStringCustom("D", 143, 70, C_GREEN, 1);
                int d_w = MeasureStringCustom(T(STR_CTRL_DASH), 1);
                DrawStringCustom(T(STR_CTRL_DASH), 134 - d_w, 70, C_GREEN, 1);

                // Botón Turbo [T] (Azul / Cian, posición inferior izquierda)
                DrawRectangle(138, 96, 14, 14, GBA_COLOR(2, 10, 26));
                DrawRectangleLines(138, 96, 14, 14, C_CYAN);
                DrawRectangle(139, 97, 12, 1, (Color){ 255, 255, 255, 120 });
                DrawStringCustom("T", 143, 100, C_CYAN, 1);
                int t_w = MeasureStringCustom(T(STR_CTRL_TURBO), 1);
                DrawStringCustom(T(STR_CTRL_TURBO), 134 - t_w, 100, C_CYAN, 1);

                // Botón Aim Lock [L] (Dorado / Amarillo, posición superior derecha)
                DrawRectangle(186, 66, 14, 14, GBA_COLOR(24, 18, 2));
                DrawRectangleLines(186, 66, 14, 14, C_YELLOW);
                DrawRectangle(187, 67, 12, 1, (Color){ 255, 255, 255, 120 });
                DrawStringCustom("L", 191, 70, C_YELLOW, 1);
                int l_w = MeasureStringCustom(T(STR_CTRL_AIM), 1);
                DrawStringCustom(T(STR_CTRL_AIM), 193 - (l_w / 2), 82, C_YELLOW, 1);

                // Botón Disparo [A] (Rojo carmesí, botón principal a la derecha)
                DrawRectangle(185, 95, 16, 16, GBA_COLOR(26, 4, 4));
                DrawRectangleLines(185, 95, 16, 16, GBA_COLOR(31, 14, 14));
                DrawRectangle(186, 96, 14, 1, (Color){ 255, 255, 255, 140 });
                DrawStringCustom("A", 190, 100, WHITE, 1);
                int a_w = MeasureStringCustom(T(STR_CTRL_SHOOT), 1);
                DrawStringCustom(T(STR_CTRL_SHOOT), 193 - (a_w / 2), 113, GBA_COLOR(31, 14, 14), 1);
            } else {
                DrawMenuSpaceFramePC(C_CTRL_BG, T(STR_CTRL_TITLE), GBA_COLOR(0, 31, 10), GBA_COLOR(0, 10, 4), GBA_COLOR(0, 26, 10));
                DrawNebulaBackgroundPC(SCREEN_W, SCREEN_H, frame_count);
                for(int m = 0; m < MAX_MENU_STARS; m++) {
                    int my = (int)roundf((float)menu_stars[m].y + (float)menu_stars[m].vy * (1.0f - alpha_interp));
                    if (my > SCREEN_H - 4) my -= (SCREEN_H - 6);
                    DrawRectangle(menu_stars[m].x, my, 1, 1, menu_stars[m].color);
                }

                int c_box_w = 210;
                int c_box_x = (SCREEN_W - c_box_w) / 2;
                int c_box_y = 42;
                int c_box_h = 92;
                DrawBevelledBoxPC(c_box_x, c_box_y, c_box_w, c_box_h, GBA_COLOR(1, 4, 3), GBA_COLOR(0, 26, 12), true);
                DrawRectangle(c_box_x + 3, c_box_y + 3, 1, 1, C_CYAN);
                DrawRectangle(c_box_x + c_box_w - 4, c_box_y + 3, 1, 1, C_CYAN);
                DrawRectangle(c_box_x + 3, c_box_y + c_box_h - 4, 1, 1, C_CYAN);
                DrawRectangle(c_box_x + c_box_w - 4, c_box_y + c_box_h - 4, 1, 1, C_CYAN);

                const char* action_labels[8] = {
                    T(STR_CTRL_UP), T(STR_CTRL_DOWN), T(STR_CTRL_LEFT), T(STR_CTRL_RIGHT),
                    T(STR_CTRL_SHOOT), T(STR_CTRL_TURBO), T(STR_CTRL_AIM), T(STR_CTRL_DASH)
                };
                int action_pads[8] = {
                    g_pad.btn_up, g_pad.btn_down, g_pad.btn_left, g_pad.btn_right,
                    g_pad.btn_shoot, g_pad.btn_turbo, g_pad.btn_aim, g_pad.btn_dash
                };

                for (int act_idx = 0; act_idx < 8; act_idx++) {
                    int row_y = c_box_y + 4 + (act_idx * 11);
                    bool is_sel = (controls_selection == act_idx);

                    int row_w = 206;
                    int row_x = c_box_x + 2;
                    if (is_sel) {
                        DrawBevelledBoxPC(row_x, row_y - 2, row_w, 10, GBA_COLOR(2, 8, 16), C_CYAN, true);
                    }

                    DrawActionIcon16Bit(act_idx, c_box_x + 5, row_y - 1, is_sel);
                    DrawStringCustom(action_labels[act_idx], c_box_x + 17, row_y, is_sel ? C_YELLOW : WHITE, 1);
                    
                    const char* bind_name = GetGamepadButtonNameCustom(action_pads[act_idx]);
                    Color btn_col = is_sel ? C_YELLOW : GetGamepadButtonColorCustom(action_pads[act_idx]);
                    DrawStringCustom(bind_name, c_box_x + 135, row_y, btn_col, 1);
                    DrawGamepadIcon16Bit(c_box_x + 187, row_y - 2, is_sel);
                }

                if (rebinding_action != -1) {
                    int r_pop_w = 210;
                    int r_pop_x = (SCREEN_W - r_pop_w) / 2;
                    int r_pop_y = (SCREEN_H - 70) / 2;
                    DrawBevelledBoxPC(r_pop_x, r_pop_y, r_pop_w, 70, GBA_COLOR(2, 3, 6), C_YELLOW, true);
                    DrawRectangle(r_pop_x + 3, r_pop_y + 3, 1, 1, C_YELLOW);
                    DrawRectangle(r_pop_x + r_pop_w - 4, r_pop_y + 3, 1, 1, C_YELLOW);
                    DrawRectangle(r_pop_x + 3, r_pop_y + 66, 1, 1, C_YELLOW);
                    DrawRectangle(r_pop_x + r_pop_w - 4, r_pop_y + 66, 1, 1, C_YELLOW);

                    DrawCenteredStringCustom(T(STR_REBIND_TITLE), r_pop_y + 9, C_YELLOW, 1);
                    DrawCenteredStringCustom(action_labels[rebinding_action], r_pop_y + 23, C_CYAN, 1);
                    DrawCenteredStringCustom(T(STR_PRESS_KEY), r_pop_y + 37, WHITE, 1);
                    DrawCenteredStringCustom(T(STR_CANCEL_KEY), r_pop_y + 51, GBA_COLOR(18, 18, 18), 1);
                }

                if (duplicate_key_warning > 0) {
                    int d_pop_w = 180;
                    int d_pop_x = (SCREEN_W - d_pop_w) / 2;
                    int d_pop_y = (SCREEN_H - 64) / 2;
                    DrawBevelledBoxPC(d_pop_x, d_pop_y, d_pop_w, 64, GBA_COLOR(4, 2, 8), GBA_COLOR(31, 2, 2), true);
                    DrawCenteredStringCustom(T(STR_INVALID_ACTION), d_pop_y + 8, GBA_COLOR(31, 6, 6), 1);
                    DrawCenteredStringCustom(T(STR_DUPLICATE_KEY), d_pop_y + 24, C_YELLOW, 1);
                    DrawCenteredStringCustom(T(STR_USE_OTHER_KEY), d_pop_y + 40, WHITE, 1);
                }
            }
        }
        else if (state == 8) {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, BLACK);
            int stage = (60 - death_timer) / 10;
            if (stage > 5) stage = 5;
            int offset = stage;
            int size = 10 - (stage * 2);
            if (size > 0) {
                DrawRectangle(px + offset, py + offset, size, size, C_YELLOW);
                if (stage < 4) DrawRectangle(px + 4, py + 4, 2, 2, WHITE);
            }
        }
        else if (state == 9 || state == 1 || state == 3) {
            Color bg_color = (nuke_timer > 0 && (nuke_timer / 6) % 2 == 0) ? GBA_COLOR(31, 16, 0) : ((slowmo_timer > 0) ? GBA_COLOR(8, 8, 8) : BLACK);
            
            if (state == 9) {
                ClearBackground(BLACK);
            } else {
                DrawRectangle(0, 0, SCREEN_W, SCREEN_H, bg_color);
                for(int g = 0; g < MAX_GAME_STARS; g++) {
                    int gy = (int)roundf((float)game_stars[g].y - (float)game_stars[g].speed * (1.0f - alpha_interp));
                    if (gy < 18) gy += (SCREEN_H - 20);
                    DrawRectangle(game_stars[g].x, gy, 1, 1, game_stars[g].color);
                }
            }

            if (p_up.active) DrawPowerUpPC(&p_up);

            for(int ex_i = 0; ex_i < MAX_EXPLO; ex_i++) {
                if (explosions[ex_i].active) {
                    Color spark_color = (explosions[ex_i].timer % 4 < 2) ? GBA_COLOR(31, 31, 0) : GBA_COLOR(31, 15, 0);
                    DrawRectangle(explosions[ex_i].x + 2, explosions[ex_i].y, 2, 2, spark_color);
                    DrawRectangle(explosions[ex_i].x + 8, explosions[ex_i].y + 2, 2, 2, spark_color);
                    DrawRectangle(explosions[ex_i].x, explosions[ex_i].y + 6, 2, 2, spark_color);
                    DrawRectangle(explosions[ex_i].x + 6, explosions[ex_i].y + 8, 2, 2, spark_color);
                }
            }

            for(int b_idx = 0; b_idx < MAX_BULLETS; b_idx++) {
                if (bullets[b_idx].active) {
                    int bx = (int)roundf((float)bullets[b_idx].x - (float)bullets[b_idx].vx * (1.0f - alpha_interp));
                    int by = (int)roundf((float)bullets[b_idx].y - (float)bullets[b_idx].vy * (1.0f - alpha_interp));
                    int vx = bullets[b_idx].vx;
                    int vy = bullets[b_idx].vy;
                    
                    if (g_cheat_size_active == 1) {
                        // Balas diminutas
                        if (p_type == 5) {
                            DrawRectangle(bx, by, 3, 3, GBA_COLOR(31, 15, 0));
                            DrawRectangle(bx + 1, by + 1, 1, 1, C_YELLOW);
                        } else if (p_type == 9) {
                            DrawRectangle(bx, by, 2, 2, C_ICE);
                            DrawRectangle(bx, by, 1, 1, WHITE);
                        } else if (p_type == 12) {
                            DrawRectangle(bx, by, 2, 2, C_POISON_BULLET);
                            DrawRectangle(bx, by, 1, 1, GBA_COLOR(31, 20, 31));
                        } else if (p_type == 14) {
                            DrawRectangle(bx, by, 2, 2, C_NAVY_BLUE);
                            DrawRectangle(bx, by, 1, 1, WHITE);
                        } else {
                            DrawRectangle(bx, by, 2, 2, C_BULLET);
                            DrawRectangle(bx, by, 1, 1, WHITE);
                        }
                    } else if (g_cheat_size_active == 2) {
                        // Balas gigantes
                        if (p_type == 5) {
                            DrawRectangle(bx - 4, by - 4, 14, 14, GBA_COLOR(31, 15, 0));
                            DrawRectangle(bx - 2, by - 2, 10, 10, C_YELLOW);
                            DrawRectangle(bx, by, 6, 6, WHITE);
                        } else if (p_type == 9) {
                            DrawRectangle(bx - 3, by - 3, 10, 10, C_ICE);
                            DrawRectangle(bx - 1, by - 1, 6, 6, WHITE);
                        } else if (p_type == 12) {
                            DrawRectangle(bx - 3, by - 3, 10, 10, C_POISON_BULLET);
                            DrawRectangle(bx - 1, by - 1, 6, 6, GBA_COLOR(31, 20, 31));
                        } else if (p_type == 14) {
                            DrawRectangle(bx - 3, by - 3, 10, 10, C_NAVY_BLUE);
                            DrawRectangle(bx - 1, by - 1, 6, 6, WHITE);
                        } else {
                            DrawRectangle(bx - 3, by - 3, 10, 10, C_BULLET);
                            DrawRectangle(bx - 1, by - 1, 6, 6, WHITE);
                        }
                    } else {
                        if (p_type == 5) { 
                            DrawRectangle(bx, by, 6, 6, GBA_COLOR(31, 15, 0));
                            DrawRectangle(bx + 1, by + 1, 4, 4, C_YELLOW);
                        } else if (p_type == 9) { 
                            DrawRectangle(bx + 1, by, 2, 4, C_ICE);
                            DrawRectangle(bx, by + 1, 4, 2, C_ICE);
                            DrawRectangle(bx + 1, by + 1, 2, 2, WHITE);
                        } else if (p_type == 12) { 
                            DrawRectangle(bx, by, 4, 4, C_POISON_BULLET);
                            DrawRectangle(bx + 1, by + 1, 2, 2, GBA_COLOR(31, 20, 31));
                        } else if (p_type == 14) { 
                            DrawRectangle(bx + 1, by, 2, 4, C_NAVY_BLUE);
                            DrawRectangle(bx, by + 1, 4, 2, C_NAVY_BLUE);
                            DrawRectangle(bx + 1, by + 1, 2, 2, WHITE);
                        } else { 
                            if (vx != 0 && vy != 0) {
                                DrawRectangle(bx, by, 3, 3, C_BULLET);
                                DrawRectangle(bx + (vx > 0 ? 2 : 0), by + (vy > 0 ? 2 : 0), 1, 1, WHITE);
                            } else if (abs(vx) > abs(vy)) {
                                DrawRectangle(bx, by + 1, 4, 2, C_BULLET);
                                DrawRectangle(bx + (vx > 0 ? 3 : 0), by + 1, 1, 2, WHITE);
                            } else {
                                DrawRectangle(bx + 1, by, 2, 4, C_BULLET);
                                DrawRectangle(bx + 1, by + (vy > 0 ? 3 : 0), 2, 1, WHITE);
                            }
                        }
                    }
                }
            }

            for(int eb_idx = 0; eb_idx < MAX_E_BULLETS; eb_idx++) {
                if (e_bullets[eb_idx].active) {
                    int ebx = (int)roundf((float)e_bullets[eb_idx].x - (float)e_bullets[eb_idx].vx * (1.0f - alpha_interp));
                    int eby = (int)roundf((float)e_bullets[eb_idx].y - (float)e_bullets[eb_idx].vy * (1.0f - alpha_interp));
                    DrawRectangle(ebx, eby, 4, 4, C_E_BULLET);
                }
            }

            for(int en_idx = 0; en_idx < MAX_ENEMIES; en_idx++) {
                if (enemies[en_idx].active) {
                    float ex = prev_enemy_fx[en_idx] + (enemy_fx[en_idx] - prev_enemy_fx[en_idx]) * alpha_interp;
                    float ey = prev_enemy_fy[en_idx] + (enemy_fy[en_idx] - prev_enemy_fy[en_idx]) * alpha_interp;
                    if (fabsf(enemy_fx[en_idx] - prev_enemy_fx[en_idx]) > 8.0f || fabsf(enemy_fy[en_idx] - prev_enemy_fy[en_idx]) > 8.0f) {
                        ex = enemy_fx[en_idx];
                        ey = enemy_fy[en_idx];
                    }
                    Enemy draw_enemy = enemies[en_idx];
                    draw_enemy.x = (int)roundf(ex);
                    draw_enemy.y = (int)roundf(ey);
                    if (g_cheat_chonky_active) {
                        draw_enemy.x -= 5;
                        draw_enemy.y -= 5;
                        DrawEnemyScaledPC(&draw_enemy, frame_count, 2);
                    } else {
                        DrawEnemyPC(&draw_enemy, frame_count);
                    }
                }
            }

            for (int ft_idx = 0; ft_idx < MAX_TEXTS; ft_idx++) {
                if (ftexts[ft_idx].timer > 0) {
                    float fty = ftexts[ft_idx].y + 0.4f * (1.0f - alpha_interp);
                    DrawStringCustom(ftexts[ft_idx].text, (int)ftexts[ft_idx].x, (int)roundf(fty), ftexts[ft_idx].color, 1);
                }
            }

            Color p_color;
            if(p_type == 1) p_color = C_SHIELD;
            else if(p_type == 2) p_color = C_RAPID;
            else if(p_type == 3) p_color = C_GREEN;
            else if(p_type == 5) p_color = GBA_COLOR(31, 15, 0);
            else if(p_type == 6) p_color = C_BOSS;
            else if(p_type == 9) p_color = C_ICE;
            else if(p_type == 10) p_color = GBA_COLOR(10, 31, 10);
            else if(p_type == 12) p_color = GBA_COLOR(20, 0, 31);
            else if(p_type == 14) p_color = C_NAVY_BLUE;
            else p_color = C_PLAYER;

            if (g_cheat_rainbow_active) {
                p_color = ColorFromHSV((float)((frame_count * 6) % 360), 0.85f, 1.0f);
            }

            if((invincibility > 0 || p_type == 6) && (invincibility % 10 < 5 || p_type == 6)) {
                p_color = WHITE;
            }
            int draw_px = (int)roundf(prev_player_x + (player_x - prev_player_x) * alpha_interp);
            int draw_py = (int)roundf(prev_player_y + (player_y - prev_player_y) * alpha_interp);

            DrawGhostTrailsPC(ghost_trails, MAX_GHOST_TRAILS);
            DrawParticlesPC(particles, MAX_PARTICLES);
            if (g_cheat_size_active == 1) {
                DrawPlayerShipScaledPC(draw_px + 2, draw_py + 2, dir_x, dir_y, p_color, 0.5f);
            } else if (g_cheat_size_active == 2) {
                DrawPlayerShipScaledPC(draw_px - 5, draw_py - 5, dir_x, dir_y, p_color, 2.0f);
            } else {
                DrawPlayerShipPC(draw_px, draw_py, dir_x, dir_y, p_color);
            }
            int gauge_y = (g_cheat_size_active == 2) ? draw_py - 5 : draw_py;
            DrawDashGaugePC(draw_px, gauge_y, dash_cd, 60, g_dash_ready_flash);
            DrawDamageFlashPC(g_damage_flash_timer);

            DrawRectangle(0, 0, SCREEN_W, 16, GBA_COLOR(2, 4, 8));
            DrawRectangle(0, 15, SCREEN_W, 1, GBA_COLOR(0, 24, 31));

            if (player_hp == 1) {
                // Parpadeo lento del último corazón para generar tensión (cadencia de latido ~0.4s encendido / ~0.4s atenuado)
                if ((frame_count / 24) % 2 == 0) {
                    DrawHeartCustom(5, 3, C_HEART);
                } else {
                    DrawHeartCustom(5, 3, GBA_COLOR(6, 1, 1));
                }
            } else {
                for(int h = 0; h < player_hp; h++) DrawHeartCustom(5 + (h * 12), 3, C_HEART);
            }

            if (current_game_mode == MODE_RUSH) {
                int cycle_wave = ((wave - 1) % 10) + 1;
                int needed = (cycle_wave == 10) ? 10 : (20 + (rush_cycle * 10));
                DrawStringCustom(T(STR_WAVE), 68, 4, GBA_COLOR(14, 20, 26), 1);
                DrawScoreCustom(wave, 96, 3, C_YELLOW);

                char prog_buf[20];
                if (cycle_wave == 10) snprintf(prog_buf, sizeof(prog_buf), "B:%d/%d", rush_kills_in_wave, needed);
                else snprintf(prog_buf, sizeof(prog_buf), "%d/%d", rush_kills_in_wave, needed);
                DrawStringCustom(prog_buf, 118, 4, (cycle_wave == 10) ? C_BOSS : C_CYAN, 1);
            } else if (current_game_mode == MODE_TIME_ATTACK) {
                int t_sec = ta_time_left_frames / 60;
                int t_min = t_sec / 60;
                int t_s = t_sec % 60;
                char time_str[16];
                snprintf(time_str, sizeof(time_str), "%02d:%02d", t_min, t_s);

                Color time_col = C_CYAN;
                Color time_border = GBA_COLOR(0, 20, 26);
                Color time_bg = GBA_COLOR(0, 4, 8);
                if (t_sec <= 10) {
                    bool flash = ((frame_count / 15) % 2 == 0);
                    time_col = flash ? C_RED : WHITE;
                    time_border = flash ? GBA_COLOR(31, 6, 6) : WHITE;
                    time_bg = flash ? GBA_COLOR(8, 1, 1) : GBA_COLOR(4, 4, 6);
                } else if (t_sec <= 30) {
                    time_col = C_YELLOW;
                    time_border = GBA_COLOR(26, 22, 0);
                    time_bg = GBA_COLOR(6, 5, 0);
                }

                // Cápsula digital centrada para el temporizador de Contrarreloj (escala 2)
                int str_w = MeasureStringCustom(time_str, 2);
                int cap_w = str_w + 8;
                int cap_x = (SCREEN_W - cap_w) / 2;
                DrawBevelledBoxPC(cap_x, 1, cap_w, 14, time_bg, time_border, false);

                // Sombra y números grandes de la cuenta atrás
                DrawStringCustom(time_str, cap_x + 5, 4, GBA_COLOR(0, 0, 0), 2);
                DrawStringCustom(time_str, cap_x + 4, 3, time_col, 2);

                // Contador de bajas (K: XX) a la izquierda de la cápsula
                char k_buf[16];
                snprintf(k_buf, sizeof(k_buf), "K:%d", ta_run_kills);
                int k_w = MeasureStringCustom(k_buf, 1);
                int k_x = cap_x - k_w - 6;
                if (k_x < 65) k_x = 65;
                DrawStringCustom(k_buf, k_x, 5, GBA_COLOR(20, 28, 31), 1);
            } else if (current_game_mode == MODE_ASSAULT) {
                int sec_left = (assault_timer_frames + 59) / 60;
                char next_buf[24];
                snprintf(next_buf, sizeof(next_buf), "%s: %02d", T(STR_ASSAULT_NEXT), sec_left);

                Color time_col = (sec_left <= 3) ? (((frame_count / 15) % 2 == 0) ? C_RED : WHITE) : GBA_COLOR(31, 18, 0);
                DrawStringCustom(next_buf, 70, 4, time_col, 1);

                char k_buf[16];
                snprintf(k_buf, sizeof(k_buf), "K:%d", assault_run_kills);
                DrawStringCustom(k_buf, 126, 4, GBA_COLOR(20, 28, 31), 1);
            } else {
                DrawStringCustom(T(STR_WAVE), 68, 4, GBA_COLOR(14, 20, 26), 1);
                DrawScoreCustom(wave, 96, 3, C_YELLOW);
            }

            Color combo_col = (combo >= 5) ? C_YELLOW : C_CYAN;
            int combo_x = SCREEN_W - 92;
            DrawStringCustom("X", combo_x, 4, combo_col, 1);
            DrawScoreCustom(combo, combo_x + 8, 3, combo_col);
            DrawScoreCustom(score, SCREEN_W - 62, 3, WHITE);

            if (state == 9) {
                int ready_y = (SCREEN_H - 16) / 2;
                if (ready_timer > 60) {
                    if ((ready_timer / 15) % 2 == 0) DrawCenteredStringCustom(T(STR_READY), ready_y, C_YELLOW, 2);
                } else {
                    DrawCenteredStringCustom(T(STR_GO), ready_y, C_GREEN, 2);
                }
            }
            if (state == 1 && current_game_mode == MODE_RUSH && rush_wave_banner_timer > 0) {
                int banner_y = (SCREEN_H - 26) / 2;
                int bw = 172;
                int bh = 26;
                int bx = (SCREEN_W - bw) / 2;
                DrawBevelledBoxPC(bx, banner_y, bw, bh, GBA_COLOR(3, 3, 0), C_YELLOW, true);
                DrawRectangle(bx + 2, banner_y + 2, 1, 1, C_YELLOW);
                DrawRectangle(bx + bw - 3, banner_y + 2, 1, 1, C_YELLOW);
                DrawRectangle(bx + 2, banner_y + bh - 3, 1, 1, C_YELLOW);
                DrawRectangle(bx + bw - 3, banner_y + bh - 3, 1, 1, C_YELLOW);

                char wave_buf[32];
                int cycle_wave = ((wave - 1) % 10) + 1;
                if (cycle_wave == 10) {
                    snprintf(wave_buf, sizeof(wave_buf), "%s %d - BOSS!", T(STR_WAVE), wave);
                } else {
                    snprintf(wave_buf, sizeof(wave_buf), "%s %d", T(STR_WAVE), wave);
                }
                DrawCenteredStringCustom(wave_buf, banner_y + 5, C_YELLOW, 2);
            }
            if (state == 1 && current_game_mode == MODE_ASSAULT && assault_banner_timer > 0) {
                int bw = 170;
                int bh = 22;
                int bx = (SCREEN_W - bw) / 2;
                int by = 22;
                DrawBevelledBoxPC(bx, by, bw, bh, GBA_COLOR(2, 2, 4), assault_cur_pu_color, true);
                DrawRectangle(bx + 2, by + 2, 1, 1, assault_cur_pu_color);
                DrawRectangle(bx + bw - 3, by + 2, 1, 1, assault_cur_pu_color);
                DrawRectangle(bx + 2, by + bh - 3, 1, 1, assault_cur_pu_color);
                DrawRectangle(bx + bw - 3, by + bh - 3, 1, 1, assault_cur_pu_color);

                DrawCenteredStringCustom(T(STR_ASSAULT_CHANGE), by + 3, WHITE, 1);
                DrawCenteredStringCustom(T(assault_cur_pu_name_id), by + 12, assault_cur_pu_color, 1);
            }
            if (state == 3) {
                int pause_box_w = 170;
                int pause_box_h = 124;
                int pause_box_x = (SCREEN_W - pause_box_w) / 2;
                int pause_y = 26;
                DrawBevelledBoxPC(pause_box_x, pause_y, pause_box_w, pause_box_h, GBA_COLOR(1, 3, 7), C_CYAN, true);
                DrawRectangle(pause_box_x + 3, pause_y + 3, 1, 1, C_CYAN);
                DrawRectangle(pause_box_x + pause_box_w - 4, pause_y + 3, 1, 1, C_CYAN);
                DrawRectangle(pause_box_x + 3, pause_y + pause_box_h - 4, 1, 1, C_CYAN);
                DrawRectangle(pause_box_x + pause_box_w - 4, pause_y + pause_box_h - 4, 1, 1, C_CYAN);

                DrawHeaderCenteredStringCustom(T(STR_PAUSE), pause_y + 8, C_CYAN, GBA_COLOR(0, 10, 14), 2);
                DrawRectangle((SCREEN_W - 150) / 2, pause_y + 25, 150, 1, GBA_COLOR(0, 16, 20));

                const char* p_opts[3] = { T(STR_RESUME), T(STR_OPTIONS), T(STR_MAIN_MENU) };
                int p_icons[3] = { MENU_ICON_PLAY, MENU_ICON_OPTIONS, MENU_ICON_EXIT };
                Color p_colors[3] = { C_YELLOW, C_YELLOW, C_CYAN };
                int p_btn_w = 156;
                int p_btn_x = (SCREEN_W - p_btn_w) / 2;
                for(int p_idx = 0; p_idx < 3; p_idx++) {
                    int py_pos = pause_y + 38 + (p_idx * 28);
                    DrawSciFiButtonPC(p_btn_x, py_pos - 3, p_btn_w, 18, p_opts[p_idx], p_icons[p_idx], (p_idx == pause_selection), p_colors[p_idx], frame_count);
                }
            }
        }
        else if (state == 2) {
            int go_top = (SCREEN_H - 128) / 2;
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, C_RED);
            for(int b = 0; b < MAX_BLOOD_DROPS; b++) DrawRectangle(blood_drops[b].x, blood_drops[b].y, 1, 1, blood_drops[b].color);
            DrawHeaderCenteredStringCustom(T(STR_GAME_OVER), go_top + 2, WHITE, GBA_COLOR(4, 0, 0), 3);
            DrawSkullIconPC((SCREEN_W - 12) / 2, go_top + 30);
            DrawCenteredStringCustom(T(STR_POINTS), go_top + 48, GBA_COLOR(24, 18, 18), 1);
            DrawScoreCustom(score, (SCREEN_W - 20) / 2, go_top + 60, WHITE);
            if (current_game_mode == MODE_RUSH) {
                char rush_info[32];
                snprintf(rush_info, sizeof(rush_info), "%s %d", T(STR_WAVE), wave);
                DrawCenteredStringCustom(rush_info, go_top + 76, C_YELLOW, 1);
            } else if (current_game_mode == MODE_TIME_ATTACK) {
                int surv_sec = ta_time_survived_frames / 60;
                char ta_info[64];
                snprintf(ta_info, sizeof(ta_info), "%s: %02d:%02d", T(STR_TIME_LEFT), surv_sec / 60, surv_sec % 60);
                if (ta_time_up) {
                    DrawCenteredStringCustom(ta_info, go_top + 73, C_CYAN, 1);
                    DrawCenteredStringCustom(T(STR_TIME_UP), go_top + 84, C_YELLOW, 1);
                } else {
                    DrawCenteredStringCustom(ta_info, go_top + 76, C_CYAN, 1);
                }
            } else if (current_game_mode == MODE_ASSAULT) {
                int surv_sec = assault_time_survived_frames / 60;
                char assault_info[64];
                snprintf(assault_info, sizeof(assault_info), "%s: %02d:%02d", T(STR_TIME_LEFT), surv_sec / 60, surv_sec % 60);
                DrawCenteredStringCustom(assault_info, go_top + 76, GBA_COLOR(31, 20, 0), 1);
            } else {
                char classic_info[32];
                snprintf(classic_info, sizeof(classic_info), "%s %d", T(STR_WAVE), wave);
                DrawCenteredStringCustom(classic_info, go_top + 76, C_YELLOW, 1);
            }
            
            const char* go_opts[2] = { T(STR_RETRY), T(STR_MAIN_MENU) };
            int go_icons[2] = { MENU_ICON_PLAY, MENU_ICON_EXIT };
            Color go_colors[2] = { C_YELLOW, C_CYAN };
            int go_btn_w = 150;
            int go_btn_x = (SCREEN_W - go_btn_w) / 2;
            for(int go_idx = 0; go_idx < 2; go_idx++) {
                int opt_y = go_top + 96 + (go_idx * 18);
                DrawSciFiButtonPC(go_btn_x, opt_y - 3, go_btn_w, 16, go_opts[go_idx], go_icons[go_idx], (go_idx == gameover_selection), go_colors[go_idx], frame_count);
            }
        }

        DrawDeviceNotificationToast(g_device_toast_timer, g_toast_device);

        // --- POP-UP DE LOGRO DESBLOQUEADO (Abajo a la izquierda) ---
        if (g_achievement_toast_timer > 0) {
            int toast_w = 118;
            int toast_h = 24;
            int toast_x = 8;
            int toast_y = SCREEN_H - 32;

            DrawBevelledBoxPC(toast_x, toast_y, toast_w, toast_h, GBA_COLOR(1, 4, 8), C_GREEN, true);
            DrawRectangle(toast_x + 2, toast_y + 2, 1, 1, C_CYAN);
            DrawRectangle(toast_x + toast_w - 3, toast_y + 2, 1, 1, C_CYAN);
            DrawRectangle(toast_x + 2, toast_y + toast_h - 3, 1, 1, C_CYAN);
            DrawRectangle(toast_x + toast_w - 3, toast_y + toast_h - 3, 1, 1, C_CYAN);
            
            DrawBevelledBoxPC(toast_x + 4, toast_y + 4, 16, 16, GBA_COLOR(2, 6, 12), GBA_COLOR(6, 14, 20), true);
            int enemy_t = (g_latest_unlocked_ach >= 0 && g_latest_unlocked_ach < NUM_ACHIEVEMENTS) ? ach_enemy_types[g_latest_unlocked_ach] : 0;
            Enemy t_enemy = { .x = toast_x + 6, .y = toast_y + 6, .type = enemy_t, .active = 1, .hp = 1 };
            DrawEnemyPC(&t_enemy, frame_count);

            DrawStringCustom(T(STR_NEW_ACHIEVEMENT), toast_x + 24, toast_y + 5, C_YELLOW, 1);
            DrawStringCustom(T(GetAchTitleId(g_latest_unlocked_ach)), toast_x + 24, toast_y + 14, WHITE, 1);
        }

        EndTextureMode();

        // --- RENDERIZADO AL MONITOR CON SCREEN SHAKE Y SHADER CRT ---
        BeginDrawing();
        ClearBackground(BLACK);

        if (g_config.crt_filter > 0 && crtShader.id > 0) {
            SetShaderValue(crtShader, filterTypeLoc, &g_config.crt_filter, SHADER_UNIFORM_INT);
            float rsize[2] = { (float)SCREEN_W, (float)SCREEN_H };
            SetShaderValue(crtShader, renderSizeLoc, rsize, SHADER_UNIFORM_VEC2);
            BeginShaderMode(crtShader);
        }

        float final_offset_x = offset_x;
        float final_offset_y = offset_y;
        if (screen_shake_timer > 0 && (state == 1 || state == 9)) {
            final_offset_x += (float)((rand() % 5) - 2) * scale_val * 0.75f;
            final_offset_y += (float)((rand() % 5) - 2) * scale_val * 0.75f;
        }

        float t_glitch = (state == 0 || state == 13) ? GetTitleBgmGlitchIntensity() : 0.0f;
        float base_jitter_x = 0.0f;
        float base_jitter_y = 0.0f;
        if (t_glitch > 0.45f && (rand() % 100 < (int)(t_glitch * 50))) {
            base_jitter_x = (float)((rand() % 5) - 2) * scale_val * t_glitch;
            base_jitter_y = (float)((rand() % 3) - 1) * scale_val * t_glitch;
        }

        float roll_angle = 0.0f;
        if (g_barrel_roll_timer > 0.0f) {
            g_barrel_roll_timer -= frame_dt;
            if (g_barrel_roll_timer < 0.0f) g_barrel_roll_timer = 0.0f;
            float p = 1.0f - (g_barrel_roll_timer / 1.0f);
            float ease = 0.5f - 0.5f * cosf(p * 3.14159265f);
            roll_angle = ease * 360.0f;
        }

        float src_w = (float)target.texture.width;
        if (g_cheat_flip_active && (state == 1 || state == 9 || state == 3 || state == 8)) {
            src_w = -src_w;
        }

        if (roll_angle > 0.0f && roll_angle < 360.0f) {
            DrawTexturePro(
                target.texture,
                (Rectangle){ 0.0f, 0.0f, src_w, (float)-target.texture.height },
                (Rectangle){ final_offset_x + base_jitter_x + render_w * 0.5f, final_offset_y + base_jitter_y + render_h * 0.5f, render_w, render_h },
                (Vector2){ render_w * 0.5f, render_h * 0.5f },
                roll_angle,
                WHITE
            );
        } else {
            DrawTexturePro(
                target.texture,
                (Rectangle){ 0.0f, 0.0f, src_w, (float)-target.texture.height },
                (Rectangle){ final_offset_x + base_jitter_x, final_offset_y + base_jitter_y, render_w, render_h },
                (Vector2){ 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        }

        if (t_glitch > 0.05f) {
            int num_slices = (int)(t_glitch * 9.0f) + 2;
            for (int sl = 0; sl < num_slices; sl++) {
                int slice_y = rand() % (SCREEN_H - 12);
                int slice_h = (rand() % 10) + 3;
                float shift_x = (float)((rand() % 17) - 8) * t_glitch * scale_val;
                float slice_dest_y = final_offset_y + ((float)slice_y / (float)SCREEN_H) * render_h;
                float slice_dest_h = ((float)slice_h / (float)SCREEN_H) * render_h;
                float src_slice_y = ((float)slice_y / (float)SCREEN_H) * (float)target.texture.height;
                float src_slice_h = ((float)slice_h / (float)SCREEN_H) * (float)target.texture.height;

                if (t_glitch > 0.25f) {
                    float ghost_offset = (3.0f * t_glitch * scale_val);
                    // Split cromatico Rojo
                    DrawTexturePro(
                        target.texture,
                        (Rectangle){ 0.0f, src_slice_y, src_w, -src_slice_h },
                        (Rectangle){ final_offset_x + shift_x - ghost_offset, slice_dest_y, render_w, slice_dest_h },
                        (Vector2){ 0.0f, 0.0f },
                        0.0f,
                        (Color){ 255, 60, 60, (unsigned char)(160 * t_glitch) }
                    );
                    // Split cromatico Cian
                    DrawTexturePro(
                        target.texture,
                        (Rectangle){ 0.0f, src_slice_y, src_w, -src_slice_h },
                        (Rectangle){ final_offset_x + shift_x + ghost_offset, slice_dest_y, render_w, slice_dest_h },
                        (Vector2){ 0.0f, 0.0f },
                        0.0f,
                        (Color){ 60, 255, 255, (unsigned char)(160 * t_glitch) }
                    );
                }

                // Tira desplazada principal
                DrawTexturePro(
                    target.texture,
                    (Rectangle){ 0.0f, src_slice_y, src_w, -src_slice_h },
                    (Rectangle){ final_offset_x + shift_x, slice_dest_y, render_w, slice_dest_h },
                    (Vector2){ 0.0f, 0.0f },
                    0.0f,
                    WHITE
                );
            }

            // Barras de ruido digital y cortes de cinta VHS
            if (t_glitch > 0.2f) {
                int num_bars = (int)(t_glitch * 5.0f) + 1;
                for (int b = 0; b < num_bars; b++) {
                    int bar_y = (int)final_offset_y + (rand() % (int)render_h);
                    int bar_h = (rand() % (int)(2 * scale_y + 1)) + 1;
                    int bar_w = (rand() % (int)(render_w * 0.7f)) + (int)(render_w * 0.2f);
                    int bar_x = (int)final_offset_x + (rand() % (int)(render_w - bar_w));
                    Color b_col = (rand() % 2 == 0) ? (Color){ 0, 255, 255, (unsigned char)(130 * t_glitch) } : (Color){ 255, 255, 255, (unsigned char)(150 * t_glitch) };
                    DrawRectangle(bar_x, bar_y, bar_w, bar_h, b_col);
                }
            }

            // Destello de resincronizacion de tubo CRT al recomenzar la pista
            float snap = GetTitleBgmSnap();
            if (snap > 0.05f) {
                int flash_alpha = (int)(snap * 220.0f);
                if (flash_alpha > 255) flash_alpha = 255;
                int center_y = (int)(final_offset_y + (render_h * 0.5f));
                int beam_h = (int)(scale_y * (2.0f + (1.0f - snap) * 6.0f));
                DrawRectangle((int)final_offset_x, center_y - (beam_h / 2), (int)render_w, beam_h, (Color){ 255, 255, 255, (unsigned char)flash_alpha });
            }
        }

        if (g_config.crt_filter > 0 && crtShader.id > 0) EndShaderMode();

        // Mascaras de recorte para barras negras (evita que el glitch o shake sangre a los bordes)
        if (offset_x > 0.0f) {
            DrawRectangle(0, 0, (int)offset_x, (int)screen_render_h, BLACK);
            DrawRectangle((int)(offset_x + render_w), 0, (int)ceilf(offset_x) + 2, (int)screen_render_h, BLACK);
        }
        if (offset_y > 0.0f) {
            DrawRectangle(0, 0, (int)screen_render_w, (int)offset_y, BLACK);
            DrawRectangle(0, (int)(offset_y + render_h), (int)screen_render_w, (int)ceilf(offset_y) + 2, BLACK);
        }

#if !defined(PLATFORM_ANDROID) && !defined(PLATFORM_IOS)
        if (m_pos.x >= offset_x && m_pos.x <= offset_x + render_w &&
            m_pos.y >= offset_y && m_pos.y <= offset_y + render_h &&
            g_last_input_device == INPUT_KEYBOARD) {
            HideCursor();
            DrawSpaceShipCursor16Bit((int)m_pos.x, (int)m_pos.y, mouse_down);
        } else {
            ShowCursor();
        }
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
        bool show_touch_hud = (state == 1 || state == 9 || state == 8) && (g_last_input_device != INPUT_GAMEPAD);
        MobileInput_Draw(show_touch_hud);
#endif
        EndDrawing();
}

static void GameShutdown(void) {
    saveAchievementsPC();
    MobileInput_Shutdown();
    UnloadShader(crtShader);
    UnloadGameAudio();
    CloseAudioDevice();

    if (logoTexture.id > 0) UnloadTexture(logoTexture);
    UnloadRenderTexture(target);
    CloseWindow();
}

#if defined(PLATFORM_IOS)
void ios_ready(void)
{
    GameInit();
}

void ios_update(bool resized)
{
    (void)resized;
    GameUpdate();
}

void ios_destroy(void)
{
    GameShutdown();
}
#else
int main(void)
{
    GameInit();
    while (!WindowShouldClose() && !request_exit) {
        GameUpdate();
    }
    GameShutdown();
    return 0;
}
#endif
