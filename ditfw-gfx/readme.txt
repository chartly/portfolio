/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

Fall 2014 / Spring 2015
CS300 / CS350 Framework & Skeleton app
Pushpak Karnick

-------------------------------------------------
(DITFW) DigiPen Institute of Technology Framework
-------------------------------------------------

Greetings, fellow student! As we both know, there can be a lot of cognitive overhead
 to being given any "complete" codebase. Additionally, given the constraints of assignment
 workload for the RTIS degree, there is very little time left over (if any at all) to figure
 out what a TA is attempting to do (or fails to do) in order to help you accomplish the assignment.

This document (as well as this framework) is intended to help reduce the effort needed to
 consume and successfully utilize the assets in this framework. I hope this helps, best of
 luck, and I'm always happy to answer any questions you may have.

 -------------------------------------------------
 WHAT DOES IT DO FOR ME?
 -------------------------------------------------

Out of the box, you are given a skeleton with three primary features:

 1) Build packaging (for the external libraries, for the core framework, and then for your assignments)
 2) A GUI that may be changed by an external editor (wxFormBuilder - think like C#'s form editor),
    without needing to write / modify any C++ code.
 3) An OpenGL window that is initialized for you, a basic (though incomplete) OBJ loader implementation,
    a basic mesh data structure, and a simple shader system with hot file reloading (so you can edit glsl code
    while the game is running).

 Ultimately, once you build the skeleton, you are able to begin implementating GLSL code immediately in the
  the text editor of your choice, and whenever you save, the framework will reload that shader for you so you
  are able to get immediate feedback. This means you can get directly to your assignment.

-------------------------------------------------
HOW DO I BUILD IT?
-------------------------------------------------

 THE LOW DOWN
--------------
There is a very simple build workflow to the ditfw. In order to package the external libraries, the
 core framework source code, and then the assignment specific GUI / implementation - while maintaing
 your sanity as a busy student - you, the user, need to generate a Visual Studio solution from the
 files you've been given for each computer you're going to build it on.

 COMPILATION!
--------------
Assuming that you have extracted the ditfw-cs300hw2.7z
 archive somewhere, and have opened up the ./ditfw directory.
 
 1) Go to ./build.
 2) Shift-right click --> "Open command window here..."
 3) GenerateBuild.bat
 4) 1
 5) .... (watch it create vs2013 project folder / solution files)
 6) Open ./build/vs2013
 7) Open deps.sln --> Build all (in debug or release or both; you will only need to build this once for each config)
 8) Open ditfw.sln --> Build all (set cs300hw2 as your startup project)
 9) Ctrl + F5

 WHAT DO I GOT??
-----------------
You should now see a window with simple Win32 GUI with a file menu, a blank property grid, and some checkboxes.
 Additionally, there is an OpenGL context initialized and created for you inside this GUI, with a coordinate
 cross located at (0,0,0).

Feel free to load an OBJ file (have fun! not all of them work, but remember - that's not the assignment - they
 are there for your ease of use, not to make your life harder) and see the face & vertex normals.

 WHY?! IT SEEMS COMPLICATED!
-----------------------------
The reason it is set up this way, is so that you can delete my code, and paste in your own. It lets you completely
 ignore any of the code I have provided and still be able to either a) use the packaged GUI / OpenGL / boost libraries
  to quickly make a gui or init a window or b) to be able to trivially create new assignment projects for future 
  homeworks.

 LOOKING AHEAD
----------------
Looking ahead, you will be able to simply copy the ./hws/cs300hw2 folder, paste and rename that to ./hws/cs300hw3
 and re-run the batch file. You will now have two completely separate assignments in ditfw.sln, and you can
 immediately start building off of the work you've done before, without getting in the way of your other source
 code.

Your life is incredibly busy. This has been constructed so that getting a window on the screen with a GUI for
 any class that requires it doesn't take any time! If you don't want it, ignore it!

For my sanity, all that's required is a very simple english-readable config file. To make a new one is trivial,
 compared to navigating the horror that is the Visual Studio project settings dialog. I have provided examples
 and some links further down.

-------------------------------------------------
HOW DOES IT WORK?
-------------------------------------------------

 PREMAKE-IFICATION
-------------------
The method used to generate the solution / project files is called Premake. It is a binary that I have
 packaged in the ./tools folder. It hosts the lua environment, and executes a simple lua dsel (domain 
 specific embedded language) to make config files easily human readable.

 * https://bitbucket.org/premake/premake-dev/wiki/browse/

The source is based on a branch of this repo:

 * https://bitbucket.org/starkos/premake-triton

