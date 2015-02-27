--/************************************************************************
--*
--* Corbin Hart © (2008-201x)
--* All rights reserved.
--*
--************************************************************************/

project "wxpropgrid"
	SetupNativeDependencyProject()

	kind "StaticLib"

	pchheader "wx/wxprec.h"
	pchsource "src/common/dummy.cpp"

	files
	{
		-- propgrid files
		"include/wx/propgrid/*.h"
		, "src/propgrid/*.cpp"

		-- wx pch files
		, "include/wx/wxprec.h"
		, "src/common/dummy.cpp"
	}

	includedirs
	{
		"include"
	}

	defines
	{
		"WXBUILDING"
		, "USE_OPENGL=1"
		, "wxMONOLITHIC=1"
		, "wxUSE_BASE=1"
		, "wxUSE_GUI=1"
		, "wxUSE_AUI=1"
		, "wxUSE_PROPGRID=1"
		, "wxUSE_GLCANVAS=1"
	}

	configuration "Debug"
		defines
		{ 
			"_HAS_ITERATOR_DEBUGGING=1"
			, "_SECURE_SCL=1"
			, "_ITERATOR_DEBUG_LEVEL=2"
		}
		flags { "Symbols" }

	configuration "Release"
		flags{ "Optimize" }

	configuration "windows"
		includedirs { "include/msvc" }

		defines
		{
			"__WXMSW__"
			, "wxSETUPH_PATH=wx/msw/setup.h"
		}