Author:				Maximilian Thompson
Date:				February 22, 2019
Version:			r1
Project ID:			4
CS Class:			CS 3013
Programming Language:		C
OS/Hardware dependencies:	N/A

Problem Description:		Implement virtual memory management on a small scale.

Overall Design:			

	System structure	N/A

	Data representation	Main memory is stored as a fixed length array of unsigned
				characters. The backing store is stored as an array of structs
				containing meta data on the page they represent, as well as the
				actual data. Page tables are stored as pages within main memory
				using fixed width bit fields, and the location (if any) of a
				proccess's page table is stored externally.

	Algorithms 		Removing pages is done with a slightly modified round robin
				algorithm. There are a bunch of linear searches almost everywhere.

Program Assumptions 
      and Restrictions:		The user will never need to address more than 4 physical
	  				frames, or access a virtual address space greater than 64
					bytes. Additionally, there will never be more than 4 processes,
					and nothing will act concurrently.

Interfaces:			

	User			Commands following the format: pid,cmd,address,value
	File/D-B		N/A
	Program/Module		N/A

Implementation Details:
	Data			
	Variables		Globals representing the current state of physical memory
				and the backing store.
	Algorithm		Pages are removed with a round robin algorithm which keeps
				track of the last removal using a static variable. As a slight
				modification, the algorithm skips over page tables of the
				currently active proccess in order to reduce the likelyhood of
				error. Additionally, when a page table is removed, all pages
				that it contains which are currently in memory are removed as
				well due to some difficulties keeping track of which frames
				were allocated after the page table was removed.

How to build the program:	Use the make file in each of the subdirectories.

Program Source:			See included heads/implementation files in each of the
					sub directories.

Additional Files:		There are test cases (tcases) and test output (tout) in
					each of the subdirectories.

Results:			See the test output files in each of the subdirectories.

Test Procedures:	Commands from the test cases were inputted in order.
Test Data:			See included test cases.

Performance Evaluation:		N/A
	Time/Space
	User Interface

References:			Various online function documentations to supplement
				man.
