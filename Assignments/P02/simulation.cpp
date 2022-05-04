///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P02 CPU Scheduling - Simulation
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
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

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
    barRepresentation = RectangleShape();
    textRepresentation = TextBox();
    textRepresentation.setMinScaling(false);
}
Process::Process(HWND inHandle)
{
    hWnd = inHandle;
    barRepresentation = RectangleShape(hWnd);
    textRepresentation = TextBox(hWnd);
    textRepresentation.setMinScaling(false);
}
void Process::setHWND(HWND inHandle)
{
    hWnd = inHandle;
    barRepresentation.setHWND(hWnd);
    textRepresentation.setHWND(hWnd);
}
void Process::setName(std::string inName)
{
    processName = inName;
    textRepresentation.setText(inName);
}
int Process::cycleCPU()
{
    // Process current job
    lastState = 'c';
    currentCPUCycles++;
    processCycles++;
    unfinishedCPU[0]--;

    // Prepare for next state
    if (unfinishedCPU[0] == 0)
    {
        // Waiting notification code
        // Switching away from a vector is good if the
        // simulation gets much larger, but usually
        // it's only less than ten elements.
        unfinishedCPU.erase(unfinishedCPU.begin());

        // Job is finished
        if (unfinishedCPU.size() == 0)
        {
            // Generic finished return code;
            return -2;
        }
        // Moving to IO
        else
        {
            // Generic IO move return code
            waitingIO = true;
            return -1;
        }
    }
    else
    {
        // Return current length of CPU burst if some left
        return currentCPUCycles;
    }
}
int Process::cycleIO()
{

    // Process current job
    lastState = 'i';
    ioCycles++;
    unfinishedIO[0]--;

    // Prepare for next state
    if (unfinishedIO[0] == 0)
    {
        unfinishedIO.erase(unfinishedIO.begin());
        waitingIO = false;
        // Ready notification code
        return -1;
    }
    else
    {
        // Generic return code
        return 1;
    }
}
void Process::cycleWait()
{
    // Adjust internal state for preemptions
    if (lastState == 'c')
    {
        // Just moved to waiting Que
        if (waitingIO == true)
        {
            lastState = 'w';
            waitingCycles++;
        }
        // Preempted
        else
        {
            currentCPUCycles = 0;
            lastState = 'r';
            readyCycles++;
        }
    }
    // Just finished IO
    else if (lastState == 'i')
    {
        lastState = 'r';
        readyCycles++;
    }
    // Is still on ready que.
    else if (lastState == 'r')
    {
        readyCycles++;
    }
    // Is still on IO waiting que
    else
    {
        waitingCycles++;
    }

    // Handle promotion
    if (canPromote == true && promotion < 2)
    {
        promotion += 0.1;
    }
}
void Process::drawBar(float inTopX, float inTopY, float inBottomX,
    float inBottomY, HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
    // Early exit to avoid potential out of bounds vector access
    // on last call
    if (unfinishedIO.size() == 0 && unfinishedCPU.size() == 0)
    {
        return;
    }

    float colorScale = 1;
    std::string barMessage = "P";
    barMessage += processName;
    barMessage += ": ";
    float adjustedTopY = 0;
    float adjustedBarScale = inBottomY - inTopY;

    // Adding text based on remaining time on relevant stage,
    // as well as bar height and redscale of bar
    if (waitingIO == true)
    {
        colorScale = adjustedTopY = unfinishedIO[0] / scale;
        barMessage += std::to_string(unfinishedIO[0]);
        barMessage += " IO in burst";
    }
    else
    {
        colorScale = adjustedTopY = unfinishedCPU[0] / scale;
        barMessage += std::to_string(unfinishedCPU[0]);
        barMessage += " CPU in burst";
    }
    adjustedTopY = inBottomY - (adjustedTopY * adjustedBarScale);

    // Bound bar height to max bar display bounds.
    if (adjustedTopY < inTopY)
    {
        adjustedTopY = inTopY;
    }

    // Update representations
    barRepresentation.setCoords(inTopX, adjustedTopY, inBottomX, inBottomY);
    textRepresentation.setCoords(inTopX, inTopY, inBottomX, inBottomY);
    textRepresentation.setText(barMessage);

    // Actually draw the representations
    // Don't draw text if it is too crowded
    barRepresentation.cycleRectangle(colorScale, 0.8, 0.8, hr, pRenderTarget);
    if (inBottomX - inTopX > 0.02)
    {
        textRepresentation.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    }
}

// SimulationLogic

