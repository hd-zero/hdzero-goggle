#ifndef __FS_WRITER_H__
#define __FS_WRITER_H__

#include <errno.h>
#include <cedarx_stream.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum tag_FSWRITEMODE {
    FSWRITEMODE_CACHETHREAD = 0,
    FSWRITEMODE_SIMPLECACHE,
    FSWRITEMODE_DIRECT,
}FSWRITEMODE;

typedef struct FsCacheMemInfo
{
    char              *mpCache;
    unsigned int             mCacheSize;
}FsCacheMemInfo;

typedef struct tag_FsWriter FsWriter;
typedef struct tag_FsWriter
{
    FSWRITEMODE mMode;
    ssize_t (*fsWrite)(FsWriter *thiz, const char *buf, size_t size);
    int (*fsSeek)(FsWriter *thiz, int64_t nOffset, int fromWhere);
    int64_t (*fsTell)(FsWriter *thiz);
    int (*fsTruncate)(FsWriter *thiz, int64_t nLength);
    int (*fsFlush)(FsWriter *thiz);
    void *mPriv;
}FsWriter;
FsWriter* createFsWriter(FSWRITEMODE mode, struct cdx_stream_info *pStream, char *pCache, unsigned int nCacheSize, unsigned int vCodec);
int destroyFsWriter(FsWriter *thiz);

extern ssize_t fileWriter(struct cdx_stream_info *pStream, const char *buffer, size_t size);

#if defined(__cplusplus)
}
#endif

#endif
