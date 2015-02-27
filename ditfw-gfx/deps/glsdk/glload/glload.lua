
newoption {
	trigger			= "plainc",
	description		= "Set this to build GL Load without C++ support.",
}


project("glload")

	SetupNativeDependencyProject()

	kind "StaticLib"

	includedirs {"include", "source"}

	if(_OPTIONS["plainc"]) then
		language "c"
	else
		language "c++"
	end

	files {
		"include/glload/gl_*.h",
		"include/glload/gl_*.hpp",
		"include/glload/gll.h",
		"include/glload/gll.hpp",
		"source/gl_*",
	};
	
	configuration "plainc"
		excludes {
			"source/*.cpp",
			"include/glload/*.hpp",
		}
	
	configuration "windows"
		defines {"WIN32"}
		files {"include/glload/wgl_*.h",}
		files {"source/wgl_*"}
	
	configuration "linux"
	    defines {"LOAD_X11"}
		files {"include/glload/glx_*.h"}
		files {"source/glx_*"}

	configuration "Debug"
		defines
		{
			"DEBUG"
			, "_DEBUG"
			, "MEMORY_DEBUGGING"
			,"_HAS_ITERATOR_DEBUGGING=1"
			, "_SECURE_SCL=1"
			, "_ITERATOR_DEBUG_LEVEL=2"
		}

		flags { "Symbols" }
 
	configuration "Release"
		defines {"NDEBUG", "RELEASE"}
		flags { "Optimize" }
