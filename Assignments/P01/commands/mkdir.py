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
# File Information: This file handles mkdir, the function 
# to make a new directory.
##############################################################################

import os

def mkdir(**kwargs):
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
    if isinstance(outputDirectory, str):
      if len(outputDirectory) > 0:
        outputDirectory = outputDirectory[0]

  # Early exit if person inputted "mkdir --help"
  if (helpFlag is not None):
    return "Creates the specified directory. \"mkdir target\""

  try:
    os.mkdir(param)
  except:
    print("Error making directory.")

if __name__ == '__main__':
  mkdir(inDirectory = "", parameters = "commands/test/")