SimulationLogic::SimulationLogic()
{
    messageLog.push_back("0 Beginning Simulation.");
    messageLog.push_back("__________________________");
    messageLog.push_back(" ");
    messageLog.push_back(" ");
    messageLog.push_back(" ");
    messageLog.push_back(" ");

    // Initialize visualization object coordinates
    simLabel.setCoords(0.0, 0.0, 1.0, 0.1);
    simLabel.setText("Project 2: CPU Scheduling");
    messageLogBox.setCoords(0.0, 0.0, 1.0, 0.4);
    cpuBox.setCoords(0.1, 0.55, 0.21, 0.9);
    ioBox.setCoords(0.24, 0.55, 0.36, 0.9);
    readyBox.setCoords(0.39, 0.55, 0.71, 0.9);
    waitingBox.setCoords(0.74, 0.55, 0.9, 0.9);
    cpuLabel.setCoords(0.1, 0.9, 0.2, 1.0);
    cpuLabel.setText("CPU Processing");
    ioLabel.setCoords(0.25, 0.9, 0.35, 1.0);
    ioLabel.setText("IO Processing");
    readyLabel.setText("CPU Ready Queue");
    readyLabel.setCoords(0.4, 0.9, 0.7, 1.0);
    waitingLabel.setText("IO Waiting Queue");
    waitingLabel.setCoords(0.75, 0.9, 0.9, 1.0);
    exitButton.setCoords(0.38, 0.73, 0.62, 0.87);
    exitButton.setText("Return To Menu");
}
SimulationLogic::SimulationLogic(HWND inHandle)
{
    hWnd = inHandle;
    messageLog.push_back("0 Beginning Simulation.");
    messageLog.push_back("__________________________");
    messageLog.push_back(" ");
    messageLog.push_back(" ");
    messageLog.push_back(" ");
    messageLog.push_back(" ");

    // Initialize visualization object coordinates
    simLabel.setCoords(0.0, 0.0, 1.0, 0.1);
    simLabel.setText("Project 2: CPU Scheduling");
    messageLogBox.setCoords(0.0, 0.0, 1.0, 0.4);
    cpuBox.setCoords(0.1, 0.55, 0.21, 0.9);
    ioBox.setCoords(0.24, 0.55, 0.36, 0.9);
    readyBox.setCoords(0.39, 0.55, 0.71, 0.9);
    waitingBox.setCoords(0.74, 0.55, 0.9, 0.9);
    cpuLabel.setCoords(0.1, 0.9, 0.2, 1.0);
    cpuLabel.setText("CPU Processing");
    ioLabel.setCoords(0.25, 0.9, 0.35, 1.0);
    ioLabel.setText("IO Processing");
    readyLabel.setText("CPU Ready Queue");
    readyLabel.setCoords(0.4, 0.9, 0.7, 1.0);
    waitingLabel.setText("IO Waiting Queue");
    waitingLabel.setCoords(0.75, 0.9, 0.9, 1.0);
    exitButton.setCoords(0.38, 0.73, 0.62, 0.87);
    exitButton.setText("Return To Menu");

    // Set visualization handles to window
    simLabel.setHWND(inHandle);
    cpuLabel.setHWND(inHandle);
    ioLabel.setHWND(inHandle);
    readyLabel.setHWND(inHandle);
    waitingLabel.setHWND(inHandle);
    cpuBox.setHWND(inHandle);
    ioBox.setHWND(inHandle);
    readyBox.setHWND(inHandle);
    waitingBox.setHWND(inHandle);
    exitButton.setHWND(inHandle);
}
void SimulationLogic::setHWND(HWND inHandle)
{
    hWnd = inHandle;

    // Set visualization handles to window
    simLabel.setHWND(inHandle);
    cpuLabel.setHWND(inHandle);
    ioLabel.setHWND(inHandle);
    readyLabel.setHWND(inHandle);
    waitingLabel.setHWND(inHandle);
    cpuBox.setHWND(inHandle);
    ioBox.setHWND(inHandle);
    readyBox.setHWND(inHandle);
    waitingBox.setHWND(inHandle);
    exitButton.setHWND(inHandle);
}

