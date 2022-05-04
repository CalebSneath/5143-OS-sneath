///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P03/P04 Reader/Writer
// Course:           CMPS 5143
// Date:             May 1, 2022
// Semester:         Spring 2022
// File Description: This file contains the implementation details for the 
//                   classes necessary for this specific simulation's logic.
//                   Many classes within this page may solve problems 
//                   somewhat specific to the reader/writer simulation.
//////////////////////////////////////////////////////////////////////////////

#pragma once

// Ensures UNICODE precompiler directive is activated
#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif
#ifndef UNICODE
#define UNICODE
#endif

#include "user_interface.h"
#include "user_interface_primitives.h"
#include "simulation.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>
#include <algorithm>

// Precompiler directive in case anyone wants to modify relevant
// code to work for Mac or Linux builds. Commented out for now
// because it interfered with the warning squiggles that show up
// before you compile, which is likely a VS bug.
//#define WINDOWSRELEASE
//#ifdef WINDOWSRELEASE
#include <Windows.h>
#include <Windowsx.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

//#include "basewin.h"
#include <d2d1.h>
#include <dwmapi.h>


// Sometimes uncommenting one of these fixes user's include issues.
// Use the linker in the command line and comment these out
// if your compiler doesn't support pragma comment.
//#pragma comment(lib,"d2d1.lib")
//#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "Dwmapi")

#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <Unknwn.h>


//////////////////////////////////////////////////////////////////////////////
//                             Normal Classes
//////////////////////////////////////////////////////////////////////////////


// Process
Process::Process()
{
    sharedMemory = SimulationMemory();
}
Process::Process(HWND inHandle)
{
    sharedMemory = SimulationMemory();

    hWnd = inHandle;

    // Set visualization handles to window
    shapeRepresentation.setHWND(inHandle);
    textRepresentation.setHWND(inHandle);
    textRepresentation.setMinScaling(false);
}
void Process::setHWND(HWND inHandle)
{
    hWnd = inHandle;

    // Set visualization handles to window
    shapeRepresentation.setHWND(inHandle);
    textRepresentation.setHWND(inHandle);
    textRepresentation.setMinScaling(false);
}
void Process::setProtectionLevel(int inLevel)
{
    sharedMemory.setProtectionStrength(inLevel);
    protectionLevel = inLevel;
}
void Process::drawRepresentation(float inTopX, float inTopY, float inBottomX,
    float inBottomY, HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
    // Find percentage
    float percentDone = ((3 * initialInstructions) - instructionQueue.size()) / float(initialInstructions * 3);

    // Calculate bar length and position
    float adjustedBottomX = (inBottomX - inTopX) * percentDone;


    // Determine output message
    std::string displayMessage = "";
    if (reader == true)
    {
        displayMessage += "Reader ";
    }
    else
    {
        displayMessage += "Writer ";
    }
    displayMessage += std::to_string(idNumber);
    displayMessage += ": ";
    displayMessage += std::to_string(int(percentDone * 100));
    displayMessage += "%\n";
    if (instructionQueue.size() == 0)
    {
        displayMessage += "Done";
    }
    else if (hasSlot == true)
    {
        displayMessage += "Run";
    }
    else
    {
        displayMessage += "Wait";
    }

    // Update visual attributes
    textRepresentation.setText(displayMessage);
    textRepresentation.setCoords(inTopX, inTopY, inBottomX, inBottomY);
    shapeRepresentation.setCoords(inTopX, inTopY, inTopX + 
        adjustedBottomX, inBottomY);

    // Display Visualization
    shapeRepresentation.cycleRectangle(1 - percentDone, 0.3, 
        percentDone, hr, pRenderTarget);
    textRepresentation.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
}
bool Process::keySearch(std::string inKey, 
    std::vector<std::string>& searchList)
{
    for (int index = 0; index < searchList.size(); index++)
    {
        if (searchList[index] == inKey)
        {
            return true;
        }
    }

    // Didn't find
    return false;
}
void Process::runProcess()
{
    // Launch the process to be ran as a separate thread
    processThread = new std::thread(&Process::executeProcess, this);

    processThread->detach();
}


