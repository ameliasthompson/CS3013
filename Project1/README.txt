Author:				Maximilian Thompson
Date:				January 25, 2019
Version:			r1
Project ID:			1
CS Class:			CS 3013
Programming Language:		C
OS/Hardware dependencies:	Project 0 VM

Problem Description:		Create a Midnight-Commander style shell that supports
				background processes and adding of custom commands.

Overall Design:			

	System structure	N/A

	Data representation	Menu options are stored as a fixed length array
				of structs, and	background processes are tracked
				via another fixed length array of structs. A number
				of string buffers are implimented as standard C-style
				character arrays.

	Algorithms 		Linear searches are performed throughout the program.
				Other than that, the program mostly just loops to
				prompt the user input and processes it.

Program Assumptions 
      and Restrictions:		The user will not input more than 40 arguments in
				one command, and 20 in one custom command. The user
				will not run more than 100 background processes at
				once. The user will not attempt to change directory
				with a path more than 127 characters long (the
				actual directory may be longer, it just has to be
				changed in steps). The user will never input more
				than 199 characters at once.

Interfaces:			

	User			The user is prompted to make choices.
	File/D-B		N/A
	Program/Module		N/A

Implementation Details:
	Data			
	Variables		key variables and their scopes
	Algorithm		In v2, a second thread constantly checks for finished
				child processes. The main thread may stop the wait
				thread at any point to exercise more control over
				the handling of processes (this is done to prevent
				the thread from sniping a process from the main
				thread while it's working on a blocking process).

How to build the program:	make all DEBUG=

Program Source:			See included heads/implementation files.

Additional Files:		testcases (which are test cases), and seymour
				(which waits for a while to finish executing).

Results:			See output file in each version directory. Note that
				for v1 and v2, input was redirected to a file so it
				does not appear.

Test Procedures:		For v1 and v2, input was redirected to a file
				with test cases. For v0, the contents of the
				file was manually inputted.
Test Data:			See included test cases.

Performance Evaluation:		N/A
	Time/Space
	User Interface

References:			Various online function documentations to supplement
				man.
