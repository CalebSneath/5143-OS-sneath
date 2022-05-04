///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P02 CPU Scheduling - Simulation
// Course:           CMPS 5143
// Date:             May 1, 2022
// Semester:         Spring 2022
// File Description: This file contains the header information for the 
//                   classes necessary for this specific simulation's logic.
//                   Many classes within this page may solve problems 
//                   somewhat specific to the reader/writer simulation.
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "user_interface_primitives.h"
#include "user_interface.h"
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

// Class:   Process
// Purpose: A class which can handle almost all relevant logic for
//          a single "process" within the scope of a simulation,
//          including its operations, decrementing or incrementing its
//          appropriate times, and accessing statistics on those same things.
//          Also handles its id and visual representation.
// Usage:   Use the nondefault constructor to intitialize, or use
//          setHWND after initialization. Use the various setters to
//          determine before launching what attributes are desired
//          for the Process, such as its name. 
//          Instructions must be added via the addX type. Separate CPU bursts
//          must equal or exceed total IO operations by at most one.
//          If IO has more operations than CPU, it causes errors.
//          Additionally, it can handle management and updating by itself
//          of the changing of states by an outside actor, but it 
//          will cause unexpected behavior if the process is not
//          moved around the states of the simulation in a valid manner.
//          Use the various cycleX to handle a single simulation cycle
//          with the program in the appropriate queue. The new and
//          finished states do not have any necessar actions,
//          and the cycleWait performs necessary actions for 
//          both the CPU ready as well as IO waiting queues, figuring
//          out the appropriate change from relevant previous information
//          with no needed extra context. drawBar will show its visual
//          representation. The inTopY coordinate is what would be the
//          "max" Y for a simulation process of the highest reasonable
//          CPU/IO burst time, and the process will scale its
//          actual size based upon its real burst size.
//          The various getters just get different direct or calculated
//          process statistics.
class Process
{
    public:
        Process();
        Process(HWND inHandle);
        void enablePromotion() { canPromote = true; }
        int cycleCPU();
        int cycleIO();
        void cycleWait();
        bool getIOStatus() { return waitingIO; }
        void drawBar(float inTopX, float inTopY, float inBottomX,
            float inBottomY, HRESULT& hr,
            ID2D1HwndRenderTarget*& pRenderTarget);

        // Generic Mutators
        void setHWND(HWND inHandle);
        void setName(std::string inName);
        void setScale(float inScale) { scale = inScale; }
        void setPriority(float inPriority) { priority = inPriority; }
        void setPromotion(int inPromotion) { canPromote = inPromotion; }
        void setArrival(int inArrival) { arrivalTime = inArrival; }
        void setWaitTime(int inWait) { waitTime = inWait; }
        void addCPUBurst(int inBurst) { unfinishedCPU.push_back(inBurst); }
        void addIOBurst(int inBurst) { unfinishedIO.push_back(inBurst); }

        // Generic Accessors
        std::string getName() { return processName; }
        int getArrival() { return arrivalTime; }
        float getPriority() { return priority + promotion; }
        int getTotalTime()
        {
            return processCycles + ioCycles + waitingCycles + readyCycles;
        }
        int getProcessCycles() { return processCycles; }
        int getIOCycles() { return ioCycles; }
        int getWaitingCycles() { return waitingCycles; }
        int getReadyCycles() { return readyCycles; }
        int getCurrentCycles() { return currentCPUCycles; }
        int getRoundPenalty() { return waitTime; }
        int getCurrentCPUBurst() { return unfinishedCPU[0]; }
        int getCurrentIOBurst() { return unfinishedIO[0]; }

    protected:
        char lastState = 'r';
        int arrivalTime = 0;
        int waitTime = 0;
        float priority = 1;
        float promotion = 0;
        bool canPromote = false;
        float scale = 50;
        bool waitingIO = false;
        std::string processName = "";
        HWND hWnd;
        RectangleShape barRepresentation;
        TextBox textRepresentation;
        int currentCPUCycles = 0;
        int processCycles = 0;
        int ioCycles = 0;
        int waitingCycles = 0;
        int readyCycles = 0;
        std::vector<int> unfinishedCPU;
        std::vector<int> unfinishedIO;
};

// Class:   SimulationLogic
// Purpose: A class which contains logic specific to the initialization
//          and management logic of this specific simulation separate from
//          its GUI elements.
// Usage:   Create a new object of this class. Pass appropriate parameters
//          via setter functions. Pres launchSim to launch the program.
//          The program will then launch and run itself until completion.
//          A graphical visualization of its progress after running can
//          be shown using the cycleLogic function. This will return
//          an int value that finally changes once the simulation 
//          finishes, notifying an outside process that the results
//          are ready. cycleResults is the results screen, 
//          and contains an exit button, as well as displaying
//          a long int parameter as the time, inputted as 
//          nanoseconds and converted to seconds.
class SimulationLogic
{
    public:
        SimulationLogic();
        SimulationLogic(HWND inHandle);
        ~SimulationLogic();

        // Generic Mutators
        void setParameters(std::string inFileName, int inQuantum,
            std::string inAlgorithm, int inCPU, int inIO);
        void togglePreempt(bool inPreempt);
        void togglePriority(bool inPriority);
        void togglePromotion(bool inPromotion);
        void setScale(float inScale);
        void setHWND(HWND inHandle);
        void setScheduleAlgorithm(std::string inAlgorithm);
        void setCPU(int inCPU) { numCPU = inCPU; }
        void setIO(int inIO) { numIO = inIO; }
        void setRoundRobin(bool inRound) { isRoundRobin = inRound; }

        // Return type determines which screen to move to, 1 stays on sim
        int cycleLogic(HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
        int cycleResults(int inX, int inY, bool inClicked,
            HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
        void drawFrame(HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
        void readInfile(std::string inFileName);
    protected:
        // Simulation parameters
        int numCPU = 1;
        int numIO = 1;
        bool canPreempt = false;
        bool canPromote = false;
        bool hasPriority = false;
        bool isRoundRobin = true;
        std::string simFileName = "datafile.dat";
        std::string scheduleAlgorithm = "FCFS";
        int scheduleAlgorithmQuickCode = 1;
        int simTime = 0;
        int processorUsage = 0;
        int timeQuantum = 10;
        HWND hWnd;
        // Controls how the number of items influence bar height.
        float scale = 50;

        // "Process State" Queues
        std::vector<Process*> newQueue;
        std::vector<Process*> readyQueue;
        std::vector<Process*> processingQueue;
        std::vector<Process*> finishedQueue;
        std::vector<Process*> waitingQueue;
        std::vector<Process*> ioQueue;

        // Message variables
        std::vector<std::string> messageLog;
        TextBox messageLogBox;

        // Visualization Objects
        TextBox simLabel;
        TextBox cpuLabel;
        TextBox ioLabel;
        TextBox readyLabel;
        TextBox waitingLabel;
        RectangularBorder cpuBox;
        RectangularBorder ioBox;
        RectangularBorder readyBox;
        RectangularBorder waitingBox;

        // Exit Sim Button
        Button exitButton;
};