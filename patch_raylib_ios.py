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

    # 1. Patch rcore.c (Collision with Apple MacTypes.h Point and Size)
    rcore_path = os.path.join(raylib_dir, "src", "rcore.c")
    if os.path.isfile(rcore_path):
        with open(rcore_path, "r", encoding="utf-8") as f:
            content = f.read()

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
        print(f"[patch] Successfully patched {rcore_path} for MacTypes.h Point/Size collision.")

    # 2. Patch platforms/rcore_ios.c (Enforce Landscape orientation and prevent vertical video)
    rcore_ios_path = os.path.join(raylib_dir, "src", "platforms", "rcore_ios.c")
    if os.path.isfile(rcore_ios_path):
        with open(rcore_ios_path, "r", encoding="utf-8") as f:
            content = f.read()

        # Enforce landscape in GameViewController
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

        # Enforce landscape in AppDelegate
        appdel_target = "@implementation AppDelegate"
        appdel_replacement = """@implementation AppDelegate

- (UIInterfaceOrientationMask)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window {
    return UIInterfaceOrientationMaskLandscape;
}"""
        if appdel_target in content and "supportedInterfaceOrientationsForWindow" not in content:
            content = content.replace(appdel_target, appdel_replacement)

        # Ensure landscape bounds in didFinishLaunchingWithOptions
        launch_target = "self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];"
        launch_replacement = """CGRect screenBounds = [UIScreen mainScreen].bounds;
    if (screenBounds.size.width < screenBounds.size.height) {
        CGFloat tmp = screenBounds.size.width;
        screenBounds.size.width = screenBounds.size.height;
        screenBounds.size.height = tmp;
    }
    self.window = [[UIWindow alloc] initWithFrame:screenBounds];"""
        if launch_target in content:
            content = content.replace(launch_target, launch_replacement)

        # Ensure width >= height in InitPlatform
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
        if init_target in content:
            content = content.replace(init_target, init_replacement)

        # Ensure width >= height in RecreatePlatformSurface
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
        print(f"[patch] Successfully patched {rcore_ios_path} for strict landscape orientation.")

if __name__ == "__main__":
    patch_raylib()
