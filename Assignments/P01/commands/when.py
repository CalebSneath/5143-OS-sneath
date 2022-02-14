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
# File Information: A bonus command I added, to get the time in
# an unconventional way.
##############################################################################

import os # For scanning directory
import stat # For converting permissions
import time # For converting ctime to human readable

def when(**kwargs):
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


  # Early exit if person inputted "ls --help"
  if (helpFlag is not None):
    helpMessage = "Gets the current date and time."
    helpMessage += "\n Ex.:\n"
    helpMessage += "when\n"
    return helpMessage


  # This will be the final string returned
  returnString = ""
  try:
    timeFile = open("timefile.temp", "w")
    returnString += getTime("timefile.temp")
    timeFile.close()
    return returnString
  except:
    return "Error finding time."

def getTime(fileName):
  # Grab ctime and convert it to human readable form.
  try:
    thisDate = time.ctime(os.stat(fileName).st_ctime)
    # Cut off day of the week
    #thisDate = thisDate[4:]
    # Cut off seconds and year.
    # Comment out to show these both.
    #thisDate = thisDate[:12]
    return thisDate
  except:
    print("Error grabbing time.")
    return "Error: Time grabbing failure."

# For testing
if __name__ == '__main__':
  a = "-a"
  l = "-l"
  h = "-h"
  flags = ["-a", "-l", "-h"]
  #flags.append("--help")
  #print(inputDict)

  #print(os.path.abspath(__name__))
  #print(os.listdir(os.path.abspath("/home/runner/shell/")))
  #print(ls(inDirectory = os.path.abspath("commands/ls.py"),flag = 18, a = a))
  print(when(inDirectory = "/home/runner/shell/", flags = flags))
  #print(os.stat("/home/runner/shell/main1.py"))    

  #print(stat.filemode(os.stat("/home/runner/shell/main1.py").st_mode))
  #print(grp.getgrgid(1000).gr_name)
  #print(pwd.getpwuid(1000).pw_name)
  #print(convertHumanSize(9999999999999))
  # Color change test
  #print('\u001b[31m' + "Test" + '\u001b[0m' + " 1")

  # Test to see if ansi codes add size to len() function.
  # They do. They also show up when printing a list as a whole,
  # but not the individual string elements of a list one at a time.
  #test = "test"
  #print(len(test))
  #test = '\u001b[31m' + test + '\u001b[0m'
  #print (len(test))

  # Time tests
  print(os.stat("/home/runner/shell/shell.py").st_ctime)    
  #print(time.ctime(os.stat("/home/runner/shell/shell.py").st_ctime))    

  # Outputting tests
  #testList = ["Length7", "length 8"]
  #print(maxLength(testList))
  testString = "|"
  testString = testString + (6*"|")
  print (testString)