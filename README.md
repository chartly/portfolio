/************************************************************************
*
* Corbin Hart © (2008-201x)
* MIT License (see LICENSE file in root)
*
************************************************************************/

# Corbin Hart's Portfolio

### WELCOME!
Greetings, fellow developer! As we both know, there can be a lot of cognitive overhead in consuming an unfamiliar and even moderately sized codebase. This document and framework is intended to help reduce the effort needed to extricate the information you seek.

### WHAT DO WE HAVE HERE??
There are two showcase pieces in this repo.

###### 1. ditfw-gfx
  * math, multi-threading, file I/O, OpenGL, GUI, and input wrapped in a modular self-contained package
  * skeleton GUI window layout generated via form editor
  * OBJ model visualization of surface geometry, face / vertex normals, and mesh holes using half-edge mesh representation
  * GLSL shader reloading at run-time

###### 2. ditfw-profiler
  * automatic instrumentation of any function in a group of compiled translation units
  * microsecond precision using the processor tick count
  * serialization, statistics, and symbol resolution of the entire call tree for hooked translation units
  * lock-free ring buffer used to offload metrics computation to separate thread

## (DITFW) DigiPen Institute of Technology Framework

### HOW DO I BUILD IT?

There is a very simple build workflow to the ditfw.

* Clone the repo.
* Generate project and solution files
* Build any solution files
* Execute demo project

##### BAM!
Once cloned, the framework should be self contained and require no further libs or source to get it working.

#### COMPILATION!

Assuming that you have cloned either ditfw-gfx or ditfw-profiler and opened that folder:
 
1. Go to ./build.
2. GenerateBuild.bat
  * Select __Visual Studio 2013__
  * Observe console output for details on processed premake scripts
4. Open ./build/vs2013
5. __(IF THIS EXISTS)__ Open deps.sln
  * Build all (in debug or release or both)
  * __Deps solution is built *once* per machine.__
6. Open ditfw.sln
  * Build all
  * Set the appropriate __startup project__ (skeleton, bsp, etc)
7. Ctrl + F5

### WHAT DO I GOT??

#### ditfw-gfx
You should now see a window with simple Win32 GUI with a file menu, a blank property grid, and some checkboxes. Additionally, there is an OpenGL context initialized and created for you inside this GUI, with a coordinate cross located at (0,0,0).

Feel free to load an OBJ file and observe the half-edge mesh data structure by displaying face / vertex normals and cracks in the mesh skin.

#### ditfw-profiler
You will see a 2D particle fluid simulation written by Cody Pritchard. It is used as a basis for a testable simulation. The fluid simulation uses DirectX and DXUT to open the window and draw the GUI.

Once the simulation has run an appropriate amount of time, press the ESC key to quit and dump the profiling information to a trivial JSON file in the ./bin directory. To make it human readable, a JSON linter can verify and format the data.