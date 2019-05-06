#include "kbmap.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define GDT_SIZE 3
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1c

#define SCANCODE_RELEASE 0x80
#define SCANCODE_NOTREL 0x7f

extern void KeyboardHandler(void);
extern void LoadIDT(unsigned long *idt_ptr);
extern void LoadGDT(unsigned long *gdt_ptr);

#define IdtEntry struct idtentry
struct idtentry {
	unsigned short offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short offset_higherbits;
};
#define GdtEntry struct gdtentry
struct gdtentry {
	uint16_t segment_limit_low;
	uint16_t base_low;
	uint8_t base_highlow; // bytes 16-23
	uint8_t access_byte;
	uint8_t flags;
	uint8_t base_highhigh; // bytes 24-31
};
GdtEntry GDT[GDT_SIZE];
IdtEntry IDT[IDT_SIZE];

void InitGDT()
{
	unsigned long gdt_addr;
	unsigned long gdt_ptr[2];
	
	// Populate GDT entry of the null descriptor
	GDT[0].segment_limit_low = 0x00;
	GDT[0].base_low = 0x00;
	GDT[0].base_highlow = 0x00;
	GDT[0].access_byte = 0x00;
	GDT[0].flags = 0x00;
	GDT[0].base_highhigh = 0; 
	
	// Populate GDT entry of code
	GDT[1].segment_limit_low = 0xff;
	GDT[1].base_low = 0x00;
	GDT[1].base_highlow = 0x00;
	GDT[1].access_byte = 0x9a;
	GDT[1].flags = 0xcf;
	GDT[1].base_highhigh = 0;
	
	// Populate GDT entry of data
	GDT[2].segment_limit_low = 0xff;
	GDT[2].base_low = 0x00;
	GDT[2].base_highlow = 0x00;
	GDT[2].access_byte = 0x92;
	GDT[2].flags = 0xcf;
	GDT[2].base_highhigh = 0;
	
	gdt_addr = (unsigned long)GDT;
	gdt_ptr[0] = (sizeof (GdtEntry) * GDT_SIZE) + ((gdt_addr & 0xffff) << 16);
	gdt_ptr[1] = gdt_addr >> 16;
	
	LoadGDT(gdt_ptr);
}

void InitIDT()
{
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	/* populate IDT entry of keyboard's interrupt */
	keyboard_address = (unsigned long)KeyboardHandler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;

	/*     Ports
	*	 PIC1	PIC2
	*Command 0x20	0xA0
	*Data	 0x21	0xA1
	*/

	/* ICW1 - begin initialization */
	WritePort(0x20 , 0x11);
	WritePort(0xA0 , 0x11);

	/* ICW2 - remap offset address of IDT */
	/*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
	WritePort(0x21 , 0x20);
	WritePort(0xA1 , 0x28);

	/* ICW3 - setup cascading */
	WritePort(0x21 , 0x00);
	WritePort(0xA1 , 0x00);

	/* ICW4 - environment info */
	WritePort(0x21 , 0x01);
	WritePort(0xA1 , 0x01);
	/* Initialization finished */

	/* mask interrupts */
	WritePort(0x21 , 0xff);
	WritePort(0xA1 , 0xff);

	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (IdtEntry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16;

	LoadIDT(idt_ptr);
}

void InitKeyboard()
{
	WritePort(0x21, 0xfd);
}
bool typing = false;
char charToType = 0;
void vga_putchar(char c);

bool ShiftPressed()
{
	bool ls = (keyboardState[KEY_LSHIFT] == KEY_PRESSED);
	bool rs = (keyboardState[KEY_RSHIFT] == KEY_PRESSED);
	return ls || rs;
}

bool IsPrintable(unsigned char keycode)
{
	if (PrintableChars[keycode] > '\x00') 
	{
		return true;
	}
	return false;
}

void KeyboardHandlerMain()
{
	unsigned char status;
	char keycode;

	/* write EOI */
	
	WritePort(0x20, 0x20);

	status = ReadPort(KEYBOARD_STATUS_PORT);
	/* Lowest bit of status will be set if buffer is not empty */
	if (status & 0x01) {
		//vga_putchar('m');
		keycode = ReadPort(KEYBOARD_DATA_PORT);
		
		if((keycode & SCANCODE_RELEASE) != 0) // the key was released
		{
			keyboardState[(keycode & SCANCODE_NOTREL)] = KEY_RELEASED;
			//printf("%x", keycode);
		}
		else
		{
			keyboardState[(keycode & SCANCODE_NOTREL)] = KEY_PRESSED;
		}
		
		getchflag = false;
	}
}
void WaitForKeyPress()
{
	char key = 0;
	while (key == 0)
	{
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
	}
}


uint8_t GetKey()
{
	for(unsigned char i = 0; i < 128; i++)
	{
		if(keyboardState[i] == KEY_PRESSED)
		{
			return i;
		}
	}
	
	return 0;
}