bool Process::loadInstruction(int index)
{
    // Register memory
    // Make sure it is its turn first
    while ((sharedMemory.sharedMemory.at("R4") < privileged) && (protectionLevel < 3))
    {
        // Wait until finished
    }

    // Load from process control block if needed
    if (processControlBlock.count(instructionQueue[index + 2]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = processControlBlock.at(instructionQueue[index + 2]);
    }

    // Perform operation on registers in shared memory
    sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = std::stoi(instructionQueue[index + 1]);

    // Update Process Control Block
    if (processControlBlock.count(instructionQueue[index + 2]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 2], sharedMemory.sharedMemory.at(instructionQueue[index + 2]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 2]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);
    }

    hasPrivileged = true;

    return true;
}
bool Process::moveInstruction(int index)
{
    // Register memory
    if (sharedMemory.registerMemory(hasPrivileged, instructionQueue[index + 1],
        instructionQueue[index + 2]) == false)
    {
        return false;
    }

    // Load from process control block if needed
    if (processControlBlock.count(instructionQueue[index + 1]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = processControlBlock.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = processControlBlock.at(instructionQueue[index + 2]);
    }

    sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);

    // Update Process Control Block
    if (processControlBlock.count(instructionQueue[index + 1]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 1], sharedMemory.sharedMemory.at(instructionQueue[index + 1]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 2], sharedMemory.sharedMemory.at(instructionQueue[index + 2]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 2]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);
    }

    // Unregister memory
    sharedMemory.unRegisterMemory(instructionQueue[index + 1], instructionQueue[index + 2]);

    return true;
}
bool Process::addInstruction(int index)
{
    // Register memory
    if (sharedMemory.registerMemory(hasPrivileged, instructionQueue[index + 1],
        instructionQueue[index + 2]) == false)
    {
        return false;
    }

    // Load from process control block if needed
    if (processControlBlock.count(instructionQueue[index + 1]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = processControlBlock.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = processControlBlock.at(instructionQueue[index + 2]);
    }

    // Perform operation on registers in shared memory
    sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]) + sharedMemory.sharedMemory.at(instructionQueue[index + 2]);

    // Update Process Control Block
    if (processControlBlock.count(instructionQueue[index + 1]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 1], sharedMemory.sharedMemory.at(instructionQueue[index + 1]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 2], sharedMemory.sharedMemory.at(instructionQueue[index + 2]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 2]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);
    }

    // Unregister memory
    sharedMemory.unRegisterMemory(instructionQueue[index + 1], instructionQueue[index + 2]);

    return true;
}
bool Process::subtractInstruction(int index)
{
    // Register memory
    if (sharedMemory.registerMemory(hasPrivileged, instructionQueue[index + 1],
        instructionQueue[index + 2]) == false)
    {
        return false;
    }

   // Load from process control block if needed
    if (processControlBlock.count(instructionQueue[index + 1]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = processControlBlock.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = processControlBlock.at(instructionQueue[index + 2]);
    }

    // Perform operation on registers in shared memory
    sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]) - sharedMemory.sharedMemory.at(instructionQueue[index + 2]);

    // Update Process Control Block
    if (processControlBlock.count(instructionQueue[index + 1]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 1], sharedMemory.sharedMemory.at(instructionQueue[index + 1]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 2], sharedMemory.sharedMemory.at(instructionQueue[index + 2]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 2]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);
    }

    // Unregister memory
    sharedMemory.unRegisterMemory(instructionQueue[index + 1], instructionQueue[index + 2]);

    return true;
}
bool Process::multiplyInstruction(int index)
{
    // Register memory
    if (sharedMemory.registerMemory(hasPrivileged, instructionQueue[index + 1],
        instructionQueue[index + 2]) == false)
    {
        return false;
    }

    // Load from process control block if needed
    if (processControlBlock.count(instructionQueue[index + 1]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = processControlBlock.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = processControlBlock.at(instructionQueue[index + 2]);
    }

    // Perform operation on registers in shared memory
    sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]) * sharedMemory.sharedMemory.at(instructionQueue[index + 2]);

    // Update Process Control Block
    if (processControlBlock.count(instructionQueue[index + 1]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 1], sharedMemory.sharedMemory.at(instructionQueue[index + 1]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 2], sharedMemory.sharedMemory.at(instructionQueue[index + 2]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 2]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);
    }

    // Unregister memory
    sharedMemory.unRegisterMemory(instructionQueue[index + 1], instructionQueue[index + 2]);

    return true;
}
bool Process::divideInstruction(int index)
{
    // Register memory
    if (sharedMemory.registerMemory(hasPrivileged, instructionQueue[index + 1],
        instructionQueue[index + 2]) == false)
    {
        return false;
    }

    // Load from process control block if needed
    if (processControlBlock.count(instructionQueue[index + 1]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = processControlBlock.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = processControlBlock.at(instructionQueue[index + 2]);
    }

    // Perform operation on registers in shared memory
    // Division requires error checking to avoid randomly dividing by 0.
    int tempInt = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);
    if (tempInt == 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = 0;
    }
    else
    {
        // Set left register to zero to avoid divide by zero error instead of the normal operation.
        sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = tempInt;
    }

    // Update Process Control Block
    if (processControlBlock.count(instructionQueue[index + 1]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 1], sharedMemory.sharedMemory.at(instructionQueue[index + 1]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 2], sharedMemory.sharedMemory.at(instructionQueue[index + 2]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 2]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);
    }

    // Unregister memory
    sharedMemory.unRegisterMemory(instructionQueue[index + 1], instructionQueue[index + 2]);

    return true;
}
bool Process::setInstruction(int index)
{
    // Register memory
    // Only one register to register for, so just pass in index + 2 twice
    if (sharedMemory.registerMemory(hasPrivileged, instructionQueue[index + 2],
        instructionQueue[index + 2]) == false)
    {
        return false;
    }

   // Load from process control block if needed
    if (processControlBlock.count(instructionQueue[index + 2]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = processControlBlock.at(instructionQueue[index + 2]);
    }

    // Perform operation on registers in shared memory
    sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = std::stoi(instructionQueue[index + 1]);

    // Update Process Control Block
    if (processControlBlock.count(instructionQueue[index + 2]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 2], sharedMemory.sharedMemory.at(instructionQueue[index + 2]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 2]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);
    }

    // Unregister memory
    sharedMemory.unRegisterMemory(instructionQueue[index + 2], instructionQueue[index + 2]);

    return true;
}
bool Process::readInstruction(int index)
{
    // Register memory
    if (sharedMemory.registerMemory(hasPrivileged, instructionQueue[index + 1],
        instructionQueue[index + 2]) == false)
    {
        return false;
    }

    // Load from process control block if needed
    if (processControlBlock.count(instructionQueue[index + 1]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = processControlBlock.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = processControlBlock.at(instructionQueue[index + 2]);
    }

    // Perform operation on registers in shared memory
    sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]);

    // Update Process Control Block
    if (processControlBlock.count(instructionQueue[index + 1]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 1], sharedMemory.sharedMemory.at(instructionQueue[index + 1]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 2], sharedMemory.sharedMemory.at(instructionQueue[index + 2]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 2]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);
    }

    // Unregister memory
    sharedMemory.unRegisterMemory(instructionQueue[index + 1], instructionQueue[index + 2]);

    return true;
}
bool Process::writeInstruction(int index)
{
    // Register memory
    if (sharedMemory.registerMemory(hasPrivileged, instructionQueue[index + 1],
        instructionQueue[index + 2]) == false)
    {
        return false;
    }

    // Load from process control block if needed
    if (processControlBlock.count(instructionQueue[index + 1]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = processControlBlock.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) > 0)
    {
        sharedMemory.sharedMemory.at(instructionQueue[index + 2]) = processControlBlock.at(instructionQueue[index + 2]);
    }

    // Perform operation on registers in shared memory
    sharedMemory.sharedMemory.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);

    // Update Process Control Block
    if (processControlBlock.count(instructionQueue[index + 1]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 1], sharedMemory.sharedMemory.at(instructionQueue[index + 1]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 1]) = sharedMemory.sharedMemory.at(instructionQueue[index + 1]);
    }
    if (processControlBlock.count(instructionQueue[index + 2]) < 1)
    {
        processControlBlock.insert({ instructionQueue[index + 2], sharedMemory.sharedMemory.at(instructionQueue[index + 2]) });
    }
    else
    {
        processControlBlock.at(instructionQueue[index + 2]) = sharedMemory.sharedMemory.at(instructionQueue[index + 2]);
    }

    // Unregister memory
    sharedMemory.unRegisterMemory(instructionQueue[index + 1], instructionQueue[index + 2]);

    return true;
}

