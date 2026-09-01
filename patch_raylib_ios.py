#!/usr/bin/env python3
import os
import sys

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

    # 2. Parche rcore_ios.c (Forzar orientación horizontal estricta)
    rcore_ios_path = os.path.join(raylib_dir, "src", "platforms", "rcore_ios.c")
    if os.path.isfile(rcore_ios_path):
        with open(rcore_ios_path, "r", encoding="utf-8") as f:
            content = f.read()

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

        # Forzar landscape en AppDelegate solo si no esta ya
        appdel_target = "@implementation AppDelegate"
        appdel_replacement = """@implementation AppDelegate

- (UIInterfaceOrientationMask)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window {
    return UIInterfaceOrientationMaskLandscape;
}"""
        if appdel_target in content and "supportedInterfaceOrientationsForWindow" not in content:
            content = content.replace(appdel_target, appdel_replacement)

        # Asegurar medidas horizontales al arrancar
        launch_target = "self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];"
        launch_replacement = """CGRect screenBounds = [UIScreen mainScreen].bounds;
    if (screenBounds.size.width < screenBounds.size.height) {
        CGFloat tmp = screenBounds.size.width;
        screenBounds.size.width = screenBounds.size.height;
        screenBounds.size.height = tmp;
    }
    self.window = [[UIWindow alloc] initWithFrame:screenBounds];"""
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
        recreate_target = "void RecreatePlatformSurface(void* layer, int width, int height) {"
        recreate_replacement = """void RecreatePlatformSurface(void* layer, int width, int height) {
    if (width < height) {
        int tmp = width;
        width = height;
        height = tmp;
    }"""
        if recreate_target in content and "if (width < height)" not in content:
            content = content.replace(recreate_target, recreate_replacement)

        with open(rcore_ios_path, "w", encoding="utf-8") as f:
            f.write(content)
        print(f"[patch] Correctamente parcheado rcore_ios.c para orientacion horizontal.")

if __name__ == "__main__":
    patch_raylib()
