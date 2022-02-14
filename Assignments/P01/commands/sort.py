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
# File Information: This function handles sort, the sorting command.
# This function can take a number of types as the parameter, including
# a string of text, a string which is a filename, a list of text, 
# and a list of files. The function will sort in ascending order,
# with numbers coming before letters.
##############################################################################

import shutil
import os

def sort(**kwargs):
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
                  return "Error: Unable to open file to sort."
              elif os.path.isfile(possibleFile):
                inFile = open(param[index], "r")
                param[index] = inFile.read()
                inFile.close()
            param = "\n".join(param)
            param = param.split("\n")
          except:
            return "Error: Parsing sort inputs failed."
        else:
          return "Error: Invalid sort input."
      else:
        return "Error: Invalid sort input."
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
          return "Error: Unable to open file to sort."
      elif os.path.isfile(possibleFile):
        inFile = open(param, "r")
        param = inFile.read()
        param = param.split('\n')
        inFile.close()
      else:
        param = param.split('\n')
  else:
    return "Error: No input to sort."

  # Early exit if person inputted "sort --help"
  if (helpFlag is not None):
    helpMessage = "Sorts the input."
    helpMessage += "Accepts one or more files, \n"
    helpMessage += "or alternatively strings of text."
    helpMessage += "Ex:\n"
    helpMessage += "\"sort test.txt\n"
    helpMessage += "Sorts by line, and goes in ascending\n"
    helpMessage += "order, with numbers before letters.\n"
    return helpMessage

  try:
    param.sort()
    param = "\n".join(param)
    return param
  except:
    return "Error: Unsortable input."

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
  print(sort(parameters = sortList))
  print(sort(parameters = testSort, flags = ["--help"]))
  print(sort(parameters = testSort))
  fileTestList = ["test.txt", "ojofwjw", "131oj"]
  print(sort(parameters = fileTestList))