void Process::executeProcess()
{
    // Initialize process page to be viewable
    printProcess();

    // Readers can read prioritized memory fine, they just
    // use a normal lock.
    if (reader == true)
    {
        hasPrivileged = true;
    }

    // Launch the process to be ran as a separate thread
    while (instructionQueue.size() > 0)
    {
        // This prints current process list out once per cycle
        // of round robin. You should comment this out if your 
        // computer uses an SSD to avoid unnecessary wear.
        printProcess();

        // Limit resource usage
        // Comment out this line for a *REAL PRACTICAL*
        // demonstration of starvation in CPU scheduling
        Sleep(10);

        while (hasSlot == false)
        {
            // You can see an easy race condition if
            // you uncomment this line. Every once in
            // a while, if two threads operate at the
            // same time, they will print something like,
            // No slotNo slot because they didn't
            // finish printing endl before the other started
            //std::cout << "No slot" << std::endl;

            // Continue sleeping until woken up
            Sleep(10);
        }

        // Try to loop through each instruction
        int index = 0;
        for (index = 0; index + 3 <= instructionQueue.size() &&
            timeQuantum > thisQuantumRan; index += 3)
        {
            bool success = false;

            if (instructionQueue[index] == "READ")
            {
                success = readInstruction(index);
            }
            else if (instructionQueue[index] == "WRITE")
            {
                success = writeInstruction(index);
            }
            else if (instructionQueue[index] == "MOV")
            {
                success = moveInstruction(index);
            }
            else if (instructionQueue[index] == "SET")
            {
                success = setInstruction(index);
            }
            else if (instructionQueue[index] == "ADD")
            {
                success = addInstruction(index);
            }
            else if (instructionQueue[index] == "SUB")
            {
                success = subtractInstruction(index);
            }
            else if (instructionQueue[index] == "MUL")
            {
                success = multiplyInstruction(index);
            }
            else if (instructionQueue[index] == "DIV")
            {
                success = divideInstruction(index);
            }
            else if (instructionQueue[index] == "LOAD")
            {
                success = loadInstruction(index);
            }



            // Increment round robin and remove instruction if successful
            if (success == true)
            {
                thisQuantumRan++;

                // Push completed operation message to the operation log
                std::string tempString = "";
                tempString += instructionQueue[index];
                tempString += " ";
                tempString += instructionQueue[index + 1];
                tempString += " ";
                tempString += instructionQueue[index + 2];


                sharedMemory.updateOpLog(tempString);

                // Erase the entire operation from queue
                instructionQueue.erase(instructionQueue.begin() + index, instructionQueue.begin() + index + 3);

                // Account for removal of three elements on index
                index -= 3;
            }


            // You might theoretically be able to create some deadlock
            // by commenting out this section in extremely rare circumstances.
            // If something registers a spot in the queue, round robins out,
            // tthis happens several other times, and it round robins
            // to a group not containing the original members, and
            // all members need the reserved resources, if 
            // running through the whole list has no cost for
            // round robin switching, they can be there forever
           

            // Kick off process when it reaches its time
            if (thisQuantumRan >= timeQuantum)
            {
                hasSlot = false;
            }
        }
        if (index >= instructionQueue.size())
        {
            thisQuantumRan++;
        }

    }

    // Allow others to use prioritized memory now it's finished with it
    if (reader == false && hasPrivileged == true)
    {
        sharedMemory.returnPriority();
    }

    // Ensure to update and print empty process page when it's done.
    printProcess();

    // Set thread to endable signal

    running = false;

    std::cout << "End of process thread" << std::endl;
}
void Process::printProcess()
{
    // Determine where to save and how to name this file
    std::string fileName = "";
    if (reader == true)
    {
        fileName = "readers/reader";
    }
    else
    {
        fileName = "writers/writer";
    }
    fileName += std::to_string(idNumber);

    // Open file
    std::ofstream processFile;
    processFile.open(fileName);

    // Print instructions one line (three tokens) at a time
    for (int index = 0; index < instructionQueue.size(); index += 3)
    {
        processFile << std::setw(10) << std::left << instructionQueue[index];
        processFile << std::setw(10) << std::left 
            << instructionQueue[index + 1];
        processFile << std::setw(10) << std::left 
            << instructionQueue[index + 2] << std::endl;
    }

    // Make sure something overwrites file on empty process.
    if (instructionQueue.size() < 1)
    {
        processFile << " " << std::endl;
    }

    // Close the file
    processFile.close();

}
void Process::fillInstructions()
{
    // Generate a random number for instructions from 100 - 200
    initialInstructions = rand() % 101 + 100;
    // Pick a spot to randomly stick the priority instruction
    // if it is necessary
    // Keep instructions in nice groups of four at first
    initialInstructions -= (initialInstructions % 4);
    int priorityPosition = -1;
    if (reader == false)
    {
        priorityPosition = (rand() % initialInstructions);
        priorityPosition -= priorityPosition % 4;
        if (priorityPosition == initialInstructions + 4)
        {
            priorityPosition -= 4;
        }
        initialInstructions += 2;

    }



    for (int index = 0; index < initialInstructions; index += 4)
    {
        if (index == priorityPosition)
        {
            index += 2;
            // Load process ID instruction
            instructionQueue.push_back("LOAD");
            instructionQueue.push_back(std::to_string(idNumber));
            instructionQueue.push_back("R3");

            // Load PIC instruction
            instructionQueue.push_back("LOAD");
            instructionQueue.push_back(std::to_string(privileged));
            instructionQueue.push_back("R4");

        }
        else
        {
            genInstruction();
        }
    }



    // Add two priority instructions if writer
    if (reader == false)
    {
        initialInstructions += 2;
    }

    /*
    // Mark the registration queue as unfinished
    for (int index = 0; index < initialInstructions; index += 3)
    {

        registrationQueue.push_back(-1);
        registrationQueue.push_back(-1);
        registrationQueue.push_back(-1);
    }
    */

}
void Process::genInstruction()
{
    // Step one, load up registers
    // Line one
    // 0 - 1 inclusive
    int randChoice = rand() % 2;
    std::string tempVar = "";
    switch (randChoice)
    {
        case 0:
            tempVar = "READ";
            break;
        case 1:
            tempVar = "SET";
    }
    instructionQueue.push_back(tempVar);
    // Handle either read or set instruction
    if (tempVar == "READ")
    {
        randChoice = rand() % 4;
        tempVar = "";
        switch (randChoice)
        {
            case 0:
                tempVar = "A";
                break;
            case 1:
                tempVar = "B";
                break;
            case 2:
                tempVar = "C";
                break;
            case 3:
                tempVar = "D";
        }
        // Add 100 - 250 (inclusive)
        // We divide by five (integer division) and then multiply
        // back to remove nondivisible by five numbers
        tempVar += std::to_string((int(rand() % 154 + 100) / 5) * 5);
        instructionQueue.push_back(tempVar);
        instructionQueue.push_back("R1");
    }
    else
    {
        tempVar = std::to_string((int(rand() % 10)));
        instructionQueue.push_back(tempVar);
        instructionQueue.push_back("R1");
    }


    // Line Two
    // Same as above, but it can aslo
    // copy the register
    randChoice = rand() % 3;
    tempVar = "";
    switch (randChoice)
    {
        case 0:
            tempVar = "READ";
            break;
        case 1:
            tempVar = "SET";
            break;
        case 2:
            tempVar = "MOV";
    }
    instructionQueue.push_back(tempVar);
    // Handle either read or set instruction
    if (tempVar == "READ")
    {
        randChoice = rand() % 3;
        tempVar = "";
        switch (randChoice)
        {
        case 0:
            tempVar = "A";
            break;
        case 1:
            tempVar = "B";
            break;
        case 2:
            tempVar = "C";
            break;
        case 3:
            tempVar = "D";
        }
        // Add 100 - 250 (inclusive)
        // We divide by five (integer division) and then multiply
        // back to remove nondivisible by five numbers
        tempVar += std::to_string((int(rand() % 154 + 100) / 5) * 5);
        instructionQueue.push_back(tempVar);
        instructionQueue.push_back("R1");
    }
    else if (tempVar == "SET")
    {
        tempVar = std::to_string((int(rand() % 10)));
        instructionQueue.push_back(tempVar);
        instructionQueue.push_back("R2");
    }
    else
    {
        instructionQueue.push_back("R1");
        instructionQueue.push_back("R2");
    }

    // Step two, perform operations
    // Line three
    // 0 - 3 inclusive
    randChoice = rand() % 4;
    tempVar = "";
    switch (randChoice)
    {
        case 0:
            tempVar = "ADD";
            break;
        case 1:
            tempVar = "SUB";
            break;
        case 2:
            tempVar = "MUL";
            break;
        case 3:
            tempVar = "DIV";
    }
    instructionQueue.push_back(tempVar);
    // Set register order
    randChoice = rand() % 2;
    if(randChoice == 0)
    {
        instructionQueue.push_back("R1");
        instructionQueue.push_back("R2");
    }
    else
    {
        instructionQueue.push_back("R2");
        instructionQueue.push_back("R1");
    }

    // Step 3: Final Read/Write
    if (reader == true)
    {
        instructionQueue.push_back("READ");
        randChoice = rand() % 4;
        tempVar = "";
        switch (randChoice)
        {
        case 0:
            tempVar = "A";
            break;
        case 1:
            tempVar = "B";
            break;
        case 2:
            tempVar = "C";
            break;
        case 3:
            tempVar = "D";
        }
        // Add 100 - 250 (inclusive)
        // We divide by five (integer division) and then multiply
        // back to remove nondivisible by five numbers
        tempVar += std::to_string((int(rand() % 154 + 100) / 5) * 5);
        instructionQueue.push_back(tempVar);
        // Set register
        randChoice = rand() % 2;
        if (randChoice == 0)
        {
            instructionQueue.push_back("R1");
        }
        else
        {
            instructionQueue.push_back("R2");
        }
    }
    else
    {
        instructionQueue.push_back("WRITE");
        randChoice = rand() % 4;
        tempVar = "";
        switch (randChoice)
        {
        case 0:
            tempVar = "A";
            break;
        case 1:
            tempVar = "B";
            break;
        case 2:
            tempVar = "C";
            break;
        case 3:
            tempVar = "D";
        }
        // Add 100 - 250 (inclusive)
        // We divide by five (integer division) and then multiply
        // back to remove nondivisible by five numbers
        tempVar += std::to_string((int(rand() % 154 + 100) / 5) * 5);
        instructionQueue.push_back(tempVar);
        // Set register
        randChoice = rand() % 2;
        if (randChoice == 0)
        {
            instructionQueue.push_back("R1");
        }
        else
        {
            instructionQueue.push_back("R2");
        }
    }
}
void Process::grantCPUSlot()
{
    hasSlot = true;
    thisQuantumRan = 0;
}
Process::~Process()
{
    while (running == true)
    {
        // Wait for thread end
    }

    // Clear dynamic thread if necessary
    delete processThread;
}


