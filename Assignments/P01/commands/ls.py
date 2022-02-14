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
# File Information: This file handles ls, the list directory function.
##############################################################################

import os # For scanning directory
import stat # For converting permissions
import pwd # For grabbing user from UID
import grp # For grabbing user group
import time # For converting ctime to human readable

def ls(**kwargs):
  # Parse kwargs
  inputDirectory = kwargs.get('inDirectory', None)
  if inputDirectory is None:
    return "Error, ls called without directory."
  flags = kwargs.get('flags', None)
  if (flags is not None):
    if "-a" in flags:
      allHidden = "a"
    else:
      allHidden = None
    if "-h" in flags:
      humanReadable = "h"
    else:
      humanReadable = None
    if "-l" in flags:
      longList = "-l"
    else:
      longList = None
    if "--help" in flags: 
      helpFlag = "help"
    else:
      helpFlag = None
  else:
    allHidden = None
    humanReadable = None
    longList = None
    helpFlag = None

  # Early exit if person inputted "ls --help"
  if (helpFlag is not None):
    helpMessage = "Lists and color codes all files in a directory."
    helpMessage += "\n -a lists all hidden files. "
    helpMessage += "-h converts file sizes to human "
    helpMessage += "\nreadable formatting. -l "
    helpMessage += "lists extra information.\n"
    helpMessage += "Prints the local directory "
    helpMessage += "by default but accepts a \n"
    helpMessage += "target directory as well."
    return helpMessage


  # Check if invalid path was inputted
  if not os.path.isdir(inputDirectory):
    print("Error: No directory inputted.")
    return("Invalid directory error.")
  else:
    fileList = os.listdir(inputDirectory)

  # Handle flags
  # Check if the 'a' flag was passed in.
  if allHidden is None:
    # If it was, removes every element of
    # the list that starts with a '.'
    # because that is what indicates it
    # is a hidden file. Uses filter and lambda,
    # then rebuilds with list.
    fileList = list(filter(lambda listItem: not \
    listItem.startswith('.'),fileList))

  # This will be the final string returned
  returnString = "\n"

  # Check if the 'l' flag was passed in.
  # The 'h' flag only does something if
  # 'l' is also present.
  if longList is not None:
    # Create variables for each column
    filePermissions = []
    fileLinks = []
    fileUsers = []
    fileGroups = []
    fileSizes = []
    fileDates = []
    # We need to add the path to get some of these,
    # so we need to store the original name too.
    fileNames = []
    for index in range (len(fileList)):
      fileNames.append(fileList[index])
      fileList[index] = inputDirectory + fileList[index]
      # Populate each cell in a row for a file
      filePermissions.append(stat.filemode(os.stat\
      (fileList[index]).st_mode))
      fileLinks.append(str(os.stat(fileList[index]).st_nlink))
      fileUsers.append(getUserName(fileList[index]))
      fileGroups.append(getGroupName(fileList[index]))
      # Process -h flag
      if humanReadable is not None:
        fileSizes.append(convertHumanSize(os.stat(\
        fileList[index]).st_size))
      else:
        fileSizes.append(str(os.stat(fileList[index]).st_size))
      fileDates.append(getTime(fileList[index]))

    
    fileNames = colorCodeFiles(fileNames)
    index = 0
    for index in range(0,len(fileList)):
      # Add lines to return string
      returnString = returnString + filePermissions[index] + " "
      # Variable length strings will have additional
      # spaces added according to the max column length
      # to keep consistent spacing.
      # File links
      returnString = returnString + str(fileLinks[index]) + " "
      returnString = returnString + (maxLength(fileLinks)\
      - len(fileLinks[index])) * " "
      # Users
      returnString = returnString + fileUsers[index] + " "
      returnString = returnString + ((maxLength(fileUsers)\
      - len(fileUsers[index])) * " ")
      # Groups
      returnString = returnString + fileUsers[index] + " "
      returnString = returnString + (maxLength(fileGroups)\
      - len(fileGroups[index])) * " "
      # File Sizes
      returnString = returnString + fileSizes[index] + " "
      returnString = returnString + (((maxLength(fileSizes)\
      - len(str(fileSizes[index]))) * " ") + ' ')
      # File Dates
      returnString = returnString + fileDates[index] + " "
      returnString = returnString + ((maxLength(fileDates)\
      - len(fileDates[index])) * " ")
      # File Names
      returnString = returnString + fileNames[index] + '\n'

    return returnString

  else:
    # Changing colors changes the name size,
    # so we have to record that before changing colors.
    nameMax = 0
    nameSize = []
    for index in range(len(fileList)):
      nameSize.append(len(str(fileList[index])))

    # Grab the max to determine column size
    nameMax = max(nameSize)
    width, height = os.get_terminal_size()
    # Namemax + 1 because of added space, width - 1 
    # for some wiggle room incase of inconsisten
    # terminal display
    maxColumns = int((width - 1) / (nameMax + 1))

    fileList = colorCodeFiles(fileList)
    for index in range(len(fileList)):
      returnString = returnString + fileList[index] + " "
      returnString = returnString + ((nameMax - \
      nameSize[index]) * " ")

      # Adjust columns by console width
      if (index + 1)  % maxColumns == 0:
        returnString = returnString + "\n"

    return returnString

