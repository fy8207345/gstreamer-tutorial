cmake_minimum_required(VERSION 3.16)
project(gstreamer_01 C)

set(CMAKE_C_STANDARD 99)

include_directories(C:/gstreamer/1.0/mingw_x86/include C:/gstreamer/1.0/mingw_x86/include/glib-2.0 C:/gstreamer/1.0/mingw_x86/include/glib-2.0/glib C:/gstreamer/1.0/mingw_x86/include/gstreamer-1.0 C:/gstreamer/1.0/mingw_x86/lib/glib-2.0/include)

link_directories(C:/gstreamer/1.0/mingw_x86/lib)
link_libraries(gobject-2.0.lib glib-2.0.lib gstreamer-1.0.lib)

add_executable(gstreamer_01 tutorial-01.c tutorial-02.c tutorial-03.c tutorial-04.c)