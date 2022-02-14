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
# File Information: This page handles pwd functin. This
# function simply returns a directory passed to it,
# or if none is given, tries to guess the working directory.
##############################################################################

import os

def pwd(**kwargs):
  # Parse kwargs
  inputDirectory = kwargs.get('inDirectory', None)
  if inputDirectory is None:
    # Just initialize the initial current directory to 
    # this directory. [:-8] removes /main.py
    inputDirectory = str(os.path.abspath('__main__'))
    foundSlash = False
    # Cut off the last file by cutting everythin at
    # the end up to and including the last / to find path
    while foundSlash is False and len(inputDirectory) > 0:
      if inputDirectory[-1] == '/':
        foundSlash = True
      inputDirectory = inputDirectory[:-1]
    if len(inputDirectory) == 0:
      return "Error, can't find directory."
  flags = kwargs.get('flags', None)
  if (flags is not None):
    if "--help" in flags: 
      helpFlag = "help"
    else:
      helpFlag = None
  else:
    helpFlag = None

  # Early exit if person inputted "rmdir --help"
  if (helpFlag is not None):
    helpMessage = "Returns the current working directory.\n"
    helpMessage += "\nUsage ex.:\n"
    helpMessage += "pwd"
    return helpMessage

  # Folders can fail to delete for a number of reasons.
  try:
    while not (os.path.isdir(inputDirectory)) and \
    len(inputDirectory) > 0:
      inputDirectory = inputDirectory[:-1]
    if len(inputDirectory) > 0:
      return inputDirectory
    else:
      return "Error: Unable to find current directory."
  except:
    print("Error finding directory.")
    return "Error: finding current directory."

# Various tests.
if __name__ == '__main__':
  help = "--help"
  flags = [""]
  # Test
  print (pwd(flags = flags,\
  inDirectory = "commands/Testdir/Testdir2/"))
  print (pwd(flags = flags,\
  inDirectory = "commands/Testdir/Testdir2/"))
  #"commands/Testdir/Testdir2"
  print(os.path.isdir("commands/Testdir/Testdir2/"))