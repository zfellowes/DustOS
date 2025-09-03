#include <cpu/timer.h>
#include <drivers/display.h>
#include <drivers/ports.h>
#include <kernel/kernel.h>

// Definitions
#define MAX_COMMANDS 32
#define MAX_ARGS 8
#define MAX_ARG_LEN 64

static fs_file_t* current_dir;

// Tokenize any arguments
int tokenize(char *input, char *argv[], int max_args)
{
	int argc = 0;
	while (*input && argc < max_args) {
		while (*input == ' ') input++;
		if (!*input) break;
		
		argv[argc++] = input;

		while (*input && *input != ' ') input++;
		if (*input) {
			*input = '\0';
			input++;
		}
	}
	return argc;
}

// Command registry
static command_t command_registry[MAX_COMMANDS];
static int command_count = 0;

// External variables
extern uint32_t tick;

void cmd_init(void) {
    command_count = 0;
    
    // Register all commands
    cmd_register("EXIT", cmd_exit, "Shutdown the system");
    cmd_register("PANIC", cmd_panic, "Trigger kernel panic for debugging");
    cmd_register("CLEAR", cmd_clear, "Clear the screen");
    cmd_register("DEBUG", cmd_debug, "Show detailed system debug information");
    cmd_register("UPTIME", cmd_uptime, "Show system uptime in ticks");
    cmd_register("LS", cmd_ls, "List files");
    cmd_register("CAT", cmd_cat, "Print file contents: CAT <filename>");
    cmd_register("RM", cmd_rm, "Remove a file: RM <filename>");
    cmd_register("WRITE", cmd_write, "Write a file: WRITE <filename> <data>");
    cmd_register("ALLOC", cmd_alloc, "Allocate memory: ALLOC <size>");
    cmd_register("ECHO", cmd_echo, "Echo input: ECHO <text>");
    cmd_register("HELP", cmd_help, "Show this help message");
    cmd_register("MKDIR", cmd_mkdir, "Create new directory: MKDIR <dirname>");
    cmd_register("CD", cmd_cd, "Change directory: CD <dirname>");
    cmd_register("REBOOT", cmd_reboot, "Reboots the system");

    current_dir = fs_root();
}

void cmd_register(const char *name, command_handler_t handler, const char *description) {
    if (command_count < MAX_COMMANDS) {
        command_registry[command_count].name = name;
        command_registry[command_count].handler = handler;
        command_registry[command_count].description = description;
        command_count++;
    }
}

void cmd_execute(char *input) {
	char *argv[MAX_ARGS];
	int argc = tokenize(input, argv, MAX_ARGS);

	if (argc == 0) {
		print_string("\ndust> ");
		return;
	}

	for (int i = 0; i < command_count; i++) {
		if (compare_string(argv[0], (char*)command_registry[i].name) == 0) {
			command_registry[i].handler(argc, argv);
			return;
		}
	}

	print_string("[-] Unknown command: ");
	print_string(argv[0]);
	print_string("\ndust> ");

}


void cmd_help(int argc, char *argv[]) {  
    if (argc > 1) {
	    for (int i = 0; i < command_count; i++) {
		    if (compare_string(argv[1], (char*)command_registry[i].name) == 0) {
			    printk((char*)command_registry[i].description);
			    printk("\ndust> ");
			    return;
		    }
	    }
	    printk("[-] Unknown command for HELP\n");
    } else {
	    print_string("[+] Available commands:\n");
	    for (int i = 0; i < command_count; i++) {
		    print_string("  ");
		    print_string((char*)command_registry[i].name);
		    print_string(" - ");
		    print_string((char*)command_registry[i].description);
		    print_string("\n");
		}
    }

    print_string("dust> ");
    
}

// Individual command implementations
void cmd_reboot(int argc, char *argv[]) {
	printk("[!] Attempting reboot...\n");
	while (port_byte_in(0x64) & 0x02) {}
	port_byte_out(0x64, 0xFE);
	asm volatile (
		"cli\n\t"
		"hlt\n\t"
	);
	printk("dust> "); // shouldn't print the prompt
}

void cmd_cd(int argc, char *argv[]) {
	if (argc < 2) {
		print_string("[-] Usage: CD <directory>\n");
	} else if (compare_string(argv[1], "..") == 0) {
		if (current_dir->parent) {
			current_dir = current_dir->parent;
		} else {
			print_string("[-] Already at root\n");
		}
	} else {
		fs_file_t* dir = fs_get_file(current_dir, argv[1]);
		if (!dir || dir->type != FS_DIR) {
			print_string("[-] Directory not found\n");
		} else {
			current_dir = dir;
		}
	}
	print_string("dust> ");
}

void cmd_ls(int argc, char *argv[]) {
    fs_list_files(current_dir, 0);
    print_string("dust> ");
}

void cmd_rm(int argc, char *argv[]) {
	if (argc > 1) {
		fs_file_t* file = fs_get_file(current_dir, argv[1]);
		if (file) {
			if (fs_rm_file(current_dir, argv[1]) == 0) {
				print_string("[+] File removed\n");
			} else {
				print_string("[-] Failed to remove file\n");
			}
		} else {
			print_string("[-] File not found\n");
		}
	}
	print_string("dust> ");
}


