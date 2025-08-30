#pragma once

#include <stdint.h>

// Command function pointer type
typedef void (*command_handler_t)(int argc, char *argv[]);

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
void cmd_help(int argc, char *argv[]);  // Fixed: added char *input parameter

// Individual command handlers
void cmd_exit(int argc, char *argv[]);
void cmd_ls(int argc, char *argv[]);
void cmd_cat(int argc, char *argv[]);
void cmd_write(int argc, char *argv[]);
void cmd_panic(int argc, char *argv[]);
void cmd_clear(int argc, char *argv[]);
void cmd_rm(int argc, char *argv[]);
void cmd_debug(int argc, char *argv[]);
void cmd_uptime(int argc, char *argv[]);
void cmd_alloc(int argc, char *argv[]);
void cmd_echo(int argc, char *argv[]);
void cmd_mkdir(int argc, char *argv[]);
void cmd_cd(int argc, char *argv[]);
//void cmd_mem(int argc, char *argv[]);

// External function declarations
extern void test_syscall(void);
extern void trigger_syscall(void);
extern void* alloc(int n);
