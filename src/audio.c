#include "audio.h"
#include "platform_paths.h"
#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Sound sndShoot, sndHit, sndHurt, sndPowerUp, sndDash, sndDeath, sndExplo, sndNukePickup;
Sound bgmTracks[5] = {0};
int bgmLoaded[5] = {0};
int currentBgmPlaying = -1;

static float g_title_bgm_duration = 30.772f;
static float g_title_bgm_timer = 0.0f;
static float g_title_glitch_snap = 0.0f;

void ApplyAudioVolumes() {
    float bgm_vol = (float)g_config.vol_bgm / 10.0f;
    float sfx_vol = (float)g_config.vol_sfx / 10.0f;

    for (int i = 0; i < 5; i++) {
        if (bgmLoaded[i]) SetSoundVolume(bgmTracks[i], bgm_vol * 0.66f);
    }
    SetSoundVolume(sndShoot, sfx_vol * 0.7f);
    SetSoundVolume(sndHit, sfx_vol * 0.8f);
    SetSoundVolume(sndHurt, sfx_vol * 0.9f);
    SetSoundVolume(sndPowerUp, sfx_vol * 0.8f);
    SetSoundVolume(sndDash, sfx_vol * 0.7f);
    SetSoundVolume(sndDeath, sfx_vol * 0.9f);
    SetSoundVolume(sndExplo, sfx_vol * 0.9f);
    SetSoundVolume(sndNukePickup, sfx_vol * 0.95f);
}

Sound GenerateTone(float freq, float duration, int type) {
    int sampleRate = 44100;
    int sampleCount = (int)(sampleRate * duration);
    short *data = (short*)malloc(sampleCount * sizeof(short));
    for (int i = 0; i < sampleCount; i++) {
        float t = (float)i / sampleRate;
        float factor = 1.0f - (float)i / sampleCount;
        float sample = 0.0f;
        if (type == 0) sample = ((int)(t * freq * 2) % 2 == 0) ? 1.0f : -1.0f;
        else if (type == 1) sample = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
        else sample = 2.0f * (t * freq - (int)(t * freq + 0.5f));
        data[i] = (short)(sample * factor * 7000.0f);
    }
    Wave wave = { .frameCount = sampleCount, .sampleRate = sampleRate, .sampleSize = 16, .channels = 1, .data = data };
    Sound s = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return s;
}

void PlayReadyTone(float freq) {
    if (g_config.vol_sfx == 0) return;
    Sound s = GenerateTone(freq, 0.11f, 0);
    SetSoundVolume(s, ((float)g_config.vol_sfx / 10.0f) * 0.7f);
    PlaySound(s);
}

void UpdateReadyJinglePC(int timer) {
    if (g_config.vol_sfx == 0) return;
    if (timer > 60) {
        int step = (120 - timer) / 7;
        const float ready_freqs[8] = { 261.6f, 293.6f, 329.6f, 349.2f, 392.0f, 440.0f, 493.8f, 523.2f };
        if ((120 - timer) % 7 == 0 && step < 8) PlayReadyTone(ready_freqs[step]);
    } else {
        int step = (60 - timer) / 6;
        const float go_freqs[8] = { 523.2f, 587.3f, 659.2f, 783.9f, 659.2f, 783.9f, 880.0f, 1046.5f };
        if ((60 - timer) % 6 == 0 && step < 8) PlayReadyTone(go_freqs[step]);
    }
}

