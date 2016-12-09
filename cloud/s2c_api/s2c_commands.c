/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */


#include "s2c_api.h"


#define CONFIG_LOADED_REG ZOS_BACKUP_REG_0


/*************************************************************
 * Getters List
 */
ZOS_GETTERS_START(s2c)
    ZOS_ADD_GETTER("s2c.device_title", device_title),
    ZOS_ADD_GETTER("s2c.device_code", device_code),
    ZOS_ADD_GETTER("s2c.cloud_url", cloud_url),
    ZOS_ADD_GETTER("s2c.capabilities_name", capabilities_name),
    ZOS_ADD_GETTER("s2c.flags", flags)
ZOS_GETTERS_END

/*************************************************************
 * Setters List
 */
ZOS_SETTERS_START(s2c)
    ZOS_ADD_SETTER("s2c.device_key", device_key),
    ZOS_ADD_SETTER("s2c.cloud_token", cloud_token),
    ZOS_ADD_SETTER("s2c.cloud_url", cloud_url),
    ZOS_ADD_SETTER("s2c.device_title", device_title),
    ZOS_ADD_SETTER("s2c.device_code", device_code),
    ZOS_ADD_SETTER("s2c.flags_enabled", flags_enabled),
    ZOS_ADD_SETTER("s2c.flags_disabled", flags_disabled),
ZOS_SETTERS_END

/*************************************************************
 * Commands List
 */
ZOS_COMMANDS_START(s2c)
    ZOS_ADD_COMMAND("s2c_restart",      0, 0, ZOS_FALSE, s2c_restart),
    ZOS_ADD_COMMAND("s2c_reset",        0, 0, ZOS_FALSE, s2c_reset),
    ZOS_ADD_COMMAND("s2c_setup",        0, 0, ZOS_FALSE, s2c_setup),
    ZOS_ADD_COMMAND("s2c_connect",      0, 0, ZOS_FALSE, s2c_connect),
    ZOS_ADD_COMMAND("s2c_disconnect",   0, 0, ZOS_FALSE, s2c_disconnect),
    ZOS_ADD_COMMAND("s2c_ota",          0, 1, ZOS_FALSE, s2c_ota),
#ifdef S2C_HOST_BUILD
    ZOS_ADD_COMMAND("s2c_bad",          1, 1, ZOS_FALSE, s2c_broadcast_app_data),
    ZOS_ADD_COMMAND("sleep",            0, 0, ZOS_FALSE, s2c_sleep),
#endif
ZOS_COMMANDS_END

ZOS_COMMAND_LISTS(s2c);



static const char* s2c_app_name;



/*************************************************************************************************/
WEAK zos_result_t s2c_commands_init(uint32_t setting_magic_number, const char *app_name, const void *default_settings, uint16_t default_settings_len)
{
    zos_result_t result = ZOS_SUCCESS;

    // check that app_settings_t hasn't overflowed the NVM
    // if the build fails here then app_settings_t is too large to fit into the NVM
    // you need to make this struct smaller
    BUILD_CHECK_NVM_SIZE(s2c_app_settings_t);

    ZOS_NVM_GET_REF(s2c_app_context.settings);
    ZOS_CMD_REGISTER_COMMANDS(s2c);

    s2c_app_name = app_name;

    const uint32_t loaded_settings_flag = zn_backup_register_read(CONFIG_LOADED_REG);

    // if the nvm settings haven't been initialized, do it now
    if(s2c_app_context.settings->magic_number != setting_magic_number)
    {
        zos_file_t file_info;

        // check if the config file is available
        // this is the device's settings just before it is OTA'd
        if(zn_file_stat(app_name, &file_info) == ZOS_SUCCESS)
        {
            // check if we've already loaded the config file
            if(loaded_settings_flag == setting_magic_number)
            {
                // the config file has already been loaded
                //  update the magic number to what this app expects (this way we don't reload the settings again)
                s2c_app_context.settings->magic_number = setting_magic_number;
                zn_settings_save(NULL);
            }
            else
            {
                ZOS_LOG("Restoring %s", app_name);

                // set a flag in the backup reg indicating that the settings have been loaded
                zn_backup_register_write(CONFIG_LOADED_REG, setting_magic_number);

                // a config file does exist meaning we just came from an OTA
                // load the config file instead of the default settings
                // NOTE: this will reboot the system if successful
                zn_settings_load(app_name);
            }
        }
        // no config file is available, just load the default settings
        else if(ZOS_FAILED(result, zn_load_ro_memory(s2c_app_context.settings, default_settings_len, default_settings, 0)))
        {
            ZOS_LOG("Failed to loaded default settings");
        }
        // save the default settings to NVM
        else if(ZOS_FAILED(result, zn_settings_save(NULL)))
        {
            ZOS_LOG("Failed to save settings");
        }
        else
        {
            ZOS_LOG("Success! Rebooting so settings take effect");
            // reboot so the new settings take effect
            zn_system_reboot();
        }
    }

    // at this point the config file must have been loaded,
    // delete it so it isn't reused (if the file exists)
    zn_file_delete(app_name);


    // now that the config file is loaded and deleted
    // we can clear the 'config loaded' flag
    zn_backup_register_write(CONFIG_LOADED_REG, 0);

    return result;
}

