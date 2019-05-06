#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b

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
	puts("Operating system demo  Copyright (C) 2019 iProgramInCpp\n");
    	puts("This program comes with ABSOLUTELY NO WARRANTY; for details type `show-w'.\n");
    	puts("This is free software, and you are welcome to redistribute it\n");
    	puts("under certain conditions; type `show-c' for details.\n");
	
	InitIDT();
	InitKeyboard();
	
	terminal_color = 0x1f;
	
	CommandPrompt_Main();
}
