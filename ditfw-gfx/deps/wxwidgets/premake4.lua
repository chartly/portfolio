--/************************************************************************
--*
--* Corbin Hart © (2008-201x)
--* All rights reserved.
--*
--************************************************************************/

project "wxWidgets"

	SetupNativeDependencyProject()
	DoNotBuildInNativeClient()
	
	kind "StaticLib"

	includedirs 
	{
		"include"
		, "src/png"
		, "../zlib/include"
	}
	
	deps { "zlib" }
	
	pchheader "wx/wxprec.h"
	pchsource "src/common/dummy.cpp"

	files { "src/common/dummy.cpp" }
			
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
		flags { "Optimize" }

	configuration { "windows" }
		defines
		{
			"__WXMSW__"
			, "wxSETUPH_PATH=wx/msw/setup.h"
		}

		includedirs
		{
			"include/msvc"
			, "include/wx/msw"
		}

		files 
		{
			"src/msw/glcanvas.cpp"
			, "include/msvc/wx/setup.h"
		}
		
		links
		{ 
			"kernel32"
			, "user32"
			, "gdi32"
			, "comdlg32"
			, "winspool"
			, "winmm"
			, "shell32"
			, "comctl32"
			, "ole32"
			, "oleauth32"
			, "uuid"
			, "rpcrt4"
			, "advapi32"
		}

	configuration "msvc"
		defines
		{
			"_CRT_SECURE_NO_WARNINGS"
			, "_CRT_SECURE_NO_DEPRECATE"
			, "_SCL_SECURE_NO_WARNINGS"
		}
		
	configuration {}
	
	dofile "wx.lua"
	
	excludes
	{
		"include/wx/arrimpl.cpp"
		, "include/wx/listimpl.cpp"
		, "include/wx/thrimpl.cpp"
		, "src/common/imagjpeg.cpp"
		, "src/common/imagtiff.cpp"
		, "src/common/regex.cpp"
	}

	files { "src/common/glcmn.cpp" }
	
	--libpng
	files { "src/png/png*.c", }
	excludes { "src/png/pngtest.c", }
	configuration "src/png/*.c"
		flags { "NoPCH" }
	
	configuration "**/extended.c"
		flags { "NoPCH" }


dofile "wxpropgrid.lua"