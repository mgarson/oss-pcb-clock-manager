Project 2: Process Table and System Clock
Author: Maija Garson
Date: 03/03/2025

Description:
This project will compile two programs into two executables using the makefile provided. One of the executables, oss, is generated from oss.cpp. The other executable,
worker, is generated from worker.cpp. The oss program launches the worker program as its child up to a specified amount of times.
The child processes run simultaneously up to a specified amount of times. The child process will also run up until a random time with the upper value for this random
time specified.

Compilation:
These prgrams will compile using the included makefile. In the command line it will compile if given:
make

Running the Program:
Once compiled, the oss program can be run with 5 options that are optional:
oss [-h] [-n proc] [-s simul] [-t timelimitForChildren] [-i intervalInMsToLaunchChildren]
Where
  	-h: Display help message
        -n proc: Proc represents the amount of total child processes to launch
        -s simul: Simul represents the amount of child processes that can run simultaneously
	-t timelimitForChildren: Represents the upper bound for the randomized time limit that child process will run
	-i intervalInMsToLaunchChildren: Represents the interval in ms to launch the next child process
Default values for options n, s, and t will be 1 and for i will be 0 if not specified in the command line

Problems Encountered:
Incremening system clock while still running child processes
	Initially I had a bit of trouble getting the child process to fork and the clock to continue running in OSS. I was able to fix this by realizing we are 
	able to use WNOHANG for this project. I had thought that could not be included at first for some reason.
Time Difference Going Negative
	I had some issues with my differences in time going into the negatives. I eventually realized it was due to overflow and used long long int instead of just int,
	this fixed the issue.
Printing Table Values Every Half Second
	This was the biggest issue I encountered. I was unable to get the PCB table to print at half second intervals. I fixed this by using different variables to calculate
	the time since the last print. I had initially thought I could use the same variables that I had used to track the time since the last fork. After changing this, this 
	issue was fixed.

Known Bugs:
The only known bug currently in my program is running the worker executable independently of oss. In the assignment guidlines, it says in this situation
worker should take in two values, the first for term time in seconds and the next for term time in nanoseconds. It is able to start running, but doesn
not move due to the system clock not incrementing. This program also does not properly check for no arguments being passed. I have a condition that
should catch it, but it does not for some reason.
