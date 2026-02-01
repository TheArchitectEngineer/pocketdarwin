#include "bootstd.h"
#include <stdarg.h>
#include "Platform/plist/plist.h"

#define MAX_ENTRIES 10

struct boot_entry {
    char name[64];
    int type; // 0 partition, 1 image
    union {
        int part_num;
        char image_path[64];
    };
};

struct boot_entry entries[MAX_ENTRIES];
int num_entries = 0;

void add_entry(const char *name, int type, ...) {
    if (num_entries >= MAX_ENTRIES) return;
    strcpy(entries[num_entries].name, name);
    entries[num_entries].type = type;
    va_list args;
    va_start(args, type);
    if (type == 0) {
        entries[num_entries].part_num = va_arg(args, int);
    } else {
        strcpy(entries[num_entries].image_path, va_arg(args, const char*));
    }
    va_end(args);
    num_entries++;
}

void scan_boot_options() {
    // Try to read from config.plist on SD
    fs_t fs;
    if (fs_mount(&fs, "/dev/mmcblk1") == 0) {
        file_t file;
        if (fs_open(&fs, "/OCM/config.plist", &file) == 0) {
            char buf[4096];
            size_t size = fs_read(&file, buf, sizeof(buf));
            fs_close(&file);
            plist_dict_t dict;
            if (plist_parse_xml(buf, size, &dict) == 0) {
                // Parse entries
                for (int i = 0; i < dict.count; i++) {
                    plist_entry_t *e = &dict.entries[i];
                    if (e->type == PLIST_STRING && strncmp(e->key, "option", 6) == 0) {
                        const char *val = e->value.string;
                        if (strstr(val, "partition")) {
                            // Extract number
                            const char *p = strstr(val, "partition ");
                            if (p) {
                                int part = atoi(p + 10);
                                add_entry(val, 0, part);
                            }
                        } else if (strstr(val, "image") || strstr(val, ".raw")) {
                            // Assume the path is after "Boot from "
                            const char *path = strstr(val, "/");
                            if (path) {
                                add_entry(val, 1, path);
                            }
                        }
                    }
                }
            }
        }
    }
    // Fallback
    if (num_entries == 0) {
        add_entry("Boot Darwin from partition 1", 0, 1);
        add_entry("Boot from SD image", 1, "/dev/mmcblk1/boot.raw");
    }
}

void boot_menu_summon() {
    console_init();
    input_init();
    scan_boot_options();
    int selected = 0;
    while (1) {
        console_clear();
        puts("OCM Boot Menu\n\n");
        for (int i = 0; i < num_entries; i++) {
            if (i == selected) puts("> ");
            else puts("  ");
            puts(entries[i].name);
            puts("\n");
        }
        boot_key_t k = input_get();
        if (k == KEY_UP) {
            selected = (selected - 1 + num_entries) % num_entries;
        } else if (k == KEY_DOWN) {
            selected = (selected + 1) % num_entries;
        } else if (k == KEY_SELECT) {
            boot_selected(selected);
            break;
        }
    }
}

void boot_selected(int idx) {
    struct boot_entry *e = &entries[idx];
    puts("Booting ");
    puts(e->name);
    puts("\n");
    if (e->type == 0) {
        load_kernel_from_partition(e->part_num);
    } else {
        load_kernel_from_image(e->image_path);
    }
}

void load_kernel_from_partition(int part_num) {
    puts("Loading from partition ");
    printf("%d\n", part_num);
    // Assume SD card /dev/mmcblk1
    block_dev_t dev;
    if (block_open(&dev, "/dev/mmcblk1") != 0) {
        panic("Failed to open SD card");
    }
    // Use GPT to find partition LBA
    // Stub: assume partition 1 starts at LBA 2048
    u64 part_lba = 2048;
    // Assume kernel at part_lba + 1
    void *kernel_addr = (void*)0x80000000ULL;
    size_t sectors = 20480;
    if (block_read(&dev, part_lba + 1, kernel_addr, sectors) != sectors) {
        panic("Failed to read kernel");
    }
    puts("Kernel loaded from partition\n");
    patch_kernel(kernel_addr, sectors * 512);
    load_kexts(kernel_addr);
    puts("Jumping to kernel\n");
    ((void(*)(void))kernel_addr)();
}

void load_kernel_from_image(const char *path) {
    puts("Loading from image ");
    puts(path);
    puts("\n");
    if (memcmp(path, "/dev/", 5) == 0) {
        // Block device
        block_dev_t dev;
        if (block_open(&dev, path) != 0) {
            panic("Failed to open block device");
        }
        // Assume kernel at LBA 1, size 20480 sectors (10MB)
        void *kernel_addr = (void*)0x80000000ULL;
        size_t sectors = 20480;
        if (block_read(&dev, 1, kernel_addr, sectors) != sectors) {
            panic("Failed to read kernel");
        }
        puts("Kernel loaded from block device\n");
        patch_kernel(kernel_addr, sectors * 512);
        load_kexts(kernel_addr);
        puts("Jumping to kernel\n");
        ((void(*)(void))kernel_addr)();
    } else {
        // File on FS
        // Stub
        panic("File loading not implemented");
    }
}