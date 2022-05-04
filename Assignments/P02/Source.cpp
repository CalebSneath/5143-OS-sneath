///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P02 CPU Scheduling - Simulation
// Course:           CMPS 5143
// Date:             May 1, 2022
// Semester:         Spring 2022
//
// Description:
//       This program is designed to simulate several possible CPU 
//       scheduling algorithms.
//       
// Requirements (Visual Studios): 
//  - Create a new empty C++ program in Visual Studios.
//  - Copy files into Source Files in Visual Studios. (.h and .cpp files)
//  - Copy .ico and .png files to Resource Files.
//  - Optionally modify the .conf files with the desired settings.
//  - Insert your desired dataset inside dataset.dat in the same directory.
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
//      datafile.dat        : Input file for simulation "processes".
//                            memory for all the processes.
//      results.simlog      : Output text file for simulation.
//      simulation.conf     : Standard config file to determine the 
//                            number of writers, number of readers,
//                            how many instructions can be processed
//                            by a process before getting kicked
//                            off the round robin queue, and how
//                            many processes at once get a turn on
//                            the "CPU".
//      icon.ico            : Program icon.
//      icon.png            : Image assets for program.
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


int main(int flagCount, char* commandFlags[])
{
    std::string defaultName = "datafile.dat";
    int timeQuantum = 1;
    std::string parameterSchedulingAlgorithm = "FCFS";
    int parameterCPU = 1;
    int parameterIO = 1;

    // Parse configuration file
    // The instructions specified a JSON config or terminal for this
    // I went ahead and also did terminal below, so even though
    // this file isn't JSON, I still made sure to fulfill the terminal
    // requirement instead, this is just extra.
    std::ifstream confFile;
    confFile.open("simulation.conf");
    if (confFile.is_open())
    {
        std::string configFlags[10];
        int grabbed = 0;
        // Overwrite defaults with terminal entries
        while (confFile && grabbed < 10)
        {
            confFile >> configFlags[grabbed];
            grabbed++;
        }
        if(grabbed == 10)
        {
            std::string grabName = configFlags[1];
            std::string grabTime = configFlags[3];
            parameterSchedulingAlgorithm = configFlags[5];
            std::string grabCPU = configFlags[7];
            std::string grabIO = configFlags[9];

            if (grabName.length() > 0)
            {
                defaultName = grabName;
            }

            // Check if actually a number then cast.
            if (std::isdigit(grabTime[0]))
            {
                timeQuantum = stoi(grabTime);
            }
            if (std::isdigit(grabCPU[0]))
            {
                parameterCPU = stoi(grabCPU);
            }
            if (std::isdigit(grabIO[0]))
            {
                parameterIO = stoi(grabIO);
            }
        }
        // Finished with config file
        confFile.close();
    }

    // Overwrite defaults with terminal entries
    if (flagCount == 6)
    {
        std::string grabName = commandFlags[1];
        std::string grabTime = commandFlags[2];
        parameterSchedulingAlgorithm = commandFlags[3];
        std::string grabCPU = commandFlags[4];
        std::string grabIO = commandFlags[5];

        if (grabName.length() > 0)
        {
            defaultName = grabName;
        }

        // Check if actually a number then cast.
        if (std::isdigit(grabTime[0]))
        {
            timeQuantum = stoi(grabTime);
        }
        if (std::isdigit(grabCPU[0]))
        {
            parameterCPU = stoi(grabCPU);
        }
        if (std::isdigit(grabIO[0]))
        {
            parameterIO = stoi(grabIO);
        }
        
    }
    else
    {
        //Remove the console so only the application shows, reducing clutter
        HWND noConsole = GetConsoleWindow();
        //ShowWindow(noConsole, SW_HIDE);
        // If the program permanently removed your console and you want
        // it back, uncomment below and run.
        //ShowWindow(noConsole, SW_SHOW);
    }

    SimulationProgram simObject;
    simObject.setParameters(defaultName, timeQuantum, 
        parameterSchedulingAlgorithm, parameterCPU, parameterIO);
    simObject.runSim();

    std::cout << "Exiting" << std::endl;
    return 0;
}
