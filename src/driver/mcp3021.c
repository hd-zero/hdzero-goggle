#include "mcp3021.h"

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

sys_battery_t g_battery;

/*
int mcp_read_vatage(void)
{
   FILE   *stream;  
   char   buf[128]; 
      
    memset( buf, '\0', sizeof(buf) );//��ʼ��buf
    stream = popen( "cat /sys/bus/iio/devices/iio:device0/in_voltage0_raw" , "r" );
   fread( buf, sizeof(char), sizeof(buf),  stream);  
   pclose( stream ); 

	return atoi(buf);
}
*/
int mcp_read_vatage(void)
{
	char  buf[128]; 
	FILE* fp;  
	int i;

	fp = fopen("/sys/bus/iio/devices/iio:device0/in_voltage0_raw","r");
	if(!fp) {
      static bool bFirst = true;
      if(bFirst) {
		   LOGE("mcp_read_vatage: open error.");
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

int mcp_detect_type(void)
{
	int v = mcp_read_vatage();
	return (v*10/1000/42+1);
}
