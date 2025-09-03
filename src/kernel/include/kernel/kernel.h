#pragma once

#define OSVERSION "v0.0.2"

#include <kernel/util.h>
#include <kernel/panic.h>
#include <kernel/printk.h>
#include <kernel/syscall.h>
#include <kernel/cmd.h>
#include <kernel/fs.h>

void execute_command(char *input);
