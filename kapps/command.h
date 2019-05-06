
#define CMDSIZE 128
#define USER "admin"
#define DOMAIN "nanoshell"
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