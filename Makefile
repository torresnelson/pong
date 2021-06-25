#**************************************************************************************************
#
#   raylib makefile for Desktop platforms, Raspberry Pi, Android and HTML5
#
#   Copyright (c) 2013-2020 Ramon Santamaria (@raysan5)
#
#   This software is provided "as-is", without any express or implied warranty. In no event
#   will the authors be held liable for any damages arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose, including commercial
#   applications, and to alter it and redistribute it freely, subject to the following restrictions:
#
#     1. The origin of this software must not be misrepresented; you must not claim that you
#     wrote the original software. If you use this software in a product, an acknowledgment
#     in the product documentation would be appreciated but is not required.
#
#     2. Altered source versions must be plainly marked as such, and must not be misrepresented
#     as being the original software.
#
#     3. This notice may not be removed or altered from any source distribution.
#
#**************************************************************************************************

.PHONY: all clean

# Define required raylib variables
PROJECT_NAME       ?= Game
RAYLIB_VERSION     ?= 3.0.0
RAYLIB_API_VERSION ?= 3
RAYLIB_PATH        ?= /home/titoreboot/raylib/src/

# Define default options

# One of PLATFORM_DESKTOP, PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
PLATFORM           ?= PLATFORM_DESKTOP

# Locations of your newly installed library and associated headers. See ../src/Makefile
# On Linux, if you have installed raylib but cannot compile the examples, check that
# the *_INSTALL_PATH values here are the same as those in src/Makefile or point to known locations.
# To enable system-wide compile-time and runtime linking to libraylib.so, run ../src/$ sudo make install RAYLIB_LIBTYPE_SHARED.
# To enable compile-time linking to a special version of libraylib.so, change these variables here.
# To enable runtime linking to a special version of libraylib.so, see EXAMPLE_RUNTIME_PATH below.
# If there is a libraylib in both EXAMPLE_RUNTIME_PATH and RAYLIB_INSTALL_PATH, at runtime,
# the library at EXAMPLE_RUNTIME_PATH, if present, will take precedence over the one at RAYLIB_INSTALL_PATH.
# RAYLIB_INSTALL_PATH should be the desired full path to libraylib. No relative paths.
DESTDIR ?= /usr/local
RAYLIB_INSTALL_PATH ?= $(DESTDIR)/lib
# RAYLIB_H_INSTALL_PATH locates the installed raylib header and associated source files.
RAYLIB_H_INSTALL_PATH ?= $(DESTDIR)/include

# Library type used for raylib: STATIC (.a) or SHARED (.so/.dll)
RAYLIB_LIBTYPE        ?= STATIC

# Build mode for project: DEBUG or RELEASE
BUILD_MODE            ?= RELEASE

# Use external GLFW library instead of rglfw module
# TODO: Review usage on Linux. Target version of choice. Switch on -lglfw or -lglfw3
USE_EXTERNAL_GLFW     ?= FALSE

# Use Wayland display server protocol on Linux desktop
# by default it uses X11 windowing system
USE_WAYLAND_DISPLAY   ?= FALSE

# Determine PLATFORM_OS in case PLATFORM_DESKTOP selected
PLATFORM_OS=LINUX


# RAYLIB_PATH adjustment for different platforms.
# If using GNU make, we can get the full path to the top of the tree. Windows? BSD?
# Required for ldconfig or other tools that do not perform path expansion.
RAYLIB_PREFIX ?= ..
RAYLIB_PATH    = $(realpath $(RAYLIB_PREFIX))

# Define raylib release directory for compiled library.
# RAYLIB_RELEASE_PATH points to provided binaries or your freshly built version
RAYLIB_RELEASE_PATH 	?= $(RAYLIB_PATH)/src

