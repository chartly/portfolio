--/************************************************************************
--*
--* Corbin Hart © (2008-201x)
--* All rights reserved.
--*
--************************************************************************/

project "ditfw-profiler"
	SetupNativeDependencyProject()

	kind "StaticLib"

	files
	{
		"*profiler.lua"
		, "profiler/Profiler.h"
		, "profiler/Profiler.cpp"

		-- reader / writer queue
		, depsdir .. "/moodycamel/*.h"
	}

	includedirs
	{
		-- hack needed because this script is an extra dir deeper than the invoking "src/premake4.lua"
		"../sample"
		
		-- collapse header dir structure
		, "profiler"
		, "platform"
		, Core.includedirs
		
		-- reader / writer queue
		, depsdir .. "/moodycamel/spsc"
		, depsdir .. "/moodycamel/mpmc"
	}

	deps
	{
		"ditfw-core"
		, Core.deps
	}
	
	libdirs { Core.libdirs }
	
	defines
	{
		-- inherit core framework defines
		Core.defines
	}

	configuration "windows"
		includedirs( Core.msvcincludedirs );
		defines( Core.msvcdefines );
		links( Core.msvclinks );
