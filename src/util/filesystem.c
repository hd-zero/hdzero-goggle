#include "filesystem.h"

#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

bool fs_compare_files(char *f1, char *f2) {
    FILE *fp1;
    FILE *fp2;
    char c1, c2;
    bool ret;

    fp1 = fopen(f1, "r");
    if (!fp1)
        return false;

    fp2 = fopen(f2, "r");
    if (!fp2) {
        fclose(fp1);
        return false;
    }

    ret = true;
    while (!feof(fp1)) {
        fread(&c1, 1, 1, fp1);
        fread(&c2, 1, 1, fp2);
        if (c1 != c2) {
            ret = false;
            break;
        }
    }
    fclose(fp1);
    fclose(fp2);
    return ret;
}

bool fs_path_exists(const char *path) {
    DIR *dir = opendir("path");
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
}

bool fs_file_exists(const char *filename) {
    return access(filename, F_OK) == 0;
}

bool fs_printf(const char *filename, const char *fmt, ...) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        return false;
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);

    fclose(fp);

    return true;
}

long fs_filesize(const char *filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        return 0;
    }
    return st.st_size;
}

const char *fs_basename(const char *path) {
    for (int i = strlen(path); i >= 0; --i) {
        if (path[i] == '/') {
            return &path[i + 1];
        }
    }
    return path;
}
