///////////////////////////////////////////////////////////////////////////////
// 
// Author:           Caleb Sneath
// Email:            ansengor@yahoo.com
// Label:            P03/P04 Reader/Writer
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
#include <thread>
#include <Windows.h>
#include <unordered_map>
#include <mutex>

//#include "basewin.h"
#include <d2d1.h>

// Sometimes uncommenting one of these fixes user's include issues
// for direct2D.
//#pragma comment(lib,"d2d1.lib")
//#pragma comment(lib, "d2d1")




//////////////////////////////////////////////////////////////////////////////
//                             Normal Classes
//////////////////////////////////////////////////////////////////////////////

// Class:   SimulationMemory
// Purpose: A class which can act mostly as an instance of shared memory
//          across all things which contain an object of this class,
//          due to several pulls of static variables that this class
//          contains as member variables.
// Usage:   Use the default constructor to intitialize. Have at least
//          one object call loadMemory, passing in the filename of
//          the file to which memory will be loaded from. Afterward,
//          all future objects of this class will have shared access
//          to that memory in the form of a map called sharedMemory.
//          sharedMemory is public and freely accessible, although
//          a map of mutexes which should contain identical keys
//          called registeredMemory exists for synchronizing access
//          access to that memory. registerMemory is a function
//          which can be used to prevent deadlock when obtaining
//          mutexes with shared memory. It uses another static
//          member mutex called registerLock to allow only one 
//          process at a time to grab up to two different items from 
//          the sharedMemory/registredMemory mutex pair. Afterward,
//          unRegisterMemory can be used to return the mutex locks.
//          registerMemory returns false if the function fails and
//          true if it succeeds. clearMemory will delete all dynamic
//          memory. updateOpLog passes a string to a shared vector
//          of strings for all memory objects, which is accessed
//          by mutex to avoid program crashes. returnPriority
//          increments the value at "R4" of sharedMemory.
//          dumpMemory prints memory to an output file.
//          Finally, registerPriority is used to determine
//          what protection level the mutexes in registerMemory
//          will use, which is not a shared attribute
//          across all SharedMemory objects and must be
//          set individually.
// File format:
//          Two different input formats are supported.
//          JSON with A, B, C, and D sections and then some pairs
//          of numbers and values is first. It is highly format
//          specific though, so an easier alternative exits.
//          Alternate Example:
//          A
//              100 valuehere
//              105 valuehere
//              etc.
//          B  
//              100 valuehere
//          ... and so on. Basically, JSON without the special
//          characters.
class SimulationMemory
{
    public:
        SimulationMemory(){};
        void loadMemory(std::string inFileName);
        void dumpMemory(std::string inFileName);
        bool registerPriority(int inID);
        bool registerMemory(bool hasPrivileged, std::string memory1,
            std::string memory2);
        void unRegisterMemory(std::string request1, std::string request2);
        void returnPriority();
        void setProtectionStrength(int inStrength);
        void clearMemory();
        void updateOpLog(std::string inString);
        static std::unordered_map<std::string, int> sharedMemory;
        static std::unordered_map<std::string, std::mutex*> registeredMemory;
        static std::vector <std::string> operationLog;
        static std::mutex registerLock;
        // 0 is global, 1 is local, 2 is specific, and 3 is off
        //static 
        int protectionLevel;
    protected:
        void stripJSON(std::string& inString);
};

