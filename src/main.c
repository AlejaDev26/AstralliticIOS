#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "types.h"
#include "audio.h"
#include "graphics.h"
#include "mobile_input.h"
#include "platform_paths.h"

InputDeviceType g_last_input_device = INPUT_KEYBOARD;
int g_device_toast_timer = 0;
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
#define ACH_TIEMPO_MUERTO 17
#define ACH_EMPODERATE 18
#define ACH_NO_TOCARME 19
#define ACH_CAZADOR_JEFES 20
#define ACH_TELETRANSP 21
#define ACH_ULTIMO_ALIENTO 22
#define ACH_FULL_ARSENAL 23
#define ACH_MODO_DIOS 24
#define ACH_DEMASIADO_RAPIDO 25
#define ACH_ESQUIVE_PERFECTO 26
#define ACH_LIMPIEZA_TOTAL 27
#define ACH_VOY_CIEGO 28
#define ACH_LEYENDA 29

#define ACHIEVEMENT_QUEUE_SIZE 30
#define ARSENAL_NORMAL_MASK 1
#define ARSENAL_RAPID_MASK 2
#define ARSENAL_TRIPLE_MASK 4
#define ARSENAL_HEAVY_MASK 8
#define ARSENAL_FREEZE_MASK 16
#define ARSENAL_POISON_MASK 32
#define ARSENAL_ALL_MASK 63

bool g_achievements_unlocked[30] = { false };
int g_achievement_progress[30] = { 0 };
int g_enemies_killed_per_diff[4] = { 0, 0, 0, 0 };
int g_achievement_queue[ACHIEVEMENT_QUEUE_SIZE] = { 0 };
int g_achievement_queue_count = 0;
int selected_ach_index = 0;
bool show_ach_details = false;
int g_achievement_toast_timer = 0;
int g_latest_unlocked_ach = 0;
bool player_took_hit_this_wave = false;

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
    .res_index = 0,
    .screen_mode = 0,
    .language = 0,
    .vol_bgm = 7,
    .vol_sfx = 8,
    .crt_filter = 0
};

