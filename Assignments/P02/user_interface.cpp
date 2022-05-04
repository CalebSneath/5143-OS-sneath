///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P02 CPU Scheduling - Simulation
// Course:           CMPS 5143
// Date:             May 1, 2022
// Semester:         Spring 2022
// File Description: This file contains the implementation details for the 
//                   class containing objects inside a
//                   a user interface, as well as the program control and 
//                   event loops.
// Credit:           Much of the code on this page is heavily based upon
//                   code from Microsoft's online documentation and tutorial
//                   code for Windows.h and Direct 2D found here:
//                      https://docs.microsoft.com/en-us/windows/win32/
//                      learnwin32/learn-to-program-for-windows
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


// SimulationProgram

void SimulationProgram::initializeSim()
{
    // Initialize sim logic
    project2 = new SimulationLogic(simulationWindow);
    project2->setParameters(simFileName, timeQuantum, scheduleAlgorithm,
        numCPU, numIO);
    project2->readInfile(simFileName);
    // Initialize some default states
    if (scheduleAlgorithm == "SJF")
    {

        noPreemptButton.toggled = true;
        noRRButton.toggled = true;
        project2->togglePreempt(false);
        project2->setRoundRobin(false);
        canPreempt = false;
    }
    else if (scheduleAlgorithm == "FCFS")
    {
        noPreemptButton.toggled = true;
        noRRButton.toggled = true;
        project2->togglePreempt(false);
        project2->setRoundRobin(false);
        canPreempt = false;
    }
    else if (scheduleAlgorithm == "SRT")
    {
        preemptButton.toggled = true;
        noRRButton.toggled = true;
        project2->setRoundRobin(false);
        project2->setRoundRobin(false);
        canPreempt = true;
    }
    else if (scheduleAlgorithm == "RR")
    {
        project2->togglePreempt(true);
        project2->setRoundRobin(true);
        rRButton.toggled = true;
        preemptButton.toggled = true;
        canPreempt = true;
    }
    if (scheduleAlgorithm == "PB")
    {
        preemptButton.toggled = true;
        promotionButton.toggled = true;
        priorityButton.toggled = true;
        noRRButton.toggled = true;
        canPreempt = true;
        hasPriority = true;
        canPromote = true;
        project2->togglePreempt(true);
        project2->togglePriority(true);
        project2->togglePromotion(true);
        project2->setRoundRobin(false);
    }
    else
    {
        hasPriority = false;
        canPromote = false;
        noPriorityButton.toggled = true;
        noPromotionButton.toggled = true;
        project2->togglePriority(false);
        project2->togglePromotion(false);
    }
}

