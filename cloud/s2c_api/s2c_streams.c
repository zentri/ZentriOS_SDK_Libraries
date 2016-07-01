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
    uint8_t buffer[32];
    MSGPACK_INIT_WITH_BUFFER(context, buffer, sizeof(buffer));
    msgpack_write_bool(&context, value);
    send_stream_value(stream, &context);
}

/*************************************************************************************************/
WEAK void s2c_streams_write_fpi_value(const char *stream, const fpi_word_t *value)
{
    char fpi_str[16];
    uint8_t buffer[32];
    MSGPACK_INIT_WITH_BUFFER(context, buffer, sizeof(buffer));
    msgpacket_write_str(&context, fpi_to_str(fpi_str, value));
    send_stream_value(stream, &context);
}

/*************************************************************************************************/
WEAK void s2c_streams_write_uint32_value(const char *stream, uint32_t value)
{
    uint8_t buffer[32];
    MSGPACK_INIT_WITH_BUFFER(context, buffer, sizeof(buffer));
    msgpack_write_uint(&context, value);
    send_stream_value(stream, &context);
}



/*************************************************************************************************/
static void send_stream_value(const char *stream, msgpack_context_t *context)
{
    zos_result_t result;
    uint64_t time;
    msgpack_context_t msg_context;

    zn_time_get_rtc_time_raw(&time);

    //  A stream message has the following format:
    // { "at" : <timestamp>,
    //   "value" : <message value>
    // }
    s2c_write_stream_context_init(&msg_context, stream);
    msgpack_write_dict_marker(&msg_context, 2);
    msgpack_write_dict_ulong(&msg_context, "at", &time);
    msgpack_write_dict_context(&msg_context, "value", context);

    if(ZOS_FAILED(result, s2c_write_stream_context_flush(&msg_context)))
    {
    }

    // If this fails because no cloud and/or local client are connected then stop the streams
    if(result == ZOS_NOT_CONNECTED)
    {
        s2c_streams_stop();
    }
}