def maxLength(inList):
  # Find the string with the max length, 
  # then find its length
  return len(max(inList, key = len))

def convertHumanSize(inBytes):
  # Byte words change every factor of 1,024
  # (2^10) starting with byte, kilo, mega,
  # and most consumers don't need more than
  # giga or terrabytes (1,024 ^ 4).
  # Each magic number just comes from manually
  # multiplying it out beforehand.
  inBytes = float(inBytes)
  outBytes = ""

  # Match each number with its prefix
  # Terrabyte
  if inBytes >= 1099511627776:
   outBytes = str(round(inBytes/1099511627776, 2)) + " T"
  # Gigabyte
  elif inBytes >= 1073741824:
    outBytes = str(round(inBytes/1073741824, 2)) + " G"
  # Megabyte
  elif inBytes >= 1048576:
    outBytes = str(round(inBytes/1048576, 2)) + " M"
  # Kilobyte
  elif inBytes >= 1024:
    outBytes = str(round(inBytes/1024, 2)) + " K"
  # Just byte
  else:
    outBytes = str(inBytes) + " B"
  
  return outBytes

def getUserName(filePath):
  return pwd.getpwuid((os.stat(filePath).st_uid)).pw_name

def getGroupName(filePath):
  return grp.getgrgid((os.stat(filePath).st_gid)).gr_name

def getTime(filePath):
  # Grab ctime and convert it to human readable form.
  try:
    thisDate = time.ctime(os.stat(filePath).st_ctime)
    # Cut off day of the week
    thisDate = thisDate[4:]
    # Cut off seconds and year.
    # Comment out to show these both.
    thisDate = thisDate[:12]
    return thisDate
  except:
    print("Error grabbing time from file.")
    return "Error: Time grabbing failure."


def colorCodeFiles(inList):
  for index in range (len(inList)):
    # Color executables red
    if ".exe" in inList[index] :
      inList[index] = '\u001b[31m' + inList[index] + '\u001b[0m'
    # Color text files yellow
    elif ".txt" in inList[index]:
      inList[index] = '\u001b[33m' + inList[index] + '\u001b[0m'
    # Color Python files cyan
    elif ".py" in inList[index]:
      inList[index] = '\u001b[32m' + inList[index] + '\u001b[0m'
    # Files without extensions (mainly folders) are green
    elif not ("." in inList[index]):
      inList[index] = '\u001b[36m' + inList[index] + '\u001b[0m'
  return inList

def colorCodeString(inString):
  # Color executables red
  if ".exe" in inString :
    inString = '\u001b[31m' + inString + '\u001b[0m'
  # Color text files yellow
  elif ".txt" in inString:
    inString = '\u001b[33m' + inString + '\u001b[0m'
  # Color Python files cyan
  elif ".py" in inString:
    inString = '\u001b[36m' + inString + '\u001b[0m'
  return inString

# For testing
if __name__ == '__main__':
  a = "-a"
  l = "-l"
  h = "-h"
  flags = ["-a", "-l", "-h"]
  #print(inputDict)

  #print(os.path.abspath(__name__))
  #print(os.listdir(os.path.abspath("/home/runner/shell/")))
  #print(ls(inDirectory = os.path.abspath("commands/ls.py"),flag = 18, a = a))
  print(ls(inDirectory = "/home/runner/shell/", flags = flags))
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
  print(time.ctime(os.stat("/home/runner/shell/shell.py").st_ctime))    

  # Outputting tests
  #testList = ["Length7", "length 8"]
  #print(maxLength(testList))
  testString = "|"
  testString = testString + (6*"|")
  print (testString)
  print(ls(inDirectory = "/home/runner/shell/", flags = []))
  print(ls(inDirectory = "/home/runner/shell/commands/", flags = flags))