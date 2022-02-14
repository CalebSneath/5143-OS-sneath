#!/usr/bin/env python3
##############################################################################
# Author: Caleb Sneath
# Assignment: P01 - Shell Project
# Date: February 12, 2022
# Python 3.8.12
# Description: A shell to practice and implement the various
# commandline commands of a basic shell.
# Usage information: Run "python3 shell.py" from the
# relevant directory. Input any commands along with
# relevant parameters and flags.
# File Information: This is the main file, and handles
# the display of characters, the collection of the command
# string, threading, and calling the command parser function.
# File Credit: Partially based upon code from:
# https://github.com/rugbyprof/5143-Operating-Systems/blob/master/
# Resources/03-Shell_Starter/shell_loop.py
##############################################################################


import os
import sys
import threading
from getch import *
from parseCommands import *
from time import sleep

# Clears the line, then over it writes the working
# directory (replacing the home directory with a ~
# if necessary), colors the directory blue, followed
# by the prompt character, along with the current command.
# The string buffer variable moves the cursor left and right
# along with user arrow presses.
def printCommand(inPrint):
  padLine = " " * 80
  tempDir = currDir
  tempDir = tempDir.replace(homeDir, "~/", 1)
  tempDir += "/"
  # The user can enter directories in a bunch of formats
  # (with or without /), so just adding directories
  # along with this function can mean up to two reduntant
  # "/" to get rid of.
  tempDir = tempDir.replace("///", '/')
  tempDir = tempDir.replace("//", '/')
  sys.stdout.write('\r' + padLine + "\033[A")
  sys.stdout.write('\r' + '\u001b[36m' + \
  tempDir + '\u001b[0m' + promptChar + inPrint + stringBuffer)
  sys.stdout.flush()

def printOutput(inPrint):
  padLine = " " * 80
  # Prints the inputted string.
  sys.stdout.write('\r' + padLine + "\033[A")
  sys.stdout.write('\r' + inPrint)
  sys.stdout.flush()

# Takes a file, loads it into a list and returns the list.
# If there is no file of the given name, it instead
# returns an empty list.
def fileToList(fileName):
  try:
    fileReader = open(fileName)
    historyLog = fileReader.read()
    fileReader.close()
    historyLog = historyLog.split("\n")
    return historyLog
  except:
    return []

# Checks for things better handled early than later.
def preParseCheck(inCommand, preParseDict):
  if len(inCommand) > 0:
    if inCommand[0] == "!":
      return "historyLoad"
    tempCommand = inCommand.split()[0]
    if tempCommand in preParseDict:
      return tempCommand
    else:
      return None
  else:
    return "empty"

# Handles cd, the change directory command.
# Accepts --help, nothing to return to the home directory,
# ~ to also return to the home directory, .. to go 
# up one directory, or just a path to change to.
def cd(inHistoryList, inCommand):
  commandList = inCommand.split()
  newDir = currDir
  if len(commandList) > 1:
    if "--help" in commandList:
      returnMessage = "Changes directory. Handles \n"
      returnMessage += "Accepts either a target destination,\n"
      returnMessage += ".. to go up one directory, or either \n"
      returnMessage += "no arguments or ~ to return to the \n"
      returnMessage += "home directory.\n"
      returnMessage += "Command Ex.:\ncd .."
      sys.stdout.write(returnMessage + '\n')
      return currDir
    elif commandList[1] == "..":
      foundSlash = False
      # Cut off the first slash if present
      # before proceeding to loop.
      if newDir[-1] == "/":
        newDir = newDir[:-1]
      # Cut off the last file by cutting everythin at
      # the end up to and including the last / to find path
      while foundSlash is False and len(newDir) > 0:
        if newDir[-1] == '/':
          foundSlash = True
        else:
          newDir = newDir[:-1]
      if len(newDir) == 0:
        sys.stdout.write("Error, can't find directory.")
        return currDir
      elif os.path.isdir(newDir):
        newDir += "/"
        newDir = newDir.replace("///", "/")
        newDir = newDir.replace("//", "/")
        return newDir
      else:
        sys.stdout.write("Error: Directory not set." + '\n')
        return currDir
    elif commandList[1] == "~":
      return homeDir
    else:
      if os.path.isdir(commandList[1]):
        tempDir = "/" + currDir + "/" + commandList[1] +"/"
        tempDir = tempDir.replace("///", "/")
        tempDir = tempDir.replace("//", "/")
        if os.path.isdir(tempDir):
          return tempDir
        else:
          commandList[1] = homeDir + "/" + commandList[1] + "/"
          commandList[1] = commandList[1].replace("///", "/")
          commandList[1] = commandList[1].replace("//", "/")
          return commandList[1]
      else:
        sys.stdout.write("Error: Invalid directory." + '\n')
        return currDir
  else:
    return homeDir

def historyLoad(inHistoryList, inCommand):
  if "--help" in inCommand:
    printCommand("Loads a command from history." + '\n')
    return ""
  try:
    # Remove all of the whitespace and the starting !
    newCommand = (inCommand.replace("!", "", 1))
    newCommand = (newCommand.replace(" ", ""))
    newCommand = inHistoryList[int(newCommand)]
    return newCommand
  except:
    return ("Syntax Error: Invalid command.")

# Gets the number of rows in the terminal.
# Then, it does as many line breaks to
# fill the screen with nothing. After,
# it uses "\033[A", the code to move the
# cursor up, to go all the way back
# to the top of the terminal
def clear(inHistoryList, inCommand):
  if ("--help" in inCommand):
    printCommand("Clears the terminal." + '\n')
    return ""

  width, height = os.get_terminal_size()
  for index in range (height):
    printOutput('\n')
  for index in range (width):
    printOutput("\033[A")
  return ""

