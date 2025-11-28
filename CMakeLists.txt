cmake_minimum_required(VERSION 3.20)
project(ArcadeGame C)

set(CMAKE_C_STANDARD 11)

# Явно указываем pkg-config
set(PKG_CONFIG_EXECUTABLE "C:/msys64/ucrt64/bin/pkg-config.exe")
find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK4 REQUIRED gtk4)


add_compile_options(${GTK4_CFLAGS_OTHER})
include_directories(${GTK4_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.c
        menu.c
        appMenu.c
        appMinesweeper.c
        utils.c
        appSettings.c
)
target_link_libraries(${PROJECT_NAME} ${GTK4_LIBRARIES})