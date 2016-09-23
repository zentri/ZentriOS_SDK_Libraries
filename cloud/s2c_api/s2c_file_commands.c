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


#define EXECUTE_IN_ZAP_THREAD(handler) issue_command_in_zap_thread(handler, argc, argv)
#define START_COMMAND() zos_cmd_result_t cmd_result = CMD_SUCCESS; file_command_context_t *context = (file_command_context_t*)arg; int argc = context->argc; (void)argc; char **argv = context->argv
#define EXIT_COMMAND() context->result = cmd_result; zn_rtos_semaphore_set(&context->wait_sem)


/*************************************************************
 * Commands List
 */
ZOS_COMMANDS_START(file)
#ifndef S2C_HOST_BUILD
    ZOS_ADD_COMMAND("s2c_fst",      2, 2, ZOS_FALSE, s2c_file_stat),
    ZOS_ADD_COMMAND("s2c_fde",      2, 2, ZOS_FALSE, s2c_file_delete),
    ZOS_ADD_COMMAND("s2c_fls",      1, 2, ZOS_FALSE, s2c_file_list),
#endif
    ZOS_ADD_COMMAND("s2c_fdo",      2, 3, ZOS_FALSE, s2c_file_download),
    ZOS_ADD_COMMAND("s2c_fup",      2, 3, ZOS_FALSE, s2c_file_upload),
    ZOS_ADD_COMMAND("$",            1, 10, ZOS_FALSE, s2c_raw_mobile_command),
ZOS_COMMANDS_END

ZOS_SETTERS_START(file)
ZOS_SETTERS_END
ZOS_GETTERS_START(file)
ZOS_GETTERS_END

ZOS_COMMAND_LISTS(file);



/*************************************************************************************************/
WEAK void s2c_file_commands_init(void)
{
    ZOS_CMD_REGISTER_COMMANDS(file);
}


/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_raw_mobile_command)
{
    return EXECUTE_IN_ZAP_THREAD(raw_mobile_command);
}

/*************************************************************************************************
 * executes in zap thread
 */
static void raw_mobile_command(void *arg)
{
    START_COMMAND();
    zos_result_t result;
    char response[128] = { 0 };
    char cmd[128] = { 0 };

    while(argc > 0)
    {
        strcat(cmd, argv[0]);

        --argc;
        ++argv;
        if(argc > 0)
        {
            strcat(cmd, " ");
        }
    }

    ZOS_LOG("Issuing raw command to mobile ...");


    if(ZOS_FAILED(result, s2c_command_issue_return_str(0, response, sizeof(response), cmd)))
    {
        if(result > ZOS_CMD_CODES_START)
        {
            ZOS_LOG("Failed to issue command: %d, %s", result, cmd);
        }
        else
        {
            ZOS_LOG("Failed to issue command: %d", result);
        }
        cmd_result = CMD_FAILED;
    }
    else
    {
        zn_cmd_write_response(CMD_SUCCESS, response, strlen(response));
        cmd_result = CMD_SUCCESS;
    }

    EXIT_COMMAND();
}

#ifndef S2C_HOST_BUILD

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_file_stat)
{
    return EXECUTE_IN_ZAP_THREAD(file_stat);
}

/*************************************************************************************************/
static void file_stat(void *arg)
{
    START_COMMAND();
    s2c_file_t file;
    zos_result_t result;
    s2c_file_target_t target = arg_to_target(argv[0]);


    if(ZOS_FAILED(result, s2c_file_stat(target, argv[1], &file)))
    {
        zn_cmd_format_response(CMD_FAILED, "Failed to get file info: %d", result);
    }
    else
    {
        const zos_time_t time =
        {
                .seconds = file.timestamp
        };
        char buffer[32];

        zn_cmd_format_response(CMD_SUCCESS, "File size: %d, CRC: 0x%04X, time: %s",
                                                             file.size, file.crc, zn_time_to_str(&time, buffer));
    }

    EXIT_COMMAND();
}

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_file_delete)
{
    return EXECUTE_IN_ZAP_THREAD(file_delete);
}

