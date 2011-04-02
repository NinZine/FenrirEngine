solution "FenrirEngine"
    configurations { "Debug", "Release" }
    platforms { "native" }
	location "projects"

	local INCLUDE_PATH = "/f/MinGW/include"
	local LIB_PATH = "/f/MinGW/lib"
	local SDL_PATH = "/f/dev/SDL-1.2.4"
	local OPENAL_PATH = "/f/dev/OpenAL 1.1 SDK"
	
    project "FenrirEngine"
		kind "ConsoleApp"
		language "C"

	configuration { "gmake" }
		if true == os.is("windows") then
		    buildoptions { "-Wall" }
		    defines { "__SDL__", "WIN32" }
		    files { "source/*.c", "source/sdl/*.c", "source/win/*.c",
			    "source/lua_wrap/*.c" }
		    includedirs {"include", "lib", "lib/lua", "lib/png",
				SDL_PATH .. "/include", "/usr/local/include",
				INCLUDE_PATH, OPENAL_PATH .. "/include" }
		    libdirs { SDL_PATH .. "/lib", "/usr/local/lib", LIB_PATH,
				OPENAL_PATH .. "/libs/Win32" }
		    links { "mingw32", "SDLmain", "SDL", "opengl32", "openal32", "ws2_32",
				"m", "z", "lua", "png", "ogg_vorbis" }
		    linkoptions { "-Wl,--subsystem,console", "-Wl,-u,_WinMain@16" }

		elseif true == os.is("linux") then
			buildoptions {"-Wall"}
			defines {"__SDL__"}
		    files { "source/*.c", "source/sdl/*.c", "source/win/*.c",
			    "source/lua_wrap/*.c" }
			includedirs {"include", "lib", "lib/lua", "lib/png", INCLUDE_PATH }
			libdirs { LIB_PATH }
			links {"SDLmain", "SDL", "GL", "openal", "m", "z", "lua", "png",
				"ogg", "vorbis", "vorbisfile" }
		end

	configuration { "gmake", "Debug" }
	    defines { "DEBUG" }
	    buildoptions { "-gdwarf-2" }

	configuration { "xcode3" }
		files { "include/**.h", "source/*.c", "source/sdl/*.c", "source/osx/*.m",
		"source/lua_wrap/*.c" }

		buildoptions {"-Wall"}
		defines {"__SDL__"}
		includedirs {"include", "lib", "lib/lua", "lib/png",
		"lib/freetype-2.4.3/include", INCLUDE_PATH }
		libdirs { LIB_PATH }
		links {"lua", "Cocoa.framework", "SDL.framework", "OpenGL.framework",
			"OpenAL.framework", "m", "z", "png", "ogg_vorbis", "freetype" }
	
	--[[ TODO: Needs to be updated (by someone that uses it). Compiler still needs
	to be MinGW though ]]--
	configuration { "vs2008", "vs2005" }
	    defines { "__SDL__", "WIN32", "_USE_MATH_DEFINES" }
	    files { "source/*.c", "source/sdl/*.c", "source/win/*.c",
		    "source/lua_wrap/*.c" }
	    includedirs {"include", "D:/dev/SDL-1.2.14/include/win32",
		    "D:/dev/lua-5.1.4-30/include",
		    "D:/dev/OpenAL 1.1 SDK/include" }
	    libdirs { "D:/dev/SDL-1.2.14/lib", "D:/dev/lua-5.1.4-30/lib",
		   "D:/dev/OpenAL 1.1 SDK/libs/Win32" }
	    links { "SDLmain", "opengl32", "openal32", "SDL" }
	    linkoptions {  }


	-- Dependencies
	project "lua"
		kind "StaticLib"
		language "C"
		files { "lib/lua/*.c" }
		includedirs { "include" }

	project "png"
		kind "StaticLib"
		language "C"
		includedirs { INCLUDE_PATH }
		files { "lib/png/*.c" }

	if false == os.is("linux") then
	project "ogg_vorbis"
		kind "StaticLib"
		language "C"
		files { "lib/ogg/*.c", "lib/vorbis/**.c" }
		includedirs { "lib/ogg", "lib/vorbis" }
	end

	project "freetype"
		kind "StaticLib"
		language "C"
		defines { "DARWIN_NO_CARBON", "FT2_BUILD_LIBRARY" }
		files {
			"lib/freetype-2.4.3/src/autofit.c",
			"lib/freetype-2.4.3/src/ftbase.c",
			"lib/freetype-2.4.3/src/ftbbox.c",
			"lib/freetype-2.4.3/src/ftbitmap.c",
			"lib/freetype-2.4.3/src/ftcid.c",
			"lib/freetype-2.4.3/src/ftdebug.c",
			"lib/freetype-2.4.3/src/ftglyph.c",
			"lib/freetype-2.4.3/src/ftinit.c",
			"lib/freetype-2.4.3/src/ftsystem.c",
			"lib/freetype-2.4.3/src/pshinter.c",
			"lib/freetype-2.4.3/src/psnames.c",
			"lib/freetype-2.4.3/src/sfnt.c",
			"lib/freetype-2.4.3/src/smooth.c",
			"lib/freetype-2.4.3/src/truetype.c",
		}
		includedirs { "lib/freetype-2.4.3/include" }