void TryLoadRawMusic(int index, const char* filename) {
    const char* path = PlatformGetAssetPath(filename);
    int size = 0;
    unsigned char* raw_data = LoadFileData(path, &size);
    if (raw_data != NULL && size > 0) {
        long zero_crossings_signed = 0, zero_crossings_unsigned = 0;
        for(long i = 0; i < size && i < 4000; i++) {
            signed char sc = (signed char)raw_data[i];
            if (sc > -20 && sc < 20) zero_crossings_signed++;
            if (raw_data[i] > 108 && raw_data[i] < 148) zero_crossings_unsigned++;
        }
        int is_unsigned = (zero_crossings_unsigned > zero_crossings_signed);

        short* pcm16 = (short*)malloc(size * sizeof(short));
        for(long i = 0; i < size; i++) {
            float sample_norm = 0.0f;
            if (is_unsigned) sample_norm = ((float)raw_data[i] - 128.0f) / 128.0f;
            else sample_norm = (float)((signed char)raw_data[i]) / 128.0f;
            pcm16[i] = (short)(sample_norm * 0.48f * 32767.0f);
        }
        UnloadFileData(raw_data);

        Wave wave = { .frameCount = (unsigned int)size, .sampleRate = 16000, .sampleSize = 16, .channels = 1, .data = pcm16 };
        bgmTracks[index] = LoadSoundFromWave(wave);
        bgmLoaded[index] = 1;
        if (index == 0) {
            g_title_bgm_duration = (float)size / 16000.0f;
        }
        UnloadWave(wave);
    }
}

