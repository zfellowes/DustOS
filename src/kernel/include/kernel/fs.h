#pragma once
#include <stdint.h>

#define FS_MAX_FILES 32
#define FS_MAX_FILENAME 32
#define FS_MAX_FILESIZE 1024
#define FS_MAX_CHILDREN 16

typedef enum {
	FS_FILE,
	FS_DIR
} fs_entry_type_t;

typedef struct fs_file {
	char name[FS_MAX_FILENAME];
	fs_entry_type_t type;
	uint32_t size;
	uint8_t data[FS_MAX_FILESIZE];

	uint32_t num_children;
	struct fs_file* children[FS_MAX_CHILDREN];

	struct fs_file* parent;
} fs_file_t;

typedef struct {
	fs_file_t* root;
} fs_t;

void fs_init();
void fs_list_files(fs_file_t* dir, int indent);
fs_file_t* fs_get_file(fs_file_t* dir, const char* name);
int fs_write_file(fs_file_t* dir, const char* name, uint8_t* data, uint32_t size);
int fs_rm_file(fs_file_t* dir, const char* name);
fs_file_t* fs_create_dir(fs_file_t* dir, const char* name);
fs_file_t* fs_root();
fs_file_t* fs_resolve_path(fs_file_t* start_dir, const char* path, char* out_filename);
