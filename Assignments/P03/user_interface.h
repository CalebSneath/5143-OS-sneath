///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P03/P04 Reader/Writer
// Course:           CMPS 5143
// Date:             May 1, 2022
// Semester:         Spring 2022
// File Description: This file contains the header information for the 
//                   class containing objects inside a
//                   a user interface, as well as the program control and 
//                   event loops.
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "user_interface_primitives.h"
#include "simulation.h"
#include <Windows.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <d2d1.h>

// Sometimes uncommenting one of these fixes user's include issues
// for direct2D.
//#pragma comment(lib,"d2d1.lib")
//#pragma comment(lib, "d2d1")

//////////////////////////////////////////////////////////////////////////////
//                             Normal Classes
//////////////////////////////////////////////////////////////////////////////

// Class:   SimulationProgram
// Purpose: A class which contains UI objects, a main program window,
//          an event loop, and a few associated intermediate variables
//          for use with a SimulationLogic object. The user uses
//          the member objects of this class to configure a 
//          simulation in a GUI and launches it when ready.
// Usage:   Make an object of this class. Use its runSim function.
class SimulationProgram
{
    public:
        SimulationProgram() : simulationWindow(NULL) { };
        void runSim();
        ~SimulationProgram();
        void setParameters(int inWriters, int inReaders, int inQuantum, 
            int inProcesses);

        BOOL Create(
            PCWSTR lpWindowName,
            DWORD dwStyle,
            DWORD dwExStyle = 0,
            int x = CW_USEDEFAULT,
            int y = CW_USEDEFAULT,
            int nWidth = CW_USEDEFAULT,
            int nHeight = CW_USEDEFAULT,
            HWND hWndParent = 0,
            HMENU hMenu = 0
        );
        bool ProcessMessages();
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg,
            WPARAM wParam, LPARAM lParam);
    private:
        void initializeSim(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void cycleFrame(UINT uMsg, WPARAM wParam, LPARAM lParam);
        bool running = true;
        // Determines which stream is selected in the GUI
        int focus = 0;

        void handleInOut();

        // UI Window Variables
        WNDCLASS wc = { 0 };
        HWND simulationWindow;
        char keyPressed = 'a';
        bool keyDown = false;
        bool mouseDown = false;
        int mouseX;
        int mouseY;
        bool clicked;
        bool waitSync = false;
        LPARAM programLongParam;
        HDC hdc;

        // Timer Variables
        std::chrono::steady_clock::time_point beginTime;
        std::chrono::steady_clock::time_point endTime;
        long long totalTime = 0;

        // UI Objects
        // Title
        TextBox titleBox;
        ImageAsset projectLogo;
        ImageAsset projectLogo2;

        Button testButton;
        Button quitButton;
        Dropdown lockPolicyButton;
        TextBox writersLabel;
        TextBox readersLabel;
        Button randomFileButton;
        Button decreaseWritersButton;
        Button increaseWritersButton;
        Button decreaseReadersButton;
        Button increaseReadersButton;
        TextEnterInt enterWritersBox;
        TextEnterInt enterReadersBox;

        // Simulation initialization variables
        int readers = 25;
        int writers = 5;
        int timeQuantum = 10;
        int processSlots = 10;
        int protectionStrength = 0;

        // Simulation Logic Object
        SimulationLogic* project3;
};




