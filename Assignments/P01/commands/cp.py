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
# File Information: This file handles cp, the copy function.
##############################################################################

import shutil
import os

def cp(**kwargs):
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
  # or strings be the inputs.
  if param is not None:
    if not isinstance(param, str):
      if len(param) > 0:
        param = param[0]
  else:
    return "Error: No target file specified."
  outputDirectory = kwargs.get('outDirectory', None)
  if outputDirectory is not None:
    if not isinstance(outputDirectory, str):
      if len(outputDirectory) > 0:
        outputDirectory = outputDirectory[0]
  else:
    return "Error: No destination or name specified."

  # Early exit if person inputted "cp ---help"
  if (helpFlag is not None):
    return "Copies the first file to the second directory."
  try:
    shutil.copy(param, outputDirectory)
    return "File copied successfully."
  except:
    # Try again but create directory first.
    try:
      # Strip the file off to find the required directory
      foundDirectory = False
      tempDirectory = outputDirectory
      while(foundDirectory == False and tempDirectory != ""):
        # Slice off the last letter until a / is found
        if(tempDirectory[-1:] == "/"):
          tempDirectory = tempDirectory[:-1]
          foundDirectory = True
        else:
          tempDirectory = tempDirectory[:-1]
      os.makedirs(tempDirectory, exist_ok = True)
      shutil.copy(param, outputDirectory)
      return "File copied successfully."
    except:
      # Some other problem existed.
      print("Error copying file.")
      return "Error: File copy error."

if __name__ == '__main__':
  # Test
  #cp(inDirectory = "", parameters = "commands/cp.py", \
  #outDirectory = \
  #"/home/runner/shell/commands/Testdir/Testdir2/test.py")

  commandsDict = {}
  commandsDict['parameters'] = "commands/cp.py"
  commandsDict['outDirectory'] = \
  "/home/runner/shell/commands/Testdir/Testdir2/test2.py"
  commandsDict['flags'] = []
  print(cp(parameters = commandsDict['parameters'], outDirectory = commandsDict['outDirectory'], flags = commandsDict['flags']))