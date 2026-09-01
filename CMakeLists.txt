cmake_minimum_required(VERSION 3.20)
project(Astrallitic C OBJC)

set(CMAKE_C_STANDARD 99)
set(CMAKE_OBJC_STANDARD 11)

# Ubicación del backend raylib-iOS
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/raylib-ios/src/rcore.c")
    set(RAYLIB_IOS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib-ios")
elseif(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/raylib-ios/src/rcore.c")
    set(RAYLIB_IOS_DIR "${CMAKE_CURRENT_BINARY_DIR}/raylib-ios")
else()
    include(FetchContent)
    FetchContent_Declare(
        raylib_ios
        GIT_REPOSITORY https://github.com/ghera/raylib-iOS.git
        GIT_TAG master
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(raylib_ios)
    set(RAYLIB_IOS_DIR "${raylib_ios_SOURCE_DIR}")
endif()

message(STATUS "Usando raylib-iOS desde: ${RAYLIB_IOS_DIR}")

# Aplicar parche para forzar orientación horizontal y corregir colisiones de tipos
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/patch_raylib_ios.py")
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_EXECUTABLE)
        execute_process(
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/patch_raylib_ios.py"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endif()
endif()

# Directorios de ANGLE
set(ANGLE_DIR "${RAYLIB_IOS_DIR}/deps/ANGLE")
set(ANGLE_EGL_XCFW "${ANGLE_DIR}/libEGL.xcframework")
set(ANGLE_GLES_XCFW "${ANGLE_DIR}/libGLESv2.xcframework")

# Directorio de cabeceras de ANGLE para arm64
set(ANGLE_EGL_HDRS "${ANGLE_EGL_XCFW}/ios-arm64/libEGL.framework/Headers")
set(ANGLE_GLES_HDRS "${ANGLE_GLES_XCFW}/ios-arm64/libGLESv2.framework/Headers")

# Preparar directorio de staging para includes como <libEGL/libEGL.h> y <GLES/gl.h>
set(INC_STAGE "${CMAKE_CURRENT_BINARY_DIR}/include_staging")
file(MAKE_DIRECTORY "${INC_STAGE}/libEGL")
file(MAKE_DIRECTORY "${INC_STAGE}/libGLESv2")
if(EXISTS "${ANGLE_EGL_HDRS}")
    file(COPY "${ANGLE_EGL_HDRS}/" DESTINATION "${INC_STAGE}/libEGL")
endif()
if(EXISTS "${ANGLE_GLES_HDRS}")
    file(COPY "${ANGLE_GLES_HDRS}/" DESTINATION "${INC_STAGE}/libGLESv2")
endif()

# Asegurar que glext.h incluya gl.h primero para definir tipos básicos (GL_API, GLenum)
if(EXISTS "${INC_STAGE}/libGLESv2/GLES/glext.h")
    file(READ "${INC_STAGE}/libGLESv2/GLES/glext.h" GLEXT_CONTENT)
    if(NOT GLEXT_CONTENT MATCHES "#include \"gl.h\"")
        file(WRITE "${INC_STAGE}/libGLESv2/GLES/glext.h" "#include \"gl.h\"\n${GLEXT_CONTENT}")
    endif()
endif()

# Fuentes de Raylib
set(RAYLIB_SOURCES
    ${RAYLIB_IOS_DIR}/src/rcore.c
    ${RAYLIB_IOS_DIR}/src/raudio.c
    ${RAYLIB_IOS_DIR}/src/rmodels.c
    ${RAYLIB_IOS_DIR}/src/rshapes.c
    ${RAYLIB_IOS_DIR}/src/rtext.c
    ${RAYLIB_IOS_DIR}/src/rtextures.c
)

# En iOS, rcore.c incluye rcore_ios.c (UIKit) y raudio.c usa CoreAudio (Obj-C)
set_source_files_properties(
    ${RAYLIB_IOS_DIR}/src/rcore.c
    ${RAYLIB_IOS_DIR}/src/raudio.c
    PROPERTIES COMPILE_FLAGS "-x objective-c"
)

add_library(raylib STATIC ${RAYLIB_SOURCES})

target_compile_definitions(raylib PUBLIC
    PLATFORM_IOS
    GRAPHICS_API_OPENGL_ES3
    GL_GLEXT_PROTOTYPES
)

target_include_directories(raylib PUBLIC
    ${RAYLIB_IOS_DIR}/src
    ${INC_STAGE}
    ${ANGLE_EGL_HDRS}
    ${ANGLE_GLES_HDRS}
)

target_compile_options(raylib PRIVATE
    "-include" "${INC_STAGE}/libGLESv2/GLES/gl.h"
)

# Fuentes del juego Astrallitic
set(GAME_SOURCES
    src/main.c
    src/audio.c
    src/graphics.c
    src/mobile_input.c
    src/platform_paths.c
    src/ios_paths.m
)

# Assets del juego
file(GLOB GAME_ASSETS "assets/*")

# Target de aplicación iOS
add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    ${GAME_SOURCES}
    ${GAME_ASSETS}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLATFORM_IOS
    GRAPHICS_API_OPENGL_ES3
    GL_GLEXT_PROTOTYPES
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${RAYLIB_IOS_DIR}/src"
    ${INC_STAGE}
    ${ANGLE_EGL_HDRS}
    ${ANGLE_GLES_HDRS}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    "-include" "${INC_STAGE}/libGLESv2/GLES/gl.h"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    raylib
    "${ANGLE_EGL_XCFW}"
    "${ANGLE_GLES_XCFW}"
    "-framework UIKit"
    "-framework Foundation"
    "-framework CoreGraphics"
    "-framework QuartzCore"
    "-framework AVFoundation"
    "-framework AudioToolbox"
    "-framework OpenGLES"
)

# Propiedades del bundle iOS
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.alejadev.astrallitic"
    MACOSX_BUNDLE_BUNDLE_NAME "Astrallitic"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.2"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.2"
    XCODE_ATTRIBUTE_PRODUCT_NAME "Astrallitic"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "15.0"
    XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "iphoneos iphonesimulator"
    XCODE_ATTRIBUTE_UIRequiresFullScreen "YES"
    XCODE_ATTRIBUTE_UIStatusBarHidden "YES"
    XCODE_ATTRIBUTE_UISupportedInterfaceOrientations "UIInterfaceOrientationLandscapeLeft UIInterfaceOrientationLandscapeRight"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
    XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO"
    XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
    XCODE_ATTRIBUTE_OTHER_CFLAGS "-include ${INC_STAGE}/libGLESv2/GLES/gl.h"
    XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "$(inherited) @executable_path/Frameworks"
    XCODE_EMBED_FRAMEWORKS "${ANGLE_EGL_XCFW};${ANGLE_GLES_XCFW}"
    XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY "YES"
    XCODE_EMBED_FRAMEWORKS_REMOVE_HEADERS_ON_COPY "YES"
    RESOURCE "${GAME_ASSETS}"
)