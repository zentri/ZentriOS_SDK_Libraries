/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "s2c_api.h"



/*************************************************************************************************/
WEAK zos_result_t s2c_cloud_connect(void)
{
    zos_result_t result;

    if(ZOS_FAILED(result, s2c_open_cloud_connection(s2c_app_context.settings->cloud.url, s2c_app_context.settings->cloud.cert)))
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
    s2c_app_context.internet_state = S2C_INTERNET_STATE_PENDING;
    s2c_close_cloud_connection();
}

/*************************************************************************************************/
WEAK void s2c_cloud_disconnected_callback(void *arg)
{
    ZOS_LOG("Disconnected from cloud server");
    s2c_app_context.internet_state = S2C_INTERNET_STATE_PENDING;
    s2c_network_state_update();
}

/*************************************************************************************************/
WEAK zos_bool_t s2c_cloud_check_internet_connection(void)
{
    const char *url = strstr(s2c_app_context.settings->cloud.url, "://");
    if(url != NULL)
    {
        url += 3;
    }
    else
    {
        url = s2c_app_context.settings->cloud.url;
    }

    strcpy(s2c_app_context.cloud_domain, url);

    char *end =  strchr(s2c_app_context.cloud_domain, ':');
    if(end != NULL)
    {
        *end = 0;
    }

    return (zn_issue_command_with_callback(zos_command_callback, "nlo %s", s2c_app_context.cloud_domain) == ZOS_SUCCESS);
}

/*************************************************************************************************/
static void zos_command_callback(const zos_command_callback_context_t *context)
{
    if(context->result == ZOS_SUCCESS)
    {
        s2c_app_context.internet_state = S2C_INTERNET_STATE_VALID;
    }
    else
    {
        ZOS_LOG("No internet connection");
        s2c_app_context.internet_state = S2C_INTERNET_STATE_FAILED;
    }
    zn_event_issue(s2c_network_state_event_handler, NULL, 0);
}
