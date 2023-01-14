/******************************************************************************
  Copyright (C), 2001-2016, Allwinner Tech. Co., Ltd.
 ******************************************************************************
  File Name     : CMediaMemory.h
  Version       : Initial Draft
  Author        : Allwinner BU3-PD2 Team
  Created       : 2017/01/24
  Last Modified :
  Description   : memory block struct
  Function List :
  History       :
******************************************************************************/
#ifndef _CMEDIAMEMORY_H_
#define _CMEDIAMEMORY_H_

namespace EyeseeLinux {

class CMediaMemory
{
public:
    CMediaMemory();
    CMediaMemory(int size);
    //copy constructor
    CMediaMemory(const CMediaMemory& ref);
    //copy assignment
    CMediaMemory& operator= (const CMediaMemory& ref);
    //move constructor
    CMediaMemory(CMediaMemory&& rRef);
    //move assignment
    CMediaMemory& operator=(CMediaMemory&& rRef);
    ~CMediaMemory();    

    void* getPointer() const;
    int getSize() const;

private:
    void    *pMem;
    int     mSize;
};

}; /* namespace EyeseeLinux */

#endif  /* _CMEDIAMEMORY_H_ */

