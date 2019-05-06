/*
   NanoShell OS
   
   Kernel.c -> Main kernel functions
   
   - Also serves to unite all of the other code (spread around in builtin/ and drivers/.
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b

#define OS_NAME "Welcome to the Operating system test"
#define COPYRIGHT_MSG "(c) 2019 iProgramInCpp"

#define BYTE unsigned char
#include "drivers/terminal.h"
#include "data.h"
#include "memory.h"
#include "drivers/port.h"
#include "stdio.h"
#include "string.h"
#include "drivers/vga.h"
#include "drivers/keyboard.h"

// Include the applications
#include "../kapps/command.h"

void KernelMain(void) 
{
	/* Initialize terminal interface */
	InitTerminal();
	terminal_color = 0x1f;
	TerminalClear();
	printf("Loading 80x50 mode...");
	SwitchMode(TMode80x50);
	TerminalClear();
	terminal_row = 0;
	terminal_column = 0;
	printf("%s\n%s\n", OS_NAME, COPYRIGHT_MSG);
	
	InitIDT();
	InitKeyboard();
	
	terminal_color = 0x1f;
	
	CommandPrompt_Main();
}