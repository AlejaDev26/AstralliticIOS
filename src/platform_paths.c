#include "platform_paths.h"

#if defined(PLATFORM_IOS)
extern const char* PlatformIOSGetDataPath(const char *filename);
extern const char* PlatformIOSGetAssetPath(const char *filename);

const char* PlatformGetDataPath(const char *filename)
{
    return PlatformIOSGetDataPath(filename);
}

const char* PlatformGetAssetPath(const char *filename)
{
    return PlatformIOSGetAssetPath(filename);
}
#elif defined(PLATFORM_ANDROID)
/* En Android, Raylib accede a los assets empaquetados en el APK usando rutas relativas directas */
const char* PlatformGetDataPath(const char *filename) { return filename; }
const char* PlatformGetAssetPath(const char *filename) { return filename; }
#else
const char* PlatformGetDataPath(const char *filename) { return filename; }
const char* PlatformGetAssetPath(const char *filename) { return filename; }
#endif