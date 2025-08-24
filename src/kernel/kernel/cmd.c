#include <kernel/cmd.h>
#include <kernel/util.h>
#include <kernel/panic.h>
#include <kernel/printk.h>
#include <kernel/mem.h>
#include <cpu/timer.h>
#include <drivers/display.h>
#include <kernel/syscall.h>
#include <kernel/fs.h>

// Maximum number of commands in registry
#define MAX_COMMANDS 16

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
    cmd_register("INFO", cmd_info, "Display system information");
    cmd_register("DEBUG", cmd_debug, "Show detailed system debug information");
    cmd_register("UPTIME", cmd_uptime, "Show system uptime in ticks");
    cmd_register("LS", cmd_ls, "List files");
    cmd_register("CAT", cmd_cat, "Print file contents: CAT <filename>");
    cmd_register("RM", cmd_rm, "Remove a file: RM <filename>");
    cmd_register("WRITE", cmd_write, "Write a file: WRITE <filename> <data>");
    cmd_register("ALLOC", cmd_alloc, "Allocate memory: ALLOC <size>");
    cmd_register("ECHO", cmd_echo, "Echo input: ECHO <text>");
    cmd_register("HELP", cmd_help, "Show this help message");
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
    // Handle empty input
    if (compare_string(input, "") == 0) {
        print_string("\ndust> ");
        return;
    }
    
    // Try to find and execute command
    for (int i = 0; i < command_count; i++) {
        // Cast const char* to char* to match compare_string signature
        if (compare_string(input, (char*)command_registry[i].name) == 0) {
            command_registry[i].handler(input);
            return;
        }
    }
    
    // Check for commands with arguments (like ALLOC and ECHO)
    if (starts_with(input, "ALLOC")) {
        cmd_alloc(input);
        return;
    }
    
    if (starts_with(input, "ECHO")) {
        cmd_echo(input);
        return;
    }

    if (starts_with(input, "CAT")) {
	    cmd_cat(input);
	    return;
    }

    if (starts_with(input, "RM")) {
	    cmd_rm(input);
	    return;
    }

    if (starts_with(input, "WRITE")) {
	    cmd_write(input);
	    return;
    }

    if (starts_with(input, "HELP")) {
	    cmd_help(input);
	    return;
    }
    
    // Unknown command
    print_string("[-] Unknown command: ");
    print_string(input);
    print_string("\ndust> ");
}

void cmd_help(char *input) {  // Fixed: added char *input parameter
    char* arg = input + 5;
    for (int i = 0; i < command_count; i++) { // get help on a specific command
        if (compare_string(arg, (char*)command_registry[i].name) == 0) {
            printk((char*)command_registry[i].description);
	    printk("\ndust> ");
            return;
        }
    }
    print_string("[+] Available commands:\n");
    for (int i = 0; i < command_count; i++) {
        print_string("  ");
        print_string((char*)command_registry[i].name);
        print_string(" - ");
        print_string((char*)command_registry[i].description);
        print_string("\n");
    }
    print_string("dust> ");
}

// Individual command implementations
void cmd_ls(char *input) {
    fs_list_files();
    print_string("dust> ");
}

void cmd_rm(char *input) {
	char* arg = input + 3;
	fs_file_t* file = fs_get_file(arg);
	if (file) {
		if (fs_rm_file(arg) == 0) {
			print_string("[+] File removed\n");
		} else {
			print_string("[-] Failed to remove file\n");
		}
	} else {
		print_string("[-] File not found\n");
	}
	print_string("dust> ");
}


void cmd_cat(char *input) {
    char* arg = input + 4; // skip "CAT "
    fs_file_t* file = fs_get_file(arg);
    if (file) {
        for (uint32_t i = 0; i < file->size; i++) {
            print_char(file->data[i]);
        }
        print_nl();
    } else {
        print_string("[-] File not found\n");
    }
    print_string("dust> ");
}

void cmd_write(char *input) {
    char filename[4] = {0}; // FIXME setup a proper kernel stack, these buffers are way too small.
    char content[8] = {0};

    // parse: WRITE <filename> <content>
    int i = 0;
    int j = 0;
    char* arg = input + 6;
    while (*arg && *arg != ' ') filename[i++] = *arg++;
    if (*arg == ' ') arg++;
    while (*arg) content[j++] = *arg++;

    fs_write_file(filename, (uint8_t*)content, j);
    print_string("[+] File written\n");
    print_string("dust> ");
}


void cmd_exit(char *input) {
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

void cmd_panic(char *input) {
    panic("DEBUG");
}

void cmd_clear(char *input) {
    clear_screen();
    print_string("dust> ");
}

void cmd_info(char *input) {
    print_string("[+] DustOS v0.0.1 - Kernel loaded\n");
    print_string("dust> ");
}

void cmd_debug(char *input) {
    print_string("[*] x86 Kernel Debug:\n");
    
    // tick count
    /*char tick_str[32] = {0};
    int_to_string(tick, tick_str);
    print_string("[+] Ticks: ");
    print_string(tick_str);
    print_string("\n");*/ // this would spam invalid opcode, so just do cmd_uptime instead
    cmd_uptime(""); // parse no arguments
    
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

void cmd_uptime(char *input) {
    char time_str[32];
    int_to_string(tick, time_str);
    print_string("[+] ");
    print_string(time_str);
    print_string("\ndust> ");
}

void cmd_alloc(char *input) {
    int n = 0;
    char *arg = input + 6;
    if (*arg) {
        n = atoi(arg);
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

void cmd_echo(char *input) {
    print_string(input + 5);
    print_string("\n");
    print_string("dust> ");
}
