
#define CMDSIZE 128
#define USER "user"
#define DOMAIN "os-test"
#define OS_NAME "Operating system test"
#define COPYRIGHT_MSG "(c) 2019 iProgramInCpp"
char CmdBuffer [CMDSIZE];

uint16_t term_x = 0;
uint16_t term_y = 0;

void PrintUserPrompt()
{
	printf("\n\n%s@%s>", USER, DOMAIN);
	term_x = terminal_column;
	term_y = terminal_row;
	putch('_');
}

uint8_t GetNumber(char m)
{
	char s = m;
	if(s >= 'a')
	{
		s -= 0x27;
	}
	if(s >= 'A')
	{
		s -= 0x07;
	}
	return s - '0'; 
}

void ProcessCommand()
{
	char cmd[128];
	int aslkfj = 0;
	char* cmdbuf = CmdBuffer;
	while(*cmdbuf != ' ' && *cmdbuf != 0)
	{
		cmd[aslkfj] = *cmdbuf;
		cmdbuf++;
		aslkfj++;
	}
	if(strcmp(CmdBuffer, "test") == 0)
	{
		printf("\n\n%s", OS_NAME);
	}
	// The GNU GPLv3 requests this
	else if(strcmp(CmdBuffer, "show-w") == 0)
	{
		puts("\n\nThe GNU GPLv3 license provides no warranty, as described in section 15 of the license file.\n\n\n15. Disclaimer of Warranty.\n\nTHERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\nAPPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\nHOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\nOF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\nTHE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\nPURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\nIS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\nALL NECESSARY SERVICING, REPAIR OR CORRECTION.\nEnd of passage.\n");
	}
	else if(strcmp(CmdBuffer, "show-c") == 0)
	{
		puts("\n\nCheck the GNU GPLv3 license for that, as I could not find it, at https://choosealicense.com/licenses/gpl-3.0/\n");
	}
	else if(strcmp(CmdBuffer, "cls") == 0)
	{
		TerminalClear();
		terminal_column = 0;
		terminal_row = 0;
	}
	else if(strcmp(CmdBuffer, "help") == 0)
	{
		printf("\n\nWow! This is some useful help info:\n- cls  -> clear screen\n- test -> shows version\n\nThis is a lite version for debugging.\n");
	}
	else
	{
		printf("\nIllegal Command: '%s'", CmdBuffer);
	}
	PrintUserPrompt();
	
	memset(CmdBuffer, 0, CMDSIZE);
	memset(cmd, 0, 128);
}

void CommandPrompt_Main()
{
	uint8_t ptr = 0xff;
	
	PrintUserPrompt();
	terminal_column = term_x;
	terminal_row = term_y;
	puts(CmdBuffer);
	bool isRunning = true;
	do
	{
		char key = 0;
		for(unsigned char i = 0; i < 128; i++)
		{
			if(keyboardState[i] == KEY_PRESSED)
			{
				if(IsPrintable(i))
				{
					key = KeyboardMap[i + (ShiftPressed() ? 0x80 : 0x00)];
					keyboardState[i] = KEY_HELD;
					break;
				}
			}
		}
		if(key != 0)
		{
			if(key == '\b')
			{
				if(ptr < 0xff)
				{
					terminal_column = term_x;
					terminal_row = term_y;
					for(uint8_t i = 0; i < strlen(CmdBuffer) + 1; i++)
					{
						putch(' ');
					}
					CmdBuffer[ptr] = 0;
					ptr--;
					terminal_column = term_x;
					terminal_row = term_y;
					puts(CmdBuffer);
					putch('_');
				}
			}
			else if(key == '\n')
			{
				terminal_column = term_x;
				terminal_row = term_y;
				puts(CmdBuffer);
				putch(' ');
				ProcessCommand();
				ptr = 0xff;
			}
			else
			{
				ptr++;
				CmdBuffer[ptr] = key;
				terminal_column = term_x;
				terminal_row = term_y;
				bool m = cputs(CmdBuffer); // checks if screen scrolled
				putch('_');
				if(m)
				{
					term_y--;
				}
				terminal_column = term_x;
				terminal_row = term_y;
			}
			key = 0;
		}
	}
	while(isRunning);
}