# EXAMPLE_RUNTIME_PATH embeds a custom runtime location of libraylib.so or other desired libraries
# into each example binary compiled with RAYLIB_LIBTYPE=SHARED. It defaults to RAYLIB_RELEASE_PATH
# so that these examples link at runtime with your version of libraylib.so in ../release/libs/linux
# without formal installation from ../src/Makefile. It aids portability and is useful if you have
# multiple versions of raylib, have raylib installed to a non-standard location, or want to
# bundle libraylib.so with your game. Change it to your liking.
# NOTE: If, at runtime, there is a libraylib.so at both EXAMPLE_RUNTIME_PATH and RAYLIB_INSTALL_PATH,
# The library at EXAMPLE_RUNTIME_PATH, if present, will take precedence over RAYLIB_INSTALL_PATH,
# Implemented for LINUX below with CFLAGS += -Wl,-rpath,$(EXAMPLE_RUNTIME_PATH)
# To see the result, run readelf -d core/core_basic_window; looking at the RPATH or RUNPATH attribute.
# To see which libraries a built example is linking to, ldd core/core_basic_window;
# Look for libraylib.so.1 => $(RAYLIB_INSTALL_PATH)/libraylib.so.1 or similar listing.
EXAMPLE_RUNTIME_PATH   ?= $(RAYLIB_RELEASE_PATH)

# Define default C compiler: gcc
# NOTE: define g++ compiler if using C++
CC = gcc

# Define default make program: Mingw32-make
MAKE = make

# Define compiler flags:
#  -O1                  defines optimization level
#  -g                   include debug information on compilation
#  -s                   strip unnecessary data from build
#  -Wall                turns on most, but not all, compiler warnings
#  -std=c99             defines C language mode (standard C from 1999 revision)
#  -std=gnu99           defines C language mode (GNU C from 1999 revision)
#  -Wno-missing-braces  ignore invalid warning (GCC bug 53119)
#  -D_DEFAULT_SOURCE    use with -std=c99 on Linux and PLATFORM_WEB, required for timespec
CFLAGS += -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g
else
    CFLAGS += -s -O1
endif

# Additional flags for compiler (if desired)
#CFLAGS += -Wextra -Wmissing-prototypes -Wstrict-prototypes
ifeq ($(RAYLIB_LIBTYPE),STATIC)
    CFLAGS += -D_DEFAULT_SOURCE
endif
ifeq ($(RAYLIB_LIBTYPE),SHARED)
    # Explicitly enable runtime link to libraylib.so
    CFLAGS += -Wl,-rpath,$(EXAMPLE_RUNTIME_PATH)
endif

# Define include paths for required headers
# NOTE: Several external required libraries (stb and others)
 INCLUDE_PATHS = -I/home/titoreboot/Documents/Pong/include -I$(RAYLIB_H_INSTALL_PATH) -isystem. -isystem$(RAYLIB_PATH)/src -isystem$(RAYLIB_PATH)/release/include -isystem$(RAYLIB_PATH)/src/external

# Define library paths containing required libs.
LDFLAGS = -L. -L$(RAYLIB_RELEASE_PATH) -L$(RAYLIB_PATH)/src

LDFLAGS = -L. -L$(RAYLIB_INSTALL_PATH) -L$(RAYLIB_RELEASE_PATH)

# Define any libraries required on linking
# if you want to link libraries (libname.so or libname.a), use the -lname

# Libraries for Debian GNU/Linux desktop compiling
# NOTE: Required packages: libegl1-mesa-dev
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt

# On X11 requires also below libraries
LDLIBS += -lX11
# NOTE: It seems additional libraries are not required any more, latest GLFW just dlopen them
#LDLIBS += -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor

# On Wayland windowing system, additional libraries requires
ifeq ($(USE_WAYLAND_DISPLAY),TRUE)
    LDLIBS += -lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon
endif
# Explicit link to libc
ifeq ($(RAYLIB_LIBTYPE),SHARED)
    LDLIBS += -lc
endif

ifeq ($(USE_EXTERNAL_GLFW),TRUE)
    # NOTE: It could require additional packages installed: libglfw3-dev
    LDLIBS += -lglfw
endif

# Define all object files required
GAME = \
    source \

CURRENT_MAKEFILE = $(lastword $(MAKEFILE_LIST))

# Default target entry
all: $(GAME) 

# target
game: $(GAME)

# Generic compilation pattern
# NOTE: Examples must be ready for Android compilation!
%: %.c
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
	$(CC) -o $@$(EXT) $< $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)
endif

# Clean everything
clean:
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),LINUX)
		find . -type f -executable -delete
		rm -fv *.o
    endif
endif
	@echo Cleaning done.