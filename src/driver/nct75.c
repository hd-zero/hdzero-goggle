#include "nct75.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#include <log/log.h>

#include "../core/common.hh"

sys_temperature_t g_temperature;
/*
int nct_read_temperature(nct_type_t type)
{
	char cmd[128];
	char   buf[128]; 
	FILE   *stream;  

    memset( buf, '\0', sizeof(buf) );//��ʼ��buf
    memset( cmd, '\0', sizeof(cmd) );//��ʼ��buf
	sprintf(cmd, "cat /sys/bus/iio/devices/iio:device%d/in_voltage0_raw", 
			((int)type) + 1);	
    stream = popen( cmd , "r" );
	fread( buf, sizeof(char), sizeof(buf),  stream);  
	pclose( stream ); 

	return atoi(buf);
}
*/
int nct_read_temperature(nct_type_t type)
{
	char  buf[128]; 
	FILE* fp;  
	int i;

	sprintf(buf, "/sys/bus/iio/devices/iio:device%d/in_voltage0_raw",  ((int)type) + 1);	
	fp = fopen(buf,"r");
	if(!fp) {
		static bool bFirst = true;
		char* sName[3] = {"Right","Top","Left"};
		if(bFirst) {
			LOGI("nct_read_temperature: [%s] side error.",sName[type]);
			bFirst = false;
		}
		return -1;
	}
	
	if( fgets (buf, 60, fp)!=NULL)
		i = atoi(buf);
	else 
		i = -1;
	fclose(fp);
	return i;
}
