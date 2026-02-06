#include "InterludeIO.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <termios.h>
#include <unistd.h>

static int open_usb_serial(const char *preferred_path) {
    if (preferred_path && preferred_path[0] != '\0') {
        int fd = open(preferred_path, O_RDWR | O_NOCTTY | O_CLOEXEC);
        if (fd >= 0) {
            return fd;
        }
    }

    for (int i = 0; i < 8; ++i) {
        char path[64];
        snprintf(path, sizeof(path), "/dev/ttyACM%d", i);
        int fd = open(path, O_RDWR | O_NOCTTY | O_CLOEXEC);
        if (fd >= 0) {
            return fd;
        }
    }

    DIR *dir = opendir("/dev");
    if (!dir) {
        return -1;
    }
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strncmp(ent->d_name, "ttyACM", 6) == 0) {
            char path[128];
            snprintf(path, sizeof(path), "/dev/%s", ent->d_name);
            int fd = open(path, O_RDWR | O_NOCTTY | O_CLOEXEC);
            if (fd >= 0) {
                closedir(dir);
                return fd;
            }
        }
    }
    closedir(dir);
    return -1;
}

static void configure_serial(int fd) {
    struct termios tio;
    if (tcgetattr(fd, &tio) != 0) {
        return;
    }
    cfmakeraw(&tio);
    cfsetispeed(&tio, B115200);
    cfsetospeed(&tio, B115200);
    tio.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &tio);
}

bool interlude_attach_usb_serial(const char *preferred_path) {
    int fd = open_usb_serial(preferred_path);
    if (fd < 0) {
        return false;
    }

    configure_serial(fd);

    if (dup2(fd, STDOUT_FILENO) < 0) {
        close(fd);
        return false;
    }
    if (dup2(fd, STDERR_FILENO) < 0) {
        close(fd);
        return false;
    }

    if (fd > STDERR_FILENO) {
        close(fd);
    }

    printf("[interlude] USB serial attached (CDC-ACM).\n");
    fflush(stdout);
    return true;
}

bool interlude_init_framebuffer(void) {
    int fd = open("/dev/fb0", O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        return false;
    }

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) != 0) {
        close(fd);
        return false;
    }
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) != 0) {
        close(fd);
        return false;
    }

    size_t size = finfo.smem_len;
    void *fb = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fb == MAP_FAILED) {
        close(fd);
        return false;
    }

    memset(fb, 0, size);
    msync(fb, size, MS_SYNC);

    munmap(fb, size);
    close(fd);

    return true;
}
