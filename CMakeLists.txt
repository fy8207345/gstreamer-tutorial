cmake_minimum_required(VERSION 3.16)
project(gstreamer_01 C)

set(CMAKE_C_STANDARD 99)

include_directories($ENV{GSTREAMER_ROOT_X86}/include $ENV{GSTREAMER_ROOT_X86}/include/glib-2.0 $ENV{GSTREAMER_ROOT_X86}/include/glib-2.0/glib $ENV{GSTREAMER_ROOT_X86}/include/gstreamer-1.0 $ENV{GSTREAMER_ROOT_X86}/lib/glib-2.0/include)
include_directories(D:/msys64/mingw32/include/gtk-3.0 D:/msys64/mingw32/include/pango-1.0 D:/msys64/mingw32/include/harfbuzz D:/msys64/mingw32/include/cairo D:/msys64/mingw32/include/gdk-pixbuf-2.0 D:/msys64/mingw32/include/atk-1.0)

link_directories($ENV{GSTREAMER_ROOT_X86}/lib)
link_directories(D:/msys64/mingw32/lib)
link_libraries(gobject-2.0.lib glib-2.0.lib gstreamer-1.0.lib)

add_executable(gstreamer_01 tutorial-01.c tutorial-02.c tutorial-03.c tutorial-04.c tutorial-05.c)