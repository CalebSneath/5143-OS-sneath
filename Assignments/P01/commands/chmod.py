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
# File Information: This page handles the chmod function, which
# allows users to modify the permissions of a file. It takes
# a file as well as three numbers, which represent the permissions.
##############################################################################

import os

def chmod(**kwargs):
  # Parse kwargs
  inputDirectory = kwargs.get('inDirectory', None)
  # Fix formatting.
  if inputDirectory is not None and len(inputDirectory) > 0:
    if inputDirectory[-1] == '/':
      inputDirectory = inputDirectory[:-1]
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
    return "Invalid syntax. Insufficient arguments."
  outputDirectory = kwargs.get('outDirectory', None)
  if outputDirectory is not None:
    if not isinstance(outputDirectory, str):
      if len(outputDirectory) > 0:
        outputDirectory = outputDirectory[0]
  else:
    return "Invalid syntax. Insufficient arguments."

  # Early exit if person inputted "rmdir --help"
  if (helpFlag is not None):
    helpMessage = "Modifies the permissions of the target."
    helpMessage += "\nUsage ex.:\n"
    helpMessage += "chmod 3digitpermissions target.txt\n"
    return helpMessage

  try:
    # Check if the output directory alone contains a valid 
    # target. if not, see if concatenating the input directory
    # with the parameter gives a valid target. If not,
    # it's probably a user error.
    if (os.path.exists(outputDirectory)):
      # chmod takes octal so it has to be cast from the string.
      os.chmod(outputDirectory, int(param, 8))
      return "File permissions successfully altered."
    elif(os.path.exists(inputDirectory + "/" + outputDirectory)):
      os.chmod((inputDirectory + "/" + outputDirectory), \
      int(param, 8))
      return "File permissions successfully altered."
    else:
      return "Error: invalid target."
  except:
    return "Error changing file permissions."

# Various tests.
if __name__ == '__main__':
  help = "--help"
  flags = [""]
  flags = ["--help"]
  # Test
  #print (history(flags = flags,\
  #inDirectory = "commands/Testdir/Testdir2/", inHistory \
  #= []))
  print(str(oct(int("777", 8))))
  print(chmod(outDirectory = ["commands/test.py"], \
  parameters = ["777"], flags = flags))
  print(chmod(outDirectory = ["/home/runner/shell/test.txt"], \
  parameters = ["777"], flags = []))