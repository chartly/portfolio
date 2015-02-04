-- This is the starting point of the build scripts for the project.
-- It defines the common build settings that all the projects share
-- and calls the build scripts of all the sub-projects.

newoption
{
   trigger     = "boost-dir",
   description = "Full path to the directory for the Boost distribution."
}

dofile "helpers.lua"

solution "ditfw"
	
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	--defaultplatform "x32"

	flags { "Symbols", common_flags }
	language "C++"
	framework "4.5"
	
	location (builddir)
	objdir (builddir .. "/obj/")
	targetdir (libdir)
	libdirs { libdir }
	
	debugdir (bindir)

	-- Build configuration options

	configuration "Debug"
		defines { "DEBUG" , "_DEBUG" }
		optimize "Off"

	configuration "Release"
		defines {"NDEBUG", "RELEASE"}
		optimize "On"
		
	configuration {}

	group "core"
		dofile( srcdir .. "/premake4.lua")

	group "homeworks"
		print("Searching for homeworks...")
		IncludePremakeProjects(path.join(hwsdir,"*"))