inline void SimulationProgram::drawFrame(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;

    // Set up Direct 2D factory and other one time only things
    static ID2D1Factory* pFactory = NULL;
    if (FAILED(D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
    {
        // Fail drawing totally, exit.
        running = false;
    }
    static ID2D1HwndRenderTarget* pRenderTarget = NULL;
    ID2D1SolidColorBrush* pBrush = NULL;
    static const D2D1_COLOR_F color = D2D1::ColorF(0.9, 0.9, 0.95);

    // Get the render target running as well as the background brush
    HRESULT hr = S_OK;
    if (pRenderTarget == NULL)
    {
        RECT rc;
        GetClientRect(simulationWindow, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(simulationWindow, size),
            &pRenderTarget);

        if (SUCCEEDED(hr))
        {
            // Get background brush.
            hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
        }
    }
    hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);

    if (SUCCEEDED(hr))
    {

        //
        // All drawing for one frame should start here
        //

        HDC hdc;

        hdc = BeginPaint(simulationWindow, &ps);
        pRenderTarget->BeginDraw();

        pRenderTarget->Clear(color);

        // Determine which screen to run
        // If there were more than three, it'd be worth moving this
        // to a separate function and using a switch statement 
        // instead, but there's not.
        // Simulation
        // Simulation cycle
        if (focus == 1)
        {
            focus = project2->cycleLogic(hr, pRenderTarget);
        }
        // Main menu
        else if (focus == 0)
        {
            // Cycle UI objects
            // Title
            titleBox.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);

            // Logo
            projectLogo.cycleImage(hr, pRenderTarget);

            // Preemption Elements
            preemptLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            noPreemptLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            if (preemptButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                // Toggle mutual exclusion, activate logic
                noPreemptButton.toggled = false;
                canPreempt = true;
                project2->togglePreempt(true);
            }
            if (noPreemptButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                // You can't have round robin without preemption
                if (scheduleAlgorithm != "RR")
                {
                    canPreempt = false;
                    preemptButton.toggled = false;
                    project2->togglePreempt(false);
                    canRoundRobin = false;
                    project2->setRoundRobin(false);
                }
            }
            // Priority Elements
            priorityLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            noPriorityLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            if (priorityButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                noPriorityButton.toggled = false;
                hasPriority = true;
                project2->togglePriority(true);
            }
            if (noPriorityButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                // Priority is required for promotion
                promotionButton.toggled = false;
                noPromotionButton.toggled = true;
                priorityButton.toggled = false;
                hasPriority = false;
                canPromote = false;
                project2->togglePriority(false);
                project2->togglePromotion(false);
            }
            // Promotion Elements
            promotionLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            noPromotionLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            if (promotionButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                if (hasPriority == false)
                {
                    hasPriority = true;
                    priorityButton.toggled = true;
                    noPriorityButton.toggled = false;
                    project2->togglePriority(true);
                }
                canPromote = true;
                noPromotionButton.toggled = false;
                project2->togglePromotion(true);
            }
            if (noPromotionButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                canPromote = false;
                promotionButton.toggled = false;
                project2->togglePromotion(false);
            }
            // Round Robin
            rRLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            noRRLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            if (rRButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                // Preemption is mandatory
                canPreempt = true;
                canRoundRobin = true;
                rRButton.toggled = true;
                noRRButton.toggled = false;
                project2->togglePreempt(true);
                project2->setRoundRobin(true);
                preemptButton.toggled = true;
                noPreemptButton.toggled = false;
            }
            if (noRRButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                canRoundRobin = false;
                rRButton.toggled = false;
                project2->setRoundRobin(false);
            }
            // CPU Burst Items
            cpuLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            if (decreaseCPUButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                if (numCPU > 1)
                {
                    numCPU--;
                    enterCPUBox.setText(std::to_string(numCPU));
                    project2->setCPU(numCPU);
                }
            }
            if (increaseCPUButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                numCPU++;
                enterCPUBox.setText(std::to_string(numCPU));
                project2->setCPU(numCPU);
            }
            int tempVar = enterCPUBox.cycleBox(keyPressed, mouseX, 
                mouseY, clicked, hr, pRenderTarget);
            if (tempVar != numCPU)
            {
                // Must have at least one CPU/IO
                if (tempVar < 1)
                {
                    numCPU = 1;
                    enterCPUBox.setText("1");
                    project2->setCPU(1);
                }
                else
                {
                    numCPU = tempVar;
                    project2->setCPU(numCPU);
                }
            }
            // IO Burst Items
            ioLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            if (decreaseIOButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                if (numIO > 1)
                {
                    numIO--;
                    enterIOBox.setText(std::to_string(numIO));
                    project2->setIO(numIO);
                }
            }
            if (increaseIOButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                numIO++;
                enterIOBox.setText(std::to_string(numIO));
                project2->setIO(numIO);
            }
            tempVar = enterIOBox.cycleBox(keyPressed, mouseX, mouseY, 
                clicked, hr, pRenderTarget);
            if (tempVar != numIO)
            {
                if (tempVar < 1)
                {
                    numIO = 1;
                    enterIOBox.setText("1");
                    project2->setIO(1);
                }
                else
                {
                    numIO = tempVar;
                    project2->setIO(numIO);
                }
            }
            // Schedule Algorithm Selection Cycle
            tempVar = scheduleLists.cycleBox(mouseX, mouseY, clicked, 
                hr, pRenderTarget);
            if (tempVar != -1)
            {
                // Initialize some default states
                // First Come First Serve
                if (tempVar == 0)
                {
                    scheduleAlgorithm = "FCFS";
                    project2->setScheduleAlgorithm(scheduleAlgorithm);
                    noPreemptButton.toggled = true;
                    preemptButton.toggled = false;
                    project2->togglePreempt(false);
                    canPreempt = false;
                }
                else if (tempVar == 1)
                    // Shortest Job First
                {
                    scheduleAlgorithm = "SJF";
                    project2->setScheduleAlgorithm(scheduleAlgorithm);
                    noPreemptButton.toggled = true;
                    preemptButton.toggled = false;
                    project2->togglePreempt(false);
                    canPreempt = false;
                }
                // Shortest Remaining Time
                else if (tempVar == 2)
                {
                    scheduleAlgorithm = "SRT";
                    project2->setScheduleAlgorithm(scheduleAlgorithm);
                    preemptButton.toggled = true;
                    project2->togglePreempt(true);
                    canPreempt = true;
                }
                // Round Robin
                else if (tempVar == 4)
                {
                    scheduleAlgorithm = "RR";
                    project2->setScheduleAlgorithm(scheduleAlgorithm);
                    project2->togglePreempt(true);
                    preemptButton.toggled = true;
                    canPreempt = true;
                }
                // Priority Based
                if (tempVar == 3)
                {
                    scheduleAlgorithm = "PB";
                    project2->setScheduleAlgorithm(scheduleAlgorithm);
                    preemptButton.toggled = true;
                    promotionButton.toggled = true;
                    priorityButton.toggled = true;
                    canPreempt = true;
                    hasPriority = true;
                    canPromote = true;
                    project2->togglePreempt(true);
                    project2->togglePriority(true);
                    project2->togglePromotion(true);
                }
                else
                {
                    hasPriority = false;
                    canPromote = false;
                    noPriorityButton.toggled = true;
                    noPromotionButton.toggled = true;
                    priorityButton.toggled = false;
                    promotionButton.toggled = false;
                    project2->togglePriority(false);
                    project2->togglePromotion(false);
                }
            }
            // Start/Quit
            if (testButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                focus = 1;
            }
            if (quitButton.cycleButton(mouseX, mouseY, clicked, hr,
                pRenderTarget))
            {
                running = false;
            }
        }
        // Results screen
        else if(focus == 2)
        {
            focus = project2->cycleResults(mouseX, mouseY, clicked, hr, 
                pRenderTarget);
        }
        // Prep simulation for restart
        else
        {
        std::cout << "Over here." << std::endl;
            delete project2;
            project2 = new SimulationLogic(simulationWindow);
            initializeSim();
            focus = 0;
        }

        hr = pRenderTarget->EndDraw();

        // Make sure to resize objects
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            if (*&pRenderTarget)
            {
                (*&pRenderTarget)->Release();
                *&pRenderTarget = NULL;
            }
        }
        EndPaint(simulationWindow, &ps);
        (*&pBrush)->Release();


        //
        // All drawing should end here
        //
    }
}

