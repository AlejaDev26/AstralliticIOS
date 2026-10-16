#!/usr/bin/env python3
import os
import sys
import re

def patch_raylib():
    raylib_dir = "raylib-ios"
    if not os.path.isdir(raylib_dir):
        if os.path.isdir("build/raylib-ios"):
            raylib_dir = "build/raylib-ios"
        else:
            print(f"[patch] raylib-ios directory not found, skipping patch.")
            return

    # 1. Parche rcore.c (Colisión de tipos Point y Size con Apple SDK)
    rcore_path = os.path.join(raylib_dir, "src", "rcore.c")
    if os.path.isfile(rcore_path):
        with open(rcore_path, "r", encoding="utf-8") as f:
            content = f.read()

        if "RaylibSize" not in content:
            replacements = [
                ("typedef struct { int x; int y; } Point;", "typedef struct { int x; int y; } RaylibPoint;"),
                ("typedef struct { unsigned int width; unsigned int height; } Size;", "typedef struct { unsigned int width; unsigned int height; } RaylibSize;"),
                ("Size display;", "RaylibSize display;"),
                ("Size screen;", "RaylibSize screen;"),
                ("Point position;", "RaylibPoint position;"),
                ("Size previousScreen;", "RaylibSize previousScreen;"),
                ("Point previousPosition;", "RaylibPoint previousPosition;"),
                ("Size render;", "RaylibSize render;"),
                ("Point renderOffset;", "RaylibPoint renderOffset;"),
                ("Size currentFbo;", "RaylibSize currentFbo;"),
                ("Size screenMin;", "RaylibSize screenMin;"),
                ("Size screenMax;", "RaylibSize screenMax;"),
            ]
            for old, new in replacements:
                content = content.replace(old, new)

            with open(rcore_path, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"[patch] Correctamente parcheado rcore.c para evitar colisiones con MacTypes.h")
        else:
            print(f"[patch] rcore.c ya estaba parcheado, omitiendo.")

    # 2. Parche rcore_ios.c (Compatibilidad iOS 12+, Orientación horizontal y Control Dinámico de FPS)
    rcore_ios_path = os.path.join(raylib_dir, "src", "platforms", "rcore_ios.c")
    if os.path.isfile(rcore_ios_path):
        with open(rcore_ios_path, "r", encoding="utf-8") as f:
            content = f.read()

        # Eliminar didUpdateEffectiveGeometry (incompatible con SDKs de Xcode 15 y anterior)
        if "didUpdateEffectiveGeometry" in content:
            content = re.sub(r'- \(void\)windowScene:[^\{]+\{[^\}]+\}', '', content)
            print(f"[patch] Eliminado didUpdateEffectiveGeometry incompatible con SDKs de Xcode 15/iOS 17.")

        # Añadir API_AVAILABLE(ios(13.0)) a SceneDelegate
        content = content.replace(
            '@interface SceneDelegate : UIResponder <UIWindowSceneDelegate>',
            'API_AVAILABLE(ios(13.0))\n@interface SceneDelegate : UIResponder <UIWindowSceneDelegate>'
        )
        content = content.replace(
            '@implementation SceneDelegate',
            'API_AVAILABLE(ios(13.0))\n@implementation SceneDelegate'
        )

        # Añadir API_AVAILABLE(ios(13.0)) a configurationForConnectingSceneSession
        content = content.replace(
            '- (UISceneConfiguration*)application:(UIApplication*)application configurationForConnectingSceneSession:(UISceneSession*)connectingSceneSession options:(UISceneConnectionOptions*)options {',
            '- (UISceneConfiguration*)application:(UIApplication*)application configurationForConnectingSceneSession:(UISceneSession*)connectingSceneSession options:(UISceneConnectionOptions*)options API_AVAILABLE(ios(13.0)) {'
        )

        # Declarar @property window en @interface AppDelegate para iOS 12
        content = content.replace(
            '@interface AppDelegate : UIResponder <UIApplicationDelegate>\n@end',
            '@interface AppDelegate : UIResponder <UIApplicationDelegate>\n@property (strong, nonatomic) UIWindow* window;\n@end'
        )

        # Forzar landscape en GameViewController solo si no se ha anadido ya
        if "shouldAutorotate" not in content:
            gvc_target = "- (bool)prefersStatusBarHidden {\n    return true;\n}"
            gvc_replacement = """- (bool)prefersStatusBarHidden {
    return true;
}

- (BOOL)shouldAutorotate {
    return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskLandscape;
}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation {
    return UIInterfaceOrientationLandscapeRight;
}

- (BOOL)prefersHomeIndicatorAutoHidden {
    return YES;
}"""
            if gvc_target in content:
                content = content.replace(gvc_target, gvc_replacement)

        # Implementar didFinishLaunchingWithOptions en AppDelegate para iOS 12 y forzar landscape
        if "supportedInterfaceOrientationsForWindow" not in content:
            appdel_target = "@implementation AppDelegate"
            appdel_replacement = """@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (@available(iOS 13.0, *)) {
        // En iOS 13+ el ciclo de vida de ventana lo gestiona SceneDelegate
    } else {
        CGRect screenBounds = [UIScreen mainScreen].bounds;
        if (screenBounds.size.width < screenBounds.size.height) {
            CGFloat tmp = screenBounds.size.width;
            screenBounds.size.width = screenBounds.size.height;
            screenBounds.size.height = tmp;
        }
        self.window = [[UIWindow alloc] initWithFrame:screenBounds];
        self.window.rootViewController = [[GameViewController alloc] init];
        [self.window makeKeyAndVisible];
        ios_ready();
        CADisplayLink* displayLink = [CADisplayLink displayLinkWithTarget:self.window.rootViewController selector:@selector(update)];
        s_ios_display_link = displayLink;
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        IOS_SetDisplayFPS(60);
    }
    return YES;
}

- (UIInterfaceOrientationMask)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window {
    return UIInterfaceOrientationMaskLandscape;
}"""
            if appdel_target in content:
                content = content.replace(appdel_target, appdel_replacement)

        # Asegurar medidas horizontales al arrancar en SceneDelegate
        launch_target = "self.window = [[UIWindow alloc] initWithWindowScene:(UIWindowScene*)scene];"
        launch_replacement = """CGRect screenBounds = [UIScreen mainScreen].bounds;
    if (screenBounds.size.width < screenBounds.size.height) {
        CGFloat tmp = screenBounds.size.width;
        screenBounds.size.width = screenBounds.size.height;
        screenBounds.size.height = tmp;
    }
    self.window = [[UIWindow alloc] initWithWindowScene:(UIWindowScene*)scene];"""
        if launch_target in content and "screenBounds" not in content:
            content = content.replace(launch_target, launch_replacement)

        # Asegurar width >= height en InitPlatform
        init_target = """        CGSize screenSize = [[UIScreen mainScreen] bounds].size;
        SetupWindowSizes(screenSize.width, screenSize.height);"""
        init_replacement = """        CGSize screenSize = [[UIScreen mainScreen] bounds].size;
        int sw = (int)screenSize.width;
        int sh = (int)screenSize.height;
        if (sw < sh) {
            int tmp = sw;
            sw = sh;
            sh = tmp;
        }
        SetupWindowSizes(sw, sh);"""
        if init_target in content and "int sw" not in content:
            content = content.replace(init_target, init_replacement)

        # Asegurar width >= height en cambio de superficie
        recreate_target1 = "static void RecreatePlatformSurface(void* layer, int width, int height, bool isInteractivelyResizing) {"
        recreate_replacement1 = """static void RecreatePlatformSurface(void* layer, int width, int height, bool isInteractivelyResizing) {
    if (width < height) {
        int tmp = width;
        width = height;
        height = tmp;
    }"""
        recreate_target2 = "void RecreatePlatformSurface(void* layer, int width, int height) {"
        recreate_replacement2 = """void RecreatePlatformSurface(void* layer, int width, int height) {
    if (width < height) {
        int tmp = width;
        width = height;
        height = tmp;
    }"""
        if recreate_target1 in content and "int tmp = width;" not in content:
            content = content.replace(recreate_target1, recreate_replacement1)
        elif recreate_target2 in content and "int tmp = width;" not in content:
            content = content.replace(recreate_target2, recreate_replacement2)

        # Añadir funciones de control dinámico de FPS y detección de pantalla
        fps_func_target = "- (bool)prefersStatusBarHidden {"
        fps_func_replacement = """static CADisplayLink *s_ios_display_link = nil;

int IOS_GetMaxRefreshRate(void) {
    if (@available(iOS 10.3, *)) {
        NSInteger maxFps = [UIScreen mainScreen].maximumFramesPerSecond;
        if (maxFps > 0) return (int)maxFps;
    }
    return 60;
}

void IOS_SetDisplayFPS(int fps) {
    if (!s_ios_display_link) return;
    int max_fps = IOS_GetMaxRefreshRate();
    if (max_fps < 60) max_fps = 60;
    if (fps <= 0) fps = max_fps;
    if (fps > max_fps) fps = max_fps;

    if (@available(iOS 15.0, *)) {
        float f = (float)fps;
        s_ios_display_link.preferredFrameRateRange = CAFrameRateRangeMake(f, f, f);
    } else {
        s_ios_display_link.preferredFramesPerSecond = fps;
    }
}

- (bool)prefersStatusBarHidden {"""
        if fps_func_target in content and "s_ios_display_link" not in content:
            content = content.replace(fps_func_target, fps_func_replacement)

        # Configurar CADisplayLink en SceneDelegate para soportar ProMotion 120Hz nativo en iPhone
        dlink_target = "[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];"
        dlink_replacement = """s_ios_display_link = displayLink;
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    IOS_SetDisplayFPS(60);"""
        if "s_ios_display_link = displayLink;" not in content:
            old_block = """if (@available(iOS 15.0, *)) {
        displayLink.preferredFrameRateRange = CAFrameRateRangeMake(30.0, 120.0, 120.0);
    } else {
        displayLink.preferredFramesPerSecond = 120;
    }
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];"""
            if old_block in content:
                content = content.replace(old_block, dlink_replacement)
            elif dlink_target in content:
                content = content.replace(dlink_target, dlink_replacement)

        with open(rcore_ios_path, "w", encoding="utf-8") as f:
            f.write(content)
        print(f"[patch] Correctamente parcheado rcore_ios.c para iOS 12+, orientación horizontal y FPS.")

if __name__ == "__main__":
    patch_raylib()
