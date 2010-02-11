solution "ConceptEngine"
    configurations { "Debug", "Release" }
    platforms { "native" }

    project "ConceptEngine"
	kind "ConsoleApp"
	language "C"
	
	configuration { "gmake" }
	    buildoptions { "-mno-cygwin", "-Wall" }
	    defines { "__SDL__", "WIN32" }
	    files { "source/*.c", "source/sdl/*.c", "source/win/*.c",
		    "source/lua_wrap/*.c" }
	    includedirs {"include", "/usr/local/include",
		    "/cygdrive/d/dev/lua-5.1.4/src",
		    "/cygdrive/d/MinGW/include"}
	    libdirs { "/usr/local/lib", "/cygdrive/d/dev/lua-5.1.4/src",
		    "/cygdrive/d/MinGW/lib" }
	    links { "mingw32", "SDLmain", "opengl32", "openal32", "SDL",
	    "lua", "cygwin" }
	    linkoptions { "-mwindows", "-mno-cygwin" }

	configuration { "gmake", "Debug" }
	    defines { "DEBUG" }
	    buildoptions { "-g" }

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


