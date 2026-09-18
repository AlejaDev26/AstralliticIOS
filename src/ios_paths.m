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

#import <UIKit/UIKit.h>

void PlatformIOSShowSecretCodeDialog(void (*on_submit)(const char* code))
{
    dispatch_async(dispatch_get_main_queue(), ^{
        UIWindow *keyWindow = nil;
        if (@available(iOS 13.0, *)) {
            for (UIWindowScene *scene in [UIApplication sharedApplication].connectedScenes) {
                if (scene.activationState == UISceneActivationStateForegroundActive && [scene isKindOfClass:[UIWindowScene class]]) {
                    for (UIWindow *w in scene.windows) {
                        if (w.isKeyWindow) { keyWindow = w; break; }
                    }
                    if (keyWindow) break;
                }
            }
        }
        if (!keyWindow) {
            keyWindow = [UIApplication sharedApplication].keyWindow;
        }
        if (!keyWindow) {
            NSArray *windows = [UIApplication sharedApplication].windows;
            if (windows.count > 0) keyWindow = windows.firstObject;
        }

        UIViewController *rootVC = keyWindow.rootViewController;
        while (rootVC.presentedViewController) {
            rootVC = rootVC.presentedViewController;
        }
        if (!rootVC) return;

        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"CÓDIGO SECRETO"
                                                                       message:@"Introduce tu código secreto:"
                                                                preferredStyle:UIAlertControllerStyleAlert];

        [alert addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
            textField.placeholder = @"CÓDIGO";
            textField.autocapitalizationType = UITextAutocapitalizationTypeAllCharacters;
            textField.autocorrectionType = UITextAutocorrectionTypeNo;
            textField.returnKeyType = UIReturnKeyDone;
        }];

        [alert addAction:[UIAlertAction actionWithTitle:@"Cancelar" style:UIAlertActionStyleCancel handler:nil]];
        [alert addAction:[UIAlertAction actionWithTitle:@"Canjear" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            UITextField *tf = alert.textFields.firstObject;
            if (tf && tf.text && on_submit) {
                on_submit([tf.text UTF8String]);
            }
        }]];

        [rootVC presentViewController:alert animated:YES completion:nil];
    });
}

