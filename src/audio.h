#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"

extern Sound sndShoot, sndHit, sndHurt, sndPowerUp, sndDash, sndDeath, sndExplo;
extern Sound bgmTracks[5];
extern Sound sndNukePickup;
extern int bgmLoaded[5];
extern int currentBgmPlaying;

#define READY_JINGLE_TONE_COUNT 16

void ApplyAudioVolumes();
Sound GenerateTone(float freq, float duration, int type);
void PlayReadyTone(float freq);
void PlayReadyToneIndex(int index);
void UpdateReadyJinglePC(int timer);
void TryLoadRawMusic(int index, const char* filename);
void InitGameAudio();
void UnloadGameAudio(void);
void PlayGameBgm(int index);
void UpdateGameBgm();
void StopAllBgm();
void PlaySfx(Sound s);

float GetTitleBgmGlitchIntensity(void);
float GetTitleBgmSnap(void);

#endif