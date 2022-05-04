///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P03/P04 Reader/Writer
// Course:           CMPS 5143
// Date:             May 1, 2022
// Semester:         Spring 2022
//
// Description:
//       This program is designed to simulate multiple independent processes
//       running simultaneously with shared memory. 
//       
// Requirements (Visual Studios): 
//  - Create a new empty C++ program in Visual Studios.
//  - Copy files into Source Files in Visual Studios. (.h and .cpp files)
//  - Copy .ico and .png files to Resource Files.
//  - Create subdirectories in the main directory of the Visual Studios
//    project called "readers" and "writers".
//  - Optionally modify the .conf files with the desired settings.
//  - Change the Random Settings seed to -1 to use a random seed, or
//    anything else to instead use that as the seed.
//  - Hit local windows debugger to run as debug or build and press F5
//  - Optionally relocated the executable and resources into the desired
//    final directoy. This should allow the use of command line arguments
//    when launching the program.
//    - Command line arguments follow the same format and order as the right
//      column of the "simulation.conf" file.
//  - Configure any final details in the GUI before launching the simulation.
//      
// Files:            
//      Source.cpp          : Driver program, collects command args. 
//      user_interface.cpp  : Implementation for program control loop,
//                            event loop, and menu code.
//      user_interface.h    : Header file for above.
//      user_interface_primitives.cpp : Contains generic menu an drawable
//                                      D2D items such as buttons or 
//                                      rectangles suitable for use in
//                                      multiple different files
//      user_interface_primitives.h   : Header file for above.
//      simulation.cpp      : Implementation for main simulation specific
//                            logic and details.
//      simulation.h        : Header file for above
//      readers/...         : Folder containing all pages created by the 
//                            reader processes over the course of the program.
//      writers/...         : Folder containing all pages created by the 
//                            writer processes over the course of the program.
//      memory.dat          : Input/output memory file of the state of shared
//                            memory for all the processes.
//      "Random Settings.conf : Contains the seed to be used for all random
//                              elements, or in the case of -1, to tell
//                              the program to be random.
//      simulation.conf     : Standard config file to determine the 
//                            number of writers, number of readers,
//                            how many instructions can be processed
//                            by a process before getting kicked
//                            off the round robin queue, and how
//                            many processes at once get a turn on
//                            the "CPU".
//      icon.ico            : Program icon.
//      icon2.png icon3.png : Image assets for program.
// Credit: 
//      Extended sections of code were used from the Microsoft Windows.h
//      online documentation and tutorial. Source:
//          https://docs.microsoft.com/en-us/windows/win32/learnwin32
///////////////////////////////////////////////////////////////////////////////

#pragma once

// Ensure UNICODE precompiler directive is activated
#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif
#ifndef UNICODE
#define UNICODE
#endif

//#include "integration_inputs.h"
//#include "integration_outputs.h"
#include "user_interface.h"
#include <iostream>
#include <fstream>

//#define WINDOWSRELEASE
//#ifdef WINDOWSRELEASE
#include <Windows.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <string>

//#include "basewin.h"
#include <d2d1.h>
#pragma comment(lib,"d2d1.lib")
//#pragma comment(lib, "d2d1")
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>



int main(int flagCount, char* paramFlags[])
{

    std::ofstream testFile;
    testFile.open("Testdir/file.txt");
    testFile << "Test" << std::endl;
    testFile.close();


    int writersParam = 5;
    int readersParam = 25;
    int timeQuantum = 10;
    int processSlot = 10;
    bool debugSession = false;
    std::ifstream confFile;
    confFile.open("simulation.conf");

    // Try loading config file
    try
    {
        std::string tempBuffer = "";
        confFile >> tempBuffer >> writersParam;
        confFile >> tempBuffer >> readersParam;
        confFile >> tempBuffer >> timeQuantum;
        confFile >> tempBuffer >> processSlot;
    }
    // Input errors
    catch (...)
    {
        writersParam = 5;
        readersParam = 25;
        timeQuantum = 10;
        processSlot = 10;
    }
    confFile.close();

    

    if (flagCount > 1)
    {
        std::string writersHold = paramFlags[1];
        if (isdigit(std::isdigit(writersHold[0])))
        {
            writersParam = std::stoi(writersHold);
            readersParam = 5 * writersParam;
        }
    }
    else
    {
        //Remove the console so only the application shows, reducing clutter
        HWND noConsole = GetConsoleWindow();
        //ShowWindow(noConsole, SW_HIDE);
    }

    SimulationProgram simObject;
    simObject.setParameters(writersParam, readersParam, timeQuantum, processSlot);
    simObject.runSim();

    std::cout << "Exiting" << std::endl;
    return 0;
}
