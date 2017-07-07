/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "iothub_mqtt_internal.h"



connection_info_t connection_info;



/*************************************************************************************************/
XIO_HANDLE getIoMqttTransportProvider(const char* fully_qualified_name, const MQTT_TRANSPORT_PROXY_OPTIONS* mqtt_transport_proxy_options)
{
    connection_info.hostname = fully_qualified_name;
    connection_info.port = 8883;

    return (void*)1;
}
