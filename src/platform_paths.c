#include "platform_paths.h"

#if defined(PLATFORM_IOS)
extern const char* PlatformIOSGetDataPath(const char *filename);
extern const char* PlatformIOSGetAssetPath(const char *filename);
extern void PlatformIOSShowSecretCodeDialog(const char* title, const char* message, const char* placeholder, const char* cancel_btn, const char* submit_btn, void (*on_submit)(const char* code));
extern void PlatformIOSSetupAudioSession(void);

const char* PlatformGetDataPath(const char *filename)
{
    return PlatformIOSGetDataPath(filename);
}

const char* PlatformGetAssetPath(const char *filename)
{
    return PlatformIOSGetAssetPath(filename);
}

void IOS_ShowSecretCodeDialog(const char* title, const char* message, const char* placeholder, const char* cancel_btn, const char* submit_btn, void (*on_submit)(const char* code))
{
    PlatformIOSShowSecretCodeDialog(title, message, placeholder, cancel_btn, submit_btn, on_submit);
}

void IOS_SetupAudioSession(void)
{
    PlatformIOSSetupAudioSession();
}
#elif defined(PLATFORM_ANDROID)
const char* PlatformGetDataPath(const char *filename) { return filename; }
const char* PlatformGetAssetPath(const char *filename) { return filename; }
void IOS_ShowSecretCodeDialog(const char* title, const char* message, const char* placeholder, const char* cancel_btn, const char* submit_btn, void (*on_submit)(const char* code)) { (void)title; (void)message; (void)placeholder; (void)cancel_btn; (void)submit_btn; (void)on_submit; }
void IOS_SetupAudioSession(void) {}
#else
const char* PlatformGetDataPath(const char *filename) { return filename; }
const char* PlatformGetAssetPath(const char *filename) { return filename; }
void IOS_ShowSecretCodeDialog(const char* title, const char* message, const char* placeholder, const char* cancel_btn, const char* submit_btn, void (*on_submit)(const char* code)) { (void)title; (void)message; (void)placeholder; (void)cancel_btn; (void)submit_btn; (void)on_submit; }
void IOS_SetupAudioSession(void) {}
#endif