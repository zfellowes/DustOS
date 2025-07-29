volatile unsigned short* vga_buffer = (unsigned short*)0xB8000;
const int VGA_WIDTH = 80;

void kernel_main() {
	const char* initmsg = "DustOS v0.0.1";
	int i = 0;

	while (initmsg[i] != '\0') {
		vga_buffer[i] = (0x0F << 8) | initmsg[i];
		i++;
	}

	while (1) {
		__asm__ volatile ("hlt");
	}
}

