/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "s2c_api.h"
#include "s2c_api_internal_types.h"



#define SOFTAP_START_MAX_RETRIES        3
#define SOFTAP_START_PERIOD             3000
#define NETWORK_MAX_CONNECT_RETRIES     4
#define NETWORK_RETRY_PERIOD            10000
#define NETWORK_RECONNECT_PERIOD        60000
#define CLOUD_MAX_CONNECT_RETRIES       4
#define CLOUD_RETRY_PERIOD              30000
#define CLOUD_RECONNECT_PERIOD          45000

#define DEVICE_KEY_VALID()                  s2c_app_context.settings->device_key.valid
#define CLOUD_TOKEN_VALID()                 (*s2c_app_context.settings->cloud.token != 0)
#define FORCED_SETUP_MODE_ASSERTED()        s2c_app_context.forced_setup_mode
#define FORCED_CLOUD_DISCONNECT_ASSERTED()  s2c_app_context.forced_cloud_disconnect
#define NOT_MAX_RETRIES(dst, max)           (s2c_app_context.dst ## _retry_count < max)
#define INCREMENT_RETRY_COUNT(dst)          ++s2c_app_context.dst ## _retry_count
#define RESET_RETRY_COUNT(dst)              s2c_app_context.dst ##_retry_count = 0



