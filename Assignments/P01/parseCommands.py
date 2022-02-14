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
# File Information: This file parses commands into the proper tokens
# and calls all of the relevant functions from the correct output.
##############################################################################

from commands import *
import re

def executeCommands(inDirectory, inString, inHistory):
  carriedInput = []
  pendingFileMode = None

  # Grab command information from the commandsTable file,
  # as well as any other variables to check commands we need.
  commandsDict = commandsTable.fillCommands()
  flagsDict = commandsTable.fillFlags()
  argumentsDict = commandsTable.fillArguments()
  delimiters = {'>' : "w", '>>' : "a", "|" : carriedInput.append}
  tokens = parseTokens(inString)

  # Process subcommands (areas separated by delimiters)
  # until all are done because the last subcommand
  # removed all remaining tokens.
  while len(tokens) > 0:
    subCommand = getStage(tokens, delimiters)
    command = ""
    flags = []
    parameters = []
    outDirectory = []
    if pendingFileMode is not None:
      try:
        # The last delimiter should have been a
        # > or >>, so the next token and hence
        # subCommand should be a filename.
        outFile = open(subCommand[0], pendingFileMode)
        for index in range(len(carriedInput)):
          outFile.write(carriedInput[index])
        outFile.close()
        return "Outputted to file successfully."
      except:
        return "Error printing to file."
    # Normal commands that output or pipe to something.
    elif len(tokens) > 0:
      # Remember to store something as an input
      # if the previous character was <
      afterInFile = False

      # Add any carried input from previous
      # subcommand to parameters, then clear
      # previous input buffer.
      if len(carriedInput) > 0:
        for index in range (len(carriedInput)):
          parameters.append(carriedInput[index])
        carriedInput = []

      # First line after piping or on command start
      # should always be a command.
      if subCommand[0] not in commandsDict:
        return "Error, invalid command syntax."
      else:
        command = subCommand[0]
      for index in range(len(subCommand)):
        # Skip the command, it was processed above.
        if index != 0:
          if subCommand[index] == '<':
            afterInFile = True
          # The previous character was a <.
          # Open a file, read it into a string,
          # then pass that string as a parameter to
          # the command.
          elif subCommand[index] in delimiters:
            if subCommand[index] != "|":
              pendingFileMode = delimiters[subCommand[index]]
          elif afterInFile == True:
            afterInFile = False
            try:
              inFile.open(subCommand[index])
              fileText = inFile.read()
              parameters.append(fileText)
              inFile.close()
            except:
              return "Command failed. File read error."
          # Start checking what unknown fields to
          # fill. Parameters are filled first,
          # then either more parameters,
          # or an outDirectory if it supports one.
          elif subCommand[index] in flagsDict[command]:
            flags.append(subCommand[index])
          elif len(parameters) <= 0 and \
          "parameters" in argumentsDict[command]:
            parameters.append(subCommand[index])
          elif len(parameters) > 0 and \
          "outDirectory" not in argumentsDict[command]:
            parameters.append(subCommand[index])
          elif "outDirectory" in argumentsDict[command]:
            outDirectory.append(subCommand[index])
          else:
            return "Possible syntax error."
          
      # Carry out the command with the sorted tokens,
      # and save it to the input buffer list.
      carriedInput.append(commandsDict[command](\
      inDirectory = inDirectory, flags = flags,\
      parameters = parameters, outDirectory = outDirectory\
      , inHistory = inHistory))
      flags = []
      parameters = []
      outDirectory = []

    # Normal commands that don't redirect or are just last
    else:
      # Remember to store something as an input
      # if the previous character was <
      afterInFile = False

      # Add any carried input from previous
      # subcommand to parameters, then clear
      # previous input buffer.
      if len(carriedInput) > 0:
        for index in range (len(carriedInput)):
          parameters.append(carriedInput[index])
        carriedInput = []

      # First line after piping or on command start
      # should always be a command.
      if subCommand[0] not in commandsDict:
        return "Error, invalid command syntax."
      else:
        command = subCommand[0]
      for index in range(len(subCommand)):
        # Skip the command, it was processed above.
        if index != 0:
          if subCommand[index] == '<':
            afterInFile = True
          # The previous character was a <.
          # Open a file, read it into a string,
          # then pass that string as a parameter to
          # the command.
          elif subCommand[index] in delimiters:
            return "Error: command redirects without target."
          elif afterInFile == True:
            afterInFile = False
            try:
              inFile.open(subCommand[index])
              fileText = inFile.read()
              parameters.append(fileText)
              inFile.close()
            except:
              return "Command failed. File read error."
          # Start checking what unknown fields to
          # fill. Parameters are filled first,
          # then either more parameters,
          # or an outDirectory if it supports one.
          elif subCommand[index] in flagsDict[command]:
            flags.append(subCommand[index])
          elif len(parameters) <= 0 and \
          "parameters" in argumentsDict[command] and \
          subCommand[index] != "&":
            parameters.append(subCommand[index])
          elif len(parameters) > 0 and \
          "outDirectory" not in argumentsDict[command]and \
          subCommand[index] != "&":
            parameters.append(subCommand[index])
          elif "outDirectory" in argumentsDict[command]and \
          subCommand[index] != "&":
            outDirectory.append(subCommand[index])
          elif subCommand[index] != '&':
            print(subCommand[index])
            return "Possible syntax error."      
      # Carry out the command with the sorted tokens,
      # and save it to the input buffer list.
      return (commandsDict[command](\
      inDirectory = inDirectory, flags = flags,\
      parameters = parameters, outDirectory = outDirectory\
      , inHistory = inHistory))

