/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */

#include "s2c_api.h"


/*************************************************************************************************
 * This is called when a client connects to the local tcp server
 */
WEAK void s2c_mobile_connected_callback(void *arg)
{
    ZOS_LOG("Mobile connected: %u", s2c_get_connection_count());

    // if the wlan interface is active then start the streams (if necessary)
    if(zn_network_is_up(ZOS_WLAN))
    {
        s2c_streams_start();
    }
}

/*************************************************************************************************
 * This is called when a client disconnects from the local tcp server
 */
WEAK void s2c_mobile_disconnected_callback(void *arg)
{
    ZOS_LOG("Mobile disconnected: %u", s2c_get_connection_count());
    if(s2c_get_connection_count() == 0 && !s2c_cloud_connection_is_active())
    {
        s2c_streams_stop();
    }
}


/*************************************************************************************************
 * This is called whenever a remote client attempt to execute a command.
 * This can be used as a security feature to ensure client's only have allowed
 * access to the device.
 */
WEAK zos_bool_t s2c_mobile_command_callback(const char *cmd_str)
{
    char buffer[128];

    strcpy(buffer, cmd_str);

    // convert to lower and remove leading whitesapce
    const char *ptr = str_lstrip(str_tolower(buffer), " \t");
    // This is just for demo purposes
    // Don't allow the client to execute the 'ls' command
    if(strncmp(ptr, "ls", 2) == 0)
    {
        ZOS_LOG("NOT Allowed to execute 'ls' command");
        return ZOS_FALSE;
    }
    else
    {
        return ZOS_TRUE;
    }
}

