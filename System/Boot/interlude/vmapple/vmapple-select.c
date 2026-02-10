#define _GNU_SOURCE
#include <dirent.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BASE_PATH "/external_sd"
#define MAX_ENTRIES 16
#define MAX_PATH 512

struct entry {
    char name[MAX_PATH];
    char kernel[MAX_PATH];
    char rootfs[MAX_PATH];
};

static struct entry entries[MAX_ENTRIES];
static int entry_count = 0;

/* ---------- helpers ---------- */

static int ends_with(const char *s, const char *suffix) {
    size_t ls = strlen(s), lf = strlen(suffix);
    return (ls >= lf && strcmp(s + ls - lf, suffix) == 0);
}

static int file_exists(const char *p) {
    struct stat st;
    return stat(p, &st) == 0;
}

static void join(char *out, const char *a, const char *b) {
    snprintf(out, MAX_PATH, "%s/%s", a, b);
}

/* ---------- scan Disks/ for rootfs ---------- */

static int find_rootfs(const char *sysdir, char *out) {
    char disks[MAX_PATH];
    DIR *d;
    struct dirent *e;

    join(disks, sysdir, "Contents/Disks");
    d = opendir(disks);
    if (!d) return 0;

    while ((e = readdir(d))) {
        if (ends_with(e->d_name, ".raw") || ends_with(e->d_name, ".img")) {
            join(out, disks, e->d_name);
            closedir(d);
            return 1;
        }
    }

    closedir(d);
    return 0;
}

/* ---------- locate kernel ---------- */

static int find_kernel(const char *sysdir, char *out) {
    static const char *candidates[] = {
        "Contents/kernel/mach_kernel",
        "Contents/kernel/kernel",
        "kernel/mach_kernel",
        "kernel/kernel",
        NULL
    };

    for (int i = 0; candidates[i]; i++) {
        char p[MAX_PATH];
        join(p, sysdir, candidates[i]);
        if (file_exists(p)) {
            strcpy(out, p);
            return 1;
        }
    }
    return 0;
}

/* ---------- scan BASE_PATH for *.System ---------- */

static void scan_systems(void) {
    DIR *d = opendir(BASE_PATH);
    struct dirent *e;

    if (!d) return;

    while ((e = readdir(d)) && entry_count < MAX_ENTRIES) {
        if (!ends_with(e->d_name, ".System"))
            continue;

        char sysdir[MAX_PATH];
        join(sysdir, BASE_PATH, e->d_name);

        struct entry *ent = &entries[entry_count];

        if (!find_kernel(sysdir, ent->kernel))
            continue;
        if (!find_rootfs(sysdir, ent->rootfs))
            continue;

        strncpy(ent->name, e->d_name, sizeof(ent->name) - 1);
        entry_count++;
    }

    closedir(d);
}

/* ---------- UI ---------- */

static void draw_menu(int sel) {
    printf("\033[2J\033[H");
    printf("vmapple-select\n\n");

    for (int i = 0; i < entry_count; i++) {
        printf("%c %s\n", (i == sel ? '>' : ' '), entries[i].name);
    }

    printf("\nVolUp / VolDown to move, Power to boot\n");
    fflush(stdout);
}

/* ---------- main ---------- */

int main(void) {
    int input;
    struct input_event ev;
    int sel = 0;

    scan_systems();

    if (entry_count == 0) {
        printf("No .System bundles found.\n");
        sleep(5);
        return 1;
    }

    input = open("/dev/input/event0", O_RDONLY);
    if (input < 0) {
        perror("open input");
        return 1;
    }

    while (1) {
        draw_menu(sel);

        if (read(input, &ev, sizeof(ev)) != sizeof(ev))
            continue;

        if (ev.type == EV_KEY && ev.value == 1) {
            if (ev.code == KEY_VOLUMEUP && sel > 0)
                sel--;
            else if (ev.code == KEY_VOLUMEDOWN && sel < entry_count - 1)
                sel++;
            else if (ev.code == KEY_POWER)
                break;
        }
    }

    close(input);

    printf("Booting %s...\n", entries[sel].name);
    sleep(1);

    execl(
        "/bin/vmapple",
        "darwin_vmapple",
        "-kernel", entries[sel].kernel,
        "-rootfs", entries[sel].rootfs,
        "-append", "-v serial=3",
        NULL
    );

    perror("exec");
    return 1;
}

