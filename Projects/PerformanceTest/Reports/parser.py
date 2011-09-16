#!/usr/bin/env python2.5

import os

files = os.listdir('.')
for file in files:
    if file[-7:] == ".report":
        f = open(file)
        
        line = ""
        while line = f.readline():
            print line
        
        f.close()
