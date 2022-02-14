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
# File Information: This page handles the rmdir function, which means
# 'remove directory'. It deletes only an empty directory, 
# failing otherwise. Use 'rm -r target' otherwise for a nonempty
# directory.
##############################################################################

import os

def rmdir(**kwargs):
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

  # Early exit if person inputted "rmdir --help"
  if (helpFlag is not None):
    helpMessage = "Removes the selected empty directory.\n"
    helpMessage += "Use 'rm -r' to remove a nonempty directory."
    helpMessage += "\nUsage ex.:\n"
    helpMessage += "rmdir commands/Testdir/Testdir2"
    return helpMessage

  # Folders can fail to delete for a number of reasons.
  try:
    if(os.path.isdir(param)):
        # Warn user and prompt a suggested other compile
        # if user accidentally tries on a nonempty directory.
        if (len((os.listdir(param))) > 0):
          invalidMessage = "Removal unsuccessful:\n"
          invalidMessage += "Directory is not empty.\n"
          invalidMessage += "Use 'rm -r' for nonempty" 
          invalidMessage += " directories.\n"
          return invalidMessage
        else:
          os.rmdir(param)
        return "Directory removal successful."
    else:
      os.remove(param)
      return "Error: Invalid directory name supplied."
  except:
    print("Error deleting directory.")
    return "Error: Directory deletion error."

# Various tests.
if __name__ == '__main__':
  help = "--help"
  flags = [""]
  # Test
  print (rmdir(inDirectory = "", flags = flags,\
  parameters = "commands/Testdir/Testdir2"))