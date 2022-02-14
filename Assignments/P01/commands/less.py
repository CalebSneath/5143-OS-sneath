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
from time import sleep
import math

def printOutput(inPrint):
  padLine = " " * 80
  # Prints the inputted string.
  sys.stdout.write('\r' + padLine + "\033[A")
  sys.stdout.write('\r' + inPrint)
  sys.stdout.flush()

def printPage(inList, inPage):
  # Clear everything from the terminal
  clearLess()

  # Grab terminal sizes and adjust
  # what section of the list will be displayed
  # accordingly, taking into account
  # how pages have moved around.
  width, height = os.get_terminal_size()
  startLine = inPage * height
  endLine = startLine + height
  if startLine > len(inList):
    startLine = len(inList) - 1 - height
    endLine = len(inList) - 1
  elif endLine >= len(inList):
    endLine = len(inList)
  if startLine < 0:
    startLine = 0
  for index in range(startLine, endLine):
    printOutput(inList[index] + '\n')

def maxPage(inList):
  width, height = os.get_terminal_size()
  maximumPage = math.ceil(len(inList) / height)
  return maximumPage

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

# Gets the number of rows in the terminal.
# Then, it does as many line breaks to
# fill the screen with nothing. After,
# it uses "\033[A", the code to move the
# cursor up, to go all the way back
# to the top of the terminal
def clearLess():
  width, height = os.get_terminal_size()
  for index in range (height):
    printOutput('\n')
  for index in range (width):
    printOutput("\033[A")
  return ""

# Main program
def less(**kwargs):
  # Parse kwargs
  inputDirectory = kwargs.get('inDirectory', None)
  flags = kwargs.get('flags', None)
  if (flags is not None):
    if "--help" in flags: 
      helpFlag = "help"
    else:
      helpFlag = None
  else:
    helpFlag = None
  param = kwargs.get('parameters', None)

  # Allow for some polymorphism by letting lists
  # or strings be the inputs, as well as files
  # within both
  if param is not None:
    # 
    if not isinstance(param, str):
      if len(param) > 0:
        # Check if function received a list of strings.
        if isinstance(param[0], str):
          try:
            for index in range(len(param)):
              possibleFile = ""
              # Will help check if the directory simply
              # needs adjustment.
              if inputDirectory is not None:
                possibleFile = inputDirectory + "/" + param[index]
                possibleFile = possibleFile.replace("//", "/")

              # Checks the normal and adjusted path.
              # If neither is a valid file, assume its just 
              # a string of text
              if os.path.isfile(param[index]):
                try: 
                  inFile = open(param[index], "r")
                  param[index] = inFile.read()
                  inFile.close()
                except:
                  return "Error: Unable to open file to view."
              elif os.path.isfile(possibleFile):
                inFile = open(param[index], "r")
                param[index] = inFile.read()
                inFile.close()
            param = "\n".join(param)
            param = param.split("\n")
          except:
            return "Error: Parsing display inputs failed."
        else:
          return "Error: Invalid display input."
      else:
        return "Error: Invalid display input."
    # Checks if the string is a file or just text.
    else:
      # Will help check if the directory simply
      # needs adjustment.
      possibleFile = ""
      if inputDirectory is not None:
       possibleFile = inputDirectory + "/" + param
       possibleFile = possibleFile.replace("//", "/")

      if os.path.isfile(param):
        try: 
          inFile = open(param, "r")
          param = inFile.read()
          param = param.split('\n')
          inFile.close()
        except:
          return "Error: Unable to open file to view."
      elif os.path.isfile(possibleFile):
        inFile = open(param, "r")
        param = inFile.read()
        param = param.split('\n')
        inFile.close()
      else:
        param = param.split('\n')
  else:
    return "Error: No input to view."

  # Early exit if person inputted "less --help"
  if (helpFlag is not None):
    helpMessage = "Displays the file one page at a time.\n"
    helpMessage += "Accepts one or more files, \n"
    helpMessage += "or alternatively strings of text."
    helpMessage += "Ex:\n"
    helpMessage += "\"less test.txt\n"
    helpMessage += "Use arrow keys to switch pages\n"
    helpMessage += "Use Ctrl-c to exit. \n"
    return helpMessage

  # end parsing kwargs

  lessPage = 0

  # Create the get character class object, set the character
  # to display on the prompt line to '%'
  getch = Getch()

  exitFlag = False

  while exitFlag is False:
    # Read character without printing to console
    inCharacter = getch()

    # \x03 is the code for ctrl-c
    # Exits the reading.
    if inCharacter == '\x03':
      sleep(2)
      sys.stdout.write("\n")
      exitFlag = True
      return "Finished reading."
    # \x7f is the code for backspaces
    # Exits the reading.
    elif inCharacter == '\x7f':
      sleep(2)
      sys.stdout.write("\n")
      exitFlag = True
      return "Finished reading."

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
        if lessPage > 0:
          lessPage -= 1
        printPage(param, lessPage)
        sleep(0.5)         
      elif direction in 'B':
        # The down key serves to print
        # the next (if applicable) command
        if lessPage < maxPage(param):
          lessPage += 1
        printPage(param, lessPage)
        sleep(0.5)   
            
      # Print the updated command
      printPage(param, lessPage)

if __name__ == '__main__':
  # Test
  #print(mv(inDirectory = "", parameters = "commands/test2.py", \
  #outDirectory = "/home/runner/shell/commands/test.py", \
  #flags = []))
  sortList = ["hello", "python", "4"]
  #sortList.sort()
  print(sortList)
  testSort = "jwojfwojfw\nojowfjowf\n"
  testSort = testSort.split('\n')
  flags = []
  flags.append("--help")
  #print(testSort)
  #print(cat(parameters = sortList))
  #print(cat(parameters = testSort, flags = ["--help"]))
  #print(cat(parameters = testSort))
  #fileTestList = ["test.txt", "ojofwjw", "131oj"]
  #print(cat(parameters = fileTestList))
  fileTestList = ["test.txt", "test.txt", "test.txt"]
  print(less(flags = ["--help"],\
  parameters = "commands/stresstest.txt"))

  fileTestList = ["test.txt", "test.txt", "test.txt"]
  print(less(flags = [],\
  parameters = "commands/stresstest.txt"))
  fileTestList = ["test.txt", "test.txt", "test.txt"]
  print(less(flags = [],\
  parameters = "commands/test.txt"))