void InitGameAudio() {
    InitAudioDevice();
    SetMasterVolume(1.0f);

    // Disparo (Láser dinámico 16-bits)
    if (IsSoundValid(sndShoot)) UnloadSound(sndShoot);
    {
        int sampleRate = 44100;
        float duration = 0.09f;
        int sampleCount = (int)(sampleRate * duration);
        short *data = (short*)malloc(sampleCount * sizeof(short));
        for (int i = 0; i < sampleCount; i++) {
            float t = (float)i / sampleRate;
            float progress = (float)i / sampleCount;
            float current_freq = 1200.0f - (progress * 900.0f);
            float factor = 1.0f - progress;
            float sample = ((int)(t * current_freq * 2) % 2 == 0) ? 1.0f : -1.0f;
            data[i] = (short)(sample * factor * 9000.0f);
        }
        Wave wave = { .frameCount = sampleCount, .sampleRate = sampleRate, .sampleSize = 16, .channels = 1, .data = data };
        sndShoot = LoadSoundFromWave(wave);
        UnloadWave(wave);
    }

    // Impacto en enemigo (Chip-snap metálico y seco)
    if (IsSoundValid(sndHit)) UnloadSound(sndHit);
    {
        int sampleRate = 44100;
        float duration = 0.05f;
        int sampleCount = (int)(sampleRate * duration);
        short *data = (short*)malloc(sampleCount * sizeof(short));
        for (int i = 0; i < sampleCount; i++) {
            float t = (float)i / sampleRate;
            float progress = (float)i / sampleCount;
            float factor = 1.0f - progress;
            float noise = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
            float tone = ((int)(t * 800.0f * 2) % 2 == 0) ? 0.5f : -0.5f;
            float sample = (noise * 0.7f) + (tone * 0.3f);
            data[i] = (short)(sample * factor * 8000.0f);
        }
        Wave wave = { .frameCount = sampleCount, .sampleRate = sampleRate, .sampleSize = 16, .channels = 1, .data = data };
        sndHit = LoadSoundFromWave(wave);
        UnloadWave(wave);
    }

    // Recibir daño / Jugador herido (Sierra descendente con alarma)
    if (IsSoundValid(sndHurt)) UnloadSound(sndHurt);
    {
        int sampleRate = 44100;
        float duration = 0.18f;
        int sampleCount = (int)(sampleRate * duration);
        short *data = (short*)malloc(sampleCount * sizeof(short));
        for (int i = 0; i < sampleCount; i++) {
            float t = (float)i / sampleRate;
            float progress = (float)i / sampleCount;
            float freq = 350.0f - (progress * 250.0f);
            float factor = 1.0f - progress;
            float sample = 2.0f * (t * freq - (int)(t * freq + 0.5f));
            data[i] = (short)(sample * factor * 10000.0f);
        }
        Wave wave = { .frameCount = sampleCount, .sampleRate = sampleRate, .sampleSize = 16, .channels = 1, .data = data };
        sndHurt = LoadSoundFromWave(wave);
        UnloadWave(wave);
    }

    // Recoger Power-Up genérico (Arpegio FM clásico ascendente)
    if (IsSoundValid(sndPowerUp)) UnloadSound(sndPowerUp);
    {
        int sampleRate = 44100;
        float duration = 0.22f;
        int sampleCount = (int)(sampleRate * duration);
        short *data = (short*)malloc(sampleCount * sizeof(short));
        for (int i = 0; i < sampleCount; i++) {
            float t = (float)i / sampleRate;
            float progress = (float)i / sampleCount;
            float freq = 440.0f + (floorf(progress * 4.0f) * 110.0f);
            float factor = 1.0f - progress;
            float sample = ((int)(t * freq * 2) % 2 == 0) ? 0.8f : -0.8f;
            data[i] = (short)(sample * factor * 8500.0f);
        }
        Wave wave = { .frameCount = sampleCount, .sampleRate = sampleRate, .sampleSize = 16, .channels = 1, .data = data };
        sndPowerUp = LoadSoundFromWave(wave);
        UnloadWave(wave);
    }

    // Sonido exclusivo de recogida de Bomba / Nuke (Estallido sordo y explosivo)
    if (IsSoundValid(sndNukePickup)) UnloadSound(sndNukePickup);
    {
        int sampleRate = 44100;
        float duration = 0.45f;
        int sampleCount = (int)(sampleRate * duration);
        short *data = (short*)malloc(sampleCount * sizeof(short));
        for (int i = 0; i < sampleCount; i++) {
            float t = (float)i / sampleRate;
            float progress = (float)i / sampleCount;
            float freq = 90.0f - (progress * 70.0f);
            float factor = 1.0f - progress;
            float noise = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
            float sub = ((int)(t * freq * 2) % 2 == 0) ? 0.8f : -0.8f;
            float sample = (noise * 0.8f) + (sub * 0.2f);
            data[i] = (short)(sample * factor * 12000.0f);
        }
        Wave wave = { .frameCount = sampleCount, .sampleRate = sampleRate, .sampleSize = 16, .channels = 1, .data = data };
        sndNukePickup = LoadSoundFromWave(wave);
        UnloadWave(wave);
    }

    sndDash = GenerateTone(440.0f, 0.12f, 2);

    // Muerte del jugador (Colapso de núcleo y ruido grave)
    if (IsSoundValid(sndDeath)) UnloadSound(sndDeath);
    {
        int sampleRate = 44100;
        float duration = 0.55f;
        int sampleCount = (int)(sampleRate * duration);
        short *data = (short*)malloc(sampleCount * sizeof(short));
        for (int i = 0; i < sampleCount; i++) {
            float t = (float)i / sampleRate;
            float progress = (float)i / sampleCount;
            float current_freq = 180.0f - (progress * 150.0f);
            float factor = 1.0f - progress;
            float square = ((int)(t * current_freq * 2) % 2 == 0) ? 0.7f : -0.7f;
            float noise = ((float)rand() / (float)RAND_MAX) * 0.6f - 0.3f;
            float sample = square + noise;
            data[i] = (short)(sample * factor * 11000.0f);
        }
        Wave wave = { .frameCount = sampleCount, .sampleRate = sampleRate, .sampleSize = 16, .channels = 1, .data = data };
        sndDeath = LoadSoundFromWave(wave);
        UnloadWave(wave);
    }

    // Explosión de enemigo (Retumbo de graves con saturación y ruido)
    if (IsSoundValid(sndExplo)) UnloadSound(sndExplo);
    {
        int sampleRate = 44100;
        float duration = 0.35f;
        int sampleCount = (int)(sampleRate * duration);
        short *data = (short*)malloc(sampleCount * sizeof(short));
        for (int i = 0; i < sampleCount; i++) {
            float t = (float)i / sampleRate;
            float progress = (float)i / sampleCount;
            float freq = 120.0f - (progress * 90.0f);
            float factor = 1.0f - progress;
            float noise = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
            float sub = ((int)(t * freq * 2) % 2 == 0) ? 0.6f : -0.6f;
            float sample = (noise * 0.7f) + (sub * 0.3f);
            data[i] = (short)(sample * factor * 10500.0f);
        }
        Wave wave = { .frameCount = sampleCount, .sampleRate = sampleRate, .sampleSize = 16, .channels = 1, .data = data };
        sndExplo = LoadSoundFromWave(wave);
        UnloadWave(wave);
    }

    TryLoadRawMusic(0, "titulo.raw");
    TryLoadRawMusic(1, "opciones.raw");
    TryLoadRawMusic(2, "records.raw");
    TryLoadRawMusic(3, "dificultad.raw");
    TryLoadRawMusic(4, "gameover.raw");

    ApplyAudioVolumes();
}

