// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/* This is a template file used for porting */


#include "zos.h"
#include "azure_c_shared_utility/tickcounter.h"


TICK_COUNTER_HANDLE tickcounter_create(void)
{
    return (TICK_COUNTER_HANDLE)1;
}

void tickcounter_destroy(TICK_COUNTER_HANDLE tick_counter)
{
}

int tickcounter_get_current_ms(TICK_COUNTER_HANDLE tick_counter, tickcounter_ms_t* current_ms)
{
    *current_ms = zn_rtos_get_time();
    return 0;
}
