#ifndef __CONFIG_PARSER_H__
#define __CONFIG_PARSER_H__

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */


#include "iniparser.h"
#include "plat_type.h"


typedef struct confparser_s
{
	dictionary *pDict;
}CONFPARSER_S, *PTR_CONFPARSER_S;


int createConfParser(const char *conf_path, CONFPARSER_S *pCfg);
void destroyConfParser(CONFPARSER_S *pCfg);

int GetConfParaInt(CONFPARSER_S *pCfg, const char * key,int notfound);
unsigned int GetConfParaUInt(CONFPARSER_S *pCfg, const char * key,int notfound);
const char *GetConfParaString(CONFPARSER_S *pCfg, const char *key, const char *def);
double GetConfParaDouble(CONFPARSER_S *pCfg, const char *key, double notfound);
BOOL GetConfParaBoolean(CONFPARSER_S *pCfg, const char *key, int notfound);



#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif //__CONFIG_PARSER_H__

