# CS 3013: Operating Systems

CS 3013 was a course on the functionality of operating systems and how they manage machine resources. The history of operating systems was also covered from the beginning when machines could only run one program at a time all the way up to modern operating systems with threading, memory paging, and CPU sharing. Additional attention was paid to threading and the invention of the test and set lock instruction.

## Assignment Breakdown

Each project folder contains a completed assignment and a more detailed readme file for that project. The assignments all have additional subfolders representing different segments of the assignment. Some represent sequential improvements and others represent completely different tasks.

### Project 0

Project 0 was less of a project and more of just setting up the development environment in a virtual machine for future projects. Because project 2 would involve the writing of kernel modules for Ubuntu that would override certain system calls, this was a more involved process that included the addition of blank system calls and recompiling the kernel.

### Project 1

Project 1 covered the usage of fork to demonstrate the strengths and limitations of spawning new processes to complete concurrent tasks. It was coupled with lecture material discussing what processes are in modern operating systems, and how resources are managed for each process (including how memory paging can make forking a process more efficient by only copying memory once one of the processes uses it).

### Project 2

Project 2 covered making system calls in the C language on the Ubuntu operating system, as well as writing kernel modules to override existing system calls. It also covered the greater difficulty there is when writing kernel level code as opposed to user level code.

The project made use of both the blank system calls introduced in project 0 and the open, close, and read system calls. The latter set demonstrated the practice of "shadowing" system calls by performing your desired action and then calling the original system call before returning.

### Project 3

Project 3 covered threading using C pthreads, mutexes, and semaphores.

The first half of the project, "Drivers," involved simulating an intersection with cars being threads and the intersection representing a set of four shared resources. Mutexes were used to control access to each section of the intersection and techniques to prevent deadlocks were employed.

The second half of the project, "PiratesVNinjas," utilized semaphores to control access to a shared resources that only one set of threads could use at a time (pirates or ninjas). Any number of threads in the group currently allowed access could utilize the shared resource together without needing to interfere with each other, but members of different groups could not.

### Project 4

Project 4 was a simulation of memory paging. A fixed length array was used to represent physical RAM while another variable length structure was used to represent the page file or partition. Loading data from the page file into memory and unloading data from memory into the page file was implemented to give the illusion of significantly more memory than was actually present in the simulation.

## Running the Projects

Each project can be run on an Ubuntu machine by running the makefile in the each of the individual folders and executing the compiled files. It is likely possible to compile and run on any Linux distro, but Ubuntu is the only one I have tested it on.

### A Note on Projects 0 and 2

Project 2 requires both the Ubuntu operating system and the specific modifications to the kernel made in project 0. Additionally, the kernel module in project 2 must be loaded after compiling it before other parts of the project will function. It is recommended to do this on a virtual machine, as it involves modifying the kernel.