/*************************************************************************************************/
WEAK zos_bool_t s2c_is_admin_command(const char *cmd)
{
    static const char* const admin_commands[] =
    {
            "s2c_reset",
            "s2c_setup",
            "s2c_ota",
            "ota",
            "fac",
            NULL
    };

    for(const char * const *c = admin_commands; *c != NULL; ++c)
    {
        if(strstr(*c, cmd) != NULL)
        {
            return ZOS_TRUE;
        }
    }

    return ZOS_FALSE;
}


/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_restart)
{
    ZOS_LOG("Restarting ...");

    // Note: we wait a moment because this will reboot ZentriOS
    // we want to ensure that the command response is sent to the mobile app before doing so
    zn_event_register_timed(system_reboot_event_handler, NULL, 500, 0);

    return CMD_EXECUTE_AOK;
}


/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_reset)
{
    ZOS_LOG("Resetting S2C device ...");

    s2c_app_context.settings->magic_number = 0;
    zn_network_set_credentials(ZOS_WLAN, NULL, NULL, ZOS_TRUE);
    zn_settings_save(NULL);
    // Note: we wait a moment because this will reboot ZentriOS
    // we want to ensure that the command response is sent to the mobile app before doing so
    zn_event_register_timed(system_reboot_event_handler, NULL, 500, 0);

    return CMD_EXECUTE_AOK;
}

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_setup)
{
    ZOS_LOG("Forcing device setup mode ...");

    s2c_app_context.forced_setup_mode = ZOS_TRUE;
    // Note: we wait a moment because this will reboot ZentriOS
    // we want to ensure that the command response is sent to the mobile app before doing so
    zn_event_register_timed(network_down_event_handler, NULL, 500, 0);

    return CMD_EXECUTE_AOK;
}

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_connect)
{
    ZOS_LOG("Connecting to cloud...");

    s2c_app_context.forced_setup_mode = ZOS_FALSE;
    s2c_app_context.forced_cloud_disconnect = ZOS_FALSE;
    s2c_app_context.cloud_retry_count = 0;
    zn_event_issue(s2c_network_state_event_handler, NULL, 0);

    return CMD_EXECUTE_AOK;
}

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_disconnect)
{
    ZOS_LOG("Disconnecting from cloud...");

    bring_down_cloud_connection();

    return CMD_EXECUTE_AOK;
}

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_ota)
{
    zos_result_t result;
    const char *version_str = NULL;

    ZOS_LOG("Preparing for OTA ...");

    // deleted the old settings (if it exists)
    zn_file_delete(s2c_app_name);

    // save the current settings to the config file
    // the settings will be loaded after the OTA
    zn_settings_save(s2c_app_name);


    if(argc > 0)
    {
        const int len = strlen(argv[0]);
        if(len > 0)
        {
           if(strcmp(argv[0], "-f") == 0)
           {
               version_str = ZOS_DMS_UPDATE_FORCED;
           }
           else
           {
               version_str = argv[0];
           }
        }
    }

    ZOS_LOG("Invoking OTA ...");
    zn_backup_register_write(CONFIG_LOADED_REG, 0);
    result = zn_dms_update_with_callback(version_str, zn_dms_update_callback);

    // the above API doesn't return if successful,
    // if we get here then somthing failed
    zn_cmd_format_response(CMD_FAILED, "Failed to invoke update, err:%d", result);

    // the response is already printed, just return success so nothing else is printed
    return CMD_SUCCESS;
}

/*************************************************************************************************/
static void zn_dms_update_callback(void)
{
    zn_cmd_format_response(CMD_SUCCESS, "Updating starting ...");
}


