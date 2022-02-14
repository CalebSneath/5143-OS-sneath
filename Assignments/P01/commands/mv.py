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
# File Information: This function handles mv, the move command.
# Additionally, 'move' can be used on the same destination to 
# instead simply rename a file.
##############################################################################

import shutil

def mv(**kwargs):
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
  outputDirectory = kwargs.get('outDirectory', None)
  if outputDirectory is not None:
    if not isinstance(outputDirectory, str):
      if len(outputDirectory) > 0:
        outputDirectory = outputDirectory[0]

  # Early exit if person inputted "mv --help"
  if (helpFlag is not None):
    helpMessage = "Moves the first file to the "
    helpMessage += "second directory. \n"
    helpMessage += "Can also be used to "
    helpMessage += "rename a file. Ex:\n"
    helpMessage += "\"mv commands/test.py "
    helpMessage += "commands/test2.py\"\n"
    return helpMessage

  try:
    shutil.move(param, outputDirectory)
    return "File moved successfully."
  except:
    print("Error moving file.")
    return "Error: File move error."

if __name__ == '__main__':
  # Test
  print(mv(inDirectory = "", parameters = "commands/test2.py", \
  outDirectory = "/home/runner/shell/commands/test.py", \
  flags = []))