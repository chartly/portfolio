/*****************************************************************************/
/*

CS391 Project 3 Profiler Scenario
This is adapted from Cody Pritchard's PHY350/399 Fluid Simulation Work

This sample provides a nice, juicy, CPU intensive application to test
your profiler on for project 3.

*/
/*****************************************************************************/

#include "API.h"
#include "Core.h"
#include "Profiler.h"

#include <windows.h>
#include "Fluid.h"

Fluid* FLUID = NULL;

void InitializeDirectX();
int RunDirectX();

// Frame Update Function - Called Every Frame
void CALLBACK OnFrameMove( double /*fTime*/, float /*fElapsedTime*/, void* /*pUserContext*/ )
{
    FLUID->Update( FluidTimestep );
}

// main
//INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
int main()
{
    using namespace dit;
    Core core;
    Profiler profiler;

    core.Init();
    profiler.Start();

    FLUID = new Fluid();
    FLUID->Create( 1.6f, 1.2f );

#ifdef _DEBUG
    // This runs really slow in Debug.  Understand?  Comment out this line, then change the next line to 0.2f
    //#error Do NOT submit a profiler report for the DEBUG version.  Understand?  You may comment out this line.
    FLUID->Fill( 0.2f );
#else
    // Do NOT change the simulation size for the profiler report you submit.
    FLUID->Fill( 0.5f );
#endif

    InitializeDirectX();

    int retval = RunDirectX();

    delete FLUID; FLUID = NULL;

    profiler.Stop();
    core.Shutdown();

    return retval;
}
