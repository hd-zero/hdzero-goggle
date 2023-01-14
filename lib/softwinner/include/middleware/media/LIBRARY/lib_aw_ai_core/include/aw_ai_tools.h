#pragma once
#include "aw_ai_core_def.h"

#define MIN_DURATION	(0)
#define MAX_LEVEL		(100)

#define tic()	_tic(__FUNCTION__)
#define toc()	_toc(__FUNCTION__)

// ��ȡ�ļ�����
unsigned int AW_AI_API GetFileSize(const char *szPath);

// �ж��ļ��Ƿ����
int AW_AI_API IsFileExists(char *filename);

// �ж�YUV�ļ�
bool AW_AI_API IsYUV(AW_PIXEL_FORMAT_E eType);

// ������ʱ
void AW_AI_API _tic(const char* szFuncName);

// ������ʱ
void AW_AI_API _toc(const char* szFuncName);

// �򿪼�ʱ
void AW_AI_API TurnOnTime();

// �رռ�ʱ
void AW_AI_API TurnOffTime();