const int g_resolutions[4][2] = {
    { 960, 640 },
    { 1200, 800 },
    { 1440, 960 },
    { 1920, 1080 }
};

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
        [STR_TOUCH_CONNECTED] = "TACTIL CONECTADO",
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
        [STR_ACH_14_TITLE] = "INTACTO", [STR_ACH_14_DESC] = "COMPLETA UNA OLEADA SIN DANIO",
        [STR_ACH_15_TITLE] = "!BOOM!", [STR_ACH_15_DESC] = "ELIMINA 5 CON UNA NUKE",
        [STR_ACH_16_TITLE] = "FRIO, FRIO", [STR_ACH_16_DESC] = "CONGELA 10 ENEMIGOS",
        [STR_ACH_17_TITLE] = "TOXICIDAD", [STR_ACH_17_DESC] = "ENVENENA 10 ENEMIGOS",
        [STR_ACH_18_TITLE] = "TIEMPO MUERTO", [STR_ACH_18_DESC] = "ELIMINA 5 ENEMIGOS EN SLOW-MO",
        [STR_ACH_19_TITLE] = "EMPODERATE", [STR_ACH_19_DESC] = "COGE UN POWER-UP POR PRIMERA VEZ",
        [STR_ACH_20_TITLE] = "NO TOCARME", [STR_ACH_20_DESC] = "30S EN INFIERNO SIN DANIO",
        [STR_ACH_21_TITLE] = "CAZADOR JEFES", [STR_ACH_21_DESC] = "DESTRUYE 5 JEFES",
        [STR_ACH_22_TITLE] = "TELETRANSP.", [STR_ACH_22_DESC] = "DESTRUYE 10 TELEPORTS",
        [STR_ACH_23_TITLE] = "ULTIMO ALIENTO", [STR_ACH_23_DESC] = "1000 PTOS CON 1 CORAZON",
        [STR_ACH_24_TITLE] = "FULL ARSENAL", [STR_ACH_24_DESC] = "USA TODOS LOS DISPAROS",
        [STR_ACH_25_TITLE] = "MODO DIOS", [STR_ACH_25_DESC] = "COMBO X10 CON POWER-UP",
        [STR_ACH_26_TITLE] = "DEMASIADO RAP.", [STR_ACH_26_DESC] = "10 KILLS EN RAPID FIRE",
        [STR_ACH_27_TITLE] = "ESQUIVE PERFE.", [STR_ACH_27_DESC] = "EVITA 10 PROYECTILES CON DASH",
        [STR_ACH_28_TITLE] = "LIMPIEZA TOT.", [STR_ACH_28_DESC] = "NUKE CON 10 O MAS ENEMIGOS",
        [STR_ACH_29_TITLE] = "VOY CIEGO", [STR_ACH_29_DESC] = "EMBORRACHATE TOMANDO LA BOTELLA",
        [STR_ACH_30_TITLE] = "LEYENDA", [STR_ACH_30_DESC] = "DESBLOQUEA TODOS LOS DEMAS LOGROS",
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
        [STR_POINTS] = "SCORE", [STR_RETRY] = "RETRY", [STR_QUIT_PROMPT] = "QUIT GAME?",
        [STR_QUIT_SUB] = "DO YOU WANT TO EXIT?", [STR_YES] = "YES", [STR_NO] = "NO", [STR_CTRL_TITLE] = "CONTROLS",
        [STR_CTRL_UP] = "UP", [STR_CTRL_DOWN] = "DOWN", [STR_CTRL_LEFT] = "LEFT", [STR_CTRL_RIGHT] = "RIGHT",
        [STR_CTRL_SHOOT] = "SHOOT", [STR_CTRL_TURBO] = "TURBO", [STR_CTRL_AIM] = "AIM LOCK",
        [STR_CTRL_DASH] = "DASH", [STR_REBIND_TITLE] = "REBIND INPUT", [STR_PRESS_KEY] = "PRESS KEY OR BUTTON...",
        [STR_CANCEL_KEY] = "BKSP / ESC CANCEL", [STR_INVALID_ACTION] = "INVALID ACTION", [STR_DUPLICATE_KEY] = "INPUT ALREADY ASSIGNED",
        [STR_USE_OTHER_KEY] = "USE A DIFFERENT INPUT", [STR_PRESS_BACK] = "PRESS %s TO GO BACK", [STR_PRESENTS] = "PRESENTS...",
        [STR_READY] = "READY!", [STR_GO] = "GO!", [STR_WAVE] = "WAVE",
        [STR_GAMEPAD_CONNECTED] = "GAMEPAD DETECTED", [STR_KEYBOARD_CONNECTED] = "KEYBOARD DETECTED",
        [STR_TOUCH_CONNECTED] = "TOUCH CONNECTED",
        [STR_FREEZE] = "FREEZE!", [STR_POISON] = "POISON!",
        [STR_LOGROS] = "ACHIEVEMENTS",
        [STR_ACH_1_TITLE] = "EASY ROOKIE", [STR_ACH_1_DESC] = "DEFEAT 10 ENEMIES ON EASY",
        [STR_ACH_2_TITLE] = "NORMAL ROOKIE", [STR_ACH_2_DESC] = "DEFEAT 10 ENEMIES ON NORMAL",
        [STR_ACH_3_TITLE] = "HARD ROOKIE", [STR_ACH_3_DESC] = "DEFEAT 10 ENEMIES ON HARD",
        [STR_ACH_4_TITLE] = "HELL ROOKIE", [STR_ACH_4_DESC] = "DEFEAT 10 ENEMIES ON HELL",
        [STR_ACH_5_TITLE] = "EASY VETERAN", [STR_ACH_5_DESC] = "DEFEAT 100 ENEMIES ON EASY",
        [STR_ACH_6_TITLE] = "NORMAL VETERAN", [STR_ACH_6_DESC] = "DEFEAT 100 ENEMIES ON NORMAL",
        [STR_ACH_7_TITLE] = "HARD VETERAN", [STR_ACH_7_DESC] = "DEFEAT 100 ENEMIES ON HARD",
        [STR_ACH_8_TITLE] = "HELL VETERAN", [STR_ACH_8_DESC] = "DEFEAT 100 ENEMIES ON HELL",
        [STR_ACH_9_TITLE] = "EASY MASTER", [STR_ACH_9_DESC] = "DEFEAT 500 ENEMIES ON EASY",
        [STR_ACH_10_TITLE] = "NORMAL MASTER", [STR_ACH_10_DESC] = "DEFEAT 500 ENEMIES ON NORMAL",
        [STR_ACH_11_TITLE] = "HARD MASTER", [STR_ACH_11_DESC] = "DEFEAT 500 ENEMIES ON HARD",
        [STR_ACH_12_TITLE] = "HELL MASTER", [STR_ACH_12_DESC] = "DEFEAT 500 ENEMIES ON HELL",
        [STR_ACH_13_TITLE] = "HEAVY IMPACT", [STR_ACH_13_DESC] = "DESTROY 25 WITH HEAVY SHOT",
        [STR_ACH_14_TITLE] = "UNTOUCHED", [STR_ACH_14_DESC] = "FINISH A WAVE WITHOUT DAMAGE",
        [STR_ACH_15_TITLE] = "!BOOM!", [STR_ACH_15_DESC] = "DESTROY 5 WITH A SINGLE NUKE",
        [STR_ACH_16_TITLE] = "DEEP FREEZE", [STR_ACH_16_DESC] = "FREEZE 10 ENEMIES",
        [STR_ACH_17_TITLE] = "TOXICITY", [STR_ACH_17_DESC] = "POISON 10 ENEMIES",
        [STR_ACH_18_TITLE] = "BULLET TIME", [STR_ACH_18_DESC] = "DEFEAT 5 ENEMIES IN SLOW-MO",
        [STR_ACH_19_TITLE] = "POWER UP", [STR_ACH_19_DESC] = "COLLECT YOUR FIRST POWER-UP",
        [STR_ACH_20_TITLE] = "GHOST", [STR_ACH_20_DESC] = "SURVIVE 30S IN HELL NO DAMAGE",
        [STR_ACH_21_TITLE] = "BOSS HUNTER", [STR_ACH_21_DESC] = "DESTROY 5 BOSSES",
        [STR_ACH_22_TITLE] = "WARP BREAKER", [STR_ACH_22_DESC] = "DESTROY 10 TELEPORTERS",
        [STR_ACH_23_TITLE] = "LAST STAND", [STR_ACH_23_DESC] = "SCORE 1000 PTS WITH 1 HP",
        [STR_ACH_24_TITLE] = "FULL ARSENAL", [STR_ACH_24_DESC] = "USE EVERY WEAPON TYPE",
        [STR_ACH_25_TITLE] = "GOD MODE", [STR_ACH_25_DESC] = "REACH X10 COMBO WITH POWER-UP",
        [STR_ACH_26_TITLE] = "RAPID FURY", [STR_ACH_26_DESC] = "10 KILLS WITH RAPID FIRE",
        [STR_ACH_27_TITLE] = "PERFECT DODGE", [STR_ACH_27_DESC] = "EVADE 10 BULLETS USING DASH",
        [STR_ACH_28_TITLE] = "TOTAL PURGE", [STR_ACH_28_DESC] = "NUKE WITH 10+ ENEMIES ON SCREEN",
        [STR_ACH_29_TITLE] = "BLIND DRUNK", [STR_ACH_29_DESC] = "GET WASTED DRINKING THE BOTTLE",
        [STR_ACH_30_TITLE] = "LEGEND", [STR_ACH_30_DESC] = "UNLOCK ALL OTHER ACHIEVEMENTS",
        [STR_TOUCH_BACK] = "< TAP HERE TO GO BACK >",
        [STR_ACH_HINT] = "TAP AN ACHIEVEMENT FOR DETAILS",
        [STR_ACH_STATUS_UNLOCKED] = "STATUS: UNLOCKED (OK)",
        [STR_ACH_PROGRESS_LBL] = "PROGRESS",
        [STR_CTRL_MOVE] = "MOVE SHIP",
        [STR_TOUCH_INFO_TITLE] = "TOUCH CONTROLS"
    },
    [2] = { // FRANÇAIS
        [STR_PLAY] = "JOUER", [STR_OPTIONS] = "OPTIONS", [STR_RECORDS] = "RECORDS", [STR_VOL_BGM] = "VOL. MUSIQUE",
        [STR_VOL_SFX] = "VOL. EFFETS", [STR_FILTER] = "FILTRE", [STR_CONTROLS] = "COMMANDES", [STR_RESOLUTION] = "RES",
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
        [STR_TOUCH_CONNECTED] = "TACTILE CONNECTE",
        [STR_FREEZE] = "GEL!", [STR_POISON] = "POISON!",
        [STR_LOGROS] = "SUCCES",
        [STR_ACH_1_TITLE] = "RECRUE FACILE", [STR_ACH_1_DESC] = "ELIMINE 10 ENNEMIS EN FACILE",
        [STR_ACH_2_TITLE] = "RECRUE NORMALE", [STR_ACH_2_DESC] = "ELIMINE 10 ENNEMIS EN NORMAL",
        [STR_ACH_3_TITLE] = "RECRUE DIFFICILE", [STR_ACH_3_DESC] = "ELIMINE 10 ENNEMIS EN DIFFICILE",
        [STR_ACH_4_TITLE] = "RECRUE ENFER", [STR_ACH_4_DESC] = "ELIMINE 10 ENNEMIS EN ENFER",
        [STR_ACH_5_TITLE] = "VETERAN FACILE", [STR_ACH_5_DESC] = "ELIMINE 100 ENNEMIS EN FACILE",
        [STR_ACH_6_TITLE] = "VETERAN NORMAL", [STR_ACH_6_DESC] = "ELIMINE 100 ENNEMIS EN NORMAL",
        [STR_ACH_7_TITLE] = "VETERAN DIFFICILE", [STR_ACH_7_DESC] = "ELIMINE 100 ENNEMIS EN DIFFICILE",
        [STR_ACH_8_TITLE] = "VETERAN ENFER", [STR_ACH_8_DESC] = "ELIMINE 100 ENNEMIS EN ENFER",
        [STR_ACH_9_TITLE] = "MAITRE FACILE", [STR_ACH_9_DESC] = "ELIMINE 500 ENNEMIS EN FACILE",
        [STR_ACH_10_TITLE] = "MAITRE NORMAL", [STR_ACH_10_DESC] = "ELIMINE 500 ENNEMIS EN NORMAL",
        [STR_ACH_11_TITLE] = "MAITRE DIFFICILE", [STR_ACH_11_DESC] = "ELIMINE 500 ENNEMIS EN DIFFICILE",
        [STR_ACH_12_TITLE] = "MAITRE ENFER", [STR_ACH_12_DESC] = "ELIMINE 500 ENNEMIS EN ENFER",
        [STR_ACH_13_TITLE] = "IMPACT LOURD", [STR_ACH_13_DESC] = "ELIMINE 25 AU TIR LOURD",
        [STR_ACH_14_TITLE] = "INTACT", [STR_ACH_14_DESC] = "FINIS UNE VAGUE SANS DEGATS",
        [STR_ACH_15_TITLE] = "!BOOM!", [STR_ACH_15_DESC] = "ELIMINE 5 AVEC UNE NUKE",
        [STR_ACH_16_TITLE] = "GRAND FROID", [STR_ACH_16_DESC] = "GELE 10 ENNEMIS",
        [STR_ACH_17_TITLE] = "TOXICITE", [STR_ACH_17_DESC] = "EMPOISONNE 10 ENNEMIS",
        [STR_ACH_18_TITLE] = "TEMPS MORT", [STR_ACH_18_DESC] = "ELIMINE 5 EN SLOW-MO",
        [STR_ACH_19_TITLE] = "SURPUISSANCE", [STR_ACH_19_DESC] = "RAMASSE UN POWER-UP",
        [STR_ACH_20_TITLE] = "INTOUCHABLE", [STR_ACH_20_DESC] = "SURVIS 30S EN ENFER SANS DEGATS",
        [STR_ACH_21_TITLE] = "CHASSEUR BOSS", [STR_ACH_21_DESC] = "DETRUIS 5 BOSS",
        [STR_ACH_22_TITLE] = "RUPTURE WARP", [STR_ACH_22_DESC] = "DETRUIS 10 TELEPORTEURS",
        [STR_ACH_23_TITLE] = "DERNIER SOUFFLE", [STR_ACH_23_DESC] = "1000 PTS AVEC 1 PV",
        [STR_ACH_24_TITLE] = "ARSENAL TOTAL", [STR_ACH_24_DESC] = "UTILISE TOUS LES TIRS",
        [STR_ACH_25_TITLE] = "MODE DIEU", [STR_ACH_25_DESC] = "COMBO X10 AVEC POWER-UP",
        [STR_ACH_26_TITLE] = "FUREUR RAPIDE", [STR_ACH_26_DESC] = "10 KILLS EN RAPID FIRE",
        [STR_ACH_27_TITLE] = "ESQUIVE PARFAITE", [STR_ACH_27_DESC] = "EVITE 10 TIRS AVEC DASH",
        [STR_ACH_28_TITLE] = "PURGE TOTALE", [STR_ACH_28_DESC] = "NUKE AVEC 10+ ENNEMIS",
        [STR_ACH_29_TITLE] = "BOURRE", [STR_ACH_29_DESC] = "BOIS LA BOUTEILLE VERTE",
        [STR_ACH_30_TITLE] = "LEGENDE", [STR_ACH_30_DESC] = "DEVERROUILLE TOUS LES SUCCES",
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
        [STR_TOUCH_CONNECTED] = "TOUCH CONNESSO",
        [STR_FREEZE] = "GELATO!", [STR_POISON] = "VELENO!",
        [STR_LOGROS] = "SUCCESSI",
        [STR_ACH_1_TITLE] = "RECLUTA FACILE", [STR_ACH_1_DESC] = "ELIMINA 10 NEMICI A FACILE",
        [STR_ACH_2_TITLE] = "RECLUTA NORMALE", [STR_ACH_2_DESC] = "ELIMINA 10 NEMICI A NORMALE",
        [STR_ACH_3_TITLE] = "RECLUTA DIFFICILE", [STR_ACH_3_DESC] = "ELIMINA 10 NEMICI A DIFFICILE",
        [STR_ACH_4_TITLE] = "RECLUTA INFERNO", [STR_ACH_4_DESC] = "ELIMINA 10 NEMICI A INFERNO",
        [STR_ACH_5_TITLE] = "VETERANO FACILE", [STR_ACH_5_DESC] = "ELIMINA 100 NEMICI A FACILE",
        [STR_ACH_6_TITLE] = "VETERANO NORMALE", [STR_ACH_6_DESC] = "ELIMINA 100 NEMICI A NORMALE",
        [STR_ACH_7_TITLE] = "VETERANO DIFFICILE", [STR_ACH_7_DESC] = "ELIMINA 100 NEMICI A DIFFICILE",
        [STR_ACH_8_TITLE] = "VETERANO INFERNO", [STR_ACH_8_DESC] = "ELIMINA 100 NEMICI A INFERNO",
        [STR_ACH_9_TITLE] = "MAESTRO FACILE", [STR_ACH_9_DESC] = "ELIMINA 500 NEMICI A FACILE",
        [STR_ACH_10_TITLE] = "MAESTRO NORMALE", [STR_ACH_10_DESC] = "ELIMINA 500 NEMICI A NORMALE",
        [STR_ACH_11_TITLE] = "MAESTRO DIFFICILE", [STR_ACH_11_DESC] = "ELIMINA 500 NEMICI A DIFFICILE",
        [STR_ACH_12_TITLE] = "MAESTRO INFERNO", [STR_ACH_12_DESC] = "ELIMINA 500 NEMICI A INFERNO",
        [STR_ACH_13_TITLE] = "COLPO PESANTE", [STR_ACH_13_DESC] = "ELIMINA 25 CON TIRO PESANTE",
        [STR_ACH_14_TITLE] = "INTATTO", [STR_ACH_14_DESC] = "COMPLETA ONDATA SENZA DANNI",
        [STR_ACH_15_TITLE] = "!BOOM!", [STR_ACH_15_DESC] = "ELIMINA 5 CON UNA NUKE",
        [STR_ACH_16_TITLE] = "GELIDO", [STR_ACH_16_DESC] = "CONGELA 10 NEMICI",
        [STR_ACH_17_TITLE] = "TOSSICITA", [STR_ACH_17_DESC] = "AVVELENA 10 NEMICI",
        [STR_ACH_18_TITLE] = "TEMPO LENTO", [STR_ACH_18_DESC] = "ELIMINA 5 IN SLOW-MO",
        [STR_ACH_19_TITLE] = "POTENZIATO", [STR_ACH_19_DESC] = "PRENDI IL PRIMO POWER-UP",
        [STR_ACH_20_TITLE] = "INTOCCABILE", [STR_ACH_20_DESC] = "30S A INFERNO SENZA DANNI",
        [STR_ACH_21_TITLE] = "CACCIA BOSS", [STR_ACH_21_DESC] = "DISTRUGGI 5 BOSS",
        [STR_ACH_22_TITLE] = "SPEZZA WARP", [STR_ACH_22_DESC] = "DISTRUGGI 10 TELETRASPORTI",
        [STR_ACH_23_TITLE] = "ULTIMO RESPIRO", [STR_ACH_23_DESC] = "1000 PUNTI CON 1 CUORE",
        [STR_ACH_24_TITLE] = "ARSENALE", [STR_ACH_24_DESC] = "USA TUTTI I TIPI DI SPARO",
        [STR_ACH_25_TITLE] = "MODO DIO", [STR_ACH_25_DESC] = "COMBO X10 CON POWER-UP",
        [STR_ACH_26_TITLE] = "FURIA RAPIDA", [STR_ACH_26_DESC] = "10 KILLS CON TIRO RAPIDO",
        [STR_ACH_27_TITLE] = "SCHIVATA", [STR_ACH_27_DESC] = "EVITA 10 COLPI CON DASH",
        [STR_ACH_28_TITLE] = "PULIZIA TOTALE", [STR_ACH_28_DESC] = "NUKE CON 10+ NEMICI IN GIOCO",
        [STR_ACH_29_TITLE] = "UBRIACO", [STR_ACH_29_DESC] = "UBRIACATI CON LA BOTTIGLIA",
        [STR_ACH_30_TITLE] = "LEGGENDA", [STR_ACH_30_DESC] = "SBLOCCA TUTTI GLI ALTRI SUCCESSI",
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
        [STR_DELETE_RECORDS] = "REKORDE LOESCHEN", [STR_CONFIRM_DELETE_TITLE] = "REKORDE LOESCHEN?", [STR_CONFIRM_DELETE_SUB] = "UNWIDERRUFLICH",
        [STR_CANCEL] = "NEIN, ABBRUCH", [STR_CONFIRM_DELETE_BTN] = "JA, LOESCHEN", [STR_DIFF_TITLE] = "SCHWIERIGKEIT",
        [STR_DIFF_EASY] = "LEICHT", [STR_DIFF_NORMAL] = "NORMAL", [STR_DIFF_HARD] = "SCHWER", [STR_DIFF_HELL] = "HOELLE",
        [STR_PAUSE] = "PAUSE", [STR_RESUME] = "WEITER", [STR_MAIN_MENU] = "HAUPTMENU", [STR_GAME_OVER] = "GAME OVER",
        [STR_POINTS] = "PUNKTE", [STR_RETRY] = "NEUSTART", [STR_QUIT_PROMPT] = "SPIEL BEENDEN?",
        [STR_QUIT_SUB] = "MOECHTEST DU BEENDEN?", [STR_YES] = "JA", [STR_NO] = "NEIN", [STR_CTRL_TITLE] = "STEUERUNG",
        [STR_CTRL_UP] = "HOCH", [STR_CTRL_DOWN] = "RUNTER", [STR_CTRL_LEFT] = "LINKS", [STR_CTRL_RIGHT] = "RECHTS",
        [STR_CTRL_SHOOT] = "SCHUSS", [STR_CTRL_TURBO] = "TURBO", [STR_CTRL_AIM] = "ZIELEN",
        [STR_CTRL_DASH] = "DASH", [STR_REBIND_TITLE] = "TASTE AENDERN", [STR_PRESS_KEY] = "TASTE/KNOPF DRUECKEN...",
        [STR_CANCEL_KEY] = "BKSP / ESC ABBRUCH", [STR_INVALID_ACTION] = "UNGULTIG", [STR_DUPLICATE_KEY] = "EINGABE BEREITS BELEGT",
        [STR_USE_OTHER_KEY] = "WAEHLE ANDERE EINGABE", [STR_PRESS_BACK] = "DRUECKE %s ZURUECK", [STR_PRESENTS] = "PRAESENTIERT...",
        [STR_READY] = "BEREIT!", [STR_GO] = "LOS!", [STR_WAVE] = "WELLE",
        [STR_GAMEPAD_CONNECTED] = "CONTROLLER ERKANNT", [STR_KEYBOARD_CONNECTED] = "TASTATUR ERKANNT",
        [STR_TOUCH_CONNECTED] = "TOUCH AKTIVIERT",
        [STR_FREEZE] = "FROST!", [STR_POISON] = "GIFT!",
        [STR_LOGROS] = "ERFOLGE",
        [STR_ACH_1_TITLE] = "LEICHT REKRUT", [STR_ACH_1_DESC] = "BESIEGE 10 FEINDE AUF LEICHT",
        [STR_ACH_2_TITLE] = "NORMAL REKRUT", [STR_ACH_2_DESC] = "BESIEGE 10 FEINDE AUF NORMAL",
        [STR_ACH_3_TITLE] = "SCHWER REKRUT", [STR_ACH_3_DESC] = "BESIEGE 10 FEINDE AUF SCHWER",
        [STR_ACH_4_TITLE] = "HOELLEN REKRUT", [STR_ACH_4_DESC] = "BESIEGE 10 FEINDE AUF HOELLE",
        [STR_ACH_5_TITLE] = "LEICHT VETERAN", [STR_ACH_5_DESC] = "BESIEGE 100 FEINDE AUF LEICHT",
        [STR_ACH_6_TITLE] = "NORMAL VETERAN", [STR_ACH_6_DESC] = "BESIEGE 100 FEINDE AUF NORMAL",
        [STR_ACH_7_TITLE] = "SCHWER VETERAN", [STR_ACH_7_DESC] = "BESIEGE 100 FEINDE AUF SCHWER",
        [STR_ACH_8_TITLE] = "HOELLEN VETERAN", [STR_ACH_8_DESC] = "BESIEGE 100 FEINDE AUF HOELLE",
        [STR_ACH_9_TITLE] = "LEICHT MEISTER", [STR_ACH_9_DESC] = "BESIEGE 500 FEINDE AUF LEICHT",
        [STR_ACH_10_TITLE] = "NORMAL MEISTER", [STR_ACH_10_DESC] = "BESIEGE 500 FEINDE AUF NORMAL",
        [STR_ACH_11_TITLE] = "SCHWER MEISTER", [STR_ACH_11_DESC] = "BESIEGE 500 FEINDE AUF SCHWER",
        [STR_ACH_12_TITLE] = "HOELLEN MEISTER", [STR_ACH_12_DESC] = "BESIEGE 500 FEINDE AUF HOELLE",
        [STR_ACH_13_TITLE] = "HEISSER SCHUSS", [STR_ACH_13_DESC] = "25 KILLS MIT SCHWEREM SCHUSS",
        [STR_ACH_14_TITLE] = "UNBERUEHRT", [STR_ACH_14_DESC] = "WELLE OHNE SCHADEN BEENDEN",
        [STR_ACH_15_TITLE] = "!BOOM!", [STR_ACH_15_DESC] = "VERNICHTE 5 MIT EINER NUKE",
        [STR_ACH_16_TITLE] = "EISESKAELTE", [STR_ACH_16_DESC] = "FRIERE 10 FEINDE EIN",
        [STR_ACH_17_TITLE] = "TOXISCH", [STR_ACH_17_DESC] = "VERGIFTE 10 FEINDE",
        [STR_ACH_18_TITLE] = "ZEITLUPE", [STR_ACH_18_DESC] = "BESIEGE 5 IN DER ZEITLUPE",
        [STR_ACH_19_TITLE] = "KRAFTVOLL", [STR_ACH_19_DESC] = "SAMMLE DEIN ERSTES POWER-UP",
        [STR_ACH_20_TITLE] = "UNANTASTBAR", [STR_ACH_20_DESC] = "30S IN HOELLE SCHADLOS",
        [STR_ACH_21_TITLE] = "BOSSJAEGER", [STR_ACH_21_DESC] = "VERNICHTE 5 BOSSE",
        [STR_ACH_22_TITLE] = "WARP BRECHER", [STR_ACH_22_DESC] = "ZERSTOERE 10 TELEPORTER",
        [STR_ACH_23_TITLE] = "LETZTER ATEM", [STR_ACH_23_DESC] = "1000 PKT MIT 1 LEBEN",
        [STR_ACH_24_TITLE] = "VOLLES ARSENAL", [STR_ACH_24_DESC] = "NUTZE ALLE SCHUSSARTEN",
        [STR_ACH_25_TITLE] = "GOTT MODUS", [STR_ACH_25_DESC] = "COMBO X10 MIT POWER-UP",
        [STR_ACH_26_TITLE] = "SCHNELLFEUER", [STR_ACH_26_DESC] = "10 KILLS MIT SCHNELLFEUER",
        [STR_ACH_27_TITLE] = "PERFEKT DUCK", [STR_ACH_27_DESC] = "WEICHE 10 SCHUESSEN PER DASH AUS",
        [STR_ACH_28_TITLE] = "TOTALREINIGUNG", [STR_ACH_28_DESC] = "NUKE MIT 10+ FEINDEN",
        [STR_ACH_29_TITLE] = "VOLLTRUNKEN", [STR_ACH_29_DESC] = "TRINKE DIE GRUENE FLASCHE",
        [STR_ACH_30_TITLE] = "LEGENDE", [STR_ACH_30_DESC] = "SCHALTE ALLE ERFOLGE FREI",
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
    return g_lang_strings[lang][id];
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

void clearAllHighScoresPC() {
    int scores[4] = {0};
    FILE *f = fopen(PlatformGetDataPath("records.dat"), "wb");
    if (f) { 
        fwrite(scores, sizeof(int), 4, f); 
        fclose(f); 
    }
}

void saveAchievementsPC() {
    FILE *f = fopen(PlatformGetDataPath("achievements.dat"), "wb");
    if (f) {
        fwrite(g_achievements_unlocked, sizeof(bool), 30, f);
        fwrite(g_achievement_progress, sizeof(int), 30, f);
        fclose(f);
    }
}

void loadAchievementsPC() {
    FILE *f = fopen(PlatformGetDataPath("achievements.dat"), "rb");
    if (f) {
        fread(g_achievements_unlocked, sizeof(bool), 30, f);
        fread(g_achievement_progress, sizeof(int), 30, f);
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

void loadConfigPC() {
    FILE *f = fopen(PlatformGetDataPath("config.dat"), "rb");
    if (f) { fread(&g_config, sizeof(GameConfig), 1, f); fclose(f); }
    if (g_config.target_fps < 0 || g_config.target_fps > 1) g_config.target_fps = 1;
#if defined(PLATFORM_IOS) || defined(PLATFORM_ANDROID)
    if (g_config.screen_mode < 0 || g_config.screen_mode > 2) g_config.screen_mode = 1;
#else
    if (g_config.screen_mode < 0 || g_config.screen_mode > 1) g_config.screen_mode = 0;
#endif
    if (g_config.language < 0 || g_config.language >= LANG_COUNT) g_config.language = 0;
    if (g_config.vol_bgm < 0 || g_config.vol_bgm > 10) g_config.vol_bgm = 7;
    if (g_config.vol_sfx < 0 || g_config.vol_sfx > 10) g_config.vol_sfx = 8;
    if (g_config.crt_filter < 0 || g_config.crt_filter > 2) g_config.crt_filter = 0;
}

void ApplyVideoSettings() {
    int fps_values[2] = { 30, 60 };
    SetTargetFPS(fps_values[g_config.target_fps]);
#if !defined(PLATFORM_ANDROID) && !defined(PLATFORM_IOS)
    if (g_config.vsync) SetWindowState(FLAG_VSYNC_HINT);
    else ClearWindowState(FLAG_VSYNC_HINT);

    int rw = g_resolutions[g_config.res_index][0];
    int rh = g_resolutions[g_config.res_index][1];

    if (g_config.screen_mode == 0) {
        if (IsWindowFullscreen()) ToggleFullscreen();
        ClearWindowState(FLAG_WINDOW_UNDECORATED);
        Vector2 cur_pos = GetWindowPosition();
        SetWindowSize(rw, rh);
        SetWindowPosition((int)cur_pos.x, (int)cur_pos.y);
    } else if (g_config.screen_mode == 1) {
        if (IsWindowFullscreen()) ToggleFullscreen();
        SetWindowState(FLAG_WINDOW_UNDECORATED);
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        SetWindowPosition(0, 0);
    }
#endif
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
    if (index < 0 || index >= 30) return;

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

/* iOS/desktop shared game state promoted from the original main() so iOS can
   drive the game through raylib's callback lifecycle. */
static int state = 14;
static int intro_timer = 0;
static int frame_count = 0;
static int menu_selection = 0;
static int options_selection = 0;
static int pause_options_selection = 0;
static int exit_confirm_selection = 1;
static int options_scroll_offset = 0;
static int pause_options_scroll_offset = 0;
static int ach_scroll_row = 0;
static int pause_options_enter_cooldown = 0;
static bool just_entered_menu = false;
static int pause_resume_cooldown = 0;
static int pause_toggle_cooldown = 0;
static int stick_menu_cooldown = 0;
static int screen_shake_timer = 0;
static int controls_origin_state = 7;
static int diff_selection = 1;
static int current_difficulty = 1;
static int pause_selection = 0;
static int gameover_selection = 0;
static int confirm_selection = 0;
static int controls_selection = 0;
static int rebinding_action = -1;
static int duplicate_key_warning = 0;
static int px = 110, py = 75, speed = 2, dir_x = 0, dir_y = -1;
static int player_hp = 3, score = 0, invincibility = 0;
static int shoot_cd = 0, combo = 1, combo_timer = 0, wave = 1;
static int p_type = 0, p_timer = 0;
static int slowmo_timer = 0, nuke_timer = 0, dash_cd = 0, dash_invincibility = 0, death_timer = 0, ready_timer = 0;
static int hell_no_damage_frames = 0;
static bool request_exit = false;
static Bullet bullets[MAX_BULLETS] = {0};
static Bullet e_bullets[MAX_E_BULLETS] = {0};
static Enemy enemies[MAX_ENEMIES] = {0};
static PowerUp p_up = {0};
static ExplosionEffect explosions[MAX_EXPLO] = {0};
static FloatingText ftexts[MAX_TEXTS] = {0};
static TitleStar title_stars[MAX_TITLE_STARS];
static MenuStar menu_stars[MAX_MENU_STARS];
static GameStar game_stars[MAX_GAME_STARS];
static BloodDrop blood_drops[MAX_BLOOD_DROPS];
static float time_accumulator = 0.0f;
static const float FIXED_DELTA = 1.0f / 60.0f;
static RenderTexture2D target = {0};
static Shader crtShader = {0};
static int filterTypeLoc = -1;
static Texture2D logoTexture = {0};

 #define SPAWN_FTEXT(fx, fy, fstr, fcol) do { \
        for (int ft_idx = 0; ft_idx < MAX_TEXTS; ft_idx++) { \
            if (ftexts[ft_idx].timer <= 0) { \
                ftexts[ft_idx].x = (fx); \
                ftexts[ft_idx].y = (fy); \
                snprintf(ftexts[ft_idx].text, sizeof(ftexts[ft_idx].text), "%s", (fstr)); \
                ftexts[ft_idx].color = (fcol); \
                ftexts[ft_idx].timer = 40; \
                break; \
            } \
        } \
    } while(0)



static void GameInit(void)
{
    loadConfigPC();
    loadAchievementsPC();

    g_achievements_unlocked[ACH_LEYENDA] = false;
    int unlocked_count_on_load = 0;
    for (int ach_idx = 0; ach_idx < ACH_LEYENDA; ach_idx++) {
        if (g_achievements_unlocked[ach_idx]) unlocked_count_on_load++;
    }
    g_achievement_progress[ACH_LEYENDA] = unlocked_count_on_load;

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(0, 0, "Astrallitic");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
#else
    InitWindow(g_resolutions[g_config.res_index][0], g_resolutions[g_config.res_index][1], "Astrallitic - PC Edition");
    SetExitKey(KEY_NULL);
    ApplyVideoSettings();
#endif

    InitGameAudio();
    loadControlsPC();
    MobileInput_Init();

    if (g_achievement_progress[ACH_LEYENDA] >= ACH_LEYENDA) {
        UnlockAchievement(ACH_LEYENDA);
    } else {
        saveAchievementsPC();
    }

    target = LoadRenderTexture(SCREEN_W, SCREEN_H);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    crtShader = LoadShaderFromMemory(crt_vertex_shader_code, crt_shader_code);
    filterTypeLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "filterType") : -1;

    logoTexture = (Texture2D){ 0 };
    const char* logo_path = PlatformGetAssetPath("image.png");
    Image img = LoadImage(logo_path);
    if (IsImageValid(img)) {
        ImageResizeNN(&img, 80, 80);
        logoTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }



    for(int s = 0; s < MAX_TITLE_STARS; s++) {
        title_stars[s].x = (s % 2 == 0) ? (rand() % 30 + 2) : (rand() % 30 + 208);
        title_stars[s].y = rand() % 154 + 2;
        title_stars[s].speed = (s % 3) + 1;
        title_stars[s].color = (title_stars[s].speed == 3) ? WHITE : ((title_stars[s].speed == 2) ? GBA_COLOR(16, 26, 31) : GBA_COLOR(8, 14, 22));
    }

    for(int m = 0; m < MAX_MENU_STARS; m++) {
        menu_stars[m].x = (rand() % 2 == 0) ? (rand() % 21 + 4) : (rand() % 21 + 215);
        menu_stars[m].y = rand() % 144 + 6;
        menu_stars[m].vy = (m % 2) + 1;
        menu_stars[m].color = (menu_stars[m].vy == 2) ? WHITE : GBA_COLOR(10, 22, 31);
    }

    for(int g = 0; g < MAX_GAME_STARS; g++) {
        game_stars[g].x = rand() % 232 + 4;
        game_stars[g].y = rand() % 138 + 18;
        game_stars[g].speed = (g % 2) + 1;
        game_stars[g].color = (game_stars[g].speed == 2) ? GBA_COLOR(12, 18, 24) : GBA_COLOR(6, 8, 14);
    }

    for(int b = 0; b < MAX_BLOOD_DROPS; b++) {
        blood_drops[b].x = rand() % 232 + 4;
        blood_drops[b].y = rand() % 145 + 10;
        blood_drops[b].speed = (b % 3) + 1;
        blood_drops[b].color = (b % 2 == 0) ? GBA_COLOR(24, 0, 0) : GBA_COLOR(31, 2, 2);
    }


    
}

static void GameUpdate(void)
{
        float frame_dt = GetFrameTime();
        if (frame_dt > 0.25f) frame_dt = 0.25f;
        time_accumulator += frame_dt;

        UpdateGameBgm();

        if (g_device_toast_timer > 0) g_device_toast_timer--;
        if (g_achievement_toast_timer > 0) {
            g_achievement_toast_timer--;
            if (g_achievement_toast_timer == 0 && g_achievement_queue_count > 0) {
                g_latest_unlocked_ach = g_achievement_queue[0];
                for (int q_idx = 1; q_idx < g_achievement_queue_count; q_idx++)
                    g_achievement_queue[q_idx - 1] = g_achievement_queue[q_idx];
                g_achievement_queue_count--;
                g_achievement_toast_timer = 180;
            }
        }
        if (stick_menu_cooldown > 0) stick_menu_cooldown--;
        if (pause_toggle_cooldown > 0) pause_toggle_cooldown--;
        if (pause_options_enter_cooldown > 0) pause_options_enter_cooldown--;
        if (screen_shake_timer > 0) screen_shake_timer--;

        // --- DETECCIÓN DINÁMICA DE ENTRADAS ---
        bool is_gameplay = (state == 1 || state == 9 || state == 8);
        MobileInput_Update(is_gameplay);
        MobileInputState mobile = MobileInput_GetState();
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

        bool pad_menu_up    = pad_active && ((IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_UP) || pad_stick_menu_up) && stick_menu_cooldown == 0);
        bool pad_menu_down  = pad_active && ((IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_DOWN) || pad_stick_menu_down) && stick_menu_cooldown == 0);
        bool pad_menu_left  = pad_active && ((IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || pad_stick_menu_left) && stick_menu_cooldown == 0);
        bool pad_menu_right = pad_active && ((IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || pad_stick_menu_right) && stick_menu_cooldown == 0);

        bool pad_lb_pressed = pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_LEFT_TRIGGER_1);
        bool pad_rb_pressed = pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_TRIGGER_1);

        bool pad_menu_accept = pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
        bool pad_menu_back   = pad_active && IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
        bool pad_btn_start   = pad_active && (IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_MIDDLE_RIGHT) || IsGamepadButtonPressed(pad_id, GAMEPAD_BUTTON_MIDDLE_LEFT));

        if (pad_stick_menu_up || pad_stick_menu_down || pad_stick_menu_left || pad_stick_menu_right ||
            IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_UP) ||
            IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ||
            IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ||
            IsGamepadButtonDown(pad_id, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
            if (stick_menu_cooldown == 0) stick_menu_cooldown = 14;
        }

        int current_frame_key = GetKeyPressed();

        bool any_pad_event = false;
        if (pad_active) {
            if (fabsf(pad_stick_x) > 0.3f || fabsf(pad_stick_y) > 0.3f) any_pad_event = true;
            for (int b = 1; b <= 17; b++) {
                if (IsGamepadButtonPressed(pad_id, b)) { any_pad_event = true; break; }
            }
        }

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
        bool any_touch_event = (GetTouchPointCount() > 0) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool any_kb_event = (current_frame_key > 0) || any_touch_event;
