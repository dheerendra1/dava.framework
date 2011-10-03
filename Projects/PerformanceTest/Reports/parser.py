#!/usr/bin/env python2.5

import os

screens = {}
currentScreen = ''
currentTest = ''

files = os.listdir('.')
for file in files:
    if file[-7:] == ".report":
        f = open(file)
        
        for line in f:
            if line == '\n':
                continue
            elif line[:1] == '$':
                nextIsTestname = True
                currentScreen = line[1:-1]
                if currentScreen not in screens.keys():
                    screens[currentScreen] = {}
            elif nextIsTestname:
                nextIsTestname = False
                currentTest = line[:-1]
                if currentTest not in screens[currentScreen].keys():
                    screens[currentScreen][currentTest] = []
            else:
                nextIsTestname = True
                timesStrings = line[:-1].split()
                timesNumbers = []
                for time in timesStrings:
                    timesNumbers.append(int(time))
                screens[currentScreen][currentTest].append(timesNumbers)
 
        f.close()
    
report = open('!report.html', 'w')

for currentScreen in screens.keys():
    report.write('<h1>'+currentScreen+'</h1>')
    report.write('<table style="border: 1px solid gray; cellspacing: 0px; cellpadding: 0px; border-collapse: collapse;">')
    for currentTest in screens[currentScreen].keys():
        report.write('<tr style="border: 1px solid gray; cellspacing: 0px; cellpadding: 0px;">')
        report.write('<td style="border: 1px solid gray; cellspacing: 0px; cellpadding: 0px;">')
        report.write(currentTest)
        baseTimes = None
        for times in screens[currentScreen][currentTest]:
            report.write('<td style="border: 1px solid gray; cellspacing: 0px; cellpadding: 0px;">')
            for time in times:
                report.write(str(time)+' ')
            if baseTimes:
                report.write('<br>')
                index = 0
                times = times[:2]
                for time in times:
                    baseTime = baseTimes[index]
                    if(baseTime):
                        baseTime = float(baseTime)
                        time = float(time)
                        delta = (time-baseTime)/baseTime
                        sign = ''
                        color = 'black'
                        if delta > 0:
                            sign = '+'
                            color = 'red'
                        elif delta < 0:
                            color = 'green'
                        report.write("<font size = '2' color = '" +color + "'>")
                        report.write(sign+'%(percent).1f%% ' % {'percent': delta*100})
                        report.write('</color>')
                    else:
                        report.write('- ')
                 
                    index += 1
            baseTimes = times
            report.write('</td>')
        report.write('</td>')
        report.write('</tr>')
    
    report.write('</table>')

report.close()