void PlayGameBgm(int index) {
    if (g_config.vol_bgm == 0) return;
    if (currentBgmPlaying == index && index != -1 && bgmLoaded[index]) {
        if (!IsSoundPlaying(bgmTracks[index])) {
            PlaySound(bgmTracks[index]);
            if (index == 0) {
                g_title_bgm_timer = 0.0f;
                g_title_glitch_snap = 0.0f;
            }
        }
        return;
    }
    if (currentBgmPlaying != -1 && bgmLoaded[currentBgmPlaying]) StopSound(bgmTracks[currentBgmPlaying]);
    currentBgmPlaying = index;
    if (index != -1 && bgmLoaded[index]) {
        PlaySound(bgmTracks[index]);
        if (index == 0) {
            g_title_bgm_timer = 0.0f;
            g_title_glitch_snap = 0.0f;
        }
    }
}

void UpdateGameBgm() {
    if (g_config.vol_bgm == 0 || currentBgmPlaying == -1 || !bgmLoaded[currentBgmPlaying]) return;
    if (currentBgmPlaying == 0) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        g_title_bgm_timer += dt;
        if (g_title_glitch_snap > 0.0f) {
            g_title_glitch_snap -= dt * 4.5f;
            if (g_title_glitch_snap < 0.0f) g_title_glitch_snap = 0.0f;
        }
        if (!IsSoundPlaying(bgmTracks[0]) || g_title_bgm_timer >= g_title_bgm_duration) {
            StopSound(bgmTracks[0]);
            PlaySound(bgmTracks[0]);
            g_title_bgm_timer = 0.0f;
            g_title_glitch_snap = 1.0f; // Activa el destello de resincronización al recomenzar
        }
    } else {
        if (!IsSoundPlaying(bgmTracks[currentBgmPlaying])) PlaySound(bgmTracks[currentBgmPlaying]);
    }
}

void StopAllBgm() {
    if (currentBgmPlaying != -1 && bgmLoaded[currentBgmPlaying]) StopSound(bgmTracks[currentBgmPlaying]);
    currentBgmPlaying = -1;
    g_title_bgm_timer = 0.0f;
    g_title_glitch_snap = 0.0f;
}

float GetTitleBgmGlitchIntensity() {
    if (g_config.vol_bgm == 0 || currentBgmPlaying != 0 || !bgmLoaded[0]) return 0.0f;
    const float lead_time = 1.8f;
    float remaining = g_title_bgm_duration - g_title_bgm_timer;
    float intensity = 0.0f;
    if (remaining <= lead_time && remaining > 0.0f) {
        float progress = 1.0f - (remaining / lead_time);
        intensity = progress * progress; // Curva suave hacia 1.0
    } else if (remaining <= 0.0f) {
        intensity = 1.0f;
    }
    if (g_title_glitch_snap > 0.0f) {
        float snap_val = g_title_glitch_snap * 0.85f;
        if (snap_val > intensity) intensity = snap_val;
    }
    if (intensity > 1.0f) intensity = 1.0f;
    return intensity;
}

float GetTitleBgmSnap() {
    if (g_config.vol_bgm == 0 || currentBgmPlaying != 0) return 0.0f;
    return g_title_glitch_snap;
}

void PlaySfx(Sound s) {
    if (g_config.vol_sfx > 0) PlaySound(s);
}
