#pragma once

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

#include "record.h"

void conf_loadViParams(char* confFile, ViParams_t* viPara);
void conf_saveViParams(char* confFile, ViParams_t* viPara);

void conf_loadVencParams(char* confFile, VencParams_t* vePara);
void conf_saveVencParams(char* confFile, VencParams_t* vePara);

void conf_loadVencParamsForLive(char* confFile, VencParams_t* vePara);

void conf_loadAiParams(char* confFile, AiParams_t* aiPara);
void conf_saveAiParams(char* confFile, AiParams_t* aiPara);

void conf_loadAencParams(char* confFile, AencParams_t* aePara);
void conf_saveAencParams(char* confFile, AencParams_t* aePara);

void conf_loadRecordParams(char* confFile, RecordParams_t* recPara);
void conf_saveRecordParams(char* confFile, RecordParams_t* recPara);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */
