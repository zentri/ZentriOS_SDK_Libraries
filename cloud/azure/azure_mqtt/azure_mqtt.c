/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */



#include "azure_mqtt.h"
#include "azure_iot_sdk_mqtt_port/iothub_mqtt_internal.h"


typedef struct
{
    IOTHUB_CLIENT_LL_HANDLE_DATA *client_handle;
    struct
    {
        IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK connection;
        IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC messsage;
    } callbacks;
} azure_mqtt_context_t;






static void azure_iot_mqtt_processing_handler(void *unused);




static azure_mqtt_context_t azure_mqtt_context;



/*************************************************************************************************/
zos_result_t azure_mqtt_start(const char *hostname, const char *device_key)
{
    zos_result_t result;
    char hostname_buffer[128];
    char uuid_buffer[DEVICE_UUID_LEN+1];

    strncpy(hostname_buffer, hostname, sizeof(hostname_buffer));

    char *suffix = strchr(hostname_buffer, '.');
    if(suffix == NULL)
    {
        result = ZOS_INVALID_ARG;
        goto exit;
    }
    *suffix++ = 0;

    azure_mqtt_stop();

    const IOTHUB_CLIENT_CONFIG client_config =
    {
            .deviceId = uuid_buffer,
            .deviceKey = device_key,
            .iotHubName = hostname_buffer,
            .iotHubSuffix = suffix
    };

    azure_mqtt_context.client_handle = IoTHubClient_LL_Create(&client_config);
    if(azure_mqtt_context.client_handle != NULL)
    {
        IoTHubClient_LL_SetMessageCallback(azure_mqtt_context.client_handle, azure_mqtt_context.callbacks.messsage, NULL);
        IoTHubClient_LL_SetConnectionStatusCallback(azure_mqtt_context.client_handle, azure_mqtt_context.callbacks.connection, NULL);
        result = ZOS_SUCCESS;

        azure_mqtt_trigger_processing();
    }
    else
    {
        result = ZOS_ERROR;
    }

    exit:
    return result;
}

/*************************************************************************************************/
zos_result_t azure_mqtt_stop(void)
{
    zn_event_unregister(azure_iot_mqtt_processing_handler, NULL);

    if(azure_mqtt_context.client_handle  != NULL)
    {
        IoTHubClient_LL_Destroy(azure_mqtt_context.client_handle);
        azure_mqtt_context.client_handle = NULL;
    }

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
zos_result_t azure_mqtt_send_message_str(const char *json_str)
{
    zos_result_t result;
    IOTHUB_MESSAGE_HANDLE messageHandle;


    if(azure_mqtt_context.client_handle == NULL)
    {
        result = ZOS_UNINITIALIZED;
    }
    else if((messageHandle = IoTHubMessage_CreateFromByteArray((void*)json_str, strlen(json_str))) ==  NULL)
    {
        result = ZOS_NO_MEM;
    }
    else
    {
        (void)IoTHubMessage_SetMessageId(messageHandle, "MSG_ID");
        (void)IoTHubMessage_SetCorrelationId(messageHandle, "CORE_ID");

        if(IoTHubClient_LL_SendEventAsync(azure_mqtt_context.client_handle,
                                          messageHandle,
                                          NULL, NULL) != IOTHUB_CLIENT_OK)
        {
            result = ZOS_ERROR;
        }
        else
        {
            result = ZOS_SUCCESS;
        }

        IoTHubMessage_Destroy(messageHandle);
    }

    return result;
}


/*************************************************************************************************/
void azure_mqtt_set_connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS_CALLBACK callback)
{
    azure_mqtt_context.callbacks.connection = callback;
}

/*************************************************************************************************/
void azure_mqtt_set_receive_message_callback(IOTHUB_CLIENT_MESSAGE_CALLBACK_ASYNC callback)
{
    azure_mqtt_context.callbacks.messsage = callback;
}


/*************************************************************************************************/
void azure_mqtt_trigger_processing(void)
{
    if(zn_event_trigger(azure_iot_mqtt_processing_handler, NULL) != ZOS_SUCCESS)
    {
        zn_event_issue(azure_iot_mqtt_processing_handler, NULL, EVENT_FLAGS1(REQUIRE_WLAN));
    }
}


/** --------------------------------------------------------------------------------------------
 *  Internal functions
 * -------------------------------------------------------------------------------------------- **/


/*************************************************************************************************/
static void azure_iot_mqtt_processing_handler(void *unused)
{
    if(azure_mqtt_context.client_handle != NULL)
    {
        IoTHubClient_LL_DoWork(azure_mqtt_context.client_handle);

        // TODO determine how many ms until the next event instead of looping
        zn_event_register_timed(azure_iot_mqtt_processing_handler, NULL, 25, EVENT_FLAGS1(REQUIRE_WLAN));
    }
}
