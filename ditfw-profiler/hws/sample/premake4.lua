--/************************************************************************
--*
--* Corbin Hart © (2008-201x)
--* All rights reserved.
--*
--************************************************************************/

dofile "ditfw-profiler.lua"

project "sample"
	SetupNativeProjects()

	kind "ConsoleApp"

	vectorextensions "SSE2"
	flags { "Unicode" }
	buildoptions { "/Gh", "/GH" }

	configuration "Debug"
		floatingpoint "Strict"

	configuration "Release"
		floatingpoint "Fast"
		--flags { "LinkTimeOptimization" }
		buildoptions { "/O2" }

	configuration{}

	files
	{
		-- local assignment files
		"premake4.lua"
		, "*.h"
		, "*.cpp"
		, "platform/**.h"
		, "platform/**.cpp"

		-- profiler hooks
		, "profiler/*Instrumentation*"
	}

	includedirs
	{
		-- hack needed because this script is an extra dir deeper than the invoking "src/premake4.lua"
		"../profiler"
		
		-- collapse header dir structure
		, "profiler"
		, "platform"
		, "platform/DXUT/Core"
		, "platform/DXUT/Optional"
		, Core.includedirs
	}

	deps
	{
		"ditfw-core"
		, "ditfw-profiler"
		, Core.deps
	}
	
	defines
	{
		-- inherit core framework defines
		Core.defines

		-- pch flag
		--, "DIT_USE_PCH"

		-- memory debugging
		, "DIT_USE_INSTRUMENTATION"
	}

	libdirs { Core.libdirs }

	links
	{
		"d3dcompiler"
		, "dxguid"
		, "winmm"
		, "comctl32"
		, "usp10"
	}

	configuration "windows"
		includedirs( Core.msvcincludedirs );
		defines( Core.msvcdefines );
		links( Core.msvclinks );
