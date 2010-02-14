solution "ConceptEngine"
    configurations { "Debug", "Release" }
    platforms { "native" }

    project "ConceptEngine"
	kind "ConsoleApp"
	language "C"
	
	configuration { "gmake" }
	    buildoptions { "-Wall", "-mno-cygwin" }
	    defines { "__SDL__", "WIN32" }
	    files { "source/*.c", "source/sdl/*.c", "source/win/*.c",
		    "source/lua_wrap/*.c" }
	    includedirs {"include", "/cygdrive/d/dev/SDL-1.2.14/lib", "/usr/local/include",
		    "/cygdrive/d/dev/lua-5.1.4/src",
		    "/cygdrive/d/MinGW/include"
		    }
	    libdirs { "/cygdrive/d/dev/SDL-1.2.14/lib", "/usr/local/lib", "/cygdrive/d/dev/lua-5.1.4/src",
		    "/cygdrive/d/MinGW/lib"
		    }
	    links { "mingw32", "SDLmain", "SDL", "lua", "opengl32", "openal32",
	    "m", }
	    linkoptions { "-mno-cygwin", "-Wl,--subsystem,console", "-Wl,-u,_WinMain@16" }

	configuration { "gmake", "Debug" }
	    defines { "DEBUG" }
	    buildoptions { "-gdwarf-2" }

	configuration { "vs2008" }
	    defines { "__SDL__", "WIN32", "_USE_MATH_DEFINES" }
	    files { "source/*.c", "source/sdl/*.c", "source/win/*.c",
		    "source/lua_wrap/*.c" }
	    includedirs {"include", "D:/dev/SDL-1.2.14/include/win32",
		    "D:/dev/lua-5.1.4-30/include",
		    "D:/dev/OpenAL 1.1 SDK/include" }
	    libdirs { "D:/dev/SDL-1.2.14/lib", "D:/dev/lua-5.1.4-30/lib",
		   "D:/dev/OpenAL 1.1 SDK/libs/Win32" }
	    links { "SDLmain", "opengl32", "openal32", "SDL",
	    "lua51" }
	    linkoptions {  }


