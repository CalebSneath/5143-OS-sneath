## P01 - Shell Project - Implementation of a basic shell.
## Caleb Sneath
#### February 14, 2022
### Description:

This folder displays the use of Python to implement a basic shell capable of some common shell commands. Although this was a group project, I decided to make this project without any other group members. The main file is "shell.py". This file contains the main program loop, handles preparser commands, creates threads, and adjusts the terminal output. The other main notable file is parseCommands.py. This receives a command from shell.py and parses it into individual tokens. After receiving the tokens, using information from the commands package (mainly commandsTable.py), it sorts the tokens into the proper type, all organized within subcommands. These tokens are then sent to the proper output to call the correct function. Otherwise, getch just assists the shell loop in grabbing inputs, and __init__.py is only used to help the program know to look for imports within the program folder. See the commands folder for information on the commands package. All commands were implemented as instructed unless listed in the notes section, including the "--help" flag.

### Files

|   #   | File            | Description                                        |
| :---: | --------------- | -------------------------------------------------- |
|   1   | [shell.py](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P01/shell.py)         | The main program that runs the shell.   |
|   2   | [parseCommands.py](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P01/parseCommands.py)         | Breaks commands into organized tokens before sending them to their proper function. |
|   3   | [\_\_init\_\_.py](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P01/__init__.py)         | A package management file which helps import all files.  |
|   4   | [getch.py](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P01/getch.py)         | A file containing code to grab user input.  |
|   5   | [commands](https://github.com/CalebSneath/5143-OS-sneath/blob/main/Assignments/P01/commands)         | A package containing almost all commands and some command management files.  |


### Commands and Special Operators

|   #   | Command         | Description                                        |
| :---: | --------------- | -------------------------------------------------- |
|   1   | ls  | List the current directory. Flags: -a, -l, -h   |
|   2   | mkdir | Makes a new directory.   |
|   3   | cd | Changes to the specified directory. Also supports '~', blank, and '..' as arguments. |
|   4   | pwd | Prints the current working directory.  |
|   5   | cp | Copies the selected file to the selected destination. |
|   6   | mv | Moves or renames the selected file to the target.  |
|   7   | rm | Removes the selected file. Supports wildcards for a specific directory as well as the -r flag. |
|   8   | rmdir | Removes an empty directory. |
|   9   | cat | Displays and if applicable concatenates one or more files. |
|  10   | less | Displays a file one page at a time. |
|  11   | head | Displays the start of a file. Flags: -n  |
|  12   | tail | Displays the end of a file. Flags: -n|
|  13   | grep | Searches for a keyword in a file. Flags: -l |
|  14   | wc | Counts a certain aspect of the input determined by flags. Flags: -l, -m, -w |
|  15   | > | Redirects output to a file. Overwrites file. |
|  16   | >> | Redirects output to a file. Appends to file. |
|  17   | '' | Counts whatever is inbetween as a single token. |
|  18   | sort | Sorts the input in ascending order. |
|  19   | who | Lists current user and user group. |
|  20   | history | Shows a history of all commands. Commands are persistent between sessions. |
|  21   | !x | With x as a numeric input, load the select commandfrom history, starting at 0. |
|  22   | clear | An extra command. Clears the terminal and resets cursor to the top left position. |
|  23   | when | Lists the current time. |
|  24   | & | Runs the command as a background process. |

### Notes and Known Issues
All commands work as instructed on the assignment or according to expectations, with the following exceptions:
- Preparse commands (mainly just cd and !x) don't support threading unlike every other process. This was mainly an intentional decision, as these two commands have minimal runtime that would not benefit from threading, would be more difficult to implement, and would have potentially disasterous effects introduced if threading was allowed. For example, if the program was for some reason unresponsive and the user entered the cd command, assumed it didn't work, then the command could cause considerable damage if the command finishes before the "rm -r" command was finsihed. !x, loading a command from history, is a similar story.
- !x and clear don't support the "--help" flag. I just forgot about it until now.
- A rare bug causes a select few commands to fail if the directory is changed from the home directory. Usually changing back to the home directory and adjusting the path fixes this.
- On Replit, resizing the terminal during runtime can sometimes cause unstable display behavior. It is unknown if this is a Replit specific issue.
- On Replit, the when command is about a day off due to incorrect date settings on Replit's virtual machines.
- Rarely, the terminal input line freezes up. Continuing on with a command or arrow almost always fixes this.

### Instructions

- Ensure Python 3 is installed and updated.
- Copy this folder.
- Run "Python3 'whichever directory you install it to'/shell.py"
- Type any command you wish to use, and add "--help" for an explanation of it.

### Replit Link
https://replit.com/@CalebSneath/shell#shell.py

### Credits
## Code Credit 
  Partially based upon code from Dr. Griffin:
 
 - https://github.com/rugbyprof/5143-Operating-Systems/blob/master/Resources/03-Shell_Starter/shell_loop.py
 - https://github.com/rugbyprof/5143-Operating-Systems/blob/master/Resources/03-Shell_Starter/cmd_pkg_use/cmd_pkg/getch.py

## Reference Material
 All other code is original, although the following sites were used to help study conceptual or syntax knowledge.
 - https://www.w3schools.com/python/ 
 - https://www.geeksforgeeks.org 
 - https://docs.python.org/3/
