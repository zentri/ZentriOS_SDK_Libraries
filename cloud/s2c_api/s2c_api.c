/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "s2c_api.h"



s2c_app_context_t s2c_app_context;



/*************************************************************************************************/
zos_result_t s2c_api_init(const s2c_config_t config)
{
    zos_result_t result;

    // initialize the s2c library
    if(ZOS_FAILED(result, s2c_init(config)))
    {
        ZOS_LOG("Failed to init s2c lib: %d", result);
    }
    // register the stream listeners
    else if(ZOS_FAILED(result, s2c_streams_init()))
    {
        ZOS_LOG("Failed to init streams: %d", result);
    }
    // initialize the network interface
    else if(ZOS_FAILED(result, s2c_network_init()))
    {
        ZOS_LOG("Failed to init network: %d", result);
    }
    else
    {
        s2c_app_context.initialized = ZOS_TRUE;
    }

    return result;
}
