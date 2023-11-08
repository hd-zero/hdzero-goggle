#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

bool     disk_insterted(void);
void     disk_dump(char* sPath);
bool     disk_mounted(char* sPath, uint32_t* mbTotal, uint32_t* mbAvail);
bool     disk_isMounted(char* sPath);
uint32_t disk_availableSize(char* sPath);
bool     disk_checkPath(char* sPath);
bool     disk_checkFile(char* sPath);
int      disk_countMovies(char* sPath, char* sPrefix, char* sExts[], int nExts, int nIndexLen);

typedef struct
{
    bool inserted;
    bool mounted;
    bool full;
} SdcardStatus_t;

void     disk_sdstat_create(char* sPath, bool bThread);
void     disk_sdstat_delete(void);
bool     disk_sdstat(uint32_t mbFull, SdcardStatus_t* sds);

#ifdef __cplusplus
}
#endif
