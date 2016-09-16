/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#pragma once





#define S2C_ADD_STREAM(_name, _handler, _listener, _set_enabled, _arg)  \
{                                                                       \
    .name = _name,                                                      \
    .event_handler = _handler,                                          \
    .listener = _listener,                                              \
    .set_enabled = _set_enabled,                                        \
    .arg = (void*)_arg                                                  \
}
#define S2C_ADD_NULL_STREAM() { .name = NULL }


#define S2C_HOST_STREAMS_ONLY()                                 \
zos_result_t s2c_streams_init(void){ return ZOS_SUCCESS; }      \
void s2c_streams_start(void){}                                  \
void s2c_streams_stop(void){}



typedef struct
{
    const char* name;
    zos_event_handler_t event_handler;
    s2c_stream_listener_t listener;
    zos_result_t (*set_enabled)(void *arg, zos_bool_t enabled);
    void *arg;
} s2c_stream_t;


zos_result_t s2c_platform_streams_init(void);
zos_result_t s2c_streams_init(void);
void s2c_streams_start(void);
void s2c_streams_stop(void);
void s2c_streams_read_request_event_handler(void *arg);
zos_result_t s2c_streams_write_bool_value(const char *stream, zos_bool_t value);
zos_result_t s2c_streams_write_float_value(const char *stream, float value);
zos_result_t s2c_streams_write_fpi_value(const char *stream, const fpi_word_t *value);
zos_result_t s2c_streams_write_uint32_value(const char *stream, uint32_t value);
zos_result_t s2c_streams_write_int32_value( const char *stream, int32_t value );
zos_result_t s2c_streams_write_str_value(const char *stream, const char *str);
zos_result_t s2c_streams_write_bin_value(const char *stream, void *data, uint16_t length);
zos_result_t s2c_streams_write_context(const char *stream, msgpack_context_t *context);

extern const s2c_stream_t const platform_streams_mapping[];
