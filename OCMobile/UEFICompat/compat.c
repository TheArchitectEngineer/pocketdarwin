/*
 * Minimal UEFI compatibility shim for Android ramdisk.
 *
 * Behavior:
 * - Runs in userspace on the device (early ramdisk/init context).
 * - Monitors /uefi for a payload named `OCMobile.efi` or `payload`.
 * - If a native executable (ELF) is present it will attempt to exec it.
 * - Logs events to /dev/kmsg when available (falls back to stdout).
 *
 * This is intentionally tiny: it's an attachable compatibility layer that
 * lets you ship a userspace helper into `boot.img`'s ramdisk and have the
 * device start it early. It does NOT implement the UEFI spec — instead it
 * provides a launch point for a userspace shim that implements any required
 * logic.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

static const char *default_payloads[] = {"/uefi/OCMobile.efi", "/uefi/payload", "/uefi/ocmobile", NULL};

int main(int argc, char **argv) {
    const char **p = default_payloads;
    FILE *log = fopen("/dev/kmsg", "w");
    if (!log) log = stdout;

    fprintf(log, "UEFICompat: started\n"); fflush(log);

    /* If argv[1] is provided, use it as payload path */
    const char *user_payload = (argc > 1) ? argv[1] : NULL;

    while (1) {
        const char *candidate = NULL;
        if (user_payload) candidate = user_payload;
        else {
            const char **q = p;
            while (*q) {
                struct stat st;
                if (stat(*q, &st) == 0) { candidate = *q; break; }
                q++;
            }
        }

        if (candidate) {
            fprintf(log, "UEFICompat: found payload %s, attempting exec\n", candidate); fflush(log);
            /* Try to exec the payload as a native program */
            execl(candidate, candidate, (char*)NULL);
            fprintf(log, "UEFICompat: exec failed errno=%d (%s)\n", errno, strerror(errno)); fflush(log);
            /* If exec fails, wait and retry — allow developer to replace payload */
            sleep(3);
        } else {
            sleep(1);
        }
    }

    return 0;
}