// SimulationMemory
std::unordered_map<std::string, int> SimulationMemory::sharedMemory;
std::vector <std::string> SimulationMemory::operationLog;
std::mutex SimulationMemory::registerLock;
std::unordered_map<std::string, std::mutex*> SimulationMemory::registeredMemory;
// 0 is global, 1 is local, 2 is specific, and 3 is off
//int protectionLevel = 0;


void SimulationMemory::setProtectionStrength(int inStrength)
{
    protectionLevel = inStrength;
}

bool isJSONFormatting(char inChar)
{
    switch (inChar)
    {
        case '{':
        case '}':
        case ':':
        case ',':
        case '"':
        {
            return true;
        }
        default:
        {
            return false;
        }
    }
}

void SimulationMemory::stripJSON(std::string& inString)
{

    inString.erase(std::remove_if(inString.begin(),
        inString.end(), &isJSONFormatting), inString.end());
}

// SimulationLogic
void SimulationMemory::loadMemory(std::string inFileName)
{
    // We will read memory from a file
    std::ifstream inFile;
    inFile.open("memory.json");
    if (inFile.is_open())
    {
        // Load in A:
        std::string sectionString = "";
        std::string bufferString = "";
        std::string numBuffer = "";
        // Grab A, or '{' if using JSON.
        inFile >> sectionString;
        if (sectionString == "{")
        {
            inFile >> sectionString >> bufferString;
        }
        // Iterate through the numbers, grabbing address and pair in sequence.
        // JSON characters will be stripped along the way.
        for (int index = 100; index < 255 && inFile; index += 5)
        {
            inFile >> bufferString >> numBuffer;
            bufferString = sectionString + bufferString;
            stripJSON(bufferString);
            sharedMemory.insert({ bufferString, std::stoi(numBuffer) });
            registeredMemory.insert({ bufferString, new std::mutex });
        }
        registeredMemory.insert({ "A", new std::mutex });

        // Load in B:
        sectionString = "";
        bufferString = "";
        numBuffer = "";
        if (inFile)
        {
            inFile >> sectionString;
        }
        if (sectionString == "},")
        {
            inFile >> sectionString >> bufferString;
        }
        for (int index = 100; index < 255 && inFile; index += 5)
        {
            inFile >> bufferString >> numBuffer;
            bufferString = sectionString + bufferString;
            stripJSON(bufferString);
            sharedMemory.insert({ bufferString, std::stoi(numBuffer) });
            registeredMemory.insert({ bufferString, new std::mutex });
        }
        registeredMemory.insert({ "B", new std::mutex });

        // Load in C:
        sectionString = "";
        bufferString = "";
        numBuffer = "";
        if (inFile)
        {
            inFile >> sectionString;
        }
        if (sectionString == "},")
        {
            inFile >> sectionString >> bufferString;
        }
        for (int index = 100; index < 255 && inFile; index += 5)
        {
            inFile >> bufferString >> numBuffer;
            bufferString = sectionString + bufferString;
            stripJSON(bufferString);
            sharedMemory.insert({ bufferString, std::stoi(numBuffer) });
            registeredMemory.insert({ bufferString, new std::mutex });
        }
        registeredMemory.insert({ "C", new std::mutex });

        // Load in D:
        sectionString = "";
        bufferString = "";
        numBuffer = "";
        if (inFile)
        {
            inFile >> sectionString;
        }
        if (sectionString == "},")
        {
            inFile >> sectionString >> bufferString;
        }
        for (int index = 100; index < 255 && inFile; index += 5)
        {
            inFile >> bufferString >> numBuffer;
            bufferString = sectionString + bufferString;
            stripJSON(bufferString);
            sharedMemory.insert({ bufferString, std::stoi(numBuffer) });
            registeredMemory.insert({ bufferString, new std::mutex });
        }
        registeredMemory.insert({ "D", new std::mutex });

        registeredMemory.insert({ "FULL", new std::mutex });

        // Initialize registers to zero
        sharedMemory.insert({ "R1", 0 });
        sharedMemory.insert({ "R2", 0 });
        sharedMemory.insert({ "R3", 0 });
        sharedMemory.insert({ "R4", 0 });
        registeredMemory.insert({ "R1", new std::mutex });
        registeredMemory.insert({ "R2", new std::mutex });
        registeredMemory.insert({ "R3", new std::mutex });
        registeredMemory.insert({ "R4", new std::mutex });

        registeredMemory.insert({ "Op", new std::mutex });

        inFile.close();
    }
}