#else
        bool any_kb_event = (current_frame_key > 0 || IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
#endif

        if (any_pad_event && g_last_input_device != INPUT_GAMEPAD) {
            g_last_input_device = INPUT_GAMEPAD;
            g_toast_device = INPUT_GAMEPAD;
            g_device_toast_timer = 120;
        } else if (any_kb_event && g_last_input_device != INPUT_KEYBOARD) {
            g_last_input_device = INPUT_KEYBOARD;
            g_toast_device = INPUT_KEYBOARD;
            g_device_toast_timer = 120;
        }

#if !defined(PLATFORM_IOS)
        if (!IsWindowFocused() && (state == 1 || state == 9)) {
            state = 3;
            pause_selection = 0;
            pause_toggle_cooldown = 15;
        }
#endif

        float screen_render_w = (float)GetScreenWidth();
        float screen_render_h = (float)GetScreenHeight();
        float scale_val = fminf(screen_render_w / (float)SCREEN_W, screen_render_h / (float)SCREEN_H);
        float draw_w = (float)SCREEN_W * scale_val;
        float draw_h = (float)SCREEN_H * scale_val;

#if defined(PLATFORM_IOS) || defined(PLATFORM_ANDROID)
        if (g_config.screen_mode == 1) {
            // 16:9 Panorámico: expande la imagen a formato panorámico 16:9
            draw_w = draw_h * (16.0f / 9.0f);
            if (draw_w > screen_render_w) draw_w = screen_render_w;
        } else if (g_config.screen_mode == 2) {
            // Completa: ocupa toda la pantalla del dispositivo
            draw_w = screen_render_w;
        }
#endif

        float offset_x = (screen_render_w - draw_w) * 0.5f;
        float offset_y = (screen_render_h - draw_h) * 0.5f;

        Vector2 m_pos = GetMousePosition();
        int mouse_x = (draw_w > 0.001f) ? (int)((m_pos.x - offset_x) * ((float)SCREEN_W / draw_w)) : 0;
        int mouse_y = (draw_h > 0.001f) ? (int)((m_pos.y - offset_y) * ((float)SCREEN_H / draw_h)) : 0;
        bool mouse_clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool mouse_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        Vector2 m_delta = GetMouseDelta();
        float v_delta_y = (scale_val > 0.001f) ? (m_delta.y / scale_val) : m_delta.y;
        float v_delta_x = (scale_val > 0.001f) ? (m_delta.x / scale_val) : m_delta.x;
        bool mouse_moved = (m_delta.x != 0 || m_delta.y != 0);
        float mouse_wheel = GetMouseWheelMove();

        if (just_entered_menu) {
            mouse_clicked = false;
            just_entered_menu = false;
        }

        if (pause_resume_cooldown > 0) pause_resume_cooldown--;

        bool m_up    = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || pad_menu_up || mobile.up_pressed;
        bool m_down  = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || pad_menu_down || mobile.down_pressed;
        bool m_left  = IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || pad_menu_left || mobile.left_pressed;
        bool m_right = IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || pad_menu_right || mobile.right_pressed;
        bool m_accept = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || pad_menu_accept || mobile.a_pressed;
        bool m_back   = IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT) || IsKeyPressed(KEY_BACK) || pad_menu_back || mobile.b_pressed;

        // En pantallas con marco de menú, tocar la barra inferior de volver equivale a pulsar Volver
        if (!is_gameplay && mouse_clicked && mouse_y >= 138 && mouse_y <= 158 && mouse_x >= 20 && mouse_x <= 220) {
            m_back = true;
        }

        bool k_hold_up    = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || IsKeyDown(g_keys.key_up) || pad_game_hold_up || mobile.up_down;
        bool k_hold_down  = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S) || IsKeyDown(g_keys.key_down) || pad_game_hold_down || mobile.down_down;
        bool k_hold_left  = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || IsKeyDown(g_keys.key_left) || pad_game_hold_left || mobile.left_down;
        bool k_hold_right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) || IsKeyDown(g_keys.key_right) || pad_game_hold_right || mobile.right_down;
        bool k_dash       = IsKeyPressed(KEY_C) || IsKeyPressed(KEY_I) || (current_frame_key == g_keys.key_dash) || pad_game_dash || mobile.dash_pressed;
        bool k_start      = (pause_toggle_cooldown == 0) && (IsKeyPressed(KEY_ESCAPE) || (current_frame_key == KEY_ESCAPE) || pad_btn_start || mobile.pause_pressed);

        if (duplicate_key_warning > 0) duplicate_key_warning--;

        // --- GESTIÓN DE MENÚS ---
        if (state == 0) {
            PlayGameBgm(0);
            if (m_up)   menu_selection = (menu_selection - 1 + 3) % 3;
            if (m_down) menu_selection = (menu_selection + 1) % 3;

            for (int opt_idx = 0; opt_idx < 3; opt_idx++) {
                int opt_y = 86 + (opt_idx * 20);
                if (mouse_x >= 45 && mouse_x <= 195 && mouse_y >= opt_y - 3 && mouse_y <= opt_y + 12) {
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
                if (menu_selection == 0) { state = 5; PlayGameBgm(3); diff_selection = current_difficulty; }
                else if (menu_selection == 1) { state = 7; PlayGameBgm(1); options_selection = 0; options_scroll_offset = 0; }
                else if (menu_selection == 2) { state = 6; PlayGameBgm(2); show_ach_details = false; selected_ach_index = 0; ach_scroll_row = 0; }
            }
        }
        else if (state == 13) {
            if (m_up || m_down) exit_confirm_selection = 1 - exit_confirm_selection;

            for (int conf_idx = 0; conf_idx < 2; conf_idx++) {
                int opt_y = 86 + (conf_idx * 14);
                if (mouse_x >= 45 && mouse_x <= 195 && mouse_y >= opt_y - 2 && mouse_y <= opt_y + 10) {
                    if (mouse_moved) exit_confirm_selection = conf_idx;
                    if (mouse_clicked) { exit_confirm_selection = conf_idx; m_accept = true; }
                }
            }

            if (m_accept) {
                if (exit_confirm_selection == 0) {
#if defined(PLATFORM_IOS)
                    state = 0;
                    just_entered_menu = true;
                    PlaySfx(sndHit);
#else
                    request_exit = true;
#endif
                }
                else { state = 0; just_entered_menu = true; PlaySfx(sndHit); }
            }
            if (m_back) { state = 0; just_entered_menu = true; }
        }
        else if (state == 5) {
            PlayGameBgm(3);
            if (m_up)   diff_selection = (diff_selection - 1 + 4) % 4;
            if (m_down) diff_selection = (diff_selection + 1) % 4;

            for (int diff_idx = 0; diff_idx < 4; diff_idx++) {
                int opt_y = 54 + (diff_idx * 20);
                if (mouse_x >= 45 && mouse_x <= 195 && mouse_y >= opt_y - 3 && mouse_y <= opt_y + 12) {
                    if (mouse_moved) diff_selection = diff_idx;
                    if (mouse_clicked) { diff_selection = diff_idx; m_accept = true; }
                }
            }

            if (m_back) { state = 0; PlayGameBgm(0); just_entered_menu = true; }
            if (m_accept) {
                StopAllBgm();
                current_difficulty = diff_selection;
                player_hp = (current_difficulty == 0) ? 5 : ((current_difficulty == 3) ? 1 : 3);
                score = 0; px = 110; py = 75; dir_x = 0; dir_y = -1; p_type = 0; p_up.active = 0;
                slowmo_timer = 0; nuke_timer = 0; dash_cd = 0; dash_invincibility = 0; death_timer = 0; combo = 1; combo_timer = 0; wave = 1;
                hell_no_damage_frames = 0; player_took_hit_this_wave = false;
                for(int e_idx=0; e_idx<MAX_ENEMIES; e_idx++) enemies[e_idx].active = 0;
                for(int b_idx=0; b_idx<MAX_BULLETS; b_idx++) bullets[b_idx].active = 0;
                for(int eb_idx=0; eb_idx<MAX_E_BULLETS; eb_idx++) e_bullets[eb_idx].active = 0;
                for(int ex_idx=0; ex_idx<MAX_EXPLO; ex_idx++) explosions[ex_idx].active = 0;
                ready_timer = 120;
                pause_resume_cooldown = 15;
                pause_toggle_cooldown = 15;
                state = 9; 
            }
        }
        else if (state == 7) {
            PlayGameBgm(1);

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            const int total_opts = 7;
            const int max_scroll = 4;
#else
            const int total_opts = 10;
            const int max_scroll = 7;
#endif

            if (mouse_wheel != 0) {
                if (mouse_wheel > 0) options_scroll_offset--;
                else if (mouse_wheel < 0) options_scroll_offset++;
                if (options_scroll_offset < 0) options_scroll_offset = 0;
                if (options_scroll_offset > max_scroll) options_scroll_offset = max_scroll;
            }

            // Desplazamiento táctil (swipe suave)
            static float s_touch_drag_opt = 0.0f;
            static bool s_is_swiping_opt = false;
            if (mouse_clicked) {
                s_touch_drag_opt = 0.0f;
                s_is_swiping_opt = false;
            }

            // Botones triangulares en el lateral derecho (x: 204..239)
            if (mouse_clicked && mouse_x >= 204 && mouse_x <= 239) {
                if (mouse_y >= 44 && mouse_y <= 84) {
                    // Triángulo Arriba: sube de opción si no está arriba del todo (Vol. Música es 0)
                    if (options_selection > 0) {
                        options_selection--;
                        PlaySfx(sndHit);
                    }
                } else if (mouse_y >= 94 && mouse_y <= 136) {
                    // Triángulo Abajo: baja de opción si no está abajo del todo (Borrar records es total_opts - 1)
                    if (options_selection < total_opts - 1) {
                        options_selection++;
                        PlaySfx(sndHit);
                    }
                }
            }

            if (m_up) {
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
                if (options_selection > 0) options_selection--;
#else
                options_selection = (options_selection - 1 + total_opts) % total_opts;
#endif
            }
            if (m_down) {
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
                if (options_selection < total_opts - 1) options_selection++;
#else
                options_selection = (options_selection + 1) % total_opts;
#endif
            }

            if (options_selection < options_scroll_offset) options_scroll_offset = options_selection;
            if (options_selection >= options_scroll_offset + 3) options_scroll_offset = options_selection - 2;
            if (options_scroll_offset < 0) options_scroll_offset = 0;
            if (options_scroll_offset > max_scroll) options_scroll_offset = max_scroll;

            const char* filter_txt[3] = { "OFF", "SCANLINES", "CRT ARCADE" };
            const char* fps_txt[2] = { "30 FPS", "60 FPS" };

            char opt_strings[10][32];
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            snprintf(opt_strings[0], 32, "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(opt_strings[1], 32, "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(opt_strings[2], 32, "%s: < %s >", T(STR_FILTER), filter_txt[g_config.crt_filter]);
            snprintf(opt_strings[3], 32, "%s", T(STR_CONTROLS));
            snprintf(opt_strings[4], 32, "%s: < %s >", T(STR_LANGUAGE), g_lang_names[g_config.language]);
            snprintf(opt_strings[5], 32, "%s: < %s >", T(STR_FPS), fps_txt[g_config.target_fps]);
            snprintf(opt_strings[6], 32, "%s", T(STR_DELETE_RECORDS));
#else
            const char* res_txt[4] = { "960X640", "1200X800", "1440X960", "1920X1080" };
            const char* mode_txt[2] = { "VENTANA", "SIN BORDES" };
            const char* mode_txt_en[2] = { "WINDOWED", "BORDERLESS" };

            snprintf(opt_strings[0], 32, "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(opt_strings[1], 32, "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(opt_strings[2], 32, "%s: < %s >", T(STR_FILTER), filter_txt[g_config.crt_filter]);
            snprintf(opt_strings[3], 32, "%s", T(STR_CONTROLS));
            snprintf(opt_strings[4], 32, "%s: < %s >", T(STR_LANGUAGE), g_lang_names[g_config.language]);
            snprintf(opt_strings[5], 32, "%s: < %s >", T(STR_RESOLUTION), res_txt[g_config.res_index]);
            snprintf(opt_strings[6], 32, "%s: < %s >", T(STR_SCREEN_MODE), (g_config.language == 0) ? mode_txt[g_config.screen_mode] : mode_txt_en[g_config.screen_mode]);
            snprintf(opt_strings[7], 32, "%s: < %s >", T(STR_FPS), fps_txt[g_config.target_fps]);
            snprintf(opt_strings[8], 32, "%s: %s", T(STR_VSYNC), g_config.vsync ? "ON" : "OFF");
            snprintf(opt_strings[9], 32, "%s", T(STR_DELETE_RECORDS));
#endif

            // Solo procesar toque de opcion si YA esta seleccionada mediante las flechas (rodeada de amarillo)
            if (mouse_clicked && mouse_x >= 25 && mouse_x <= 195) {
                for (int slot = 0; slot < 3; slot++) {
                    int opt_idx = options_scroll_offset + slot;
                    if (opt_idx >= total_opts) break;
                    int row_y = 52 + (slot * 27);
                    if (mouse_y >= row_y - 2 && mouse_y <= row_y + 22) {
                        if (options_selection == opt_idx) {
                            const char* str = opt_strings[opt_idx];
                            const char* left_arrow = strchr(str, '<');
                            const char* right_arrow = strrchr(str, '>');

                            if (left_arrow && right_arrow) {
                                if (mouse_x < 100) m_left = true;
                                else if (mouse_x > 140) m_right = true;
                                else m_accept = true;
                            } else {
                                m_accept = true;
                            }
                        }
                    }
                }
            }

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            if (options_selection == 0) { 
                if ((m_right || m_accept) && g_config.vol_bgm < 10) { g_config.vol_bgm++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_bgm > 0) { g_config.vol_bgm--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 1) { 
                if ((m_right || m_accept) && g_config.vol_sfx < 10) { g_config.vol_sfx++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_sfx > 0) { g_config.vol_sfx--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 2) { 
                if (m_right || m_accept) { g_config.crt_filter = (g_config.crt_filter + 1) % 3; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.crt_filter = (g_config.crt_filter - 1 + 3) % 3; saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 3) { 
                if (m_accept) { state = 4; controls_origin_state = 7; controls_selection = 0; rebinding_action = -1; just_entered_menu = true; PlaySfx(sndHit); }
            } else if (options_selection == 4) { 
                if (m_right || m_accept) { g_config.language = (g_config.language + 1) % LANG_COUNT; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.language = (g_config.language - 1 + LANG_COUNT) % LANG_COUNT; saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 5) { 
                if (m_accept || m_left || m_right) { g_config.target_fps = 1 - g_config.target_fps; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 6) { 
                if (m_accept) { state = 10; confirm_selection = 0; just_entered_menu = true; PlaySfx(sndHit); }
            }
#else
            if (options_selection == 0) { 
                if ((m_right || m_accept) && g_config.vol_bgm < 10) { g_config.vol_bgm++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_bgm > 0) { g_config.vol_bgm--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 1) { 
                if ((m_right || m_accept) && g_config.vol_sfx < 10) { g_config.vol_sfx++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_sfx > 0) { g_config.vol_sfx--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 2) { 
                if (m_right || m_accept) { g_config.crt_filter = (g_config.crt_filter + 1) % 3; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.crt_filter = (g_config.crt_filter - 1 + 3) % 3; saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 3) { 
                if (m_accept) { state = 4; controls_origin_state = 7; controls_selection = 0; rebinding_action = -1; just_entered_menu = true; PlaySfx(sndHit); }
            } else if (options_selection == 4) { 
                if (m_right || m_accept) { g_config.language = (g_config.language + 1) % LANG_COUNT; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.language = (g_config.language - 1 + LANG_COUNT) % LANG_COUNT; saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 5) { 
                if (m_right || (m_accept && g_config.res_index < 3)) { g_config.res_index = (g_config.res_index + 1) % 4; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.res_index = (g_config.res_index - 1 + 4) % 4; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 6) { 
                if (m_right || (m_accept && g_config.screen_mode < 1)) { g_config.screen_mode = (g_config.screen_mode + 1) % 2; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.screen_mode = (g_config.screen_mode - 1 + 2) % 2; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 7) { 
                if (m_accept || m_left || m_right) { g_config.target_fps = 1 - g_config.target_fps; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 8) { 
                if (m_accept || m_left || m_right) { g_config.vsync = !g_config.vsync; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (options_selection == 9) { 
                if (m_accept) { state = 10; confirm_selection = 0; just_entered_menu = true; PlaySfx(sndHit); }
            }
#endif

            if (m_back) { state = 0; PlayGameBgm(0); just_entered_menu = true; }
        }
        else if (state == 12) {
            bool pause_options_input_locked = (pause_options_enter_cooldown > 0);
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

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            const int total_pause_opts = 5;
            const int max_pause_scroll = 2;
#else
            const int total_pause_opts = 8;
            const int max_pause_scroll = 5;
#endif

            if (mouse_wheel != 0) {
                if (mouse_wheel > 0) pause_options_scroll_offset--;
                else if (mouse_wheel < 0) pause_options_scroll_offset++;
                if (pause_options_scroll_offset < 0) pause_options_scroll_offset = 0;
                if (pause_options_scroll_offset > max_pause_scroll) pause_options_scroll_offset = max_pause_scroll;
            }

            // Desplazamiento táctil (swipe) y botones triangulares
            static float s_touch_drag_pause = 0.0f;
            static bool s_is_swiping_pause = false;
            if (mouse_clicked) {
                s_touch_drag_pause = 0.0f;
                s_is_swiping_pause = false;
            }

            // Botones triangulares en el lateral derecho (x: 204..239)
            if (mouse_clicked && mouse_x >= 204 && mouse_x <= 239) {
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
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
                if (pause_options_selection > 0) pause_options_selection--;
#else
                pause_options_selection = (pause_options_selection - 1 + total_pause_opts) % total_pause_opts;
#endif
            }
            if (m_down) {
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
                if (pause_options_selection < total_pause_opts - 1) pause_options_selection++;
#else
                pause_options_selection = (pause_options_selection + 1) % total_pause_opts;
#endif
            }

            if (pause_options_selection < pause_options_scroll_offset) pause_options_scroll_offset = pause_options_selection;
            if (pause_options_selection >= pause_options_scroll_offset + 3) pause_options_scroll_offset = pause_options_selection - 2;
            if (pause_options_scroll_offset < 0) pause_options_scroll_offset = 0;
            if (pause_options_scroll_offset > max_pause_scroll) pause_options_scroll_offset = max_pause_scroll;

            const char* filter_txt[3] = { "OFF", "SCANLINES", "CRT ARCADE" };
            const char* fps_txt[2] = { "30 FPS", "60 FPS" };

            char pause_opt_strings[8][32];
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            snprintf(pause_opt_strings[0], 32, "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(pause_opt_strings[1], 32, "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(pause_opt_strings[2], 32, "%s: < %s >", T(STR_FILTER), filter_txt[g_config.crt_filter]);
            snprintf(pause_opt_strings[3], 32, "%s", T(STR_CONTROLS));
            snprintf(pause_opt_strings[4], 32, "%s: < %s >", T(STR_FPS), fps_txt[g_config.target_fps]);
#else
            const char* res_txt[4] = { "960X640", "1200X800", "1440X960", "1920X1080" };
            const char* mode_txt[2] = { "VENTANA", "SIN BORDES" };
            const char* mode_txt_en[2] = { "WINDOWED", "BORDERLESS" };

            snprintf(pause_opt_strings[0], 32, "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(pause_opt_strings[1], 32, "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(pause_opt_strings[2], 32, "%s: < %s >", T(STR_FILTER), filter_txt[g_config.crt_filter]);
            snprintf(pause_opt_strings[3], 32, "%s", T(STR_CONTROLS));
            snprintf(pause_opt_strings[4], 32, "%s: < %s >", T(STR_RESOLUTION), res_txt[g_config.res_index]);
            snprintf(pause_opt_strings[5], 32, "%s: < %s >", T(STR_SCREEN_MODE), (g_config.language == 0) ? mode_txt[g_config.screen_mode] : mode_txt_en[g_config.screen_mode]);
            snprintf(pause_opt_strings[6], 32, "%s: < %s >", T(STR_FPS), fps_txt[g_config.target_fps]);
            snprintf(pause_opt_strings[7], 32, "%s: %s", T(STR_VSYNC), g_config.vsync ? "ON" : "OFF");
#endif

            // Solo procesar toque de opcion si YA esta seleccionada mediante las flechas (rodeada de amarillo)
            if (mouse_clicked && mouse_x >= 25 && mouse_x <= 195) {
                for (int slot = 0; slot < 3; slot++) {
                    int opt_idx = pause_options_scroll_offset + slot;
                    if (opt_idx >= total_pause_opts) break;
                    int row_y = 52 + (slot * 27);
                    if (mouse_y >= row_y - 2 && mouse_y <= row_y + 22) {
                        if (pause_options_selection == opt_idx) {
                            const char* str = pause_opt_strings[opt_idx];
                            const char* left_arrow = strchr(str, '<');
                            const char* right_arrow = strrchr(str, '>');

                            if (left_arrow && right_arrow) {
                                if (mouse_x < 100) m_left = true;
                                else if (mouse_x > 140) m_right = true;
                                else m_accept = true;
                            } else {
                                m_accept = true;
                            }
                        }
                    }
                }
            }

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            if (pause_options_selection == 0) { 
                if ((m_right || m_accept) && g_config.vol_bgm < 10) { g_config.vol_bgm++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_bgm > 0) { g_config.vol_bgm--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 1) { 
                if ((m_right || m_accept) && g_config.vol_sfx < 10) { g_config.vol_sfx++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_sfx > 0) { g_config.vol_sfx--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 2) { 
                if (m_right || m_accept) { g_config.crt_filter = (g_config.crt_filter + 1) % 3; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.crt_filter = (g_config.crt_filter - 1 + 3) % 3; saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 3) { 
                if (m_accept) { state = 4; controls_origin_state = 12; controls_selection = 0; rebinding_action = -1; just_entered_menu = true; PlaySfx(sndHit); }
            } else if (pause_options_selection == 4) { 
                if (m_accept || m_left || m_right) { g_config.target_fps = 1 - g_config.target_fps; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
            }
#else
            if (pause_options_selection == 0) { 
                if ((m_right || m_accept) && g_config.vol_bgm < 10) { g_config.vol_bgm++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_bgm > 0) { g_config.vol_bgm--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 1) { 
                if ((m_right || m_accept) && g_config.vol_sfx < 10) { g_config.vol_sfx++; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left && g_config.vol_sfx > 0) { g_config.vol_sfx--; ApplyAudioVolumes(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 2) { 
                if (m_right || m_accept) { g_config.crt_filter = (g_config.crt_filter + 1) % 3; saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.crt_filter = (g_config.crt_filter - 1 + 3) % 3; saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 3) { 
                if (m_accept) { state = 4; controls_origin_state = 12; controls_selection = 0; rebinding_action = -1; just_entered_menu = true; PlaySfx(sndHit); }
            } else if (pause_options_selection == 4) { 
                if (m_right || (m_accept && g_config.res_index < 3)) { g_config.res_index = (g_config.res_index + 1) % 4; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.res_index = (g_config.res_index - 1 + 4) % 4; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 5) { 
                if (m_right || (m_accept && g_config.screen_mode < 1)) { g_config.screen_mode = (g_config.screen_mode + 1) % 2; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
                else if (m_left) { g_config.screen_mode = (g_config.screen_mode - 1 + 2) % 2; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 6) { 
                if (m_accept || m_left || m_right) { g_config.target_fps = 1 - g_config.target_fps; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
            } else if (pause_options_selection == 7) { 
                if (m_accept || m_left || m_right) { g_config.vsync = !g_config.vsync; ApplyVideoSettings(); saveConfigPC(); PlaySfx(sndHit); }
            }
#endif

            if (m_back) { state = 3; just_entered_menu = true; }
        }
        else if (state == 10) {
            if (m_up || m_down) confirm_selection = 1 - confirm_selection;

            for (int conf_idx = 0; conf_idx < 2; conf_idx++) {
                int opt_y = 86 + (conf_idx * 14);
                if (mouse_x >= 45 && mouse_x <= 195 && mouse_y >= opt_y - 2 && mouse_y <= opt_y + 10) {
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
                ach_scroll_row = 0;
                selected_ach_index = 0;
                show_ach_details = false;
                PlaySfx(sndHit);
                just_entered_menu = true;
            } else if (state == 15 && pad_lb_pressed) {
                state = 6;
                show_ach_details = false;
                PlaySfx(sndHit);
                just_entered_menu = true;
            }

            if (mouse_clicked && mouse_y >= 14 && mouse_y <= 32) {
                if (mouse_x >= 25 && mouse_x <= 105) { state = 6; show_ach_details = false; PlaySfx(sndHit); }
                if (mouse_x >= 125 && mouse_x <= 205) {
                    state = 15;
                    ach_scroll_row = 0;
                    selected_ach_index = 0;
                    show_ach_details = false;
                    PlaySfx(sndHit);
                }
            }

            if (state == 15) {
                static float s_touch_drag_ach = 0.0f;
                static bool s_is_swiping_ach = false;
                if (mouse_clicked) {
                    s_touch_drag_ach = 0.0f;
                    s_is_swiping_ach = false;
                }

                if (mouse_wheel != 0) {
                    if (mouse_wheel > 0 && ach_scroll_row > 0) {
                        ach_scroll_row -= 2;
                        if (ach_scroll_row < 0) ach_scroll_row = 0;
                        selected_ach_index = ach_scroll_row * 4;
                        show_ach_details = false;
                    } else if (mouse_wheel < 0 && ach_scroll_row < 6) {
                        ach_scroll_row += 2;
                        if (ach_scroll_row > 6) ach_scroll_row = 6;
                        selected_ach_index = ach_scroll_row * 4;
                        show_ach_details = false;
                    }
                }

                // Botones triangulares en lateral derecho (x: 204..239)
                if (mouse_clicked && mouse_x >= 204 && mouse_x <= 239) {
                    if (mouse_y >= 44 && mouse_y <= 84) {
                        // Triángulo Arriba: sube de página si no está en la primera página
                        if (ach_scroll_row > 0) {
                            ach_scroll_row -= 2;
                            if (ach_scroll_row < 0) ach_scroll_row = 0;
                            selected_ach_index = ach_scroll_row * 4;
                            show_ach_details = false;
                            PlaySfx(sndHit);
                        }
                    } else if (mouse_y >= 94 && mouse_y <= 136) {
                        // Triángulo Abajo: baja de página si no está en la última página
                        if (ach_scroll_row < 6) {
                            ach_scroll_row += 2;
                            if (ach_scroll_row > 6) ach_scroll_row = 6;
                            selected_ach_index = ach_scroll_row * 4;
                            show_ach_details = false;
                            PlaySfx(sndHit);
                        }
                    }
                } else if (mouse_down && mouse_x >= 10 && mouse_x <= 200 && mouse_y >= 35 && mouse_y <= 135) {
                    s_touch_drag_ach += v_delta_y;
                    if (fabsf(s_touch_drag_ach) > 5.0f) {
                        s_is_swiping_ach = true;
                        if (show_ach_details) show_ach_details = false;
                    }
                    if (s_touch_drag_ach <= -20.0f) {
                        if (ach_scroll_row < 6) {
                            ach_scroll_row += 2;
                            if (ach_scroll_row > 6) ach_scroll_row = 6;
                            selected_ach_index = ach_scroll_row * 4;
                            show_ach_details = false;
                            PlaySfx(sndHit);
                        }
                        s_touch_drag_ach = 0.0f;
                    } else if (s_touch_drag_ach >= 20.0f) {
                        if (ach_scroll_row > 0) {
                            ach_scroll_row -= 2;
                            if (ach_scroll_row < 0) ach_scroll_row = 0;
                            selected_ach_index = ach_scroll_row * 4;
                            show_ach_details = false;
                            PlaySfx(sndHit);
                        }
                        s_touch_drag_ach = 0.0f;
                    }
                }

                if (m_left)  { selected_ach_index = (selected_ach_index > 0) ? selected_ach_index - 1 : 29; PlaySfx(sndHit); }
                if (m_right) { selected_ach_index = (selected_ach_index < 29) ? selected_ach_index + 1 : 0; PlaySfx(sndHit); }
                if (m_up)    { selected_ach_index = (selected_ach_index >= 4) ? selected_ach_index - 4 : selected_ach_index; PlaySfx(sndHit); }
                if (m_down)  { selected_ach_index = (selected_ach_index + 4 < 30) ? selected_ach_index + 4 : selected_ach_index; PlaySfx(sndHit); }

                if (m_left || m_right || m_up || m_down) {
                    int sel_row = selected_ach_index / 4;
                    if (sel_row < ach_scroll_row) ach_scroll_row = (sel_row / 2) * 2;
                    if (sel_row >= ach_scroll_row + 2) ach_scroll_row = (sel_row / 2) * 2;
                    if (ach_scroll_row < 0) ach_scroll_row = 0;
                    if (ach_scroll_row > 6) ach_scroll_row = 6;
                }

                if (!s_is_swiping_ach && (mouse_x < 204) && mouse_clicked) {
                    if (show_ach_details && mouse_x >= 12 && mouse_x <= 212 && mouse_y >= 70 && mouse_y <= 138) {
                        show_ach_details = false;
                        PlaySfx(sndHit);
                    } else {
                        for (int r = 0; r < 2; r++) {
                            int row_idx = ach_scroll_row + r;
                            if (row_idx >= 8) break;
                            for (int col = 0; col < 4; col++) {
                                int ach_i = row_idx * 4 + col;
                                if (ach_i >= 30) break;
                                int bx = 16 + (col * 48);
                                int by = 38 + (r * 44);
                                if (mouse_x >= bx && mouse_x <= bx + 42 && mouse_y >= by && mouse_y <= by + 40) {
                                    if (selected_ach_index == ach_i && show_ach_details) {
                                        show_ach_details = false;
                                    } else {
                                        selected_ach_index = ach_i;
                                        show_ach_details = true;
                                    }
                                    PlaySfx(sndHit);
                                }
                            }
                        }
                    }
                }

                if (!mouse_down) {
                    s_is_swiping_ach = false;
                    s_touch_drag_ach = 0.0f;
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
                // En modo táctil, solo volver si se pulsa sobre el texto inferior "< TOCA AQUI PARA VOLVER >" o atrás
                bool clicked_back_text = (mouse_clicked && mouse_y >= 140 && mouse_y <= 158);
                if (clicked_back_text || m_back) {
                    state = controls_origin_state;
                    just_entered_menu = true;
                    PlaySfx(sndHit);
                }
            } else {
                if (rebinding_action != -1) {
                    for (int b = 1; b <= 17; b++) {
                        if (IsGamepadButtonPressed(pad_id, b)) {
                            // Un botón solo puede pertenecer a una acción.
                            if (IsPadButtonAssignedPC(b, rebinding_action)) {
                                duplicate_key_warning = 120;
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

                    for (int act_idx = 0; act_idx < 8; act_idx++) {
                        int row_y = 51 + (act_idx * 11);
                        if (mouse_x >= 15 && mouse_x <= 225 && mouse_y >= row_y - 2 && mouse_y <= row_y + 9) {
                            if (mouse_moved) controls_selection = act_idx;
                            if (mouse_clicked) {
                                controls_selection = act_idx;
                                if (mouse_x >= 140 && mouse_x <= 218) {
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

            for (int go_idx = 0; go_idx < 2; go_idx++) {
                int opt_y = 112 + (go_idx * 18);
                if (mouse_x >= 45 && mouse_x <= 195 && mouse_y >= opt_y - 3 && mouse_y <= opt_y + 11) {
                    if (mouse_moved) gameover_selection = go_idx;
                    if (mouse_clicked) { gameover_selection = go_idx; m_accept = true; }
                }
            }

            if (m_accept) {
                StopAllBgm();
                if (gameover_selection == 0) {
                    player_hp = (current_difficulty == 0) ? 5 : ((current_difficulty == 3) ? 1 : 3);
                    score = 0; px = 110; py = 75; dir_x = 0; dir_y = -1; p_type = 0; p_up.active = 0;
                    slowmo_timer = 0; nuke_timer = 0; dash_cd = 0; dash_invincibility = 0; death_timer = 0; combo = 1; combo_timer = 0; wave = 1;
                    hell_no_damage_frames = 0; player_took_hit_this_wave = false;
                    for(int e_idx=0; e_idx<MAX_ENEMIES; e_idx++) enemies[e_idx].active = 0;
                    for(int b_idx=0; b_idx<MAX_BULLETS; b_idx++) bullets[b_idx].active = 0;
                    for(int eb_idx=0; eb_idx<MAX_E_BULLETS; eb_idx++) e_bullets[eb_idx].active = 0;
                    for(int ex_idx=0; ex_idx<MAX_EXPLO; ex_idx++) explosions[ex_idx].active = 0;
                    ready_timer = 120; pause_resume_cooldown = 15; pause_toggle_cooldown = 15; state = 9; 
                } else {
                    state = 0; PlayGameBgm(0); just_entered_menu = true;
                }
            }
        }
        else if (state == 3) {
            if (k_start && pause_toggle_cooldown == 0) {
                state = 1;
                pause_selection = 0;
                pause_toggle_cooldown = 15;
                k_start = false;
                just_entered_menu = true;
            }

            if (m_up) pause_selection = (pause_selection - 1 + 3) % 3;
            if (m_down) pause_selection = (pause_selection + 1) % 3;

            for (int p_idx = 0; p_idx < 3; p_idx++) {
                int py_pos = 58 + (p_idx * 27);
                if (mouse_x >= 42 && mouse_x <= 198 && mouse_y >= py_pos - 3 && mouse_y <= py_pos + 16) {
                    if (mouse_moved) pause_selection = p_idx;
                    if (mouse_clicked) { pause_selection = p_idx; m_accept = true; }
                }
            }

            if (m_accept) {
                if (pause_selection == 0) {
                    state = 1;
                    pause_resume_cooldown = 15;
                    pause_toggle_cooldown = 15;
                    k_start = false;
                    just_entered_menu = true;
                }
                else if (pause_selection == 1) {
                    state = 12;
                    pause_options_selection = 0;
                    pause_options_scroll_offset = 0;
                    pause_options_enter_cooldown = 2;
                    StopAllBgm();
                    just_entered_menu = true;
                }
                else if (pause_selection == 2) { state = 0; PlayGameBgm(0); just_entered_menu = true; }
            }
            if (m_back) {
                state = 1;
                pause_resume_cooldown = 15;
                pause_toggle_cooldown = 15;
                k_start = false;
                just_entered_menu = true;
            }
        }

        // --- BUCLE FIXED UPDATE (LÓGICA FÍSICA A 60 HZ) ---
        while (time_accumulator >= FIXED_DELTA) {
            time_accumulator -= FIXED_DELTA;
            frame_count++;

            for (int ft_idx = 0; ft_idx < MAX_TEXTS; ft_idx++) {
                if (ftexts[ft_idx].timer > 0) {
                    ftexts[ft_idx].timer--;
                    ftexts[ft_idx].y -= 0.4f;
                }
            }

            if (state == 14) {
                intro_timer++;
                if (intro_timer > 250) { state = 0; PlayGameBgm(0); }
            } 
            else if (state == 0 || state == 13) {
                for(int s = 0; s < MAX_TITLE_STARS; s++) {
                    title_stars[s].y += title_stars[s].speed;
                    if (title_stars[s].y >= 158) title_stars[s].y = 2;
                }
            }
            else if (state == 5 || state == 6 || state == 15 || state == 7 || state == 12 || state == 4) {
                for(int m = 0; m < MAX_MENU_STARS; m++) {
                    menu_stars[m].y -= menu_stars[m].vy;
                    if(menu_stars[m].y <= 2) menu_stars[m].y = 156;
                }
            }
            else if (state == 9) {
                UpdateReadyJinglePC(ready_timer);
                ready_timer--;
                if (ready_timer <= 0) state = 1; 
            }
            else if (state == 8) {
                death_timer--;
                if (death_timer <= 0) { saveHighScorePC(current_difficulty, score); gameover_selection = 0; state = 2; PlayGameBgm(4); }
            }
            else if (state == 2) {
                for(int b = 0; b < MAX_BLOOD_DROPS; b++) {
                    blood_drops[b].y -= blood_drops[b].speed;
                    if(blood_drops[b].y <= 2) blood_drops[b].y = 156;
                }
            }
            else if (state == 1) {
                if (k_start && pause_toggle_cooldown == 0) {
                    state = 3;
                    pause_selection = 0;
                    pause_toggle_cooldown = 15;
                    k_start = false;
                }

                if (nuke_timer > 0) {
                    nuke_timer--;
                    invincibility = 5;
                    screen_shake_timer = 20;
                    int nuke_kills_count = 0;
                    for(int k = 0; k < MAX_ENEMIES; k++) {
                        if (enemies[k].active) {
                            enemies[k].active = 0;
                            nuke_kills_count++;
                            for(int ex = 0; ex < MAX_EXPLO; ex++) {
                                if (!explosions[ex].active) {
                                    explosions[ex].active = 1;
                                    explosions[ex].x = enemies[k].x;
                                    explosions[ex].y = enemies[k].y;
                                    explosions[ex].timer = 30;
                                    break;
                                }
                            }
                            int nuke_points = (enemies[k].type == 3) ? 500 : (enemies[k].type == 4 ? 200 : (enemies[k].type == 7 ? 150 : (enemies[k].type == 6 ? 50 : 100)));
                            score += nuke_points * combo;
                            if (combo < 5 && p_type != 8 && p_type != 13) combo++;
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
                    if (game_stars[g].y >= 158) {
                        game_stars[g].y = 18;
                        game_stars[g].x = rand() % 232 + 4;
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
                if (p_timer > 0) {
                    p_timer--;
                    if (p_timer == 0) { p_type = 0; if (combo > 1) combo = 1; }
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

                if (k_dash && dash_cd == 0 && mx_dir != 0) {
                    PlaySfx(sndDash);
                    px += mx_dir * 28;
                    dash_cd = 60;
                    invincibility = 15;
                    dash_invincibility = 15;
                }

                bool is_aim_locked = IsKeyDown(g_keys.key_aim) || (pad_active && IsGamepadButtonDown(pad_id, g_pad.btn_aim)) || mobile.aim_down;
                if (is_aim_locked) {
                    if (mx_dir != 0 || my_dir != 0) { dir_x = mx_dir; dir_y = my_dir; }
                    mx_dir = 0;
                    my_dir = 0;
                } else {
                    if (mx_dir != 0 || my_dir != 0) { dir_x = mx_dir; dir_y = my_dir; }
                    px += mx_dir * speed;
                    py += my_dir * speed;
                }

                if (px < 0) px = 0; if (px > 230) px = 230;
                if (py < 16) py = 16; if (py > 150) py = 150;

                if (shoot_cd > 0) shoot_cd--;
                bool is_shooting = IsKeyDown(g_keys.key_shoot) || (current_frame_key == g_keys.key_shoot) || (pad_active && (IsGamepadButtonDown(pad_id, g_pad.btn_shoot) || IsGamepadButtonPressed(pad_id, g_pad.btn_shoot))) || mobile.fire_down;
                if (pause_resume_cooldown == 0 && is_shooting && shoot_cd == 0) {
                    shoot_cd = (p_type == 2) ? 6 : ((p_type == 5) ? 20 : 15);
                    PlaySfx(sndShoot);

                    if (p_type == 0) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_NORMAL_MASK;
                    else if (p_type == 2) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_RAPID_MASK;
                    else if (p_type == 3) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_TRIPLE_MASK;
                    else if (p_type == 5) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_HEAVY_MASK;
                    else if (p_type == 9) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_FREEZE_MASK;
                    else if (p_type == 12) g_achievement_progress[ACH_FULL_ARSENAL] |= ARSENAL_POISON_MASK;

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

                if (frame_count % (current_difficulty == 0 ? 500 : 700) == 0 && !p_up.active && p_type == 0 && slowmo_timer == 0 && nuke_timer == 0) {
                    p_up.active = 1;
                    p_up.type = (rand() % 13) + 1;
                    p_up.x = (rand() % 180) + 20;
                    p_up.y = (rand() % 110) + 30;
                    p_up.life_timer = 1800;
                    
                    int active_enemies_count = 0;
                    for (int en_c = 0; en_c < MAX_ENEMIES; en_c++) if (enemies[en_c].active) active_enemies_count++;
                }

                if (p_up.active) {
                    p_up.life_timer--;
                    if (p_up.life_timer <= 0) {
                        p_up.active = 0;
                    } else {
                        if (px < p_up.x + 8 && px + 10 > p_up.x && py < p_up.y + 8 && py + 10 > p_up.y) {
                            p_up.active = 0;

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
                                if (p_up.type == 4) { if (player_hp < (current_difficulty == 3 ? 3 : 5)) player_hp++; } 
                                else if (p_up.type == 7) { slowmo_timer = 600; } 
                                else if (p_up.type == 8) { combo = 5; p_type = 8; p_timer = 600; } 
                                else if (p_up.type == 13) { combo = 10; p_type = 13; p_timer = 600; } 
                                else { p_type = p_up.type; p_timer = 600; }
                            }
                        }
                    }
                }

                for(int b_idx = 0; b_idx < MAX_BULLETS; b_idx++) {
                    if(bullets[b_idx].active) {
                        bullets[b_idx].x += bullets[b_idx].vx;
                        bullets[b_idx].y += bullets[b_idx].vy;
                        if(bullets[b_idx].x > 240 || bullets[b_idx].x < 0 || bullets[b_idx].y < 16 || bullets[b_idx].y > 160) {
                            bullets[b_idx].active = 0;
                        } else {
                            for(int en_idx = 0; en_idx < MAX_ENEMIES; en_idx++) {
                                if(enemies[en_idx].active && enemies[en_idx].type != 8 &&
                                   bullets[b_idx].x < enemies[en_idx].x + 10 && bullets[b_idx].x + ((p_type == 5) ? 6 : 4) > enemies[en_idx].x &&
                                   bullets[b_idx].y < enemies[en_idx].y + 10 && bullets[b_idx].y + ((p_type == 5) ? 6 : 4) > enemies[en_idx].y) {
                                    bullets[b_idx].active = 0;
                                    int dmg = (p_type == 5) ? 5 : 1;
                                    
                                    if (p_type == 5 && enemies[en_idx].hp <= dmg) {
                                        g_achievement_progress[12]++;
                                        if (g_achievement_progress[12] >= 25) UnlockAchievement(ACH_CALIENTE);
                                    }

                                    enemies[en_idx].hp -= dmg;
                                    
                                    if (p_type == 9) {
                                        if (enemies[en_idx].freeze_timer <= 0) {
                                            SPAWN_FTEXT((float)enemies[en_idx].x, (float)enemies[en_idx].y - 6, T(STR_FREEZE), C_ICE);
                                        }
                                        enemies[en_idx].freeze_timer = 90;
                                        g_achievement_progress[ACH_FRIO_FRIO]++; // FRÍO, FRÍO (Índice 15 correcto)
                                        if (g_achievement_progress[15] >= 10) UnlockAchievement(ACH_FRIO_FRIO); 
                                    } else if (p_type == 12) {
                                        if (enemies[en_idx].poison_timer <= 0) {
                                            SPAWN_FTEXT((float)enemies[en_idx].x, (float)enemies[en_idx].y - 6, T(STR_POISON), C_POISON_BULLET);
                                        }
                                        enemies[en_idx].poison_timer = 150;
                                        g_achievement_progress[ACH_TOXICIDAD]++; // TOXICIDAD (Índice 16 correcto)
                                        if (g_achievement_progress[16] >= 10) UnlockAchievement(ACH_TOXICIDAD); 
                                    }

                                    PlaySfx(sndHit);

                                    if (enemies[en_idx].hp <= 0) {
                                        if (p_type == 2) {
                                            g_achievement_progress[ACH_DEMASIADO_RAPIDO]++;
                                            if (g_achievement_progress[ACH_DEMASIADO_RAPIDO] >= 10)
                                                UnlockAchievement(ACH_DEMASIADO_RAPIDO);
                                        }
                                        // TIEMPO MUERTO (Slow-Mo kills) -> Índice 17 correcto
                                        if (slowmo_timer > 0) {
                                            g_achievement_progress[ACH_TIEMPO_MUERTO]++;
                                            if (g_achievement_progress[17] >= 5) UnlockAchievement(ACH_TIEMPO_MUERTO);
                                        }

                                        if (current_difficulty >= 0 && current_difficulty < 4) {
                                            g_enemies_killed_per_diff[current_difficulty]++;
                                            
                                            int base_idx = current_difficulty; 
                                            g_achievement_progress[base_idx]++;
                                            if (!g_achievements_unlocked[base_idx] && g_achievement_progress[base_idx] >= 10) UnlockAchievement(base_idx);

                                            int vet_idx = 4 + current_difficulty; 
                                            g_achievement_progress[vet_idx]++;
                                            if (!g_achievements_unlocked[vet_idx] && g_achievement_progress[vet_idx] >= 100) UnlockAchievement(vet_idx);

                                            int mas_idx = 8 + current_difficulty; 
                                            g_achievement_progress[mas_idx]++;
                                            if (!g_achievements_unlocked[mas_idx] && g_achievement_progress[mas_idx] >= 500) UnlockAchievement(mas_idx);
                                        }

                                        if (enemies[en_idx].type == 3) {
                                            g_achievement_progress[ACH_CAZADOR_JEFES]++;
                                            if (g_achievement_progress[ACH_CAZADOR_JEFES] >= 5) UnlockAchievement(ACH_CAZADOR_JEFES);
                                        }
                                        if (enemies[en_idx].type == 9) {
                                            g_achievement_progress[ACH_TELETRANSP]++;
                                            if (g_achievement_progress[ACH_TELETRANSP] >= 10) UnlockAchievement(ACH_TELETRANSP);
                                        }

                                        enemies[en_idx].active = 0;

                                        for(int ex_slot = 0; ex_slot < MAX_EXPLO; ex_slot++) {
                                            if(!explosions[ex_slot].active) {
                                                explosions[ex_slot].active = 1;
                                                explosions[ex_slot].x = enemies[en_idx].x;
                                                explosions[ex_slot].y = enemies[en_idx].y;
                                                explosions[ex_slot].timer = 30;
                                                break;
                                            }
                                        }
                                        int base_pts = (enemies[en_idx].type == 3) ? 500 : (enemies[en_idx].type == 4 ? 200 : (enemies[en_idx].type == 7 ? 150 : (enemies[en_idx].type == 6 ? 50 : 100)));
                                        int pts = base_pts * combo;
                                        score += pts;

                                        if (player_hp == 1 && score >= 1000) UnlockAchievement(ACH_ULTIMO_ALIENTO);

                                        if (enemies[en_idx].type == 3) screen_shake_timer = 25;

                                        char pts_buf[20];
                                        snprintf(pts_buf, sizeof(pts_buf), "+%d", pts);
                                        SPAWN_FTEXT((float)enemies[en_idx].x, (float)enemies[en_idx].y - 8, pts_buf, C_YELLOW);

                                        if (combo < 5 && p_type != 8 && p_type != 13) combo++;
                                        combo_timer = 300;
                                        PlaySfx(sndExplo);

                                        if (enemies[en_idx].type == 5) {
                                            int spawned_minis = 0;
                                            for(int m_slot = 0; m_slot < MAX_ENEMIES && spawned_minis < 2; m_slot++) {
                                                if (!enemies[m_slot].active) {
                                                    enemies[m_slot].active = 1;
                                                    enemies[m_slot].type = 6;
                                                    enemies[m_slot].hp = 1;
                                                    enemies[m_slot].freeze_timer = 0;
                                                    enemies[m_slot].poison_timer = 0;
                                                    enemies[m_slot].timer = 0;
                                                    enemies[m_slot].x = enemies[en_idx].x + (spawned_minis == 0 ? -14 : 14);
                                                    enemies[m_slot].y = enemies[en_idx].y;
                                                    spawned_minis++;
                                                }
                                            }
                                        }
                                    }
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
                        if (e_bullets[eb_idx].x > 240 || e_bullets[eb_idx].x < 0 || e_bullets[eb_idx].y < 16 || e_bullets[eb_idx].y > 160) {
                            e_bullets[eb_idx].active = 0;
                        } else {
                            if (p_type == 6) {
                                e_bullets[eb_idx].active = 0;
                            } else if (dash_invincibility > 0 && px < e_bullets[eb_idx].x + 4 && px + 10 > e_bullets[eb_idx].x && py < e_bullets[eb_idx].y + 4 && py + 10 > e_bullets[eb_idx].y) {
                                e_bullets[eb_idx].active = 0;
                                g_achievement_progress[ACH_ESQUIVE_PERFECTO]++;
                                if (g_achievement_progress[ACH_ESQUIVE_PERFECTO] >= 10)
                                    UnlockAchievement(ACH_ESQUIVE_PERFECTO);
                            } else if (invincibility == 0 && px < e_bullets[eb_idx].x + 4 && px + 10 > e_bullets[eb_idx].x && py < e_bullets[eb_idx].y + 4 && py + 10 > e_bullets[eb_idx].y) {
                                e_bullets[eb_idx].active = 0;
                                player_took_hit_this_wave = true;
                                hell_no_damage_frames = 0;
                                if (p_type == 1) {
                                    PlaySfx(sndHurt);
                                    p_type = 0; p_timer = 0;
                                    invincibility = 90;
                                } else {
                                    player_hp--;
                                    screen_shake_timer = 15;
                                    if (p_type != 13) { combo = 1; combo_timer = 0; if (p_type == 8) { p_type = 0; p_timer = 0; } }
                                    if (player_hp <= 0) { PlaySfx(sndDeath); state = 8; death_timer = 60; } 
                                    else { PlaySfx(sndHurt); invincibility = 90; }
                                }
                            }
                        }
                    }
                }

                int old_wave = wave;
                wave = 1 + (score / (current_difficulty == 0 ? 4000 : (current_difficulty == 1 ? 3000 : (current_difficulty == 2 ? 2000 : 1200))));
                if (wave > old_wave) {
                    if (!player_took_hit_this_wave) UnlockAchievement(ACH_INTACTO);
                    player_took_hit_this_wave = false;
                }

                if (current_difficulty == 3) {
                    hell_no_damage_frames++;
                    g_achievement_progress[ACH_NO_TOCARME] = hell_no_damage_frames;
                    if (hell_no_damage_frames >= 1800) UnlockAchievement(ACH_NO_TOCARME);
                } else {
                    hell_no_damage_frames = 0;
                    g_achievement_progress[ACH_NO_TOCARME] = 0;
                }

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

                            int is_boss = (wave % 3 == 0) && (rand() % 100 > (current_difficulty == 3 ? 60 : 80));
                            int boss_exists = 0;
                            for(int b_en = 0; b_en < MAX_ENEMIES; b_en++) if (enemies[b_en].type == 3 && enemies[b_en].active) boss_exists = 1;

                            if (is_boss && !boss_exists) {
                                enemies[e_slot].type = 3; enemies[e_slot].hp = (current_difficulty == 3 ? 25 : 15) + wave;
                                enemies[e_slot].x = 110; enemies[e_slot].y = 16;
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
                                enemies[e_slot].target_x = (rand() % 180) + 30;
                                enemies[e_slot].target_y = (rand() % 105) + 25;

                                if (enemies[e_slot].type == 8) {
                                    int edge = rand() % 4;
                                    if (edge == 0) {
                                        enemies[e_slot].x = 0; enemies[e_slot].y = (rand() % 120) + 20;
                                        enemies[e_slot].target_x = 240; enemies[e_slot].target_y = enemies[e_slot].y;
                                    } else if (edge == 1) {
                                        enemies[e_slot].x = 230; enemies[e_slot].y = (rand() % 120) + 20;
                                        enemies[e_slot].target_x = 0; enemies[e_slot].target_y = enemies[e_slot].y;
                                    } else if (edge == 2) {
                                        enemies[e_slot].x = (rand() % 200) + 20; enemies[e_slot].y = 16;
                                        enemies[e_slot].target_x = enemies[e_slot].x; enemies[e_slot].target_y = 150;
                                    } else {
                                        enemies[e_slot].x = (rand() % 200) + 20; enemies[e_slot].y = 150;
                                        enemies[e_slot].target_x = enemies[e_slot].x; enemies[e_slot].target_y = 16;
                                    }
                                } else if (enemies[e_slot].type == 9) {
                                    enemies[e_slot].x = (rand() % 190) + 20;
                                    enemies[e_slot].y = (rand() % 95) + 25;
                                } else {
                                    do {
                                        int side = rand() % 4;
                                        if (side == 0) { enemies[e_slot].x = rand() % 230; enemies[e_slot].y = 16; }
                                        else if (side == 1) { enemies[e_slot].x = 230; enemies[e_slot].y = (rand() % 134) + 16; }
                                        else if (side == 2) { enemies[e_slot].x = rand() % 230; enemies[e_slot].y = 150; }
                                        else { enemies[e_slot].x = 0; enemies[e_slot].y = (rand() % 134) + 16; }
                                    } while((enemies[e_slot].x - px)*(enemies[e_slot].x - px) + (enemies[e_slot].y - py)*(enemies[e_slot].y - py) < 3000);
                                }
                            }
                            break;
                        }
                    }
                }

                for(int en_idx = 0; en_idx < MAX_ENEMIES; en_idx++) {
                    if(enemies[en_idx].active) {
                        if (enemies[en_idx].poison_timer > 0) {
                            enemies[en_idx].poison_timer--;
                            if (frame_count % 30 == 0) {
                                enemies[en_idx].hp--;
                                PlaySfx(sndHit);
                            }
                        }

                        if (enemies[en_idx].hp <= 0) {
                            enemies[en_idx].active = 0;

                            for(int ex_slot = 0; ex_slot < MAX_EXPLO; ex_slot++) {
                                if(!explosions[ex_slot].active) {
                                    explosions[ex_slot].active = 1;
                                    explosions[ex_slot].x = enemies[en_idx].x;
                                    explosions[ex_slot].y = enemies[en_idx].y;
                                    explosions[ex_slot].timer = 30;
                                    break;
                                }
                            }
                            int base_pts = (enemies[en_idx].type == 3) ? 500 : (enemies[en_idx].type == 4 ? 200 : (enemies[en_idx].type == 7 ? 150 : (enemies[en_idx].type == 6 ? 50 : 100)));
                            score += base_pts * combo;
                            if (combo < 5 && p_type != 8 && p_type != 13) combo++;
                            combo_timer = 300;
                            PlaySfx(sndExplo);

                            if (enemies[en_idx].type == 5) {
                                int spawned_minis = 0;
                                for(int m_slot = 0; m_slot < MAX_ENEMIES && spawned_minis < 2; m_slot++) {
                                    if (!enemies[m_slot].active) {
                                        enemies[m_slot].active = 1;
                                        enemies[m_slot].type = 6;
                                        enemies[m_slot].hp = 1;
                                        enemies[m_slot].freeze_timer = 0;
                                        enemies[m_slot].poison_timer = 0;
                                        enemies[m_slot].timer = 0;
                                        enemies[m_slot].x = enemies[en_idx].x + (spawned_minis == 0 ? -14 : 14);
                                        enemies[m_slot].y = enemies[en_idx].y;
                                        spawned_minis++;
                                    }
                                }
                            }
                            continue;
                        }

                        if (enemies[en_idx].freeze_timer > 0) {
                            enemies[en_idx].freeze_timer--;
                        } else {
                            int move_mod = (slowmo_timer > 0) ? 4 : 2;
                            int is_fast = (enemies[en_idx].type == 1 || enemies[en_idx].type == 6);
                            if (is_fast && slowmo_timer == 0) move_mod = 1;
                            else if (is_fast && slowmo_timer > 0) move_mod = 2;

                            if (frame_count % move_mod == 0) {
                                if (enemies[en_idx].type == 8) {
                                    if (enemies[en_idx].x < enemies[en_idx].target_x) enemies[en_idx].x++;
                                    else if (enemies[en_idx].x > enemies[en_idx].target_x) enemies[en_idx].x--;
                                    if (enemies[en_idx].y < enemies[en_idx].target_y) enemies[en_idx].y++;
                                    else if (enemies[en_idx].y > enemies[en_idx].target_y) enemies[en_idx].y--;

                                    if (abs(enemies[en_idx].x - enemies[en_idx].target_x) <= 1 && abs(enemies[en_idx].y - enemies[en_idx].target_y) <= 1) {
                                        enemies[en_idx].active = 0;
                                        continue;
                                    }
                                } else if (enemies[en_idx].type == 9) {
                                    enemies[en_idx].timer++;
                                    if (enemies[en_idx].x < px) enemies[en_idx].x++; else if (enemies[en_idx].x > px) enemies[en_idx].x--;
                                    if (enemies[en_idx].y < py) enemies[en_idx].y++; else if (enemies[en_idx].y > py) enemies[en_idx].y--;

                                    if (enemies[en_idx].timer >= 120) {
                                        enemies[en_idx].timer = 0;
                                        int offset_x_val = (rand() % 81) - 40;
                                        int offset_y_val = (rand() % 81) - 40;
                                        enemies[en_idx].x = px + offset_x_val;
                                        enemies[en_idx].y = py + offset_y_val;
                                        if (enemies[en_idx].x < 4) enemies[en_idx].x = 4;
                                        if (enemies[en_idx].x > 226) enemies[en_idx].x = 226;
                                        if (enemies[en_idx].y < 18) enemies[en_idx].y = 18;
                                        if (enemies[en_idx].y > 146) enemies[en_idx].y = 146;
                                    }
                                } else if (enemies[en_idx].type == 3) {
                                    enemies[en_idx].x += (frame_count / 30 % 2 == 0) ? 1 : -1;
                                    if (enemies[en_idx].y < py) enemies[en_idx].y++;
                                } else if (enemies[en_idx].type == 4) {
                                    if (enemies[en_idx].x < enemies[en_idx].target_x) enemies[en_idx].x++;
                                    else if (enemies[en_idx].x > enemies[en_idx].target_x) enemies[en_idx].x--;
                                    if (enemies[en_idx].y < enemies[en_idx].target_y) enemies[en_idx].y++;
                                    else if (enemies[en_idx].y > enemies[en_idx].target_y) enemies[en_idx].y--;
                                } else {
                                    if (enemies[en_idx].x < px) enemies[en_idx].x++; else if (enemies[en_idx].x > px) enemies[en_idx].x--;
                                    if (enemies[en_idx].y < py) enemies[en_idx].y++; else if (enemies[en_idx].y > py) enemies[en_idx].y--;
                                }
                            }

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

                        if ((invincibility == 0 || p_type == 6) && px < enemies[en_idx].x + 10 && px + 10 > enemies[en_idx].x && py < enemies[en_idx].y + 10 && py + 10 > enemies[en_idx].y) {
                            player_took_hit_this_wave = true;
                            hell_no_damage_frames = 0;
                            if (p_type == 6) {
                                enemies[en_idx].active = 0;
                                for(int ex_slot = 0; ex_slot < MAX_EXPLO; ex_slot++) {
                                    if(!explosions[ex_slot].active) {
                                        explosions[ex_slot].active = 1;
                                        explosions[ex_slot].x = enemies[en_idx].x;
                                        explosions[ex_slot].y = enemies[en_idx].y;
                                        explosions[ex_slot].timer = 30;
                                        break;
                                    }
                                }
                                int base_pts = (enemies[en_idx].type == 3) ? 500 : (enemies[en_idx].type == 4 ? 200 : (enemies[en_idx].type == 7 ? 150 : (enemies[en_idx].type == 6 ? 50 : 100)));
                                int pts = base_pts * combo;
                                score += pts;

                                char pts_buf[20];
                                snprintf(pts_buf, sizeof(pts_buf), "+%d", pts);
                                SPAWN_FTEXT((float)enemies[en_idx].x, (float)enemies[en_idx].y - 8, pts_buf, C_YELLOW);

                                if (combo < 5 && p_type != 8 && p_type != 13) combo++;
                                combo_timer = 300;
                                PlaySfx(sndExplo);
                            } else {
                                if (enemies[en_idx].type == 7) {
                                    enemies[en_idx].active = 0;
                                    for(int ex_slot = 0; ex_slot < MAX_EXPLO; ex_slot++) {
                                        if(!explosions[ex_slot].active) {
                                            explosions[ex_slot].active = 1;
                                            explosions[ex_slot].x = enemies[en_idx].x;
                                            explosions[ex_slot].y = enemies[en_idx].y;
                                            explosions[ex_slot].timer = 30;
                                            break;
                                        }
                                    }
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
                                    } else {
                                        player_hp--;
                                        screen_shake_timer = 15;
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
                                    enemies[en_idx].x -= mx_dir * 15;
                                    enemies[en_idx].y -= my_dir * 15;
                                    if(enemies[en_idx].x < 0) enemies[en_idx].x = 0;
                                    if(enemies[en_idx].x > 230) enemies[en_idx].x = 230;
                                    if(enemies[en_idx].y < 16) enemies[en_idx].y = 16;
                                    if(enemies[en_idx].y > 150) enemies[en_idx].y = 150;
                                }
                            }
                        }
                    }
                }
            }
        }

        // --- RENDERIZADO AL VIRTUAL BUFFER (240x160) ---
        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (state == 14) {
            int alpha = 16;
            if (intro_timer <= 48) alpha = (intro_timer * 16) / 48;
            else if (intro_timer > 170 && intro_timer <= 218) alpha = 16 - (((intro_timer - 170) * 16) / 48);
            else if (intro_timer > 218) alpha = 0;

            if (alpha > 0) {
                if (logoTexture.id > 0) {
                    DrawTexture(logoTexture, 80, 20, (Color){ 255, 255, 255, (unsigned char)(alpha * 15.9f) });
                }
                Color c_brand = BlendColorAlpha(GBA_COLOR(31, 28, 0), alpha);
                Color c_text  = BlendColorAlpha(GBA_COLOR(24, 24, 24), alpha);
                DrawStringCustom("ALEJADEV", 80, 118, c_brand, 1);
                DrawStringCustom(T(STR_PRESENTS), 120, 118, c_text, 1);
            }
        }
        else if (state == 0) {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, C_TITLE_BG);
            for(int s = 0; s < MAX_TITLE_STARS; s++) DrawRectangle(title_stars[s].x, title_stars[s].y, 1, 1, title_stars[s].color);
            DrawAstralliticLogoPC(16, 26);
            DrawRectangle(35, 70, 170, 1, GBA_COLOR(10, 20, 30));
            DrawStringCustom("2026 ALEJADEV", 4, 148, GBA_COLOR(10, 18, 26), 1);
#if defined(PLATFORM_IOS)
            DrawStringCustom("V1.0 - IOS", SCREEN_W - MeasureStringCustom("V1.0 - IOS", 1) - 4, 148, GBA_COLOR(10, 18, 26), 1);
#elif defined(PLATFORM_ANDROID)
            DrawStringCustom("V1.0 - Android", SCREEN_W - MeasureStringCustom("V1.0 - Android", 1) - 4, 148, GBA_COLOR(10, 18, 26), 1);
#else
            DrawStringCustom("V1.2 PC", 188, 148, GBA_COLOR(10, 18, 26), 1);
#endif

            const char* m_options[3] = { T(STR_PLAY), T(STR_OPTIONS), T(STR_RECORDS) };
            for(int opt_idx = 0; opt_idx < 3; opt_idx++) {
                int opt_y = 86 + (opt_idx * 20);
                if(opt_idx == menu_selection) {
                    DrawRectangle(45, opt_y - 3, 150, 15, GBA_COLOR(2, 6, 14));
                    DrawRectangleLines(45, opt_y - 3, 150, 15, GBA_COLOR(0, 31, 31));
                    DrawStringCustom(">", 55, opt_y, C_YELLOW, 1);
                    DrawCenteredStringCustom(m_options[opt_idx], opt_y, C_YELLOW, 1);
                    DrawStringCustom("<", 178, opt_y, C_YELLOW, 1);
                } else {
                    DrawCenteredStringCustom(m_options[opt_idx], opt_y, GBA_COLOR(18, 22, 26), 1);
                }
            }
        }
        else if (state == 13) { 
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, C_TITLE_BG);
            for(int s = 0; s < MAX_TITLE_STARS; s++) DrawRectangle(title_stars[s].x, title_stars[s].y, 1, 1, title_stars[s].color);
            DrawAstralliticLogoPC(16, 26);
            DrawRectangle(35, 70, 170, 1, GBA_COLOR(10, 20, 30));
            DrawStringCustom("2026 ALEJADEV", 4, 148, GBA_COLOR(10, 18, 26), 1);
#if defined(PLATFORM_IOS)
            DrawStringCustom("V1.0 - IOS", SCREEN_W - MeasureStringCustom("V1.0 - IOS", 1) - 4, 148, GBA_COLOR(10, 18, 26), 1);
#elif defined(PLATFORM_ANDROID)
            DrawStringCustom("V1.0 - Android", SCREEN_W - MeasureStringCustom("V1.0 - Android", 1) - 4, 148, GBA_COLOR(10, 18, 26), 1);
#else
            DrawStringCustom("V1.5 PC", 188, 148, GBA_COLOR(10, 18, 26), 1);
#endif

            DrawRectangle(30, 35, 180, 90, GBA_COLOR(2, 3, 6));
            DrawRectangleLines(30, 35, 180, 90, C_CYAN);
            DrawCenteredStringCustom(T(STR_QUIT_PROMPT), 45, GBA_COLOR(31, 28, 0), 1);
            DrawCenteredStringCustom(T(STR_QUIT_SUB), 60, GBA_COLOR(20, 20, 20), 1);
            DrawRectangle(45, 74, 150, 1, GBA_COLOR(10, 16, 22));

            const char* exit_opts[2] = { T(STR_YES), T(STR_NO) };
            Color exit_cols[2] = { GBA_COLOR(0, 31, 10), GBA_COLOR(31, 6, 6) };

            for(int conf_idx = 0; conf_idx < 2; conf_idx++) {
                int opt_y = 86 + (conf_idx * 14);
                if (conf_idx == exit_confirm_selection) {
                    DrawStringCustom(">", 55, opt_y, exit_cols[conf_idx], 1);
                    DrawCenteredStringCustom(exit_opts[conf_idx], opt_y, exit_cols[conf_idx], 1);
                    DrawStringCustom("<", 178, opt_y, exit_cols[conf_idx], 1);
                } else {
                    DrawCenteredStringCustom(exit_opts[conf_idx], opt_y, GBA_COLOR(14, 16, 20), 1);
                }
            }
        }
        else if (state == 5) {
            DrawMenuSpaceFramePC(C_DIFF_BG, T(STR_DIFF_TITLE), C_CYAN, GBA_COLOR(0, 10, 14), GBA_COLOR(0, 24, 31));
            for(int m = 0; m < MAX_MENU_STARS; m++) DrawRectangle(menu_stars[m].x, menu_stars[m].y, 1, 1, menu_stars[m].color);
            const char* diff_names[4] = { T(STR_DIFF_EASY), T(STR_DIFF_NORMAL), T(STR_DIFF_HARD), T(STR_DIFF_HELL) };
            Color diff_colors[4] = { GBA_COLOR(0, 31, 10), C_CYAN, GBA_COLOR(31, 18, 0), GBA_COLOR(31, 5, 5) };
            for(int diff_idx = 0; diff_idx < 4; diff_idx++) {
                int opt_y = 54 + (diff_idx * 20);
                if (diff_idx == diff_selection) {
                    DrawRectangle(45, opt_y - 3, 150, 15, GBA_COLOR(3, 8, 16));
                    DrawRectangleLines(45, opt_y - 3, 150, 15, diff_colors[diff_idx]);
                    DrawStringCustom(">", 55, opt_y, diff_colors[diff_idx], 1);
                    DrawCenteredStringCustom(diff_names[diff_idx], opt_y, diff_colors[diff_idx], 1);
                    DrawStringCustom("<", 178, opt_y, diff_colors[diff_idx], 1);
                } else {
                    DrawCenteredStringCustom(diff_names[diff_idx], opt_y, GBA_COLOR(16, 20, 24), 1);
                }
            }
        }
        else if (state == 6 || state == 15) {
            DrawMenuSpaceFramePC(C_REC_BG, "", GBA_COLOR(31, 28, 0), GBA_COLOR(10, 8, 0), GBA_COLOR(0, 28, 31));
            for(int m = 0; m < MAX_MENU_STARS; m++) DrawRectangle(menu_stars[m].x, menu_stars[m].y, 1, 1, menu_stars[m].color);

            bool is_rec = (state == 6);
            
            DrawStringCustom(T(STR_RECORDS), 30, 16, is_rec ? C_YELLOW : GBA_COLOR(12, 16, 20), 2);
            DrawStringCustom(T(STR_LOGROS), 135, 16, !is_rec ? C_GREEN : GBA_COLOR(12, 16, 20), 2);
            DrawRectangle(15, 30, 210, 1, GBA_COLOR(0, 24, 31));

            if (state == 6) {
                const char* diff_labels[4] = { T(STR_DIFF_EASY), T(STR_DIFF_NORMAL), T(STR_DIFF_HARD), T(STR_DIFF_HELL) };
                Color rank_colors[4] = { GBA_COLOR(0, 31, 10), C_CYAN, GBA_COLOR(31, 18, 0), GBA_COLOR(31, 6, 6) };
                for(int r_idx = 0; r_idx < 4; r_idx++) {
                    int row_y = 48 + (r_idx * 24);
                    DrawRectangle(35, row_y - 2, 170, 19, GBA_COLOR(1, 6, 12));
                    DrawRectangleLines(35, row_y - 2, 170, 19, GBA_COLOR(0, 20, 26));
                    DrawStringCustom(diff_labels[r_idx], 45, row_y + 3, rank_colors[r_idx], 1);
                    DrawStringCustom(":", 100, row_y + 3, GBA_COLOR(16, 20, 24), 1);
                    DrawScoreCustom(loadHighScorePC(r_idx), 125, row_y + 1, GBA_COLOR(31, 28, 0));
                }
            } else {
                StringId ach_titles[30] = { 
                    STR_ACH_1_TITLE, STR_ACH_2_TITLE, STR_ACH_3_TITLE, STR_ACH_4_TITLE, 
                    STR_ACH_5_TITLE, STR_ACH_6_TITLE, STR_ACH_7_TITLE, STR_ACH_8_TITLE, 
                    STR_ACH_9_TITLE, STR_ACH_10_TITLE, STR_ACH_11_TITLE, STR_ACH_12_TITLE, 
                    STR_ACH_13_TITLE, STR_ACH_14_TITLE, STR_ACH_15_TITLE, STR_ACH_16_TITLE,
                    STR_ACH_17_TITLE, STR_ACH_18_TITLE, STR_ACH_19_TITLE, STR_ACH_20_TITLE,
                    STR_ACH_21_TITLE, STR_ACH_22_TITLE, STR_ACH_23_TITLE, STR_ACH_24_TITLE,
                    STR_ACH_25_TITLE, STR_ACH_26_TITLE, STR_ACH_27_TITLE, STR_ACH_28_TITLE,
                    STR_ACH_29_TITLE, STR_ACH_30_TITLE
                };
                StringId ach_descs[30] = { 
                    STR_ACH_1_DESC, STR_ACH_2_DESC, STR_ACH_3_DESC, STR_ACH_4_DESC, 
                    STR_ACH_5_DESC, STR_ACH_6_DESC, STR_ACH_7_DESC, STR_ACH_8_DESC, 
                    STR_ACH_9_DESC, STR_ACH_10_DESC, STR_ACH_11_DESC, STR_ACH_12_DESC, 
                    STR_ACH_13_DESC, STR_ACH_14_DESC, STR_ACH_15_DESC, STR_ACH_16_DESC,
                    STR_ACH_17_DESC, STR_ACH_18_DESC, STR_ACH_19_DESC, STR_ACH_20_DESC,
                    STR_ACH_21_DESC, STR_ACH_22_DESC, STR_ACH_23_DESC, STR_ACH_24_DESC,
                    STR_ACH_25_DESC, STR_ACH_26_DESC, STR_ACH_27_DESC, STR_ACH_28_DESC,
                    STR_ACH_29_DESC, STR_ACH_30_DESC
                };

                for (int r = 0; r < 2; r++) {
                    int row_idx = ach_scroll_row + r;
                    if (row_idx >= 8) break;
                    for (int col = 0; col < 4; col++) {
                        int ach_idx = row_idx * 4 + col;
                        if (ach_idx >= 30) break;

                        int bx = 16 + (col * 48);
                        int by = 38 + (r * 44);
                        int bw = 40;
                        int bh = 38;
                        bool is_unlocked = g_achievements_unlocked[ach_idx];
                        bool is_selected = (selected_ach_index == ach_idx);

                        DrawRectangle(bx, by, bw, bh, GBA_COLOR(2, 6, 12));
                        DrawRectangleLines(bx, by, bw, bh, is_selected ? C_YELLOW : (is_unlocked ? C_GREEN : GBA_COLOR(10, 14, 18)));

                        if (!is_unlocked) {
                            DrawRectangle(bx + 16, by + 15, 8, 7, WHITE);
                            DrawRectangle(bx + 18, by + 17, 4, 3, GBA_COLOR(2, 6, 12));
                            DrawRectangle(bx + 17, by + 11, 6, 5, GBA_COLOR(18, 18, 18));
                            DrawRectangle(bx + 18, by + 12, 4, 4, GBA_COLOR(2, 6, 12));
                        } else {
                            Enemy preview_enemy = { .x = bx + 15, .y = by + 14, .type = ach_idx % 7, .active = 1, .hp = 1 };
                            DrawEnemyPC(&preview_enemy, frame_count);
                        }
                    }
                }

                // Triángulos de navegación de Logros en lateral derecho (x: 224)
                bool can_ach_up = (ach_scroll_row > 0);
                Color ach_up_col = can_ach_up ? C_YELLOW : GBA_COLOR(6, 8, 12);
                Color ach_up_border = can_ach_up ? WHITE : GBA_COLOR(10, 14, 18);
                Vector2 a1_up = { 224, 52 };
                Vector2 a2_up = { 213, 72 };
                Vector2 a3_up = { 235, 72 };
                DrawTriangle(a1_up, a2_up, a3_up, ach_up_col);
                DrawTriangleLines(a1_up, a2_up, a3_up, ach_up_border);

                bool can_ach_dn = (ach_scroll_row < 6);
                Color ach_dn_col = can_ach_dn ? C_YELLOW : GBA_COLOR(6, 8, 12);
                Color ach_dn_border = can_ach_dn ? WHITE : GBA_COLOR(10, 14, 18);
                Vector2 a1_dn = { 213, 106 };
                Vector2 a2_dn = { 235, 106 };
                Vector2 a3_dn = { 224, 126 };
                DrawTriangle(a1_dn, a2_dn, a3_dn, ach_dn_col);
                DrawTriangleLines(a1_dn, a2_dn, a3_dn, ach_dn_border);

                // Pop-up flotante grande superpuesto abajo
                if (show_ach_details) {
                    DrawRectangle(12, 70, 200, 68, GBA_COLOR(1, 4, 8));
                    DrawRectangleLines(12, 70, 200, 68, g_achievements_unlocked[selected_ach_index] ? C_GREEN : C_CYAN);
                    DrawStringCustom(T(ach_titles[selected_ach_index]), 18, 76, g_achievements_unlocked[selected_ach_index] ? C_GREEN : C_YELLOW, 1);
                    DrawStringCustom(T(ach_descs[selected_ach_index]), 18, 90, WHITE, 1);

                    char prog_buf[48];
                    if (g_achievements_unlocked[selected_ach_index]) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s", T(STR_ACH_STATUS_UNLOCKED));
                    } else if (selected_ach_index == ACH_FULL_ARSENAL) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/6", T(STR_ACH_PROGRESS_LBL), CountAchievementBits(g_achievement_progress[ACH_FULL_ARSENAL]));
                    } else if (selected_ach_index == ACH_LEYENDA) {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d/%d", T(STR_ACH_PROGRESS_LBL), g_achievement_progress[ACH_LEYENDA], ACH_LEYENDA);
                    } else {
                        snprintf(prog_buf, sizeof(prog_buf), "%s: %d", T(STR_ACH_PROGRESS_LBL), g_achievement_progress[selected_ach_index]);
                    }
                    DrawStringCustom(prog_buf, 18, 106, g_achievements_unlocked[selected_ach_index] ? C_GREEN : GBA_COLOR(31, 10, 10), 1);
                    DrawCenteredStringCustom(T(STR_TOUCH_BACK), 126, GBA_COLOR(10, 18, 24), 1);
                } else {
                    DrawCenteredStringCustom(T(STR_ACH_HINT), 136, GBA_COLOR(10, 14, 18), 1);
                }
            }
        }
        else if (state == 7) {
            DrawMenuSpaceFramePC(C_OPT_BG, T(STR_OPTIONS), GBA_COLOR(31, 28, 0), GBA_COLOR(12, 10, 0), GBA_COLOR(20, 10, 31));
            for(int m = 0; m < MAX_MENU_STARS; m++) DrawRectangle(menu_stars[m].x, menu_stars[m].y, 1, 1, menu_stars[m].color);

            const char* filter_txt[3] = { "OFF", "SCANLINES", "CRT ARCADE" };
            const char* fps_txt[2] = { "30 FPS", "60 FPS" };

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            char opt_strings[7][32];
            snprintf(opt_strings[0], 32, "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(opt_strings[1], 32, "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(opt_strings[2], 32, "%s: < %s >", T(STR_FILTER), filter_txt[g_config.crt_filter]);
            snprintf(opt_strings[3], 32, "%s", T(STR_CONTROLS));
            snprintf(opt_strings[4], 32, "%s: < %s >", T(STR_LANGUAGE), g_lang_names[g_config.language]);
            snprintf(opt_strings[5], 32, "%s: < %s >", T(STR_FPS), fps_txt[g_config.target_fps]);
            snprintf(opt_strings[6], 32, "%s", T(STR_DELETE_RECORDS));

            int total_opts = 7;
            int max_scroll = 4;
            int lang_idx = 4;
#else
            const char* res_txt[4] = { "960X640", "1200X800", "1440X960", "1920X1080" };
            const char* mode_txt[2] = { "VENTANA", "SIN BORDES" };
            const char* mode_txt_en[2] = { "WINDOWED", "BORDERLESS" };

            char opt_strings[10][32];
            snprintf(opt_strings[0], 32, "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(opt_strings[1], 32, "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(opt_strings[2], 32, "%s: < %s >", T(STR_FILTER), filter_txt[g_config.crt_filter]);
            snprintf(opt_strings[3], 32, "%s", T(STR_CONTROLS));
            snprintf(opt_strings[4], 32, "%s: < %s >", T(STR_LANGUAGE), g_lang_names[g_config.language]);
            snprintf(opt_strings[5], 32, "%s: < %s >", T(STR_RESOLUTION), res_txt[g_config.res_index]);
            snprintf(opt_strings[6], 32, "%s: < %s >", T(STR_SCREEN_MODE), (g_config.language == 0) ? mode_txt[g_config.screen_mode] : mode_txt_en[g_config.screen_mode]);
            snprintf(opt_strings[7], 32, "%s: < %s >", T(STR_FPS), fps_txt[g_config.target_fps]);
            snprintf(opt_strings[8], 32, "%s: %s", T(STR_VSYNC), g_config.vsync ? "ON" : "OFF");
            snprintf(opt_strings[9], 32, "%s", T(STR_DELETE_RECORDS));

            int total_opts = 10;
            int max_scroll = 7;
            int lang_idx = 4;
#endif

            for (int slot = 0; slot < 3; slot++) {
                int opt_idx = options_scroll_offset + slot;
                if (opt_idx >= total_opts) break;
                int row_y = 52 + (slot * 27);
                bool is_sel = (options_selection == opt_idx);

                if (is_sel) {
                    DrawRectangle(25, row_y - 3, 175, 22, GBA_COLOR(6, 4, 14));
                    DrawRectangleLines(25, row_y - 3, 175, 22, GBA_COLOR(31, 24, 0));
                    DrawStringCustom(">", 32, row_y + 4, C_YELLOW, 1);
                    DrawCenteredStringCustom(opt_strings[opt_idx], row_y + 4, C_YELLOW, 1);
                    DrawStringCustom("<", 192, row_y + 4, C_YELLOW, 1);
                } else {
                    DrawCenteredStringCustom(opt_strings[opt_idx], row_y + 4, GBA_COLOR(18, 16, 24), 1);
                }

                if (opt_idx == lang_idx) {
                    DrawLanguageFlag16Bit(g_config.language, 170, row_y + 3);
                }
            }

            // Botones triangulares arriba y abajo en el lateral derecho
            bool can_scroll_up = (options_selection > 0);
            Color up_col = can_scroll_up ? C_YELLOW : GBA_COLOR(6, 8, 12);
            Color up_border = can_scroll_up ? WHITE : GBA_COLOR(10, 14, 18);
            Vector2 p1_up = { 224, 52 };
            Vector2 p2_up = { 213, 72 };
            Vector2 p3_up = { 235, 72 };
            DrawTriangle(p1_up, p2_up, p3_up, up_col);
            DrawTriangleLines(p1_up, p2_up, p3_up, up_border);

            bool can_scroll_down = (options_selection < total_opts - 1);
            Color down_col = can_scroll_down ? C_YELLOW : GBA_COLOR(6, 8, 12);
            Color down_border = can_scroll_down ? WHITE : GBA_COLOR(10, 14, 18);
            Vector2 p1_dn = { 213, 106 };
            Vector2 p2_dn = { 235, 106 };
            Vector2 p3_dn = { 224, 126 };
            DrawTriangle(p1_dn, p2_dn, p3_dn, down_col);
            DrawTriangleLines(p1_dn, p2_dn, p3_dn, down_border);
        }
        else if (state == 12) {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, C_PAUSE_OPT_BG);

            for (int m = 0; m < MAX_MENU_STARS; m++) {
                DrawRectangle(menu_stars[m].x, menu_stars[m].y, 1, 1, menu_stars[m].color);
            }

            DrawStringCustom(T(STR_OPTIONS), 82, 16, C_CYAN, 2);
            DrawRectangle(28, 47, 184, 1, GBA_COLOR(0, 24, 31));

            const char* filter_txt[3] = { "OFF", "SCANLINES", "CRT ARCADE" };
            const char* fps_txt[2] = { "30 FPS", "60 FPS" };

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            char pause_opt_strings[5][64];
            snprintf(pause_opt_strings[0], sizeof(pause_opt_strings[0]), "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(pause_opt_strings[1], sizeof(pause_opt_strings[1]), "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(pause_opt_strings[2], sizeof(pause_opt_strings[2]), "%s: < %s >", T(STR_FILTER), filter_txt[g_config.crt_filter]);
            snprintf(pause_opt_strings[3], sizeof(pause_opt_strings[3]), "%s", T(STR_CONTROLS));
            snprintf(pause_opt_strings[4], sizeof(pause_opt_strings[4]), "%s: < %s >", T(STR_FPS), fps_txt[g_config.target_fps]);

            int total_pause_opts = 5;
            int max_pause_scroll = 2;
#else
            const char* res_txt[4] = { "960X640", "1200X800", "1440X960", "1920X1080" };
            const char* mode_txt[2] = { "VENTANA", "SIN BORDES" };
            const char* mode_txt_en[2] = { "WINDOWED", "BORDERLESS" };

            char pause_opt_strings[8][64];
            snprintf(pause_opt_strings[0], sizeof(pause_opt_strings[0]), "%s: < %d%% >", T(STR_VOL_BGM), g_config.vol_bgm * 10);
            snprintf(pause_opt_strings[1], sizeof(pause_opt_strings[1]), "%s: < %d%% >", T(STR_VOL_SFX), g_config.vol_sfx * 10);
            snprintf(pause_opt_strings[2], sizeof(pause_opt_strings[2]), "%s: < %s >", T(STR_FILTER), filter_txt[g_config.crt_filter]);
            snprintf(pause_opt_strings[3], sizeof(pause_opt_strings[3]), "%s", T(STR_CONTROLS));
            snprintf(pause_opt_strings[4], sizeof(pause_opt_strings[4]), "%s: < %s >", T(STR_RESOLUTION), res_txt[g_config.res_index]);
            snprintf(pause_opt_strings[5], sizeof(pause_opt_strings[5]), "%s: < %s >", T(STR_SCREEN_MODE), (g_config.language == 0) ? mode_txt[g_config.screen_mode] : mode_txt_en[g_config.screen_mode]);
            snprintf(pause_opt_strings[6], sizeof(pause_opt_strings[6]), "%s: < %s >", T(STR_FPS), fps_txt[g_config.target_fps]);
            snprintf(pause_opt_strings[7], sizeof(pause_opt_strings[7]), "%s: %s", T(STR_VSYNC), g_config.vsync ? "ON" : "OFF");

            int total_pause_opts = 8;
            int max_pause_scroll = 5;
#endif

            for (int slot = 0; slot < 3; slot++) {
                int opt_idx = pause_options_scroll_offset + slot;
                if (opt_idx >= total_pause_opts) break;

                int row_y = 55 + (slot * 27);
                bool is_sel = (pause_options_selection == opt_idx);

                DrawRectangle(25, row_y - 4, 175, 22,
                              is_sel ? GBA_COLOR(2, 10, 18) : GBA_COLOR(1, 5, 11));
                DrawRectangleLines(25, row_y - 4, 175, 22,
                                   is_sel ? C_CYAN : GBA_COLOR(0, 16, 24));

                if (is_sel) {
                    DrawStringCustom(">", 32, row_y + 4, C_YELLOW, 1);
                }

                DrawCenteredStringCustom(pause_opt_strings[opt_idx], row_y + 4,
                                         is_sel ? C_YELLOW : GBA_COLOR(14, 22, 28), 1);

#if !defined(PLATFORM_ANDROID) && !defined(PLATFORM_IOS)
                if (opt_idx != 3) {
                    DrawStringCustom("<", 192, row_y + 4,
                                     is_sel ? C_YELLOW : GBA_COLOR(8, 18, 24), 1);
                }
#else
                DrawStringCustom("<", 192, row_y + 4,
                                 is_sel ? C_YELLOW : GBA_COLOR(8, 18, 24), 1);
#endif
            }

            // Botones triangulares arriba y abajo en el lateral derecho
            bool can_pause_up = (pause_options_selection > 0);
            Color p_up_col = can_pause_up ? C_CYAN : GBA_COLOR(4, 8, 12);
            Color p_up_border = can_pause_up ? WHITE : GBA_COLOR(8, 14, 18);
            Vector2 p1_pup = { 224, 52 };
            Vector2 p2_pup = { 213, 72 };
            Vector2 p3_pup = { 235, 72 };
            DrawTriangle(p1_pup, p2_pup, p3_pup, p_up_col);
            DrawTriangleLines(p1_pup, p2_pup, p3_pup, p_up_border);

            bool can_pause_dn = (pause_options_selection < total_pause_opts - 1);
            Color p_dn_col = can_pause_dn ? C_CYAN : GBA_COLOR(4, 8, 12);
            Color p_dn_border = can_pause_dn ? WHITE : GBA_COLOR(8, 14, 18);
            Vector2 p1_pdn = { 213, 106 };
            Vector2 p2_pdn = { 235, 106 };
            Vector2 p3_pdn = { 224, 126 };
            DrawTriangle(p1_pdn, p2_pdn, p3_pdn, p_dn_col);
            DrawTriangleLines(p1_pdn, p2_pdn, p3_pdn, p_dn_border);

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
            DrawCenteredStringCustom(T(STR_TOUCH_BACK), 143, GBA_COLOR(14, 22, 28), 1);
#else
            DrawCenteredStringCustom(T(STR_CANCEL_KEY), 143, GBA_COLOR(8, 18, 24), 1);
#endif
        }
        else if (state == 10) {
            DrawMenuSpaceFramePC(C_OPT_BG, T(STR_OPTIONS), GBA_COLOR(31, 28, 0), GBA_COLOR(12, 10, 0), GBA_COLOR(20, 10, 31));
            DrawRectangle(30, 35, 180, 90, GBA_COLOR(2, 3, 6));
            DrawRectangleLines(30, 35, 180, 90, C_CYAN);
            DrawCenteredStringCustom(T(STR_CONFIRM_DELETE_TITLE), 45, GBA_COLOR(31, 28, 0), 1);
            DrawCenteredStringCustom(T(STR_CONFIRM_DELETE_SUB), 60, GBA_COLOR(20, 10, 10), 1);
            DrawRectangle(45, 74, 150, 1, GBA_COLOR(10, 16, 22));
            const char* conf_opts[2] = { T(STR_CANCEL), T(STR_CONFIRM_DELETE_BTN) };
            Color conf_cols[2] = { GBA_COLOR(0, 31, 10), GBA_COLOR(31, 6, 6) };
            for(int c_idx = 0; c_idx < 2; c_idx++) {
                int opt_y = 86 + (c_idx * 14);
                if (c_idx == confirm_selection) {
                    DrawStringCustom(">", 55, opt_y, conf_cols[c_idx], 1);
                    DrawCenteredStringCustom(conf_opts[c_idx], opt_y, conf_cols[c_idx], 1);
                    DrawStringCustom("<", 178, opt_y, conf_cols[c_idx], 1);
                } else {
                    DrawCenteredStringCustom(conf_opts[c_idx], opt_y, GBA_COLOR(14, 16, 20), 1);
                }
            }
        }
        else if (state == 4) {
            if (g_last_input_device != INPUT_GAMEPAD) {
                // Pestaña rediseñada: Dibujo esquemático de un móvil en horizontal con controles táctiles
                DrawMenuSpaceFramePC(C_CTRL_BG, T(STR_CONTROLS), GBA_COLOR(0, 31, 10), GBA_COLOR(0, 10, 4), GBA_COLOR(0, 26, 10));
                for(int m = 0; m < MAX_MENU_STARS; m++) DrawRectangle(menu_stars[m].x, menu_stars[m].y, 1, 1, menu_stars[m].color);

                // Chasis exterior del teléfono móvil en horizontal (desplazado ligeramente hacia abajo)
                DrawRectangle(22, 38, 196, 100, GBA_COLOR(2, 4, 8));
                DrawRectangleLines(22, 38, 196, 100, GBA_COLOR(0, 26, 31));
                // Pantalla interior
                DrawRectangle(34, 42, 172, 92, GBA_COLOR(1, 2, 5));
                DrawRectangleLines(34, 42, 172, 92, GBA_COLOR(0, 16, 22));

                // Cámara frontal (izq) y altavoz (der)
                DrawCircle(28, 88, 2, GBA_COLOR(8, 12, 16));
                DrawRectangle(212, 82, 2, 12, GBA_COLOR(8, 12, 16));

                // --- LADO IZQUIERDO: JOYSTICK VIRTUAL FLOTANTE (ESTILO CUADRADO 16-BIT) ---
                DrawRectangle(46, 64, 40, 40, GBA_COLOR(1, 4, 10));
                DrawRectangleLines(46, 64, 40, 40, GBA_COLOR(0, 24, 31));
                DrawRectangle(64, 66, 4, 3, WHITE);
                DrawRectangle(64, 99, 4, 3, WHITE);
                DrawRectangle(48, 82, 3, 4, WHITE);
                DrawRectangle(81, 82, 3, 4, WHITE);
                DrawRectangle(57, 75, 18, 18, C_CYAN);
                DrawRectangleLines(57, 75, 18, 18, WHITE);
                DrawRectangle(63, 81, 6, 6, WHITE);
                int move_w = MeasureStringCustom(T(STR_CTRL_MOVE), 1);
                DrawStringCustom(T(STR_CTRL_MOVE), 66 - (move_w / 2), 112, C_CYAN, 1);

                // --- LADO DERECHO: BOTONES TÁCTILES ---
                // Botón Pausa [II]
                DrawCircle(192, 53, 7, GBA_COLOR(8, 10, 14));
                DrawCircleLines(192, 53, 7, WHITE);
                DrawStringCustom("II", 190, 50, WHITE, 1);
                DrawStringCustom(T(STR_PAUSE), 154, 50, GBA_COLOR(18, 22, 26), 1);

                // Botón Aim Lock [L]
                DrawCircle(144, 73, 7, GBA_COLOR(4, 22, 18));
                DrawCircleLines(144, 73, 7, C_GREEN);
                DrawStringCustom("L", 142, 70, C_GREEN, 1);
                DrawStringCustom(T(STR_CTRL_AIM), 108, 70, C_GREEN, 1);

                // Botón Dash [D]
                DrawCircle(144, 97, 7, GBA_COLOR(6, 14, 28));
                DrawCircleLines(144, 97, 7, C_CYAN);
                DrawStringCustom("D", 142, 94, C_CYAN, 1);
                DrawStringCustom(T(STR_CTRL_DASH), 114, 94, C_CYAN, 1);

                // Botón Disparo [A]
                DrawCircle(188, 77, 9, GBA_COLOR(24, 6, 6));
                DrawCircleLines(188, 77, 9, GBA_COLOR(31, 12, 12));
                DrawStringCustom("A", 186, 74, WHITE, 1);
                DrawStringCustom(T(STR_CTRL_SHOOT), 162, 89, GBA_COLOR(31, 14, 14), 1);

                // Botón Turbo [T]
                DrawCircle(188, 109, 7, GBA_COLOR(26, 22, 4));
                DrawCircleLines(188, 109, 7, C_YELLOW);
                DrawStringCustom("T", 186, 106, C_YELLOW, 1);
                DrawStringCustom(T(STR_CTRL_TURBO), 158, 119, C_YELLOW, 1);
            } else {
                DrawMenuSpaceFramePC(C_CTRL_BG, T(STR_CTRL_TITLE), GBA_COLOR(0, 31, 10), GBA_COLOR(0, 10, 4), GBA_COLOR(0, 26, 10));
                for(int m = 0; m < MAX_MENU_STARS; m++) DrawRectangle(menu_stars[m].x, menu_stars[m].y, 1, 1, menu_stars[m].color);

                DrawRectangle(15, 46, 210, 96, GBA_COLOR(1, 4, 2));
                DrawRectangle(15, 46, 210, 1, GBA_COLOR(0, 20, 8));
                DrawRectangle(15, 141, 210, 1, GBA_COLOR(0, 20, 8));

                const char* action_labels[8] = {
                    T(STR_CTRL_UP), T(STR_CTRL_DOWN), T(STR_CTRL_LEFT), T(STR_CTRL_RIGHT),
                    T(STR_CTRL_SHOOT), T(STR_CTRL_TURBO), T(STR_CTRL_AIM), T(STR_CTRL_DASH)
                };
                int action_pads[8] = {
                    g_pad.btn_up, g_pad.btn_down, g_pad.btn_left, g_pad.btn_right,
                    g_pad.btn_shoot, g_pad.btn_turbo, g_pad.btn_aim, g_pad.btn_dash
                };

                for (int act_idx = 0; act_idx < 8; act_idx++) {
                    int row_y = 51 + (act_idx * 11);
                    bool is_sel = (controls_selection == act_idx);

                    if (is_sel) {
                        DrawRectangle(17, row_y - 2, 206, 10, GBA_COLOR(3, 8, 16));
                        DrawRectangleLines(17, row_y - 2, 206, 10, GBA_COLOR(0, 31, 31));
                    }

                    DrawActionIcon16Bit(act_idx, 20, row_y - 1, is_sel);
                    DrawStringCustom(action_labels[act_idx], 32, row_y, is_sel ? C_YELLOW : WHITE, 1);
                    
                    const char* bind_name = GetGamepadButtonNameCustom(action_pads[act_idx]);
                    Color btn_col = is_sel ? C_YELLOW : GetGamepadButtonColorCustom(action_pads[act_idx]);
                    DrawStringCustom(bind_name, 150, row_y, btn_col, 1);
                    DrawGamepadIcon16Bit(202, row_y - 2, is_sel);
                }

                if (rebinding_action != -1) {
                    DrawRectangle(15, 45, 210, 70, GBA_COLOR(2, 3, 6));
                    DrawRectangleLines(15, 45, 210, 70, C_YELLOW);
                    DrawCenteredStringCustom(T(STR_REBIND_TITLE), 54, C_YELLOW, 1);
                    DrawCenteredStringCustom(action_labels[rebinding_action], 68, C_CYAN, 1);
                    DrawCenteredStringCustom(T(STR_PRESS_KEY), 82, WHITE, 1);
                    DrawCenteredStringCustom(T(STR_CANCEL_KEY), 96, GBA_COLOR(18, 18, 18), 1);
                }

                if (duplicate_key_warning > 0) {
                    DrawRectangle(30, 48, 180, 64, GBA_COLOR(4, 2, 8));
                    DrawRectangleLines(30, 48, 180, 64, GBA_COLOR(31, 2, 2));
                    DrawCenteredStringCustom(T(STR_INVALID_ACTION), 56, GBA_COLOR(31, 6, 6), 1);
                    DrawCenteredStringCustom(T(STR_DUPLICATE_KEY), 72, C_YELLOW, 1);
                    DrawCenteredStringCustom(T(STR_USE_OTHER_KEY), 88, WHITE, 1);
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
                for(int g = 0; g < MAX_GAME_STARS; g++) DrawRectangle(game_stars[g].x, game_stars[g].y, 1, 1, game_stars[g].color);
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
                    int bx = bullets[b_idx].x;
                    int by = bullets[b_idx].y;
                    int vx = bullets[b_idx].vx;
                    int vy = bullets[b_idx].vy;
                    
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

            for(int eb_idx = 0; eb_idx < MAX_E_BULLETS; eb_idx++) {
                if (e_bullets[eb_idx].active) DrawRectangle(e_bullets[eb_idx].x, e_bullets[eb_idx].y, 4, 4, C_E_BULLET);
            }

            for(int en_idx = 0; en_idx < MAX_ENEMIES; en_idx++) {
                if (enemies[en_idx].active) DrawEnemyPC(&enemies[en_idx], frame_count);
            }

            for (int ft_idx = 0; ft_idx < MAX_TEXTS; ft_idx++) {
                if (ftexts[ft_idx].timer > 0) {
                    DrawStringCustom(ftexts[ft_idx].text, (int)ftexts[ft_idx].x, (int)ftexts[ft_idx].y, ftexts[ft_idx].color, 1);
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
            else p_color = C_PLAYER;

            if((invincibility > 0 || p_type == 6) && (invincibility % 10 < 5 || p_type == 6)) {
                p_color = WHITE;
            }
            DrawPlayerShipPC(px, py, dir_x, dir_y, p_color);

            DrawRectangle(0, 0, SCREEN_W, 16, GBA_COLOR(2, 4, 8));
            DrawRectangle(0, 15, SCREEN_W, 1, GBA_COLOR(0, 24, 31));
            for(int h = 0; h < player_hp; h++) DrawHeartCustom(5 + (h * 12), 3, C_HEART);
            DrawStringCustom(T(STR_WAVE), 64, 4, GBA_COLOR(14, 20, 26), 1);
            DrawScoreCustom(wave, 94, 3, C_YELLOW);
            Color combo_col = (combo >= 5) ? C_YELLOW : C_CYAN;
            DrawStringCustom("X", 140, 4, combo_col, 1);
            DrawScoreCustom(combo, 148, 3, combo_col);
            DrawScoreCustom(score, 178, 3, WHITE);

            if (state == 9) {
                if (ready_timer > 60) {
                    if ((ready_timer / 15) % 2 == 0) DrawCenteredStringCustom(T(STR_READY), 105, C_YELLOW, 2);
                } else {
                    DrawCenteredStringCustom(T(STR_GO), 105, C_GREEN, 2);
                }
            }
            if (state == 3) {
                DrawRectangle(35, 19, 170, 130, GBA_COLOR(1, 3, 7));
                DrawRectangleLines(35, 19, 170, 130, C_CYAN);
                DrawHeaderCenteredStringCustom(T(STR_PAUSE), 28, C_CYAN, GBA_COLOR(0, 10, 14), 2);
                DrawRectangle(45, 46, 150, 1, GBA_COLOR(0, 16, 20));

                const char* p_opts[3] = { T(STR_RESUME), T(STR_OPTIONS), T(STR_MAIN_MENU) };
                for(int p_idx = 0; p_idx < 3; p_idx++) {
                    int py_pos = 58 + (p_idx * 27);
                    if (p_idx == pause_selection) {
                        DrawRectangle(42, py_pos - 3, 156, 16, GBA_COLOR(4, 10, 20));
                        DrawRectangleLines(42, py_pos - 3, 156, 16, C_YELLOW);
                        DrawStringCustom(">", 52, py_pos, C_YELLOW, 1);
                        Color opt_col = (p_idx == 2) ? C_CYAN : C_YELLOW;
                        DrawCenteredStringCustom(p_opts[p_idx], py_pos, opt_col, 1);
                        DrawStringCustom("<", 176, py_pos, C_YELLOW, 1);
                    } else {
                        Color opt_col = (p_idx == 2) ? C_CYAN : GBA_COLOR(16, 20, 24);
                        DrawCenteredStringCustom(p_opts[p_idx], py_pos, opt_col, 1);
                    }
                }
            }
        }
        else if (state == 2) {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, C_RED);
            for(int b = 0; b < MAX_BLOOD_DROPS; b++) DrawRectangle(blood_drops[b].x, blood_drops[b].y, 1, 1, blood_drops[b].color);
            DrawHeaderCenteredStringCustom(T(STR_GAME_OVER), 18, WHITE, GBA_COLOR(4, 0, 0), 3);
            DrawSkullIconPC(114, 46);
            DrawCenteredStringCustom(T(STR_POINTS), 64, GBA_COLOR(24, 18, 18), 1);
            DrawScoreCustom(score, 110, 76, WHITE);
            const char* go_opts[2] = { T(STR_RETRY), T(STR_MAIN_MENU) };
            for(int go_idx = 0; go_idx < 2; go_idx++) {
                int opt_y = 112 + (go_idx * 18);
                if (go_idx == gameover_selection) {
                    DrawRectangle(45, opt_y - 3, 150, 14, GBA_COLOR(20, 2, 4));
                    DrawRectangleLines(45, opt_y - 3, 150, 14, C_YELLOW);
                    DrawStringCustom(">", 55, opt_y, C_YELLOW, 1);
                    DrawCenteredStringCustom(go_opts[go_idx], opt_y, C_YELLOW, 1);
                    DrawStringCustom("<", 178, opt_y, C_YELLOW, 1);
                } else {
                    DrawCenteredStringCustom(go_opts[go_idx], opt_y, GBA_COLOR(24, 16, 16), 1);
                }
            }
        }

        DrawDeviceNotificationToast(g_device_toast_timer, g_toast_device);

        // --- POP-UP DE LOGRO DESBLOQUEADO (Centrado bajo la línea celeste del HUD con animación desde arriba) ---
        if (g_achievement_toast_timer > 0) {
            int toast_w = 114;
            int toast_h = 24;
            int toast_x = (SCREEN_W - toast_w) / 2;
            int resting_y = 18; // Justo bajo la línea celeste (y = 15)
            int hidden_y = 15 - toast_h; // Oculto detrás de la línea del HUD

            int toast_y = resting_y;
            if (g_achievement_toast_timer > 150) {
                // Entrada suave desde la línea del HUD hacia abajo
                float t = (180 - g_achievement_toast_timer) / 30.0f;
                float ease = 1.0f - (1.0f - t) * (1.0f - t);
                toast_y = (int)(hidden_y + (resting_y - hidden_y) * ease);
            } else if (g_achievement_toast_timer < 30) {
                // Salida suave hacia arriba ocultándose tras la línea
                float t = (30 - g_achievement_toast_timer) / 30.0f;
                float ease = t * t;
                toast_y = (int)(resting_y + (hidden_y - resting_y) * ease);
            }

            DrawRectangle(toast_x, toast_y, toast_w, toast_h, GBA_COLOR(1, 4, 8));
            DrawRectangleLines(toast_x, toast_y, toast_w, toast_h, C_GREEN);
            
            DrawRectangle(toast_x + 4, toast_y + 4, 16, 16, GBA_COLOR(2, 6, 12));
            Enemy t_enemy = { .x = toast_x + 6, .y = toast_y + 6, .type = 0, .active = 1, .hp = 1 };
            DrawEnemyPC(&t_enemy, frame_count);

            const char* ach_header = "!LOGRO NUEVO!";
            if (g_config.language == 1) ach_header = "ACHIEVEMENT!";
            else if (g_config.language == 2) ach_header = "NOUVEAU SUCCES!";
            else if (g_config.language == 3) ach_header = "NUOVO SUCCESSO!";
            else if (g_config.language == 4) ach_header = "NEUER ERFOLG!";

            DrawStringCustom(ach_header, toast_x + 24, toast_y + 5, C_YELLOW, 1);
            StringId titles[30] = { 
                STR_ACH_1_TITLE, STR_ACH_2_TITLE, STR_ACH_3_TITLE, STR_ACH_4_TITLE, 
                STR_ACH_5_TITLE, STR_ACH_6_TITLE, STR_ACH_7_TITLE, STR_ACH_8_TITLE, 
                STR_ACH_9_TITLE, STR_ACH_10_TITLE, STR_ACH_11_TITLE, STR_ACH_12_TITLE, 
                STR_ACH_13_TITLE, STR_ACH_14_TITLE, STR_ACH_15_TITLE, STR_ACH_16_TITLE,
                STR_ACH_17_TITLE, STR_ACH_18_TITLE, STR_ACH_19_TITLE, STR_ACH_20_TITLE,
                STR_ACH_21_TITLE, STR_ACH_22_TITLE, STR_ACH_23_TITLE, STR_ACH_24_TITLE,
                STR_ACH_25_TITLE, STR_ACH_26_TITLE, STR_ACH_27_TITLE, STR_ACH_28_TITLE,
                STR_ACH_29_TITLE, STR_ACH_30_TITLE
            };
            DrawStringCustom(T(titles[g_latest_unlocked_ach]), toast_x + 24, toast_y + 14, WHITE, 1);

            // Redibujar el HUD encima si estamos en partida para que la animación nunca tape el HUD
            if (state == 1 || state == 9 || state == 3) {
                DrawRectangle(0, 0, SCREEN_W, 16, GBA_COLOR(2, 4, 8));
                DrawRectangle(0, 15, SCREEN_W, 1, GBA_COLOR(0, 24, 31));
                for(int h = 0; h < player_hp; h++) DrawHeartCustom(5 + (h * 12), 3, C_HEART);
                DrawStringCustom(T(STR_WAVE), 64, 4, GBA_COLOR(14, 20, 26), 1);
                DrawScoreCustom(wave, 94, 3, C_YELLOW);
                Color combo_col = (combo >= 5) ? C_YELLOW : C_CYAN;
                DrawStringCustom("X", 140, 4, combo_col, 1);
                DrawScoreCustom(combo, 148, 3, combo_col);
                DrawScoreCustom(score, 178, 3, WHITE);
            }
        }

        EndTextureMode();

        // --- RENDERIZADO AL MONITOR CON SCREEN SHAKE Y SHADER CRT ---
        BeginDrawing();
        ClearBackground(BLACK);

        if (g_config.crt_filter > 0 && crtShader.id > 0) {
            SetShaderValue(crtShader, filterTypeLoc, &g_config.crt_filter, SHADER_UNIFORM_INT);
            BeginShaderMode(crtShader);
        }

        float final_offset_x = offset_x;
        float final_offset_y = offset_y;
        if (screen_shake_timer > 0 && (state == 1 || state == 9)) {
            final_offset_x += (float)((rand() % 5) - 2) * scale_val * 0.75f;
            final_offset_y += (float)((rand() % 5) - 2) * scale_val * 0.75f;
        }

        DrawTexturePro(
            target.texture,
            (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
            (Rectangle){ final_offset_x, final_offset_y, draw_w, draw_h },
            (Vector2){ 0.0f, 0.0f },
            0.0f,
            WHITE
        );

        if (g_config.crt_filter > 0 && crtShader.id > 0) EndShaderMode();

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_IOS)
        bool show_touch_hud = (state == 1 || state == 9 || state == 8) && (g_last_input_device != INPUT_GAMEPAD);
        MobileInput_Draw(show_touch_hud);
#endif

#if !defined(PLATFORM_ANDROID) && !defined(PLATFORM_IOS)
        if (m_pos.x >= offset_x && m_pos.x <= offset_x + ((float)SCREEN_W * scale_val) &&
            m_pos.y >= offset_y && m_pos.y <= offset_y + ((float)SCREEN_H * scale_val) &&
            g_last_input_device == INPUT_KEYBOARD) {
            HideCursor();
            DrawSpaceShipCursor16Bit((int)m_pos.x, (int)m_pos.y, mouse_down);
        } else {
            ShowCursor();
        }
#endif

        EndDrawing();
    
}

static void GameShutdown(void)
{
    MobileInput_Shutdown();
    UnloadShader(crtShader);
    UnloadSound(sndShoot);
    UnloadSound(sndHit);
    UnloadSound(sndHurt);
    UnloadSound(sndPowerUp);
    UnloadSound(sndNukePickup);
    UnloadSound(sndDash);
    UnloadSound(sndDeath);
    UnloadSound(sndExplo);
    for (int trk_idx = 0; trk_idx < 5; trk_idx++) {
        if (bgmLoaded[trk_idx]) UnloadSound(bgmTracks[trk_idx]);
    }
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
