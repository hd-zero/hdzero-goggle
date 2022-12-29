#ifndef __SELF_TEST_H__
#define __SELF_TEST_H__

#include <stdio.h>

#define LOG_FILE    "/mnt/extsd/self_test.txt"

extern FILE* log_file;

void self_test();
#endif