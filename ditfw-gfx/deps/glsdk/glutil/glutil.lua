
project("glutil")

	SetupNativeDependencyProject()

	kind "StaticLib"
	language "c++"
	
	includedirs {
		"include",
		"source",
		"../glload/include",
		"../glm",
		BoostDir()
	}
	

	files {
		"include/glutil/*.h",
		"source/*.cpp",
		"source/*.h",
	};
	
	configuration "windows"
		defines {"WIN32"}
	
	configuration "linux"
	    defines {"LOAD_X11"}

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
