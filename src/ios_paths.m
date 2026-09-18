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
#import <AVFoundation/AVFoundation.h>

@interface AstralliticAudioSessionManager : NSObject
+ (instancetype)sharedInstance;
- (void)setupAudioSession;
@end

@implementation AstralliticAudioSessionManager

+ (instancetype)sharedInstance {
    static AstralliticAudioSessionManager *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[AstralliticAudioSessionManager alloc] init];
    });
    return instance;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(handleRouteChange:)
                                                     name:AVAudioSessionRouteChangeNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(handleInterruption:)
                                                     name:AVAudioSessionInterruptionNotification
                                                   object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(handleAppDidBecomeActive:)
                                                     name:UIApplicationDidBecomeActiveNotification
                                                   object:nil];
    }
    return self;
}

- (void)setupAudioSession {
    AVAudioSession *session = [AVAudioSession sharedInstance];
    NSError *error = nil;

    // AVAudioSessionCategoryPlayback ensures:
    // 1. Audio is NOT muted by the hardware silent/vibrate switch or focus modes.
    // 2. Audio routes automatically to connected Bluetooth devices (AirPods, Bluetooth headsets, and Ray-Ban Meta glasses).
    AVAudioSessionCategoryOptions options = AVAudioSessionCategoryOptionMixWithOthers;
    if (@available(iOS 10.0, *)) {
        options |= AVAudioSessionCategoryOptionAllowBluetoothA2DP | AVAudioSessionCategoryOptionAllowAirPlay;
    }

    if (@available(iOS 10.0, *)) {
        [session setCategory:AVAudioSessionCategoryPlayback
                        mode:AVAudioSessionModeDefault
                     options:options
                       error:&error];
    } else {
        [session setCategory:AVAudioSessionCategoryPlayback
                 withOptions:options
                       error:&error];
    }

    if (error) {
        NSLog(@"[Astrallitic Audio] Warning setting audio category with options: %@", error.localizedDescription);
        error = nil;
        [session setCategory:AVAudioSessionCategoryPlayback error:&error];
        if (error) {
            NSLog(@"[Astrallitic Audio] Error setting fallback audio category: %@", error.localizedDescription);
        }
    }

    // Set preferred low latency buffer and sample rate
    [session setPreferredIOBufferDuration:0.005 error:nil];
    [session setPreferredSampleRate:44100.0 error:nil];

    [session setActive:YES error:&error];
    if (error) {
        NSLog(@"[Astrallitic Audio] Error activating audio session: %@", error.localizedDescription);
    } else {
        NSLog(@"[Astrallitic Audio] Audio session configured and active (Playback + Bluetooth/AirPlay/Mix).");
    }
}

- (void)handleRouteChange:(NSNotification *)notification {
    NSDictionary *userInfo = notification.userInfo;
    NSNumber *reasonNum = userInfo[AVAudioSessionRouteChangeReasonKey];
    AVAudioSessionRouteChangeReason reason = (AVAudioSessionRouteChangeReason)[reasonNum unsignedIntegerValue];
    NSLog(@"[Astrallitic Audio] Route changed (reason: %lu). Re-activating session...", (unsigned long)reason);
    NSError *error = nil;
    [[AVAudioSession sharedInstance] setActive:YES error:&error];
}

- (void)handleInterruption:(NSNotification *)notification {
    NSDictionary *userInfo = notification.userInfo;
    NSNumber *typeNum = userInfo[AVAudioSessionInterruptionTypeKey];
    AVAudioSessionInterruptionType type = (AVAudioSessionInterruptionType)[typeNum unsignedIntegerValue];
    if (type == AVAudioSessionInterruptionTypeEnded) {
        NSNumber *optNum = userInfo[AVAudioSessionInterruptionOptionKey];
        AVAudioSessionInterruptionOptions options = (AVAudioSessionInterruptionOptions)[optNum unsignedIntegerValue];
        if (options & AVAudioSessionInterruptionOptionShouldResume) {
            NSLog(@"[Astrallitic Audio] Interruption ended. Resuming audio session...");
            NSError *error = nil;
            [[AVAudioSession sharedInstance] setActive:YES error:&error];
        }
    }
}

- (void)handleAppDidBecomeActive:(NSNotification *)notification {
    NSLog(@"[Astrallitic Audio] App became active. Verifying audio session...");
    NSError *error = nil;
    [[AVAudioSession sharedInstance] setActive:YES error:&error];
}

@end

void PlatformIOSSetupAudioSession(void)
{
    [[AstralliticAudioSessionManager sharedInstance] setupAudioSession];
}

void PlatformIOSShowSecretCodeDialog(const char* title, const char* message, const char* placeholder, const char* cancel_btn, const char* submit_btn, void (*on_submit)(const char* code))
{
    NSString *nsTitle = (title && title[0]) ? [NSString stringWithUTF8String:title] : @"CÓDIGO SECRETO";
    NSString *nsMessage = (message && message[0]) ? [NSString stringWithUTF8String:message] : @"Introduce tu código secreto:";
    NSString *nsPlaceholder = (placeholder && placeholder[0]) ? [NSString stringWithUTF8String:placeholder] : @"CÓDIGO";
    NSString *nsCancel = (cancel_btn && cancel_btn[0]) ? [NSString stringWithUTF8String:cancel_btn] : @"Cancelar";
    NSString *nsSubmit = (submit_btn && submit_btn[0]) ? [NSString stringWithUTF8String:submit_btn] : @"Canjear";

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

        UIAlertController *alert = [UIAlertController alertControllerWithTitle:nsTitle
                                                                       message:nsMessage
                                                                preferredStyle:UIAlertControllerStyleAlert];

        [alert addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
            textField.placeholder = nsPlaceholder;
            textField.autocapitalizationType = UITextAutocapitalizationTypeAllCharacters;
            textField.autocorrectionType = UITextAutocorrectionTypeNo;
            textField.returnKeyType = UIReturnKeyDone;
        }];

        [alert addAction:[UIAlertAction actionWithTitle:nsCancel style:UIAlertActionStyleCancel handler:nil]];
        [alert addAction:[UIAlertAction actionWithTitle:nsSubmit style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            UITextField *tf = alert.textFields.firstObject;
            if (tf && tf.text && on_submit) {
                on_submit([tf.text UTF8String]);
            }
        }]];

        [rootVC presentViewController:alert animated:YES completion:nil];
    });
}


