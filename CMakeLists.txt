cmake_minimum_required(VERSION 3.16)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "Vita toolchain")
  else()
    message(FATAL_ERROR "Define VITASDK antes de compilar")
  endif()
endif()
project(rovita C)
include("$ENV{VITASDK}/share/vita.cmake" REQUIRED)
add_executable(rovita src/main.c src/game.c)
target_compile_features(rovita PRIVATE c_std_11)
target_compile_options(rovita PRIVATE -Wall -Wextra -O2)
target_link_libraries(rovita vita2d SceDisplay_stub SceGxm_stub SceSysmodule_stub
  SceCtrl_stub ScePgf_stub ScePvf_stub SceCommonDialog_stub SceAppMgr_stub
  SceIofilemgr_stub freetype png16 jpeg bz2 z m)
vita_create_self(eboot.bin rovita)
vita_create_vpk(rovita.vpk ROVT00001 eboot.bin VERSION 01.00 NAME "RO Vita Prototype"
  FILE licenses/libvita2d-MIT.txt licenses/libvita2d-MIT.txt
       licenses/COPYING.NEWLIB licenses/COPYING.NEWLIB)