void SimulationMemory::dumpMemory(std::string inFileName)
{
    std::ofstream saveFileGen;
    saveFileGen.open(inFileName);

    saveFileGen << "{" << std::endl;


    // Store contents of A
    saveFileGen << "    \"";
    saveFileGen << "A" << "\": {" << std::endl;
    std::string tempVar = "";
    for (int index = 100; index < 255; index += 5)
    {
        // Generate from 1 to nine (inclusive)
        tempVar = std::to_string(index);
        saveFileGen << "        \"" << index << "\": " << std::to_string(sharedMemory.at("A" + tempVar));
        if (index != 250)
        {
            saveFileGen << ",";
        }
        saveFileGen << std::endl;
    }
    saveFileGen << "    }," << std::endl;

    // Store contents of B
    saveFileGen << "B" << "\": {" << std::endl;
    tempVar = "";
    for (int index = 100; index < 255; index += 5)
    {
        // Generate from 1 to nine (inclusive)
        tempVar = std::to_string(index);
        saveFileGen << "        \"" << index << "\": " << std::to_string(sharedMemory.at("B" + tempVar));
        if (index != 250)
        {
            saveFileGen << ",";
        }
        saveFileGen << std::endl;
    }
    saveFileGen << "    }," << std::endl;

    // Store contents of C
    saveFileGen << "C" << "\": {" << std::endl;
    tempVar = "";
    for (int index = 100; index < 255; index += 5)
    {
        // Generate from 1 to nine (inclusive)
        tempVar = std::to_string(index);
        saveFileGen << "        \"" << index << "\": " << std::to_string(sharedMemory.at("C" + tempVar));
        if (index != 250)
        {
            saveFileGen << ",";
        }
        saveFileGen << std::endl;
    }
    saveFileGen << "    }," << std::endl;

    // Store contents of D
    saveFileGen << "A" << "\": {" << std::endl;
    tempVar = "";
    for (int index = 100; index < 255; index += 5)
    {
        // Generate from 1 to nine (inclusive)
        tempVar = std::to_string(index);
        saveFileGen << "        \"" << index << "\": " << std::to_string(sharedMemory.at("A" + tempVar));
        if (index != 250)
        {
            saveFileGen << ",";
        }
        saveFileGen << std::endl;
    }
    saveFileGen << "    }" << std::endl;
    saveFileGen << "}";

    saveFileGen.close();
}
bool SimulationMemory::registerPriority(int inID)
{
    registeredMemory.at("R3")->lock();


    return true;
}
bool SimulationMemory::registerMemory(bool hasPrivileged, 
    std::string memory1, std::string memory2)
{
    // Only allow priority after grabbing the signal
    if ((hasPrivileged == false && (memory1[0] == 'D' 
        || memory2[0] == 'D')) && protectionLevel != 3)
    {
        return false;
    }

    bool returnResult = true;

    // Acquire lock to the signals
    registerLock.lock();
    std::string part1 = memory1;
    std::string part2 = memory2;
    switch (protectionLevel)
    {
    case 0:
        // High
        registeredMemory.at("FULL")->lock();
        break;
    case 1:
        // Section
        // Make sure they aren't the same
        if (part1 == part2)
        {
            // Registers get a specific lock still
            if (part1[0] == 'R')
            {
                registeredMemory.at(part1)->lock();
            }
            else
            {
                std::string tempString = "";
                tempString += part1[0];
                registeredMemory.at(tempString)->lock();
            }
        }
        else
        {
            // Registers get a specific lock still
            if (part1[0] == 'R')
            {
                registeredMemory.at(part1)->lock();
            }
            else
            {
                std::string tempString = "";
                tempString += part1[0];
                registeredMemory.at(tempString)->lock();
            }

            if (part2[0] == 'R')
            {
                registeredMemory.at(part2)->lock();
            }
            else
            {
                std::string tempString = "";
                tempString += part2[0];
                registeredMemory.at(tempString)->lock();
            }
        }
        break;
    case 2:
        // Specific
        // Make sure they aren't the same
        if (part1 == part2)
        {
            registeredMemory.at(part1)->lock();
        }
        else
        {
            registeredMemory.at(part1)->lock();
            registeredMemory.at(part2)->lock();
        }
        break;
    case 3:
        // None
        returnResult = true;
    }


    registerLock.unlock();

    return returnResult;
}

void SimulationMemory::unRegisterMemory(std::string request1, std::string request2)
{
    // Acquire lock to the signals
    std::string part1 = request1;
    std::string part2 = request2;
    switch (protectionLevel)
    {
    case 0:
        // High
        registeredMemory.at("FULL")->unlock();
        break;
    case 1:
        // Section
        // Make sure they aren't the same
        if (part1 == part2)
        {
            // Registers get a specific lock still
            if (part1[0] == 'R')
            {
                registeredMemory.at(part1)->unlock();
            }
            else
            {
                std::string tempString = "";
                tempString += part1[0];
                registeredMemory.at(tempString)->unlock();
            }
        }
        else
        {
            // Registers get a specific lock still
            if (part1[0] == 'R')
            {
                registeredMemory.at(part1)->unlock();
            }
            else
            {
                std::string tempString = "";
                tempString += part1[0];
                registeredMemory.at(tempString)->unlock();
            }

            if (part2[0] == 'R')
            {
                registeredMemory.at(part2)->unlock();
            }
            else
            {
                std::string tempString = "";
                tempString += part2[0];
                registeredMemory.at(tempString)->unlock();
            }
        }
        break;
    case 2:
        // Specific
        // Make sure they aren't the same
        if (part1 == part2)
        {
            registeredMemory.at(part1)->unlock();
        }
        else
        {
            registeredMemory.at(part1)->unlock();
            registeredMemory.at(part2)->unlock();
        }
        break;
    }
}

void SimulationMemory::returnPriority()
{
    sharedMemory.at("R4") += 1;
}

void SimulationMemory::updateOpLog(std::string inString)
{

    registeredMemory.at("Op")->lock();
    operationLog.push_back(inString);
    registeredMemory.at("Op")->unlock();
}

void SimulationMemory::clearMemory()
{
    // Delete all dynamically allocated mutexes
    for (int index = 100; index < 255; index += 5)
    {
        std::string tempString = "A";
        std::string tempString2 = "B";
        std::string tempString3 = "C";
        std::string tempString4 = "D";

        delete registeredMemory.at(tempString + std::to_string(index));
        delete registeredMemory.at(tempString2 + std::to_string(index));
        delete registeredMemory.at(tempString3 + std::to_string(index));
        delete registeredMemory.at(tempString4 + std::to_string(index));
    }
    delete registeredMemory.at("A");
    delete registeredMemory.at("B");
    delete registeredMemory.at("C");
    delete registeredMemory.at("D");


    delete registeredMemory.at("R1");
    delete registeredMemory.at("R3");
    delete registeredMemory.at("R3");
    delete registeredMemory.at("R4");


    delete registeredMemory.at("FULL");

    delete registeredMemory.at("Op");
}

