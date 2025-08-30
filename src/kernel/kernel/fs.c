#include <kernel/fs.h>
#include <kernel/printk.h>
#include <kernel/util.h>
#include <kernel/mem.h>
#include <drivers/display.h>

static fs_t fs;

fs_file_t* fs_root() {
	return fs.root;
}

void fs_init() {
	fs.root = (fs_file_t*)mem_alloc(sizeof(fs_file_t));
	memory_copy((uint8_t*)"root", (uint8_t*)fs.root->name, string_length("root") + 1);
	fs.root->type = FS_DIR;
	fs.root->num_children = 0;

	const char* test_content = "This is a test file!";
	fs_write_file(fs.root, "HELLO.TXT", (uint8_t*)test_content, string_length((char*)test_content));
}

void fs_list_files(fs_file_t* dir, int indent) {
	if (!dir || dir->type != FS_DIR) return;

	for (uint32_t i = 0; i < dir->num_children; i++) {
		for (int j = 0; j < indent; j++) print_string("   ");
		if (dir->children[i]->type == FS_DIR) {
			print_string("[DIR]    ");
		} else {
			print_string("[FILE]   ");
		}
		print_string(dir->children[i]->name);
		print_nl();
	}
}

fs_file_t* fs_get_file(fs_file_t* dir, const char* name) {
	if (!dir || dir->type != FS_DIR) return NULL;

	for (uint32_t i = 0; i < dir->num_children; i++) {
		if (compare_string(dir->children[i]->name, name) == 0) {
			return dir->children[i];
		}
	}
	return NULL;
}

int fs_write_file(fs_file_t* dir, const char* name, uint8_t* data, uint32_t size) {
	if (!dir || dir->type != FS_DIR) return -1;

	// Check if file already exists, overwrite it
	fs_file_t* file = fs_get_file(dir, name);
	if (file) {
		if (size > FS_MAX_FILESIZE) {
			print_string("[-] File too large\n");
			return -1;
		}
		file->size = size;
		memory_copy(data, file->data, size);
		print_string("[+] File overwritten\n");
		return 0;
	}

	// Create new file
	if (dir->num_children >= FS_MAX_CHILDREN) {
		print_string("[-] Directory full\n");
		return -1;
	}
    
	if (size > FS_MAX_FILESIZE) {
		print_string("[-] File too large\n");
		return -1;
	}

	file = (fs_file_t*)mem_alloc(sizeof(fs_file_t));
	if (!file) {
		print_string("[-] Out of memory\n");
		return -1;
	}
	memory_copy((uint8_t*)name, (uint8_t*)file->name, string_length(name) + 1);
	file->type = FS_FILE;
	file->size = size;
	memory_copy(data, file->data, size);
	file->num_children = 0;
	file->parent = dir;
	dir->children[dir->num_children++] = file;

	print_string("[+] File written\n");
	return 0;
}


fs_file_t* fs_resolve_path(fs_file_t* start_dir, const char* path, char* out_filename) {
	fs_file_t* dir = start_dir;
	const char* p = path;
	char temp[FS_MAX_FILENAME];
	int idx = 0;

	while (*p) {
		if (*p == '/') {
			temp[idx] = '\0';
			fs_file_t* next = fs_get_file(dir, temp);
			if (!next || next->type != FS_DIR) {
				return NULL;
			}
			dir = next;
			idx = 0;
		} else {
			if (idx < FS_MAX_FILENAME - 1) temp[idx++] = *p;
		}
		p++;
	}
	temp[idx] = '\0';
	memory_copy((uint8_t*)temp, (uint8_t*)out_filename, string_length(temp) + 1);
	return dir;
}

int fs_rm_file(fs_file_t* dir, const char* name) {
	if (!dir || dir->type != FS_DIR) return -1;

	for (uint32_t i = 0; i < dir->num_children; i++) {
		if (compare_string(dir->children[i]->name, name) == 0) {
			mem_free(dir->children[i]);
			for (uint32_t j = i; j < dir->num_children - 1; j++) {
				dir->children[j] = dir->children[j + 1];
			}
			dir->num_children--;
			return 0;
		}
	}
	return -1;
}

fs_file_t* fs_create_dir(fs_file_t* dir, const char* name) {
	if (!dir || dir->type != FS_DIR) return NULL;
	if (dir->num_children >= FS_MAX_CHILDREN) {
		print_string("[-] Directory full\n");
		return NULL;
	}

	// Check for duplicate dir
	if (fs_get_file(dir, name)) {
		print_string("[-] Directory already exists\n");
		return NULL;
	}

	fs_file_t* new_dir = (fs_file_t*)mem_alloc(sizeof(fs_file_t));
	if (!new_dir) {
		print_string("[-] Out of memory\n");
		return NULL;
	}
	memory_copy((uint8_t*)name, (uint8_t*)new_dir->name, string_length(name) +1);
	new_dir->type = FS_DIR;
	new_dir->num_children = 0;
	new_dir->parent = dir;

	dir->children[dir->num_children++] = new_dir;
	return new_dir;
}

