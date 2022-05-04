## P04 - Reader Writer Part 2 - Simulating Race Conditions
#### Caleb Sneath
#### May 4, 2022

<img src="Running Screenshot.png" width="1000">

### Description:
This folder displays the use of C++ and the Windows.h library to implement a basic simulation of several reader/writer processes utilizing shared memory. This simulation displays the concepts of multiprocessing, race conditions, shared resource usage, synchronization, and deadlock avoidance.

Although this was a group project, I decided to make this project without any other group members. The main file is "Source.cpp". This file creates an object responsible for the simulation window and calls the function to open the simulation UI. It can also accept command line parameters. The user interface files contain the code associated with collecting data to manage the UI as well as the main program control and drawing loop. The user interface primitives file is a small library I made with several classes to crete several common UI elements such as buttons or text that is drawable to the screen. The buttons are objects, mostly within the UI class, that activate logic to interact with objects of the classes on the simulation page. To run the simulation, there are several control options. Several command line arguments are accepted, as well as a config file. Alternatively, simply running the program and using the GUI is an option.  

### Files
|   #   | File            | Description                                        |
| :---: | --------------- | -------------------------------------------------- |
|   1   | [Source.cpp](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/Source.cpp)         | The main function resides here.   |
|   2   | [user_interface.cpp](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/user_interface.cpp)         | Contains general GUI logic and program control loop. |
|   3   | [user_interface.h](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/user_interface.h)         | Contains header information for user_interface.cpp. |
|   4   | [user_interface_primitives.cpp](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/user_interface_primitives.cpp)         | Contains general GUI element classes drawable shapes. |
|   5   | [user_interface_primitives.h](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/user_interface_primitives.h)         | Contains header information for user_interface_primitives.cpp. |
|   6   | [simulation.cpp](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/simulation.cpp)         | Contains simulation specific logic. |
|   7   | [simulation.h](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/simulation.h)         | Header for simulation.cpp.  |
|   8   | [memory.json](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/memory.dat)         | Simulation shared memory input/output file.  |
|   9   | [Icon.ico](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/Icon.ico)         | The program thumbnail.  |
|   10  | [Icon2.png](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/Icon2.png)         | Program image asset.  |
|   11  | [Icon3.png](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/Icon3.png)         | Program image asset.  |
|   12  | [Random Settings.conf](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/Random_Settings.conf)         | The program's random number seed config file.  |
|   13  | [simulation.conf](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P04/simulation.conf)         | The program's random number seed config file.  |
|   14  | [readers/readersX]   | A process file showing the remaining instructions for a reader process.  |
|   15  | [writers/writersX] | A process file showing the remaining instructions for a writer process.  |
|   16  | other .pngs | Various screenshots.  |


### Notes and Observations
Performance can be drastically improved in some cases by not bothering to lock down shared memory. Unfortunately, this is at the cost of the program executing in a seemingly nondeterministic fashion, with different results almost every time it is executed. Additionally, in some cases the lack of synchronization can cause actual program crashing issues. Fortunately, less restrictive lock policies can somewhat speed the program up, although a lot of overhead is just performing extra work to lock things down in the first place. Additionally, locking things down can lead to deadlock, but there are ways to avoid this. In this program, deadlock was avoided by allowing one person to register at a time, and letting them grab locks for everything they need at once for an instruction. In the end, it is important to remember that resources spent synchronizing multiple simultaneous processes do not come from nowhere, and it may be counterproductive and produce too much overhead to have signalling be too complex when coordinating different threads/processes with shared resources. In these cases a somewhat more simple but restrictive lock policy may make sense even from a performance perspective.

Another important thing to note is that even if you lock a program down completely, it can still execute differently if it begins in a different state. This was not always controlled for in this simulation, meaning that even properly locking resources down leads to some occasional variance in outcomes.

<img src="Menu Screenshot.png" width="1000">

### Instructions
- Create a new empty C++ program in Visual Studios.
- Copy files into Source Files in Visual Studios. (.h and .cpp files)
- Copy .ico and .png files to Resource Files.
- Create subdirectories in the main directory of the Visual Studios project called "readers" and "writers".
- Optionally modify the .conf files with the desired settings.
- Change the Random Settings seed to -1 to use a random seed, or anything else to instead use that as the seed.
- Hit local windows debugger to run as debug or build and press F5
- Optionally relocated the executable and resources into the desired final directoy. This should allow the use of command line arguments when launching the program.
- Command line arguments follow the same format and order as the right column of the "simulation.conf" file.
- Configure any final details in the GUI before launching the simulation.

## Credits
Some code was reused from one of my other projects in my software engineering class, the MTIL. I tried to ensure no code originally developed by my group members was used for this project.
### Reference Material
 Microsoft's official documentaion and tutorials for Windows.h and Direct 2D were heavily referenced at the start of the project.
 - https://docs.microsoft.com/en-us/windows/win32/learnwin32/learn-to-program-for-windows
