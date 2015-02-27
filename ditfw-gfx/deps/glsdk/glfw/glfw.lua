
project "glfw"

	SetupNativeDependencyProject()

	kind "StaticLib"
	language "c"

	includedirs {"include", "lib"}
	files {"lib/*.c"};
	
	defines {"_LIB"}
	
	configuration "windows"
		defines "WIN32"
		files {"lib/win32/*.c"};
		includedirs {"lib/win32"}
		
	configuration "linux"
	    defines {"_GLFW_USE_LINUX_JOYSTICKS", "_GLFW_HAS_XRANDR",
	        "_GLFW_HAS_PTHREAD", "_GLFW_HAS_SYSCONF", "_GLFW_HAS_SYSCTL",
	        "_GLFW_HAS_GLXGETPROCADDRESS"}
	    includedirs { "/usr/X11/include" }
		files {"lib/x11/*.c"};
		includedirs {"lib/x11"}
		
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
