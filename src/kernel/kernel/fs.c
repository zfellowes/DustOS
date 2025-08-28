#include <kernel/fs.h>
#include <kernel/printk.h>
#include <kernel/util.h>
#include <kernel/mem.h>
#include <drivers/display.h>

static fs_t fs;

void fs_init() {
	fs.num_files = 0;

	const char* test_content = "This is a test file!";
	fs_write_file("HELLO.TXT", (uint8_t*)test_content, string_length((char*)test_content));

}

void fs_list_files() {
	for (uint32_t i = 0; i < fs.num_files; i++) {
		print_string("  ");
		print_string(fs.files[i].name);
		print_string("\n");
	}
}

int fs_rm_file(const char *name) {
	for (uint32_t i = 0; i < fs.num_files; i++) {
		if (compare_string((char*)fs.files[i].name, (char*)name) == 0) {
			for (uint32_t j = i; j < fs.num_files - 1; j++) {
				fs.files[j] = fs.files[j + 1];
			}
			fs.num_files--;
			return 0;
		}
	}
	return -1;
}

fs_file_t* fs_get_file(const char* name) {
	for (uint32_t i = 0; i < fs.num_files; i++) {
		if (compare_string((char*)fs.files[i].name, (char*)name) == 0) {
			return &fs.files[i];
		}
	}
	return NULL;
}

int fs_write_file(const char* name, uint8_t* data, uint32_t size) {
	if (fs.num_files >= FS_MAX_FILES) {
		print_string("[-] FS full\n");
		return -1;
	}
	if (size > FS_MAX_FILESIZE) {
		print_string("[-] File too large\n");
		return -1;
	}
	if (string_length(name) > FS_MAX_FILENAME - 1) {
		print_string("[-] Filename too long\n");
		return -1;
	}

	fs_file_t* file = &fs.files[fs.num_files++];
	memory_copy((uint8_t*)name, (uint8_t*)file->name, string_length(name) + 1);
	file->size = size;
	memory_copy(data, file->data, size);
	print_string("[+] File written\n");
	return 0;
}

