cmake_minimum_required(VERSION 3.19)

set(CMAKE_TOOLCHAIN_FILE "$ENV{SCE_PSP2_SDK_DIR}/host_tools/build/cmake/psp2-snc-toolchain.cmake")

project(glfw3 LANGUAGES C)

include(VitaDevelopmentSuite)
set(CMAKE_C_STANDARD 99)

add_compile_options(
  -Xdiag=0 -Xquit=2 -O3
)

add_link_options(
  -s
)

add_compile_definitions(
  _GLFW_VITA
  _GLFW_EGL_STATIC
)

include_directories(
  ${VDSUITE_USER_INCLUDE_DIRECTORIES}
  ${VDSUITE_STANDARD_INCLUDE_DIRECTORIES}
  include
  src
)

link_directories(
  ${VDSUITE_LIBRARY_DIRECTORIES}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(${PROJECT_NAME} STATIC
  src/context.c
  src/egl_context.c
  src/init.c
  src/input.c
  src/monitor.c
  src/posix_thread.c
  src/vita_context.c
  src/vita_init.c
  src/vita_joystick.c
  src/vita_monitor.c
  src/vita_time.c
  src/vita_window.c
  src/window.c
  src/xkb_unicode.c
)

install(DIRECTORY include/GLFW
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/vdsuite/user
)

install(TARGETS ${PROJECT_NAME}
  DESTINATION ${VDSUITE_LIBRARY_DIRECTORIES}
)