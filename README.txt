========
OVERLORD
========


Overview
	Central control system in which one overlord program can directly control
	underlings.

	
Features
	Overlord Program
		Manage underlings
			Add underling through a user-specified port
			
			Remove underlings
			
			View list of underlings

		Order underlings
			Overlord can feed commands to an underling
				One underling at a time
				Supports tab completion and history through GNU Readline

	Underling Program
		Connects to overlord through a user-specified address and port
		
		Executes single commands received from overlord and sends the output back
			Includes cd
			Supports quotes and backslashes)

	Supports graceful SIGINT exiting, sorta

	Generally works

	
Compilation and Execution
	$ make
	$ overlord (on server computer, then follow instructions)
	$ underling (on each client, then follow instructions)

	
Dependencies
	GNU Readline

	
Project Files
	underling.c
	overlord.c
	util.c
	util.h
	overlist.c
	overlist.h
	makefile
	DESIGN.txt
	DEVLOG.txt
	README.txt

	
Known Bugs
	Trying to cancel a command like cat with SIGINT from the overlord is only
	partially implemented and segfaults

	Exiting the underling program directly causes major issues in the overlord;
	workaround is to remove underling in overlord interface

	
Next Steps
	Implement grouping of underlings, allowing commands to be executed on
	many machines at once (very useful! would take maybe another day)
		Imagine apt-get install here.... (requires sudo, another hurdle)

	Expand possible command inputs from just single commands; redirection,
	multiple commands, piping, etc. (could have done but with many potential bugs,
	decided to skip)

	Fix bugs

	
Made with ??? by Kenneth Li
