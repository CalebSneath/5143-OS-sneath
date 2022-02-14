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
# File Information: This function handles head, which
# displays the first few lines of a file. By default,
# it will only display as many lines as there
# are rows on the terminal, but with the -n flag,
# it will display however many lines are specified.
##############################################################################

import os

def head(**kwargs):
  # Parse kwargs
  inputDirectory = kwargs.get('inDirectory', None)
  flags = kwargs.get('flags', None)
  if (flags is not None):
    if "-n" in flags:
      numberSpecified = True
    else:
      numberSpecified = None
    if "--help" in flags: 
      helpFlag = "help"
    else:
      helpFlag = None
  else:
    helpFlag = None
    numberSpecified = None

  # Early exit if person inputted "head --help"
  if (helpFlag is not None):
    helpMessage = "Displays the top of the file. "
    helpMessage += "Accepts one file. \n"
    helpMessage += "Tries to fit to the terminal, "
    helpMessage += "but can specify number of lines\n"
    helpMessage += "with -n\n"
    helpMessage += "Ex:\n"
    helpMessage += "head test.txt -n 20\n"
    return helpMessage

  param = kwargs.get('parameters', None)
  # Allow for some polymorphism by letting lists
  # or strings be the inputs, as well as files
  # within both
  if param is not None:
    # 
    if isinstance(param, str):
      tempList = []
      tempList.append(param)
      param = tempList
    #There should be at least two arguments
    # with the '-n' flag.
    if numberSpecified is not None:
      if len(param) > 1:
        numberSpecified = int(param[1])
      else:
        return "Error: Insufficient arguments."

    if len(param) > 0:
      # Check if function received a list of strings.
      if isinstance(param[0], str):
        try:
          possibleFile = ""
          # Will help check if the directory simply
          # needs adjustment.
          if inputDirectory is not None:
            possibleFile = inputDirectory + "/" + param[0]
            possibleFile = possibleFile.replace("///", "/")
            possibleFile = possibleFile.replace("//", "/")

          # Checks the normal and adjusted path.
          # If neither is a valid file, assume its just 
          # a string of text
          if os.path.isfile(param[0]):
            try: 
              inFile = open(param[0], "r")
              param[0] = inFile.read()
              inFile.close()
            except:
              return "Error: Unable to open file to view."
          elif os.path.isfile(possibleFile):
            try:
              inFile = open(possibleFile, "r")
              param[0] = inFile.read()
              inFile.close()
            except:
              return "Error: Unable to open file to view."
          else:
            return "Error, unable to open file to view."
        except:
          return "Error: Parsing display inputs failed."
      else:
        return "Error: Invalid display input."
  else:
    return "Error: Invalid display input."



  try:
    displayList = []
    param[0] = param[0].split("\n")

    # Iterate through the list,
    # adding each line up to the limit,
    # whether that is decided by the parameter
    # or by terminal size if not specified.
    # This temporary list will be recombined into
    # a string to get our final answer.
    if numberSpecified is not None:
      if numberSpecified <= len(param[0]):
        for index in range(0, numberSpecified):
          displayList.append(param[0][index])
      else:
        for index in range(0, len(param[0])):
          displayList.append(param[0][index])
    else:
      width, height = os.get_terminal_size()
      if height <= len(param[0]):
        for index in range(0, height):
          displayList.append(param[0][index])
      else:
        for index in range(0, len(param[0])):
          displayList.append(param[0][index])
    displayList = "\n".join(displayList)
        
    return displayList
  except:
    return "Error: Undisplayable input."

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
  print(testSort)
  print(head(parameters = ["test.txt"], \
  flags = ["--help", "-n"]))
  print(head(parameters = ["test.txt", "20"], \
  flags = ["-n"]))
  print(head(parameters = ["test.txt"], \
  flags = []))

  print(head(parameters = ["commands/stresstest.txt"], \
  flags = []))

  print(head(parameters = ["commands/stresstest.txt", 20], \
  flags = ["-n"]))