/*************************************************************************************************/
static void file_delete(void *arg)
{
    START_COMMAND();
    zos_result_t result;
    s2c_file_target_t target = arg_to_target(argv[0]);

    if(ZOS_FAILED(result, s2c_file_delete(target, argv[1])))
    {
        zn_cmd_format_response(CMD_FAILED, "Failed to delete file: %d", result);
    }
    else
    {
        zn_cmd_format_response(CMD_SUCCESS, "File deleted");
    }

    EXIT_COMMAND();
}

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_file_list)
{
    return EXECUTE_IN_ZAP_THREAD(file_list);
}

/*************************************************************************************************/
static void file_list(void *arg)
{
    START_COMMAND();
    s2c_file_t *files;
    zos_result_t result;
    s2c_file_target_t target = arg_to_target(argv[0]);
    s2c_file_list_parameters_t params =
    {
            .filter = (argc > 1) ? argv[1] : NULL
    };

    if(ZOS_FAILED(result, s2c_file_list(target, &params, &files)))
    {
        zn_cmd_format_response(CMD_FAILED, "Failed to get file list: %d", result);
    }
    else
    {
        for(const s2c_file_t *f = files; f != NULL; f = f->next)
        {
            zn_cmd_format_response(CMD_SUCCESS, "%-30s %6u 0x%04X", f->filename, f->size, f->crc);
        }

        s2c_file_list_destroy(files);
    }

    EXIT_COMMAND();
}

#endif

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_file_download)
{
    return EXECUTE_IN_ZAP_THREAD(file_download);
}

/*************************************************************************************************/
static void file_download(void *arg)
{
    START_COMMAND();
    zos_result_t result;
    s2c_file_target_t target = arg_to_target(argv[0]);
    const char *remote_filename = argv[1];
    const char *local_filename = (argc > 2) ? argv[2] : remote_filename;

    zn_file_delete(local_filename);

    if(ZOS_FAILED(result, s2c_file_download(target, remote_filename, local_filename)))
    {
        zn_cmd_format_response(CMD_FAILED, "Failed to download file: %d", result);
    }
    else
    {
        zn_cmd_format_response(CMD_SUCCESS, "File downloaded");
    }

    EXIT_COMMAND();
}

/*************************************************************************************************/
ZOS_DEFINE_COMMAND(s2c_file_upload)
{
    return EXECUTE_IN_ZAP_THREAD(file_upload);
}

/*************************************************************************************************/
static void file_upload(void *arg)
{
    START_COMMAND();
    zos_result_t result;
    s2c_file_target_t target = arg_to_target(argv[0]);
    const char *local_filename = argv[1];
    const char *remote_filename = (argc > 2) ? argv[2] : NULL;

    if(ZOS_FAILED(result, s2c_file_upload(target, local_filename, remote_filename)))
    {
        zn_cmd_format_response(CMD_FAILED, "Failed to upload file: %d", result);
    }
    else
    {
        zn_cmd_format_response(CMD_SUCCESS, "File uploaded");
    }

    EXIT_COMMAND();
}

/*************************************************************************************************/
static s2c_file_target_t arg_to_target(char *target_arg)
{
    if(strcmp(target_arg, "cloud") == 0)
    {
        return S2C_FILE_TARGET_CLOUD;
    }
    else if(strcmp(target_arg, "dms_device") == 0)
    {
        return S2C_FILE_TARGET_DMS_DEVICE;
    }
    else if(strcmp(target_arg, "dms_product") == 0)
    {
        return S2C_FILE_TARGET_DMS_PRODUCT;
    }
    else if(strncmp(target_arg, "mobile", 6) == 0)
    {
        const uint8_t mobile_num = target_arg[6] - '0';
        return  S2C_FILE_TARGET_MOBILE(mobile_num);
    }
    else
    {
        return S2C_FILE_TARGET_MOBILE(0);
    }
}

/*************************************************************************************************/
static zos_cmd_result_t issue_command_in_zap_thread(zos_event_handler_t handler, int argc, char **argv)
{
    file_command_context_t context = {.result = CMD_FAILED };
    zn_rtos_semaphore_init(&context.wait_sem);
    context.argc = argc;
    context.argv = argv;

    if(zn_network_up(ZOS_WLAN, ZOS_TRUE) != ZOS_SUCCESS)
    {
        return CMD_FAILED;
    }

    zn_event_issue(handler, &context, 0);

    zn_rtos_semaphore_get(&context.wait_sem, 60000);

    zn_rtos_semaphore_deinit(&context.wait_sem);

    return context.result;
}
