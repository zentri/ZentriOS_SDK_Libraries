/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "s2c_api.h"


#define INTERNET_CHECK_TIMEOUT 1000


/*************************************************************************************************/
WEAK zos_result_t s2c_cloud_connect(void)
{
    zos_result_t result;

    if(!check_internet_connection())
    {
        ZOS_LOG("No internet connection");
        result = ZOS_NOT_CONNECTED;
    }
    else if(ZOS_FAILED(result, s2c_open_cloud_connection(s2c_app_context.settings->cloud.url, s2c_app_context.settings->cloud.cert)))
    {
        ZOS_LOG("Cloud connection failed: %d", result);

        if(result == ZOS_INVALID_TOKEN || result == ZOS_NULL_PTR)
        {
            ZOS_LOG("Can't connect to cloud until cloud token is configured");
            *s2c_app_context.settings->cloud.token = 0;
        }
    }
    else
    {
        // we've successfully (re)connected to the cloud, start the streams (if they're not already started)
        ZOS_LOG("Cloud connection opened");

        s2c_streams_start();

        ZOS_LOG("Requesting current state of streams from cloud");
        if(s2c_request_stream_data(NULL) != ZOS_SUCCESS)
        {
            ZOS_LOG("Failed to request stream state");
        }
    }

    return result;
}

/*************************************************************************************************/
WEAK void s2c_cloud_disconnect(void *unused)
{
    ZOS_LOG("Closing cloud connection");
    s2c_close_cloud_connection();
}

/*************************************************************************************************/
WEAK void s2c_cloud_disconnected_callback(void *arg)
{
    ZOS_LOG("Disconnected from cloud server");
    s2c_network_state_update();
}




/*************************************************************************************************/
static zos_bool_t check_internet_connection(void)
{
    uint32_t ip_address;
    return (zn_network_lookup_with_timeout(ZOS_WLAN, s2c_app_context.settings->cloud.url, &ip_address, INTERNET_CHECK_TIMEOUT) == ZOS_SUCCESS);
}