It is pretty trivial to create a new premake build script. Here is a great example:

 * ./deps/glew/premake4.lua

To create a new homework project, for instance, you simply copy the files you want into that folder
 and create a new lua script named "premake4.lua". Please refer to the above example for a simple
 starting point. Then place this folder into the ./hws folder and re-run GenerateBuild.bat. Ditfw.sln will
 now contain a new project generated from the config in your build script.

 WXWIDGETS GUI
---------------
The GUI library that I have packaged is called wxWidgets, I have packaged version 3. There is a
 convenient form editor called wxFormBuilder, you can find it here:

 * http://sourceforge.net/projects/wxformbuilder/

The form layout that is implemented in the cs300hw2 skeleton was created with this tool. You can find
 the form builder project file in the following location:

 * ./hws/cs300hw2/platform/MainWindow.fbp

Feel to open this file and make some changes. Once you have done so, press either F8 or the gear icon
 in the toolbar, and then simply rebuild ditfw.sln. The form builder project will generate the C++ code
 that is required to create and layout your gui. In the skeleton, the following two files are the generated
 source code for the editor window:

 * ./hws/cs300hw2/platform/AppWindow.h
 * ./hws/cs300hw2/platform/AppWindow.cpp

You will find the remainder of the skeleton implementation inside of the following two files:

 * ./hws/cs300hw2/platform/App.h
 * ./hws/cs300hw2/platform/App.cpp

 OPENGL SHADERS
----------------
I have implemented a rudimentary shader system, as well as crude vertex n-dot-l lighting. This
 is so that you have something to see when you compile and run the skeleton. You will be required
 to change the implementation in the GLSL shaders and implement a different lighting model. These
 shaders can be found in the following location:

 * ./hws/cs300hw2/shaders/model.vs.glsl
 * ./hws/cs300hw2/shaders/model.fs.glsl

Feel free to modify these files in your text editor of choice. The framework will watch these files
 for changes if the executable is running, and it will reload / compile / link the shader code.

 BASIC MESH WHAT?
-------------------
For your convenience, I have implemented a simple mesh class that I call BasicMesh. It provides
 a simple memory layout and an easy way to populate your VBO's / VAO's. In order to easily
 traverse a mesh that you've loaded in a sane manner, a simple iterator pattern is exposed.

  * NOTE: these examples use C++11 lambdas.
  * http://stackoverflow.com/questions/7627098/what-is-a-lambda-expression-in-c11

For example, in order to linearly walk the triangles in a mesh, you may type:

  walk( faces(mesh), [&](const FaceIterator& face) -> FaceIterator
  	{
  		// ...

  		return next(face);
  	});

You may also traverse the vertices in a mesh:

  walk( vertices(mesh), [&](const VertexIterator& vtx) -> VertexIterator
    {
    	// ...

    	return next(vtx);
    });

You may also combine the two together. For instance, you may want to iterate through each triangle
 in a mesh, and then iterate through each triangle's vertices:

   walk( faces(mesh), [&](const FaceIterator& face) -> FaceIterator
     {
 		// ...
 		
 		auto fverts = vertices(face);
 		walk( begin(fverts), [&](const VertexIterator& vtx) -> VertexIterator
 		  {
 		  	// ...

 		  	return next(vtx);
 		  });

 		return next(face);
     });

Finally, you may also do it the other way around, and walk the triangles that share a vertex:

   walk( vertices(mesh), [&](const VertexIterator& vtx) -> VertexIterator
     {
 		// ...
 		
 		auto vfaces = faces(vtx);
 		walk( begin(vfaces), [&](const FaceIterator& face) -> FaceIterator
 		  {
 		  	// ...

 		  	return next(face);
 		  });

 		return next(vtx);
     });

See where I'm going with this? For additional reference, please see the following:

 * App::DrawVertexNormals()
 * App::DrawFaceNormals()

 BUILD OUTPUT?!
----------------
Additionally, I have commented ./cs300hw2/platform/App.cpp with messages in the build window, if you
 didn't notice them already. Double clicking on one of notes in the output window will take you to the
 line of code, so you can easily navigate the internals of the skeleton implementation, should you
 want to make any changes.

-------------------------------------------------
OH NOES! IT CRASHED!
-------------------------------------------------
There are several expected crashes in the skeleton implementation.
 1) When an exception occurs while asynchronously loading a file in another thread.
 2) Some OBJ files are formatted using annotations that skeleton doesn't support.
 3) If your GPU does not have enough memory, and you attempt to draw the vert or face normals.

These are expected, and you will not be in any way graded on whether they are fixed or not.

-------------------------------------------------
THAT'S ALL FOLKS!
-------------------------------------------------
Good luck!