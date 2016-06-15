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
WEAK zos_result_t s2c_streams_init(void)
{
    for(const s2c_stream_t *stream = platform_streams_mapping; stream->name != NULL; ++stream)
    {
        if(stream->listener != NULL)
        {
            ZOS_LOG("Registering: %s", stream->name);
            s2c_register_listener(stream->name, stream->listener, stream->arg);
        }
    }

    zos_result_t result = s2c_platform_streams_init();

    return result;
}

/*************************************************************************************************/
WEAK zos_result_t s2c_platform_streams_init(void)
{
    // This function should be overriden by the zap
    return ZOS_SUCCESS;
}


/*************************************************************************************************/
WEAK void s2c_streams_start(void)
{
    // if the streams loops haven't been started then start them now
    if(!s2c_app_context.streams_started)
    {
        ZOS_LOG("Starting streams ...");

        for(const s2c_stream_t *stream = platform_streams_mapping; stream->name != NULL; ++stream)
        {
            if(stream->set_enabled != NULL)
            {
                stream->set_enabled(stream->arg, ZOS_TRUE);
            }
        }

        s2c_app_context.streams_started = ZOS_TRUE;
    }
}

/*************************************************************************************************/
WEAK void s2c_streams_stop(void)
{
    // if the streams are running then stop them now
    if(s2c_app_context.streams_started)
    {
        ZOS_LOG("Stopping streams ...");
        s2c_app_context.streams_started = ZOS_FALSE;
        for(const s2c_stream_t *stream = platform_streams_mapping; stream->name != NULL; ++stream)
        {
            if(stream->set_enabled != NULL)
            {
                stream->set_enabled(stream->arg, ZOS_FALSE);
            }
        }
    }
}

/*************************************************************************************************
 * This is called when a mobile requests a streams data
 */
WEAK void s2c_streams_read_request_event_handler(void *stream_name)
{
    ZOS_LOG("Stream read request: %s", stream_name);

    for(const s2c_stream_t *stream = platform_streams_mapping; stream->name != NULL; ++stream)
    {
        if(strcmp(stream->name, stream_name) == 0)
        {
            stream->event_handler(stream->arg);
        }
    }
}


/*************************************************************************************************/
WEAK void s2c_streams_write_bool_value(const char *stream, zos_bool_t value)
{
    char buffer[32];
    sprintf(buffer, "\"value\":%u}", (unsigned int)value);
    send_stream_data(stream, buffer);
}

/*************************************************************************************************/
WEAK void s2c_streams_write_fpi_value(const char *stream, const fpi_word_t *value)
{
    char buffer[32];
    char fpi_str[16];
    sprintf(buffer, "\"value\":%s}", fpi_to_str(fpi_str, value));
    send_stream_data(stream, buffer);
}

/*************************************************************************************************/
WEAK void s2c_streams_write_uint32_value(const char *stream, uint32_t value)
{
    char buffer[32];
    sprintf(buffer, "\"value\":%u}", (unsigned int)value);
    send_stream_data(stream, buffer);
}



/*************************************************************************************************/
static void send_stream_data(const char *stream, const char *json_data)
{
    zos_result_t result;
    uint64_t time;
    char buffer[96];
    char *ptr = buffer;
    zn_time_get_rtc_time_raw(&time);

    ptr += sprintf(ptr, "{\"at\":");
    uint64_to_str(&time, ptr);
    ptr += strlen(ptr);
    *ptr++ = ',';
    strcpy(ptr, json_data);

    if(ZOS_FAILED(result, s2c_write_stream_str(stream, buffer)))
    {
    }

    // If this fails because no cloud and/or local client are connected then stop the streams
    if(result == ZOS_NOT_CONNECTED)
    {
        s2c_streams_stop();
    }
}
