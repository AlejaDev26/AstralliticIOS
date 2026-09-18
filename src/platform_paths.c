#include "platform_paths.h"

#if defined(PLATFORM_IOS)
extern const char* PlatformIOSGetDataPath(const char *filename);
extern const char* PlatformIOSGetAssetPath(const char *filename);
extern void PlatformIOSShowSecretCodeDialog(void (*on_submit)(const char* code));

const char* PlatformGetDataPath(const char *filename)
{
    return PlatformIOSGetDataPath(filename);
}

const char* PlatformGetAssetPath(const char *filename)
{
    return PlatformIOSGetAssetPath(filename);
}

void IOS_ShowSecretCodeDialog(void (*on_submit)(const char* code))
{
    PlatformIOSShowSecretCodeDialog(on_submit);
}
#elif defined(PLATFORM_ANDROID)
const char* PlatformGetDataPath(const char *filename) { return filename; }
const char* PlatformGetAssetPath(const char *filename) { return filename; }
void IOS_ShowSecretCodeDialog(void (*on_submit)(const char* code)) { (void)on_submit; }
#else
const char* PlatformGetDataPath(const char *filename) { return filename; }
const char* PlatformGetAssetPath(const char *filename) { return filename; }
void IOS_ShowSecretCodeDialog(void (*on_submit)(const char* code)) { (void)on_submit; }
#endif