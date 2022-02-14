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
# File Information: This page handles history function. It
# returns a string with the command history, as long as
# it is passed to it.
##############################################################################

def history(**kwargs):
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

  inputHistory = kwargs.get('inHistory', None)
  if inputHistory is not None:
    if not isinstance(inputHistory, str):
      # Combine each element of the list, separated
      # by a line break.
      if len(inputHistory) > 0:
        inputHistory = '\n'.join(inputHistory)
      # Empty history
      else:
        inputHistory = ""
  elif helpFlag is None:
    return "Error: No history passed."

  # Early exit if person inputted "rmdir --help"
  if (helpFlag is not None):
    helpMessage = "Returns the history of commands entered.\n"
    helpMessage += "\nUsage ex.:\n"
    helpMessage += "history\n"
    helpMessage += "History can be viewed and modified in \n"
    helpMessage += "the history.log file as well."
    return helpMessage

  return inputHistory

# Various tests.
if __name__ == '__main__':
  help = "--help"
  flags = [""]
  # Test
  print (history(flags = flags,\
  inDirectory = "commands/Testdir/Testdir2/", inHistory \
  = []))
  print (history(flags = flags,\
  inDirectory = "commands/Testdir/Testdir2/", inHistory \
  = ["Test1"]))
  print (history(flags = flags,\
  inDirectory = "commands/Testdir/Testdir2/"))
  print (history(flags = ["--help"],\
  inDirectory = "commands/Testdir/Testdir2/"))
  #"commands/Testdir/Testdir2"
