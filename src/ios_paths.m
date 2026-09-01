#import <Foundation/Foundation.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static const char *CopyNSStringToStatic(NSString *path)
{
    #define NUM_BUFFERS 4
    static char buffers[NUM_BUFFERS][PATH_MAX];
    static int buf_index = 0;

    if (!path) return "";

    buf_index = (buf_index + 1) % NUM_BUFFERS;
    char *buffer = buffers[buf_index];

    const char *utf8 = [path UTF8String];
    if (!utf8) {
        buffer[0] = '\0';
        return buffer;
    }
    strncpy(buffer, utf8, PATH_MAX - 1);
    buffer[PATH_MAX - 1] = '\0';
    return buffer;
}

const char* PlatformIOSGetDataPath(const char *filename)
{
    NSString *base = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES).firstObject;
    if (!base) {
        base = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    }
    NSString *dir = [base stringByAppendingPathComponent:@"Astrallitic"];
    [[NSFileManager defaultManager] createDirectoryAtPath:dir
                              withIntermediateDirectories:YES
                                               attributes:nil
                                                    error:nil];
    NSString *name = filename ? [NSString stringWithUTF8String:filename] : @"";
    return CopyNSStringToStatic([dir stringByAppendingPathComponent:name]);
}

const char* PlatformIOSGetAssetPath(const char *filename)
{
    if (!filename || filename[0] == '\0') return "";

    NSString *name = [NSString stringWithUTF8String:filename];

    // Check main bundle directly by resource name and extension
    NSString *resourceName = [name stringByDeletingPathExtension];
    NSString *resourceExt = [name pathExtension];
    NSString *full = [[NSBundle mainBundle] pathForResource:resourceName ofType:resourceExt];

    // If not found with pathForResource, check resourcePath/filename
    if (!full) {
        NSString *bundlePath = [[NSBundle mainBundle] resourcePath];
        NSString *candidate = [bundlePath stringByAppendingPathComponent:name];
        if ([[NSFileManager defaultManager] fileExistsAtPath:candidate]) {
            full = candidate;
        }
    }

    if (!full) {
        // Fallback to original filename
        full = name;
    }

    return CopyNSStringToStatic(full);
}
