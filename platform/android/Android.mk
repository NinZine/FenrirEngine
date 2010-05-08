# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LUA_SOURCE_PATH := /home/nihilist/development/sdk/lua-5.1.4/src
PNG_SOURCE_PATH := /home/nihilist/development/sdk/libpng-1.4.1
SYSTEM_INCLUDE_PATH := /usr/include

SAVED_PATH := $(call my-dir)

######################### LUA

LOCAL_PATH := $(LUA_SOURCE_PATH)
LOCAL_MODULE    := lua 
LOCAL_SRC_FILES := ../etc/lua.hpp \
   lapi.c \
   lauxlib.c \
   lbaselib.c \
   lcode.c \
   ldblib.c \
   ldebug.c \
   ldo.c \
   ldump.c \
   lfunc.c \
   lgc.c \
   linit.c \
   liolib.c \
   llex.c \
   lmathlib.c \
   lmem.c \
   loadlib.c \
   lobject.c \
   lopcodes.c \
   loslib.c \
   lparser.c \
   lstate.c \
   lstring.c \
   lstrlib.c \
   ltable.c \
   ltablib.c \
   ltm.c \
   lua.c \
   luac.c \
   lundump.c \
   lvm.c \
   lzio.c \
   print.c 
include $(BUILD_STATIC_LIBRARY) 
######################### !LUA

include $(CLEAR_VARS)
######################### PNG

LOCAL_PATH := $(PNG_SOURCE_PATH)
LOCAL_MODULE    := png
LOCAL_SRC_FILES := png.c \
   pngerror.c \
   pngget.c \
   pngmem.c \
   pngpread.c \
   pngread.c \
   pngrio.c \
   pngrtran.c \
   pngrutil.c \
   pngset.c \
   pngtest.c \
   pngtrans.c \
   pngwio.c \
   pngwrite.c \
   pngwtran.c \
   pngwutil.c

include $(BUILD_STATIC_LIBRARY) 
######################### !PNG
include $(CLEAR_VARS)

######################### ConceptEngine
LOCAL_PATH := /home/nihilist/development/sdk/lua-5.1.4/src
LOCAL_PATH := $(SAVED_PATH)

LOCAL_CFLAGS    := -Werror
LOCAL_LDLIBS    := -llog -lGLESv1_CM -lz

LOCAL_MODULE     := concept-engine
LOCAL_SRC_FILES  := ../../source/android/concept-engine.c \
../../source/lua_wrap/mat4_wrap.c 	\
../../source/lua_wrap/quat_wrap.c 	\
../../source/lua_wrap/vec3_wrap.c 	\
../../source/lua_wrap/render_wrap.c 	\
../../source/lua_wrap/image_wrap.c 	\
../../source/lua_wrap/event_wrap.c 	\
../../source/lua_wrap/sound_wrap.c 	\
../../source/lua_wrap/blender_wrap.c 	\
../../source/lua_wrap/net_wrap.c 	\
../../source/mat4.c			\
../../source/quat.c			\
../../source/vec3.c			\
../../source/rendering.c		\
../../source/image.c			\
../../source/system.c			\
../../source/event.c			\
../../source/android/sound.c		\
../../source/net.c			\
../../source/blender.c			\
../../source/log.c

LOCAL_CFLAGS     := -D__ANDROID__

LOCAL_C_INCLUDES := $(LUA_SOURCE_PATH) 	\
	$(PNG_SOURCE_PATH)		\
	$(SYSTEM_INCLUDE_PATH)		\
	$(SAVED_PATH)/../../include \

LOCAL_STATIC_LIBRARIES := lua png

include $(BUILD_SHARED_LIBRARY)
