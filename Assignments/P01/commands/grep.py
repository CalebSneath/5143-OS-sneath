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
# File Information: This function handles grep, a searching 
# function. Either returns whole lines, or just filenames.
##############################################################################

import shutil
import os

def grep(**kwargs):
  targetString = ""
  fileList = []
  foundList = []

  # Parse kwargs
  inputDirectory = kwargs.get('inDirectory', None)
  flags = kwargs.get('flags', None)
  if (flags is not None):
    if "-l" in flags:
      fileReturnFlag = True
    else:
      fileReturnFlag = None
    if "--help" in flags: 
      helpFlag = "help"
    else:
      helpFlag = None
  else:
    helpFlag = None
    fileReturnFlag = None
  param = kwargs.get('parameters', None)

  # At least two things should be supplied,
  # with the first being the search substring
  if param is not None:
    if not isinstance(param, str):
      if len(param) > 1:
        # Check if function received a list of strings.
        if isinstance(param[0], str):
          fileList.append(param[0])
          try:
            targetString = param[0]
            for index in range(1, len(param)):
              fileList.append(param[index])
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
          except:
            return "Error: Parsing search inputs failed."
        else:
          return "Error: Invalid search input."
      else:
        return "Error: Invalid search input."
    # Checks if the string is a file or just text.
    else:
      return "Error: Insufficient arguments."
  else:
    return "Error: No input to seach."

  # Early exit if person inputted "sort --help"
  if (helpFlag is not None):
    helpMessage = "Searches for a substring."
    helpMessage += "Returns either the lines with a match, \n"
    helpMessage += "or alternatively the filename with -l.\n"
    helpMessage += "Ex:\n"
    helpMessage += "grep \'lorem\' test.txt\n"
    return helpMessage

  try:
    if fileReturnFlag is not None:
      for index in range(1, len(param)):
        if targetString in param[index]:
          foundList.append(fileList[index])
    else:
      # Add everything to one big string, except the
      # search string. That way, all strings
      # can be split with every line having its
      # own list indices.
      param = "\n".join(param[1:])
      param = param.split("\n")
      for index in range(len(param)):
        if targetString in param[index]:
          foundList.append(param[index])
  except:
    return "Error seaching substrings."

  try:
    if len(foundList) > 0:
      foundList = "\n".join(foundList)
      return foundList
    else:
      return "No matches found."
  except:
    return "Error: Unsearchable input."

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
  #print(sort(parameters = sortList))
  #print(sort(parameters = testSort, flags = ["--help"]))
  #print(sort(parameters = testSort))
  fileTestList = ["test.txt", "ojofwjw", "131oj"]
  #print(sort(parameters = fileTestList))

  print(grep(flags = ["--help"], inputDirectory \
  = "/home/runner/shell/", parameters = \
  ["lorem", "test.txt"]))

  print(grep(flags = [], inputDirectory \
  = "/home/runner/shell/", parameters = \
  ["lorem", "test.txt"]))

  print(grep(flags = ["-l"], inputDirectory \
  = "/home/runner/shell/", parameters = \
  ["lorem", "test.txt"]))