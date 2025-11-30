cmake_minimum_required(VERSION 3.20)
project(ArcadeGame C)

set(CMAKE_C_STANDARD 11)

# Set pkg-config explicitly
set(PKG_CONFIG_EXECUTABLE "C:/msys64/ucrt64/bin/pkg-config.exe")
find_package(PkgConfig REQUIRED)

# Find GTK4
pkg_check_modules(GTK4 REQUIRED gtk4)

# Find GStreamer 1.0
pkg_check_modules(GST REQUIRED gstreamer-1.0)

# Add include directories
include_directories(
        ${GTK4_INCLUDE_DIRS}
        ${GST_INCLUDE_DIRS}
)

# Add compile flags
add_compile_options(
        ${GTK4_CFLAGS_OTHER}
        ${GST_CFLAGS_OTHER}
)

# Add linker directories
link_directories(
        ${GTK4_LIBRARY_DIRS}
        ${GST_LIBRARY_DIRS}
)

# ----------------------------
# ИКОНКА ДЛЯ WINDOWS EXE
# ----------------------------
set(APP_ICON_RESOURCE "")
if (WIN32)
    set(POTENTIAL_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon.rc")
    if (EXISTS "${POTENTIAL_ICON}")
        set(APP_ICON_RESOURCE "${POTENTIAL_ICON}")
        message(STATUS "Windows icon reesource found: ${APP_ICON_RESOURCE}")
    else()
        message(WARNING "Windows icon resource not found: ${POTENTIAL_ICON}")
    endif()
endif()

# ----------------------------
# Добавляем исходники
# ----------------------------
add_executable(${PROJECT_NAME}
        main.c
        apps/appMenu.c
        apps/appMinesweeper.c
        utils/utils.c
        apps/appSettings.c
        utils/musicUtils.c
        utils/imageUtils.c
        ${APP_ICON_RESOURCE}
)

# Link GTK4 first
target_link_libraries(${PROJECT_NAME}
        ${GTK4_LIBRARIES}
)

# Then GStreamer
target_link_libraries(${PROJECT_NAME}
        ${GST_LIBRARIES}
)
