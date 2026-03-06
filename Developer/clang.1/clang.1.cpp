#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

bool is_executable(const char* path) {
    struct stat st;
    return stat(path, &st) == 0 && (st.st_mode & S_IXUSR);
}

int main(int argc, char* argv[]) {
    const char* path_env = std::getenv("PATH");
    if (!path_env) {
        std::cerr << "PATH not set\n";
        return 1;
    }

    char self_path[PATH_MAX];
    if (!realpath(argv[0], self_path)) {
        perror("realpath failed");
        return 1;
    }

    char* path_copy = strdup(path_env);
    char* dir = strtok(path_copy, ":");

    while (dir) {
        std::string candidate = std::string(dir) + "/clang";

        char resolved[PATH_MAX];
        if (realpath(candidate.c_str(), resolved)) {
            if (strcmp(resolved, self_path) != 0 && is_executable(resolved)) {
                argv[0] = strdup(resolved);
                execv(resolved, argv);
                perror("execv failed");
                free(path_copy);
                return 1;
            }
        }

        dir = strtok(nullptr, ":");
    }

    std::cerr << "Real clang not found in PATH\n";
    free(path_copy);
    return 1;
}
