#ifndef PLATFORM_PATHS_H
#define PLATFORM_PATHS_H

#ifdef __cplusplus
extern "C" {
#endif

const char* PlatformGetDataPath(const char *filename);
const char* PlatformGetAssetPath(const char *filename);

#ifdef __cplusplus
}
#endif

#endif
