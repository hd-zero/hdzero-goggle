/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : CMediaBuffer.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/01/24
  Last Modified :
  Description   : memory block struct
  Function List :
  History       :
******************************************************************************/
#ifndef _VRENCBUFFER_H_
#define _VRENCBUFFER_H_


namespace EyeseeLinux {

class VREncBuffer
{
public:
	int total_size;
	int stream_type; /* RawPacketType; 0:video, 1:audio data, 128:video data header, 129:audio data header */
	int data_size;
	long long pts;  //unit: different muxer has different unit, raw muxer is us, ts muxer is ms.
    //for video frame.
    int CurrQp;
    int avQp;
	int nGopIndex;      //index of gop
	int nFrameIndex;    //index of current frame in gop.
	int nTotalIndex;    //index of current frame in whole encoded frames.

    VREncBuffer(){};
    VREncBuffer(int size);
    ~VREncBuffer();   

    VREncBuffer(const VREncBuffer& ref);
    VREncBuffer& operator= (const VREncBuffer& ref);
    VREncBuffer(VREncBuffer&& rRef);
    VREncBuffer& operator=(VREncBuffer&& rRef);

    void* getPointer();
    int getSize();
    void* reallocBuffer(int size);
    void freeBuffer();

private:
    void    *pMem;
    int     mSize;
};

}; /* namespace EyeseeLinux */

#endif  /* _VRENCBUFFER_H_ */