void cmd_cat(int argc, char *argv[]) {
	if (argc < 2) {
		print_string("[-] Usage: CAT <filename>\n");
		print_string("dust> ");
		return;
	}

	fs_file_t* file = fs_get_file(current_dir, argv[1]);
	if (!file) {
		print_string("[-] File not found\n");
	} else {
		if (file->type != FS_FILE) {
			print_string("[-] Not a file\n");
		} else {
			for (uint32_t i = 0; i < file->size; i++) {
				print_char(file->data[i]);
			}
			print_nl();
		}
	}
	print_string("dust> ");
}


void cmd_write(int argc, char *argv[]) {
	if (argc < 3) {
		printk("[-] Usage: WRITE <filename> <data>\n");
	} else {
		const char* filename = argv[1];
		char buffer[FS_MAX_FILESIZE];
		int offset = 0;
		
		for (int i = 0; argv[1][i]; i++) {
			if (argv[1][i] == '/') {
				print_string("[-] '/' not allowed in filename\n");
				printk("dust> ");
				return;
			}
		}

		for (int i = 2; i < argc; i ++) {
			int len = string_length(argv[i]);
			if (offset + len >= FS_MAX_FILESIZE) break;
			memory_copy((uint8_t*)argv[i], (uint8_t*)(buffer + offset), len);
			offset += len;
			if (i < argc -1) buffer[offset++] = ' ';
		}
		fs_write_file(current_dir, filename, (uint8_t*)buffer, offset);
	}
	printk("dust> ");
}

void cmd_mkdir(int argc, char *argv[]) {
	if (argc < 2) {
		print_string("[-] Usage: MKDIR <dirname>\n");
	} else {
		if (fs_create_dir(current_dir, argv[1])) {
			print_string("[+] Directory created\n");
		} else {
			print_string("[-] Failed to create directory\n");
		}
	}
	print_string("dust> ");
}

void cmd_exit(int argc, char *argv[]) {
    print_string("[!] Attempting Shutdown. Goodbye!\n");
    asm volatile (
        "mov $0x2000, %%eax\n\t" // shuts down qemu
        "mov $0x604, %%dx\n\t"
        "out %%ax, %%dx\n\t"
        "hlt\n\t" // fallback if it didn't work
        :
        :
        : "eax", "dx"
    );
}

void cmd_panic(int argc, char *argv[]) {
    panic("DEBUG");
}

void cmd_clear(int argc, char *argv[]) {
    clear_screen();
    print_string("dust> ");
}

void cmd_debug(int argc, char *argv[]) {
    print_string("[*] x86 Kernel Debug:\n");
    
    // tick count
    /*char tick_str[32] = {0};
    int_to_string(tick, tick_str);
    print_string("[+] Ticks: ");
    print_string(tick_str);
    print_string("\n");*/ // this would spam invalid opcode, so just do cmd_uptime instead
    char *_argv[] = { "UPTIME" };
    cmd_uptime(1, _argv); 
    
    // dynamic memory node size
    print_dynamic_node_size();
    
    // dynamic memory layout
    print_string("[+] Dynamic memory layout:\n");
    print_dynamic_mem();
    
    // total used memory
    print_dynamic_mem_summary();
    
    // cpu registers
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp;
    asm volatile("mov %%eax, %0" : "=r"(eax));
    asm volatile("mov %%ebx, %0" : "=r"(ebx));
    asm volatile("mov %%ecx, %0" : "=r"(ecx));
    asm volatile("mov %%edx, %0" : "=r"(edx));
    asm volatile("mov %%esi, %0" : "=r"(esi));
    asm volatile("mov %%edi, %0" : "=r"(edi));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));
    asm volatile("mov %%esp, %0" : "=r"(esp));
    
    char reg_str[32];
    print_string("[+] CPU Registers:\n");

    int_to_string(eax, reg_str);
    print_string("EAX = "); print_string(reg_str); print_string("\n");

    int_to_string(ebx, reg_str);
    print_string("EBX = "); print_string(reg_str); print_string("\n");

    int_to_string(ecx, reg_str);
    print_string("ECX = "); print_string(reg_str); print_string("\n");

    int_to_string(edx, reg_str);
    print_string("EDX = "); print_string(reg_str); print_string("\n");

    int_to_string(esi, reg_str);
    print_string("ESI = "); print_string(reg_str); print_string("\n");

    int_to_string(edi, reg_str);
    print_string("EDI = "); print_string(reg_str); print_string("\n");

    int_to_string(ebp, reg_str);
    print_string("EBP = "); print_string(reg_str); print_string("\n");

    int_to_string(esp, reg_str);
    print_string("ESP = "); print_string(reg_str); print_string("\n");

    register_syscall(0, test_syscall);
    trigger_syscall();

    print_string("[+] End of debug dump.\n");
    print_string("dust> ");
}

void cmd_uptime(int argc, char *argv[]) {
    char time_str[32];
    int_to_string(tick, time_str);
    print_string("[+] ");
    print_string(time_str);
    print_string("\ndust> ");
}

void cmd_alloc(int argc, char *argv[]) {
    if (argc > 1) {
	int n = atoi(argv[1]);
        if (n > 0) {
            alloc(n);
            print_string("[+] Allocation complete\n");
        } else {
            print_string("[-] Invalid size\n");
        }
    } else {
        print_string("[-] Usage: ALLOC <size>\n"); 
    }
    print_string("dust> ");
}

void cmd_echo(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
	    print_string(argv[i]);
	    if (i < argc - 1) print_string(" ");
    }
    print_string("\n");
    print_string("dust> ");
}
