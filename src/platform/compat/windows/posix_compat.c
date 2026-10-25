/* POSIX functions mingw/Windows lacks, for the Windows emulator build only.
   Compiled solely for WIN32 (see CMakeLists.txt). getline/scandir back real
   emulator paths (settings parsing, playback listing); sync is a best-effort
   flush; loader_port_debug_print gives the (weak-in-lib) esp-loader symbol a
   strong definition the mingw linker will resolve. */
#if defined(EMULATOR_BUILD) && defined(_WIN32)

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    if (!lineptr || !n || !stream)
        return -1;
    if (*lineptr == NULL || *n == 0) {
        *n = 128;
        *lineptr = (char *)malloc(*n);
        if (*lineptr == NULL)
            return -1;
    }
    size_t pos = 0;
    int c;
    while ((c = fgetc(stream)) != EOF) {
        if (pos + 1 >= *n) {
            size_t nn = *n * 2;
            char *np = (char *)realloc(*lineptr, nn);
            if (np == NULL)
                return -1;
            *lineptr = np;
            *n = nn;
        }
        (*lineptr)[pos++] = (char)c;
        if (c == '\n')
            break;
    }
    if (pos == 0 && c == EOF)
        return -1;
    (*lineptr)[pos] = '\0';
    return (ssize_t)pos;
}

int scandir(const char *dirp, struct dirent ***namelist,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **)) {
    DIR *d = opendir(dirp);
    if (d == NULL)
        return -1;
    struct dirent **list = NULL;
    size_t count = 0, cap = 0;
    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (filter != NULL && filter(ent) == 0)
            continue;
        if (count >= cap) {
            size_t ncap = cap ? cap * 2 : 16;
            struct dirent **nl = (struct dirent **)realloc(list, ncap * sizeof(*list));
            if (nl == NULL)
                break;
            list = nl;
            cap = ncap;
        }
        struct dirent *copy = (struct dirent *)malloc(sizeof(struct dirent));
        if (copy == NULL)
            break;
        memcpy(copy, ent, sizeof(struct dirent));
        list[count++] = copy;
    }
    closedir(d);
    if (compar != NULL && count > 1)
        qsort(list, count, sizeof(*list), (int (*)(const void *, const void *))compar);
    *namelist = list;
    return (int)count;
}

void sync(void) {
    _flushall(); // best-effort: flush all open C streams
}

// esp-loader declares this weak; give it a strong (silent) definition so the
// mingw linker resolves it. The ESP32 flasher is device-only and inert here.
void loader_port_debug_print(const char *str) {
    (void)str;
}

#endif // EMULATOR_BUILD && _WIN32
