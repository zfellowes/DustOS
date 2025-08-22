#pragma once

#include <stdint.h>

// Command function pointer type
typedef void (*command_handler_t)(char *input);

// Command structure
typedef struct {
    const char *name;
    command_handler_t handler;
    const char *description;
} command_t;

// Command registry functions
void cmd_init(void);
void cmd_register(const char *name, command_handler_t handler, const char *description);
void cmd_execute(char *input);
void cmd_help(char *input);  // Fixed: added char *input parameter

// Individual command handlers
void cmd_exit(char *input);
void cmd_panic(char *input);
void cmd_clear(char *input);
void cmd_info(char *input);
void cmd_debug(char *input);
void cmd_uptime(char *input);
void cmd_alloc(char *input);
void cmd_echo(char *input);

// External function declarations
extern void test_syscall(void);
extern void trigger_syscall(void);
extern void* alloc(int n);