/*************************************************************************************************/
WEAK zos_result_t s2c_network_init(void)
{
    s2c_app_context.state = S2C_NETWORK_STATE_SETUP_MODE;
    zn_network_register_event_handler(ZOS_WLAN, network_event_handler);
    zn_event_issue(s2c_network_state_event_handler, NULL, 0);

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
WEAK void s2c_network_state_update(void)
{
    s2c_network_state_event_handler(NULL);
}



/*************************************************************************************************/
WEAK void s2c_network_state_event_handler(void *arg)
{
    const network_current_status_t status =
    {
            .network_is_up          = zn_network_is_up(ZOS_WLAN),
            .cloud_connected        = s2c_cloud_connection_is_active(),
            .cond_network_connect   = condition_do_network_connect(),
            .cond_cloud_connect     = condition_do_cloud_connect()
    };

    ZOS_LOG("State:%s, Status: net_up:%d, cloud_up:%d, net_cond:%d, cloud_cond:%d",
            state_to_str(s2c_app_context.state), status.network_is_up, status.cloud_connected, status.cond_network_connect, status.cond_cloud_connect);

    switch(s2c_app_context.state)
    {
    case S2C_NETWORK_STATE_SETUP_MODE:
        handle_setup_mode_state(&status);
        break;

    case S2C_NETWORK_STATE_JOINING:
        handle_network_connect_state(&status);
        break;

    case S2C_NETWORK_STATE_LOCAL_MODE:
        handle_local_mode_state(&status);
        break;

    case S2C_NETWORK_STATE_CLOUD_CONNECTING:
        handle_cloud_connect_state(&status);
        break;

    case S2C_NETWORK_STATE_CLOUD_MODE:
        handle_cloud_mode_state(&status);
        break;

    default:
        break;
    }
}

/*************************************************************************************************/
static void handle_setup_mode_state(const network_current_status_t *status)
{
    // while in Setup Mode, we want to be sure the SoftAP and S2C server is running
    if(!status->cond_network_connect && !zn_network_is_up(ZOS_SOFTAP))
    {
        zos_result_t result;

        ZOS_LOG("Starting SoftAP and server");

        if(ZOS_FAILED(result, zn_network_up(ZOS_SOFTAP, ZOS_TRUE)))
        {
        }
        else if(ZOS_FAILED(result, s2c_start_server(ZOS_SOFTAP)))
        {
            ZOS_LOG("Failed to start server");
        }

        if(result != ZOS_SUCCESS)
        {
            if(NOT_MAX_RETRIES(softap, SOFTAP_START_MAX_RETRIES))
            {
                zn_event_register_timed(s2c_network_state_event_handler, NULL, SOFTAP_START_PERIOD, 0);
            }
            else
            {
                zn_system_reboot();
            }

            return;
        }
    }

    if(status->cond_network_connect)
    {
        update_state(S2C_NETWORK_STATE_JOINING);
    }
}

/*************************************************************************************************/
static void handle_network_connect_state(const network_current_status_t *status)
{
    s2c_streams_stop();

    s2c_app_context.internet_state = S2C_INTERNET_STATE_PENDING;

    if(status->network_is_up)
    {
        update_state(S2C_NETWORK_STATE_LOCAL_MODE);
    }
    else if(status->cond_network_connect)
    {
        zos_result_t result;

        INCREMENT_RETRY_COUNT(network);
        if(ZOS_FAILED(result, zn_network_up(ZOS_WLAN, ZOS_TRUE)))
        {
            zos_wlan_result_t wlan_result = zn_network_get_wlan_join_result();

            ZOS_LOG("Join failed, result: %d", wlan_result);

            if(!NOT_MAX_RETRIES(network, NETWORK_MAX_CONNECT_RETRIES) &&
               (wlan_result == ZOS_WLAN_RESULT_NOT_FOUND || wlan_result == ZOS_WLAN_RESULT_FAILED))
            {
                zn_event_register_timed(s2c_network_state_event_handler, NULL, NETWORK_RETRY_PERIOD,0);
            }
            else
            {
                s2c_app_context.network_retry_count = NETWORK_MAX_CONNECT_RETRIES;
                zn_event_register_timed(clear_retries_event_handler, (void*)&s2c_app_context.network_retry_count, NETWORK_RECONNECT_PERIOD,0);
                update_state(S2C_NETWORK_STATE_SETUP_MODE);
            }
        }
        else
        {
            update_state(S2C_NETWORK_STATE_LOCAL_MODE);
        }
    }
    else
    {
        update_state(S2C_NETWORK_STATE_SETUP_MODE);
    }
}

/*************************************************************************************************/
static void handle_local_mode_state(const network_current_status_t *status)
{
    if(!status->network_is_up)
    {
        update_state(S2C_NETWORK_STATE_JOINING);
        return;
    }
    else if(status->cond_cloud_connect)
    {
        update_state(S2C_NETWORK_STATE_CLOUD_CONNECTING);
    }
}

/*************************************************************************************************/
static void handle_cloud_connect_state(const network_current_status_t *status)
{
    if(status->cloud_connected)
    {
        update_state(S2C_NETWORK_STATE_CLOUD_MODE);
    }
    else if(status->cond_cloud_connect)
    {
        zos_result_t result;

        if(s2c_app_context.internet_state == S2C_INTERNET_STATE_PENDING)
        {
            if(!s2c_cloud_check_internet_connection())
            {
                goto error_exit;
            }
            return;
        }
        else if(s2c_app_context.internet_state == S2C_INTERNET_STATE_FAILED)
        {
            goto error_exit;
        }

        INCREMENT_RETRY_COUNT(cloud);
        if(ZOS_FAILED(result, s2c_cloud_connect()))
        {
            if(!NOT_MAX_RETRIES(cloud, CLOUD_MAX_CONNECT_RETRIES) && CLOUD_TOKEN_VALID())
            {
                zn_event_register_timed(s2c_network_state_event_handler, NULL, CLOUD_RETRY_PERIOD,0);
            }
            else
            {
                error_exit:
                s2c_app_context.cloud_retry_count = CLOUD_MAX_CONNECT_RETRIES;
                s2c_app_context.internet_state = S2C_INTERNET_STATE_PENDING;
                zn_event_register_timed(clear_retries_event_handler, (void*)&s2c_app_context.cloud_retry_count, CLOUD_RECONNECT_PERIOD,0);
                update_state(S2C_NETWORK_STATE_LOCAL_MODE);
            }
        }
        else
        {
            update_state(S2C_NETWORK_STATE_CLOUD_MODE);
        }
    }
    else
    {
        update_state(S2C_NETWORK_STATE_LOCAL_MODE);
    }
}

/*************************************************************************************************/
static void handle_cloud_mode_state(const network_current_status_t *status)
{
    if(!status->network_is_up)
    {
        update_state(S2C_NETWORK_STATE_JOINING);
    }
    else if(!status->cloud_connected)
    {
        update_state(S2C_NETWORK_STATE_CLOUD_CONNECTING);
    }
}

/*************************************************************************************************/
static void update_state(s2c_network_state_t new_state)
{
    const s2c_network_state_t current_state = s2c_app_context.state;
    ZOS_LOG("Current State: %s, New state: %s", state_to_str(current_state), state_to_str(new_state));

    if(new_state == S2C_NETWORK_STATE_JOINING)
    {
        RESET_RETRY_COUNT(network);
    }
    else if(new_state == S2C_NETWORK_STATE_CLOUD_CONNECTING)
    {
        RESET_RETRY_COUNT(cloud);
    }

    if((current_state == S2C_NETWORK_STATE_JOINING))
    {
        if(new_state == S2C_NETWORK_STATE_SETUP_MODE)
        {
            s2c_start_server(ZOS_SOFTAP);
        }
        else if(new_state == S2C_NETWORK_STATE_LOCAL_MODE)
        {
            zn_network_down(ZOS_SOFTAP);
            s2c_start_server(ZOS_WLAN);
        }
    }


    s2c_app_context.state = new_state;
    zn_event_issue(s2c_network_state_event_handler, NULL, 0);
}

/*************************************************************************************************/
static zos_bool_t condition_do_network_connect(void)
{
    zos_ssid_t ssid;
    uint8_t password[64];
    zos_buffer_t pass_buffer = { .data = password, .size = sizeof(password) };
    zos_security_t security;

    zn_network_get_credentials(ZOS_WLAN, &ssid, &pass_buffer);
    zn_network_get_security(ZOS_WLAN, &security);

    return (DEVICE_KEY_VALID() &&
           (ssid.len > 0) &&
           ((security == ZOS_SECURITY_OPEN) || (pass_buffer.size > 0)) &&
           !FORCED_SETUP_MODE_ASSERTED() &&
           NOT_MAX_RETRIES(network, NETWORK_MAX_CONNECT_RETRIES));
}

/*************************************************************************************************/
static zos_bool_t condition_do_cloud_connect(void)
{
    return (zn_network_is_up(ZOS_WLAN) &&
            CLOUD_TOKEN_VALID() &&
            !FORCED_CLOUD_DISCONNECT_ASSERTED() &&
            NOT_MAX_RETRIES(cloud, CLOUD_MAX_CONNECT_RETRIES));
}

/*************************************************************************************************/
static void network_event_handler(uint32_t is_up)
{
    if(is_up)
    {
        ZOS_LOG("Network up event");
    }
    else
    {
        ZOS_LOG("Network down event");
    }

    s2c_network_state_event_handler(NULL);
}

/*************************************************************************************************/
static void clear_retries_event_handler(void *arg)
{
    uint8_t *retries_counter = (uint8_t*)arg;
    *retries_counter = 0;
    ZOS_LOG("Clearing retries");
    zn_event_issue(s2c_network_state_event_handler, NULL, 0);
}

/*************************************************************************************************/
static const char* state_to_str(s2c_network_state_t state)
{
    switch(state)
    {
    case S2C_NETWORK_STATE_SETUP_MODE:
        return "setup";
    case S2C_NETWORK_STATE_JOINING:
        return "joining network";
    case S2C_NETWORK_STATE_LOCAL_MODE:
        return "local only";
    case S2C_NETWORK_STATE_CLOUD_CONNECTING:
        return "cloud connecting";
    case S2C_NETWORK_STATE_CLOUD_MODE:
        return "local + cloud";
    default:
        return "unknown";
    }
}
