Author:				Maximilian Thompson
Date:				Feburary 5, 2019
Version:			r1
Project ID:			2
CS Class:			CS 3013
Programming Language:		C
OS/Hardware dependencies:	Project 0 VM

Problem Description:		Impliment a "virus scanner" and a process ancestry
					analyzer through the use of a kernel module and userland code.

Overall Design:			

	System structure	N/A

	Data representation	The ancestry data is stored in a struct containing a
				few arrays.

	Algorithms 		A naive string search is used in the virus scan.

Program Assumptions 
      and Restrictions:		A process will have no more than 100 siblings, 100
      				children, and 10 ancestors.

Interfaces:			

	User			The procAncestry executable requires a numerical PID argument.
	File/D-B		N/A
	Program/Module		N/A

Implementation Details:
	Data			
	Variables		Pointers to the original system calls are stored.
	Algorithm		A naive string search algorithm is implimented for the virus
				scan. It essentially tries all starting positions in the string
				looking for the substring until it either finds the first occurance
				or finds nothing.

How to build the program:	Run the makefile in module/, then use insmod to insert
				it before proceeding to run the makefiles in goodbye/ and procAncestry/

Program Source:		See included heads/implementation files.

Additional Files:	virusScan/aVirus and /notAVirus are used with cat to test
				the functionality of the system calls which replace open, close,
				and read. virusScan/syslog and procAncestry/syslog contain excerpts
				from the system log while testing those aspects of the project.

Results:			See the syslog files.

Test Procedures:	Insert the module and run the included userland executables.
				Additionally, cat the virus test files.
Test Data:			virusScan/aVirus and /notAvirus

Performance Evaluation:		N/A
	Time/Space
	User Interface

References:		The provided sample for an interceptor and the wikipedia page on
			string search algorithms.
