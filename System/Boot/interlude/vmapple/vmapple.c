#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <linux/loop.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static void usage(void) {
    fprintf(stderr,
        "darwin_vmapple \\\n"
        "  -kernel <mach_kernel> \\\n"
        "  -dtb <dtb> \\\n"
        "  -ram <MB> \\\n"
        "  -append <boot-args> \\\n"
        "  -rootfs <disk.img>\n");
    exit(1);
}

int mount_hfs_image(const char *image, const char *mountpoint) {
    int img_fd = -1, loop_fd = -1;
    char loopdev[64] = "/dev/loop0";

    img_fd = open(image, O_RDWR);
    if (img_fd < 0) {
        perror("open image");
        return -1;
    }

    /* find free loop device */
    for (int i = 0; i < 8; i++) {
        snprintf(loopdev, sizeof(loopdev), "/dev/loop%d", i);
        loop_fd = open(loopdev, O_RDWR);
        if (loop_fd >= 0)
            break;
    }

    if (loop_fd < 0) {
        perror("open loop");
        close(img_fd);
        return -1;
    }

    if (ioctl(loop_fd, LOOP_SET_FD, img_fd) < 0) {
        perror("LOOP_SET_FD");
        close(loop_fd);
        close(img_fd);
        return -1;
    }

    struct loop_info64 info = {0};
    info.lo_flags = LO_FLAGS_AUTOCLEAR;
    strcpy((char *)info.lo_file_name, image);
    ioctl(loop_fd, LOOP_SET_STATUS64, &info);

    if (mount(loopdev, mountpoint, "hfsplus", 0, "") < 0) {
        perror("mount hfsplus");
        ioctl(loop_fd, LOOP_CLR_FD, 0);
        close(loop_fd);
        close(img_fd);
        return -1;
    }

    close(loop_fd);
    close(img_fd);
    return 0;
}

int main(int argc, char **argv) {
    const char *kernel = NULL;
    const char *dtb = NULL;
    const char *ram = "2048";
    const char *append = "";
    const char *rootfs = NULL;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-kernel") && i + 1 < argc)
            kernel = argv[++i];
        else if (!strcmp(argv[i], "-dtb") && i + 1 < argc)
            dtb = argv[++i];
        else if (!strcmp(argv[i], "-ram") && i + 1 < argc)
            ram = argv[++i];
        else if (!strcmp(argv[i], "-append") && i + 1 < argc)
            append = argv[++i];
        else if (!strcmp(argv[i], "-rootfs") && i + 1 < argc)
            rootfs = argv[++i];
        else
            usage();
    }

    if (!kernel || !dtb || !rootfs)
        usage();

    char mem_arg[64];
    snprintf(mem_arg, sizeof(mem_arg), "%s", ram);

    char drive_arg[512];
    snprintf(drive_arg, sizeof(drive_arg),
        "file=%s,if=virtio,format=raw", rootfs);

    char *qemu_argv[] = {
        "qemu-system-aarch64",
        "-machine", "vmapple",
        "-cpu", "max",
        "-m", mem_arg,
        "-kernel", (char *)kernel,
        "-dtb", (char *)dtb,
        "-append", (char *)append,
        "-drive", drive_arg,
        "-serial", "mon:stdio",
        "-display", "none",
        "-no-reboot",
        NULL
    };

    execvp(qemu_argv[0], qemu_argv);
    perror("execvp failed");
    return 1;
}

