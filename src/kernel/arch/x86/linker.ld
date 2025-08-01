ENTRY(_start)

SECTIONS
{
	. = 0x00100000;

	.text :
	{
		*(.multiboot_header)
		*(.text*)
	}

	.rodata :
	{
		*(.rodata*)
	}

	.data :
	{
		*(.data*)
	}

	.bss :
	{
		*(.bss*)
		*(COMMON)
	}
}
