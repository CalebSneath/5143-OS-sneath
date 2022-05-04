///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P02 CPU Scheduling - Simulation
// Course:           CMPS 5143
// Date:             May 1, 2022
// Semester:         Spring 2022
// File Description: This file contains the header information for the 
//                   class containing objects inside a
//                   a user interface, as well as the program control and 
//                   event loops.
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "user_interface_primitives.h"
#include "simulation.h"

#include <Windows.h>

//#include "basewin.h"
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
        void setParameters(std::string inFileName, int inTime,
            std::string inAlgorithm, int inCPU, int inIO);

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
        void initializeSim();
        void drawFrame(UINT uMsg, WPARAM wParam, LPARAM lParam);

        bool running = true;
        // Determines which stream is selected in the GUI
        int focus = 0;

        // Simulation Parameters
        int numCPU = 1;
        int numIO = 1;
        bool canRoundRobin = false;
        bool canPreempt = false;
        bool canPromote = false;
        bool hasPriority = false;
        std::string simFileName = "datafile.dat";
        std::string scheduleAlgorithm = "FCFS";
        // Controls bar height per burst time
        // Smaller is taller.
        float barScale = 50;
        // How long to wait between frames
        int timeQuantum = 1;
        float simulationDelay = 100;

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

        // UI Objects
        // Title
        TextBox titleBox;
        ImageAsset projectLogo;
        TextBox preemptLabel;
        TextBox noPreemptLabel;
        ToggleButton preemptButton;
        ToggleButton noPreemptButton;
        TextBox priorityLabel;
        TextBox noPriorityLabel;
        ToggleButton priorityButton;
        ToggleButton noPriorityButton;
        TextBox promotionLabel;
        TextBox noPromotionLabel;
        ToggleButton promotionButton;
        ToggleButton noPromotionButton;
        Button testButton;
        Button quitButton;
        Dropdown scheduleLists;
        TextBox cpuLabel;
        TextBox ioLabel;
        Button decreaseCPUButton;
        Button increaseCPUButton;
        Button decreaseIOButton;
        Button increaseIOButton;
        TextEnterInt enterCPUBox;
        TextEnterInt enterIOBox;
        TextBox rRLabel;
        TextBox noRRLabel;
        ToggleButton rRButton;
        ToggleButton noRRButton;

        // Simulation Logic Object
        SimulationLogic* project2;
};




