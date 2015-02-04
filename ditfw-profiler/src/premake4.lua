--/************************************************************************
--*
--* Corbin Hart © (2008-201x)
--* All rights reserved.
--*
--************************************************************************/

Core = {}
Core.libdirs = {}
Core.deps = {}

-- global config
Core.inlcudedirs = {}
Core.defines = {}
Core.links = {}

-- msw configs
Core.msvcincludedirs = {}
Core.msvcdefines = {}
Core.msvclinks = {}

project "ditfw-core"
	SetupNativeDependencyProject()
	
	kind "StaticLib"

	pchheader "API.h"
	pchsource "core/Core.cpp"

	files
	{
		"*.lua"
		, "**.cpp"
		, "**.hpp"
		, "**.h"

		-- miniformat
		, depsdir .. "/miniformat/**.h"

		-- stackwalker
		, depsdir .. "/StackWalker/StackWalker.h"
		, depsdir .. "/StackWalker/StackWalker.cpp"
	}

	Core.includedirs = 
	{
		-- collapse header dir structure
		srcdir
		, srcdir .. "/core"

		-- deps folders
		, depsdir .. "/miniformat"
		, depsdir .. "/StackWalker"
	}

	Core.excludes = 
	{
		"ConcurrentQueue.h"
		, "ConcurrentQueue.cpp"
	}

	Core.deps = {}

	Core.libdirs = {}

	Core.defines = 
	{
		-- ditfw defines
		"DIT_USE_PCH"
	}

	configuration "windows"
		Core.msvcincludedirs = {}
		Core.msvcdefines = {}

		Core.msvclinks = {}

		includedirs( Core.msvcincludedirs );
		defines( Core.msvcdefines );
		links( Core.msvclinks );

	configuration {}

		includedirs( Core.includedirs );
		defines( Core.defines );
		libdirs( Core.libdirs );
		links( Core.links );
		deps( Core.deps ) ;