// SimulationLogic
SimulationLogic::SimulationLogic()
{
    // Initialize pseudorandomness if applicable
    std::string holdSeed = "";
    int seed = -1;

    std::ifstream randFile;
    randFile.open("Random Settings.conf");
    if (randFile.is_open())
    {
        // Read in seed
        while (randFile)
        {
            randFile >> holdSeed;
        }

        // Grab in seed if valid format
        if (holdSeed.size() > 0 && std::isdigit(holdSeed[0]))
        {
            seed = stoi(holdSeed);
        }

        // Close file
        randFile.close();
    }

    if (seed != -1)
    {
        srand(seed);
    }
    else
    {
        srand(time(NULL));
    }

    // Initialize visualization object coordinates
    simLabel.setCoords(0.0, 0.05, 1.0, 0.15);
    simLabel.setText("Project 3: Reader/Writer");
    cpuLabel.setText("CPU");
    cpuBox.setCoords(0.1, 0.2, 0.6, 0.3);
    cpuLabel.setCoords(0.1, 0.25, 0.6, 0.4);
    memoryBox.setCoords(0.1, 0.37, 0.6, 0.87);
    memoryLabel.setCoords(0.1, 0.83, 0.6, 0.98);
    memoryLabel.setText("Memory");
    writerBox.setCoords(0.64, 0.2, 0.7, 0.87);
    readerBox.setCoords(0.74, 0.2, 0.9, 0.87);
    writerLabel.setCoords(0.64, 0.83, 0.7, 0.98);
    readerLabel.setCoords(0.74, 0.83, 0.9, 0.98);
    readerLabel.setText("Readers");
    writerLabel.setText("Writers");
    exitButton.setCoords(0.38, 0.73, 0.62, 0.87);
    exitButton.setText("Abort Simulation");
    r1Box.setCoords(0.3, 0.22, 0.35, 0.28);
    r2Box.setCoords(0.38, 0.22, 0.43, 0.28);
    r3Box.setCoords(0.46, 0.22, 0.51, 0.28);
    r4Box.setCoords(0.54, 0.22, 0.59, 0.28);
    r1Label.setCoords(0.3, 0.22, 0.35, 0.28);
    r1Label.setText("R1: ");
    r2Label.setCoords(0.38, 0.22, 0.43, 0.28);
    r2Label.setText("R2: ");
    r3Label.setCoords(0.46, 0.22, 0.51, 0.28);
    r3Label.setText("R3: ");
    r4Label.setCoords(0.54, 0.22, 0.59, 0.28);
    r4Label.setText("R4: ");
    operationLabel.setCoords(0.1, 0.2, 0.3, 0.3);
    operationLabel.setText("");
    memoryCellText.setCoords(0.0, 0.25, 1.0, 0.5);
    memoryCellText.setText("");
    memoryCellText.setMinScaling(false);
}
SimulationLogic::SimulationLogic(HWND inHandle)
{
    // Initialize pseudorandomness if applicable
    std::string holdSeed = "";
    int seed = -1;

    std::ifstream randFile;
    randFile.open("Random Settings.conf");
    if (randFile.is_open())
    {
        // Read in seed
        while (randFile)
        {
            randFile >> holdSeed;
        }

        // Grab in seed if valid format
        if (holdSeed.size() > 0 && std::isdigit(holdSeed[0]))
        {
            seed = stoi(holdSeed);
        }

        // Close file
        randFile.close();
    }

    if (seed != -1)
    {
        srand(seed);
    }

    hWnd = inHandle;

    // Initialize visualization object coordinates
    simLabel.setCoords(0.0, 0.05, 1.0, 0.15);
    simLabel.setText("Project 3/4: Reader/Writer");
    cpuLabel.setText("CPU");
    cpuBox.setCoords(0.1, 0.2, 0.6, 0.3);
    cpuLabel.setCoords(0.1, 0.25, 0.6, 0.4);
    memoryBox.setCoords(0.1, 0.37, 0.6, 0.87);
    memoryLabel.setCoords(0.1, 0.83, 0.6, 0.98);
    memoryLabel.setText("Memory");
    writerBox.setCoords(0.64, 0.2, 0.7, 0.87);
    readerBox.setCoords(0.74, 0.2, 0.9, 0.87);
    writerLabel.setCoords(0.64, 0.83, 0.7, 0.98);
    readerLabel.setCoords(0.74, 0.83, 0.9, 0.98);
    readerLabel.setText("Readers");
    writerLabel.setText("Writers");
    exitButton.setCoords(0.38, 0.73, 0.62, 0.87);
    exitButton.setText("Exit Simulation");
    r1Box.setCoords(0.3, 0.22, 0.35, 0.28);
    r2Box.setCoords(0.38, 0.22, 0.43, 0.28);
    r3Box.setCoords(0.46, 0.22, 0.51, 0.28);
    r4Box.setCoords(0.54, 0.22, 0.59, 0.28);
    r1Label.setCoords(0.3, 0.22, 0.35, 0.28);
    r1Label.setText("R1: ");
    r2Label.setCoords(0.38, 0.22, 0.43, 0.28);
    r2Label.setText("R2: ");
    r3Label.setCoords(0.46, 0.22, 0.51, 0.28);
    r3Label.setText("R3: ");
    r4Label.setCoords(0.54, 0.22, 0.59, 0.28);
    r4Label.setText("R4: ");
    operationLabel.setCoords(0.1, 0.2, 0.3, 0.3);
    operationLabel.setText("");
    memoryCellText.setCoords(0.0, 0.25, 1.0, 0.5);
    memoryCellText.setText("");
    memoryCellText.setMinScaling(false);

    // Set visualization handles to window
    setHWND(inHandle);
}

