#+TITLE: OS Lab 06
#+AUTHOR: Anurag Shirolkar (120050003) Dheerendra Rathor (120050033)
#+OPTIONS: ^:nil
* Question 1
Init_Sound_Devices
	calls a TODO_P function which should print "Initializing sound card".
Mount_Root_Filesystem
	Mounts the root filesystem
TODO_P(PROJECT_VIRTUAL_MEMORY_A, "initialize page file.");
Set_Current_Attr(ATTRIB(BLACK, GREEN | BRIGHT));
Print("Welcome to GeekOS!\n");
Set_Current_Attr(ATTRIB(BLACK, GRAY));
	The first command sets the font color to bright green.
	The second line print the string "Welcome to GeekOS!".
	The third line sets the font color back to gray.
TODO_P(PROJECT_SOUND, "play startup sound");
	This function checks if the first argument is true or not, if yes it prints the
	second argument
Spawn_Init_Process
	Creates a kernel_thread called initProcess.
	Calls Spawn_Foregroud function with the executable /c/shell.exe and initProcess.
	Then waits for the initProcess to exit.
Hardware_Shutdown
	Sends shutdown code "Shutdown" for Qemu and Boch