// Class:   Process
// Purpose: A class which can handle almost all relevant logic for
//          a single "process" within the scope of a simulation,
//          including its operations, handling its instruction queue,
//          as well as its id and visual representation.
// Usage:   Use the nondefault constructor to intitialize, or use
//          setHWND after initialization. Use the various setters to
//          determine before launching what attributes are desired
//          for the Process, such as its ID and whether it is a reader,
//          as well as what ID it will have for the privileged instructions
//          waiting queue. Use fillInstruction to generate a bunch of 
//          instructions, including a privileged if it is a writer.
//          Finally, use runProcess to fire off a thread that 
//          can run the process until its instructions are all through.
//          The instruction will intially be inactive and will
//          frequently return to this state once it fulfills
//          a certain amount of instructions (or tries and fails to)
//          due to the round robin simulatin. It must be activated again
//          during its next turn with grantCPUSlot.
class Process
{
    public:
        Process();
        Process(HWND inHandle);
        void setHWND(HWND inHandle);
        void setQuantum(int inQuantum) { timeQuantum = inQuantum; }
        void setID(int inID) { idNumber = inID; }
        int getID() { return idNumber; }
        void runProcess();
        void executeProcess();
        void printProcess();
        void setReader(bool inReader) { reader = inReader; }
        bool getReader() { return reader; }
        void drawRepresentation(float inTopX, float inTopY, float inBottomX,
            float inBottomY, HRESULT& hr,
            ID2D1HwndRenderTarget*& pRenderTarget);
        void grantCPUSlot();
        int getRemaining() { return instructionQueue.size(); }
        bool getActive() { return hasSlot; }
        void setPriviliged(int inPrivileged) { privileged = inPrivileged; }
        void genInstruction();
        void fillInstructions();
        bool keySearch(std::string inKey,
            std::vector<std::string> &searchList);
        SimulationMemory sharedMemory;
        void setProtectionLevel(int inLevel);
        ~Process();
    protected:
        int protectionLevel = 0;
        HWND hWnd;
        bool hasPrivileged = false;
        std::thread* processThread = nullptr;
        void processLoop();
        int privileged = 0;
        std::vector <std::string> instructionQueue;
        std::vector <int> registrationQueue;
        std::unordered_map <std::string, int> processControlBlock;
        int idNumber = 0;
        int initialInstructions = 0;
        int timeQuantum = 10;
        int thisQuantumRan = 0;
        bool running = false;
        bool reader = true;
        bool hasSlot = false;
        bool loadInstruction(int index);
        bool moveInstruction(int index);
        bool addInstruction(int index);
        bool subtractInstruction(int index);
        bool multiplyInstruction(int index);
        bool divideInstruction(int index);
        bool setInstruction(int index);
        bool readInstruction(int index);
        bool writeInstruction(int index);

        // Visualizations
        RectangleShape shapeRepresentation;
        TextBox textRepresentation;
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
        void genRandomFile();
        void setProtectionStrength(int inStrength);
        void setParameters(int inWriters, int inReaders, int inQuantum,
            int inProcesses);
        void setHWND(HWND inHandle);
        void setReaders(int inReaders) { readers = inReaders; };
        void setWriters(int inWriters) { writers = inWriters; };
        // Return type determines which screen to move to, 1 stays on sim
        int cycleLogic(HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
        void drawFrame(HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
        int cycleResults(int inX, int inY, long inTime, bool inClicked, 
            HRESULT& hr, ID2D1HwndRenderTarget*& pRenderTarget);
        void launchSim();
        void roudRobinDispatcher();
    protected:
        bool endSafe = true;
        std::thread* dispatcherThread = nullptr;
        bool running = false;
        int protectionLevel = 0;
        int writers = 5;
        int readers = 25;
        int timeQuantum = 10;
        int numCPUs = 10;
        int robinPlacement = 0;
        std::vector<Process> processList;
        HWND hWnd;
        SimulationMemory initializerMemory;

        // Visualization Objects
        TextBox simLabel;
        TextBox cpuLabel;
        TextBox readerLabel;
        TextBox writerLabel;
        TextBox memoryLabel;
        RectangularBorder cpuBox;
        RectangularBorder readerBox;
        RectangularBorder writerBox;
        RectangularBorder memoryBox;

        RectangularBorder r1Box;
        RectangularBorder r2Box;
        RectangularBorder r3Box;
        RectangularBorder r4Box;
        TextBox r1Label;
        TextBox r2Label;
        TextBox r3Label;
        TextBox r4Label;
        TextBox operationLabel;

        TextBox memoryCellText;

        // Exit Sim Button
        Button exitButton;
};