void SimulationLogic::setHWND(HWND inHandle) 
{ 
    // Set visualization handles to window
    simLabel.setHWND(inHandle);
    cpuLabel.setHWND(inHandle);
    cpuBox.setHWND(inHandle);
    memoryLabel.setHWND(inHandle);
    readerLabel.setHWND(inHandle);
    writerLabel.setHWND(inHandle);
    readerBox.setHWND(inHandle);
    writerBox.setHWND(inHandle);
    memoryBox.setHWND(inHandle);
    exitButton.setHWND(inHandle);

    r1Box.setHWND(inHandle);
    r2Box.setHWND(inHandle);
    r3Box.setHWND(inHandle);
    r4Box.setHWND(inHandle);
    r1Label.setHWND(inHandle);
    r2Label.setHWND(inHandle);
    r3Label.setHWND(inHandle);
    r4Label.setHWND(inHandle);
    operationLabel.setHWND(inHandle);
    memoryCellText.setHWND(inHandle);
}
void SimulationLogic::setParameters(int inWriters, int inReaders, 
    int inQuantum, int inProcesses)
{
    writers = inWriters;
    readers = inReaders;
    timeQuantum = inQuantum;
    numCPUs = inProcesses;
}
void SimulationLogic::setProtectionStrength(int inStrength)
{
    protectionLevel = inStrength;
    initializerMemory.setProtectionStrength(protectionLevel);
}
int SimulationLogic::cycleLogic(HRESULT& hr,
    ID2D1HwndRenderTarget*& pRenderTarget)
{

    drawFrame(hr, pRenderTarget);

    // Iterate through processes and check if we are done
    bool foundRemaining = false;
    for (int index = 0; index < processList.size(); index++)
    {
        if (processList[index].getRemaining() > 0)
        {
            foundRemaining = true;
        }
    }

    // Signal to return to results screen if done
    // Otherwise signal to continue simulation
    if (foundRemaining == false)
    {
        //return 2;
        initializerMemory.dumpMemory("memory.json");
        return 2;
    }
    else
    {
        return 1;
    }
}
void SimulationLogic::drawFrame(HRESULT& hr,
    ID2D1HwndRenderTarget*& pRenderTarget)
{
    // Draw Outline Text and borders
    cpuLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    readerLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    writerLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    memoryLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    cpuBox.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    readerBox.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    writerBox.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    memoryBox.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    simLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);

    // Correct Register Values Display
    std::string tempString = "R1: ";
    tempString += std::to_string(initializerMemory.sharedMemory.at("R1"));
    r1Label.setText(tempString);
    tempString = "R2: ";
    tempString += std::to_string(initializerMemory.sharedMemory.at("R2"));
    r2Label.setText(tempString);
    tempString = "R3: ";
    tempString += std::to_string(initializerMemory.sharedMemory.at("R3"));
    r3Label.setText(tempString);
    tempString = "R4: ";
    tempString += std::to_string(initializerMemory.sharedMemory.at("R4"));
    r4Label.setText(tempString);
    tempString = "";
    if (initializerMemory.operationLog.size() > 0)
    {

        tempString = initializerMemory.operationLog[
            initializerMemory.operationLog.size() - 1];
    }
    operationLabel.setText(tempString);

    // Display registers
    r1Box.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    r2Box.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    r3Box.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    r4Box.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    r1Label.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    r2Label.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    r3Label.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    r4Label.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    operationLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);

    // Display Memory
    // Display "A"
    // We will be displaying memory sections A - D.
    // We will begin by calculating the base position and
    // offsets for A. Then, we will go ahead and apply
    // the offsets for B-D as we go, drawing the
    // appropriate memory cells along the way
    std::string regString = "A";
    // Positions and offsets were obtained by looking
    // at the coordinates for the memoryBox object,
    // then trying to calculate how much space each
    // cell would take if we divided all memory cells
    // into an area the size of one fourth of that.
    // Then we have to add in some wiggle room
    // because the 31 is prime, so we will have to
    // waste a bunch of space for one row.
    // That also means we have to manually do
    // one position after the loop.
    float baseX = 0.11;
    float baseY = 0.38;
    const float offsetX = 0.024;
    const float offsetY = (0.5) / 30;
    for (int index = 0; index < 30; index += 5)
    {
        // Do row
        for (int inIndex = index; inIndex < index + 5; inIndex++)
        {
            int adjustedIndex = (5 * inIndex) + 100;
            tempString = regString + std::to_string(adjustedIndex);
            int tempValue = initializerMemory.sharedMemory.at(tempString);
            tempString = tempString + ": " + std::to_string(tempValue);
            memoryCellText.setCoords(
                baseX + ((inIndex % 5) * offsetX ), //Left
                baseY + (index * offsetY), // Up
                baseX + offsetX + ((inIndex % 5) * offsetX), // Right
                baseY + offsetY + (index * offsetY)); // Down
            memoryCellText.setText(tempString);
            memoryCellText.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
        }
    }
    // Manually do the last one since it won't fit
    int adjustedIndex = 250;
    tempString = regString + std::to_string(adjustedIndex);
    int tempValue = initializerMemory.sharedMemory.at(tempString);
    tempString = tempString + ": " + std::to_string(tempValue);
    memoryCellText.setCoords(
        baseX + ((2) * offsetX), //Left
        baseY + (27 * offsetY), // Up
        baseX + offsetX + ((2.15) * offsetX), // Right
        baseY + offsetY + (27 * offsetY)); // Down
    memoryCellText.setText(tempString);
    memoryCellText.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    // Display "B"
    regString = "B";
    baseX = baseX + (5 * offsetX);
    for (int index = 0; index < 30; index += 5)
    {
        // Do row
        for (int inIndex = index; inIndex < index + 5; inIndex++)
        {
            adjustedIndex = (5 * inIndex) + 100;
            tempString = regString + std::to_string(adjustedIndex);
            tempValue = initializerMemory.sharedMemory.at(tempString);
            tempString = tempString + ": " + std::to_string(tempValue);
            memoryCellText.setCoords(
                baseX + ((inIndex % 5) * offsetX), //Left
                baseY + (index * offsetY), // Up
                baseX + offsetX + ((inIndex % 5) * offsetX), // Right
                baseY + offsetY + (index * offsetY)); // Down
            memoryCellText.setText(tempString);
            memoryCellText.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
        }
    }
    // Manually do the last one since it won't fit
    adjustedIndex = 250;
    tempString = regString + std::to_string(adjustedIndex);
    tempValue = initializerMemory.sharedMemory.at(tempString);
    tempString = tempString + ": " + std::to_string(tempValue);
    memoryCellText.setCoords(
        baseX + ((2) * offsetX), //Left
        baseY + (27 * offsetY), // Up
        baseX + offsetX + ((2.15) * offsetX), // Right
        baseY + offsetY + (27 * offsetY)); // Down
    memoryCellText.setText(tempString);
    memoryCellText.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    // Display "C"
    regString = "C";
    baseX = baseX + (5 * offsetX);
    for (int index = 0; index < 30; index += 5)
    {
        // Do row
        for (int inIndex = index; inIndex < index + 5; inIndex++)
        {
            adjustedIndex = (5 * inIndex) + 100;
            tempString = regString + std::to_string(adjustedIndex);
            tempValue = initializerMemory.sharedMemory.at(tempString);
            tempString = tempString + ": " + std::to_string(tempValue);
            memoryCellText.setCoords(
                baseX + ((inIndex % 5) * offsetX), //Left
                baseY + (index * offsetY), // Up
                baseX + offsetX + ((inIndex % 5) * offsetX), // Right
                baseY + offsetY + (index * offsetY)); // Down
            memoryCellText.setText(tempString);
            memoryCellText.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
        }
    }
    // Manually do the last one since it won't fit
    adjustedIndex = 250;
    tempString = regString + std::to_string(adjustedIndex);
    tempValue = initializerMemory.sharedMemory.at(tempString);
    tempString = tempString + ": " + std::to_string(tempValue);
    memoryCellText.setCoords(
        baseX + ((2) * offsetX), //Left
        baseY + (27 * offsetY), // Up
        baseX + offsetX + ((2.15) * offsetX), // Right
        baseY + offsetY + (27 * offsetY)); // Down
    memoryCellText.setText(tempString);
    memoryCellText.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    // Display "D"
    regString = "D";
    baseX = baseX + (5 * offsetX);
    for (int index = 0; index < 30; index += 5)
    {
        // Do row
        for (int inIndex = index; inIndex < index + 5; inIndex++)
        {
            adjustedIndex = (5 * inIndex) + 100;
            tempString = regString + std::to_string(adjustedIndex);
            tempValue = initializerMemory.sharedMemory.at(tempString);
            tempString = tempString + ": " + std::to_string(tempValue);
            memoryCellText.setCoords(
                baseX + ((inIndex % 5) * offsetX), //Left
                baseY + (index * offsetY), // Up
                baseX + offsetX + ((inIndex % 5) * offsetX), // Right
                baseY + offsetY + (index * offsetY)); // Down
            memoryCellText.setText(tempString);
            memoryCellText.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
        }
    }
    // Manually do the last one since it won't fit
    adjustedIndex = 250;
    tempString = regString + std::to_string(adjustedIndex);
    tempValue = initializerMemory.sharedMemory.at(tempString);
    tempString = tempString + ": " + std::to_string(tempValue);
    memoryCellText.setCoords(
        baseX + ((2) * offsetX), //Left
        baseY + (27 * offsetY), // Up
        baseX + offsetX + ((2.15) * offsetX), // Right
        baseY + offsetY + (27 * offsetY)); // Down
    memoryCellText.setText(tempString);
    memoryCellText.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);

    // Calculate position and call functions to draw readers
    //writerBox.setCoords(0.64, 0.2, 0.7, 0.87);
    //readerBox.setCoords(0.74, 0.2, 0.9, 0.87);
    baseX = 0.74;
    baseY = 0.2;
    float readOffsetX = (0.9 - baseX) / 5;
    // Find how much space each row will take.
    float readOffsetY = (0.87 - baseY) / std::ceil(float(readers) / 5);
    for (int index = 0; index < readers; index += 5)
    {
        // Do row
        for (int inIndex = index; inIndex < index + 5 && 
            inIndex < readers; inIndex++)
        {

            // Find readers in right order
            for (int deepIndex = 0; deepIndex < processList.size(); 
                deepIndex++)
            {
                // Make sure this is the right one
                // and pass it the drawing coordinates
                // Making a map instead would reduce
                // Big O complexity here a lot.
                if (processList[deepIndex].getReader() == true && 
                    processList[deepIndex].getID() == inIndex)
                {
                    processList[deepIndex].drawRepresentation(
                        baseX + ((inIndex % 5) * readOffsetX), //Left
                        baseY + ((index  / 5) * readOffsetY), //Right
                        baseX + readOffsetX + ((inIndex % 5) * readOffsetX),
                        baseY + readOffsetY + (((index / 5) * readOffsetY)),
                        hr, pRenderTarget);

                    // break to do the next one
                    deepIndex = processList.size();
                }
            }
        }
    }

    // Calculate position and call functions to draw readers
    //writerBox.setCoords(0.64, 0.2, 0.7, 0.87);
    //readerBox.setCoords(0.74, 0.2, 0.9, 0.87);
    baseX = 0.64;
    baseY = 0.2;
    float writeOffsetX = (0.7 - baseX);
    // Find how much space each row will take.
    float writeOffsetY = (0.87 - baseY) / std::ceil(float(writers));
    for (int index = 0; index < writers; index++)
    {
        // Find readers in right order
        for (int deepIndex = 0; deepIndex < processList.size();
            deepIndex++)
        {
            // Make sure this is the right one
            // and pass it the drawing coordinates
            // Making a map instead would reduce
            // Big O complexity here a lot.
            if (processList[deepIndex].getReader() == false &&
                processList[deepIndex].getID() == index)
            {
                processList[deepIndex].drawRepresentation(
                    baseX, //Left
                    baseY + (index * writeOffsetY), //Right
                    baseX + writeOffsetX,
                    baseY + writeOffsetY + (index * writeOffsetY),
                    hr, pRenderTarget);

                // break to do the next one
                deepIndex = processList.size();
            }
        }
    }
}
int SimulationLogic::cycleResults(int inX, int inY, long inTime, 
    bool inClicked, HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
   // Draw title
    simLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);

    std::string resultsText = "Final Results: Simulation finished in ";
    resultsText += std::to_string(inTime / 1000.0);
    resultsText += " seconds.";

    operationLabel.setCoords(0.0, 0.3, 1.0, 0.6);
    operationLabel.setText(resultsText);
    operationLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);

    // Exit button
    if (exitButton.cycleButton(inX, inY, inClicked, hr, pRenderTarget))
    {
        // Exit simulation and restart
        return 3;
    }
    else
    {
        // Return here next frame
        return 2;
    }
}
void SimulationLogic::genRandomFile()
{
    std::ofstream inFileGen;
    inFileGen.open("memory.json");

    // Generate contents of A
    inFileGen << "{" << std::endl;
    inFileGen << "    \"";
    inFileGen << "A" << "\": {" << std::endl;
    std::string tempVar = "";
    for (int index = 100; index < 255; index += 5)
    {
        // Generate from 1 to nine (inclusive)
        tempVar = std::to_string((rand() % 9 + 1));
        inFileGen << "        \"" << index << "\": " << tempVar;
        if (index != 250)
        {
            inFileGen << ",";
        }
        inFileGen << std::endl;
    }
    inFileGen << "    }," << std::endl;

    // Generate contents of B
    inFileGen << "    \"B\": {" << std::endl;
    tempVar = "";
    for (int index = 100; index < 255; index += 5)
    {
        // Generate from 1 to nine (inclusive)
        tempVar = std::to_string((rand() % 9 + 1));
        inFileGen << "        \"" << index << "\": " << tempVar;
        if (index != 250)
        {
            inFileGen << ",";
        }
        inFileGen << std::endl;
    }
    inFileGen << "    }," << std::endl;

    // Generate contents of C
    inFileGen << "    \"C\": {" << std::endl;
    tempVar = "";
    for (int index = 100; index < 255; index += 5)
    {
        // Generate from 1 to nine (inclusive)
        tempVar = std::to_string((rand() % 9 + 1));
        inFileGen << "        \"" << index << "\": " << tempVar;
        if (index != 250)
        {
            inFileGen << ",";
        }
        inFileGen << std::endl;
    }
    inFileGen << "    }," << std::endl;

    // Generate contents of C
    inFileGen << "    \"D\": {" << std::endl;
    tempVar = "";
    for (int index = 100; index < 255; index += 5)
    {
        // Generate from 1 to nine (inclusive)
        tempVar = std::to_string((rand() % 9 + 1));
        inFileGen << "        \"" << index << "\": " << tempVar;
        if (index != 250)
        {
            inFileGen << ",";
        }
        inFileGen << std::endl;
    }
    inFileGen << "    }" << std::endl;
    inFileGen << "}";

    inFileGen.close();
}
// Simple function to ensure shuffle gives us consistent results 
// witht the same srand() seed.
int shuffleHelper(int inRange)
{
    return rand() % inRange;
}
void SimulationLogic::launchSim()
{
    // Load up memory
    initializerMemory.loadMemory("memory.json");

    // Make threads
    for (int index = 0; index < (writers + readers); index++)
    {
        processList.push_back(Process(hWnd));
        processList[index].setQuantum(timeQuantum);
        processList[index].setProtectionLevel(protectionLevel);
    }

    // Pseudorandomly assign chosen priority instructions
    for (int index = 0; index < writers; index++)
    {
        processList[index].setReader(false);
        processList[index].setPriviliged(index);
    }
    // Shuffle to ensure the randomness
    if (writers > 0)
    {
        random_shuffle(processList.begin(), processList.begin() + writers, 
            shuffleHelper);
    }

    // Assign the process IDs to each writer
    for (int index = 0; index < writers; index++)
    {
        processList[index].setID(index);
        processList[index].fillInstructions();
    }

    // Assign reader IDs after all writers
    for (int index = writers; index < (writers + readers); index++)
    {
        processList[index].setID(index - writers);
        processList[index].fillInstructions();
    }

    // Shuffle one more time so readers and writers are in pseudorandom order
    random_shuffle(processList.begin(), processList.end(), shuffleHelper);

    // Launch threads
    for (int index = 0; index < processList.size(); index++)
    {
        processList[index].runProcess();
    }

    // Make a thread to carry out round robin assignments
    running = true;
    dispatcherThread = new std::thread(
        &SimulationLogic::roudRobinDispatcher, this);
    dispatcherThread->detach();
}
void SimulationLogic::roudRobinDispatcher()
{
    endSafe = false;
    while (running == true)
    {
        // Just continuously activate if no need for robin
        if (numCPUs >= (writers + readers))
        {
            for (int index = 0; index < processList.size(); index++)
            {
                processList[index].grantCPUSlot();
            }
        }
        else
        {
            // Count how many threads are still going and how many
            // are using a CPU slot
            int numGranted = 0;
            int numContinued = 0;
            for (int index = 0; index < processList.size(); index++)
            {
                if (processList[index].getRemaining() > 0)
                {
                    numContinued++;
                    if (processList[index].getActive() == true)
                    {
                        numGranted++;
                    }
                }
            }

            // Continue down the order granting robin slots
            // to inactive processes with work left
            while ((numGranted < numCPUs) && (robinPlacement < 
                processList.size()))
            {
                if (processList[robinPlacement].getActive() == false)
                {
                    if (processList[robinPlacement].getRemaining() > 0)
                    {
                        processList[robinPlacement].grantCPUSlot();
                        numGranted++;
                    }
                }
                robinPlacement++;
            }
            // Reset position for next cycle
            if (robinPlacement >= processList.size())
            {
                robinPlacement = 0;
            }
        }

        // Limit resource use
        Sleep(100);
    }


    endSafe = true;

    std::cout << "End of dispatcher." << std::endl;
}
SimulationLogic::~SimulationLogic()
{
    running = false;
    while (endSafe == false)
    {
        // Wait until thread is done closing.
        Sleep(10);
    }

    delete dispatcherThread;

    initializerMemory.clearMemory();
}