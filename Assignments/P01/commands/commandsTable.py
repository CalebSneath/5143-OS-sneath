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
# File Information: This file can also a dictionary of all 
# loaded commands, as long as the command is added to the
# __init__.py file and updated on this page.
# Page Update Information: add imports to the top of
# this page. Add a line for the dictionary fill as well
# in the functions.
# General command kwargs: 
# inDirectory = gives function a string of additional
# input directory information
# flags = is a list, which can usually be empty. All 
# files should support a --help flag for more info.
# parameters = is usually able to accept a string or list,
# although some commands only support a list.
# outDirectory = is used for a few functions,
# to specify a target destination, although
# some commands use just parameters instead.
# inDirectory and flags are fairly univeral,
# although see the table below to see which
# commands accept parameters and flags.
##############################################################################

# Relative vs absolute imports are different if
# this is ran as a standalone test vs as part of package.
try:
  from .cp import *
  from .ls import *
  from .mkdir import *
  from .mv import *
  from .rm import *
  from .rmdir import *
  from .pwd import *
  from .history import *
  from .chmod import *
  from .when import *
  from .sort import *
  from .who import *
  from .cat import *
  from .less import *
  from .grep import *
  from .wc import *
  from .head import *
  from .tail import *
except:
  from cp import *
  from ls import *
  from mkdir import *
  from mv import *
  from rm import *
  from rmdir import *
  from pwd import *
  from history import *
  from chmod import *
  from when import *
  from sort import *
  from who import *
  from cat import *
  from less import *
  from grep import *
  from wc import *
  from tail import *

# A dictionary linking commands to their functions.
def fillCommands():
  commandsDict = {}
  commandsDict['cp'] = cp
  commandsDict['ls'] = ls
  commandsDict['mkdir'] = mkdir
  commandsDict['mv'] = mv
  commandsDict['rm'] = rm
  commandsDict['rmdir'] = rmdir
  commandsDict['pwd'] = pwd
  commandsDict['history'] = history
  commandsDict['chmod'] = chmod
  commandsDict['when'] = when
  commandsDict['sort'] = sort
  commandsDict['who'] = who
  commandsDict['cat'] = cat
  commandsDict['less'] = less
  commandsDict['grep'] = grep
  commandsDict['wc'] = wc
  commandsDict['head'] = head
  commandsDict['tail'] = tail

  return commandsDict

# A dictionary listing all flags for a function.
def fillFlags():
  flagsDict = {}
  flagsDict['cp'] = ["--help"]
  flagsDict['ls'] = ["--help", "-l", "-h", "-a"]
  flagsDict['mkdir'] = ["--help"]
  flagsDict['mv'] = ["--help"]
  flagsDict['rm'] = ["--help", "-r"]
  flagsDict['rmdir'] = ["--help"]
  flagsDict['pwd'] = ["--help"]
  flagsDict['history'] = ["--help"]
  flagsDict['chmod'] = ["--help"]
  flagsDict['when'] = ["--help"]
  flagsDict['sort'] = ["--help"]
  flagsDict['who'] = ["--help"]
  flagsDict['cat'] = ["--help"]
  flagsDict['less'] = ["--help"]
  flagsDict['grep'] = ["--help", "-l"]
  flagsDict['wc'] = ["--help", "-l", "-m", "-w"]
  flagsDict['head'] = ["--help", "-n"]
  flagsDict['tail'] = ["--help", "-n"]

  return flagsDict

# A dictionary listing all parameter types for a function.
def fillArguments():
  argsDict = {}
  argsDict['cp'] = ["parameters", "outDirectory"]
  argsDict['ls'] = []
  argsDict['mkdir'] = ["parameters"]
  argsDict['mv'] = ["parameters", "outDirectory"]
  argsDict['rm'] = ["parameters"]
  argsDict['rmdir'] = ["parameters"]
  argsDict['pwd'] = []
  argsDict['history'] = []
  argsDict['chmod'] = ["parameters", "outDirectory"]
  argsDict['when'] = []
  argsDict['sort'] = ["parameters"]
  argsDict['who'] = []
  argsDict['cat'] = ["parameters"]
  argsDict['less'] = ["parameters"]
  argsDict ['grep'] = ["parameters"]
  argsDict['wc'] = ["parameters"]
  argsDict ['head'] = ["parameters"]
  argsDict ['tail'] = ["parameters"]

  return argsDict



# For testing the command dictionary functions.
if __name__ == '__main__':
  testDict = fillCommands()
  testArg = "cp"
  testArg2 = "rm"
  testArg3 = "-h"
  print(testDict[testArg](arguments = "commands/cp.py", outDirectory = "/home/runner/shell/commands/Testdir/Testdir2/test2.py"))

  # Quick list test
  testList = ["Test1", "test2"]
  if "test2" in testList:
    print("Present")
