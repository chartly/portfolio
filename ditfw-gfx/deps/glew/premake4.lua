--/************************************************************************
--*
--* Corbin Hart © (2008-201x)
--* All rights reserved.
--*
--************************************************************************/

project "glew"

	SetupNativeDependencyProject()
		
	local version = "1.9.0"
	local repo = "https://github.com/martell/glew"
	
  kind "StaticLib"
  
  files 
  {
  	"glew/include/gl/glew.h",
  	"glew/src/*.c"
  }

  includedirs { "glew/include" }
  
  defines { "GLEW_BUILD", "GLEW_STATIC", "GLEW_MX" }

  configuration "windows"
  	files { "glew/include/gl/wglew.h" }

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