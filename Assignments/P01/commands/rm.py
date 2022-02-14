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
# File Information: This file handles rm, the 'remove' command.
# By default, rm only handles files and will fail if used on
# a directory. Using in addition to the -r command however,
# will recursively remove all files and directories in the 
# target directory, eventually including the directory itself. 
# This can break things very fast, so use with care.
# Using with * will delete all files in the directory matching
# the wildcard.
##############################################################################

import os
import re # To handle wildcards

def rm(**kwargs):
  # Parse kwargs
  inputDirectory = kwargs.get('inDirectory', None)
  flags = kwargs.get('flags', None)
  if (flags is not None):
    if "-r" in flags:
      recursive = "r"
    else:
      recursive = None
    if "--help" in flags: 
      helpFlag = "help"
    else:
      helpFlag = None
  else:
    recursive = None
    helpFlag = None
  param = kwargs.get('parameters', None)
  # Allow for some polymorphism by letting lists
  # or strings be the inputs.
  if param is not None:
    if not isinstance(param, str):
      if len(param) > 0:
        param = param[0]
  else:
    return "Error: No removal target supplied."

  # Early exit if person inputted "rm --help"
  if (helpFlag is not None):
    helpMessage = "Removes the selected file. \nWith -r can"
    helpMessage += " remove a directory and its contents.\n"
    helpMessage += "Warning: This CAN break things quickly."
    helpMessage += "\nUse with intention and care.\n"
    helpMessage += "-r recursively removes files from "
    helpMessage += "the directory.\n"
    helpMessage += "'fil*e' or '*file' or `file*':\n"
    helpMessage += "Remove files that match a wildcard"
    return helpMessage

  # Files can fail to delete for a number of reasons.
  try:
    # Handle wildcards
    if('*' in param):
      # tweak format to better suit regex
      param = param.replace('*', '.*')
      # Strip the file off to find the target directory
      foundDirectory = False
      tempDirectory = param
      while(foundDirectory == False) and (len(tempDirectory) != 0):
        # Slice off the last letter until a / is found
        if(tempDirectory[-1] == "/"):
          tempDirectory = tempDirectory[:-1]
          foundDirectory = True
        else:
          tempDirectory = tempDirectory[:-1]
      if(os.path.isdir(tempDirectory)):
        fileList = os.listdir(tempDirectory)
        deletedCount = 0

        # This runs through the current directory
        # and checks if any files match the wildcard,
        # then just recursively calls this function
        # on all of the matches as it goes.
        for index in range (len(fileList)):
          # Empty lists, the result of a negative regex,
          # findall result, are false in boolen checks
          if re.findall(param, tempDirectory + '/' + fileList[index]):
            rm(parameters = tempDirectory + '/' + \
            fileList[index])
            deletedCount += 1
        print(int(bool(deletedCount)) * 's')

        # Return a success message
        # The middle stuff just adjusts for the plural "s"
        return(str(deletedCount) + " file" + \
        ((not (deletedCount == 1)) * 's') + " deleted.")

      else:
        print("Error parsing directory for wildcard.")
        return "Error: Invalid wildcard expression."
    # If the given argument is a directory,
    # its either a user error, or the -r flag was used.
    elif(os.path.isdir(param)):
      if(recursive is None):
        failNote = "rm failed: Directory chosen as target."
        failNote += "\nUse the -r flag or rmdir to remove \n"
        failNote += "directories instead of just files.\n"
        return failNote
      else: 
        # If the -r flag is used, recursively
        # call this function until all directories
        # and nested files are removed, then delete
        # this directory.
        fileList = os.listdir(param)
        for index in range(len(fileList)):
          if(os.path.isdir(param + '/' + fileList[index])):
            rm(parameters = param + '/' + fileList[index], r = "-r")
          else:
            os.remove(param + '/' + fileList[index])
        # Recursion ended, delete this directory
        os.rmdir(param)
        return "Recursive removal successful."
    else:
      os.remove(param)
      return "File removal complete."
  except:
    print("Error deleting file.")
    return "Error: File deletion error."


if __name__ == '__main__':
  flags = ["-"]
  #flags.append("--help")
  flags.append("")
  # Test
  print (rm(inDirectory = "", flags = flags,\
  parameters = "commands/Testdir/Testdir2"))