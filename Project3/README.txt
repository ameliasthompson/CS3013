Author:				Maximilian Thompson
Date:				February 15, 2019
Version:			r1
Project ID:			3
CS Class:			CS 3013
Programming Language:		C
OS/Hardware dependencies:	N/A

Problem Description:	Solve two congruency related problems using either
				mutexes or semaphores. The first being the scheduling of
				pirates and ninjas in a costume department, and the second
				being the coordination of Massachusettes drivers.

Overall Design:			

	System structure	N/A

	Data representation	The state of each problem is stored as a set of global
				variables which all threads have access to.

	Algorithms 			Box-Muller transformation is used in the first problem,
				and general queue algorithms are used throughout. The first
				problem uses a solution similar to that of the readers-writers
				problem, while the second uses a solution similar to the dining
				philosophers problem.

Program Assumptions 
      and Restrictions:		The assumptions given in the spec regarding the
      				quantities of various things.

Interfaces:			

	User			N/A
	File/D-B		N/A
	Program/Module		N/A

Implementation Details:
	Data			
	Variables		Various global and local mutexes and semaphores. Additionally,
				global state representing variables.
	Algorithm		See the problem explanation files.

How to build the program:	Navigate to the subdirectories for each problem and
					run make.

Program Source:			See included heads/implementation files.

Additional Files:		problem1_explanation.txt problem2_explanation.txt

Results:			See the output file in each problem's directory.

Test Procedures:		The first problem was run several times with various
					arguments. The second problem was allowed to run continuously
					for several hours.
Test Data:			N/A

Performance Evaluation:		N/A
	Time/Space
	User Interface

References:			A few online documentations for pthread functions, as well
				as the Wikipedia's implementation of Box-Muller transformation.
				Most importantly, solutions to the readers-writers problem and
				dining philosophers problem.
