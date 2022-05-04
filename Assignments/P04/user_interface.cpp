///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P03/P04 Reader/Writer
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
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>

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
#include <random>
#include <time.h>


//////////////////////////////////////////////////////////////////////////////
//                             Normal Classes
//////////////////////////////////////////////////////////////////////////////



// SimulationProgram

void SimulationProgram::setParameters(int inWriters, int inReaders, int inQuantum,
    int inProcesses)
{
    writers = inWriters;
    readers = inReaders;
    timeQuantum = inQuantum;
    processSlots = inProcesses;
}


void SimulationProgram::initializeSim(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //
    //Initialize UI objects
    //

    // Title Bar
    titleBox = TextBox(simulationWindow);
    titleBox.setCoords(0.0, 0.0, 1.0, 0.2);
    titleBox.setText("Program 3/4: Reader/Writer");

    // Logos
    projectLogo = ImageAsset(simulationWindow);
    projectLogo.setCoords(0.15, 0.70, 0.3, 0.95);
    projectLogo.setFile("Icon2.png");
    projectLogo2 = ImageAsset(simulationWindow);
    projectLogo2.setCoords(0.7, 0.70, 0.85, 0.95);
    projectLogo2.setFile("Icon3.png");


    // Start/Quit Buttons
    testButton = Button(simulationWindow);
    testButton.setCoords(0.53, 0.78, 0.61, 0.86);
    testButton.setText("Start Simulation");
    quitButton = Button(simulationWindow);
    quitButton.setCoords(0.39, 0.78, 0.47, 0.86);
    quitButton.setText("Quit Simulation");
    // Schedule Algorithms
    lockPolicyButton = Dropdown(simulationWindow);
    lockPolicyButton.setCoords(0.31, 0.48, 0.39, 0.58);
    lockPolicyButton.appendItem("Complete Lock");
    lockPolicyButton.appendItem("Section Lock");
    lockPolicyButton.appendItem("Specific and Priority Lock");
    lockPolicyButton.appendItem("No Lock");

    // CPU Items
    writersLabel = TextBox(simulationWindow);
    writersLabel.setText("Writers #:");
    writersLabel.setCoords(0.385, 0.46, 0.505, 0.57);
    decreaseWritersButton = Button(simulationWindow);
    decreaseWritersButton.setText("<");
    decreaseWritersButton.setCoords(0.51, 0.49, 0.55, 0.53);
    increaseWritersButton = Button(simulationWindow);
    increaseWritersButton.setText(">");
    increaseWritersButton.setCoords(0.61, 0.49, 0.65, 0.53);
    enterWritersBox = TextEnterInt(simulationWindow);
    enterWritersBox.setText(std::to_string(writers));
    enterWritersBox.setCoords(0.56, 0.49, 0.60, 0.53);
    // Reader Items
    readersLabel = TextBox(simulationWindow);
    readersLabel.setText("Readers #:");
    readersLabel.setCoords(0.385, 0.495, 0.505, 0.60);
    decreaseReadersButton = Button(simulationWindow);
    decreaseReadersButton.setText("<");
    decreaseReadersButton.setCoords(0.51, 0.54, 0.55, 0.58);
    increaseReadersButton = Button(simulationWindow);
    increaseReadersButton.setText(">");
    increaseReadersButton.setCoords(0.61, 0.54, 0.65, 0.58);
    enterReadersBox = TextEnterInt(simulationWindow);
    enterReadersBox.setText(std::to_string(readers));
    enterReadersBox.setCoords(0.56, 0.54, 0.60, 0.58);
    // Random File
    randomFileButton = Button(simulationWindow);
    randomFileButton.setText("Generate Random File");
    randomFileButton.setCoords(0.45, 0.28, 0.55, 0.36);

    // Initialize sim logic
    project3 = new SimulationLogic(simulationWindow);

    //
    // End initializing UI Elements
    //
}
void SimulationProgram::cycleFrame(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    PAINTSTRUCT ps;
    PAINTSTRUCT screenPaintStruct;

    // Set up Direct 2D factory and other one time only things
    static ID2D1Factory* pFactory = NULL;
    if (FAILED(D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
    {
        // Fail drawing totally, exit.
        running = false;
    }
    static ID2D1HwndRenderTarget* pRenderTarget = NULL;
    static ID2D1HwndRenderTarget* screenRenderTarget = NULL;
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
            focus = project3->cycleLogic(hr, pRenderTarget);
        }
        // Main menu
        else if (focus == 0)
        {
            // Cycle UI objects
            // Title
            titleBox.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            // Preemption Elements

            // Logos
            projectLogo.cycleImage(hr, pRenderTarget);
            projectLogo2.cycleImage(hr, pRenderTarget);

            // Start/Quit Buttons
            if (testButton.cycleButton(mouseX, mouseY, clicked,
                hr, pRenderTarget) == true)
            {
                focus = 1;
                project3->setParameters(writers, readers, timeQuantum, processSlots);
                totalTime = 0;
                beginTime = std::chrono::steady_clock::now();
                project3->launchSim();
            }
            if (quitButton.cycleButton(mouseX, mouseY, clicked,
                hr, pRenderTarget) == true)
            {
                running = false;
            }

            // Lock policy selection
            int menuSelection = -1;
            menuSelection = lockPolicyButton.cycleBox(mouseX, mouseY,
                clicked, hr, pRenderTarget);
            if (menuSelection != -1)
            {
                // Lock policy setters
                switch (menuSelection)
                {
                    // Global locking of memory
                    case 0:
                        protectionStrength = 0;
                        break;
                    // Section locking of memory
                    case 1:
                        protectionStrength = 1;
                        break;
                    case 2:
                        protectionStrength = 2;
                        break;
                    default:
                        protectionStrength = 3;
                }
                project3->setProtectionStrength(protectionStrength);
            }

            // Random file generation option
            if (randomFileButton.cycleButton(mouseX, mouseY,
                clicked, hr, pRenderTarget) == true)
            {
                // Generate random file
                project3->genRandomFile();
            }

            // Reader/Writer number settings
            // Writer Buttons
            writersLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            if (decreaseWritersButton.cycleButton(mouseX, mouseY, clicked,
                hr, pRenderTarget))
            {
                // Decrement writers and update other UI elements
                if (writers > 0)
                {
                    writers--;
                }
                enterWritersBox.setText(std::to_string(writers));
                project3->setWriters(writers);
            }
            if (increaseWritersButton.cycleButton(mouseX, mouseY, clicked,
                hr, pRenderTarget))
            {
                // Increment writers and update other UI elements
                writers++;
                enterWritersBox.setText(std::to_string(writers));
                project3->setWriters(writers);
            }
            menuSelection = enterWritersBox.cycleBox(keyPressed,
                mouseX, mouseY, clicked, hr, pRenderTarget);
            if (menuSelection != writers)
            {
                // Update changed writer sections
                writers = menuSelection;
                project3->setWriters(writers);
            }

            // Reader Buttons
            readersLabel.cycleText(0.1, 0.1, 0.1, hr, pRenderTarget);
            if (decreaseReadersButton.cycleButton(mouseX, mouseY, clicked,
                hr, pRenderTarget))
            {
                // Decrement writers and update other UI elements
                if (readers > 0)
                {
                    readers--;
                }
                enterReadersBox.setText(std::to_string(readers));
                project3->setReaders(readers);
            }
            if (increaseReadersButton.cycleButton(mouseX, mouseY, clicked,
                hr, pRenderTarget))
            {
                // Increment writers and update other UI elements
                readers++;
                enterReadersBox.setText(std::to_string(readers));
                project3->setReaders(readers);
            }
            menuSelection = enterReadersBox.cycleBox(keyPressed,
                mouseX, mouseY, clicked, hr, pRenderTarget);
            if (menuSelection != readers)
            {
                // Update changed reader sections
                                // Update changed writer sections
                                // Update changed writer sections
                writers = menuSelection;
                project3->setReaders(readers);

            }
        }
        // Results screen
        else if(focus == 2)
        {
            // Calculate total time in milliseconds
            if (totalTime == 0)
            {
                endTime = std::chrono::steady_clock::now();
                totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
            }
            focus = project3->cycleResults(mouseX, mouseY, totalTime, 
                clicked, hr, pRenderTarget);
        }
        // Delete simulation object and create another to allow retrying
        else
        {
            delete project3;
            initializeSim(uMsg, wParam, lParam);
            focus = 0;
            totalTime = 0;
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
        // Check for a mouse cursor and adjust if the program is clicked
        mouseX = GET_X_LPARAM(lParam);
        mouseY = GET_Y_LPARAM(lParam);
        break;
    case WM_CREATE:
        programLongParam = lParam;

        // Create a new simulation and handle buttons
        initializeSim(uMsg, wParam, lParam);

        break;
    case WM_CLOSE:
        running = false;
        DestroyWindow(simulationWindow);
        break;
    case WM_DESTROY:
        running = false;
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        cycleFrame(uMsg, wParam, lParam);

        // Reset status of controls and check again
        keyPressed = 'a'; // Arbitrary noninput key
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
    wc.lpszClassName = L"Project 3: Reader Writer Part 1";

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
    if (!this->Create(L"Project 3/4: Reader/Writer", WS_OVERLAPPEDWINDOW))
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
        Sleep(1);
    }

    inOutThread.join();
}


SimulationProgram::~SimulationProgram()
{
    delete project3;
}

