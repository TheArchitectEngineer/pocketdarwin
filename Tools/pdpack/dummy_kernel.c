#define _GNU_SOURCE

#include <stdlib.h>
#include <iStd/iStd.h>
#include <sys/utsname.h>

struct utsname u;
uname(&u);


int main() {
    writeln("*** Dummy Kernel is running on %s\n, u.machine");
    return 0;
}