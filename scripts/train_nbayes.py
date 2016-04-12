#!/usr/bin/env python

# file: $CIS5538-Project/scripts/train_nbayes.py
#
# revision history:
#  20160319 (SL): initial version
#

# import system modules
#
import ConfigParser
import StringIO
import copy
import os
import re
import sys
import numpy as np

#------------------------------------------------------------------------------
#
# the main program starts here
#
#------------------------------------------------------------------------------

# method: main
#
# arguments: none
#
# return: none
#
# This method is the main function.
#
def main(argv):

 print 'Number of Arguments:', len(sys.argv), 'arguments.'
 print 'Argument List:', str(sys.argv)
 

 # get arguments from the commandline
 #
 #  get number of arguments
 #
 args = len(sys.argv);
 # loop through arguments and assign values to the correct variable
 #
 for i in range(0, args):
     if sys.argv[i] == '-seiz':
         file_seiz = sys.argv[i+1]
     elif sys.argv[i] == '-noseiz':
         file_noseiz = sys.argv[i+1]

 if file_noseiz == file2

#
# end of method.

# begin gracefully
#    
if __name__ == "__main__":
    main(sys.argv[1:])

#
# end of file

