ENTRY(_start)

SECTIONS
{
	. = 1M; /*0x00100000;*/

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
