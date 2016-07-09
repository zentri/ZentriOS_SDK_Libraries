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
ZOS_COMMANDS_END

ZOS_COMMAND_LISTS(s2c);



static const char* s2c_app_name;



/*************************************************************************************************/
WEAK zos_result_t s2c_commands_init(uint32_t setting_magic_number, const char *app_name)
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
        else if(ZOS_FAILED(result, zn_load_ro_memory(s2c_app_context.settings, sizeof(s2c_app_settings_t), &s2c_default_app_settings, 0)))
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
    zos_bool_t update_available;
    char msg_buffer[128] = { 0 };

    bring_down_cloud_connection();

    if(argc == 0)
    {
        if(ZOS_FAILED(result, check_for_update(msg_buffer, &update_available)))
        {
            return CMD_FAILED;
        }

        if(update_available)
        {
            ZOS_LOG("New firmware available: %s", msg_buffer);
        }
        else
        {
            ZOS_LOG("Firmware up-to-date");
            s2c_app_context.forced_cloud_disconnect = ZOS_FALSE;
            zn_event_issue(s2c_network_state_event_handler, NULL, 0);
            return CMD_SUCCESS;
        }
    }

    ZOS_LOG("Preparing for OTA ...");

    // deleted the old settings (if it exists)
    zn_file_delete(s2c_app_name);

    // save the current settings to the config file
    // the settings will be loaded after the OTA
    zn_settings_save(s2c_app_name);

    char *version_str = NULL;

    if(argc > 0)
    {
        const int len = strlen(argv[0]);
        if(len > 0)
        {
            version_str = zn_malloc_ptr(len+1);
            if(version_str != NULL)
            {
                strcpy(version_str, argv[0]);
            }
        }
    }

    // Note: we wait a moment because this will reboot ZentriOS
    // we want to ensure that the command response is sent to the mobile app before doing so
    zn_event_register_timed(ota_event_handler, version_str, 500, 0);

    return CMD_EXECUTE_AOK;
}

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
    zn_network_down(ZOS_WLAN);
}

/*************************************************************************************************/
static void ota_event_handler(void *arg)
{
    const char *version_str = (arg != NULL) ? arg : ZOS_DMS_UPDATE_FORCED;

    zn_backup_register_write(CONFIG_LOADED_REG, 0);
    zn_dms_update(version_str);
    if(version_str != NULL)
    {
        zn_free((void*)version_str);
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

/*************************************************************************************************/
static zos_result_t check_for_update(char *msg_buffer, zos_bool_t *update_available)
{
    zos_result_t result;
    zos_buffer_t server_msg = {.data = (uint8_t*)msg_buffer, .size = 128 };

    // need to bring down cloud connection first
    // since only one TLS connection is allowed
    bring_down_cloud_connection();

    ZOS_LOG("Checking if OTA required ...");

    if(ZOS_FAILED(result, zn_dms_check_for_update(update_available, &server_msg)))
    {
        s2c_app_context.forced_cloud_disconnect = ZOS_FALSE;
        zn_event_issue(s2c_network_state_event_handler, NULL, 0);
        ZOS_LOG("Failed to check for update: %d", result);
    }

    return result;
}

