
#ifndef _CHANNELREGIONINFO_H_
#define _CHANNELREGIONINFO_H_

//#include <errno.h>
#include <mm_comm_region.h>

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

typedef struct
{
    RGN_HANDLE mRgnHandle;  //[0,RGN_HANDLE_MAX)
    RGN_ATTR_S mRgnAttr;
    RGN_CHN_ATTR_S mRgnChnAttr;
    BOOL mbSetBmp;
    BITMAP_S mBmp;
    BOOL mbDraw;    //if draw
    struct list_head mList;
    struct list_head mTmpList;
} ChannelRegionInfo;

ChannelRegionInfo* ChannelRegionInfo_Construct();
void ChannelRegionInfo_Destruct(ChannelRegionInfo *pRegion);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* _CHANNELREGIONINFO_H_ */