# Separates the string into a list of all parts
# separated by an apostrophe. Then, all nonapostrophe items are
# separated by spaces.Currently, if there are more
# than three parts, that is an input error as only
# a maximum of one substring in quotes is
# in any of the commands.
def parseTokens(inString):
  inList = []
  if '\'' in inString:
    try:
      # Cut left apostrophe
      aposList = inString.split(" \'")

      # First items (commands) don't start with apostrophes
      outList = aposList[0].split()
      del aposList[0]

      # Go through each element one at a time.
      # Since we removed all left apostrophes and index 0,
      # we know that the part in between the apostrophes
      # will be on the left if we further split by apostrophe.
      # This we will grap and delete from temporary lists,
      # to preserve its exact string value, and split
      # everything else by spaces.
      while len(aposList) > 0:
        tempList = []
        tempList = aposList[0].split("\'")
        outList.append(tempList[0])
        del tempList[0]
        if len(tempList) > 0:
          tempList = tempList[0].split()
          outList = outList + tempList
        del aposList[0]
      return outList
    except:
      return "Error: Invalid command syntax."
  return inString.split()

# Note, lists are one of the few things in Python that
# behave like proper passing by reference.
# Takes a list, returns everything before any of the items
# in the dictionary that is passed. The list that is passed
# is modified to contain the rest of the list.
def getStage(inList, delimiterDict):
  outList = []
  foundSeparator = False
  while foundSeparator == False and len(inList) > 0:
    outList.append(inList[0]) 
    if (inList[0]) in delimiterDict:
      foundSeparator = True
    del inList[0]
  return outList

# Various tests
if __name__ == '__main__':
  commandsDict = {}
  commandsDict['parameters'] = "commands/cp.py"
  commandsDict['outDirectory'] = \
  "/home/runner/shell/commands/Testdir/Testdir2/test2.py"
  commandsDict['flags'] = []
  print(cp.cp(parameters = commandsDict['parameters'], outDirectory = commandsDict['outDirectory'], flags = commandsDict['flags']))
  #print (rm.rm(inDirectory = "", \
  #parameters = "commands/Testdir/Testdir2/t*"))
  testDict = commandsTable.fillCommands()
  print(testDict)
  print(parseTokens("Hello \'goodbye so long thanks for fish\' hi."))

  carriedInput = []
  outputSignal = {'>' : "w", '>>' : "a", "|" : carriedInput.append}
  print(outputSignal)
  testList = ["jwpjfpw", "jowfj", ">", "fwfw", "jowjfow"]
  cutList = getStage(testList, outputSignal)
  print(testList)
  print(cutList)
  print(executeCommands("/home/runner/shell", \
  "cp commands/cp.py /home/runner/shell/commands/Testdir/Testdir2/test2.py",\
  []))
  print(executeCommands("/home/runner/shell", \
  "chmod 777 test.txt", []))
  print(executeCommands("/home/runner/shell", "sort wjofjwojf ojowfjwo jowjfwo", \
  ["Hi", "test"]))
  print(executeCommands("/home/runner/shell", 
  "less commands/stresstest.txt", []))