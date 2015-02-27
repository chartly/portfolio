--/************************************************************************
--*
--* Corbin Hart © (2008-201x)
--* All rights reserved.
--*
--************************************************************************/

project "filewatcher"

	SetupNativeDependencyProject()
		
	local version = "1.0"
	local repo = "https://code.google.com/p/simplefilewatcher/"
	
  kind "StaticLib"
  
  files 
  {
  	"include/FileWatcher/FileWatcher.h",
  	"src/FileWatcher.cpp"
  }

  includedirs { "include" }

  configuration "Debug"
    flags{ "Symbols" }
    defines
    { 
      "_HAS_ITERATOR_DEBUGGING=1"
      , "_SECURE_SCL=1"
      , "_ITERATOR_DEBUG_LEVEL=2"
    }

  configuration "Release"
    flags { "Optimize" }

  configuration "windows"
  	files
    {
      "include/FileWatcher/FileWatcherWin32.h"
      , "src/FileWatcherWin32.cpp"
    }