def launchThread(inDirectory, inString, inHistory):
  printOutput(str(executeCommands(inDirectory, inString,\
  inHistory)) + "\n")

  # For illustration purposes to show off
  # the &.
  #sleep(10)

# Main program
if __name__ == '__main__':
  # Create the get character class object, set the character
  # to display on the prompt line to '%'
  getch = Getch()
  promptChar = " %: "

  # Handle history creation and initialization.
  historyList = fileToList("history.log")
  historyWriter = open("history.log", "a")
  historySelection = len(historyList)


  # Tracks where we are inserting the new character
  # if user uses arrow keys. We can't write directly
  # to "command" because then it would add the ansi
  # keys to the string.
  insertionPoint = 0
  stringBuffer = ""

  # Just initialize the initial current directory to 
  # this directory. [:-8] removes shell.py
  currDir = str(os.path.abspath("__name__"))
  while currDir[-1] != "/":
    currDir = currDir[:-1]
  homeDir = currDir

  # Command will start blank
  command = ""
  printCommand(command)  

  # A series of commands it is helpful to check 
  # for first, either because they could cause 
  # timing issues if threaded (think if you did
  # "rm -r" while cd was running in the background)
  # or they modify the shell's own internal state.
  preParserDict ={"cd" : cd, "historyLoad": historyLoad, \
  "clear" : clear}
  preParserDest = {"cd" : currDir, "historyLoad" : command, \
  "clear" : command}

  
  while True:
    # Read character without printing to console
    inCharacter = getch()

    # \x03 is the code for ctrl-c
    if inCharacter == '\x03' or command == 'exit':
      sys.stdout.write("\n")
      # This try block is separate from the raise,
      # need to add the pass except part for syntax.
      try:
        historyWriter.close()
      except:
        pass
      raise SystemExit("Exiting.")
    # \x7f is the code for backspaces
    elif inCharacter == '\x7f':
      if insertionPoint > 0:
        command = command[:insertionPoint - 1] \
        + command[insertionPoint:]
        insertionPoint -= 1
      printCommand(command)
      sleep(0.1)

    # Arrow key presses all have x1b in their code
    elif inCharacter in '\x1b':
      null = getch()                  # waste a character
      direction = getch()             # grab the direction
            
      # Hex values for arrows being pressed are consecutive,
      # starting at A, going to D.
      # It will be assumed only one per axis (up/down, left/right)
      # will be pressed at a time.
      # A -  Up, B - Down, C - Right, D - Left
      if direction in 'A': 
        # The up key serves to print
        # the previous (if applicable) command
        if historySelection > 0:
          historySelection -= 1
          command = historyList[historySelection]
        printCommand(command)
        insertionPoint = len(command)
        sleep(0.1)
        #command = command[:-1]          
      elif direction in 'B':
        # The down key serves to print
        # the next (if applicable) command
        if historySelection < (len(historyList) - 1):
          historySelection += 1
          command = historyList[historySelection]
        printCommand(command)
        insertionPoint = len(command)
        sleep(0.1)
        #command = command[:-1]
      if direction in 'C':
        # The right key serves to move
        # the cursor right on the prompt.
        #command += u"\u2192"
        if insertionPoint < len(command):
          stringBuffer += '\x1b[1;C'
          insertionPoint += 1
          printCommand(command)
        sleep(0.1)
      elif direction in 'D':
        # The left key serves to move
        # the cursor left on the prompt.
        #command += u"\u2190"
        if insertionPoint > 0:
          stringBuffer += '\x1b[1;D'
          insertionPoint -= 1
          printCommand(command)
        sleep(0.1)
            
      # Print the updated command
      printCommand(command)

    # \r is the carriage return character code, 
    # and will usually run the command
    # This section handles launching commands and resetting
    # the command line ones enter is pushed.
    elif inCharacter in '\r':
      # Print the final command and start a new line,
      # updating the history logs and list as well
      printCommand(command + '\n')
      historyList.append(command)
      historyWriter.write(command + '\n')
      historySelection = len(historyList)

      # Check for anything that modifies the internal
      # state of the shell or empty commands.
      preParseResult = preParseCheck(command, preParserDict)
      backGroundCommand = False
      # Execute normal commands.
      if preParseResult is None:
        if command[-1] == '&':
          commandThread = threading.Thread(target=launchThread,\
          args=(currDir,command,historyList,), daemon = True)
          commandThread.start()
          backGroundCommand = True
          command = "Executing background thread...."
          printCommand(command)

        else:
          commandThread = threading.Thread(target=launchThread,\
          args=(currDir,command,historyList,), daemon = True)
          commandThread.start()
          command = "Executing command...."
          printCommand(command)
          commandThread.join()
      # Execute shell state changing commands, or nothing.
      else:
        if preParseResult != "empty":
          if preParseResult == "historyLoad":
            command = historyLoad(historyList, command)
          elif preParseResult == "cd":
            currDir = cd(historyList, command)
          elif preParseResult == "clear":
            clear(historyList, command)
      if preParseResult != "historyLoad" and \
      backGroundCommand is False:
        command = "Executing command...." 
      printCommand(command)
      sleep(1)
      printCommand(command)   
      if preParseResult != "historyLoad":
        # reset command to nothing (since we just executed it)
        command = ""
      elif command == "Syntax Error: Invalid command.":
        printCommand(command + '\n')
        command = ""
        printCommand(command + '\n')
      # now print empty cmd prompt
      printCommand(command)   
      if preParseResult == "historyLoad":
        insertionPoint = len(command)
      else:
        insertionPoint = 0
      stringBuffer = ""
    # Just add the character to our line if its not special and print
    else:
      command = command[:insertionPoint] + inCharacter \
      + command[insertionPoint:]
      insertionPoint += 1
      printCommand(command)
