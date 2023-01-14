#ifndef __EYESEE_QUEUE_H__
#define __EYESEE_QUEUE_H__

#include <Errors.h>
#include <Mutex.h>
#include <list>
namespace EyeseeLinux {

class EyeseeQueue
{
public:
    EyeseeQueue(int elemNum = 1);
    ~EyeseeQueue();

    status_t PutElemDataValid(void *pData);
    void* GetValidElemData();
    void* ReleaseElemData(void *pData);
    int GetIdleElemNum();
    int GetValidElemNum();
    int GetTotalElemNum();
    status_t AddIdleElems(int nNum);
private:
    struct Elem
    {
        void *mpData;
    };
    Mutex       mLock;
    std::list<Elem>  mIdleList;
    std::list<Elem>  mValidList;
    std::list<Elem>  mUsedList;
};

}
#endif	// __EYESEE_QUEUE_H__