void SimulationLogic::setParameters(std::string inFileName, int inQuantum,
    std::string inAlgorithm, int inCPU, int inIO)
{
    // Check filename
    if (inFileName.length() > 0)
    {
        simFileName = inFileName;
    }

    // Set round robin time quantum
    if (inQuantum > 0)
    {
        timeQuantum = inQuantum;
    }

    // Bounds check CPU
    if (inCPU > 0)
    {
        numCPU = inCPU;
    }

    // Bounds Check IO
    if (inIO > 0)
    {
        numIO = inIO;
    }

    // Check validity of input algorithm
    // First Come First Serve: Code 1
    if (inAlgorithm == "FCFS")
    {
        scheduleAlgorithm = inAlgorithm;
        scheduleAlgorithmQuickCode = 1;
    }
    // Shortest Job First: Code 2
    else if (inAlgorithm == "SJF")
    {
        scheduleAlgorithm = inAlgorithm;
        scheduleAlgorithmQuickCode = 2;
    }
    // Shortest Remaining Time: Code 3
    else if (inAlgorithm == "SRT")
    {
        scheduleAlgorithm = inAlgorithm;
        scheduleAlgorithmQuickCode = 3;
    }
    // Priority Based: Code 4
    else if (inAlgorithm == "PB")
    {
        scheduleAlgorithm = inAlgorithm;
        scheduleAlgorithmQuickCode = 4;
    }
    // Round Robin: Code 5
    else if (inAlgorithm == "RR")
    {
        scheduleAlgorithm = inAlgorithm;
        scheduleAlgorithmQuickCode = 5;
    }
}
void SimulationLogic::togglePreempt(bool inPreempt)
{
    canPreempt = inPreempt;
}
void SimulationLogic::togglePriority(bool inPriority)
{
    hasPriority = inPriority;
}
void SimulationLogic::togglePromotion(bool inPromotion)
{
    canPromote = inPromotion;

    int queueLength = newQueue.size();
    // Turn on the promotion for each element
    for (int toggleIndex = 0; toggleIndex < queueLength; toggleIndex++)
    {
        newQueue[toggleIndex]->setPromotion(canPromote);
    }
}
void SimulationLogic::setScale(float inScale)
{
    scale = inScale;

    int queueLength = newQueue.size();
    for (int toggleIndex = 0; toggleIndex < queueLength; queueLength++)
    {
        newQueue[toggleIndex]->setScale(scale);
    }
}
void SimulationLogic::setScheduleAlgorithm(std::string inAlgorithm)
{
    // Check validity of input algorithm
    // First Come First Serve: Code 1
    if (inAlgorithm == "FCFS")
    {
        scheduleAlgorithm = inAlgorithm;
        scheduleAlgorithmQuickCode = 1;
    }
    // Shortest Job First: Code 2
    else if (inAlgorithm == "SJF")
    {
        scheduleAlgorithm = inAlgorithm;
        scheduleAlgorithmQuickCode = 2;
    }
    // Shortest Remaining Time: Code 3
    else if (inAlgorithm == "SRT")
    {
        scheduleAlgorithm = inAlgorithm;
        scheduleAlgorithmQuickCode = 3;
    }
    // Priority Based: Code 4
    else if (inAlgorithm == "PB")
    {
        scheduleAlgorithm = inAlgorithm;
        scheduleAlgorithmQuickCode = 4;
    }
    // Round Robin: Code 5
    else if (inAlgorithm == "RR")
    {
        scheduleAlgorithm = inAlgorithm;
        scheduleAlgorithmQuickCode = 5;
    }
}
int SimulationLogic::cycleLogic(HRESULT& hr,
    ID2D1HwndRenderTarget*& pRenderTarget)
{
    // Cycle as appropriate before moving.
    // Cycle ready queue
    for (int index = 0; index < readyQueue.size(); index++)
    {
        readyQueue[index]->cycleWait();
    }
    // Cycle waiting queue
    for (int index = 0; index < waitingQueue.size(); index++)
    {
        waitingQueue[index]->cycleWait();
    }

    // Pick next batch of IO/CPU
    int selectedCPU = 0;
    int selectedIO = 0;
    std::unordered_map<std::string, Process*> selectionCPUList;
    std::unordered_map<std::string, Process*> selectionIOList;

    // Go through and mark each as elgible/ineligible
    // Go until we fill up our selections, or until we run out of items

    //////////////////////////////////////////////////////////////////////////
    // CPU
    //////////////////////////////////////////////////////////////////////////
    while (selectedCPU < numCPU &&
        !(selectedCPU >= (processingQueue.size() + readyQueue.size())))
    {
        std::unordered_map<std::string, bool> eligibleCPUList;
        // Fill up selection and eligible lists
        // Mark everything as initially valid
        for (int index = 0; index < processingQueue.size(); index++)
        {
            eligibleCPUList.insert(
                { processingQueue[index]->getName(), true });
        }
        for (int index = 0; index < readyQueue.size(); index++)
        {
            eligibleCPUList.insert({ readyQueue[index]->getName(), true });
        }

        // Mark already reserved as invalid
        if (selectionCPUList.size() > 0)
        {
            for (int index = 0; index < processingQueue.size(); index++)
            {
                std::string holdString = processingQueue[index]->getName();

                // Prevent double placement
                if (selectionCPUList.count(holdString) > 0)
                {
                    eligibleCPUList.at(holdString) = false;
                }
            }
            for (int index = 0; index < readyQueue.size(); index++)
            {
                std::string holdString = readyQueue[index]->getName();

                // Prevent double placement
                if (selectionCPUList.count(holdString) > 0)
                {
                    eligibleCPUList.at(holdString) = false;
                }
            }
        }
        // Everything in CPU queue is guaranteed a spot if no preemption
        if (canPreempt == false)
        {
            for (int index = 0; index < numCPU && index < processingQueue.size(); index++)
            {
                std::string holdString = processingQueue[index]->getName();

                // Prevent double placement
                if (selectionCPUList.count(holdString) < 1)
                {
                    selectionCPUList.insert({ holdString, processingQueue[index] });

                    // Increment outer loop counter
                    selectedCPU++;
                }

                eligibleCPUList.at(holdString) = false;
            }
        }
        // Rule out all but highest priority if true
        // Rule out all but lowest round robin time if true
        if (hasPriority == true)
        {


            // Replace with lowest
            int highestPriority = 0;

            // Find highest in processing queue
            for (int index = 0; index < processingQueue.size(); index++)
            {
                std::string holdString = processingQueue[index]->getName();

                // Prevent double placement
                if (eligibleCPUList.at(holdString) == true && highestPriority
                    < processingQueue[index]->getPriority())
                {
                    highestPriority =
                        processingQueue[index]->getPriority();
                }
            }
            // Find highest in ready queue
            for (int index = 0; index < readyQueue.size(); index++)
            {
                std::string holdString = readyQueue[index]->getName();

                // Prevent double placement
                if (eligibleCPUList.at(holdString) == true && highestPriority
                    < readyQueue[index]->getPriority())
                {
                    highestPriority =
                        readyQueue[index]->getPriority();
                }
            }
            // Invalidate all but largest
            for (int index = 0; index < processingQueue.size(); index++)
            {
                std::string holdString = processingQueue[index]->getName();

                // Prevent double placement
                if (eligibleCPUList.at(holdString) == true && highestPriority
                    > processingQueue[index]->getPriority())
                {
                    eligibleCPUList.at(holdString) = false;
                }
            }
            // Invalidate all but largest
            for (int index = 0; index < readyQueue.size(); index++)
            {
                std::string holdString = readyQueue[index]->getName();

                // Prevent double placement
                if (eligibleCPUList.at(holdString) == true && highestPriority
                    > readyQueue[index]->getPriority())
                {
                    eligibleCPUList.at(holdString) = false;
                }
            }
        }

        // Rule out all but lowest round robin time if true
        if (isRoundRobin == true)
        {
            // Replace with lowest
            int lowestRoundTime = INT_MAX;

            // Find Lowest in processing queue
            for (int index = 0; index < processingQueue.size(); index++)
            {
                std::string holdString = processingQueue[index]->getName();

                // Prevent double placement
                if (eligibleCPUList.at(holdString) == true && lowestRoundTime
                    > processingQueue[index]->getRoundPenalty())
                {
                    lowestRoundTime =
                        processingQueue[index]->getRoundPenalty();
                }
            }
            // Find Lowest in ready queue
            for (int index = 0; index < readyQueue.size(); index++)
            {
                std::string holdString = readyQueue[index]->getName();

                // Prevent double placement
                if (eligibleCPUList.at(holdString) == true && lowestRoundTime
                    > readyQueue[index]->getRoundPenalty())
                {
                    lowestRoundTime =
                        readyQueue[index]->getRoundPenalty();
                }
            }
            // Invalidate all but shortest
            for (int index = 0; index < processingQueue.size(); index++)
            {
                std::string holdString = processingQueue[index]->getName();

                // Prevent double placement
                if (eligibleCPUList.at(holdString) == true && lowestRoundTime
                    < processingQueue[index]->getRoundPenalty())
                {
                    eligibleCPUList.at(holdString) = false;
                }
            }
            // Find Lowest
            for (int index = 0; index < readyQueue.size(); index++)
            {
                std::string holdString = readyQueue[index]->getName();

                // Prevent double placement
                if (eligibleCPUList.at(holdString) == true && lowestRoundTime
                    < readyQueue[index]->getRoundPenalty())
                {
                    eligibleCPUList.at(holdString) = false;
                }
            }
        }

        // We've already dealth with some of these, 
        // by ruling out items, so some of these are the same.
        switch (scheduleAlgorithmQuickCode)
        {
            // First Come First Served
            // Round Robin
            // Get earliest arrival time
        case 1:
        case 5:
        {
            // Replace with lowest
            int lowestArriveTime = INT_MAX;

            // Find Lowest in processing queue
            for (int index = 0; index < processingQueue.size(); index++)
            {
                std::string holdString = processingQueue[index]->getName();

                // Prevent double placement
                if (eligibleCPUList.at(holdString) == true && lowestArriveTime
                    > processingQueue[index]->getArrival())
                {
                    lowestArriveTime =
                        processingQueue[index]->getArrival();
                }
            }
            // Find Lowest in ready queue
            for (int index = 0; index < readyQueue.size(); index++)
            {
                std::string holdString = readyQueue[index]->getName();

                // Prevent double placement
                if (eligibleCPUList.at(holdString) == true && lowestArriveTime
                    > readyQueue[index]->getArrival())
                {
                    lowestArriveTime =
                        readyQueue[index]->getArrival();
                }
            }
            // Pick shortest CPU Queue first
            for (int index = 0; index < processingQueue.size() &&
                numCPU > selectedCPU; index++)
            {
                std::string holdString = processingQueue[index]->getName();

                // Add if eligible
                if (eligibleCPUList.at(holdString) == true &&
                    processingQueue[index]->getArrival() <= lowestArriveTime)
                {
                    selectionCPUList.insert({ holdString, processingQueue[index] });

                    // Increment outer loop counter
                    selectedCPU++;
                }

                eligibleCPUList.at(holdString) = false;
            }
            // Pick shortest
            for (int index = 0; index < readyQueue.size() && 
                numCPU > selectedCPU; index++)
            {
                std::string holdString = readyQueue[index]->getName();

                // Prevent extra placement
                if (eligibleCPUList.at(holdString) == true &&
                    readyQueue[index]->getArrival() <= lowestArriveTime)
                {
                    selectionCPUList.insert({ holdString, readyQueue[index] });

                    // Increment outer loop counter
                    selectedCPU++;
                }

                eligibleCPUList.at(holdString) = false;
            }
        }
        break;
        // Shortest Job First
        // Shortest Remaining Time
        // Priority Based
        case 2:
        case 3:
        case 4:
        {
            // Replace with lowest
            int lowestCPUBurst = INT_MAX;

            // Find Lowest in processing queue
            for (int index = 0; index < processingQueue.size(); index++)
            {
                std::string holdString = processingQueue[index]->getName();

                // Prevent double placement
                if ((eligibleCPUList.at(holdString) == true) && (lowestCPUBurst
                    > processingQueue[index]->getCurrentCPUBurst()))
                {
                    lowestCPUBurst =
                        processingQueue[index]->getCurrentCPUBurst();
                }
            }
            // Find Lowest in ready queue
            for (int index = 0; index < readyQueue.size(); index++)
            {
                std::string holdString = readyQueue[index]->getName();

                // Prevent double placement
                if ((eligibleCPUList.at(holdString) == true) && (lowestCPUBurst
                    > readyQueue[index]->getCurrentCPUBurst()))
                {
                    lowestCPUBurst =
                        readyQueue[index]->getCurrentCPUBurst();
                }
            }
            // Pick shortest CPU Queue first
            for (int index = 0; index < processingQueue.size() &&
                numCPU > selectedCPU; index++)
            {
                std::string holdString = processingQueue[index]->getName();

                // Add if eligible
                if (eligibleCPUList.at(holdString) == true &&
                    processingQueue[index]->getCurrentCPUBurst() <= lowestCPUBurst)
                {
                    selectionCPUList.insert({ holdString, processingQueue[index] });

                    // Increment outer loop counter
                    selectedCPU++;
                }

                eligibleCPUList.at(holdString) = false;
            }
            // Pick shortest
            for (int index = 0; index < readyQueue.size() && 
                numCPU > selectedCPU; index++)
            {
                std::string holdString = readyQueue[index]->getName();

                // Prevent extra placement
                if (eligibleCPUList.at(holdString) == true &&
                    readyQueue[index]->getCurrentCPUBurst() <= lowestCPUBurst)
                {
                    selectionCPUList.insert({ holdString, readyQueue[index] });

                    // Increment outer loop counter
                    selectedCPU++;
                }

                eligibleCPUList.at(holdString) = false;
            }
        }
        break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    // IO
    //////////////////////////////////////////////////////////////////////////
    while (selectedIO < numIO &&
        !(selectedIO >= (ioQueue.size() + waitingQueue.size())))
    {
        std::unordered_map<std::string, bool> eligibleIOList;
        // Fill up selection and eligible lists
        // Mark everything as initially valid
        for (int index = 0; index < ioQueue.size(); index++)
        {
            eligibleIOList.insert(
                { ioQueue[index]->getName(), true });
        }
        for (int index = 0; index < waitingQueue.size(); index++)
        {
            eligibleIOList.insert({ waitingQueue[index]->getName(), true });
        }
        // Mark already reserved as invalid
        if (selectionIOList.size() > 0)
        {
            for (int index = 0; index < ioQueue.size(); index++)
            {
                std::string holdString = ioQueue[index]->getName();


                // Prevent double placement
                if (selectionIOList.count(holdString) > 0)
                {
                    eligibleIOList.at(holdString) = false;
                }
            }
            for (int index = 0; index < waitingQueue.size(); index++)
            {
                std::string holdString = waitingQueue[index]->getName();

                // Prevent double placement
                if (selectionIOList.count(holdString) > 0)
                {
                    eligibleIOList.at(holdString) = false;
                }
            }
        }

        // Everything in IO queue is guaranteed a spot if no preemption
        // and IO has none
        for (int index = 0; index < ioQueue.size(); index++)
        {
            std::string holdString = ioQueue[index]->getName();

            // Prevent double placement
            if (selectionIOList.count(holdString) < 1)
            {
                selectionIOList.insert({ holdString, ioQueue[index] });

                // Increment outer loop counter
                selectedIO++;
            }

            eligibleIOList.at(holdString) = false;
        }


        // Rule out all but highest priority if true
        if (hasPriority == true)
        {
            // Replace with lowest
            int highestPriority = 0;

            // Find Lowest in processing queue
            for (int index = 0; index < ioQueue.size(); index++)
            {
                std::string holdString = ioQueue[index]->getName();

                // Prevent double placement
                if (eligibleIOList.at(holdString) == true && highestPriority
                    < ioQueue[index]->getPriority())
                {
                    highestPriority =
                        ioQueue[index]->getPriority();
                }
            }
            // Find Lowest in waiting queue
            for (int index = 0; index < waitingQueue.size(); index++)
            {
                std::string holdString = waitingQueue[index]->getName();

                // Prevent double placement
                if (eligibleIOList.at(holdString) == true && highestPriority
                    < waitingQueue[index]->getPriority())
                {
                    highestPriority =
                        waitingQueue[index]->getPriority();
                }
            }
            // Invalidate all but largest
            for (int index = 0; index < ioQueue.size(); index++)
            {
                std::string holdString = ioQueue[index]->getName();

                // Prevent double placement
                if (eligibleIOList.at(holdString) == true && highestPriority
                    > ioQueue[index]->getPriority())
                {
                    eligibleIOList.at(holdString) = false;
                }
            }
            // Invalidate all but largest
            for (int index = 0; index < waitingQueue.size(); index++)
            {
                std::string holdString = waitingQueue[index]->getName();

                // Prevent double placement
                if (eligibleIOList.at(holdString) == true && highestPriority
                    > waitingQueue[index]->getPriority())
                {
                    eligibleIOList.at(holdString) = false;
                }
            }
        }


        // Rule out all but lowest round robin time if true
        // IO has no preemption and therefore no round robin


        // We've already dealt with some of these, 
        // by ruling out items, so some of these are the same.
        switch (scheduleAlgorithmQuickCode)
        {
            // First Come First Served
            // Round Robin
            // Get earliest arrival time
        case 1:
        case 5:
        {

            // Replace with lowest
            int lowestArriveTime = INT_MAX;

            // Find Lowest in processing queue
            for (int index = 0; index < ioQueue.size(); index++)
            {
                std::string holdString = ioQueue[index]->getName();

                // Prevent double placement
                if (eligibleIOList.at(holdString) == true && lowestArriveTime
                    > ioQueue[index]->getArrival())
                {
                    lowestArriveTime =
                        ioQueue[index]->getArrival();
                }
            }
            // Find Lowest in waiting queue
            for (int index = 0; index < waitingQueue.size(); index++)
            {
                std::string holdString = waitingQueue[index]->getName();

                // Prevent double placement
                if (eligibleIOList.at(holdString) == true && lowestArriveTime
                    > waitingQueue[index]->getArrival())
                {
                    lowestArriveTime =
                        waitingQueue[index]->getArrival();
                }
            }
            // Pick shortest IO Queue first
            for (int index = 0; index < ioQueue.size() &&
                numIO > selectedIO; index++)
            {
                std::string holdString = ioQueue[index]->getName();

                // Add if eligible
                if (eligibleIOList.at(holdString) == true &&
                    ioQueue[index]->getArrival() <= lowestArriveTime)
                {
                    selectionIOList.insert({ holdString, ioQueue[index] });

                    // Increment outer loop counter
                    selectedIO++;
                }

                eligibleIOList.at(holdString) = false;
            }
            // Pick shortest
            for (int index = 0; index < waitingQueue.size() && 
                numIO > selectedIO; index++)
            {
                std::string holdString = waitingQueue[index]->getName();

                // Prevent extra placement
                if (eligibleIOList.at(holdString) == true &&
                    waitingQueue[index]->getArrival() == lowestArriveTime)
                {
                    selectionIOList.insert({ holdString, waitingQueue[index] });

                    // Increment outer loop counter
                    selectedIO++;
                }

                eligibleIOList.at(holdString) = false;
            }
        }
        break;
        // Shortest Job First
        // Lowest burst time
        case 2:
        case 3:
        case 4:
        {
            // Replace with lowest
            int lowestIOBurst = INT_MAX;

            // Find Lowest in IO queue
            for (int index = 0; index < ioQueue.size(); index++)
            {
                std::string holdString = ioQueue[index]->getName();

                // Prevent double placement
                if (eligibleIOList.at(holdString) == true && lowestIOBurst
                    > ioQueue[index]->getCurrentIOBurst())
                {
                    lowestIOBurst = ioQueue[index]->getCurrentIOBurst();
                }
            }
            // Find Lowest in waiting queue
            for (int index = 0; index < waitingQueue.size(); index++)
            {
                std::string holdString = waitingQueue[index]->getName();

                // Prevent double placement
                if (eligibleIOList.at(holdString) == true && lowestIOBurst
                    > waitingQueue[index]->getCurrentIOBurst())
                {
                    lowestIOBurst =
                        waitingQueue[index]->getCurrentIOBurst();
                }
            }

            // Pick shortest IO Queue first
            for (int index = 0; index < ioQueue.size() &&
                numIO > selectedIO; index++)
            {
                std::string holdString = ioQueue[index]->getName();

                // Add if eligible
                if (eligibleIOList.at(holdString) == true &&
                    ioQueue[index]->getCurrentIOBurst() <= lowestIOBurst)
                {
                    selectionIOList.insert({ holdString, ioQueue[index] });

                    // Increment outer loop counter
                    selectedIO++;
                }

                eligibleIOList.at(holdString) = false;
            }
            // Pick shortest
            for (int index = 0; index < waitingQueue.size() && 
                numIO > selectedIO; index++)
            {
                std::string holdString = waitingQueue[index]->getName();

                // Prevent extra placement
                if (eligibleIOList.at(holdString) == true &&
                    waitingQueue[index]->getCurrentIOBurst() <= lowestIOBurst)
                {
                    selectionIOList.insert({ holdString, waitingQueue[index] });

                    // Increment outer loop counter
                    selectedIO++;
                }

                eligibleIOList.at(holdString) = false;
            }
        }
        break;
        }
    }


    // Cycle CPU Queue
    for (int index = 0; index < processingQueue.size(); index++)
    {
        // Real quick update usage info on CPU for later
        processorUsage++;

        int returnCPU = processingQueue[index]->cycleCPU();
        // Check if we need to move it
        // Round Robin 
        if (returnCPU >= timeQuantum && isRoundRobin == true)
        {
            // Message
            std::string updateMessage = std::to_string(simTime);
            updateMessage += " P";
            updateMessage += processingQueue[index]->getName();
            updateMessage += " preempted to ready queue";
            messageLog.push_back(updateMessage);

            // Ensure process isn't moved back immediately
            std::string holdString = processingQueue[index]->getName();
            // Prevent double placement
            if (selectionCPUList.count(holdString) > 0)
            {
                selectionCPUList.at(holdString) = nullptr;
                selectionCPUList.erase(holdString);
            }

            // Apply round robin penalty
            processingQueue[index]->setWaitTime(simTime);

            // Preempt to ready queue
            readyQueue.push_back(processingQueue[index]);
            processingQueue[index] = nullptr;
            processingQueue.erase(processingQueue.begin() + index);

            // Set back iterator since we just moved everything forward
            index--;
        }
        // Finished burst, move to IO
        else if (returnCPU == -1)
        {
            std::string updateMessage = std::to_string(simTime);
            updateMessage += " P";
            updateMessage += processingQueue[index]->getName();
            updateMessage += " finished CPU burst and moved to IO wait queue";
            messageLog.push_back(updateMessage);

            // Ensure process isn't moved back immediately
            std::string holdString = processingQueue[index]->getName();
            // Prevent double placement
            if (selectionCPUList.count(holdString) > 0)
            {
                selectionCPUList.at(holdString) = nullptr;
                selectionCPUList.erase(holdString);
            }

            // Move to waiting queue
            waitingQueue.push_back(processingQueue[index]);
            processingQueue[index] = nullptr;
            processingQueue.erase(processingQueue.begin() + index);

            // Set back iterator since we just moved everything forward
            index--;
        }
        // Finished job, move to finished
        else if (returnCPU == -2)
        {
            // Report job results
            std::string updateMessage = std::to_string(simTime);
            updateMessage += " P";
            updateMessage += processingQueue[index]->getName();
            updateMessage += " completed:";
            messageLog.push_back(updateMessage);
            updateMessage = "TAT: ";
            updateMessage += std::to_string(
                processingQueue[index]->getTotalTime());
            updateMessage += ", Ready Time: ";
            updateMessage += std::to_string(
                processingQueue[index]->getReadyCycles());
            updateMessage += ", IO Wait Time: ";
            updateMessage += std::to_string(
                processingQueue[index]->getWaitingCycles());
            messageLog.push_back(updateMessage);

            // Ensure process isn't moved back immediately
            std::string holdString = processingQueue[index]->getName();
            // Prevent double placement
            if (selectionCPUList.count(holdString) > 0)
            {
                selectionCPUList.at(holdString) = nullptr;
                selectionCPUList.erase(holdString);
            }

            // Move to finished
            finishedQueue.push_back(processingQueue[index]);
            processingQueue[index] = nullptr;
            processingQueue.erase(processingQueue.begin() + index);

            // Set back iterator since we just moved everything forward
            index--;
        }
    }

    // Cycle IO
    int returnIO = 0;
    for (int index = 0; index < ioQueue.size(); index++)
    {
        returnIO = ioQueue[index]->cycleIO();
        // Check if we need to move it
        if (returnIO == -1)
        {
            std::string updateMessage = std::to_string(simTime);
            updateMessage += " P";
            updateMessage += ioQueue[index]->getName();
            updateMessage +=
                " finished IO burst and moved to CPU ready queue";
            messageLog.push_back(updateMessage);

            // Ensure process isn't moved back immediately
            std::string holdString = ioQueue[index]->getName();
            // Prevent double placement
            if (selectionIOList.count(holdString) > 0);
            {
                selectionIOList.at(holdString) = nullptr;
                selectionIOList.erase(holdString);
            }

            // Move to waiting queue
            readyQueue.push_back(ioQueue[index]);
            ioQueue[index] = nullptr;
            ioQueue.erase(ioQueue.begin() + index);

            // Set back iterator since we just moved everything forward
            index--;
        }
    }

    // Move items up from new when time
    for (int index = 0; index < newQueue.size(); index++)
    {
        if (simTime >= newQueue[index]->getArrival())
        {
            std::string updateMessage = std::to_string(simTime);
            updateMessage += " P";
            updateMessage += newQueue[index]->getName();
            updateMessage +=
                " moved to the new queue";
            messageLog.push_back(updateMessage);

            // Move pointers
            readyQueue.push_back(newQueue[index]);
            newQueue[index] = nullptr;
            newQueue.erase(newQueue.begin() + index);

            // Set back iterator since we just moved everything forward
            index--;
        }
    }

    // Handle non-Round Robin preemptions
    for (int index = 0; index < processingQueue.size(); index++)
    {
        std::string holdString = processingQueue[index]->getName();
        // Remove preempted items
        if (selectionCPUList.count(holdString) < 1)
        {
            std::string updateMessage = std::to_string(simTime);
            updateMessage += " P";
            updateMessage += processingQueue[index]->getName();
            updateMessage +=
                " preempted to ready queue";
            messageLog.push_back(updateMessage);

            // Move pointers
            readyQueue.push_back(processingQueue[index]);
            processingQueue[index] = nullptr;
            processingQueue.erase(processingQueue.begin() + index);

            // Set back iterator since we just moved everything forward
            index--;
        }
        else
        {
            selectionCPUList.erase(holdString);
        }

    }
    // Append items
    // Move chosen items to CPU processing queue finally
    for (int index = 0; index < readyQueue.size(); index++)
    {
        std::string holdString = readyQueue[index]->getName();
        if (selectionCPUList.count(holdString) > 0)
        {
            std::string updateMessage = std::to_string(simTime);
            updateMessage += " P";
            updateMessage += readyQueue[index]->getName();
            updateMessage +=
                " moved from ready to CPU processing queue";
            messageLog.push_back(updateMessage);

            // pointers
            processingQueue.push_back(readyQueue[index]);
            readyQueue[index] = nullptr;
            readyQueue.erase(readyQueue.begin() + index);

            // Set back iterator since we just moved everything forward
            index--;
        }
        else
        {
            selectionCPUList.erase(holdString);
        }
    }
    // Move chosen items to IO processing queue finally
    for (int index = 0; index < waitingQueue.size(); index++)
    {
        std::string holdString = waitingQueue[index]->getName();
        // Remove preempted items
        if (selectionIOList.count(holdString) > 0)
        {
            std::string updateMessage = std::to_string(simTime);
            updateMessage += " P";
            updateMessage += waitingQueue[index]->getName();
            updateMessage +=
                " moved from waiting to IO processing queue";
            messageLog.push_back(updateMessage);

            // Move pointers
            ioQueue.push_back(waitingQueue[index]);
            waitingQueue[index] = nullptr;
            waitingQueue.erase(waitingQueue.begin() + index);

            // Set back iterator since we just moved everything forward
            index--;
        }
    }


    // Increment sim time
    simTime++;

    // Display visual representation of turn end state
    drawFrame(hr, pRenderTarget);

    // Uncomment to print to log/vice-versa.
    // Draw and report frame results
    for (int index = messageLog.size() - 5; index < messageLog.size(); index++)
    {
        std::cout << messageLog[index] << std::endl;
    }
    

    // End simulation and signal to move to results screen if all queues
    // are finished.
    if ((newQueue.size() + processingQueue.size() + readyQueue.size()
        + ioQueue.size() + waitingQueue.size()) > 1)
    {
        return 1;
    }
    else
    {
        // Calculate and add results
        // Calculate and add CPU Utilization
        // Results are intentionally truncated for display
        int utilizationCPU = 0;
        if (simTime != 0)
        {
            // Results must account for the CPU's capacity, so it
            // further divides the results by the capacity rather than
            // just time like everything else
            utilizationCPU = 100 * float(processorUsage) / numCPU / simTime;
        }
        std::string updateMessage = std::to_string(simTime);
        updateMessage += " Simulation Finished - Results:";
        messageLog.push_back(updateMessage);
        updateMessage = std::to_string(utilizationCPU);
        updateMessage += "% average CPU Utilization.";
        messageLog.push_back(updateMessage);

        // Calculate and add average ready time
        // Results are intentionally truncated for display
        int averageTAT = 0;
        int sumTAT = 0;
        for (int index = 0; index < finishedQueue.size(); index++)
        {
            sumTAT += finishedQueue[index]->getTotalTime();
        }
        if (finishedQueue.size() > 0)
        {
            averageTAT = float(sumTAT) / finishedQueue.size();
        }
        updateMessage = "Average turn around time: ";
        updateMessage += std::to_string(averageTAT);
        updateMessage += " cycles.";
        messageLog.push_back(updateMessage);

        // Calculate and add average IO waiting time
        // Results are intentionally truncated for display
        int averageWait = 0;
        int sumWait = 0;
        for (int index = 0; index < finishedQueue.size(); index++)
        {
            sumWait += finishedQueue[index]->getWaitingCycles();
        }
        if (simTime > 0)
        {
            averageWait = float(sumWait) / finishedQueue.size();
        }
        updateMessage = "Average IO wait time: ";
        updateMessage += std::to_string(averageWait);
        updateMessage += " cycles.";
        messageLog.push_back(updateMessage);

        // Calculate and add average CPU ready time
        // Results are intentionally truncated for display
        int averageReady = 0;
        sumWait = 0;
        for (int index = 0; index < finishedQueue.size(); index++)
        {
            sumWait += finishedQueue[index]->getReadyCycles();
        }
        if (simTime > 0)
        {
            averageReady = float(sumWait) / finishedQueue.size();
        }
        updateMessage = "Average CPU wait time: ";
        updateMessage += std::to_string(averageReady);
        updateMessage += " cycles.";
        messageLog.push_back(updateMessage);

        // Print results to a file
        std::ofstream logFile;
        logFile.open("results.simlog");
        for (int index = 0; index < messageLog.size(); index++)
        {
            logFile << messageLog[index] << std::endl;
        }
        logFile.close();

        // Go to results
        return 2;
    }
}

int SimulationLogic::cycleResults(int inX, int inY, bool inClicked,
    HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget)
{
    // Draw title
    simLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);

    // Display messages
    int messageBase = messageLog.size() - 7;
    for (int index = messageBase; index < messageLog.size();
        index++)
    {
        messageLogBox.setText(messageLog[index]);
        messageLogBox.setCoords(0.0, 0.1 + (0.05 * (index - messageBase)),
            1.0, 0.18 + (0.05 * (index - messageBase)));
        messageLogBox.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    }

    // Exit button
    if (exitButton.cycleButton(inX, inY, inClicked, hr, pRenderTarget))
    {
        // Return here next frame
        return 3;
    }
    else
    {
        // Exit simulation and restart
        return 2;
    }
}
void SimulationLogic::readInfile(std::string inFileName)
{
    std::ifstream infile;
    infile.open(inFileName);

    // Error guard
    if (infile.is_open())
    {
        while (infile)
        {
            std::string tempString = "";
            std::getline(infile, tempString);

            // Try to not have errors from empty lines
            if (tempString.length() > 0)
            {
                // Overwrite if proven wrong
                bool badLine = true;

                // Check line isn't just whitespace
                for (int charIndex = 0; charIndex < tempString.size()
                    ; charIndex++)
                {
                    if (!std::isspace(tempString[charIndex]))
                    {
                        badLine = false;
                    }
                }



                // Verified something is in line
                if (badLine == false)
                {
                    std::stringstream inLineStream(tempString);
                    std::vector<std::string> lineArguments;
                    // Extract string into explicit arguments
                    while (inLineStream)
                    {
                        inLineStream >> tempString;
                        lineArguments.push_back(tempString);
                    }
                    // Make sure we have the right size and
                    // order of argumetns (Must end on CPU burst)
                    // Because of format, length of line will always be even
                    if (lineArguments.size() > 3)
                    {
                        bool badEntry = false;
                        // Ensure proper format of entries
                        if (!std::isdigit(lineArguments[0][0]))
                        {
                            badEntry = true;
                        }
                        if (!std::isdigit(lineArguments[2][1]))
                        {
                            badEntry = true;
                        }
                        else
                        {
                            // Remove "P"
                            lineArguments[2].erase(lineArguments[2].begin());
                        }
                        if (!std::isdigit(lineArguments[3][0]))
                        {
                            badEntry = true;
                        }
                        int queueSize = 0;
                        if (badEntry == false)
                        {
                            // Add new object to new queue
                            Process* passProcess = new Process(hWnd);
                            newQueue.push_back(passProcess);
                            passProcess = nullptr;

                            queueSize = newQueue.size();

                            // Set Scale
                            newQueue[queueSize - 1]->setScale(
                                scale);
                            // Set Arrival Time
                            newQueue[queueSize - 1]->setArrival(
                                stoi(lineArguments[0]));
                            // Set Name
                            newQueue[queueSize - 1]->setName(
                                (lineArguments[1]));
                            // Set Priority
                            newQueue[queueSize - 1]->setPriority(
                                stoi(lineArguments[2]));
                            // Set Promotion
                            newQueue[queueSize - 1]->setPromotion(
                                canPromote);

                            int lineSize = lineArguments.size();

                            // Add remaining arguments to burst lists
                            // Force ending on CPU Burst for improper format
                            for (int argumentIndex = 3
                                ; argumentIndex < lineSize - (lineSize % 2)
                                ; argumentIndex += 2)
                            {
                                // Add CPU Burst
                                if (std::isdigit(
                                    lineArguments[argumentIndex][0]))
                                {
                                    newQueue[queueSize - 1]->addCPUBurst(
                                        stoi(lineArguments[argumentIndex]));
                                }
                                // Input file error, attempt to 
                                // skip to next line
                                else
                                {
                                    argumentIndex = queueSize;
                                }
                                // Make sure it wasn't the last argument
                                if ((lineSize - (lineSize % 2)) - argumentIndex > 1)
                                {
                                    // Add IO Burst
                                    if (std::isdigit(
                                        lineArguments[argumentIndex][0]))
                                    {
                                        newQueue[queueSize - 1]->addIOBurst(
                                            stoi(lineArguments[argumentIndex + 1]));
                                    }
                                    // Input file error, attempt to 
                                    // skip to next line
                                    else
                                    {
                                        argumentIndex = queueSize;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }


        // Done with file
        infile.close();
    }
    else
    {
        std::cout << "Error, no infile found." << std::endl;
    }
}

void SimulationLogic::drawFrame(HRESULT& hr,
    ID2D1HwndRenderTarget*& pRenderTarget)
{
    // Draw CPU Processing Queue
    float gapSize = (0.21 - 0.1) / (processingQueue.size());
    for (int index = 0; index < processingQueue.size(); index++)
    {
        processingQueue[index]->drawBar(0.1 + (index * gapSize), 0.6,
            0.1 + ((index + 1) * gapSize), 0.9, hr, pRenderTarget);
    }

    // Draw IO Processing Queue
    gapSize = (0.36 - 0.24) / (ioQueue.size());
    for (int index = 0; index < ioQueue.size(); index++)
    {
        ioQueue[index]->drawBar(0.24 + (index * gapSize), 0.6,
            0.24 + ((index + 1) * gapSize), 0.9, hr, pRenderTarget);
    }

    // Draw CPU Ready Queue
    gapSize = (0.71 - 0.39) / (readyQueue.size());
    for (int index = 0; index < readyQueue.size(); index++)
    {
        readyQueue[index]->drawBar(0.39 + (index * gapSize), 0.6,
            0.39 + ((index + 1) * gapSize), 0.9, hr, pRenderTarget);
    }

    // Draw IO Waiting Queue
    gapSize = (0.9 - 0.74) / (waitingQueue.size());
    for (int index = 0; index < waitingQueue.size(); index++)
    {
        waitingQueue[index]->drawBar(0.74 + (index * gapSize), 0.6,
            0.74 + ((index + 1) * gapSize), 0.9, hr, pRenderTarget);
    }


    // Draw Text and borders
    cpuLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    ioLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    readyLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    waitingLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    cpuBox.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    ioBox.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    readyBox.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    waitingBox.cycleRectangle(0.1, 0.1, 0.1, hr, pRenderTarget);
    simLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);

    // Display messages
    int messageBase = messageLog.size() - 7;
    for (int index = messageBase; index < messageLog.size();
        index++)
    {
        messageLogBox.setText(messageLog[index]);
        messageLogBox.setCoords(0.0, 0.1 + (0.05 * (index - messageBase)),
            1.0, 0.18 + (0.05 * (index - messageBase)));
        messageLogBox.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
    }
}

SimulationLogic::~SimulationLogic()
{
    int queueSize = 0;

    // Deallocate memory if necessary
    // New Queue
    queueSize = newQueue.size();
    if (queueSize > 0)
    {
        // Deallocate queues
        for (int queueIndex = queueSize; queueIndex <= 0; queueIndex--)
        {
            delete newQueue[queueIndex];
        }
    }

    // Ready Queue
    queueSize = readyQueue.size();
    if (queueSize > 0)
    {
        // Deallocate queues
        for (int queueIndex = queueSize; queueIndex <= 0; queueIndex--)
        {
            delete readyQueue[queueIndex];
        }
    }

    // Processing Queue
    queueSize = processingQueue.size();
    if (queueSize > 0)
    {
        // Deallocate queues
        for (int queueIndex = queueSize; queueIndex <= 0; queueIndex--)
        {
            delete processingQueue[queueIndex];
        }
    }

    // Finished Queue
    queueSize = finishedQueue.size();
    if (queueSize > 0)
    {
        // Deallocate queues
        for (int queueIndex = queueSize; queueIndex <= 0; queueIndex--)
        {
            delete finishedQueue[queueIndex];
        }
    }

    // Waiting Queue
    queueSize = waitingQueue.size();
    if (queueSize > 0)
    {
        // Deallocate queues
        for (int queueIndex = queueSize; queueIndex <= 0; queueIndex--)
        {
            delete waitingQueue[queueIndex];
        }
    }

    // IO Queue
    queueSize = ioQueue.size();
    if (queueSize > 0)
    {
        // Deallocate queues
        for (int queueIndex = queueSize; queueIndex <= 0; queueIndex--)
        {
            delete ioQueue[queueIndex];
        }
    }
}
