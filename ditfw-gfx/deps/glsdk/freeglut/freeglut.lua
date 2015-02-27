if(_ACTION == "gmake") then
	if(os.get() == "linux" or os.get() == "bsd") then
		os.execute("sh ./configure");
	end
end

project "freeglut"

	SetupNativeDependencyProject()

	kind "StaticLib"
	language "c"
	
	includedirs {"include"}
	files {"src/*.c"};
	
	defines {"FREEGLUT_STATIC", "FREEGLUT_LIB_PRAGMAS=0"}
	
	configuration "windows"
		defines "WIN32"

	configuration {"gmake", "linux or bsd"}
        defines {"HAVE_CONFIG_H", }
        includedirs {"."}

	configuration {"linux or bsd"}
        defines {"HAVE_CONFIG_H", }
        includedirs {"."}
		
	configuration "Debug"
		defines
		{
			"DEBUG"
			, "_DEBUG"
			,"_HAS_ITERATOR_DEBUGGING=1"
			, "_SECURE_SCL=1"
			, "_ITERATOR_DEBUG_LEVEL=2"
		}

		flags { "Symbols" }

	configuration "Release"
		defines {"NDEBUG", "RELEASE"}
		flags { "Optimize" }

