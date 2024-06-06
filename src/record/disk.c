#include "disk.h"

#include <stdint.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <libgen.h>

#include <log/log.h>

#include "log.h"

#define MAX_pathLEN   128
#define FILE_NAME_LEN 256


typedef struct
{
    bool inserted;
    bool mounted;
    bool updated;
    uint32_t total;
    uint32_t free;
    uint32_t avail;
    uint32_t tick;
    uint32_t tkUpdated;

    pthread_t threadId;
    bool      exit;

    char path[MAX_pathLEN];
} SdcardContext_t;

static SdcardContext_t sdContext;

static uint32_t get_tickCount(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

inline static uint32_t kscale(uint32_t b, uint32_t bs)
{
    return (b * (uint64_t) bs + 1024/2) >> 10;
}

bool disk_insterted(void)
{
    int file = -1;
    file = open( "/dev/mmcblk0", O_RDONLY);
    close(file);
    return (file>=0);
}

bool disk_mounted(char* sPath, uint32_t* mbTotal, uint32_t* mbAvail)
{
    struct statfs diskinfo;
    int ret = 0;

    memset(&diskinfo, 0, sizeof(diskinfo));
    ret = statfs(sPath, &diskinfo);
    if( ret == 0)
    {
        uint32_t total_size = 0;
        uint32_t avail_size = 0;

        total_size = kscale(diskinfo.f_blocks, diskinfo.f_bsize);
        total_size = total_size >> 10; //MB

        avail_size = kscale(diskinfo.f_bavail, diskinfo.f_bsize),
        avail_size = avail_size >> 10; //MB

        *mbAvail= avail_size;
        *mbTotal= total_size;

        return (diskinfo.f_type == 0x00004d44);
    }

    *mbAvail = 0;
    *mbTotal = 0;

    return false;
}

/***********************************************************
 * check the path
 * create the path if not exist
***********************************************************/
bool disk_isMounted(char* sPath)
{
    #if(defined(DISK_NFS))
    return true;
    #endif

    char line[255] = {0};
	FILE *fp = fopen("/proc/mounts", "r");

    if (NULL == fp) {
        return false;
    }

    while(fgets(line, sizeof(line), fp))
	{
		if (line[0] == '/' && (strstr(line, sPath) != NULL))
		{
			if( strstr(line,"rw,") == NULL)
			{
				LOGE("sd card is read only");
				fclose(fp);
				return false;
			}

			fclose(fp);
			fp = NULL;
			return true;
		}

        memset(line,'\0',sizeof(line));
        continue;
    }

    if(fp) {
        fclose(fp);
    }

    return false;
}

/***********************************************************
 * check the path
 * create the path if not exist
***********************************************************/
bool disk_checkPath(char* sPath)
{
	struct stat st;

	stat(sPath, &st);
	if(!S_ISDIR(st.st_mode))
	{
		char command[128];
		snprintf(command, sizeof(command), "mkdir -p %s && chmod -R 777 $(dirname %s)", sPath, sPath);
		int ret =  system(command);

		if (ret != 0) {
			return false;
		}

		memset(&st, 0, sizeof(st));
		stat(sPath, &st);
		if(!S_ISDIR(st.st_mode))
		{
			return false;
		}
	}

	return true;
}

/***********************************************************
 * check the file exist
***********************************************************/
bool disk_checkFile(char* sPath)
{
	struct stat st;

	if(sPath == NULL) {
        return false;
	}

	stat(sPath, &st);
	return S_ISREG(st.st_mode);
}

uint32_t disk_availableSize(char* sPath)
{
    uint32_t total_size = 0;
    uint32_t avail_size = 0;
    struct statfs diskinfo;

    if( statfs(sPath, &diskinfo) != -1) {
        //LOGD("blocks=%d, f_bsize=%d");

        total_size = kscale(diskinfo.f_blocks, diskinfo.f_bsize);
        total_size = total_size >> 10; //MB

        avail_size = kscale(diskinfo.f_bavail, diskinfo.f_bsize),
        avail_size = avail_size >> 10; //MB
    }
    else {
        //LOGD("failed");
        //avail_size = 100; //for debug only
    }

    return avail_size;
}

void disk_dump(char* sPath)
{
#define KB 1024.0       // 2^10
#define MB 1048576.0    // 2^20
#define GB 1073741824.0 // 2^30
    struct statfs diskInfo;

    statfs(sPath, &diskInfo);	// 设备挂载的节点
    uint64_t blocksize = diskInfo.f_bsize;                   // 每一个block里包含的字节数
    uint64_t totalsize = blocksize * diskInfo.f_blocks;      // 总的字节数，f_blocks为block的数目
    printf("blocks= %lld\n",diskInfo.f_blocks);
    printf("total = %lld B = %f KB = %f MB = %f GB\n",
            totalsize,
            totalsize / KB,
            totalsize / MB,
            totalsize / GB);

    uint64_t freeDisk = diskInfo.f_bfree * blocksize;       // 剩余空间的大小
    uint64_t availableDisk = diskInfo.f_bavail * blocksize; // 可用空间大小
    printf("free  = %f MB = %f GB\n"
           "avail = %f MB = %f GB\n",
            freeDisk / MB,
            freeDisk / GB,
            availableDisk / MB,
            availableDisk / GB);
}

int disk_countMovies(char* sPath, char* sPrefix, char* sExts[], int nExts, int nIndexLen)
{
    DIR* dp = opendir(sPath);
    if( dp == NULL ) {
        LOGE("opendir %s fail\n", sPath);
        return -1;
    }

    struct dirent *dirp;
    char sTemp[FILE_NAME_LEN];
    int  nExtLen = 0;
    int  nPrefix = strlen(sPrefix);
    int  nCount = 0;
    int  nIndex = 0;
    int  nIndexMax = -1;
    int  i;

    while( (dirp = readdir(dp)) != NULL ) {
        if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0){//exclude "." and ".."
            continue;
        }

        //LOGD("%s", dirp->d_name);

        int size = strlen(dirp->d_name);

        if( strstr(dirp->d_name, sPrefix) != dirp->d_name ) {
            continue;
        }

        for(i=0; i<nExts; i++) {
            nExtLen = strlen(sExts[i]);
            if( size < (nIndexLen + nPrefix + nExtLen) ) {
                continue;
            }
            if( strcmp((dirp->d_name + (size - nExtLen)), sExts[i]) == 0 ) {
                break;
            }
        }
        if( i >= nExts ) {
            continue;
        }

        strcpy(sTemp, sPath);
        strcat(sTemp, dirp->d_name);

        //LOGD("%s", sTemp);

        struct stat myStat;
        if( stat(sTemp, &myStat) < 0 ) {
            continue;
        }
        if( S_ISREG(myStat.st_mode) ) {
            memset(sTemp, 0, sizeof(sTemp));
            memcpy(sTemp, dirp->d_name+nPrefix, nIndexLen);
            nIndex = atoi(sTemp);
            nCount++;
            if( nIndex > nIndexMax ) {
                nIndexMax = nIndex;
            }
        }
#if(0)
        if( S_ISDIR(myStat.st_mode) ) {
            continue;
        }
        else{//link file and others
        }
#endif
    }

    return nIndexMax;
}

