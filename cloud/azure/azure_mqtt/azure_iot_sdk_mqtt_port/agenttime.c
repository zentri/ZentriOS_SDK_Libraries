// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/* This is a template file used for porting */

/* TODO: If all the below adapters call the standard C functions, please simply use the agentime.c that already exists. */

#include <time.h>
#include "zos.h"
#include "azure_c_shared_utility/agenttime.h"

time_t get_time(time_t* p)
{
    uint64_t time_buffer;
    uint64_t *ptr = (p != NULL) ? (uint64_t*)p : &time_buffer;

    zn_time_get_rtc_time_raw(ptr);

    *ptr = *ptr / 1000;

    return *ptr;
}

//struct tm* get_gmtime(time_t* currentTime)
//{
//	/* TODO: replace the gmtime call below with your own. Note that converting to struct tm needs to be done ...
//    return gmtime(currentTime);*/
//}
//
//time_t get_mktime(struct tm* cal_time)
//{
//	/* TODO: replace the mktime call below with your own. Note that converting to time_t needs to be done ...
//	return mktime(cal_time);*/
//}
//
//char* get_ctime(time_t* timeToGet)
//{
//	/* TODO: replace the ctime call below with calls available on your platform.
//    return ctime(timeToGet);*/
//}

double get_difftime(time_t stopTime, time_t startTime)
{
    time_t diff = (stopTime - startTime);
    return (double)diff;
}
