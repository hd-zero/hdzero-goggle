#ifndef _IPCLINUX_PICTUREREGIONCALLBACK_H_
#define _IPCLINUX_PICTUREREGIONCALLBACK_H_

#include <mm_comm_region.h>
#include <list>

typedef struct
{
    RGN_TYPE_E mType;
    RECT_S mRect; /* overlay or cover win */
    union
    {
        struct
        {
            PIXEL_FORMAT_E mPixFmt;      //
            void *mBitmap;
            BOOL mbInvColEn;    //overlay color invert enable
            int mPriority;  //overlay:[0,VI_MAX_OVERLAY_NUM-1]
        } mOverlay;
        struct
        {
            int mChromaKey; //color value in mode cover. ARGB
            int mPriority;  //cover:[0,VI_MAX_COVER_NUM-1]
        } mCover;
    } mInfo;
}PictureRegionType;

namespace EyeseeLinux {

    class PictureRegionCallback
    {
    public:
        PictureRegionCallback(){}        
        virtual ~PictureRegionCallback(){}
        virtual void addPictureRegion(std::list<PictureRegionType> &rPictureRegionList) = 0;
        
    };

}




#endif