void SimulationProgram::setParameters(std::string inFileName, int inTime,
    std::string inAlgorithm, int inCPU, int inIO)
{
    // Check filename
    if (inFileName.length() > 0)
    {
        simFileName = inFileName;
    }

    // Bounds check time
    if (inTime > 0)
    {
        timeQuantum = inTime;
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
    }
    // Shortest Job First: Code 2
    else if (inAlgorithm == "SJF")
    {
        scheduleAlgorithm = inAlgorithm;
    }
    // Shortest Remaining Time: Code 3
    else if (inAlgorithm == "SRT")
    {
        scheduleAlgorithm = inAlgorithm;
    }
    // Priority Based: Code 4
    else if (inAlgorithm == "PB")
    {
        scheduleAlgorithm = inAlgorithm;
    }
    // Round Robin: Code 5
    else if (inAlgorithm == "RR")
    {
        scheduleAlgorithm = inAlgorithm;
    }
}

LRESULT CALLBACK SimulationProgram::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SimulationProgram* pThis = NULL;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (SimulationProgram*)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->simulationWindow = hWnd;
    }
    else
    {
        pThis = (SimulationProgram*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }
    if (pThis)
    {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

void SimulationProgram::handleInOut()
{
    while (running == true)
    {
        // Wait until the next cycle of processing.
        if (waitSync == false)
        {
            waitSync = true;

            // 0x8000 and 0x10000 are the values for 32 and 64 bit machines
            // for the left mouse being clicked. Does a bitwise
            // and and checks if its not equal to all zeros. Just
            // checking if the bitwise of 8000 is not all zeroes is probably
            // fastest and less architecture dependent.
            // The same is true for all others.
            // There are better ways to normally get keyboard input than this,
            // but this is fine.
            if ((GetKeyState(VK_LBUTTON) & 0x8000) > 0)
            {
                // We don't want the keys/mouse UI options
                // to be spammed with events from being
                // held down, so we move between multiple
                // states to ensure it only gets a message once.
                if (mouseDown == false)
                {
                    mouseDown = true;
                    clicked = true;
                }
                else
                {
                    clicked = false;
                }
            }
            else
            {
                clicked = false;
                mouseDown = false;
            }
            // Enter/Return
            if ((GetKeyState(VK_RETURN) & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = 'e';
                    keyDown = true;
                }
            }
            // Backspace
            else if ((GetKeyState(VK_BACK) & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = 'b';
                    keyDown = true;
                }
            }
            // Numerics
            else if ((GetKeyState('0') & 0x8000) > 0)
            {

                if (keyDown == false)
                {
                    keyPressed = '0';
                    keyDown = true;
                }
            }
            else if ((GetKeyState('1') & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = '1';
                    keyDown = true;
                }
            }
            else if ((GetKeyState('2') & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = '2';
                    keyDown = true;
                }
            }
            else if ((GetKeyState('3') & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = '3';
                    keyDown = true;
                }
            }
            else if ((GetKeyState('4') & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = '4';
                    keyDown = true;
                }
            }
            else if ((GetKeyState('5') & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = '5';
                    keyDown = true;
                }
            }
            else if ((GetKeyState('6') & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = '6';
                    keyDown = true;
                }
            }
            else if ((GetKeyState('7') & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = '7';
                    keyDown = true;
                }
            }
            else if ((GetKeyState('8') & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = '8';
                    keyDown = true;
                }
            }
            else if ((GetKeyState('9') & 0x8000) > 0)
            {
                if (keyDown == false)
                {
                    keyPressed = '9';
                    keyDown = true;
                }
            }
            else
            {
                keyDown = false;
            }
        }
        Sleep(0.1);
    }
}

LRESULT SimulationProgram::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

        case WM_MOUSEMOVE:
        {
            // Check for a mouse cursor and adjust if the program is clicked
            mouseX = GET_X_LPARAM(lParam);
            mouseY = GET_Y_LPARAM(lParam);
        }
            break;
        case WM_CREATE:
        {
            programLongParam = lParam;

            //
            //Initialize UI objects
            //

            // Title Bar
            titleBox = TextBox(simulationWindow);
            titleBox.setCoords(0.0, 0.0, 1.0, 0.2);
            titleBox.setText("Program 2: CPU Scheduling Simulation");

            // Logo
            projectLogo = ImageAsset(simulationWindow);
            projectLogo.setCoords(0.15, 0.70, 0.3, 0.95);
            projectLogo.setFile("Icon.png");

            // Start/Quit Buttons
            testButton = Button(simulationWindow);
            testButton.setCoords(0.53, 0.78, 0.61, 0.86);
            testButton.setText("Start Simulation");
            quitButton = Button(simulationWindow);
            quitButton.setCoords(0.39, 0.78, 0.47, 0.86);
            quitButton.setText("Quit Simulation");
            // Preemption Elements
            preemptLabel = TextBox(simulationWindow);
            preemptLabel.setText("Preemption");
            preemptLabel.setCoords(0.01, 0.1, 0.51, 0.4);
            noPreemptLabel = TextBox(simulationWindow);
            noPreemptLabel.setText("No Preemption");
            noPreemptLabel.setCoords(0.021, 0.2, 0.521, 0.5);
            preemptButton = ToggleButton(simulationWindow);
            preemptButton.setCoords(0.17, 0.25, 0.025);
            noPreemptButton = ToggleButton(simulationWindow);
            noPreemptButton.setCoords(0.17, 0.35, 0.025);
            // Priority Elements
            priorityLabel = TextBox(simulationWindow);
            priorityLabel.setText("Priority");
            priorityLabel.setCoords(0.24, 0.1, 0.74, 0.4);
            noPriorityLabel = TextBox(simulationWindow);
            noPriorityLabel.setText("No Priority");
            noPriorityLabel.setCoords(0.255, 0.2, 0.75, 0.5);
            priorityButton = ToggleButton(simulationWindow);
            priorityButton.setCoords(0.42, 0.25, 0.03);
            noPriorityButton = ToggleButton(simulationWindow);
            noPriorityButton.setCoords(0.42, 0.35, 0.03);
            // Promotion Elements
            promotionLabel = TextBox(simulationWindow);
            promotionLabel.setText("Promotion");
            promotionLabel.setCoords(0.465, 0.1, 0.97, 0.4);
            noPromotionLabel = TextBox(simulationWindow);
            noPromotionLabel.setText("No Promotion");
            noPromotionLabel.setCoords(0.48, 0.2, 0.98, 0.5);
            promotionButton = ToggleButton(simulationWindow);
            promotionButton.setCoords(0.64, 0.25, 0.03);
            noPromotionButton = ToggleButton(simulationWindow);
            noPromotionButton.setCoords(0.64, 0.35, 0.03);
            // Schedule Algorithms
            scheduleLists = Dropdown(simulationWindow);
            scheduleLists.setCoords(0.24, 0.48, 0.34, 0.58);
            scheduleLists.appendItem("First Come First Serve");
            scheduleLists.appendItem("Shortest Job First");
            scheduleLists.appendItem("Shortest Remaining Time");
            scheduleLists.appendItem("Priority Based");
            scheduleLists.appendItem("Round Robin");
            // CPU Items
            cpuLabel = TextBox(simulationWindow);
            cpuLabel.setText("CPU Numbers:");
            cpuLabel.setCoords(0.335, 0.46, 0.455, 0.57);
            decreaseCPUButton = Button(simulationWindow);
            decreaseCPUButton.setText("<");
            decreaseCPUButton.setCoords(0.46, 0.49, 0.5, 0.53);
            increaseCPUButton = Button(simulationWindow);
            increaseCPUButton.setText(">");
            increaseCPUButton.setCoords(0.56, 0.49, 0.60, 0.53);
            enterCPUBox = TextEnterInt(simulationWindow);
            enterCPUBox.setText(std::to_string(numCPU));
            enterCPUBox.setCoords(0.51, 0.49, 0.55, 0.53);
            // IO Items
            ioLabel = TextBox(simulationWindow);
            ioLabel.setText("IO Numbers:");
            ioLabel.setCoords(0.335, 0.495, 0.455, 0.60);
            decreaseIOButton = Button(simulationWindow);
            decreaseIOButton.setText("<");
            decreaseIOButton.setCoords(0.46, 0.54, 0.50, 0.58);
            increaseIOButton = Button(simulationWindow);
            increaseIOButton.setText(">");
            increaseIOButton.setCoords(0.56, 0.54, 0.60, 0.58);
            enterIOBox = TextEnterInt(simulationWindow);
            enterIOBox.setText(std::to_string(numIO));
            enterIOBox.setCoords(0.51, 0.54, 0.55, 0.58);
            // Round Robin
            rRLabel = TextBox(simulationWindow);
            rRLabel.setText("Round Robin");
            rRLabel.setCoords(0.450, 0.43, 1.00, 0.54);
            noRRLabel = TextBox(simulationWindow);
            noRRLabel.setText("No Round Robin");
            noRRLabel.setCoords(0.47, 0.54, 1.0, 0.65);
            rRButton = ToggleButton(simulationWindow);
            rRButton.setCoords(0.64, 0.485, 0.03);
            noRRButton = ToggleButton(simulationWindow);
            noRRButton.setCoords(0.64, 0.585, 0.03);

            // Initialize Simulation Variables
            initializeSim();

            //
            // End initializing UI Elements
            //
        }
            break;
        case WM_CLOSE:
        {
            DestroyWindow(simulationWindow);
        }
            break;
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
            break;
        case WM_PAINT:
        {
            drawFrame(uMsg, wParam, lParam);
            // Reset status of controls and check again
            keyPressed = 'a'; // Arbitrary noninput key
        }
    }
    return DefWindowProc(simulationWindow, uMsg, wParam, lParam);
}

