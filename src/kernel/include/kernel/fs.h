#pragma once
#include <stdint.h>

#define FS_MAX_FILES 32
#define FS_MAX_FILENAME 32
#define FS_MAX_FILESIZE 1024

typedef struct {
	char name[FS_MAX_FILENAME];
	uint32_t size;
	uint8_t data[FS_MAX_FILESIZE];
} fs_file_t;

typedef struct {
	uint32_t num_files;
	fs_file_t files[FS_MAX_FILES];
} fs_t;

void fs_init();
void fs_list_files();
fs_file_t* fs_get_file(const char* name);
int fs_write_file(const char* name, uint8_t* data, uint32_t size);