void sdcard_check(SdcardContext_t* sdstat, uint32_t tkNow)
{
	uint32_t mbTotal=0;
	uint32_t mbAvail=0;
	bool mbMounted = false;
	bool mbInserted = false;
	bool mbUpdated = false;
	bool mbSizeUpdated = false;

	mbInserted = disk_insterted();
    mbMounted = disk_mounted(sdstat->path, &mbTotal, &mbAvail);

    if(sdstat->inserted != mbInserted) {
        sdstat->inserted = mbInserted;
        mbUpdated = true;
    }

    if(sdstat->mounted != mbMounted) {
        sdstat->mounted = mbMounted;
        mbUpdated = true;
    }

    if(sdstat->total != mbTotal) {
        sdstat->total = mbTotal;
        mbUpdated = true;
    }

    if(sdstat->avail != mbAvail) {
        sdstat->avail = mbAvail;
        mbSizeUpdated = true;
    }

    if(sdstat->tkUpdated == 0) {
        mbUpdated = true;
    }

    if(mbUpdated) {
        LOGE("sdcard: %s\n", mbInserted ? "plugged" : "unplugged");
        LOGE("sdcard: %s\n", mbMounted ? "mounted" : "unmounted");
        LOGE("sdcard: %d/%d (MB)\n", mbAvail, mbTotal);
        disk_dump(sdstat->path);

        log_write(mbInserted?INFO:WARN, "sdcard: %s", mbInserted ? "plugged" : "unplugged");
        log_write(mbMounted ?INFO:WARN, "sdcard: %s", mbMounted ? "mounted" : "unmounted");
        log_write(mbMounted ?INFO:WARN, "sdcard: %d/%d (MB)\n", mbAvail, mbTotal);
        //LOGE("sdcard: logwrite %d, %d, %d\n", x, y, z);

        sdstat->tkUpdated = tkNow;
        sdstat->updated = mbUpdated;
        //LOGE("log_period: %d\n", log_period());
    }
    else if( mbSizeUpdated ) {
        if( (tkNow-sdstat->tkUpdated) >= log_period()*1000 ) {
            log_write(mbMounted ?INFO:WARN, "sdcard: %d/%d (MB)\n", mbAvail, mbTotal);
            LOGE("sdcard: %d/%d (MB)\n", mbAvail, mbTotal);
            sdstat->tkUpdated = tkNow;
        }
    }
}

static inline bool sdcard_process(SdcardContext_t* sdstat, uint32_t now)
{
    if( (now - sdstat->tick) >= 490 ) {
        sdcard_check(sdstat, now);
        sdstat->tick = now;
        return true;
    }

    return false;
}

static void* disk_sdstat_thread_process(void *arg)
{
    SdcardContext_t* sdstat = (SdcardContext_t*)arg;

    while( !sdstat->exit ) {
        sdcard_process(sdstat, get_tickCount());
        usleep(100 * 1000);
    }

    return NULL;
}

void disk_sdstat_create(char* sPath, bool bThread)
{
    if( bThread && (sdContext.threadId > 0) ) {
        return;
    }

    memset(&sdContext, 0, sizeof(sdContext));
    sdContext.exit = false;
    strncpy(sdContext.path, sPath, MAX_pathLEN);

    if( bThread ) {
        pthread_create(&sdContext.threadId, NULL, disk_sdstat_thread_process, (void *)&sdContext);
    }
}

void disk_sdstat_delete(void)
{
    if( sdContext.threadId <= 0 ) {
        return;
    }

    sdContext.exit = true;
    pthread_join(sdContext.threadId, NULL);
    sdContext.threadId = 0;
}

bool disk_sdstat(uint32_t mbFull, SdcardStatus_t* sds)
{
    if( sdContext.threadId <= 0 ) {
        sdcard_process(&sdContext, get_tickCount());
    }

    bool updated = sdContext.updated;
    sds->inserted = sdContext.inserted;
    sds->mounted = sdContext.mounted;
    if( sds->full != (sdContext.avail < mbFull)) {
        sds->full = (sdContext.avail < mbFull);
        updated = true;
    }
    else if( sdContext.updated ) {
        sdContext.updated = false;
    }

    return updated;
}