bool SimulationProgram::ProcessMessages()
{
    MSG msg = {};


    while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

BOOL SimulationProgram::Create(
    PCWSTR lpWindowName,
    DWORD dwStyle,
    DWORD dwExStyle,
    int x,
    int y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu
)
{

    wc.lpfnWndProc = SimulationProgram::WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"Project 2: CPU Scheduling";

    // Set application icon
    wc.hIcon = (HICON)LoadImage(
        // Loading from image file, not resource file
        NULL, L"Icon.ico", IMAGE_ICON,
        // Length and width
        256, 256,
        // Using a file, not a resource file, it could be an ico
        // derived from a transparent png, it should be the normal size
        // and it should close with the window without issue.
        LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED | LR_LOADTRANSPARENT
    );

    // Register the window class.
    // Note: class here refers to the windows.h
    // window class meaning, not the traditional C++
    // class meaning.
    RegisterClass(&wc);

    // See: https://docs.microsoft.com/en-us/windows/
    //      win32/api/winuser/nf-winuser-createwindowexa
    // for more information on these parameters
    simulationWindow = CreateWindowEx(
        dwExStyle,                          // Default exWindow style
        wc.lpszClassName,                   // Window class name
        lpWindowName,                       // Window name, using default
        dwStyle |
        WS_VISIBLE |
        NULL,                            // Windwo style, using default
        x, y,                               // Default screen coordinates            
        nWidth, nHeight,                    // Default window dimensions
        hWndParent, hMenu, GetModuleHandle(NULL), this
    );


    MoveWindow(simulationWindow,
        0, 0,                   // Top Left of screen
        1066,    //
        600,
        TRUE);

    DrawIcon(hdc, 10, 20, wc.hIcon);

    return (simulationWindow ? TRUE : FALSE);
}

void SimulationProgram::runSim()
{
    //Create a window if no errors
    if (!this->Create(L"Project 2: CPU Scheduling", WS_OVERLAPPEDWINDOW))
    {
        std::cout << "Error creating window" << std::endl;
        return;
    }
    else
    {
        // Second argument to this function seems to
        // mostly be antiquated these days,
        // SW_NORMAL just uses an OS default value.
        ShowWindow(simulationWindow, SW_NORMAL);
    }

    hdc = GetDC(simulationWindow);

    // Launch a thread to always listen for 
    // key/mouse
    std::thread inOutThread(&SimulationProgram::handleInOut, this);

    // Main event loop
    running = true;
    while (running)
    {
        // Handle application messages and process a frame
        if (!ProcessMessages())
        {
            running = false;
        }

        // Set the window to ready to draw for the next frame.
        RedrawWindow(simulationWindow, NULL, NULL, RDW_INTERNALPAINT);

        PostMessage(simulationWindow, WM_KEYDOWN, 0u, 0u);
        waitSync = false;
        // Cap framerate
        if (focus != 1)
        {
            Sleep(0.1);
        }
        else
        {
            Sleep(simulationDelay);

        }
    }

    inOutThread.join();
}


SimulationProgram::~SimulationProgram()
{
    delete project2;
}

