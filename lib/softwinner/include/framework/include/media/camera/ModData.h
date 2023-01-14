#ifndef _AW_MOTION_DETECT_TYPE_H
#define _AW_MOTION_DETECT_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _MotionDetectResult_
{
	int nResult;
}MotionDetectResult;

typedef struct _MOTION_DETECT_ATTR_S
{
	int nSensitivity;
}MOTION_DETECT_ATTR_S;

#ifdef __cplusplus
}
#endif

#endif