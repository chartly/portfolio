
project("glimg")

	SetupNativeDependencyProject()

	kind "StaticLib"
	language "c++"

	includedirs {"include", "source", "../glload/include", BoostDir()}

	files {
		"include/glimg/*.h",
		"source/*.h",
		"source/*.cpp",
		"source/*.c",
		"source/*.inc",
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
