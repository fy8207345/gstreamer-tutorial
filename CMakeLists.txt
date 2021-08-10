cmake_minimum_required(VERSION 3.16)
project(gstreamer_01 C)

set(CMAKE_C_STANDARD 99)

set(GTK C:/gtk-build/gtk/Win32/release/include/gtk-3.0)
set(PANGO C:/gtk-build/gtk/Win32/release/include/pango-1.0)
set(HARFBUZZ C:/gtk-build/gtk/Win32/release/include/harfbuzz)
set(CAIRO C:/gtk-build/gtk/Win32/release/include/cairo)
set(PIXBUF C:/gtk-build/gtk/Win32/release/include/gdk-pixbuf-2.0)
set(ATK C:/gtk-build/gtk/Win32/release/include/atk-1.0)

include_directories($ENV{GSTREAMER_ROOT_X86}/include $ENV{GSTREAMER_ROOT_X86}/include/glib-2.0 $ENV{GSTREAMER_ROOT_X86}/include/glib-2.0/glib $ENV{GSTREAMER_ROOT_X86}/include/gstreamer-1.0 $ENV{GSTREAMER_ROOT_X86}/lib/glib-2.0/include)
include_directories(${GTK} ${PANGO} ${HARFBUZZ} ${CAIRO} ${PIXBUF} ${ATK})

link_directories($ENV{GSTREAMER_ROOT_X86}/lib/)
link_directories(C:/gtk-build/gtk/Win32/release/lib)
link_libraries(gstreamer-1.0.lib gobject-2.0.lib glib-2.0.lib gstvideo-1.0.lib cairo.lib cairo-gobject.lib gstaudio-1.0.lib gstpbutils-1.0.lib)
link_libraries(gdk-3.lib gtk-3.lib)

add_executable(gstreamer_01 tutorial-09.c tutorial-12.c)