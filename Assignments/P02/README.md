## P02 - CPU Scheduling Simulation - Simulating basic scheduling algorithms.
### Caleb Sneath
#### May 4, 2022

### Description:

This folder displays the use of C++ and the Windows.h library to implement a basic simulation of several real CPU scheduling algorithms. Although this was a group project, I decided to make this project without any other group members. The main file is "Source.cpp". This file creates an object responsible for the simulation window and calls the function to open the simulation UI. It can also accept command line parameters. The user interface files contain the code associated with collecting data to manage the UI as well as the main program control and drawing loop. The user interface primitives file is a small library I made with several classes to crete several common UI elements such as buttons or text that is drawable to the screen. The buttons are objects, mostly within the UI class, that activate logic to interact with objects of the classes on the simulation page. To run the simulation, there are several control options. Several command line arguments are accepted, as well as a config file. Alternatively, simply running the program and using the GUI is an option. 

### Files

|   #   | File            | Description                                        |
| :---: | --------------- | -------------------------------------------------- |
|   1   | [Source.cpp](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/Source.cpp)         | The main function resides here.   |
|   2   | [user_interface.cpp](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/user_interface.cpp)         | Contains general GUI logic and program control loop. |
|   3   | [user_interface.h](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/user_interface.h)         | Contains header information for user_interface.cpp. |
|   4   | [user_interface_primitives.cpp](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/user_interface_primitives.cpp)         | Contains general GUI element classes drawable shapes. |
|   5   | [user_interface_primitives.h](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/user_interface_primitives.h)         | Contains header information for user_interface_primitives.cpp. |
|   6   | [simulation.cpp](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/simulation.cpp)         | Contains simulation specific logic. |
|   7   | [simulation.h](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/simulation.h)         | Header for simulation.cpp.  |
|   8   | [simulation.dat](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/simulation.dat)         | Simulation user configuration file.  |
|   9   | [Icon.ico](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/Icon.ico)         | The program thumbnail.  |
|   10  | [Icon.png](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/Icon.png)         | Program image asset.  |
|   11  | [results.simlog](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P02/results.simlog)         | The program's output log of the last simulation run.  |

### Notes and Observations
#### Round Robin
Round robin has two possible reasons the user may want to implement it. If the goal is to treat most processes equally and give each process a fairly low time quantum relevant to most burst times, it tends to perform quite poorly. On large datasets, round robin carried out like this can produce some interesting behavior. While throughput can naturally be pretty low on a big enough dataset as limited resources are spread out across a very large dataset, it introduces extremely large latency with round robin. The program must execute for a tremendous length of time even before the first instruction finishes as every process is moved around to get a turn. It also lowers throughput by wasting time moving processes around and completely wasting possible early IO time. This is compounded heavily if one round robin time slice isn't enough for any reasonable CPU burst length, although somewhat mitigated if it is. In the end, round robin executed in this manner actually is counter productive to its intended goal by introducing too much CPU and IO capacity wastage. 
On the other hand, if round robin is implemented with the goal of increasing overall IO usage, it can occasionally improve efficiency. For example, if an early single process had an unreasonably large CPU burst time and small future IO burst times, meanwhile most other processes later on had low CPU wait times, but high IO burst times, it makes sense to temporarily kick the early process off the CPU in order to begin the other processes' IO bursts as soon as possible and not waste it.

#### First Come First Served
This is a pretty standard scheduling algorithm. It's not especially efficient or inefficient since it doesn't bother trying to move things around or kick them off the CPU. This all does make it one of the simpler ones to implement, assuming the arrival times or order of instructions are being tracked. In the end though, it is also incapable of responding to anything important such as high priority instructions without modification, forcing important items to wait through long lines if the CPU ready queue gets too long.

#### Shortest Job First/ Shortest Remaining
These sheduling policies are decently effective and can even be somewhat simple to introduce. By finishing jobs quickly, they usually keep the IO queue fairly well utilized. Their main downsides are that they can keep things waiting for a while by ignoring long items that show up early. Additionally, like first come first served, they can ignore high priority items for a while, in this case important items with long burst times.

#### Priority
Priority simply focuses first on items in the queue with the highest priority. This lets it handle important new items that need immediate processing. Additionally, it can be mixed with the above schedules as tiebreakers for some interesting results. When priority is too varied, execution can almost be random, with all the associated pros and cons that brings. A low priority item can also have a very long wait. This can be a challenge to solve. A simple solution to this seems to be to elevate the priority of things the longer they wait. Unfortunately this can also defeat the point of priority in the first place, as it forces new items to have to mature in a crowded queue. Additionally, in a crowded queue the maturation becomes a moot point anyway as the high priority items and low priority items mature as well.

#### Preemption
Preemption is not a policy by itself, but rather a component of other policies that is common enough to warrant its own discussion. Preemption can lead to wasted CPU bursts, so it should be used sparingly. Ideally preemption is used only to improve the IO utilization or to deal with a very important and pressing process. Additionally, if evaluation criteria is constantly changing, such as by maturing the priorities of waiting processes, care must be taken to ensure that processes don't get stuck in a trap of being swapped back and forth from the CPU to ready queue.

#### Takeaway
At the end of the day, while there may in some cases be a policy which will finish all jobs in the shortest total time, due to the fact that processes have differing levels of importance warranting differing levels of immediate attention, there isn't always a best scheduling policy. Additionally, no scheduling policy can solve the CPU and IO simply having inadequate capacity for the incoming workload, so in some cases at least a few processes having a long wait time is simply inevitable. With that said, while it may be impossible to have a "perfect" scheduling policy, it is definitely easy to have a poor one. Any policy that moves items around too much without finishing a burst is likely counterproductive to any goals it may have in doing so, especially if this results in the CPU or IO being largely idle. 

### Instructions

- Download Visual Studio. Only VS 2019 is tested, but newer versions likely work as well.
- Create a new project and add all files as source files. Add the .ico file as a resource if you want.
- Modify any values in the .conf file if you want.
- Either press build, or just hit the local windows debugger.
- If you pressed build and grabbed your .exe, you can try command line parameters when you run it.
- Use any GUI options you want to configure the simulation.
- Hit run. The simulation will run until completion and display results.

## Credits
Some code was reused from one of my other projects in my software engineering class, the MTIL. I tried to ensure no code originally developed by my group members was used for this project.
### Reference Material
 Microsoft's official documentaion and tutorials for Windows.h and Direct 2D were heavily referenced at the start of the project.
 - https://docs.microsoft.com/en-us/windows/win32/learnwin32/learn-to-program-for-windows