#ifdef S2C_HOST_BUILD

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_broadcast_app_data)
{
    zos_result_t result = ZOS_SUCCESS;

    ZOS_CMD_PARSE_INT_ARG(uint32_t, data_size, argv[0], 0, 256);

    s2c_set_setting(S2C_SETTING_BROADCAST_CALLBACK, NULL);
    if(s2c_app_context.broadcast_app_data.buffer != NULL)
    {
        free(s2c_app_context.broadcast_app_data.buffer);
        s2c_app_context.broadcast_app_data.length = 0;
        s2c_app_context.broadcast_app_data.buffer = NULL;
        s2c_set_setting(S2C_SETTING_BROADCAST_CALLBACK, NULL);
    }

    if(data_size > 0)
    {
        uint8_t data_buffer[data_size];
        zos_buffer_t buffer =
        {
                .data = data_buffer,
                .size = data_size
        };

        if(ZOS_FAILED(result, zn_cmd_read_write_data(data_size, 10000, &buffer, broadcast_app_data_writer)))
        {
        }
        else if(ZOS_FAILED(result, zn_malloc(&s2c_app_context.broadcast_app_data.buffer, data_size)))
        {
        }
        else
        {
            memcpy(s2c_app_context.broadcast_app_data.buffer, data_buffer, data_size);
            s2c_app_context.broadcast_app_data.length = data_size;
            //zn_dump_buffer(s2c_app_context.broadcast_app_data.buffer, data_size, "AppData", ZOS_DUMP_FLAGS(16, 1, LITTLE, ADD_SPACE, NO_ADDRESSES, PRINT_ASCII));
            s2c_set_setting(S2C_SETTING_BROADCAST_CALLBACK, user_broadcast_callback);
            s2c_trigger_status_broadcast();
        }
    }

    return (result == ZOS_SUCCESS) ? CMD_EXECUTE_AOK : CMD_FAILED;
}

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_sleep)
{
    // add a minor delay before sleeping so the command response is returned
    zn_event_register_timed(sleep_event_handler, NULL, 500, 0);
    return CMD_EXECUTE_AOK;
}

#endif

/*************************************************************************************************/
ZOS_DEFINE_GETTER(device_code)
{
    zn_cmd_write_response(CMD_SUCCESS, s2c_app_context.settings->device_code, strlen(s2c_app_context.settings->device_code));

    return CMD_SUCCESS;
}

/*************************************************************************************************/
ZOS_DEFINE_SETTER(device_code)
{
    zos_cmd_result_t result = zn_cmd_set_str(argc, argv, s2c_app_context.settings->device_code, sizeof(s2c_app_context.settings->device_code)-1);
    if(result == CMD_SET_OK)
    {
        s2c_set_setting(S2C_SETTING_DEVICE_CODE, s2c_app_context.settings->device_code);
    }

    return result;
}

/*************************************************************************************************/
ZOS_DEFINE_GETTER(device_title)
{
    zn_cmd_write_response(CMD_SUCCESS, s2c_app_context.settings->device_title, strlen(s2c_app_context.settings->device_title));

    return CMD_SUCCESS;
}

/*************************************************************************************************/
ZOS_DEFINE_SETTER(device_title)
{
    zos_cmd_result_t result = zn_cmd_set_str(argc, argv, s2c_app_context.settings->device_title, sizeof(s2c_app_context.settings->device_title)-1);
    if(result == CMD_SET_OK)
    {
        s2c_set_setting(S2C_SETTING_DEVICE_TITLE, s2c_app_context.settings->device_title);
    }

    return result;
}

/*************************************************************************************************/
ZOS_DEFINE_GETTER(cloud_url)
{
    zn_cmd_write_response(CMD_SUCCESS, s2c_app_context.settings->cloud.url, strlen(s2c_app_context.settings->cloud.url));

    return CMD_SUCCESS;
}

/*************************************************************************************************/
ZOS_DEFINE_SETTER(cloud_url)
{
    return zn_cmd_set_str(argc, argv, s2c_app_context.settings->cloud.url, sizeof(s2c_app_context.settings->cloud.url)-1);
}

/*************************************************************************************************/
ZOS_DEFINE_GETTER(flags)
{
    zn_cmd_format_response(CMD_SUCCESS, "0x%02X", s2c_app_context.settings->flags);
    return CMD_SUCCESS;
}

