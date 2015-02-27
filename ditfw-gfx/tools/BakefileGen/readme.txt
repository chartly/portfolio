/************************************************************************
*
* Flood Project © (2008-201x)
* Licensed under the simplified BSD license. All rights reserved.
*
************************************************************************/
---------------------------------------
BAKEFILEGEN TOOL
---------------------------------------

---------------------------------------
OBJECTIVE
---------------------------------------

This tool generates a simple premake script from a list of files within a bakefile. In
this framework, it is used to create the "./deps/wxwidgets/wx.lua" script to add the
wxWidgets source files from a specific revision.

To use this tool, you first compile the two C# files into an executable binary. Second, pass
two arguments to the executable on the 

---------------------------------------
BUILD & USAGE
---------------------------------------
The easiest way to build and use this project is from the command line:

	1) Open a commandline
	2) cd C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC
	3) vcvarsall.bat
	4) cd <root>\tools\BakefileGen
	5) csc Program.cs TextGenerator.cs
	6) Program <root>\deps\wxwidgets\build\bakefiles\files.bkl wx.lua
	7) Cut wx.lua and paste into <root>\deps\wxwidgets