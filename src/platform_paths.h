#ifndef PLATFORM_PATHS_H
#define PLATFORM_PATHS_H

#ifdef __cplusplus
extern "C" {
#endif

const char* PlatformGetDataPath(const char *filename);
const char* PlatformGetAssetPath(const char *filename);
void IOS_ShowSecretCodeDialog(const char* title, const char* message, const char* placeholder, const char* cancel_btn, const char* submit_btn, void (*on_submit)(const char* code));
void IOS_SetupAudioSession(void);

#ifdef __cplusplus
}
#endif

#endif