/*************************************************************************************************/
ZOS_DEFINE_SETTER(flags_enabled)
{
    const uint32_t flags = str_hex_to_uint32(argv[1]);
    if(flags ==  UINT32_MAX)
    {
        return CMD_BAD_ARGS;
    }
    else
    {
        s2c_app_context.settings->flags |= flags;
        s2c_set_setting(S2C_SETTING_FLAGS, (void*)s2c_app_context.settings->flags);
        return CMD_SET_OK;
    }
}

/*************************************************************************************************/
ZOS_DEFINE_SETTER(flags_disabled)
{
    const uint32_t flags = str_hex_to_uint32(argv[1]);
    if(flags ==  UINT32_MAX)
    {
        return CMD_BAD_ARGS;
    }
    else
    {
        s2c_app_context.settings->flags &= ~flags;
        s2c_set_setting(S2C_SETTING_FLAGS, (void*)s2c_app_context.settings->flags);
        return CMD_SET_OK;
    }
}



/*************************************************************************************************/
ZOS_DEFINE_SETTER(device_key)
{
    if(str_parse_hex_buf(argv[1]) != sizeof(s2c_app_context.settings->device_key.data))
    {
        return CMD_BAD_ARGS;
    }
    else
    {
        memcpy(s2c_app_context.settings->device_key.data, argv[1], sizeof(s2c_app_context.settings->device_key.data));
        s2c_app_context.settings->device_key.valid = ZOS_TRUE;
        s2c_app_context.forced_setup_mode = ZOS_FALSE;

        return CMD_SET_OK;
    }
}

/*************************************************************************************************/
ZOS_DEFINE_SETTER(cloud_token)
{
    zos_cmd_result_t res = zn_cmd_set_str(argc, argv, s2c_app_context.settings->cloud.token, sizeof(s2c_app_context.settings->cloud.token));
    if(res == CMD_SET_OK)
    {
        const char *token = s2c_app_context.settings->cloud.token;
        s2c_set_setting(S2C_SETTING_CLOUD_TOKEN, (*token == 0) ? NULL : token);
        s2c_app_context.forced_cloud_disconnect = ZOS_FALSE;
    }

    return res;
}

/*************************************************************************************************/
ZOS_DEFINE_GETTER(capabilities_name)
{
    zos_file_t file;
    const char *filename = s2c_app_context.settings->caps_filename;
    if(zn_file_stat(filename, &file) != ZOS_SUCCESS)
    {
        ZOS_LOG("Failed to get file info for: %s", filename);
        return CMD_FAILED;
    }
    zn_cmd_format_response(CMD_SUCCESS, "%s,%u", filename, file.size);

    return CMD_SUCCESS;
}



/*************************************************************************************************/
static void system_reboot_event_handler(void *arg)
{
    zn_system_reboot();
}

/*************************************************************************************************/
static void network_down_event_handler(void *arg)
{
    if(zn_network_is_up(ZOS_WLAN))
    {
        zn_network_down(ZOS_WLAN);
    }
    else
    {
        s2c_network_state_event_handler(NULL);
    }
}

/*************************************************************************************************/
static void bring_down_cloud_connection(void)
{
    s2c_app_context.forced_cloud_disconnect = ZOS_TRUE;
    s2c_cloud_disconnect(NULL);
    zn_event_issue(s2c_network_state_event_handler, NULL, 0);

    // wait for the cloud connection to disconnect
    while(s2c_cloud_connection_is_active())
    {
        zn_rtos_delay_milliseconds(10);
    }
}


#ifdef S2C_HOST_BUILD

/*************************************************************************************************/
static zos_result_t broadcast_app_data_writer(void *user, const void *data, int len)
{
    zos_buffer_t *buffer = (zos_buffer_t*)user;

    memcpy(buffer->data, data, len);
    buffer->data += len;

    return ZOS_SUCCESS;
}

/*************************************************************************************************/
static uint8_t* user_broadcast_callback(uint8_t *buffer, uint16_t length)
{
    if(s2c_app_context.broadcast_app_data.buffer != NULL && length >= s2c_app_context.broadcast_app_data.length)
    {
        memcpy(buffer, s2c_app_context.broadcast_app_data.buffer, s2c_app_context.broadcast_app_data.length);
        return buffer + s2c_app_context.broadcast_app_data.length;
    }
    else
    {
        return buffer;
    }
}

/*************************************************************************************************/
static void sleep_event_handler(void *arg)
{
    bring_down_cloud_connection();
    zn_powersave_sleep(NULL);
}

#endif

